#!/bin/bash

premake5 gmake2

make config=release
make config=debug

cp -r ./trigon/* ./include/

find ./include -type f \( -name "*.c" -o -name "*.cpp" -o -name "*.cc" \) -exec rm -f {} \;

