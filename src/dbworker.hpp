#pragma once

#include "database.hpp"
#include <memory>
class DBWorker
{
    private:
    std::unique_ptr<Database> db_;
    public:
    DBWorker(std::unique_ptr<Database> db);
    DBWorker(const DBWorker&) = delete;
    DBWorker operator=(const DBWorker&) = delete;
    void run();
};