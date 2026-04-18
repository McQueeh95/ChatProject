#include "server_protocol.hpp"
#include <boost/json.hpp>
#include <boost/json/conversion.hpp>
#include <boost/json/detail/value_to.hpp>
#include <boost/json/object.hpp>
#include <boost/json/value_from.hpp>
#include <boost/json/value_to.hpp>
#include <cstdint>

namespace json = boost::json;

namespace server_protocol
{
    login_req tag_invoke(json::value_to_tag<login_req>, const json::value& jv)
    {
        const auto& obj = jv.as_object();

        return {
            json::value_to<std::string>(obj.at("username")),
            json::value_to<std::string>(obj.at("auth_key"))
        };

    }

    reg_req tag_invoke(json::value_to_tag<reg_req>, const json::value& jv)
    {
        const auto& obj = jv.as_object();
        return {
            json::value_to<std::string>(obj.at("username")),
            json::value_to<std::string>(obj.at("auth_key")),
            json::value_to<std::string>(obj.at("salt")),
            json::value_to<std::string>(obj.at("public_key")),
            json::value_to<std::string>(obj.at("encrypted_vault")),
            json::value_to<std::string>(obj.at("vault_nonce"))
        };

    }

    //json to incoming_message
    msg_forw_req tag_invoke(json::value_to_tag<msg_forw_req>, const json::value& jv)
    {
        const auto& obj = jv.as_object();
        return {
            json::value_to<int64_t>(obj.at("local_id")),
            json::value_to<int64_t>(obj.at("chat_id")),
            json::value_to<std::string>(obj.at("payload"))
        };
    }

    start_chat_req tag_invoke(json::value_to_tag<start_chat_req>, const json::value& jv)
    {
        const auto& obj = jv.as_object();
        return {
            json::value_to<int64_t>(obj.at("local_id")),
            json::value_to<int64_t>(obj.at("target_id")),
            json::value_to<std::string>(obj.at("payload"))
        };
    }

    //json to read_ack_incoming
    read_conf_req tag_invoke(json::value_to_tag<read_conf_req>, const json::value& jv)
    {
        const auto& obj = jv.as_object();
        return{
            json::value_to<int64_t>(obj.at("chat_id")),
            json::value_to<int64_t>(obj.at("last_read_msg_id"))
        };
    }

    search_req tag_invoke(json::value_to_tag<search_req>, const json::value& jv)
    {
        const auto& obj = jv.as_object();
        return{
            json::value_to<std::string>(obj.at("to_find"))
        };
    }

    //outgoing_message to json
    void tag_invoke(json::value_from_tag,  json::value& jv, const msg_deliv& msg)
    {
        jv = {
            {"type", static_cast<int8_t>(message_type::FORW)},
            {"message_id", msg.message_id},
            {"local_id", msg.local_id},
            {"chat_id", msg.chat_id},
            {"sender_id", msg.sender_id},
            {"payload", msg.payload},
            {"timestamp", msg.timestamp},
            {"is_read", msg.is_read}
        };
    }

    //ack_message to json
    void tag_invoke(json::value_from_tag, json::value& jv, const status_res& msg)
    {
        jv = {
            {"local_id", msg.local_id},
            {"message_id", msg.message_id},
            {"chat_id", msg.chat_id},
            {"timestamp", msg.timestamp}
        };
    }

    //ack_read_outgoing to json
    void tag_invoke(json::value_from_tag, json::value& jv, const read_confirm_notif& msg)
    {
        jv = {
            {"chat_id", msg.chat_id},
            {"last_read_message_id", msg.last_read_message_id},
            {"reader_id", msg.reader_id}
        };
    }

    void tag_invoke(json::value_from_tag, json::value& jv, const login_res& msg)
    {
        boost::json::object obj;
        obj["type"] = static_cast<int8_t>(message_type::LOGIN_RES);
        obj["status"] = msg.status;

        if(msg.status == "ok")
        {
            obj["user_info"] =json::value_from(msg.user_info);
            obj["chats"] = json::value_from(msg.chats);
        }
        else
            obj["error_msg"] = msg.error_msg;

        jv = std::move(obj);
    }

    void tag_invoke(json::value_from_tag, json::value& jv, const reg_res& msg)
    {
        boost::json::object obj;
        obj["type"] = static_cast<int8_t>(message_type::REG_RES);
        obj["status"] = msg.status;

        if(msg.status == "ok")
        {
            obj["user_id"] = msg.user_id;
        }
        else
            obj["error_msg"] = msg.error_msg;

        jv = std::move(obj);
    }

    void tag_invoke(json::value_from_tag, json::value& jv, const user_search& msg)
    {
        jv = {
            {"user_id", msg.user_id},
            {"username", msg.username},
            {"public_key", msg.public_key}
        };
    }

    void tag_invoke(json::value_from_tag, json::value& jv, const search_res& msg)
    {
        json::object obj;
        obj["type"] = static_cast<int8_t>(message_type::SEARCH_RES);
        obj["found_users"] = json::value_from(msg.found_users);

        jv = std::move(obj);
    }

    void tag_invoke(json::value_from_tag, json::value& jv, const deliv_ack& msg)
    {
        json::object obj;
        obj["type"] = static_cast<int8_t>(message_type::DELIV_ACK);
        obj["status"] = msg.status;
        obj["chat_id"] = msg.chat_id;
        obj["local_id"] = msg.local_id;

        if(msg.status == "ok")
        {
            obj["real_id"] = msg.real_id; 
            obj["timestamp"] = msg.timestamp;
            obj["peer_id"] = msg.peer_id;
        }
        else
        {
            obj["error_msg"] = msg.error_msg;
        }
        jv = std::move(obj);
    }

    void tag_invoke(json::value_from_tag, json::value& jv, const user_info& msg)
    {
        jv = {
            {"user_id", msg.user_id},
            {"encrypted_vault", msg.encrypted_vault},
            {"vault_nonce", msg.vault_nonce}
        };
    }

    void tag_invoke(json::value_from_tag, json::value& jv, const chat_info& msg)
    {
        jv = {
            {"chat_id", msg.chat_id},
            {"peer_id", msg.peer_id},
            {"peer_username", msg.peer_username},
            {"public_key", msg.public_key}
        };
    }

    history_req tag_invoke(json::value_to_tag<history_req>, const json::value &jv)
    {
        const auto& obj = jv.as_object();
        return { json::value_to<int64_t>(obj.at("chat_id"))};
    }

    void tag_invoke(json::value_from_tag, json::value &jv, const history_res& msg)
    {
        json::object obj;
        obj["type"] = static_cast<int64_t>(message_type::HISTORY_RES);
        obj["status"] = msg.status;
        obj["chat_id"] = msg.chat_id;
        if(msg.status == "ok")
        {
            obj["messages"] = json::value_from(msg.messages);
        }
        else {
            obj["error_msg"] = msg.error_message;
        }
        jv = (std::move(obj));
    }

    void tag_invoke(json::value_from_tag, json::value &jv,const salt_res& msg)
    {
        json::object obj;
        obj["type"] = static_cast<uint8_t>(server_protocol::message_type::SALT_RES);
        obj["salt"] = msg.salt;
        jv = std::move(obj);
    }

    salt_req tag_invoke(json::value_to_tag<salt_req>, const json::value &jv)
    {
        const auto& obj = jv.as_object();
        return{
            json::value_to<std::string>(obj.at("username"))
        };
    }

}