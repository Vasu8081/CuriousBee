# Collect all source files in parser/src/
file(GLOB PARSER_SOURCES src/*.cpp)

add_executable(parser src/parser.cpp ${PARSER_SOURCES})
target_link_libraries(parser PRIVATE server)
