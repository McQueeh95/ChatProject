#pragma once
#include <string>
#include <boost/json.hpp>
#include <cstdint>

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
        HISTORY_RES = 11,
        START_CHAT_REQ = 12,
        NEW_CHAT_EVENT = 13,
        SALT_REQ = 14,
        SALT_RES = 15,
        RECON_REQ = 16,
        RECON_RES = 17
    };

    struct user_info
    {
        int64_t user_id;
        std::string encrypted_vault;
        std::string vault_nonce;
        friend void tag_invoke(boost::json::value_from_tag, boost::json::value& jv, 
            const user_info& msg);
    };

    struct chat_info{
        int64_t chat_id;
        int64_t peer_id;
        std::string peer_username;
        std::string public_key;
        std::string last_msg;
        std::string timestamp;
        std::string nonce;

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value& jv, 
            const chat_info& msg);
    };

    struct user_search
    {
        int64_t user_id;
        std::string username;
        std::string public_key;

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value& jv, 
            const user_search& msg);
    };

    struct msg_deliv
    {
        int64_t message_id;
        int64_t local_id;
        int64_t chat_id;
        int64_t sender_id;
        std::string payload;
        std::string nonce;
        std::string timestamp;
        bool is_read;
        friend void tag_invoke(boost::json::value_from_tag,
            boost::json::value& jv,  const msg_deliv &msg);
        
    };
}