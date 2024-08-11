@echo off

set ENABLE_LUAU=true
set ENABLE_CGLM=true
set ENABLE_SDL=true
set ENABLE_ASSIMP=true

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

    rem Move .lib files to the destination folder
    pushd RelWithDebInfo
    set "dest=..\..\..\trigon\ven\lib\debug"

    if not exist "%dest%" (
        mkdir "%dest%"
    )
    move /Y *.lib "%dest%"
    popd

    set "dest=..\..\..\trigon\ven\lib\release"
    if not exist "%dest%" (
        mkdir "%dest%"
    )

    pushd Release
    move /Y *.lib "%dest%"
    popd

    rem Copy header files from include folders
    set "dest=..\..\..\trigon\ven\include"

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
    popd
    popd
    
    rmdir /S /Q luau
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

    cmake -DCMAKE_BUILD_TYPE=RelWithDebInfo -DSDL_STATIC=ON ..
    cmake --build . --config RelWithDebInfo --parallel

    xcopy RelWithDebInfo\SDL3-static.lib ..\..\trigon\ven\lib\debug

    cmake -DCMAKE_BUILD_TYPE=Release -DSDL_STATIC=ON ..
    cmake --build . --config Release --parallel
    
    xcopy Release\SDL3-static.lib ..\..\trigon\ven\lib\release

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


premake5 vs2022
