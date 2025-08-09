# add_executable(publisher_server youtube_publisher.cpp)
# target_link_libraries(publisher_server PRIVATE server)

# add_executable(subscriber_server youtube_subscriber.cpp)
# target_link_libraries(subscriber_server PRIVATE server)

# add_executable(requester_server youtube_request.cpp)
# target_link_libraries(requester_server PRIVATE server)

# add_executable(responder_server youtube_response.cpp)
# target_link_libraries(responder_server PRIVATE server)

# add_executable(hyrid_server youtube_hybrid.cpp)
# target_link_libraries(hyrid_server PRIVATE server)

add_executable(db_test db_test.cpp)
target_link_libraries(db_test PUBLIC database newodb)