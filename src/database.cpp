#include "database.h"
#include "db_protocol.h"
#include <boost/asio/executor_work_guard.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/asio/post.hpp>
#include <exception>
#include <iostream>
#include <optional>
#include <thread>

Database::Database(const std::string& connection_string): 
    connection_(connection_string), work_guard_(boost::asio::make_work_guard(ioc_))
{
    if(connection_.is_open())
    {
        std::cout <<  "Connection" << std::endl;
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

db_protocol::message Database::get_msg()
{
    ;
}

int64_t Database::get_recepeint_id(int64_t chat_id, int64_t sender_id)
{
    try{
        pqxx::nontransaction n(connection_);
    
        pqxx::result r(
        n.exec("SELECT user1_id, user2_id FROM chats WHERE id = $1", {chat_id})
        );
        if(r.empty()) return 0;

        int64_t u1 = (r[0][0]).as<int64_t>();
        int64_t u2 = (r[0][1]).as<int64_t>();
        return (u1 == sender_id) ? u2 : u1;
    }
    catch(std::exception& e)
    {
        std::cerr << "DB error(get_recepeint_id): " << e.what() << std::endl;
        return 0;
    }
}

std::optional<db_protocol::message> Database::insert_msg(int64_t chat_id, int64_t sender_id, const std::string& text)
{
    try{
        pqxx::work w(connection_);
        pqxx::result r = w.exec("INSERT INTO messages (chat_id, sender_id, encrypted_payload) "
            "VALUES ($1, $2, $3) RETURNING id, created_at", pqxx::params{chat_id, sender_id, text});
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