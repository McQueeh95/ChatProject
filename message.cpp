#include "message.h"

message::message(const json::value& v)
{
	if (!v.is_object())
	{
		throw std::invalid_argument("Invalid JSON: not an object");
	}
	const auto& obj = v.as_object();
	type = json::value_to<int>(obj.at("type"));
	uuid_from = json::value_to<std::string>(obj.at("uuid_from"));
	if (type == Forward) 
	{
		uuid_to = json::value_to<std::string>(obj.at("uuid_to"));
		text = json::value_to<std::string>(obj.at("text"));
	}
	if (type == AddContact || type == ContactAdded || type == ContactRejected)
	{
		uuid_to = json::value_to<std::string>(obj.at("uuid_to"));
		username = json::value_to<std::string>(obj.at("username"));
	}
	/*if (type == ContactAdded)
	{
		uuid_to = json::value_to<std::string>(obj.at("uuid_to"));
		username = json::value_to<std::string>(obj.at("username"));
	}
	if (type == ContactRejected)
	{
		uui
	}*/
}

//Json doesn't work with enum so it's better to perform 
//cast like switch
message::message_type message::get_type() const
{
	switch (type) {
	case 0: return Forward;
	case 1: return Connect;
	case 2: return Disconnect;
	case 3: return AddContact;
	case 4: return ContactAdded;
	case 5: return ContactRejected;
	default: 
		throw std::invalid_argument(
		"Unknown message type received from the JSON: " +
		std::to_string(type));
	}
}

std::string message::get_uuid_from() const
{
	return uuid_from;
}

std::string message::get_uuid_to() const
{
	return uuid_to;
}

std::string message::get_username() const
{
	return username;
}
