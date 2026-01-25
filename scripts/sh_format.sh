#!/bin/bash


# find ./network-wrapper/src/ \( -iname "*.h" -o -iname "*.hpp" -o -iname "*.cpp" \) | xargs clang-format -i

find ./network-wrapper/src/ \( -iname "*.h" -o -iname "*.hpp" -o -iname "*.cpp" \) -exec clang-format -i {} \;
find ./samples/basic-client-server/client-side/src/ \( -iname "*.h" -o -iname "*.hpp" -o -iname "*.cpp" \) -exec clang-format -i {} \;
find ./samples/basic-client-server/server-side/src/ \( -iname "*.h" -o -iname "*.hpp" -o -iname "*.cpp" \) -exec clang-format -i {} \;

# find ./samples/basic-client-server/client-side/src \
#   \( -iname "*.h" -o -iname "*.hpp" -o -iname "*.cpp" \) | xargs -0 clang-format -i

# find ./samples/basic-client-server/server-side/src \
#   \( -iname "*.h" -o -iname "*.hpp" -o -iname "*.cpp" \) | xargs -0 clang-format -i

# find ./tests/src \
#   \( -iname "*.h" -o -iname "*.hpp" -o -iname "*.cpp" \) | xargs -0 clang-format -i

