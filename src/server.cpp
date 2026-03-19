// server.cpp : Defines the entry point for the application.
//

#include "server.h"
#include "dbworker.h"
#include "listener.h"
#include "database.h"
#include "sessions_manager.h"
#include <boost/asio/io_context.hpp>
#include <memory>
#include <thread>

using namespace std;

int main(int argc, char* argv[])
{
    // Check command line arguments.
    /*if (argc != 4)
    {
        std::cerr <<
            "Usage: websocket-server-async <address> <port> <threads>\n" <<
            "Example:\n" <<
            "    websocket-server-async 0.0.0.0 8080 1\n";
        return EXIT_FAILURE;
    }*/
    std::string conn_string{"dbname=serverdb user=postgres password=1234 host=localhost port=5432"};
    auto db = std::make_shared<Database>(conn_string);
     


    std::cout << "Server is running" << std::endl;
    auto const address = net::ip::make_address("0.0.0.0");
    auto const port = static_cast<unsigned short>(8080);
    //auto const threads = std::max<int>(1, 4);

    // The io_context is required for all I/O
    net::io_context ioc_main;
    auto manager = std::make_shared<sessions_manager>(db, ioc_main);
    std::make_shared<listener>(ioc_main, tcp::endpoint{address, port}, *manager)->run();
    ioc_main.run();
    /*net::io_context ioc{ threads };

    // Create and launch a listening port
    std::make_shared<listener>(ioc, tcp::endpoint{ address, port })->run();

    // Run the I/O service on the requested number of threads
    std::vector<std::thread> v;
    v.reserve(threads - 1);
    for (auto i = threads - 1; i > 0; --i)
        v.emplace_back(
            [&ioc]
            {
                ioc.run();
            });
    ioc.run();*/

    return EXIT_SUCCESS;
}
