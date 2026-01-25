
cmake_minimum_required(VERSION 3.24)

project(all-cpp-websocket-client-server)

# message("CMAKE_CXX_FLAGS_DEBUG 1: ${CMAKE_CXX_FLAGS_DEBUG}")

if (DEFINED EMSCRIPTEN)

    execute_process (COMMAND sh ./scripts/sh_ensure_wasm_compiler.sh WORKING_DIRECTORY .)

endif()

add_subdirectory(./network-wrapper)
add_subdirectory(./samples/basic-client-server)
