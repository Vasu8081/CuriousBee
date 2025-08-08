# Collect all source files in server/src/
file(GLOB SERVER_SOURCES src/*.cpp)

# Build the library named "server"
add_library(server STATIC ${SERVER_SOURCES})

# Make headers available to anything that links this
target_include_directories(server
    PUBLIC ${CMAKE_SOURCE_DIR}/include
)

target_link_libraries(server PRIVATE network)
