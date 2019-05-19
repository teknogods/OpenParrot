project "iDmacDrv"
	targetname "iDmacDrv"
	language "C++"
	kind "SharedLib"

	files
	{
		"src/**.cpp", "src/**.h",
		"deps/cpp/**.cpp", "deps/inc/**.h",
		"src/iDmacDrv.rc"
	}

	includedirs { "src", }
	
	prebuildcommands {
		"if not exist $(TargetDir)output mkdir $(TargetDir)output",
	}
		
	postbuildcommands {
	  "if exist $(TargetDir)iDmacDrv32.dll xcopy /y $(TargetDir)iDmacDrv32.dll $(TargetDir)output\\",
	  "if exist $(TargetDir)iDmacDrv64.dll xcopy /y $(TargetDir)iDmacDrv64.dll $(TargetDir)output\\"
	}

	filter "platforms:x86"	
		targetsuffix "32"
		files { "Source32.def" }

	filter "platforms:x64"	
		targetsuffix "64"
		files { "Source64.def" }