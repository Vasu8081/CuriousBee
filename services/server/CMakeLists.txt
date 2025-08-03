# Collect all source files in server/src/
file(GLOB SERVER_SOURCES src/*.cpp)

# Build the library named "server"
add_library(server STATIC ${SERVER_SOURCES})

# Make headers available to anything that links this
target_include_directories(server
    PUBLIC ${CMAKE_SOURCE_DIR}/include
)
target_include_directories(server PUBLIC /usr/local/include)

# Link external libraries if needed (e.g., nlohmann_json)
find_package(nlohmann_json REQUIRED)
find_package(spdlog REQUIRED)
target_link_libraries(server PRIVATE network nlohmann_json::nlohmann_json zmq spdlog::spdlog)
