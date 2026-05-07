#pragma once
#include "../database/db_protocol.hpp"
#include "responses.hpp"
#include <iostream>

namespace protocol_mappers{
	inline server_protocol::msg_deliv map_msg_db_to_net(const db_protocol::message& db_msg)
	{
		server_protocol::msg_deliv net_msg;
		net_msg.message_id = db_msg.id;
		net_msg.chat_id = db_msg.chat_id;
		net_msg.sender_id = db_msg.sender_id;
		net_msg.payload = db_msg.encrypted_payload;
		net_msg.nonce = db_msg.nonce;
		net_msg.timestamp = db_msg.created_at;
		net_msg.is_read = db_msg.is_read;
		return net_msg;
	}

	inline server_protocol::user_info map_user_db_to_net(const db_protocol::user_info& db_user)
	{
		server_protocol::user_info net_user;
		net_user.user_id = db_user.id;
		net_user.encrypted_vault = db_user.encypted_vault;
		net_user.vault_nonce = db_user.vaule_nonce;
		return net_user;
	}

	inline server_protocol::chat_info map_chat_db_to_net(const db_protocol::user_chat& db_chat)
	{
		server_protocol::chat_info net_chat;
		net_chat.chat_id = db_chat.chat_id;
		std::cout << "chat id: " << net_chat.chat_id << std::endl; 
		net_chat.peer_username = db_chat.peer_username;
		std::cout << "peer username: " << net_chat.peer_username << std::endl; 
		net_chat.public_key = db_chat.public_key;
		std::cout << "public key: " << net_chat.public_key << std::endl; 
		net_chat.last_msg = db_chat.last_msg;
		std::cout << "last msg: " << net_chat.last_msg << std::endl; 
		net_chat.timestamp = db_chat.timestamp;
		std::cout << "timestamp: " << net_chat.timestamp << std::endl; 
		net_chat.nonce = db_chat.nonce;
		std::cout << "nonce: " << net_chat.nonce << std::endl; 
		return net_chat;
	}

	inline server_protocol::deliv_ack map_msg_db_to_ack(const db_protocol::message& db_msg)
	{
		server_protocol::deliv_ack del_ack;
		del_ack.chat_id = db_msg.chat_id;
		del_ack.real_id = db_msg.id;
		del_ack.timestamp = db_msg.created_at;
		del_ack.status = "ok";
		return del_ack;
	}

	inline server_protocol::user_search map_users_db_to_search(db_protocol::found_user& db_user)
	{
		server_protocol::user_search user_search;
		user_search.user_id = db_user.user_id;
		user_search.username = db_user.username;
		user_search.public_key = db_user.public_key;
		return user_search;
	}
}