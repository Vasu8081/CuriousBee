file(GLOB VIDEOS_SRC "src/*.cpp")
list(FILTER VIDEOS_SRC EXCLUDE REGEX ".*/videod\\.cpp$")

add_executable(videosd src/videod.cpp ${VIDEOS_SRC})

target_include_directories(videosd
  PRIVATE
    ${CMAKE_SOURCE_DIR}/include
)

target_link_libraries(videosd
  PRIVATE
    server
    network
)