project "OpenParrotKonamiLoader"
	targetname "OpenParrotKonamiLoader"
	language "C++"
	kind "ConsoleApp"
	removeplatforms { "x64" }

	characterset "MBCS"

	files
	{
		"src/**.cpp", "src/**.h",
		"deps_inc/**.cpp", "deps_inc/**.h"
	}

	includedirs { "src", "deps_inc" }
	libdirs { }

	links { "MinHook", "udis86" }

	filter "platforms:x64"
		targetsuffix "64"
		
	prebuildcommands {
		"if not exist $(TargetDir)output mkdir $(TargetDir)output",
	}
		
	postbuildcommands {
		"if exist $(TargetDir)OpenParrotKonamiLoader.exe xcopy /y $(TargetDir)OpenParrotKonamiLoader.exe $(TargetDir)output\\"
	}