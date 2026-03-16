#pragma once

#include <iostream>
#include <memory>
#include <string>

#include <boost/beast/core.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/asio/dispatch.hpp>
#include <boost/asio/strand.hpp>

namespace beast = boost::beast;
namespace http = beast::http;
namespace websocket = beast::websocket;
namespace net = boost::asio;
using tcp = boost::asio::ip::tcp;

//Report an error
inline void fail(beast::error_code ec, char const* what)
{
	std::cerr << what << ": " << ec.message() << std::endl;
}