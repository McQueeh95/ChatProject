#pragma once
#include <boost/json/conversion.hpp>
#include <boost/json/detail/value_to.hpp>
#include <boost/json/object.hpp>
#include <boost/json/value_to.hpp>
#include <string>
#include <boost/json.hpp>
namespace server_protocol
{
    enum class message_type
    {
        AUTH = 0,
        FORW = 1,
        REG = 2,
        AUTH_RES = 3,
        SEARCH = 4,
        SEARCH_RES = 5,
        DISCONN = 6,
        ACK = 7,
        READ_ACK = 8
    };
    
    //received by server on connect
    struct connect_message
    {
        //int64_t sender_id;
        std::string username;
        std::string hashed_password;
        friend connect_message tag_invoke(boost::json::value_to_tag<connect_message>, 
            const boost::json::value& jv);
    };

    //received by server
    struct incoming_message
    {
        int64_t local_id;
        int64_t chat_id;
        std::string payload;
        friend incoming_message tag_invoke(boost::json::value_to_tag<incoming_message>, 
            const boost::json::value& jv);
    };

    struct search_request
    {
        std::string peer_username;

        friend search_request tag_invoke(boost::json::value_to_tag<server_protocol::search_request>, 
            const boost::json::value& jv);
    };

    //sent by server
    struct outgoing_message
    {
        int64_t message_id;
        int64_t chat_id;
        int64_t sender_id;
        std::string payload;
        std::string timestamp;
        bool is_read;
        friend void tag_invoke(boost::json::value_from_tag,
            boost::json::value& jv,  const outgoing_message &msg);
        
    };

    //saved in DB ACK
    struct ack_message
    {
        int64_t local_id;
        int64_t message_id;
        int64_t chat_id;
        std::string timestamp;

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value& jv, const ack_message& msg);
    };

    //msg read ACK to server
    struct read_ack_incoming
    {
        int64_t chat_id;
        int64_t last_read_message_id;

        friend read_ack_incoming tag_invoke(boost::json::value_to_tag<read_ack_incoming>, 
            const boost::json::value& jv);
    };

    //msg read ACK to client(sender)
    struct read_ack_outgoing
    {
        int64_t chat_id;
        int64_t last_read_message_id;
        int64_t reader_id;

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value& jv, 
            const read_ack_outgoing& msg);
    };

    struct auth_response
    {
        std::string status;
        int64_t user_id;
        std::string error_msg;

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value& jv, 
            const auth_response& msg);
    };
}
