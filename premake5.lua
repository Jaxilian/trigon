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

local function project_new(name, isApp, hide_console)
    project(name)
    location(name)
    objdir(name .. "/obj/%{cfg.buildcfg}")
    targetdir(name .. "/bin/%{cfg.buildcfg}")
    debugdir(name .. "/bin/%{cfg.buildcfg}")
    includedirs({ name .. "/src/" })
    files {
        name .. "/src/**.h",
        name .. "/src/**.c",
        name .. "/src/**.cpp"
    }

    if not isApp then
        kind("StaticLib")
    else
        kind "ConsoleApp"
        includedirs({ LIB_NAME .. "/src/" })
        links(LIB_NAME)
    end

    if os.target() == "windows" then
        defines { "_WIN32" }
    elseif os.target() == "linux" then
        defines { "_LINUX", "_UNIX" }
    elseif os.target() == "macosx" then
        defines { "_MACOSX", "_UNIX" }
    end

    filter "configurations:debug"
        -- Add debug-specific settings here

    filter "configurations:release"
        if isApp and hide_console then
            kind "WindowedApp"
        end

    filter {}
end

local function setup_workspace()
    workspace(WORKSPACE_NAME)

    defines { "CGLM_STATIC" }
    configurations { "debug", "release" }
    architecture("x86_64")
    startproject(APP_NAME)
    cdialect("C17")
    includedirs { LIB_NAME .. "/src/cglm/include" }
    
    if USE_CPP then
        compileas "C++"
        language("C++")
        cppdialect("C++latest")
    else
        compileas "C"
        language("C")
    end

    if os.target() == "windows" then
        defines { "VISUAL_STUDIO", "_WIN32", "_WIN64" }
        buildoptions { "/GR-", "/wd4996" }
        disablewarnings { 4996 }
    end

    flags { "FatalWarnings" }

    filter "configurations:debug"
        symbols "On"
        defines { "_DEBUG" }

    filter "configurations:release"
        flags {
            "LinkTimeOptimization",
            "MultiProcessorCompile",
            "FatalWarnings",
            "NoMinimalRebuild",
            "NoBufferSecurityCheck",
            "NoIncrementalLink"
        }

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

        optimize "Full"
        symbols "Off"
        defines { "_NDEBUG", "_RELEASE" }

    filter {}
end

setup_workspace()
project_new(LIB_NAME, false, true)
link_vulkan()
project_new(APP_NAME, true, true)
