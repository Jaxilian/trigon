@echo off

if not exist "trigon\ven\include" (
    mkdir "trigon\ven\include"
)

if not exist "trigon\ven\lib\release" (
    mkdir "trigon\ven\lib\release"
)

if not exist "trigon\ven\lib\debug" (
    mkdir "trigon\ven\lib\debug"
)

if not exist "trigon\ven\src" (
    mkdir "trigon\ven\src"
)


rem Clone the repository if it doesn't exist
if not exist "luau" (
    git clone https://github.com/luau-lang/luau.git
) else (
    echo Directory 'luau' already exists. Skipping clone.
)

cd luau

rem Create and navigate to the cmake directory
if not exist "cmake" (
    mkdir cmake
)
cd cmake

rem Build with RelWithDebInfo configuration
cmake .. -DCMAKE_BUILD_TYPE=RelWithDebInfo
cmake --build . --target Luau.Repl.CLI --config RelWithDebInfo
cmake --build . --target Luau.Analyze.CLI --config RelWithDebInfo

rem Build with Release configuration
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build . --target Luau.Repl.CLI --config Release
cmake --build . --target Luau.Analyze.CLI --config Release

rem Move .lib files to the destination folder
cd RelWithDebInfo
set "dest=..\..\..\trigon\ven\lib\debug"

if not exist "%dest%" (
    mkdir "%dest%"
)
move /Y *.lib "%dest%"

set "dest=..\..\..\trigon\ven\lib\release"
if not exist "%dest%" (
    mkdir "%dest%"
)

cd ..\Release
move /Y *.lib "%dest%"

cd ..\..

rem Copy header files from include folders
setlocal
set "dest=..\trigon\ven\include"

if not exist "%dest%" (
    echo Creating destination folder "%dest%"
    mkdir "%dest%"
)

for /d %%d in (*) do (
    if exist "%%d\include" (
        echo Found include folder in "%%d"
        xcopy /E /I /Y "%%d\include" "%dest%\%%~nd"
    )
)

echo All header files have been copied to "%dest%"
endlocal

cd ..

rmdir /S /Q luau

premake5 vs2022