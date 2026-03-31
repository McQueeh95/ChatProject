
#include "sessions_manager.hpp"
#include "database.hpp"
#include "message.hpp"
#include "db_protocol.hpp"
#include <boost/asio/io_context.hpp>
#include <boost/asio/post.hpp>
#include <boost/json.hpp>
#include <boost/json/conversion.hpp>
#include <boost/json/fwd.hpp>
#include <boost/json/parse.hpp>
#include <boost/json/serialize.hpp>
#include <boost/json/value_from.hpp>
#include <exception>
#include <iostream>
#include <memory>
#include <optional>
#include <string>
#include <utility>
#include <vector>
#include "server_protocol.hpp"
#include "session.hpp"

namespace {
	server_protocol::msg_deliv map_msg_db_to_net(const db_protocol::message& db_msg)
	{
		server_protocol::msg_deliv net_msg;
		net_msg.message_id = db_msg.id;
		net_msg.chat_id = db_msg.chat_id;
		net_msg.sender_id = db_msg.sender_id;
		net_msg.payload = db_msg.encrypted_payload;
		net_msg.timestamp = db_msg.created_at;
		net_msg.is_read = db_msg.is_read;
		return net_msg;
	}

	server_protocol::chat_info map_chat_db_to_net(const db_protocol::user_chat& db_chat)
	{
		server_protocol::chat_info net_chat;
		net_chat.chat_id = db_chat.chat_id;
		net_chat.peer_username = db_chat.peer_username;
		return net_chat;
	}

	server_protocol::deliv_ack map_msg_db_to_ack(const db_protocol::message& db_msg)
	{
		server_protocol::deliv_ack del_ack;
		del_ack.chat_id = db_msg.chat_id;
		del_ack.real_id = db_msg.id;
		del_ack.timestamp = db_msg.created_at;
		del_ack.status = "ok";
		return del_ack;
	}

	server_protocol::user_search map_users_db_to_search(db_protocol::found_user& db_user)
	{
		server_protocol::user_search user_search;
		user_search.user_id = db_user.user_id;
		user_search.username = db_user.username;
		return user_search;
	}
}



sessions_manager::sessions_manager(std::shared_ptr<database> db, net::io_context &ioc_main)
	:db_(std::move(db)), ioc_main_(ioc_main)
	{};


std::optional<sessions_manager::decoded_packet> sessions_manager::decode_packet(const std::string& raw)
{
	std::cout << "decode_packet" << std::endl;
	try
	{
		auto jv = json::parse(raw);
		auto type_val = jv.at("type").as_int64();
		std::cout << "Type:" << type_val << std::endl;
		auto type = static_cast<server_protocol::message_type>(type_val);
		return sessions_manager::decoded_packet{type, std::move(jv)};
	}
	catch(const std::exception& e)
	{
		std::cerr << "Error converting types: " << e.what() << std::endl;
		return std::nullopt;
	}
}

void sessions_manager::add_session(int64_t user_id, std::weak_ptr<session> session)
{
	std::cout << user_id << " was added!" << std::endl;
	sessions_[user_id] = session;
}

void sessions_manager::remove_session(int64_t user_id)
{
	auto it = sessions_.find(user_id);
	if(it != sessions_.end())
	{
		sessions_.erase(it);
		std::cout << "session: " << user_id << " removed" << std::endl;
	}
}


void sessions_manager::on_auth_attempt(std::weak_ptr<session> session_ptr, const std::string& raw)
{
	auto packet = decode_packet(raw);
	if(!packet || (packet->type != server_protocol::message_type::LOGIN &&
					packet->type != server_protocol::message_type::REG)) return;
	//login 
	std::cout << "Started login" << std::endl;
	auto type = packet->type;
	if(type == server_protocol::message_type::LOGIN)
	{
		auto msg = boost::json::value_to<server_protocol::login_req>(packet->jv);
		handle_login(session_ptr, msg);

	}
	else if(type == server_protocol::message_type::REG) {
		auto msg = boost::json::value_to<server_protocol::reg_req>(packet->jv);
		handle_reg(session_ptr, msg);
	}
}

