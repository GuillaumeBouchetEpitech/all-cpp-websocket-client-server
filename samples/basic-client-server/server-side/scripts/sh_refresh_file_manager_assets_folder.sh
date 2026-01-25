#!/bin/bash


echo "pwd -> $PWD"

echo " -> refreshing the server files"
rm -rf ./files
mkdir -p ./files
cp -rf ../client-side/dist/* ./files/

echo " ---> server files updated"

tree --du -hF ../client-side/dist
tree --du -hF ./files
