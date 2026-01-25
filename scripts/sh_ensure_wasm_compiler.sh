#!/bin/bash

# clear

INITIAL_CWD=$PWD

#
#
#
#
#

DIR_THIRDPARTIES=$PWD/thirdparties
DIR_DEPENDENCIES=$DIR_THIRDPARTIES/dependencies

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

  # echo "the env var 'EMSDK' is missing, the web-wasm builds will be skipped"
  # echo " => check the readme if you want to install emscripten"
  # echo " => if emscripten is already installed, you may just need to run '. ./emsdk_env.sh' in this terminal"

  mkdir -p "$DIR_DEPENDENCIES"

  # will not do anything if already present
  sh ./scripts/sh_install_one_git_thirdparty.sh \
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

# since the ./emsdk command can be a bit chatty
export EMSDK_QUIET=1

./emsdk install $EMSDK_VERSION || exit 1

echo " -> activating the correct version"

./emsdk activate --embedded $EMSDK_VERSION || exit 1

. "./emsdk_env.sh" || exit 1

# sometimes required? (suspected hiccups in emsdk)
# em++ --clear-cache

cd "$INITIAL_CWD" || exit 1

echo " -> success, the 'C++ to WebAssembly compiler (emsdk)' is ready"

echo ""
echo "###"
echo "### DONE!"
echo "###"
echo ""
