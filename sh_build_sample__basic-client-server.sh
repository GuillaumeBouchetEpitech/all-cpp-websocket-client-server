#!/bin/bash

CURRENT_DIR=$PWD

#
#
#
#
#


echo ""
echo "#"
echo "# building network-wrapper (C++ -> WebAssembly)"
echo "#"
echo ""

cd $CURRENT_DIR
# done to avoid the slow wasm build process that happen even if nothing changed
RETVAL=$(sh sh_check_if_up_to_date.sh  ./network-wrapper/src  ./network-wrapper/lib/wasm)
if [ "$RETVAL" != "latest" ]
then

  echo "network-wrapper is not built or not up to date with the latest source code"

  cd ./network-wrapper
  make build_platform="web-wasm" build_mode="release" all -j4
  cd $CURRENT_DIR

else
  echo "network-wrapper is built and up to date with the latest source code"
fi

# exit 0

#
#
#
#
#

echo ""
echo "#"
echo "# building client-side (C++ -> Native)"
echo "#"
echo ""

# done to avoid the slow wasm build process that happen even if nothing changed
RETVAL=$(sh sh_check_if_up_to_date.sh  ./samples/basic-client-server/client-side/src  ./samples/basic-client-server/client-side/bin)
if [ "$RETVAL" != "latest" ]
then

  echo "client-side is not built or not up to date with the latest source code"

  cd ./samples/basic-client-server/client-side
  make build_platform="native" build_mode="release" all -j4
  cd $CURRENT_DIR

else
  echo "client-side is built and up to date with the latest source code"
fi

# exit 0

echo ""
echo "#"
echo "# building client-side (C++ -> WebAssembly)"
echo "#"
echo ""

# done to avoid the slow wasm build process that happen even if nothing changed
RETVAL=$(sh sh_check_if_up_to_date.sh  ./samples/basic-client-server/client-side/src  ./samples/basic-client-server/client-side/dist/wasm)
if [ "$RETVAL" != "latest" ]
then

  echo "client-side is not built or not up to date with the latest source code"

  cd ./samples/basic-client-server/client-side
  make build_platform="web-wasm" build_mode="release" all -j4
  cd $CURRENT_DIR

else
  echo "client-side is built and up to date with the latest source code"
fi

# exit 0

echo ""
echo "#"
echo "# building web-wasm-loader (TypeScript -> JavaScript)"
echo "#"
echo ""

cd ./samples/basic-client-server/client-side/

if [ -f "./dist/index.js" && -f "./dist/index.html" ]
then

  echo " ===> 'dist' folder content up to date"
  echo " =====> skipping further checks"

else

  echo " ===> 'dist' folder content is missing"
  echo " =====> checking"

  cd ./web-wasm-loader

  if [ -d "./node_modules" ]
  then
    echo " =====> up to date dependencies"
    echo " =======> skip install"
  else
    echo " =====> missing dependencies"
    echo " =======> installing"

    npm install
  fi

  if [ -f "./js/bundle.js" ]
  then
    echo " =====> up to date bundle.js"
    echo " =======> skip bundling"
  else
    echo " =====> outdated bundle.js"
    echo " =======> bundling"

    npm run release
  fi

  echo " ===> ensure 'dist' folder exist"

  npm run ensure-dist-folders

  echo " ===> copying html to 'dist' folder"

  npm run copy-html-to-dist-folders

  echo " ===> copying js to 'dist' folder"

  npm run copy-js-to-dist-folders

fi

cd $CURRENT_DIR

echo ""
echo "#"
echo "# building server-side (C++ -> binary)"
echo "#"
echo ""

cd ./samples/basic-client-server/server-side
echo "building server-side - application"
make build_mode="release" application -j4
# make build_mode="debug" application -j4

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

./bin/exec 127.0.0.1 7777 8888 1 1
# gdb ./bin/exec 127.0.0.1 7777 8888 1 1

cd $CURRENT_DIR

#
#
#
#
#

