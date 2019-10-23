cmake_minimum_required(VERSION 3.2)
project(cppecho)

set(SOURCE_FILES main.cpp)
add_executable(server ${SOURCE_FILES})

find_path(WEBSOCKSETS_INCLUDE_DIR NAMES websocketpp/server.hpp)
target_include_directories(server PRIVATE "${PROJECT_SOURCE_DIR}/include" ${WEBSOCKSETS_INCLUDE_DIR})

install(TARGETS server DESTINATION ${SERVER_INSTALL_BIN_DIR})
