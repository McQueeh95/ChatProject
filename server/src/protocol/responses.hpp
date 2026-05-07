#pragma once
#include "models.hpp"

namespace server_protocol
{
    struct reg_res
    {
        std::string status;
        int64_t user_id;
        std::string error_msg;
        friend void tag_invoke(boost::json::value_from_tag, boost::json::value& jv, 
            const reg_res& msg);
    };

    struct login_res
    {
        std::string status;
        server_protocol::user_info user_info;
        std::string error_msg;
        std::vector<chat_info> chats;
        friend void tag_invoke(boost::json::value_from_tag, boost::json::value& jv, 
            const login_res& msg);
    };

    struct salt_res
    {
        std::string salt;

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv,
            const salt_res& msg);
    };

    struct search_res
    {
        std::vector<user_search> found_users; 

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value& jv,
            const search_res& msg);
    };

    //pair for msg_forw_req
    struct deliv_ack
    {
        std::string status;
        int64_t chat_id;
        int64_t local_id;
        int64_t real_id;
        std::string timestamp;
        std::string error_msg;
        int64_t peer_id;

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value& jv,
            const deliv_ack& msg);
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

    struct recon_res
    {
        std::string status;
        int64_t user_id;
        std::string error_msg;

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv,
            const recon_res& msg);
    };
}