#pragma once
#include <bitset>
#include <boost/asio/executor_work_guard.hpp>
#include <optional>
#include <pqxx/pqxx>
#include <boost/asio/io_context.hpp>
#include <thread>
#include "dependencies.hpp"
#include "db_protocol.hpp"
#include <cstdint>
#include <vector>

class database
{
    private:
    pqxx::connection connection_;
    net::io_context ioc_;
    boost::asio::executor_work_guard<net::io_context::executor_type> work_guard_;
    std::thread db_thread_;
    void prepare_statements();
    public:
    database(const std::string& connection_string);
    void start();
    ~database();
    database (const database&) = delete;
    database& operator= (const database&) = delete;
    void post_task(std::function<void()> task);
    std::optional<int64_t> create_user(const std::string &username, const std::string &password);
    std::optional<int64_t> login_user(const std::string &username, const std::string &password);
    std::optional<int64_t> get_recepeint_id(int64_t chat_id, int64_t sender_id);
    std::optional<db_protocol::message> insert_msg(int64_t chat_id, int64_t sender_id, const std::string& text);
    std::optional<int64_t> upsert_chat(int64_t user1_id, int64_t user2_id);
    std::vector<db_protocol::found_user> get_searched_users(const std::string &query);
    std::vector<db_protocol::user_chat> get_user_chats(int64_t user_id);
    std::vector<db_protocol::message> get_messages(int64_t chat_id);
    std::optional<std::string> get_username(int64_t user_id);

};