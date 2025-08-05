cmake_minimum_required(VERSION 3.16)
project(curious_backend)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Output all binaries to bin/
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Global include path for headers like include/server/server_config.h
include_directories(${CMAKE_SOURCE_DIR}/include)
include_directories(${CMAKE_BINARY_DIR}/)

# Add submodules
add_subdirectory(messages)
add_subdirectory(network)
add_subdirectory(server)
add_subdirectory(network_parser)
add_subdirectory(database_parser)
add_subdirectory(youtube)