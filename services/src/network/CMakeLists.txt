# Collect all source files in network/src/
file(GLOB NETWORK_SOURCES src/*.cpp src/readonly/*.cpp)

# Build the library named "network"
add_library(network STATIC ${NETWORK_SOURCES})

# Make headers available to anything that links this
target_include_directories(network
    PUBLIC ${CMAKE_SOURCE_DIR}/include
)

target_link_libraries(network PRIVATE messages base)
