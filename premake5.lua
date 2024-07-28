local function link_vulkan()
    local vulkan_sdk = os.getenv("VULKAN_SDK")
    if not vulkan_sdk then
        error("VULKAN_SDK environment variable is not set, have you installed Vulkan SDK?")
    end

    defines {
         "_USING_VULKAN_SDK" 
        }

    includedirs {
        vulkan_sdk .. "/Include"
    }

    libdirs { vulkan_sdk .. "/Lib" }
    links { "vulkan-1" }
end

local WORKSPACE_NAME = "trigon"
local LIB_NAME = "trigon"
local APP_NAME = "app"

local function project_new(name, isApp, hide_console)
    project(name)
    location(name)
    objdir(name .. "/obj/%{cfg.buildcfg}")
    targetdir(name .. "/bin/%{cfg.buildcfg}")
    debugdir(name .. "/bin/%{cfg.buildcfg}")
    includedirs({ name .. "/src/" })
    files {
        name .. "/src/**.h",
        name .. "/src/**.c"
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
      

    filter "configurations:release"
        if isApp and hide_console then
            kind "WindowedApp"
        end

    filter {}
end

-- [WORKSPACE] WORKSPACE BEGIN
workspace(WORKSPACE_NAME)

    configurations({ "debug", "release" })
    architecture("x86_64")
    startproject(APP_NAME)
    compileas "C"
    language("C")
    cdialect("C17")
    includedirs { LIB_NAME .. "/src/cglm/include" }
    --flags { "FatalCompileWarnings" }
    flags{"FatalWarnings"}
-- [WORKSPACE] VISUAL STUDIO 
filter "action:vs*"
    defines { "VISUAL_STUDIO" }
    buildoptions { "/wd4996" }
    disablewarnings { 4996 }

-- [WORKSPACE] DEBUG 
filter "configurations:debug"
    symbols "On"

-- [WORKSPACE] RELEASE 
filter "configurations:release"
    optimize "Full"
    symbols "Off"

filter {}

project_new(LIB_NAME, false, false)
link_vulkan()
project_new(APP_NAME, true, false)
