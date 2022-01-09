cmake_minimum_required(VERSION 3.22)
project(okex_gateway)

set(CMAKE_CXX_STANDARD 20)

find_package(Boost 1.78.0 REQUIRED)
find_package(Threads REQUIRED)
find_package(OpenSSL REQUIRED)

add_executable(websocket_client_async_ssl websocket_client_async_ssl.cpp)
target_link_libraries(websocket_client_async_ssl Threads::Threads OpenSSL::SSL OpenSSL::Crypto)
