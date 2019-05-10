#!/usr/bin/env sh
set -evx

clang-format --version

git ls-files -- 'unit*.*pp' '*test*.*pp' '*fuzz*.cpp' | xargs clang-format -sort-includes -i -style=file

git diff --exit-code --color

set +evx
