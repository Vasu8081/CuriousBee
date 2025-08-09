file(GLOB SOURCES src/*.c++)

add_library(messages STATIC ${SOURCES})

target_include_directories(messages PUBLIC ${CMAKE_SOURCE_DIR}/include)

target_link_libraries(messages PUBLIC capnp kj capnp-rpc)
