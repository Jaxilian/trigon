local function link_vulkan()
    if os.target() == "windows" then
        local vulkan_sdk = os.getenv("VULKAN_SDK")
        if not vulkan_sdk then
            error("VULKAN_SDK environment variable is not set. Have you installed Vulkan SDK?")
        end

        includedirs { path.join(vulkan_sdk, "Include") }
        libdirs { path.join(vulkan_sdk, "Lib") }
    else
    end
    
    defines { "_USING_VULKAN_SDK" }
    links { "vulkan-1" , "glslang"}
end

workspace("cop")
configurations({"debug", "release"})

startproject("cop")
language("C")
cdialect "C17"
architecture "x86_64"

if os.target() == "windows" then
    defines { "_WIN32" }
else
    defines { "_LINUX", "_UNIX" }
end

flags { "FatalWarnings" }

filter { "action:gmake*" }
    buildoptions { "-march=native" }
    linkoptions { "-march=native" }
filter { "action:vs*" }
    defines { "VISUAL_STUDIO", "_WIN32", "_WIN64" }
    buildoptions { "/GR-", "/wd4996", "/wd4099", "/arch:AVX2" }
    disablewarnings { "4996", "4099", 4099}

filter("configurations:debug")
    symbols "On"
    defines { "_DEBUG" }

filter("configurations:release")
    flags {
        "LinkTimeOptimization",
        "MultiProcessorCompile",
        "FatalWarnings",
        "NoMinimalRebuild",
        "NoBufferSecurityCheck",
        "NoIncrementalLink"
    }

    optimize "Full"
    symbols "On"
    --symbols "Off"
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

filter({})

local PNAME = "tgn"
project(PNAME)
    kind("StaticLib")
    files({ PNAME.."/**.h", PNAME.."/**.c" })
    targetdir("gen/bin/"..PNAME.."-%{cfg.buildcfg}")
    objdir("gen/obj/"..PNAME.."-%{cfg.buildcfg}")
    debugdir("gen/bin/"..PNAME.."-%{cfg.buildcfg}")
    includedirs { PNAME .. "/", PNAME .. "/cglm/include" }
    location(PNAME)
    link_vulkan()

PNAME = "cop"
project(PNAME)
    kind("ConsoleApp")
    files({ PNAME.."/**.h", PNAME.."/**.c" })
    targetdir("gen/bin/"..PNAME.."-%{cfg.buildcfg}")
    objdir("gen/obj/"..PNAME.."-%{cfg.buildcfg}")
    debugdir("gen/bin/"..PNAME.."-%{cfg.buildcfg}")
    includedirs { PNAME .. "/", "tgn/" }
    location(PNAME)
    links("tgn")

