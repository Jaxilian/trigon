@echo off
setlocal enabledelayedexpansion


premake5 vs2022
msbuild trigon.sln -p:Configuration=release
msbuild trigon.sln -p:Configuration=debug
xcopy trigon C:\Development\include /s /e /I /Y
set start_dir=C:\Development\include

for /r "%start_dir%" %%f in (*.filters *.vcxproj *.user *.c *.cpp *.cc) do (
    echo Deleting %%f
    del "%%f"
)

endlocal
