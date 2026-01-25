
cmake_minimum_required(VERSION 3.24)

project(client)

add_executable(${PROJECT_NAME}
    ./src/application/Application.cpp
    ./src/application/main.cpp
    ./src/application/utilities/TraceLogger.cpp
)

target_include_directories(${PROJECT_NAME} PRIVATE ../../../network-wrapper/src/)

target_link_libraries(${PROJECT_NAME} PRIVATE network-wrapper)

if (DEFINED EMSCRIPTEN)

    # emscripten will generate both .js and .wasm files
	set(CMAKE_EXECUTABLE_SUFFIX ".js")

	set_target_properties(${PROJECT_NAME} PROPERTIES COMPILE_FLAGS "-O3 -s SIDE_MODULE=1 ")

    set(EM_LDFLAGS "")
    set(EM_LDFLAGS "${EM_LDFLAGS} -O3")
    set(EM_LDFLAGS "${EM_LDFLAGS} -s WASM=1")
    set(EM_LDFLAGS "${EM_LDFLAGS} -s TOTAL_MEMORY=128Mb")
    set(EM_LDFLAGS "${EM_LDFLAGS} -lwebsocket.js")
    set(EM_LDFLAGS "${EM_LDFLAGS} -s BINARYEN_IGNORE_IMPLICIT_TRAPS=1")
    set(EM_LDFLAGS "${EM_LDFLAGS} -s EXPORTED_FUNCTIONS=\"['_free']\"")
    set(EM_LDFLAGS "${EM_LDFLAGS} -s EXPORTED_RUNTIME_METHODS=\"['cwrap','stringToUTF8','lengthBytesUTF8','UTF8ToString','stringToNewUTF8']\"")
    set(EM_LDFLAGS "${EM_LDFLAGS} -s MODULARIZE=1")
    set(EM_LDFLAGS "${EM_LDFLAGS} -s EXPORT_NAME=\"clientWeb\"")
    set_target_properties(${PROJECT_NAME} PROPERTIES LINK_FLAGS ${EM_LDFLAGS})

    set_target_properties(${PROJECT_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/dist/wasm")

    # ensure the web-wasm-loader page/script
    execute_process(COMMAND sh ./scripts/sh_ensure_web_wasm_loader.sh
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        RESULT_VARIABLE ENSURE_WEB_WASM_LOADER_RESULT)

    if(NOT ENSURE_WEB_WASM_LOADER_RESULT EQUAL "0")
        message(FATAL_ERROR "sh ./scripts/sh_ensure_web_wasm_loader.sh failed with ${ENSURE_WEB_WASM_LOADER_RESULT}")
    endif()

else()

    set_target_properties(${PROJECT_NAME} PROPERTIES COMPILE_FLAGS "-O3")

    set_target_properties(${PROJECT_NAME} PROPERTIES LINK_FLAGS "-O3")

    set_target_properties(${PROJECT_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/bin")

endif()






