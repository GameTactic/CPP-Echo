cmake_minimum_required(VERSION 3.2)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -O3")
add_definitions(-DBOOST_ERROR_CODE_HEADER_ONLY)

include_directories(include)

set(CMAKE_INSTALL_PREFIX ${PROJECT_SOURCE_DIR})

set(SERVER_INSTALL_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/include)
set(SERVER_INSTALL_BIN_DIR ${PROJECT_SOURCE_DIR}/bin)

# Something is broken with Libboost on Debian & Ubuntu.
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")

add_subdirectory(src)

