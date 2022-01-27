cmake_minimum_required(VERSION 3.22)
project(okx_gateway)

set(CMAKE_CXX_STANDARD 20)

find_package(Boost 1.78.0 REQUIRED COMPONENTS log)
find_package(Threads REQUIRED)
find_package(OpenSSL REQUIRED)

include_directories(/home/ubuntu/aeron/aeron-client/src/main/cpp)

add_executable(
        main main.cpp
        src/Publisher.cpp   src/includes/Publisher.h
        src/Subscriber.cpp  src/includes/Subscriber.h
        src/WSSession.cpp   src/includes/WSSession.h
        src/HTTPSession.cpp src/includes/HTTPSession.h
        src/OKXPublic.cpp   src/includes/OKXPublic.h
        src/OKXPrivate.cpp  src/includes/OKXPrivate.h
        src/OKXREST.cpp     src/includes/OKXREST.h
        src/utils.cpp       src/includes/utils.h
)
target_link_libraries(
        main
        Threads::Threads
        OpenSSL::SSL
        Boost::log
        libaeron_client.a
)
