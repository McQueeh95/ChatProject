#pragma once
#include "models.hpp"

namespace server_protocol
{
    struct reg_req
    {
        std::string username;
        std::string auth_key;
        std::string salt;
        std::string public_key;
        std::string ecnrypted_vault;
        std::string vault_nonce;
        std::string session_token;
        friend reg_req tag_invoke(boost::json::value_to_tag<reg_req>, 
            const boost::json::value& jv);
    };

    struct login_req
    {
        std::string username;
        std::string auth_key;
        std::string session_token;
        friend login_req tag_invoke(boost::json::value_to_tag<login_req>, 
            const boost::json::value& jv);
    };

    struct salt_req
    {
        std::string username;

        friend salt_req tag_invoke(boost::json::value_to_tag<salt_req>, 
            const boost::json::value &jv);
    };

    struct search_req
    {
        std::string to_find;

        friend search_req tag_invoke(boost::json::value_to_tag<server_protocol::search_req>, 
            const boost::json::value& jv);
    };

    //pair for deliv_ack
    struct msg_forw_req
    {
        int64_t local_id;
        int64_t chat_id;
        std::string payload;
        std::string nonce;
        friend msg_forw_req tag_invoke(boost::json::value_to_tag<msg_forw_req>, 
            const boost::json::value& jv);
    };

    struct history_req
    {
        int64_t chat_id;

        friend history_req tag_invoke(boost::json::value_to_tag<history_req>, 
            const boost::json::value &jv);
    };

    struct recon_req
    {
        std::string session_token;

        friend recon_req tag_invoke(boost::json::value_to_tag<recon_req>, 
            const boost::json::value &jv);
    };

    struct start_chat_req
    {
        int64_t msg_local_id;
        int64_t target_id;
        std::string payload;
        std::string nonce;
        friend start_chat_req tag_invoke(boost::json::value_to_tag<start_chat_req>, 
            const boost::json::value& jv);
    };

}

