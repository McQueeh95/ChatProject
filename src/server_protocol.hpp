#pragma once
#include <boost/json/conversion.hpp>
#include <boost/json/detail/value_to.hpp>
#include <boost/json/object.hpp>
#include <boost/json/value_to.hpp>
#include <cstdint>
#include <string>
#include <boost/json.hpp>
#include <vector>
namespace server_protocol
{
    enum class message_type
    {
        LOGIN = 0,
        FORW = 1,
        REG = 2,
        LOGIN_RES = 3,
        REG_RES = 4,
        SEARCH_REQ = 5,
        SEARCH_RES = 6,
        LOGOUT = 7,
        DELIV_ACK = 8,
        READ_ACK = 9,
        HISTORY_REQ = 10,
        HISTORY_RES = 11
    };

    struct login_req
    {
        std::string username;
        std::string hashed_password;
        friend login_req tag_invoke(boost::json::value_to_tag<login_req>, 
            const boost::json::value& jv);
    };

    struct reg_req
    {
        std::string username;
        std::string hashed_password;
        friend reg_req tag_invoke(boost::json::value_to_tag<reg_req>, 
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
        std::string to_find;

        friend search_req tag_invoke(boost::json::value_to_tag<server_protocol::search_req>, 
            const boost::json::value& jv);
    };

    //sent by server
    struct msg_deliv
    {
        int64_t message_id;
        int64_t local_id;
        int64_t chat_id;
        int64_t sender_id;
        std::string payload;
        std::string timestamp;
        bool is_read;
        friend void tag_invoke(boost::json::value_from_tag,
            boost::json::value& jv,  const msg_deliv &msg);
        
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

    struct chat_info{
        int64_t chat_id;
        std::string peer_username;

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value& jv, 
            const chat_info& msg);
    };

    struct login_res
    {
        std::string status;
        int64_t user_id;
        std::string error_msg;
        std::vector<chat_info> chats;
        friend void tag_invoke(boost::json::value_from_tag, boost::json::value& jv, 
            const login_res& msg);
    };

    struct reg_res
    {
        std::string status;
        int64_t user_id;
        std::string error_msg;
        friend void tag_invoke(boost::json::value_from_tag, boost::json::value& jv, 
            const reg_res& msg);
    };

    struct user_search
    {
        int64_t user_id;
        std::string username;

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value& jv, 
            const user_search& msg);
    };

    struct search_res
    {
        std::vector<user_search> found_users; 

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value& jv,
            const search_res& msg);
    };

    /*struct search_res
    {
        std::string status;
        int64_t chat_id = -1;
        std::string peer_username;
        std::string error_msg;

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value& jv,
            const search_res& msg);
    };*/

    struct deliv_ack
    {
        std::string status;
        int64_t chat_id;
        int64_t local_id;
        int64_t real_id;
        std::string timestamp;
        std::string error_msg;

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value& jv,
            const deliv_ack& msg);
    };

    struct history_req
    {
        int64_t chat_id;

        friend history_req tag_invoke(boost::json::value_to_tag<history_req>, 
            const boost::json::value &jv);
    };

    struct history_res
    {
        std::string status;
        std::string error_message;
        int64_t chat_id;
        std::vector<msg_deliv> messages;

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv,
            const history_res& msg);
    };
}
