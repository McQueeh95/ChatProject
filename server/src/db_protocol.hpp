#pragma once
#include <string>

namespace db_protocol
{
    struct message
    {
        int64_t id;
        int64_t chat_id;
        int64_t sender_id;
        std::string encrypted_payload;
        std::string nonce;
        std::string created_at;
        bool is_read = false;
    };

    struct user_info
    {
        int64_t id;
        std::string encypted_vault;
        std::string vaule_nonce;
    };

    struct user_chat
    {
        int64_t chat_id;
        int64_t peer_id;
        std::string peer_username;
        std::string public_key;
    };

    struct found_user
    {
        int64_t user_id;
        std::string username;
        std::string public_key;
    };

}