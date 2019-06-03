workspace "OpenParrot"
	configurations { "Debug", "Release"}
	platforms { "x64", "x86" }

	flags { "StaticRuntime", "No64BitChecks" }

	systemversion "latest"

	symbols "On"

	characterset "Unicode"

	flags { "NoIncrementalLink", "NoEditAndContinue", "NoMinimalRebuild" }

	includedirs { "deps/inc/", "deps/udis86/" }

	libdirs { "deps/lib/" }

	buildoptions { "/MP", "/std:c++17" }

	configuration "Debug*"
		targetdir "build/bin/debug"
		defines "NDEBUG"
		objdir "build/obj/debug"

	configuration "Release*"
		targetdir "build/bin/release"
		defines "NDEBUG"
		optimize "speed"
		objdir "build/obj/release"

	filter "platforms:x86"
		architecture "x32"

	filter "platforms:x64"
		architecture "x64"

project "MinHook"
	targetname "MinHook"
	language "C"
	kind "StaticLib"

	files
	{
		"deps/src/buffer.c", "deps/src/hook.c",
		"deps/src/trampoline.c",
	}

	filter "platforms:x86"
		files { "deps/src/hde/hde32.c" }

	filter "platforms:x64"
		files { "deps/src/hde/hde64.c" }

project "udis86"
	targetname "udis86"
	language "C"
	kind "StaticLib"

	includedirs
	{
		"deps/udis86/"
	}

	files
	{
		"deps/udis86/libudis86/*.c"
	}

include "OpenParrot"
include "OpenParrotLoader"
include "OpenParrotKonamiLoader"
include "iDmacDrv"