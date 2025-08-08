file(GLOB SOURCES src/*.cpp)

add_executable(dbparser src/parser.cpp ${SOURCES})
target_link_libraries(dbparser PRIVATE base)