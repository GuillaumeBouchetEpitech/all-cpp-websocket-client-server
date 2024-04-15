
# clang-tidy \
#   ./client-side/src/*.cpp \
#   -extra-arg=-std=c++17 \
#   -- \
#   -I./client-side/src/ \
#   -I${EMSDK}/upstream/emscripten/system/include/ \
#   ...


# clang-tidy \
#   ./client-side/src/*.cpp \
#   -extra-arg=-std=c++17 \
#   -- \
#   -I./client-side/src/ \
#   -I${EMSDK}/upstream/emscripten/cache/sysroot/include/ \
#   ...



clang-tidy \
  ./server-side/src/application/*.cpp \
  -extra-arg=-std=c++17 \
  -- \
  -I./server-side/src/ \
  ...


clang-tidy \
  ./server-side/src/network-wrapper/http-server/*.cpp \
  -extra-arg=-std=c++17 \
  -- \
  -I./server-side/src/ \
  ...


clang-tidy \
  ./server-side/src/network-wrapper/websocket-server/*.cpp \
  -extra-arg=-std=c++17 \
  -- \
  -I./server-side/src/ \
  ...

