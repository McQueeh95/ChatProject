#pragma once
#include <boost/asio/executor_work_guard.hpp>
#include <pqxx/pqxx>
#include <boost/asio/io_context.hpp>
#include <thread>
#include "dependencies.h"
#include "db_protocol.h"

class Database
{
    private:
    pqxx::connection connection_;
    net::io_context ioc_;
    std::thread db_thread_;
    boost::asio::executor_work_guard<net::io_context::executor_type> work_guard_;
    public:
    Database(const std::string& connection_string);
    ~Database();
    Database (const Database&) = delete;
    Database& operator= (const Database&) = delete;
    void post_task(std::function<void()> task);
    void save_msg();
    db_protocol::message get_msg();

};