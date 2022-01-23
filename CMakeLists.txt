cmake_minimum_required(VERSION 3.22)
project(okx_gateway)

set(CMAKE_CXX_STANDARD 20)

find_package(Boost 1.78.0 REQUIRED COMPONENTS log)
find_package(Threads REQUIRED)
find_package(OpenSSL REQUIRED)

include_directories(/home/ubuntu/aeron/aeron-client/src/main/cpp)

add_executable(main main.cpp includes/Publisher.h Publisher.cpp includes/Subscriber.h Subscriber.cpp
        includes/WSSession.h WSSession.cpp includes/HTTPSession.h HTTPSession.cpp includes/OKXPublic.h OKXPublic.cpp
        includes/OKXPrivate.h OKXPrivate.cpp includes/utils.h utils.cpp)
target_link_libraries(main Threads::Threads OpenSSL::SSL Boost::log libaeron_client.a)
