project "OpenParrotLoader"
	targetname "OpenParrotLoader"
	language "C++"
	kind "ConsoleApp"

	characterset "MBCS"

	files
	{
		"src/**.cpp", "src/**.h",
		"deps_inc/**.cpp", "deps_inc/**.h"
	}

	includedirs { "src", "deps_inc" }
	libdirs { }

	links { }

	postbuildcommands { }

	filter "platforms:x64"
		targetsuffix "64"