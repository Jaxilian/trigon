
local USE_LUNA = true

local function compileShaders()

end

local function linkVulkan()
   local vulkan_sdk = os.getenv("VULKAN_SDK")

   if not vulkan_sdk then
       error("VULKAN_SDK environment variable is not set")
   end

   defines {"_VULKAN_EXPOSED"}
   includedirs {
       vulkan_sdk .. "/Include"
   }
   libdirs { vulkan_sdk .. "/Lib" }
   links { "vulkan-1" }
end

local function linkLuna()
   if USE_LUNA then
      defines { "_LUNA" } 

      includedirs {
          "luna/src/"
      }

      files { 
         "luna/src/lua.h",
         "luna/src/lualib.h",
         "luna/src/lauxlib.h",
         "luna/src/luajit.h"
      }
      links {
         "lua51",
         "luajit"
      }
      libdirs {
         "luna/src"
      }
   end
end

local function defaultSettings(name, isLib)
   language "C++"
   cdialect("C17")
   cppdialect("C++20")
   debugdir (name .. "/bin/%{cfg.buildcfg}")
   targetdir(name .. "/bin/%{cfg.buildcfg}")
   objdir(name .. "/obj/%{cfg.buildcfg}")
   location(name)
   files {
      name .. "/src/**.h", name .. "/src/**.c",
      name .. "/ven/**.h", name .. "/ven/**.c",
    }
   includedirs {
      name .. "/src",
      name .. "/ven/include/"
   }
   disablewarnings{4996}
   
   if os.target() == "windows" then
      defines { "_WIN32", "CGLM_STATIC" } 
   elseif os.target() == "linux" then
      defines { "_LINUX", "_UNIX" } 
   elseif os.target() == "macosx" then
      defines { "_MACOSX", "_UNIX" } 
   end


  filter "configurations:debug"
      defines { "_DEBUG" }
      symbols "On"
      if os.target() == "windows" then
         buildoptions { "/Odi" } 
      elseif os.target() == "linux" then
         buildoptions { "-Og" } 
      elseif os.target() == "macosx" then
         buildoptions { "-Og" } 
      end


   filter "configurations:release"

      if not isLib then
         kind("WindowedApp")
      end

      defines { "_NDEBUG" }
      optimize "On"
      if os.target() == "windows" then
         buildoptions { "/O2", "/GL" } 
         linkoptions { "/LTCG" }     
     elseif os.target() == "linux" then
         buildoptions { "-O3", "-flto" }  
         linkoptions { "-flto" }        
     elseif os.target() == "macosx" then
         buildoptions { "-O3", "-flto" }  
         linkoptions { "-flto" }   
     end
end

local function runtime()
   project("runtime")
   kind("StaticLib")
   ignoredefaultlibraries { "libraries" }
   linkVulkan()
   linkLuna()
   defaultSettings("runtime", true) -- do last!
end

local function editor()
   project("editor")
   kind("ConsoleApp")
   links {"runtime"}
   includedirs {
      "runtime/src"
   }
   defaultSettings("editor", false) -- do last!
end


workspace "trigon"
   configurations { "debug", "release" }
   startproject "editor"
   architecture "x86_64"

   runtime()
   editor()

filter "action:vs*"
   defines { "VISUAL_STUDIO" }
   buildoptions { "/wd4996" }
