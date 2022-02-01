cmake_minimum_required(VERSION 3.22)
project(okx_gateway)

set(CMAKE_CXX_STANDARD 20)

find_package(Boost 1.78.0 REQUIRED COMPONENTS log)
find_package(Threads REQUIRED)
find_package(OpenSSL REQUIRED)

# Aeron client
set(AERON_VERSION_FILE "${CMAKE_CURRENT_SOURCE_DIR}/libs/aeron/version.txt")
file(STRINGS ${AERON_VERSION_FILE} AERON_VERSION_TXT LIMIT_COUNT 1 REGEX "^[0-9]+(\\.[0-9]+)+")
add_definitions(-DAERON_VERSION_TXT="${AERON_VERSION_TXT}")
set(AERON_CLIENT_SOURCE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/libs/aeron/aeron-client/src/main/cpp")
add_subdirectory(${AERON_CLIENT_SOURCE_PATH})

# toml++
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/libs/tomlplusplus/include)

SET(SOURCE
    ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Publisher.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Subscriber.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/WSSession.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/HTTPSession.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/OKXPublic.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/OKXPrivate.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/OKXREST.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/utils.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/log.cpp)

SET(HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Publisher.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Subscriber.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/WSSession.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/HTTPSession.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/OKXPublic.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/OKXPrivate.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/OKXREST.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/utils.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/log.h)

add_executable(okx_gateway ${SOURCE} ${HEADERS})

target_link_libraries(okx_gateway
    Threads::Threads
    OpenSSL::SSL
    Boost::log
    aeron_client
)
