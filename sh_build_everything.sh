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
echo "# building sample: basic-client-server"
echo "#"
echo ""

sh ./sh_build_sample__basic_client_server.sh

#
#
#
#
#

