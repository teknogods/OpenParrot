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
		
	prebuildcommands {
		"if not exist $(TargetDir)output mkdir $(TargetDir)output",
	}
		
	postbuildcommands {
	  "if exist $(TargetDir)OpenParrot.dll xcopy /y $(TargetDir)OpenParrot.dll $(TargetDir)output\\",
	  "if exist $(TargetDir)OpenParrot64.dll xcopy /y $(TargetDir)OpenParrot64.dll $(TargetDir)output\\"
	}