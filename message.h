#pragma once

#include "dependencies.h"
#include  "boost\json.hpp"
#include <chrono>
#include <ctime>

namespace json = boost::json;

//Class for serialization messages
class message
{
public:
	enum message_type {
		Forward,
		Connect,
		Disconnect
	};
private:
	int type;
	std::string uuid_from;
	std::string uuid_to;
public:
	message() = default;
	//On receiving to json
	message(const json::value& v);
	message_type get_type() const;
	std::string get_uuid_from() const;
	std::string get_uuid_to() const;
	

};