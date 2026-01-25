cmake_minimum_required(VERSION 3.24)
project(network-wrapper)

set(CMAKE_CXX_STANDARD 20)

if (NOT DEFINED EMSCRIPTEN)

    set(SOURCE_FILES
        ./src/network-wrapper/http-server/AbstractHttpServer.cpp
        ./src/network-wrapper/http-server/internals/HttpConnection.cpp
        ./src/network-wrapper/http-server/internals/HttpServer.cpp
        ./src/network-wrapper/common/AbstractTcpListener.cpp
        ./src/network-wrapper/common/internals/TcpListener.cpp
        ./src/network-wrapper/websocket-server/AbstractWebSocketServer.cpp
        ./src/network-wrapper/websocket-server/internals/WebSocketServer.cpp
        ./src/network-wrapper/websocket-server/internals/WebSocketSession.cpp
        ./src/network-wrapper/websocket-client/AbstractWebSocketConnection.cpp
        ./src/network-wrapper/websocket-client/native/internals/WebSocketConnection.cpp
        ./src/network-wrapper/websocket-client/native/internals/worker-thread/AbstractWorkerThread.cpp
        ./src/network-wrapper/websocket-client/native/internals/worker-thread/internals/WorkerThread.cpp
        ./src/network-wrapper/websocket-client/native/internals/worker-thread/internals/ThreadSynchronizer.cpp
        ./src/network-wrapper/websocket-client/native/createWebSocketConnection.cpp
        ./src/network-wrapper/utilities/TestAndSetAtomicLock.cpp
    )

else()

    # web-wasm build only uses the websocket-client
    set(SOURCE_FILES
        ./src/network-wrapper/websocket-client/AbstractWebSocketConnection.cpp
        ./src/network-wrapper/websocket-client/wasm/internals/WebSocketConnection.cpp
        ./src/network-wrapper/websocket-client/wasm/createWebSocketConnection.cpp
    )

endif()

add_library(${PROJECT_NAME} ${SOURCE_FILES})

target_include_directories(${PROJECT_NAME} PRIVATE ./src)

if (DEFINED EMSCRIPTEN)

    set_target_properties(${PROJECT_NAME} PROPERTIES COMPILE_FLAGS "-O3")

    set_target_properties(${PROJECT_NAME} PROPERTIES LINK_FLAGS " -O3 -s WASM=1 -s BINARYEN_IGNORE_IMPLICIT_TRAPS=1 ")

    set_target_properties(${PROJECT_NAME} PROPERTIES ARCHIVE_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/lib/wasm")

else()

    set_target_properties(${PROJECT_NAME} PROPERTIES COMPILE_FLAGS "-O3")

    set_target_properties(${PROJECT_NAME} PROPERTIES LINK_FLAGS "-O3")

    set_target_properties(${PROJECT_NAME} PROPERTIES ARCHIVE_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/lib/native")

endif()
