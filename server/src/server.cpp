// server.cpp : Defines the entry point for the application.
//
#include "listener.hpp"
#include "database.hpp"
#include "sessions_manager.hpp"
#include <boost/asio/io_context.hpp>
#include <memory>

using namespace std;

int main(int argc, char* argv[])
{
    std::string conn_string{"dbname=serverdb user=postgres password=1234 host=localhost port=5432"};
    auto db = std::make_shared<database>(conn_string);
     
    std::cout << "Server is running" << std::endl;
    auto const address = net::ip::make_address("0.0.0.0");
    auto const port = static_cast<unsigned short>(8080);

    net::io_context ioc_main;
    auto manager = std::make_shared<sessions_manager>(db, ioc_main);
    std::make_shared<listener>(ioc_main, tcp::endpoint{address, port}, manager)->run();
    ioc_main.run();

    return EXIT_SUCCESS;
}
