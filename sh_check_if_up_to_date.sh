#!/bin/bash


SRC_PATH=$1
DST_PATH=$2


func_lol() {

    INPUT=$1

    # echo "##############"

    ALL_FILES=$(find $INPUT -printf '%T@  %TY-%Tm-%Td %TH-%TM  %p\n')
    # echo "$ALL_FILES"

    # echo "##############"

    LASTEST_FILE=$(echo "$ALL_FILES" | sort -nrk1,1 | head -n 1)
    # echo "$LASTEST_FILE"

    # echo "##############"

    LASTEST_TIME=$(echo "$LASTEST_FILE" | awk '{print $1}' | sed -r 's/([0-9]*?).([0-9]*?)/\1\2/g')
    # echo "$LASTEST_TIME"

    # echo "##############"

    eval "$2='$LASTEST_TIME'"
}

return_src=""
# func_lol "./client-web/src" return_src
func_lol "$SRC_PATH" return_src

return_dst=""
# func_lol "./client-web/dist/wasm" return_dst
func_lol "$DST_PATH" return_dst

# echo "return_src=$return_src"
# echo "return_dst=$return_dst"

DIFF=$(echo "$return_dst - $return_src" | bc)

if [ $DIFF -lt 0 ]
then

    # echo "SRC is newer than DST, should build"
    echo "outdated"
    exit 1

else

    # echo "DST is up to date with SRC"
    echo "latest"
    exit 0

fi


