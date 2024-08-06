@echo off
setlocal enabledelayedexpansion

set SOURCE_DIR=./
set TARGET_DIR=../../bin/debug/data/shaders/

if not exist "%TARGET_DIR%" mkdir "%TARGET_DIR%"

for %%f in (%SOURCE_DIR%\*.vert %SOURCE_DIR%\*.frag) do (
    if "%%~xf"==".vert" (
        set OUT_FILE=%%~nf_vert.spv
    ) else if "%%~xf"==".frag" (
        set OUT_FILE=%%~nf_frag.spv
    )

    "%VULKAN_SDK%\Bin\glslc.exe" %%f -o "%TARGET_DIR%/!OUT_FILE!"
)
