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


#
#
#
#
#

DIR_THIRDPARTIES=$PWD/thirdparties
DIR_DEPENDENCIES=$DIR_THIRDPARTIES/dependencies

mkdir -p $DIR_DEPENDENCIES

#
#
#
#
#


func_ensure_wasm_compiler() {

  echo ""
  echo "###"
  echo "###"
  echo "### ensuring the cpp to wasm compiler (emsdk) is installed"
  echo "###"
  echo "###"
  echo ""

  EMSDK_VERSION=3.1.44

  if [ -z "${EMSDK}" ]; then

    echo " -> not installed"
    echo "   -> installing"

    echo "the env var 'EMSDK' is missing, the web-wasm builds will be skipped"
    echo " => check the readme if you want to install emscripten"
    echo " => it emscripten is laready installed, you may just need to run '. ./emsdk_env.sh' in this terminal"


    # will not do anything if already present
    sh sh_install_one_git_thirdparty.sh \
      $DIR_DEPENDENCIES \
      "EMSDK" \
      "emsdk" \
      "emscripten-core/emsdk" \
      $EMSDK_VERSION \
      "not-interactive"

    # goto local install emscripten folder
    cd $DIR_DEPENDENCIES/emsdk

  else

    echo " -> already installed"

    # goto local install emscripten folder
    cd $EMSDK
  fi

  echo " -> ensuring the correct version is installed"

  ./emsdk install $EMSDK_VERSION

  echo " -> activating the correct version"

  ./emsdk activate --embedded $EMSDK_VERSION

  . ./emsdk_env.sh

  # sometimes required? (suspected hiccups in emsdk)
  # em++ --clear-cache

  cd $CURRENT_DIR

  echo " -> success, C++ to WebAssembly compiler is ready"

}

func_ensure_wasm_compiler

#
#
#
#
#

echo ""
echo "#"
echo "# building network-wrapper (C++ -> Native)"
echo "#"
echo ""

cd ./network-wrapper
make build_platform="native" build_mode="release" all -j4
cd $CURRENT_DIR

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
RETVAL=$(sh sh_check_if_up_to_date.sh  ./client-side/src  ./client-side/bin)
if [ "$RETVAL" != "latest" ]
then

  echo "client-side is not built or not up to date with the latest source code"

  cd ./client-side
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
RETVAL=$(sh sh_check_if_up_to_date.sh  ./client-side/src  ./client-side/dist/wasm)
if [ "$RETVAL" != "latest" ]
then

  echo "client-side is not built or not up to date with the latest source code"

  cd ./client-side
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

cd ./client-side/

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

cd ./server-side
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


