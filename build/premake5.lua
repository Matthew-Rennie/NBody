-- project root

local ROOT = "../"


-- premake5.lua
workspace "Engine"
   configurations { "Debug", "Release"}
   platforms { "x64", "x32" }
   location( ROOT .. "project_" .. _ACTION)
   startproject "Editor"
  -- when building any visual studio project
  filter { "system:windows", "action:vs*"}
    flags         { "MultiProcessorCompile", "NoMinimalRebuild" }


  cppdialect "C++17"

  dofile "projects/corePM5.lua"
  dofile "projects/sandboxPM5.lua"
  dofile "projects/graphicsPM5.lua"
  dofile "projects/extern/imguiPM5.lua"
  dofile "projects/extern/directXtkPM5.lua"
  