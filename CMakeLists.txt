
cmake_minimum_required(VERSION 3.24)

project(all-cpp-websocket-client-server)

# message("CMAKE_CXX_FLAGS_DEBUG 1: ${CMAKE_CXX_FLAGS_DEBUG}")

#
#
#

if (NOT DEFINED EMSCRIPTEN)

    find_program(CLANG_TIDY_EXE "clang-tidy")

    if (CLANG_TIDY_EXE)

        # file generated at ./build.release.native/compile_commands.json

        message(STATUS "clang-tidy found: ${CLANG_TIDY_EXE}")
        set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
        set(CMAKE_CXX_CLANG_TIDY ${CLANG_TIDY_EXE})

    else()

        message(WARNING "clang-tidy not found!")

    endif()

endif()

#
#
#

if (DEFINED EMSCRIPTEN)

    execute_process (COMMAND sh ./scripts/sh_ensure_wasm_compiler.sh WORKING_DIRECTORY .)

endif()

add_subdirectory(./network-wrapper)
add_subdirectory(./samples/basic-client-server)
add_subdirectory(./tests)
