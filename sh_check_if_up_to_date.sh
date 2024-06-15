#!/bin/bash


SRC_PATH=$1
DST_PATH=$2


get_latest_mtime_of_folder() {

    INPUT=$1
    NOT_FOUND_MSG=$2

    if [ ! -d "$INPUT" ]; then

        eval "$2='-1'"

        echo "$NOT_FOUND_MSG"
        exit 0
    fi

    # echo "##############"

    ALL_FILES=$(find "$INPUT" -printf '%T@  %TY-%Tm-%Td %TH-%TM  %p\n')
    # echo "$ALL_FILES"

    # echo "##############"

    LASTEST_FILE=$(echo "$ALL_FILES" | sort -nrk1,1 | head -n 1)
    # echo "$LASTEST_FILE"

    # echo "##############"

    LASTEST_TIME=$(echo "$LASTEST_FILE" | awk '{print $1}' | sed -r 's/([0-9]*?).([0-9]*?)/\1\2/g')
    # echo "$LASTEST_TIME"

    # echo "##############"

    eval "$3='$LASTEST_TIME'"
}

return_dst=""
get_latest_mtime_of_folder "$DST_PATH" "outdated" return_dst

return_src=""
get_latest_mtime_of_folder "$SRC_PATH" "outdated" return_src

# echo "return_dst: $return_dst"
# echo "return_src: $return_src"

DIFF=$(echo "$return_dst - $return_src" | bc)

if [ "$DIFF" -lt 0 ]
then

    # echo "SRC is newer than DST, should build"
    echo "outdated"
    exit 0

else

    # echo "DST is up to date with SRC"
    echo "latest"
    exit 0

fi