void sessions_manager::handle_login(std::weak_ptr<session> session_ptr, const server_protocol::login_req &login_msg)
{
	db_->post_task([this, msg = std::move(login_msg), session_ptr](){
		std::optional<int64_t> user_id_opt = db_->login_user(msg.username, msg.hashed_password);
		std::vector<server_protocol::chat_info> chats;
		if(user_id_opt)
		{
			auto db_chats = db_->get_user_chats(*user_id_opt);
			chats.reserve(db_chats.size());
			for(const auto& c: db_chats)
			{
				chats.push_back({c.chat_id, c.peer_username});
			}
		}
		boost::asio::post(this->ioc_main_, [this, session_ptr, user_id_opt, chats = std::move(chats)](){
			this->send_login_res(session_ptr, user_id_opt, chats);
		});
	});
}

void sessions_manager::handle_reg(std::weak_ptr<session> session_ptr, const server_protocol::reg_req &reg_msg)
{
	db_->post_task([this, msg = std::move(reg_msg), session_ptr](){
		std::optional<int64_t> user_id_opt = db_->create_user(msg.username, msg.hashed_password);

		boost::asio::post(this->ioc_main_, [this, session_ptr, user_id_opt](){
			this->send_reg_res(session_ptr, user_id_opt);
		});
	});
}


void sessions_manager::on_data(int64_t sender_id, const std::string& raw)
{
	auto packet = decode_packet(raw);
	if(!packet.has_value() || packet->type == server_protocol::message_type::LOGIN
					|| packet->type == server_protocol::message_type::REG || sender_id == -1) return;
	std::cout << "on_data packet has value" << std::endl;;
	switch (packet->type) {
		case server_protocol::message_type::FORW:
		{
			std::cout << "before FORW msg converted!";
			auto msg = boost::json::value_to<server_protocol::msg_forw_req>(packet->jv);
			std::cout << "FORW msg converted!";
			handle_msg_forward(sender_id, msg);	
			break;
		}
		case server_protocol::message_type::SEARCH_REQ:
		{
			auto msg = boost::json::value_to<server_protocol::search_req>(packet->jv);
			handle_search(sender_id, msg);
			break;
		}
		case server_protocol::message_type::DELIV_ACK:
		{
			break;
		}
		case server_protocol::message_type::READ_ACK:
		{
			break;
		}
		case server_protocol::message_type::LOGOUT:
		{
			remove_session(sender_id);
			break;
		}
		case server_protocol::message_type::HISTORY_REQ:
		{
			auto msg = boost::json::value_to<server_protocol::history_req>(packet->jv);
			
			handle_history_req(sender_id, msg.chat_id);
		}
		case server_protocol::message_type::LOGIN:
		case server_protocol::message_type::REG:
		case server_protocol::message_type::LOGIN_RES:
		case server_protocol::message_type::REG_RES:
		case server_protocol::message_type::SEARCH_RES:
		{
			break;
		}
	}
}

void sessions_manager::handle_msg_forward(int64_t sender_id, const server_protocol::msg_forw_req &incoming_msg)
{
	std::cout << "handle_msg_forward" << std::endl;
	db_->post_task([this, sender_id, msg = std::move(incoming_msg)](){
			auto recepeint_id = db_->get_recepeint_id(msg.chat_id, sender_id);
			if(!recepeint_id.has_value()) return;
			auto db_message = db_->insert_msg(msg.chat_id, sender_id, msg.payload);
			if(!db_message.has_value())
			{
				std::cerr << "Failed to save message to DB" << std::endl;
				return;
			}
			int64_t local_id = msg.local_id;
			boost::asio::post(this->ioc_main_, [this, local_id, recepeint_id = *recepeint_id, msg = *db_message, sender_id]{
				this->send_deliv_ack(sender_id, local_id, msg);
				this->send_to_recepient(recepeint_id, msg);
			});
		}
	);
}

void sessions_manager::handle_history_req(int64_t sender_id, int64_t chat_id)
{
	db_->post_task([this, chat_id, sender_id](){
		std::vector<db_protocol::message> messages = db_->get_messages(chat_id);
		boost::asio::post(this->ioc_main_, [this, messages, sender_id, chat_id]{
			this->send_history(sender_id, chat_id, messages);
		});
	});
}

