
cmake_minimum_required(VERSION 3.24)

project(unit-tests)

if (NOT DEFINED EMSCRIPTEN)

    add_executable(${PROJECT_NAME}
        ./src/websockets-session/test_web_socket_session.cpp
    )

    target_include_directories(${PROJECT_NAME} PRIVATE ../network-wrapper/src/)

    target_link_libraries(${PROJECT_NAME} PRIVATE network-wrapper gtest gtest_main)

    set_target_properties(${PROJECT_NAME} PROPERTIES COMPILE_FLAGS "-O3")
    set_target_properties(${PROJECT_NAME} PROPERTIES LINK_FLAGS "-O3 -pthread")

    set_target_properties(${PROJECT_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/bin")

endif()

