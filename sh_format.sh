


find ./client-side/src -iname *.h -o -iname *.hpp -o -iname *.cpp | xargs clang-format -i
find ./server-side/src -iname *.h -o -iname *.hpp -o -iname *.cpp | xargs clang-format -i
find ./network-wrapper/src -iname *.h -o -iname *.hpp -o -iname *.cpp | xargs clang-format -i


