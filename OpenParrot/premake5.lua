project "OpenParrot"
	targetname "OpenParrot"
	language "C++"
	kind "SharedLib"

	files
	{
		"src/**.cpp", "src/**.h",
		"deps/cpp/**.cpp", "deps/inc/**.h"
	}

	includedirs { "src", "deps/inc/" }
	libdirs { "deps/lib/" }

	--pchheader 'StdInc.h'
	--pchsource 'src/StdInc.cpp'

	links { "MinHook", "udis86" }

	postbuildcommands { }

	filter "platforms:x64"
		files { "src/win64init.asm" }
	
		targetsuffix "64"