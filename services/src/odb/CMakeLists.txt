file(GLOB SOURCES src/*.cxx)

add_library(newodb STATIC ${SOURCES})

target_include_directories(newodb
    PUBLIC ${CMAKE_SOURCE_DIR}/include
)

target_link_libraries(newodb PRIVATE odb odb-pgsql pq odb-boost)

target_compile_features(newodb PUBLIC cxx_std_17)
set_target_properties(newodb PROPERTIES POSITION_INDEPENDENT_CODE ON)