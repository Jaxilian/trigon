@echo off

set ENABLE_LUAU=true
set ENABLE_CGLM=false
set ENABLE_SDL=false
set ENABLE_ASSIMP=false

if "%ENABLE_LUAU%"=="true" (
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

    REM LUAU BEGIN

    rem Clone the repository if it doesn't exist
    if not exist "luau" (
        git clone https://github.com/luau-lang/luau.git
    ) else (
        echo Directory 'luau' already exists. Skipping clone.
    )

    pushd luau

    rem Create and navigate to the cmake directory
    if not exist "cmake" (
        mkdir cmake
    )
    pushd cmake

    rem Build with RelWithDebInfo configuration
    cmake .. -DCMAKE_BUILD_TYPE=RelWithDebInfo
    cmake --build . --target Luau.Repl.CLI --config RelWithDebInfo
    cmake --build . --target Luau.Analyze.CLI --config RelWithDebInfo

    rem Build with Release configuration
    cmake .. -DCMAKE_BUILD_TYPE=Release
    cmake --build . --target Luau.Repl.CLI --config Release
    cmake --build . --target Luau.Analyze.CLI --config Release

    rem Copy the built libraries to the appropriate directories
    xcopy /E /I /Y "Release\*.lib" "..\..\trigon\ven\lib\release"
    xcopy /E /I /Y "RelWithDebInfo\*.lib" "..\..\trigon\ven\lib\debug"

    popd

    set "dest=..\trigon\ven\include"
    
    rem Copy only the contents of the include directories without the parent folder
    for /d %%d in (.\*) do (
        if exist "%%d\include" (
            echo Found include folder in "%%d"
            xcopy /E /I /Y "%%d\include\*" "%dest%\"
            echo Copied contents of %%d\include\* to "%dest%\"
        )
    )

    popd
    
    REM rmdir /S /Q luau
)

if "%ENABLE_CGLM%"=="true" (
    if not exist "cglm" (
        git clone https://github.com/recp/cglm.git
    )

    pushd cglm

    set "dest=..\trigon\ven\include"
    xcopy /E /I /Y ".\include" "%dest%"
    echo All header files have been copied to "%dest%"

    set "dest=..\trigon\ven\src"
    xcopy /E /I /Y ".\src" "%dest%"
    echo All source files have been copied to "%dest%"

    popd

    rmdir /S /Q cglm
)

if "%ENABLE_SDL%"=="true" (
    if not exist "SDL" (
        git clone https://github.com/libsdl-org/SDL.git
    )

    pushd SDL
    if not exist "build" (
        mkdir build
    )

    xcopy /E /I /Y ".\include" "..\trigon\ven\include"

    pushd build

    cmake -DCMAKE_BUILD_TYPE=Debug -DSDL_STATIC=ON ..
    cmake --build . --config Debug --parallel

    xcopy /E /I /Y "Debug\" "..\..\trigon\ven\lib\debug"

    cmake -DCMAKE_BUILD_TYPE=Release -DSDL_STATIC=ON ..
    cmake --build . --config Release --parallel
    
    xcopy /E /I /Y "Release\" "..\..\trigon\ven\lib\release"


    popd
    popd

    rmdir /S /Q SDL
)

if "%ENABLE_ASSIMP%"=="true" (
    if not exist "assimp" (
        git clone https://github.com/assimp/assimp.git
    )

    pushd assimp
    xcopy /E /I /Y ".\include" "..\trigon\ven\include"
    
    if not exist "build" (
        mkdir build
    )


    pushd build
    cmake -DCMAKE_BUILD_TYPE=Release -DBUILD_SHARED_LIBS=OFF ..
    cmake --build . --config Release --parallel
    xcopy lib\Release\assimp-vc143-mt.lib ..\..\trigon\ven\lib\release

    cmake -DCMAKE_BUILD_TYPE=RelWithDebInfo -DBUILD_SHARED_LIBS=OFF ..
    cmake --build . --config RelWithDebInfo --parallel
    xcopy lib\RelWithDebInfo\assimp-vc143-mt.lib ..\..\trigon\ven\lib\debug

    popd
    popd

     rmdir /S /Q assimp
)

xcopy /Y /S /I "trigon\ven\lib\debug\*.pdb" "..\..\trigon\bin\debug"

premake5 vs2022
