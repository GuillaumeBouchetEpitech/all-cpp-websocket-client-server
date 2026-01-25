
cmake_minimum_required(VERSION 3.24)

if (DEFINED EMSCRIPTEN)

    message("web-wasm build will be ignored for basic-client-server--server-side")

else()

    project(server)

    add_executable(${PROJECT_NAME}
        ./src/application/main.cpp
        ./src/application/https-server/file-manager/FileManager.cpp
        ./src/application/https-server/HttpFileServer.cpp
        ./src/application/websocket-server/session-manager/AbstractWebSocketSessionManager.cpp
        ./src/application/websocket-server/session-manager/internals/ThreadSafeWebSocketSessionManager.cpp
        ./src/application/websocket-server/session-manager/internals/ThreadUnsafeWebSocketSessionManager.cpp
        ./src/application/websocket-server/WebSocketMainLogicServer.cpp
    )

    target_include_directories(${PROJECT_NAME} PRIVATE ../../../network-wrapper/src/)

    target_link_libraries(${PROJECT_NAME} PRIVATE network-wrapper)

    # the server also need zlib
    target_link_libraries(${PROJECT_NAME} PRIVATE -lz)

    set_target_properties(${PROJECT_NAME} PROPERTIES COMPILE_FLAGS "-O3")

    set_target_properties(${PROJECT_NAME} PROPERTIES LINK_FLAGS "-O3")

    set_target_properties(${PROJECT_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/bin")

    execute_process (COMMAND sh ./scripts/sh_refresh_file_manager_assets_folder.sh WORKING_DIRECTORY ${PROJECT_SOURCE_DIR})

endif()
