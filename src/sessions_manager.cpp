#include "sessions_manager.h"
//#include "server_message.h"
#include "server_protocol.h"
#include "db_protocol.h"

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
sessions_manager& sessions_manager::instance()
{
	static sessions_manager instance;
	return instance;
}

void sessions_manager::add_session(const std::string& uuid, std::shared_ptr<session> session)
{
	sessions_list[uuid] = session;
}

std::shared_ptr<session> sessions_manager::get_client(const std::string& user_name)
{
	if (sessions_list.count(user_name))
		return sessions_list[user_name];
	return nullptr;
}

void sessions_manager::remove_session(const std::string& uuid)
{
	if (sessions_list.count(uuid))
		sessions_list.erase(uuid);
}

//void sessions_manager::add_message(const std::string& uuid, const std::string& message_str, const message &msg)
//{
//	undeliverd_messages[uuid].push(message_str);
//}

void sessions_manager::add_message(const message& msg)
{
	undeliverd_messages[msg.get_uuid_to()].push(msg);
	std::cout << "Pushed into queue msg: " << msg.get_text() << " " << msg.get_time() << std::endl;
}

//std::queue<std::string>& sessions_manager::get_undelieverd(const std::string& uuid)
//{
//	return undeliverd_messages[uuid];
//}

std::queue<message>& sessions_manager::get_undelieverd(const std::string& uuid)
{
	return undeliverd_messages[uuid];
}

/*void sessions_manager::send_message(const server_protocol::message& m)
{
	auto it = sessions_.find(m.receiver_id);
	if(it != sessions_.end())
	{
		it->second->deleiver(m);
	}
}*/

