#!/bin/bash

#

cd ./web-wasm-loader || exit 1

#

if [ -d "./node_modules" ]
then
    echo " =====> up to date dependencies"
    echo " =======> skip install"
else
    echo " =====> missing dependencies"
    echo " =======> installing"

    npm install || exit 1
fi

#

if [ -f "./js/bundle.js" ]
then
    echo " =====> up to date bundle.js"
    echo " =======> skip bundling"
else
    echo " =====> outdated bundle.js"
    echo " =======> bundling"

    npm run release || exit 1
fi

#

echo " ===> ensure 'dist' folder exist"

npm run ensure-dist-folders || exit 1

#

echo " ===> copying html to 'dist' folder"

npm run copy-html-to-dist-folders || exit 1

#

echo " ===> copying js to 'dist' folder"

npm run copy-js-to-dist-folders || exit 1

#
