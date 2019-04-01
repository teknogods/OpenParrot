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

	prebuildcommands {
		"if not exist $(TargetDir)output mkdir $(TargetDir)output",
	}
	
	postbuildcommands {
	  "if exist $(TargetDir)OpenParrotLoader.exe xcopy /y $(TargetDir)OpenParrotLoader.exe $(TargetDir)output\\",
	  "if exist $(TargetDir)OpenParrotLoader64.exe xcopy /y $(TargetDir)OpenParrotLoader64.exe $(TargetDir)output\\"
	}

	filter "platforms:x64"
		targetsuffix "64"