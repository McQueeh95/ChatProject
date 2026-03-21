#pragma once
#include <boost/asio/executor_work_guard.hpp>
#include <optional>
#include <pqxx/pqxx>
#include <boost/asio/io_context.hpp>
#include <thread>
#include "dependencies.h"
#include "db_protocol.h"
#include <cstdint>

class Database
{
    private:
    pqxx::connection connection_;
    std::thread db_thread_;
    net::io_context ioc_;
    boost::asio::executor_work_guard<net::io_context::executor_type> work_guard_;
    public:
    Database(const std::string& connection_string);
    void start();
    ~Database();
    Database (const Database&) = delete;
    Database& operator= (const Database&) = delete;
    void post_task(std::function<void()> task);
    std::optional<int64_t> login_user(const std::string &username, const std::string &password);
    std::optional<int64_t> get_recepeint_id(int64_t chat_id, int64_t sender_id);
    std::optional<db_protocol::message> insert_msg(int64_t chat_id, int64_t sender_id, const std::string& text);

};