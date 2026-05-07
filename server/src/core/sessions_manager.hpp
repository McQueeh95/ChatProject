#pragma once
#include <boost/asio/io_context.hpp>
#include <memory>
#include <optional>
#include <unordered_map>
#include <vector>
#include "../database/database.hpp"
#include "../database/db_protocol.hpp"
#include "../dependencies.hpp"
#include "../protocol/requests.hpp"



class session;

class sessions_manager
{
public:
	sessions_manager(std::shared_ptr<database> db, net::io_context &ioc_main);
	
	//No copy semantics
	sessions_manager(const sessions_manager&) = delete;
	sessions_manager& operator=(const sessions_manager&) = delete;
	
	//used by session
	void on_auth_attempt(std::weak_ptr<session> session, const std::string& raw);
	void on_data(int64_t sender_id, const std::string& raw);
	void remove_session(int64_t user_id);

private:

	struct decoded_packet{
		server_protocol::message_type type;
		boost::json::value jv;
	};

	std::optional<decoded_packet> decode_packet(const std::string& raw);
	void add_session(int64_t user_id, std::weak_ptr<session> session_ptr);
	void deliver(int64_t recepeint_id, const std::string &data);
	
	/**********
	Handlers
	***********/
	//Connection
	void handle_login(std::weak_ptr<session> session_ptr, const server_protocol::login_req &login_msg);
	void handle_reg(std::weak_ptr<session> session_ptr, const server_protocol::reg_req &reg_msg);
	void handle_salt_req(std::weak_ptr<session> session_ptr, const server_protocol::salt_req &salt_req);
	void handle_reconnect_req(std::weak_ptr<session> session_ptr, const server_protocol::recon_req &recon_req);

	//Chats/messages
	void handle_msg_forward(int64_t sender_id, 	const server_protocol::msg_forw_req &incoming_msg);
	void handle_create_and_forward(int64_t sender_id, const server_protocol::start_chat_req &incoming_msg);

	//Data
	void handle_search(int64_t sender_id, const server_protocol::search_req &search_req);
	void handle_history_req(int64_t sender_id, int64_t chat_id);

	/**********
	Responses
	***********/
	
	//Connection
	void send_login_res(std::weak_ptr<session> session_ptr, const server_protocol::user_info &user_info, 
		const std::vector<server_protocol::chat_info> &chats);
	void send_reg_res(std::weak_ptr<session> session_ptr, std::optional<int64_t> user_id_opt);
	void send_salt_res(std::weak_ptr<session> session_ptr, const std::string& user_salt);
	void send_recon_res(std::weak_ptr<session> session_ptr, std::optional<int64_t> user_id);
	
	//Chats/messages
	void send_to_recepient(int64_t id, const db_protocol::message& msg);
	void send_deliv_ack(int64_t sender_id, int64_t local_id, const db_protocol::message &msg, int64_t peer_id = -1);
	void send_new_chat_event(int64_t receiver_id, int64_t chat_id, int64_t sender_id, const std::string &sender_username, const std::string &public_key);

	//Data
	void send_search_res(int64_t sender_id, const std::vector<db_protocol::found_user> &users);
	void send_history(int64_t sender_id, int64_t chat_id, const std::vector<db_protocol::message> &db_messages);


	std::shared_ptr<database> db_;
	net::io_context& ioc_main_;
	std::unordered_map<int64_t, std::weak_ptr<session>> sessions_;
};