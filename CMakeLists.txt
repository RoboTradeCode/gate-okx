cmake_minimum_required(VERSION 3.22)
project(okex_gateway)

set(CMAKE_CXX_STANDARD 20)

find_package(Boost 1.78.0 REQUIRED)
find_package(Threads REQUIRED)
find_package(OpenSSL REQUIRED)

include_directories(/home/ubuntu/aeron/aeron-client/src/main/cpp)

#include(FetchContent)
#FetchContent_Declare(simdjson GIT_REPOSITORY https://github.com/simdjson/simdjson.git GIT_TAG  tags/v1.0.2 GIT_SHALLOW TRUE)
#FetchContent_MakeAvailable(simdjson)

add_executable(main main.cpp OKExSession.h Publisher.cpp Publisher.h Subscriber.cpp Subscriber.h)
target_link_libraries(main Threads::Threads OpenSSL::SSL OpenSSL::Crypto libaeron_client.a)
