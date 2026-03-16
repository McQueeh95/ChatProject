#pragma once
#include <boost/json/object.hpp>
#include <string>
#include <boost/json.hpp>
namespace server_protocol
{
    enum class message_type
    {
        connect,
        disconnect,
        forward
    };
    struct message
    {
        message_type type;
        long long sender_id;
        long long receiver_id;
        std::string payload;
    };
    message parse_message(const boost::json::value& v);
    boost::json::value serialize_message(const message& m);
    message_type parse_message_type(const std::string& t);
    std::string serialize_message_type(const message_type& mt);
}
