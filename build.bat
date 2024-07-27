@echo off
setlocal enabledelayedexpansion

if not exist "C:\Applications\Premake5" (
    git clone https://github.com/Jaxilian/Premake5.git
    xcopy Premake5 C:\Applications\Premake5 /s /e /I /Y
      REM Delete the Premake5 folder after copying its contents
    rd /s /q Premake5
)


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
