file(GLOB_RECURSE SOURCES src/*.cpp)

add_library(database STATIC ${SOURCES})

target_include_directories(database
    PUBLIC
        ${CMAKE_CURRENT_BINARY_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(database
    PRIVATE odb odb-pgsql pq
)