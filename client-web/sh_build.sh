#!/bin/bash


echo ""
echo "Build mode?"
echo "=> release: 1 (default)"
echo "=> debug:   2"
echo ""

read USER_INPUT_MODE

case $USER_INPUT_MODE in
2)
    echo ""
    echo "selected mode: debug"
    echo ""
    selected_mode=debug
    ;;
*)
    echo ""
    echo "selected mode: release"
    echo ""
    selected_mode=release
    ;;
esac

echo ""
echo "Rebuild?"
echo "=> no:  1 (default)"
echo "=> yes: 2"
echo ""

read USER_INPUT_REBUILD

case $USER_INPUT_REBUILD in
2)
    echo ""
    echo "rebuilding: yes"
    echo ""
    must_clean=yes
    ;;
*)
    echo ""
    echo "rebuilding: no"
    echo ""
    must_clean=no
    ;;
esac

#
#
#

case $selected_mode in
debug)
    ARG_SEL_MODE="debug"
    ;;
release)
    ARG_SEL_MODE="release"
    ;;
esac

#
#
#

echo ""
echo "=> building"
echo ""

case $must_clean in
yes)
    make build_mode="${ARG_SEL_MODE}" fclean
    ;;
esac

make build_mode="${ARG_SEL_MODE}" all -j10
