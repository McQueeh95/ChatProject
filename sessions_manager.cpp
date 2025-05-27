#include "sessions_manager.h"

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

void sessions_manager::add_message(const std::string& uuid, const std::string& message)
{
	undeliverd_messages[uuid].push(message);
}

std::queue<std::string>& sessions_manager::get_undelieverd(const std::string& uuid)
{
	return undeliverd_messages[uuid];
}


