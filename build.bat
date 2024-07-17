REM git submodule update --init --recursive

REM cd luna\src
REM msvcbuild static

cd ..
cd cglm
cd win
.\build.bat
devenv cglm.sln /Build Release
cd ..\..

REM premake5 vs2022