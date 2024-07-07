git submodule update --init --recursive

cd luna\src
msvcbuild static

cd ..

premake5 vs2022