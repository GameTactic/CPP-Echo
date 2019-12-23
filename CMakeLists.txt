cmake_minimum_required(VERSION 3.2)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -O3")
set(CMAKE_INSTALL_PREFIX ${PROJECT_SOURCE_DIR})
set(SERVER_INSTALL_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/include)
set(SERVER_INSTALL_BIN_DIR ${PROJECT_SOURCE_DIR}/bin)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")
add_definitions(-DBOOST_ERROR_CODE_HEADER_ONLY)

include_directories(include)

# JSON Library Config
option(SERVER_USE_EXTERNAL_JSON "Use an external JSON library" OFF)
if(SERVER_USE_EXTERNAL_JSON)
  find_package(nlohmann_json 3.2.0 REQUIRED)
else()
  set(JSON_BuildTests OFF CACHE INTERNAL "")
  add_subdirectory(${SERVER_INSTALL_INCLUDE_DIR}/json)
endif()

add_subdirectory(src)

