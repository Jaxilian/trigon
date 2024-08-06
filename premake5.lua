local USE_CPP = false
local WORKSPACE_NAME = "trigon"
local LIB_NAME = "trigon"
local APP_NAME = "app"

local function link_vulkan()
    local vulkan_sdk = os.getenv("VULKAN_SDK")
    if not vulkan_sdk then
        error("VULKAN_SDK environment variable is not set. Have you installed Vulkan SDK?")
    end

    defines { "_USING_VULKAN_SDK" }
    includedirs { path.join(vulkan_sdk, "Include") }
    libdirs { path.join(vulkan_sdk, "Lib") }
    links { "vulkan-1" }
end

local function compile_shaders()
    if os.target() == "windows" then
        prebuildcommands {
            "$(ProjectDir)\\res\\shaders\\compile.bat %{cfg.buildcfg}"
        }
    else
        prebuildcommands {
            "chmod +x " .. APP_NAME .. "/res/shaders/compile.sh",
            "./" .. APP_NAME .. "/res/shaders/compile.sh %{cfg.buildcfg}"
        }
    end
end

local function project_new(name, is_app, hide_console)
    project(name)
    location(name)
    objdir(name .. "/obj/%{cfg.buildcfg}")
    targetdir(name .. "/bin/%{cfg.buildcfg}")
    debugdir(name .. "/bin/%{cfg.buildcfg}")
    includedirs { name .. "/src/" }
    files {
        name .. "/src/**.h",
        name .. "/src/**.c",
        name .. "/ven/src/**.c",
        name .. "/ven/src/**.h",
    }


    if USE_CPP then
        files {
            name .. "/src/**.cpp",
            name .. "/src/**.cc",
            name .. "/ven/src/**.cc",
            name ..  "/ven/src/**.cpp",
        }
    end

    includedirs { name .. "/ven/include/" }
    libdirs { name .. "/ven/lib/" }

    if is_app then
        kind "ConsoleApp"
        includedirs { LIB_NAME .. "/src/" }
        links { LIB_NAME }
    else
        kind "StaticLib"
    end

    filter { "system:windows" }
        defines { "_WIN32" }
    filter { "system:linux" }
        defines { "_LINUX", "_UNIX" }
    filter { "system:macosx" }
        defines { "_MACOSX", "_UNIX" }

    filter { "configurations:debug" }
        -- Add debug-specific settings here

    filter { "configurations:release" }
        if is_app and hide_console then
            kind "WindowedApp"
        end

    filter {}
end

local function setup_workspace()
    workspace(WORKSPACE_NAME)
    configurations { "debug", "release" }
    architecture "x86_64"
    startproject(APP_NAME)
    cdialect "C17"
    defines { "CGLM_STATIC" }

    if USE_CPP then
        language "C++"
        cppdialect "C++latest"
    else
        language "C"
    end

    flags { "FatalWarnings" }

    filter { "action:gmake*" }
        buildoptions { "-march=native" }
        linkoptions { "-march=native" }
    filter { "action:vs*" }
        defines { "VISUAL_STUDIO", "_WIN32", "_WIN64" }
        buildoptions { "/GR-", "/wd4996", "/arch:AVX2" }
        disablewarnings { "4996" }

    filter { "configurations:debug" }
        symbols "On"
        defines { "_DEBUG" }

    filter { "configurations:release" }
        flags {
            "LinkTimeOptimization",
            "MultiProcessorCompile",
            "FatalWarnings",
            "NoMinimalRebuild",
            "NoBufferSecurityCheck",
            "NoIncrementalLink"
        }

        optimize "Full"
        symbols "Off"
        defines { "_NDEBUG", "_RELEASE" }

        if os.target() == "windows" then
            buildoptions { "/O2", "/arch:AVX2", "/GL", "/wd4996" }
            linkoptions { "/LTCG" }
        else
            buildoptions {
                "-fno-rtti",
                "-O2",
                "-march=native",
                "-Wall",
                "-Wextra",
                "-Werror",
                "-flto"
            }
            linkoptions { "-flto" }
        end

    filter {}
end

setup_workspace()
project_new(LIB_NAME, false, true)
link_vulkan()
project_new(APP_NAME, true, true)
compile_shaders()
