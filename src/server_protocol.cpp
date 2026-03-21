#include "server_protocol.hpp"
#include <boost/json.hpp>
#include <boost/json/conversion.hpp>
#include <boost/json/detail/value_to.hpp>
#include <boost/json/object.hpp>
#include <boost/json/value_to.hpp>
#include <iostream>

namespace json = boost::json;

namespace server_protocol
{
    //json to connect_message
    connect_message tag_invoke(json::value_to_tag<connect_message>, const json::value& jv)
    {
        std::cout << "to connect start" << std::endl;
        const auto& obj = jv.as_object();
         std::cout << "to connect after to obj" << std::endl;
        return {
            //json::value_to<int64_t>(obj.at("sender_id")),
            json::value_to<std::string>(obj.at("username")),
            json::value_to<std::string>(obj.at("hashed_password"))
        };
    }

    //json to incoming_message
    incoming_message tag_invoke(json::value_to_tag<incoming_message>, const json::value& jv)
    {
        const auto& obj = jv.as_object();
        return {
            json::value_to<int64_t>(obj.at("local_id")),
            json::value_to<int64_t>(obj.at("chat_id")),
            json::value_to<std::string>(obj.at("payload"))
        };
    }

    //json to read_ack_incoming
    read_ack_incoming tag_invoke(json::value_to_tag<read_ack_incoming>, const json::value& jv)
    {
        const auto& obj = jv.as_object();
        return{
            json::value_to<int64_t>(obj.at("chat_id")),
            json::value_to<int64_t>(obj.at("last_read_msg_id"))
        };
    }

    search_request tag_invoke(json::value_to_tag<search_request>, const json::value& jv)
    {
        const auto& obj = jv.as_object();
        return{
            json::value_to<std::string>(obj.at("peer_username"))
        };
    }

    //outgoing_message to json
    void tag_invoke(json::value_from_tag,  json::value& jv, const outgoing_message& msg)
    {
        jv = {
            {"type", static_cast<int>(message_type::FORW)},
            {"message_id", msg.message_id},
            {"chat_id", msg.chat_id},
            {"sender_id", msg.sender_id},
            {"payload", msg.payload},
            {"timestamp", msg.timestamp},
            {"is_read", msg.is_read}
        };
    }

    //ack_message to json
    void tag_invoke(json::value_from_tag, json::value& jv, const ack_message& msg)
    {
        jv = {
            {"local_id", msg.local_id},
            {"message_id", msg.message_id},
            {"chat_id", msg.chat_id},
            {"timestamp", msg.timestamp}
        };
    }

    //ack_read_outgoing to json
    void tag_invoke(json::value_from_tag, json::value& jv, const read_ack_outgoing& msg)
    {
        jv = {
            {"chat_id", msg.chat_id},
            {"last_read_message_id", msg.last_read_message_id},
            {"reader_id", msg.reader_id}
        };
    }

    void tag_invoke(json::value_from_tag, json::value& jv, const auth_response& msg)
    {
        boost::json::object obj;
        obj["type"] = static_cast<int>(message_type::AUTH_RES);
        obj["status"] = msg.status;

        if(msg.status == "ok")
            obj["user_id"] = msg.user_id;
        else
            obj["error_msg"] = msg.error_msg;

        jv = std::move(obj);
    }

}