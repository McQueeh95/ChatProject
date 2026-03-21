#include "dbworker.hpp"
#include <iostream>
#include <thread>
DBWorker::DBWorker(std::unique_ptr<Database> db)
    :db_(std::move(db))
{}

void DBWorker::run(){
    std::cout << "db worker running in: " << std::this_thread::get_id() << std::endl; 
}