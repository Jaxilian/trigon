
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

local function defaultSettings(name)
   language "C"
   cdialect("C17")
   targetdir(name .. "/bin/%{cfg.buildcfg}")
   objdir(name .. "/obj/%{cfg.buildcfg}")
   location(name)
   files { name .. "/src/**.h", name .. "/src/**.c" }
   includedirs {
      name .. "/src"
  }


   if os.target() == "windows" then
      defines { "_WIN32" } 
   elseif os.target() == "linux" then
      defines { "_LINUX" } 
   elseif os.target() == "macosx" then
      defines { "_MACOSX" } 
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
   linkVulkan()
   defaultSettings("runtime") -- do last!
end

local function editor()
   project("editor")
   kind("ConsoleApp")
   links {"runtime"}
   includedirs {
      "runtime/src"
   }
   defaultSettings("editor") -- do last!
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
