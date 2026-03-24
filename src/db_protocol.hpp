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
        std::string created_at;
        bool is_read = false;
    };

    struct user_chat
    {
        int64_t chat_id;
        std::string peer_username;
    };
}