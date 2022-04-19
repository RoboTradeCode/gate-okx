cmake_minimum_required(VERSION 3.22)
project(okx_gateway)

set(CMAKE_CXX_STANDARD 20)

find_package(Threads REQUIRED)
find_package(OpenSSL REQUIRED)
find_package(Boost 1.78.0 REQUIRED COMPONENTS log_setup log)

# Aeron Utils
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/libs/aeron_cpp EXCLUDE_FROM_ALL)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/libs/aeron_cpp/src)

# Sentry
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/libs/sentry-native EXCLUDE_FROM_ALL)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/libs/sentry-native/include)

# Simdjson
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/libs/simdjson EXCLUDE_FROM_ALL)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/libs/simdjson/include)

# Toml++
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/libs/tomlplusplus EXCLUDE_FROM_ALL)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/libs/tomlplusplus/include)

SET(SOURCE
    ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/WSSession.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/HTTPSession.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/OKXPublic.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/OKXPrivate.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/OKXREST.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/utils.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/log.cpp src/OKXGateway.cpp src/OKXGateway.h)

SET(HEADERS
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
    Boost::log_setup
    Boost::log
    aeron_utils::aeron_utils
    aeron_client
    sentry::sentry
    simdjson::simdjson
    tomlplusplus::tomlplusplus)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/config.toml ${CMAKE_CURRENT_BINARY_DIR}/config.toml COPYONLY)
