#pragma once
#include "dependencies.hpp"
#include "message.hpp"
#include "server_protocol.hpp"
#include "sessions_manager.hpp"
#include <deque>
#include <memory>
#include <queue>
#include <string>

class sessions_manager;

class session : public std::enable_shared_from_this<session>
{
	//WebSocket connection which creates over tcp
	websocket::stream<beast::tcp_stream> ws_;
	//Buffer for saving incoming message
	beast::flat_buffer read_buffer_;
	std::deque<std::string> write_queue_;
	int64_t session_id = -1;
	std::shared_ptr<sessions_manager> manager_;
	void do_write();
	void on_write(beast::error_code ec, std::size_t bytes_transferred);

public:
	// Passing ownership of the socket
		explicit session(tcp::socket && socket, std::shared_ptr<sessions_manager> manager)
		: ws_(std::move(socket)), manager_(std::move(manager))
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
	void send(std::string message);
	//Handles logic with messages
	void on_read(beast::error_code ec, std::size_t bytes_transferd);

	void send_message(const std::string& message, std::function<void()> on_sent);
	void on_close(beast::error_code ec);
	void handle_connect(const message &msg);
	void handle_disconnect(const message &msg);
	void handle_forward(const message &msg, std::shared_ptr<session> receiver);
	void handle_add_contact(const message &msg, std::shared_ptr<session> receiver);
	void send_request(const std::string& msg);
	void handle_message_before_forwarding(const message &msg, const std::string &string_data);
	void set_session_id(const int64_t session_id);
};

