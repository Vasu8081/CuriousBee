cmake_minimum_required(VERSION 3.16)
project(curious_backend)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

include_directories(${CMAKE_SOURCE_DIR}/include)
find_package(spdlog REQUIRED)
find_package(fmt REQUIRED)
find_package(nlohmann_json REQUIRED)


add_subdirectory(base)
add_subdirectory(network_parser)

# add_subdirectory(messages)
# add_subdirectory(network)
# add_subdirectory(server)
# add_subdirectory(database_parser)
# add_subdirectory(youtube)