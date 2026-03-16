#pragma once

#include "dependencies.h"

// Accepts incoming connections and launches the sessions
class listener : public std::enable_shared_from_this<listener>
{
	net::io_context& ioc_;
	tcp::acceptor acceptor_;
public:
	listener(net::io_context& ioc,
		tcp::endpoint endpoint);
	//Start accepting incoming connections
	void run();

private:
	void do_accept();
	void on_accept(beast::error_code ec, tcp::socket socket);
};