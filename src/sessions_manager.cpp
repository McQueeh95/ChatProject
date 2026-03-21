
#include "database.hpp"
#include "message.hpp"
#include "db_protocol.hpp"
#include <boost/asio/io_context.hpp>
#include <boost/asio/post.hpp>
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
#include "server_protocol.hpp"
#include "session.hpp"

namespace {
	server_protocol::outgoing_message map_msg_db_to_net(const db_protocol::message& db_msg)
	{
		server_protocol::outgoing_message net_msg;
		net_msg.message_id = db_msg.id;
		net_msg.chat_id = db_msg.chat_id;
		net_msg.sender_id = db_msg.sender_id;
		net_msg.payload = db_msg.encrypted_payload;
		net_msg.timestamp = db_msg.created_at;
		net_msg.is_read = db_msg.is_read;
		return net_msg;
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


void sessions_manager::on_auth_attempt(std::weak_ptr<session> session_ptr, const std::string& raw)
{
	auto packet = decode_packet(raw);
	if(!packet || (packet->type != server_protocol::message_type::AUTH &&
					packet->type != server_protocol::message_type::REG)) return;
	//login 
	auto type = packet->type;
	auto msg = boost::json::value_to<server_protocol::connect_message>(packet->jv);
	db_->post_task([this, msg = std::move(msg), session_ptr, type](){
		std::optional<int64_t> user_id_opt;
		if(type == server_protocol::message_type::REG)
			user_id_opt = db_->create_user(msg.username, msg.hashed_password);
		else
			user_id_opt = db_->login_user(msg.username, msg.hashed_password);

		boost::asio::post(this->ioc_main_, [this, session_ptr, user_id_opt](){
			auto s = session_ptr.lock();
			if(!s) return;
			
			server_protocol::auth_response res;
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
				res.error_msg = "Invalid credentials or user already exists";
			}
			auto jv = json::value_from(res);
			s->send(json::serialize(jv));
		});
	});
}


void sessions_manager::on_data(int64_t sender_id, const std::string& raw)
{
	auto packet = decode_packet(raw);
	if(!packet.has_value() || packet->type == server_protocol::message_type::AUTH) return;
	std::cout << "on_data packet has value" << std::endl;;
	switch (packet->type) {
		case server_protocol::message_type::FORW:
		{
			auto msg = boost::json::value_to<server_protocol::incoming_message>(packet->jv);
			handle_msg_forward(sender_id, msg);	
			break;
		}
		case server_protocol::message_type::SEARCH:
		{
			auto msg = boost::json::value_to<server_protocol::search_request>(packet->jv);
			break;
		}
		case server_protocol::message_type::ACK:
		{
			//auto msg = boost::json::value_to<server_protocol::connect_message>(jv);
			break;
		}
		case server_protocol::message_type::READ_ACK:
		{
			break;
		}
		case server_protocol::message_type::DISCONN:
		{
			break;
		}
		case server_protocol::message_type::AUTH:
		case server_protocol::message_type::REG:
		case server_protocol::message_type::AUTH_RES:
		{
			break;
		}
	}
}

void sessions_manager::handle_msg_forward(int64_t sender_id, const server_protocol::incoming_message &incoming_msg)
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
			boost::asio::post(this->ioc_main_, [this, recepeint_id = *recepeint_id, msg = *db_message]{
				this->send_to_recepient(recepeint_id, msg);
			});
		}
	);
}

void sessions_manager::send_to_recepient(int64_t recepeint_id, const db_protocol::message& msg)
{
	server_protocol::outgoing_message outgoing_msg = map_msg_db_to_net(msg);
	auto jv = json::value_from(outgoing_msg);
	std::string raw_data = json::serialize(jv);
	deliver(recepeint_id, raw_data);
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

void sessions_manager::handle_connection(const server_protocol::connect_message &connection_msg)
{

}