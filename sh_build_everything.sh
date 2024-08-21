#!/bin/bash

clear

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

mkdir -p "$DIR_DEPENDENCIES"

#
#
#
#
#

sh ./sh_ensure_wasm_compiler.sh || exit 1

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

cd ./network-wrapper || exit 1
make build_platform="native" build_mode="release" all -j4 || exit 1
# make build_platform="native" build_mode="debug" all -j4 || exit 1
cd "$CURRENT_DIR" || exit 1

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

sh ./sh_build_sample__basic-client-server.sh || exit 1

#
#
#
#
#

