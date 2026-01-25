#!/bin/bash

analyze_one_file () {
    echo " -> analyzing file: \"$1\""
    clang-tidy -header-filter=.* -p ./build.release.native -extra-arg=-std=c++20 "$1"
}
export -f analyze_one_file

find ./network-wrapper/src -not -path "./network-wrapper/src/network-wrapper/websocket-client/wasm/*" \
    \( -iname "*.h" -o -iname "*.hpp" -o -iname "*.cpp" \) -exec bash -c 'analyze_one_file "$0"' {} \;
find ./samples/basic-client-server/server-side/src \
    \( -iname "*.h" -o -iname "*.hpp" -o -iname "*.cpp" \) -exec bash -c 'analyze_one_file "$0"' {} \;
find ./samples/basic-client-server/client-side/src \
    \( -iname "*.h" -o -iname "*.hpp" -o -iname "*.cpp" \) -exec bash -c 'analyze_one_file "$0"' {} \;
