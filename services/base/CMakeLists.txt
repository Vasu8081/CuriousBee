file(GLOB_RECURSE SOURCES src/*.cpp)

add_library(base STATIC ${SOURCES})

target_include_directories(base
    PUBLIC
        ${CMAKE_CURRENT_BINARY_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(base
    PRIVATE spdlog fmt::fmt
    PUBLIC nlohmann_json::nlohmann_json zmq
)