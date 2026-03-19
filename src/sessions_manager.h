#pragma once
#include <boost/asio/io_context.hpp>
#include <memory>
#include <unordered_map>
#include "database.h"
#include "dependencies.h"
#include "session.h"
#include "message.h"
#include "server_protocol.h"

class sessions_manager
{
private:
	//Map for keeping session, after change key to UUID
	//std::unordered_map<std::string, std::shared_ptr<session>> sessions_list;
	//std::unordered_map<std::string, std::queue<std::string>> undeliverd_messages; 
	//std::unordered_map<std::string, std::queue<message>> undeliverd_messages;
	//sessions_manager() = default;
	//std::map<long long, std::shared_ptr<session>> sessions_;
	std::shared_ptr<Database> db_;
	net::io_context& ioc_main_;
	std::unordered_map<int64_t, std::weak_ptr<session>> sessions_;
public:
	//No copy semantics
	sessions_manager(std::shared_ptr<Database> db, net::io_context &ioc_main);
	sessions_manager(const sessions_manager&) = delete;
	sessions_manager& operator=(const sessions_manager&) = delete;
	//static sessions_manager& instance();
	//void add_session(const std::string& uuid, std::shared_ptr<session> session);
	//std::shared_ptr<session> get_client(const std::string& user_name);
	//void remove_session(const std::string& uuid);
	//void add_message(const std::string& uuid, const std::string& message_str, const message &msg);
	//void add_message(const message& msg);
	//std::queue<std::string>& get_undelieverd(const std::string& uuid); 
	//std::queue<message>& get_undelieverd(const std::string& uuid);
	void on_auth_attempt(std::weak_ptr<session> session, const std::string& raw);
	void on_data(int64_t sender_id, const std::string& raw);
	void handle_msg_forward(int64_t sender_id, 	const server_protocol::incoming_message &incoming_msg);
	void deliver(int64_t recepeint_id, std::string data);
	void send_to_recepient(int64_t id, const db_protocol::message& msg);
	void handle_connection(const server_protocol::connect_message &connection_msg);
};