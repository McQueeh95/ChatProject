#include "database.h"
#include <boost/asio/executor_work_guard.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/asio/post.hpp>
#include <iostream>
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

Database::~Database()
{
    ioc_.stop();
    db_thread_.join();
}

void Database::post_task(std::function<void()> task)
{
    boost::asio::post(ioc_, std::move(task));
}