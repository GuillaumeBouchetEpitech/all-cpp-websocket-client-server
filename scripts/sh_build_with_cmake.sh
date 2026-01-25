#!/bin/bash


clear


INITIAL_CWD="$PWD"

#
#

echo ""
echo "#"
echo "# WEB_WASM BUILD"
echo "#"
echo ""

emcmake cmake -B ./build.release.wasm
cd ./build.release.wasm || exit 1
cmake --build . --config Release --parallel 5 # --target install
# cmake --install . --prefix "$INITIAL_CWD/target"

#
#

# reset the cwd
cd "$INITIAL_CWD" || exit 1

#
#

echo ""
echo "#"
echo "# NATIVE BUILD"
echo "#"
echo ""

cmake -B ./build.release.native
cd ./build.release.native || exit 1
cmake --build . --config Release --parallel 5

#
#

# reset the cwd
cd "$INITIAL_CWD" || exit 1

#
#

echo ""
echo "#"
echo "# PRINT"
echo "#"
echo ""

tree -Dh ./network-wrapper/lib
tree -Dh ./tests/bin
tree -Dh ./samples/basic-client-server/server-side/bin
tree -Dh ./samples/basic-client-server/client-side/bin
tree -Dh ./samples/basic-client-server/client-side/dist

echo ""
echo "#"
echo "# DONE!"
echo "#"
echo ""

