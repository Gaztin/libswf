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
  project( "SDL" )
    kind( "StaticLib" )
    configure_project_base()
    warnings( "Off" )
    staticruntime( "On" )
    includedirs { "third_party/SDL/include/" }
    files {
      "third_party/SDL/src/*.c",
      "third_party/SDL/src/*/*.c",
      "third_party/SDL/src/*/dummy/*.c",
      "third_party/SDL/src/render/software/*.c",
    }
    removefiles {
      "third_party/SDL/src/main/dummy/SDL_dummy_main.c",
    }
    -- Windows settings
    filter { "system:windows" }
      files {
        "third_party/SDL/src/*/windows/*.c",
        "third_party/SDL/src/audio/directsound/*.c",
        "third_party/SDL/src/audio/disk/*.c",
        "third_party/SDL/src/audio/wasapi/*.c",
        "third_party/SDL/src/audio/winmm/*.c",
        "third_party/SDL/src/joystick/hidapi/*.c",
        "third_party/SDL/src/render/direct3d/*.c",
        "third_party/SDL/src/render/direct3d11/*.c",
        "third_party/SDL/src/render/opengl/*.c",
        "third_party/SDL/src/render/opengles2/*.c",
        "third_party/SDL/src/thread/generic/SDL_syscond.c",
        "third_party/SDL/src/video/yuv2rgb/*.c",
      }
    -- Linux settings
    filter { "system:linux" }
      files {
        "third_party/SDL/src/*/unix/*.c",
        "third_party/SDL/src/*/linux/*.c",
        "third_party/SDL/src/render/opengl/*.c",
        "third_party/SDL/src/thread/pthread/*.c",
      }

group( "libswf" )
  project( "libswf" )
  	language( "C" )
  	configure_project_base()
  	files { "src/swf/**.c", "src/swf/**.h" }
    defines { "SWF_BUILD" }
    includedirs { "src/swf/", "third_party/zlib/" }
    -- Options
    filter { "options:librarykind=dynamic" }
      kind( "SharedLib" )
  	filter { "options:librarykind=static" }
      kind( "StaticLib" )

group( "Samples" )
  project( "01-Log" )
    kind( "ConsoleApp" )
  	configure_project_base()
    files { "src/samples/%{prj.name:lower()}/**" }
    links { "libswf", "zlib" }

  project( "02-SDL" )
    kind( "WindowedApp" )
    configure_project_base()
    files { "src/samples/%{prj.name:lower()}/**" }
    includedirs { "third_party/SDL/include/" }
    links { "libswf", "zlib", "SDL" }
    -- Windows settings
    filter { "system:windows" }
      links { "Winmm", "version", "Imm32", "Setupapi" }
