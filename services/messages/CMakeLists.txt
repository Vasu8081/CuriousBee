# === Step 0: Configurable option to skip DSL/gen ===
option(SKIP_SCHEMA_GENERATION "Skip generating .capnp from DSL schema" ON)

if(SKIP_SCHEMA_GENERATION)
    message(STATUS "Cap'n Proto schema generation is disabled (SKIP_SCHEMA_GENERATION=ON)")
else()
    message(STATUS "Cap'n Proto schema will be generated from DSL")
endif()

# === Step 1: Build the DSL Generator ===
add_executable(dslgen dslgen/dslgen.cpp)
target_include_directories(dslgen PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/dslgen)

# === Step 2: Define schema generation target ===
set(GENERATED_CAPNP ${CMAKE_CURRENT_SOURCE_DIR}/src/network_msg.capnp)

if(NOT SKIP_SCHEMA_GENERATION)
    add_custom_command(
        OUTPUT ${GENERATED_CAPNP}
        COMMAND dslgen ${CMAKE_SOURCE_DIR}/network/schema/application.schema ${CMAKE_SOURCE_DIR}
        DEPENDS dslgen ${CMAKE_SOURCE_DIR}/network/schema/application.schema
        COMMENT "Generating Cap'n Proto schema file from DSL"
    )
endif()

add_custom_target(generate_schema DEPENDS ${GENERATED_CAPNP})

# === Step 3: Compile .capnp -> .capnp.c++/.capnp.h ===
find_program(CAPNP_COMPILER capnp REQUIRED)

get_filename_component(FILE_NAME ${GENERATED_CAPNP} NAME_WE)
set(GEN_CPP ${CMAKE_CURRENT_BINARY_DIR}/${FILE_NAME}.capnp.c++)
set(GEN_H   ${CMAKE_CURRENT_BINARY_DIR}/${FILE_NAME}.capnp.h)

add_custom_command(
    OUTPUT ${GEN_CPP} ${GEN_H}
    COMMAND ${CAPNP_COMPILER} compile
        -oc++
        --src-prefix=${CMAKE_CURRENT_SOURCE_DIR}/src
        -I ${CMAKE_CURRENT_SOURCE_DIR}/src
        ${GENERATED_CAPNP}
    DEPENDS ${GENERATED_CAPNP} generate_schema
    COMMENT "Compiling Cap'n Proto: ${GENERATED_CAPNP}"
    VERBATIM
)

# === Step 4: Create the messages static library ===
add_library(messages STATIC ${GEN_CPP})
add_dependencies(messages generate_schema)

target_include_directories(messages PUBLIC ${CMAKE_CURRENT_BINARY_DIR})
target_link_libraries(messages PUBLIC capnp kj capnp-rpc)
