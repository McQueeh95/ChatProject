#include "server_protocol.h"
#include <boost/json.hpp>
#include <boost/json/object.hpp>
#include <boost/json/value_to.hpp>
#include <stdexcept>



server_protocol::message server_protocol::parse_message(const boost::json::value &v)
{
   	if (!v.is_object())
	{
		throw std::invalid_argument("Invalid JSON: not an object");
	}
    message m;
    const auto& obj = v.as_object();
    m.type = parse_message_type(boost::json::value_to<std::string>(obj.at("type")));
    m.sender_id = boost::json::value_to<long long>(obj.at("sender"));
    m.receiver_id = boost::json::value_to<long long>(obj.at("receiver"));
    m.payload = boost::json::value_to<long long>(obj.at("payload"));
    return m;
}

boost::json::value server_protocol::serialize_message(const message &m)
{
    boost::json::object j_object;
    j_object["type"] = serialize_message_type(m.type);
    j_object["sender"] = m.sender_id;
    j_object["receiver"] = m.receiver_id;
    j_object["payload"] = m.payload;
    return j_object;
}

server_protocol::message_type server_protocol::parse_message_type(const std::string &t)
{
    if(t == "connect") return message_type::connect;
    if(t == "disconnect") return message_type::disconnect;
    if(t == "forward") return message_type::forward;
    throw std::invalid_argument("Invalid Json: type of message parse");
}

std::string server_protocol::serialize_message_type(const message_type &mt)
{
    if(mt == message_type::connect) return "connect";
    if(mt == message_type::disconnect) return "disconnect";
    if(mt == message_type::forward) return "forward";
    throw std::invalid_argument("Invalid JSON: type of message serialize");
}