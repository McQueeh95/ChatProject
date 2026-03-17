#pragma once
#include <string>

namespace db_protocol
{
    struct message
    {
        long long id;
        int chat_id;
        long long sender_id;
        std::string encrypted_payload;
        std::string created_at;
        bool is_read;
    };
}