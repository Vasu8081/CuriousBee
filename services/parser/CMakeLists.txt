# Collect all source files in parser/src/
file(GLOB PARSER_SOURCES src/*.cpp)

add_executable(parser src/parser.cpp ${PARSER_SOURCES})
target_link_libraries(parser PRIVATE server)

# === Step 0: Configurable option to skip DSL/gen ===
option(SKIP_SCHEMA_GENERATION "Skip generating .capnp from DSL schema" OFF)

if(SKIP_SCHEMA_GENERATION)
    message(STATUS "Cap'n Proto schema generation is disabled (SKIP_SCHEMA_GENERATION=ON)")
else()
    message(STATUS "Cap'n Proto schema will be generated from DSL")
endif()

# === Step 1: Collect all dependencies for schema generation ===
set(SCHEMA_INPUT_FILE ${CMAKE_SOURCE_DIR}/network/schema/application.schema)

# Collect all parser source files as dependencies
file(GLOB_RECURSE PARSER_HEADER_FILES 
    ${CMAKE_CURRENT_SOURCE_DIR}/include/*.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/*.hpp
)

file(GLOB_RECURSE PARSER_SOURCE_FILES 
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.c
)

# Define all generated files
set(GENERATED_CAPNP ${CMAKE_CURRENT_SOURCE_DIR}/src/network_msg.capnp)
set(GENERATED_MESSAGE_TYPE ${CMAKE_SOURCE_DIR}/include/network/message_type.h)
set(GENERATED_FACTORY_BUILDER ${CMAKE_SOURCE_DIR}/include/network/factory_builder.h)

# Collect all generated header files
file(GLOB_RECURSE GENERATED_HEADERS 
    ${CMAKE_SOURCE_DIR}/include/network/*.h
)

# Collect all generated implementation files
file(GLOB_RECURSE GENERATED_IMPLEMENTATIONS 
    ${CMAKE_SOURCE_DIR}/src/network/*.cpp
)

# === Step 2: Define schema generation target ===
if(NOT SKIP_SCHEMA_GENERATION)
    # Create a custom target that depends on all the generated files
    add_custom_command(
        OUTPUT 
            ${GENERATED_CAPNP}
            ${GENERATED_MESSAGE_TYPE}
            ${GENERATED_FACTORY_BUILDER}
        COMMAND 
            ${CMAKE_COMMAND} -E echo "Generating schema files from ${SCHEMA_INPUT_FILE}"
        COMMAND 
            parser ${SCHEMA_INPUT_FILE} ${CMAKE_SOURCE_DIR}
        COMMAND
            ${CMAKE_COMMAND} -E touch ${GENERATED_CAPNP}
        COMMAND
            ${CMAKE_COMMAND} -E touch ${GENERATED_MESSAGE_TYPE}
        COMMAND
            ${CMAKE_COMMAND} -E touch ${GENERATED_FACTORY_BUILDER}
        DEPENDS 
            parser
            ${SCHEMA_INPUT_FILE}
            ${PARSER_HEADER_FILES}
            ${PARSER_SOURCE_FILES}
        COMMENT "Generating Cap'n Proto schema and C++ files from DSL"
        VERBATIM
    )

    # Create a target for generated headers (useful for dependencies)
    add_custom_target(generate_schema_headers
        DEPENDS 
            ${GENERATED_CAPNP}
            ${GENERATED_MESSAGE_TYPE}
            ${GENERATED_FACTORY_BUILDER}
    )

    # Create a target that regenerates all implementation files if they're missing
    add_custom_command(
        OUTPUT regenerate_implementations_stamp
        COMMAND 
            ${CMAKE_COMMAND} -E echo "Checking and regenerating implementation files if needed"
        COMMAND 
            parser ${SCHEMA_INPUT_FILE} ${CMAKE_SOURCE_DIR}
        COMMAND
            ${CMAKE_COMMAND} -E touch regenerate_implementations_stamp
        DEPENDS 
            generate_schema_headers
            ${SCHEMA_INPUT_FILE}
        COMMENT "Regenerating implementation files if needed"
        VERBATIM
    )

    add_custom_target(generate_implementations
        DEPENDS regenerate_implementations_stamp
    )

    # Optional: Create a target to force regeneration
    add_custom_target(force_regenerate_schema
        COMMAND 
            ${CMAKE_COMMAND} -E remove -f ${GENERATED_CAPNP}
        COMMAND 
            ${CMAKE_COMMAND} -E remove -f ${GENERATED_MESSAGE_TYPE}
        COMMAND 
            ${CMAKE_COMMAND} -E remove -f ${GENERATED_FACTORY_BUILDER}
        COMMAND 
            ${CMAKE_COMMAND} -E remove -f regenerate_implementations_stamp
        COMMAND 
            ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR} --target generate_implementations
        COMMENT "Force regenerating all schema files"
        VERBATIM
    )

    # Optional: Create a target to clean generated files
    add_custom_target(clean_generated_schema
        COMMAND 
            ${CMAKE_COMMAND} -E remove -f ${GENERATED_CAPNP}
        COMMAND 
            ${CMAKE_COMMAND} -E remove -f ${GENERATED_MESSAGE_TYPE}
        COMMAND 
            ${CMAKE_COMMAND} -E remove -f ${GENERATED_FACTORY_BUILDER}
        COMMAND
            ${CMAKE_COMMAND} -E remove -f regenerate_implementations_stamp
        COMMAND 
            ${CMAKE_COMMAND} -E remove -f ${CMAKE_SOURCE_DIR}/src/network/*.cpp
        COMMAND 
            ${CMAKE_COMMAND} -E remove -f ${CMAKE_SOURCE_DIR}/include/network/*.h
        COMMENT "Cleaning all generated schema files"
        VERBATIM
    )

else()
    # When schema generation is skipped, create dummy targets
    add_custom_target(generate_schema_headers)
    add_custom_target(generate_implementations)
    add_custom_target(force_regenerate_schema
        COMMAND ${CMAKE_COMMAND} -E echo "Schema generation is disabled"
    )
    add_custom_target(clean_generated_schema
        COMMAND ${CMAKE_COMMAND} -E echo "Schema generation is disabled"
    )
endif()

# === Step 3: Advanced dependency tracking ===
if(NOT SKIP_SCHEMA_GENERATION)
    # Create a function to check if regeneration is needed
    function(check_schema_dependencies)
        # Check if the schema file is newer than generated files
        if(EXISTS ${SCHEMA_INPUT_FILE})
            file(TIMESTAMP ${SCHEMA_INPUT_FILE} SCHEMA_TIME)
            
            set(NEEDS_REGEN FALSE)
            
            # Check each generated file
            foreach(GENERATED_FILE ${GENERATED_CAPNP} ${GENERATED_MESSAGE_TYPE} ${GENERATED_FACTORY_BUILDER})
                if(NOT EXISTS ${GENERATED_FILE})
                    set(NEEDS_REGEN TRUE)
                    break()
                else()
                    file(TIMESTAMP ${GENERATED_FILE} GENERATED_TIME)
                    if(${SCHEMA_TIME} STREQUAL "" OR ${GENERATED_TIME} STREQUAL "" OR 
                       ${SCHEMA_TIME} GREATER ${GENERATED_TIME})
                        set(NEEDS_REGEN TRUE)
                        break()
                    endif()
                endif()
            endforeach()
            
            # Check parser executable timestamp
            if(TARGET parser AND NOT NEEDS_REGEN)
                get_target_property(PARSER_LOCATION parser LOCATION)
                if(EXISTS ${PARSER_LOCATION})
                    file(TIMESTAMP ${PARSER_LOCATION} PARSER_TIME)
                    if(${PARSER_TIME} GREATER ${GENERATED_TIME})
                        set(NEEDS_REGEN TRUE)
                    endif()
                endif()
            endif()
            
            if(NEEDS_REGEN)
                message(STATUS "Schema regeneration needed - dependencies have changed")
            else()
                message(STATUS "Schema files are up to date")
            endif()
        endif()
    endfunction()
    
    # Call the dependency check
    check_schema_dependencies()
endif()

# === Step 4: Integration with build system ===
if(NOT SKIP_SCHEMA_GENERATION)
    # Make sure schema generation happens before any targets that might need it
    add_dependencies(generate_implementations generate_schema_headers)
    
    # If you have other targets that depend on generated files, add dependencies like:
    # add_dependencies(your_target_name generate_implementations)
endif()