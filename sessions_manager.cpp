#include "sessions_manager.h"

sessions_manager& sessions_manager::instance()
{
	static sessions_manager instance;
	return instance;
}

void sessions_manager::add_session(std::string user_name, std::shared_ptr<session> session)
{
	sessions_list[user_name] = session;
}

std::shared_ptr<session> sessions_manager::get_client(const std::string& user_name)
{
	if (sessions_list.count(user_name))
		return sessions_list[user_name];
	return nullptr;
}
