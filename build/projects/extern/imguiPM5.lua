local ROOT = "../../../"

 project "imGUI"
  kind "StaticLib"
  language "C++"
  targetname "imGUI" -- the name of the executable saved to targetdir

  -----------------------------------
-- COMPILER/LINKER CONFIGURATIONS
-----------------------------------
   
filter { "platforms:*32" }
  architecture "x86"
filter { "platforms:*64" }
  architecture "x64"
  
filter { "configurations:Debug" }
  defines { "DEBUG" }
  symbols "On"
filter { "configurations:Release" }
  defines { "NDEBUG" }
  optimize "On"

    -- Exclude template files from project (so they don't accidentally get compiled)
 filter { "files:**.tpp" }
 flags {"ExcludeFromBuild"}

  filter {} -- reset filter

  local output_dir_root         = ROOT .. "/bin/lib/lib_%{cfg.platform}_%{cfg.buildcfg}"
  targetdir(output_dir_root)    -- Where all output files are stored

  local source_dir_root         = ROOT .. "src"
  local source_dir_dependencies = source_dir_root .. "/extern"
  local source_dir_engine       = source_dir_root .. "/extern/imGUI"

  local source_dir_includes     = source_dir_dependencies
  local source_dir_libs         = source_dir_dependencies .. "/**/" .. "Libs_" .. os.host()
  -- optional for libs that are 32 or 64 bit specific
  local source_dir_libs32       = source_dir_libs .. "/lib_x32"
  local source_dir_libs64       = source_dir_libs .. "/lib_x64"
  -- pchheader "stdafx.hpp"
  -- pchsource (source_dir_engine .. "/stdafx.cpp")
   
  files
  {
    source_dir_engine .. "/**.c",
    source_dir_engine .. "/**.h",
    source_dir_engine .. "/**.cpp",
    source_dir_engine .. "/**.hpp",
    source_dir_engine .. "/**.hlsl",
    source_dir_engine .. "/**.glsl",
  }

   -- Where compiler should look for library includes
   -- NOTE: for library headers always use  '#include <LIB/lib.hpp>' not quotes
   includedirs
   {
    source_dir_root,
     source_dir_includes
   }
   
   -- Where linker should look for library files
   -- NOTE: currently each library must have "LIBS_<OS>" in its path.
   libdirs
   {
     source_dir_libs,                                           -- default: look for any libs here (does not recurse)
     source_dir_libs .. "/lib_%{cfg.platform}",                 -- libs with ONLY x32/x64 (no Release/Debug) versions
     source_dir_libs .. "/%{cfg.buildcfg}",                     -- libs with ONLY Release/Debug (no x32/x64) versions
     source_dir_libs .. "/%{cfg.buildcfg}/lib_%{cfg.platform}", -- libs with BOTH Release/Debug AND x32/x64 versions
     source_dir_libs .. "/lib_%{cfg.platform}/%{cfg.buildcfg}",  -- libs with BOTH x32/x64 AND Release/Debug versions (order reversed)
     output_dir_root
   }

   
   -- OS-specific Libraries - Dynamic libs will need to be copied to output

   filter { "system:windows" }  -- Currently all static libs; No copying
     links
     {
       "opengl32",
     }