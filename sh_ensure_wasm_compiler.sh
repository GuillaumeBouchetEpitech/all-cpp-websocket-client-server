#!/bin/bash

clear

CURRENT_DIR=$PWD

#
#
#
#
#

DIR_THIRDPARTIES=$PWD/thirdparties
DIR_DEPENDENCIES=$DIR_THIRDPARTIES/dependencies

mkdir -p "$DIR_DEPENDENCIES"

#
#
#
#
#

echo ""
echo "###"
echo "###"
echo "### ensuring the 'C++ to WebAssembly compiler (emsdk)' is installed"
echo "###"
echo "###"
echo ""

EMSDK_VERSION=3.1.74

if [ -z "${EMSDK}" ]; then

  echo " -> not installed"
  echo "   -> installing"

  echo "the env var 'EMSDK' is missing, the web-wasm builds will be skipped"
  echo " => check the readme if you want to install emscripten"
  echo " => it emscripten is laready installed, you may just need to run '. ./emsdk_env.sh' in this terminal"


  # will not do anything if already present
  sh sh_install_one_git_thirdparty.sh \
    "$DIR_DEPENDENCIES" \
    "EMSDK" \
    "emsdk" \
    "emscripten-core/emsdk" \
    $EMSDK_VERSION \
    "not-interactive"

  # goto local install emscripten folder
  cd "$DIR_DEPENDENCIES/emsdk" || exit 1

else

  echo " -> already installed"

  # goto local install emscripten folder
  cd "$EMSDK" || exit 1
fi

echo " -> ensuring the correct version is installed"

./emsdk install $EMSDK_VERSION || exit 1

echo " -> activating the correct version"

./emsdk activate --embedded $EMSDK_VERSION || exit 1

. "./emsdk_env.sh" || exit 1

# sometimes required? (suspected hiccups in emsdk)
# em++ --clear-cache

cd "$CURRENT_DIR" || exit 1

echo " -> success, the 'C++ to WebAssembly compiler (emsdk)' is ready"
