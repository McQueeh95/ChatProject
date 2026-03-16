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
add_executable (server "src/server.cpp" "src/server.h" 
"src/session.cpp" "src/session.h" 
"src/listener.cpp" "src/listener.h" 
"src/dependencies.h" 
"src/sessions_manager.cpp" "src/sessions_manager.h" 
"src/message.h" "src/message.cpp" 
"src/database.h" "src/database.cpp"
"src/dbworker.h" "src/dbworker.cpp"
"src/server_protocol.h" "src/server_protocol.cpp")

target_link_libraries(server PRIVATE 
                    Boost::system
                    Boost::json
                    pqxx)

# TODO: Add tests and install targets if needed.
