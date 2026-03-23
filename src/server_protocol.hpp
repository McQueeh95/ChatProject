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
        LOGOUT = 6,
        ACK = 7,
        READ_ACK = 8
    };
    
    //received by server on connect
    struct auth_req
    {
        //int64_t sender_id;
        std::string username;
        std::string hashed_password;
        friend auth_req tag_invoke(boost::json::value_to_tag<auth_req>, 
            const boost::json::value& jv);
    };

    //received by server
    struct msg_forw_req
    {
        int64_t local_id;
        int64_t chat_id;
        std::string payload;
        friend msg_forw_req tag_invoke(boost::json::value_to_tag<msg_forw_req>, 
            const boost::json::value& jv);
    };

    struct search_req
    {
        std::string peer_username;

        friend search_req tag_invoke(boost::json::value_to_tag<server_protocol::search_req>, 
            const boost::json::value& jv);
    };

    //sent by server
    struct msg_del
    {
        int64_t message_id;
        int64_t chat_id;
        int64_t sender_id;
        std::string payload;
        std::string timestamp;
        bool is_read;
        friend void tag_invoke(boost::json::value_from_tag,
            boost::json::value& jv,  const msg_del &msg);
        
    };

    //saved in DB ACK
    struct status_res
    {
        int64_t local_id;
        int64_t message_id;
        int64_t chat_id;
        std::string timestamp;

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value& jv, const status_res& msg);
    };

    //msg read ACK to server
    struct read_conf_req
    {
        int64_t chat_id;
        int64_t last_read_message_id;

        friend read_conf_req tag_invoke(boost::json::value_to_tag<read_conf_req>, 
            const boost::json::value& jv);
    };

    //msg read ACK to client(sender)
    struct read_confirm_notif
    {
        int64_t chat_id;
        int64_t last_read_message_id;
        int64_t reader_id;

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value& jv, 
            const read_conf_req& msg);
    };

    struct auth_res
    {
        std::string status;
        int64_t user_id;
        std::string error_msg;

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value& jv, 
            const auth_res& msg);
    };

    struct logout_req{
        int64_t user_id;
    };

    struct search_res
    {
        std::string status;
        int64_t chat_id = -1;
        std::string peer_username;
        std::string error_msg;

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value& jv,
            const search_res& msg);
    };

}
