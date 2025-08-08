file(GLOB SOURCES src/*.cpp)

add_executable(networkparser src/parser.cpp ${SOURCES})
target_link_libraries(networkparser PRIVATE base)