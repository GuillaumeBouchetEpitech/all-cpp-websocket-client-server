
# clang-tidy \
#   ./client-web/src/*.cpp \
#   -extra-arg=-std=c++17 \
#   -- \
#   -I./client-web/src/ \
#   -I${EMSDK}/upstream/emscripten/system/include/ \
#   ...


# clang-tidy \
#   ./client-web/src/*.cpp \
#   -extra-arg=-std=c++17 \
#   -- \
#   -I./client-web/src/ \
#   -I${EMSDK}/upstream/emscripten/cache/sysroot/include/ \
#   ...



clang-tidy \
  ./server-side/src/*.cpp \
  -extra-arg=-std=c++17 \
  -- \
  -I./server-side/src/ \
  ...

