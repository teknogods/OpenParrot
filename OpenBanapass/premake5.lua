project "OpenBanapass"
	targetname "bngrw"
	language "C++"
	kind "SharedLib"

	files
	{
		"src/**.cpp", "src/**.h",
		"deps/cpp/**.cpp", "deps/inc/**.h",
		"src/OpenParrotPass.rc"
	}

	includedirs { "src", }
	
	prebuildcommands {
		"if not exist $(TargetDir)output mkdir $(TargetDir)output",
	}
		
	postbuildcommands {
	  "if exist $(TargetDir)bngrw.dll xcopy /y $(TargetDir)bngrw.dll $(TargetDir)output\\",
	}