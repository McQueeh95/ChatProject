#include "database.hpp"
#include "db_protocol.hpp"
#include <boost/asio/executor_work_guard.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/asio/post.hpp>
#include <cstddef>
#include <cstdint>
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
    connection_.prepare("get_salt", "SELECT salt FROM users WHERE username = $1");

    connection_.prepare("login", "SELECT auth_key, id, encrypted_vault, vault_nonce FROM users WHERE username = $1");
    
    connection_.prepare("get_recepeint_id", "SELECT user1_id, user2_id FROM chats WHERE id = $1");

    connection_.prepare("insert_msg", "INSERT INTO messages (chat_id, sender_id, encrypted_payload, nonce) "
        "VALUES ($1, $2, $3, $4) RETURNING id, created_at");

    connection_.prepare("create_user", "INSERT INTO users (username, auth_key, salt, public_key, encrypted_vault, vault_nonce) "
        "VALUES ($1, $2, $3, $4, $5, $6) RETURNING id");

    connection_.prepare("upsert_chat", "INSERT INTO chats (user1_id, user2_id) "
        "VALUES ($1, $2) "
        "ON CONFLICT (LEAST(user1_id, user2_id), GREATEST(user1_id, user2_id)) "
        "DO UPDATE SET created_at = EXCLUDED.created_at "
        "RETURNING id;");
    
    connection_.prepare("get_searched_users", "SELECT id, username, public_key FROM users WHERE username ILIKE $1 || '%' LIMIT 10");

    connection_.prepare("get_user_chats", 
                        "SELECT c.id AS chat_id , u.id AS peer_id, u.username AS peer_name, "
                        "u.public_key AS public_key FROM chats c "
                            "JOIN users u ON u.id = CASE  "
                                    "WHEN "
                                    "c.user1_id = $1 THEN c.user2_id "
                                    "ELSE c.user1_id " 
                                    "END "
                                "WHERE c.user1_id = $1 OR c.user2_id = $1;");
    
    connection_.prepare("get_messages", "SELECT * FROM (SELECT id, chat_id, sender_id, encrypted_payload, nonce, created_at, is_read "
        "FROM messages WHERE chat_id = $1 ORDER BY id DESC LIMIT 50) AS sub ORDER BY id ASC" );
    
    connection_.prepare("get_username", "SELECT username FROM users WHERE id = $1");

    connection_.prepare("get_public_key", "SELECT public_key FROM users WHERE id = $1");
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

