#include "database.hpp"
#include "db_protocol.hpp"
#include <boost/asio/executor_work_guard.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/asio/post.hpp>
#include <cstddef>
#include <exception>
#include <initializer_list>
#include <iostream>
#include <optional>
#include <pqxx/prepared_statement.hxx>
#include <stdexcept>
#include <thread>
#include <vector>

database::database(const std::string& connection_string): 
    connection_(connection_string), work_guard_(boost::asio::make_work_guard(ioc_))
{
    if(!connection_.is_open())
        throw std::runtime_error("Failed to connect to DB");

    prepare_statements();
    db_thread_ = std::thread([this]{ioc_.run();}); 
}

void database::prepare_statements()
{
    connection_.prepare("login", "SELECT id, password_hash FROM users WHERE username = $1");
    
    connection_.prepare("get_recepeint_id", "SELECT user1_id, user2_id FROM chats WHERE id = $1");

    connection_.prepare("insert_msg", "INSERT INTO messages (chat_id, sender_id, encrypted_payload) "
        "VALUES ($1, $2, $3) RETURNING id, created_at");

    connection_.prepare("create_user", "INSERT INTO users (username, password_hash) "
        "VALUES ($1, $2) RETURNING id");

    connection_.prepare("upsert_chat", "INSERT INTO chats (user1_id, user2_id) "
        "VALUES ($1, $2) "
        "ON CONFLICT (LEAST(user1_id, user2_id), GREATEST(user1_id, user2_id)) "
        "DO UPDATE SET created_at = EXCLUDED.created_at "
        "RETURNING id;");
    
    connection_.prepare("get_peer_id", "SELECT id FROM users WHERE username = $1");

    connection_.prepare("get_user_chats", 
                        "SELECT c.id AS chat_id, u.username AS peer_name FROM chats c "
                            "JOIN users u ON u.id = CASE  "
                                    "WHEN "
                                    "c.user1_id = $1 THEN c.user2_id "
                                    "ELSE c.user1_id " 
                                    "END "
                                "WHERE c.user1_id = $1 OR c.user2_id = $1;");
}

database::~database()
{
    ioc_.stop();
    if(db_thread_.joinable())
        db_thread_.join();
}

void database::post_task(std::function<void()> task)
{
    boost::asio::post(ioc_, std::move(task));
}

std::optional<int64_t> database::create_user(const std::string &username, const std::string &password)
{
    try{
        pqxx::work w(connection_);
        pqxx::result r(
            w.exec(pqxx::prepped("create_user"), pqxx::params{username, password})
        );
        w.commit();

        if(r.empty())
            return std::nullopt;
        int64_t user_id = r[0][0].as<int64_t>();
        return user_id;
    }
    catch(const std::exception& e){
        std::cerr << "DB error (create_user): " << e.what();
        return std::nullopt;
    }
}

std::optional<int64_t> database::login_user(const std::string &username, const std::string &password)
{
    std::cout << "database::login_user" << std::endl;
    try {
        pqxx::nontransaction n(connection_);
        pqxx::result r(
            n.exec(pqxx::prepped("login"), pqxx::params{username})
        );

        if(r.empty()) 
            return std::nullopt;
        int64_t user_id = r[0][0].as<int64_t>();
        std::string stored_hash = r[0][1].as<std::string>();
        if(stored_hash != password)
            return std::nullopt;
        return user_id;
    }
    catch (const std::exception &e) {
        std::cerr << "DB error(login_user): " << e.what() << std::endl;
        return std::nullopt;
    }
}

std::optional<int64_t> database::get_recepeint_id(int64_t chat_id, int64_t sender_id)
{
    try{
        pqxx::nontransaction n(connection_);
    
        pqxx::result r(
            n.exec(pqxx::prepped("get_recepeint_id"), pqxx::params{chat_id})
        );
        if(r.empty()) return std::nullopt;

        int64_t u1 = (r[0][0]).as<int64_t>();
        int64_t u2 = (r[0][1]).as<int64_t>();
        return (u1 == sender_id) ? u2 : u1;
    }
    catch(const std::exception& e)
    {
        std::cerr << "DB error(get_recepeint_id): " << e.what() << std::endl;
        return std::nullopt;
    }
}

std::optional<db_protocol::message> database::insert_msg(int64_t chat_id, int64_t sender_id, const std::string& text)
{
    try{
        pqxx::work w(connection_);
        pqxx::result r = w.exec(pqxx::prepped("insert_msg"), pqxx::params{chat_id, sender_id, text});
        w.commit();
        
        if(r.empty()) return std::nullopt;

        db_protocol::message message;
        message.chat_id = chat_id;
        message.sender_id = sender_id;
        message.encrypted_payload = text;
        message.id = r[0][0].as<int64_t>();
        message.created_at = r[0][1].as<std::string>();
        return message;
       
    }
    catch(const std::exception& e)
    {
        std::cerr << "DB error(insert_msg): " << e.what() << std::endl;
        return std::nullopt;
    }
}

std::optional<int64_t> database::upsert_chat(int64_t user1_id, int64_t user2_id)
{
   try 
   {
        pqxx::work w(connection_);
        pqxx::result r = w.exec(pqxx::prepped("upsert_msg"), pqxx::params{user1_id, user2_id});
        w.commit();

        if(r.empty()) return std::nullopt;

        int64_t chat_id = r[0][0].as<int64_t>();
        return chat_id;
    
   } catch (const std::exception& e) 
   {
        std::cerr << "DB error(upsert_chat): " << e.what() << std::endl;
        return std::nullopt;
   } 
}

std::optional<int64_t> database::get_user_id_by_nickname(const std::string &username)
{
    try {
        pqxx::nontransaction n(connection_);
        pqxx::result r (
            n.exec(pqxx::prepped("get_peer_id"), pqxx::params{username})
        );
        if(r.empty()) return std::nullopt;
        
        int64_t chat_id = r[0][0].as<int64_t>();
        return chat_id;
    } catch (const std::exception& e) {
        std::cerr << "DB error(get_user_id_by_nickname): " << e.what() << std::endl;
        return std::nullopt;
    }
}

std::vector<db_protocol::user_chat> database::get_user_chats(int64_t user_id)
{
    std::vector<db_protocol::user_chat> chats;
    try{
        pqxx::nontransaction n(connection_);
        pqxx::result r (
            n.exec(pqxx::prepped("get_user_chats"), pqxx::params{user_id})
        );
        for(auto const row: r)
        {
            chats.push_back({
                row[0].as<int64_t>(),
                row[1].as<std::string>()
            });
        }
    }
    catch(const std::exception &e){
        std::cerr << "DB error(get_user_chats): " << e.what() << std::endl;
    }
    return chats;
}