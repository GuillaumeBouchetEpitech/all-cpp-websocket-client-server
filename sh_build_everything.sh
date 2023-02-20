#!/bin/bash

CURRENT_DIR=$PWD

#
#
#
#
#

echo ""
echo "#################################################"
echo "#                                               #"
echo "# IF THIS SCRIPT FAIL -> TRY THOSE TWO COMMANDS #"
echo "# -> 'chmod +x ./sh_everything.sh'              #"
echo "# -> './sh_everything.sh'                       #"
echo "#                                               #"
echo "#################################################"
echo ""

#
#
#
#
#

if [ -z "${EMSDK}" ]; then
  echo "the env var 'EMSDK' is missing, the web-wasm builds will be skipped"
  echo " => check the readme if you want to install emscripten"
  echo " => if emscripten is laready installed, you may just need to run '. ./emsdk_env.sh' in this terminal"
  exit 1
else
  echo "the env var 'EMSDK' was found"
fi

#
#
#
#
#

echo ""
echo "#"
echo "# building client-web (C++ -> WebAssembly)"
echo "#"
echo ""

cd ./client-web
make build_mode="release" all -j4
cd $CURRENT_DIR

echo ""
echo "#"
echo "# building web-wasm-loader (TypeScript -> JavaScript)"
echo "#"
echo ""

cd ./client-web/web-wasm-loader
npm install
npm run build
cd $CURRENT_DIR

echo ""
echo "#"
echo "# building server-side (C++ -> binary)"
echo "#"
echo ""

echo "building server-side"
cd ./server-side
make build_mode="release" all -j4

echo ""
echo "#"
echo "# updating file server asset folder with latest client build"
echo "#"
echo ""

sh sh_refresh_file_manager_assets_folder.sh

echo ""
echo "#"
echo "# starting server side (localhost)"
echo "#"
echo ""

./bin/exec 127.0.0.1 7777 8888 1

cd $CURRENT_DIR

#
#
#
#
#


