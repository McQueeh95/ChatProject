#pragma once

#include "dependencies.h"


class session : public std::enable_shared_from_this<session>
{
	//WebSocket connection which creates over tcp
	websocket::stream<beast::tcp_stream> ws_;
	//Buffer for saving incoming message
	beast::flat_buffer buffer_;
public:
	// Passing ownership of the socket
		explicit session(tcp::socket && socket)
		: ws_(std::move(socket))
	{
	}
	//Method for correct init of handshake cuz it is async, for safety
	void run();
	//Starts the async operation
	void on_run();
	//Check for no errors
	void on_accept(beast::error_code ec);
	//Reading through WebSocket
	void do_read();
	//Handles logic with messages
	void on_read(beast::error_code ec, std::size_t bytes_transferd);
	void on_write(beast::error_code ec, std::size_t bytes_transferred);
};

