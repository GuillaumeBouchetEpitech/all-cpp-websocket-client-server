#!/bin/bash

CURRENT_DIR=$PWD

#
#
#
#
#

func_build_network_wrapper() {

  echo ""
  echo "#"
  echo "# building network-wrapper (C++ -> WebAssembly)"
  echo "#"
  echo ""

  cd "$CURRENT_DIR" || exit 1
  # done to avoid the slow wasm build process that happen even if nothing changed
  RETVAL=$(sh \
    sh_check_if_up_to_date.sh \
      ./network-wrapper/src \
      ./network-wrapper/lib/wasm \
  )
  if [ "$RETVAL" != "latest" ]
  then

    echo "network-wrapper is not built or not up to date with the latest source code"

    cd "./network-wrapper" || exit 1
    make build_platform="web-wasm" build_mode="release" all -j4 || exit 1
    # make build_platform="web-wasm" build_mode="debug" all -j4 || exit 1
    cd "$CURRENT_DIR" || exit 1

  else
    echo "network-wrapper is built and up to date with the latest source code"
  fi
}

#
#
#
#
#

func_build_client_side__native() {

  echo ""
  echo "#"
  echo "# building client-side (C++ -> Native)"
  echo "#"
  echo ""

  # done to avoid the slow wasm build process that happen even if nothing changed
  RETVAL=$(sh \
    sh_check_if_up_to_date.sh \
      ./samples/basic-client-server/client-side/src \
      ./samples/basic-client-server/client-side/bin \
  )
  if [ "$RETVAL" != "latest" ]
  then

    echo "client-side is not built or not up to date with the latest source code"

    cd "./samples/basic-client-server/client-side" || exit 1
    make build_platform="native" build_mode="release" all -j4 || exit 1
    # make build_platform="native" build_mode="debug" all -j4 || exit 1
    cd "$CURRENT_DIR" || exit 1

  else
    echo "client-side is built and up to date with the latest source code"
  fi
}

#
#
#
#
#

func_build_client_side__wasm() {

  echo ""
  echo "#"
  echo "# building client-side (C++ -> WebAssembly)"
  echo "#"
  echo ""

  # done to avoid the slow wasm build process that happen even if nothing changed
  RETVAL=$(sh \
    sh_check_if_up_to_date.sh \
      ./samples/basic-client-server/client-side/src \
      ./samples/basic-client-server/client-side/dist/wasm \
  )
  if [ "$RETVAL" != "latest" ]
  then

    echo "client-side is not built or not up to date with the latest source code"

    cd "./samples/basic-client-server/client-side" || exit 1
    make build_platform="web-wasm" build_mode="release" all -j4 || exit 1
    # make build_platform="web-wasm" build_mode="debug" all -j4 || exit 1
    cd "$CURRENT_DIR" || exit 1

  else
    echo "client-side is built and up to date with the latest source code"
  fi
}

#
#
#
#
#

func_build_client_side__web_wasm_loader() {

  echo ""
  echo "#"
  echo "# building web-wasm-loader (TypeScript -> JavaScript)"
  echo "#"
  echo ""

  cd "./samples/basic-client-server/client-side/" || exit 1

  if [ -f "./dist/index.js" ] && [ -f "./dist/index.html" ]
  then

    echo " ===> 'dist' folder content up to date"
    echo " =====> skipping further checks"

  else

    echo " ===> 'dist' folder content is missing"
    echo " =====> checking"

    cd "./web-wasm-loader" || exit 1

    if [ -d "./node_modules" ]
    then
      echo " =====> up to date dependencies"
      echo " =======> skip install"
    else
      echo " =====> missing dependencies"
      echo " =======> installing"

      npm install || exit 1
    fi

    if [ -f "./js/bundle.js" ]
    then
      echo " =====> up to date bundle.js"
      echo " =======> skip bundling"
    else
      echo " =====> outdated bundle.js"
      echo " =======> bundling"

      npm run release || exit 1
    fi

    echo " ===> ensure 'dist' folder exist"

    npm run ensure-dist-folders || exit 1

    echo " ===> copying html to 'dist' folder"

    npm run copy-html-to-dist-folders || exit 1

    echo " ===> copying js to 'dist' folder"

    npm run copy-js-to-dist-folders || exit 1

  fi

  cd "$CURRENT_DIR" || exit 1
}

#
#
#
#
#

func_build_server_side() {

  echo ""
  echo "#"
  echo "# building server-side (C++ -> binary)"
  echo "#"
  echo ""

  cd "./samples/basic-client-server/server-side" || exit 1
  echo "building server-side - application"
  make build_mode="release" application -j4 || exit 1
  # make build_mode="debug" application -j4 || exit 1

  echo ""
  echo "#"
  echo "# updating file server asset folder with latest client build"
  echo "#"
  echo ""

  sh sh_refresh_file_manager_assets_folder.sh

  cd "$CURRENT_DIR" || exit 1
}

#
#
#
#
#

func_build_network_wrapper
func_build_client_side__native
func_build_client_side__wasm
func_build_client_side__web_wasm_loader
func_build_server_side

#
#
#
#
#

echo ""
echo "#"
echo "# starting server side (localhost)"
echo "#"
echo ""

cd "./samples/basic-client-server/server-side" || exit 1
./bin/exec 127.0.0.1 7777 8888 4 4
# gdb ./bin/exec 127.0.0.1 7777 8888 4 4

cd "$CURRENT_DIR" || exit 1

#
#
#
#
#

