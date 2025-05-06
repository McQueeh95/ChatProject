#pragma once
#include <unordered_map>
#include "dependencies.h"
#include "session.h"

class sessions_manager
{
private:
	//Map for keeping session, after change key to UUID
	std::unordered_map<std::string, std::shared_ptr<session>> sessions_list;
	sessions_manager() = default;
public:
	//No copy semantics
	sessions_manager(const sessions_manager&) = delete;
	sessions_manager& operator=(const sessions_manager&) = delete;
	static sessions_manager& instance();
	void add_session(std::string user_name, std::shared_ptr<session> session);
	std::shared_ptr<session> get_client(const std::string& user_name);
	void remove_session(const std::string& uuid);
};