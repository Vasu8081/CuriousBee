add_executable(publisher_test publisher_test.cpp)
target_link_libraries(publisher_test PRIVATE server)

add_executable(subscriber_test subscriber_test.cpp)
target_link_libraries(subscriber_test PRIVATE server)

add_executable(client_test client_test.cpp)
target_link_libraries(client_test PRIVATE server)

add_executable(server_test server_test.cpp)
target_link_libraries(server_test PRIVATE server)

add_executable(hybrid_server_test hybrid_server_test.cpp)
target_link_libraries(hybrid_server_test PRIVATE server)

add_executable(db_test db_test.cpp)
target_link_libraries(db_test PUBLIC database newodb)