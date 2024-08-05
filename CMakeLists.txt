cmake_minimum_required(VERSION 3.15)
project(DiscordBot VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(DPP_ROOT "/usr/local")
set(DPP_INCLUDE_DIR "${DPP_ROOT}/include")
set(DPP_LIBRARY "${DPP_ROOT}/lib/libdpp.dylib")

set(OPENSSL_ROOT_DIR "/usr/local/opt/openssl")

set(YAML_CPP_ROOT "/usr/local/opt/yaml-cpp")
set(YAML_CPP_INCLUDE_DIR "${YAML_CPP_ROOT}/include")
set(YAML_CPP_LIBRARY "${YAML_CPP_ROOT}/lib/libyaml-cpp.dylib")

find_package(CURL REQUIRED)

find_package(nlohmann_json 3.2.0 REQUIRED)
find_package(OpenSSL REQUIRED)

set(SOURCES
    src/main.cpp
    src/bot.cpp
    src/commands/ping_command.cpp
    src/commands/chat_command.cpp
    src/logic/commandr_logic.cpp
    src/utils/config_loader.cpp
)

add_executable(discord-bot ${SOURCES})

target_include_directories(discord-bot PRIVATE 
    ${DPP_INCLUDE_DIR}
    ${YAML_CPP_INCLUDE_DIR}
    ${CMAKE_SOURCE_DIR}/src
    ${CURL_INCLUDE_DIRS}
)

target_link_libraries(discord-bot PRIVATE 
    ${DPP_LIBRARY}
    ${YAML_CPP_LIBRARY}
    OpenSSL::SSL 
    OpenSSL::Crypto
    ${CURL_LIBRARIES}
    nlohmann_json::nlohmann_json
)