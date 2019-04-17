newoption {
  trigger     = "librarykind",
  description = "library linkage (static vs dynamic)",
  default     = "static",
  allowed     = {
    { "static",  "Statically linked" },
    { "dynamic", "Dynamically linked" },
  }
}

local function configure_project_base()
  -- Directories
  debugdir( "assets/" )
  location( "build/%{_ACTION}/" )
  objdir( "build/%{_ACTION}/%{cfg.platform}/%{cfg.buildcfg}/obj/" )
  targetdir( "bin/%{cfg.buildcfg}/" )

  -- Single-value settings
  cdialect( "C11" )
  cppdialect( "C++17" )
  exceptionhandling( "Off" )
  rtti( "Off" )
  warnings( "Extra" )

  -- Multi-value settings
  flags { "FatalCompileWarnings" }
  includedirs { "src/" }
  sysincludedirs { "src/" }

  -- Debug settings
  filter { "configurations:Debug" }
    optimize( "Off" )
    symbols( "On" )

  -- Release settings
  filter { "configurations:Release" }
  	optimize( "Full" )
  	symbols( "On" )

  -- Windows
  filter { "system:windows" }
    defines { "NOMINMAX", "WIN32_LEAN_AND_MEAN" }

  -- Determine settings from options
  filter { "options:librarykind=static" }
    defines { "SWF_STATIC_LIB" }

  -- Reset filter
  filter { }
end

workspace( "libswf" )
  platforms { io.popen("uname -m", "r"):read("*l") }
  configurations { "Debug", "Release" }

group( "ThirdParty" )
  project( "zlib" )
    kind( "StaticLib" )
    configure_project_base()
    warnings( "Off" )
    files {
      "third_party/zlib/*.c",
      "third_party/zlib/*.h",
    }

group( "libswf" )
  project( "libswf" )
  	language( "C" )
  	configure_project_base()
  	files { "src/swf/*.c", "src/swf/*.h" }
    defines { "SWF_BUILD" }
    includedirs { "third_party/zlib/" }
    -- Options
    filter { "options:librarykind=dynamic" }
      kind( "SharedLib" )
  	filter { "options:librarykind=static" }
      kind( "StaticLib" )

group( "Samples" )
  project( "01-Log" )
    kind( "ConsoleApp" )
  	configure_project_base()
    files { "src/samples/%{prj.name:lower()}/*" }
    links { "libswf", "zlib" }
