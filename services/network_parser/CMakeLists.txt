file(GLOB SOURCES src/*.cpp)

add_executable(networkparser src/parser.cpp ${SOURCES})
target_link_libraries(networkparser PRIVATE base)

option(SKIP_SCHEMA_GENERATION "Skip generating .capnp from DSL schema" ON)

set(SCHEMA_INPUT_FILE ${CMAKE_SOURCE_DIR}/messages/schema/network.schema)
set(GENERATED_CAPNP ${CMAKE_CURRENT_SOURCE_DIR}/src/network_msg.capnp)
set(GENERATED_MESSAGE_TYPE ${CMAKE_SOURCE_DIR}/include/network/message_type.h)
set(GENERATED_FACTORY_BUILDER ${CMAKE_SOURCE_DIR}/include/network/factory_builder.h)

if(NOT SKIP_SCHEMA_GENERATION)
    add_custom_command(
        OUTPUT ${GENERATED_CAPNP} ${GENERATED_MESSAGE_TYPE} ${GENERATED_FACTORY_BUILDER}
        COMMAND ${CMAKE_COMMAND} -E echo "Generating schema files from ${SCHEMA_INPUT_FILE}"
        COMMAND networkparser ${SCHEMA_INPUT_FILE} ${CMAKE_SOURCE_DIR}
        DEPENDS networkparser ${SCHEMA_INPUT_FILE}
        COMMENT "Generating Cap'n Proto schema and C++ files from DSL"
    )

    add_custom_target(generate_schema_files
        DEPENDS ${GENERATED_CAPNP} ${GENERATED_MESSAGE_TYPE} ${GENERATED_FACTORY_BUILDER}
    )
else()
    add_custom_target(generate_schema_files
        COMMAND ${CMAKE_COMMAND} -E echo "Schema generation is disabled"
    )
endif()

# Make sure other targets depend on generated files (example)
# add_dependencies(your_target generate_schema_files)