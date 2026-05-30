# CMakeList.txt : CMake project for server, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.10)

project ("server")

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(Boost REQUIRED COMPONENTS system json)
find_package(libpqxx REQUIRED)

add_executable (server "src/server.cpp"
"src/dependencies.hpp"
"src/network/session.cpp" "src/network/session.hpp" 
"src/network/listener.cpp" "src/network/listener.hpp" 
"src/core/sessions_manager.cpp" "src/core/sessions_manager.hpp" 
"src/database/database.hpp" "src/database/database.cpp"
"src/database/db_protocol.hpp"
"src/protocol/requests.hpp" "src/protocol/responses.hpp"
"src/protocol/server_protocol.cpp"
"src/protocol/mappers.hpp")

target_link_libraries(server PRIVATE 
                    Boost::system
                    Boost::json
                    pqxx)

# ===============================================
# Tests
# ===============================================
find_package(GTest REQUIRED)

add_executable(server_tests
    tests/test_protocol.cpp
    src/protocol/server_protocol.cpp
)

target_link_libraries(server_tests
    PRIVATE
    GTest::gtest_main
    Boost::json
)

include(GoogleTest)
gtest_discover_tests(server_tests)



# TODO: Add tests and install targets if needed.