void sessions_manager::send_history(int64_t sender_id, int64_t chat_id, std::vector<db_protocol::message> db_messages)
{
	std::vector<server_protocol::msg_deliv> net_messages;
	net_messages.reserve(db_messages.size());
	for(const auto m: db_messages)
	{
		net_messages.push_back(map_msg_db_to_net(m));
	}

	server_protocol::history_res his_res;
	his_res.chat_id = chat_id;
	if(net_messages.empty())
	{
		his_res.status = "error";
		his_res.error_message = "Chat is empty";
	}
	else 
	{
		his_res.status = "ok";
		his_res.messages = std::move(net_messages);
	}
	auto jv = json::value_from(his_res);
	deliver(sender_id, json::serialize(jv));
}

void sessions_manager::send_login_res(std::weak_ptr<session> session_ptr, std::optional<int64_t> user_id_opt, 
		std::vector<server_protocol::chat_info> chats)
{
	auto s = session_ptr.lock();
	if(!s) return;
	
	server_protocol::login_res res;
	if(user_id_opt)
	{
		res.status = "ok";
		res.user_id = *user_id_opt;
		res.chats = chats;
		this->add_session(*user_id_opt, session_ptr);
		s->set_session_id(*user_id_opt);
	}
	else 
	{
		res.status = "error";
		res.error_msg = "Invalid credentials";
	}
	auto jv = json::value_from(res);
	s->send(json::serialize(jv));
}

void sessions_manager::send_reg_res(std::weak_ptr<session> session_ptr, std::optional<int64_t> user_id_opt)
{
	auto s = session_ptr.lock();
	if(!s) return;
	
	server_protocol::reg_res res;
	if(user_id_opt)
	{
		res.status = "ok";
		res.user_id = *user_id_opt;
		this->add_session(*user_id_opt, session_ptr);
		s->set_session_id(*user_id_opt);
	}
	else 
	{
		res.status = "error";
		res.error_msg = "User already exists";
	}
	auto jv = json::value_from(res);
	s->send(json::serialize(jv));
}


void sessions_manager::send_to_recepient(int64_t recepeint_id, const db_protocol::message& msg)
{
	std::cout << "send_to_recepient start" << std::endl;
	server_protocol::msg_deliv outgoing_msg = map_msg_db_to_net(msg);
	auto jv = json::value_from(outgoing_msg);
	std::string raw_data = json::serialize(jv);
	deliver(recepeint_id, raw_data);
}

void sessions_manager::send_deliv_ack(int64_t sender_id, int64_t local_id, const db_protocol::message& msg)
{
	server_protocol::deliv_ack deliv_ack = map_msg_db_to_ack(msg);
	deliv_ack.local_id = local_id;
	auto jv = json::value_from(deliv_ack);
	std::string raw_data = json::serialize(jv);
	deliver(sender_id, raw_data);
}

void sessions_manager::deliver(int64_t recepeint_id, std::string data)
{
	std::cout << "deliver" << std::endl;;
	if(auto recepeint = sessions_.find(recepeint_id); recepeint != sessions_.end())
	{
		if(auto recepeint_session = recepeint->second.lock())
			recepeint_session->send(data);
		else 
			sessions_.erase(recepeint);
	}
}

void sessions_manager::handle_search(int64_t sender_id, const server_protocol::search_req &search_req)
{
	std::string query = search_req.to_find;
	db_->post_task([this, sender_id, search_req, query](){
		auto found_users = db_->get_searched_users(query);

		boost::asio::post(ioc_main_, [this, sender_id, found_users](){
			send_search_res(sender_id, found_users);
		});
	});
}

void sessions_manager::send_search_res(int64_t sender_id, std::vector<db_protocol::found_user> db_users)
{
	std::vector<server_protocol::user_search> net_users;
	net_users.reserve(db_users.size());
	for(auto u: db_users)
	{
		net_users.push_back(map_users_db_to_search(u));
	}
	server_protocol::search_res search_res;
	search_res.found_users = net_users;
	auto jv = json::value_from(search_res);
	std::string raw_data = json::serialize(jv);

	deliver(sender_id, raw_data);
}
