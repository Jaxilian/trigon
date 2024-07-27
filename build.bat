premake5 vs2022
msbuild trigon.sln -p:Configuration=release
msbuild trigon.sln -p:Configuration=debug

xcopy trigon\src include /s /e /I

@echo off
setlocal enabledelayedexpansion

REM 
set start_dir=include

REM 
for /r "%start_dir%" %%f in (*.c *.cpp *.cc) do (
    echo Deleting %%f
    del "%%f"
)

endlocal