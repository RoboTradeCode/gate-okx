cmake_minimum_required(VERSION 3.22)
project(okx_gateway)

set(CMAKE_CXX_STANDARD 20)

find_package(Boost 1.78.0 REQUIRED COMPONENTS log)
find_package(Threads REQUIRED)
find_package(OpenSSL REQUIRED)

include_directories(/home/ubuntu/aeron/aeron-client/src/main/cpp)

add_executable(main main.cpp Publisher.cpp Subscriber.cpp WSSession.cpp HTTPSession.cpp OKXPublic.cpp OKXPrivate.cpp
        OKXREST.cpp utils.cpp includes/Publisher.h includes/Subscriber.h includes/WSSession.h includes/HTTPSession.h
        includes/OKXPublic.h includes/OKXPrivate.h includes/OKXREST.h includes/utils.h)
target_link_libraries(main Threads::Threads OpenSSL::SSL Boost::log libaeron_client.a)
