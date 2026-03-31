#pragma once
#include <boost/asio/io_context.hpp>
#include <boost/json.hpp>
#include <memory>
#include <optional>
#include <unordered_map>
#include <vector>
#include "database.hpp"
#include "db_protocol.hpp"
#include "dependencies.hpp"
#include "server_protocol.hpp"



class session;

class sessions_manager
{
private:
	struct decoded_packet{
		server_protocol::message_type type;
		boost::json::value jv;
	};

	std::shared_ptr<database> db_;
	net::io_context& ioc_main_;
	std::unordered_map<int64_t, std::weak_ptr<session>> sessions_;
	std::optional<decoded_packet> decode_packet(const std::string& raw);
	void add_session(int64_t user_id, std::weak_ptr<session> session_ptr);
	
	void handle_login(std::weak_ptr<session> session_ptr, const server_protocol::login_req &login_msg);
	void handle_reg(std::weak_ptr<session> session_ptr, const server_protocol::reg_req &reg_msg);
	void handle_msg_forward(int64_t sender_id, 	const server_protocol::msg_forw_req &incoming_msg);
	void deliver(int64_t recepeint_id, std::string data);
	void send_to_recepient(int64_t id, const db_protocol::message& msg);
	void send_deliv_ack(int64_t sender_id, int64_t local_id, const db_protocol::message& msg);
	void send_login_res(std::weak_ptr<session> session_ptr, std::optional<int64_t> user_id_opt, 
		std::vector<server_protocol::chat_info> chats);
	void send_reg_res(std::weak_ptr<session> session_ptr, std::optional<int64_t> user_id_opt);
	void handle_search(int64_t sender_id, const server_protocol::search_req &search_req);
	void handle_logout(int64_t user_id);
	void send_search_res(int64_t sender_id, std::vector<db_protocol::found_user> users);
	void handle_history_req(int64_t sender_id, int64_t chat_id);
	void send_history(int64_t sender_id, int64_t chat_id, std::vector<db_protocol::message> db_messages);
	
public:
	//No copy semantics
	sessions_manager(std::shared_ptr<database> db, net::io_context &ioc_main);
	sessions_manager(const sessions_manager&) = delete;
	sessions_manager& operator=(const sessions_manager&) = delete;
	
	void on_auth_attempt(std::weak_ptr<session> session, const std::string& raw);
	void on_data(int64_t sender_id, const std::string& raw);
	void remove_session(int64_t user_id);
};