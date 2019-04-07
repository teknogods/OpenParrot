project "iDmacDrv"
	targetname "iDmacDrv"
	language "C++"
	kind "SharedLib"

	files
	{
		"src/**.cpp", "src/**.h",
		"deps/cpp/**.cpp", "deps/inc/**.h"
	}

	includedirs { "src", }
	
	linkoptions { "/DEF:iDmacDrv/src/Source.def" }

	prebuildcommands {
		"if not exist $(TargetDir)output mkdir $(TargetDir)output",
	}
		
	postbuildcommands {
	  "if exist $(TargetDir)iDmacDrv32.dll xcopy /y $(TargetDir)iDmacDrv32.dll $(TargetDir)output\\",
	  "if exist $(TargetDir)iDmacDrv64.dll xcopy /y $(TargetDir)iDmacDrv64.dll $(TargetDir)output\\"
	}

	filter "platforms:x86"	
		targetsuffix "32"

	filter "platforms:x64"	
		targetsuffix "64"