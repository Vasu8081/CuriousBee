# Check if network_msg.capnp exists
set(CAPNP_SCHEMA ${CMAKE_CURRENT_SOURCE_DIR}/src/network_msg.capnp)

if(EXISTS ${CAPNP_SCHEMA})
    # Find the Cap'n Proto compiler
    find_program(CAPNP_COMPILER capnp REQUIRED)
    
    # Generate output file paths
    get_filename_component(FILE_NAME ${CAPNP_SCHEMA} NAME_WE)
    set(GEN_CPP ${CMAKE_CURRENT_BINARY_DIR}/${FILE_NAME}.capnp.c++)
    set(GEN_H   ${CMAKE_CURRENT_BINARY_DIR}/${FILE_NAME}.capnp.h)
    
    # Compile .capnp -> .capnp.c++/.capnp.h (only when schema changes)
    add_custom_command(
        OUTPUT ${GEN_CPP} ${GEN_H}
        COMMAND ${CAPNP_COMPILER} compile
            -oc++
            --src-prefix=${CMAKE_CURRENT_SOURCE_DIR}/src
            -I ${CMAKE_CURRENT_SOURCE_DIR}/src
            ${CAPNP_SCHEMA}
        DEPENDS ${CAPNP_SCHEMA}
        COMMENT "Compiling Cap'n Proto schema (changed): ${CAPNP_SCHEMA}"
        VERBATIM
    )
    
    # Create the messages static library
    add_library(messages STATIC ${GEN_CPP})
    target_include_directories(messages PUBLIC ${CMAKE_CURRENT_BINARY_DIR})
    target_link_libraries(messages PUBLIC capnp kj capnp-rpc)
    
    message(STATUS "Cap'n Proto schema found and will be compiled")
else()
    message(STATUS "Cap'n Proto schema not found, skipping compilation")
endif()