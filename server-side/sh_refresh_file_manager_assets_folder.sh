
rm -rf ./files
mkdir -p ./files
cp -rf ../client-web/dist/* ./files/

tree --du -hF ../client-web/dist
tree --du -hF ./files