std::optional<int64_t> database::create_user(const std::string &username, const std::string &auth_key, 
        const std::string &salt, const std::string & public_key, 
        const std::string &encrypted_vault, const std::string &vault_nonce)
{
    try{
        pqxx::work w(connection_);
        pqxx::result r(
            w.exec(pqxx::prepped("create_user"), 
            pqxx::params{
                username, auth_key, salt, public_key, encrypted_vault, vault_nonce})
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

std::string database::get_salt(const std::string &username)
{
    try {
        pqxx::nontransaction n(connection_);
        pqxx::result r(
            n.exec(pqxx::prepped("get_salt"), pqxx::params{username})
        );
        std::string salt = r[0][0].as<std::string>();
        std::cout << "DB(get salt) salt: " << salt << std::endl;
        return salt;
    }
    catch (const std::exception &e) {
        std::cerr << "DB error(get_salt): " << e.what() << std::endl;
        return {};
    }
}

std::optional<db_protocol::user_info> database::login(const std::string &username, const std::string &auth_key)
{
    std::cout << "database::login" << std::endl;
    try {
        pqxx::nontransaction n(connection_);
        pqxx::result r(
            n.exec(pqxx::prepped("login"), pqxx::params{username})
        );

        if(r.empty()) 
            return std::nullopt;
        
        std::string stored_auth_key = r[0][0].as<std::string>();
        std::cout << "db login(auth_key) << " << auth_key << std::endl;
        std::cout << "db login(auth_key_stored) << " << stored_auth_key << std::endl;
        if(auth_key != stored_auth_key)
            return std::nullopt;

        db_protocol::user_info user_info;
        user_info.id = r[0][1].as<int64_t>();
        user_info.encypted_vault = r[0][2].as<std::string>();
        user_info.vaule_nonce = r[0][3].as<std::string>();
        std::cout << "db login(user_id): " << user_info.id << std::endl; 
        std::cout << "db login(encypted_vault)" << user_info.encypted_vault << std::endl;
        return user_info;
    }
    catch (const std::exception &e) {
        std::cerr << "DB error(login): " << e.what() << std::endl;
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

std::optional<db_protocol::message> database::insert_msg(int64_t chat_id, int64_t sender_id, const std::string& text, const std::string &nonce)
{
    try{
        pqxx::work w(connection_);
        pqxx::result r = w.exec(pqxx::prepped("insert_msg"), pqxx::params{chat_id, sender_id, text, nonce});
        w.commit();
        
        if(r.empty()) return std::nullopt;

        db_protocol::message message;
        message.chat_id = chat_id;
        message.sender_id = sender_id;
        message.encrypted_payload = text;
        message.nonce = nonce;
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
        pqxx::result r = w.exec(pqxx::prepped("upsert_chat"), pqxx::params{user1_id, user2_id});
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

std::vector<db_protocol::found_user> database::get_searched_users(const std::string &query)
{
    std::vector<db_protocol::found_user> users;
    try {
        pqxx::nontransaction n(connection_);
        pqxx::result r (
            n.exec(pqxx::prepped("get_searched_users"), pqxx::params{query})
        );
        
        for(auto const row: r)
        {
            users.push_back({
                row[0].as<int64_t>(),
                row[1].as<std::string>(),
                row[2].as<std::string>()
            });
        }  
    } catch (const std::exception& e) {
        std::cerr << "DB error(get_user_id_by_nickname): " << e.what() << std::endl;
    }
    return users;
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
                row[1].as<int64_t>(),
                row[2].as<std::string>(),
                row[3].as<std::string>()
            });
        }
    }
    catch(const std::exception &e){
        std::cerr << "DB error(get_user_chats): " << e.what() << std::endl;
    }
    return chats;
}

std::vector<db_protocol::message> database::get_messages(int64_t chat_id)
{
    std::vector<db_protocol::message> messages;
    try{
        pqxx::nontransaction n(connection_);
        pqxx::result r(
            n.exec(pqxx::prepped("get_messages"), pqxx::params{chat_id})
        );
        for(auto const row: r)
        {
            messages.push_back({
                row[0].as<int64_t>(),
                row[1].as<int64_t>(),
                row[2].as<int64_t>(),
                row[3].as<std::string>(),
                row[4].as<std::string>(),
                row[5].as<std::string>(),
                row[6].as<bool>()
            });
        }
    }
    catch(const std::exception &e)
    {
        std::cerr << "DB error(get_messages): " << e.what() << std::endl;
    }
    return messages;
}

std::optional<std::string> database::get_username(int64_t user_id)
{
    try
    {
        pqxx::nontransaction n(connection_);
        pqxx::result r(
            n.exec(pqxx::prepped("get_username"), pqxx::params(user_id))
        );
        if(r.empty()) return std::nullopt;

        std::string username = r[0][0].as<std::string>();
        return username;
    }
    catch(const std::exception &e)
    {
        std::cerr << "DB error(get_username): " << e.what() << std::endl;
        return std::nullopt;
    }
}

std::optional<std::string> database::get_public_key(int64_t user_id)
{
    try
    {
        pqxx::nontransaction n(connection_);
        pqxx::result r(
            n.exec(pqxx::prepped("get_public_key"), pqxx::params(user_id))
        );
        if(r.empty()) return std::nullopt;

        std::string public_key = r[0][0].as<std::string>();
        return public_key;
    }
    catch(const std::exception &e)
    {
        std::cerr << "DB error(get_public_key): " << e.what() << std::endl;
        return std::nullopt;
    }
}