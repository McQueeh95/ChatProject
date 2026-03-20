#include "database.h"
#include "db_protocol.h"
#include <boost/asio/executor_work_guard.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/asio/post.hpp>
#include <exception>
#include <initializer_list>
#include <iostream>
#include <optional>
#include <thread>

Database::Database(const std::string& connection_string): 
    connection_(connection_string), work_guard_(boost::asio::make_work_guard(ioc_))
{
    if(connection_.is_open())
    {
        std::cout <<  "Connection" << std::endl;
        connection_.prepare("login", "SELECT id, password_hash FROM users WHERE username = $1");
        connection_.prepare("get_recepeint_id", "SELECT user1_id, user2_id FROM chats WHERE id = $1");
        connection_.prepare("insert_msg", "INSERT INTO messages (chat_id, sender_id, encrypted_payload) "
            "VALUES ($1, $2, $3) RETURNING id, created_at");
    }
    db_thread_ = std::thread([this]{ioc_.run();});
    std::cout << "running in: " << db_thread_.get_id();
}

void Database::start()
{
    if(db_thread_.joinable()) return;

    db_thread_ = std::thread([this]{ioc_.run();});
}

Database::~Database()
{
    ioc_.stop();
    if(db_thread_.joinable())
        db_thread_.join();
}

void Database::post_task(std::function<void()> task)
{
    boost::asio::post(ioc_, std::move(task));
}

std::optional<int64_t> Database::login_user(std::string &username, std::string &password)
{
    try {
        pqxx::nontransaction n(connection_);
        pqxx::result r(
            n.exec("login", pqxx::params{username})
        );
        if(r.empty()) return std::nullopt;
        int64_t user_id = r[0][0].as<int64_t>();
        std::string stored_hash = r[0][1].as<std::string>();
        if(stored_hash != password)
            return std::nullopt;
        return user_id;
    }
    catch (std::exception &e) {
        std::cerr << "DB error(login_user): " << e.what() << std::endl;
        return std::nullopt;
    }
}


db_protocol::message Database::get_msg()
{
    ;
}

std::optional<int64_t> Database::get_recepeint_id(int64_t chat_id, int64_t sender_id)
{
    try{
        pqxx::nontransaction n(connection_);
    
        pqxx::result r(
            n.exec("get_recepeint_id", pqxx::params{chat_id})
        );
        if(r.empty()) return std::nullopt;

        int64_t u1 = (r[0][0]).as<int64_t>();
        int64_t u2 = (r[0][1]).as<int64_t>();
        return (u1 == sender_id) ? u2 : u1;
    }
    catch(std::exception& e)
    {
        std::cerr << "DB error(get_recepeint_id): " << e.what() << std::endl;
        return std::nullopt;
    }
}

std::optional<db_protocol::message> Database::insert_msg(int64_t chat_id, int64_t sender_id, const std::string& text)
{
    try{
        pqxx::work w(connection_);
        pqxx::result r = w.exec("insert_msg", pqxx::params{chat_id, sender_id, text});
        w.commit();
        
        if(!r.empty())
        {
            db_protocol::message message;
            message.chat_id = chat_id;
            message.sender_id = sender_id;
            message.encrypted_payload = text;
            message.id = r[0][0].as<int64_t>();
            message.created_at = r[0][1].as<std::string>();
            return message;
        }
        return std::nullopt;
    }
    catch(std::exception& e)
    {
        std::cerr << "DB error(insert_msg): " << e.what() << std::endl;
        return std::nullopt;
    }
}