cmake_minimum_required(VERSION 3.16)
project(curious_backend)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

find_package(spdlog REQUIRED)
find_package(fmt REQUIRED)
find_package(nlohmann_json REQUIRED)

# Find the installed ODB headers and libraries (default locations)
include_directories(/usr/include)
include_directories(/usr/include/odb)          # ODB core
include_directories(/usr/include/odb/pgsql)    # ODB PGSQL (if using Postgres)

include_directories(${CMAKE_SOURCE_DIR}/include)
add_subdirectory(src)