
rm -rf ./files
mkdir -p ./files
cp -rf ../client-side/dist/* ./files/

tree --du -hF ../client-side/dist
tree --du -hF ./files
