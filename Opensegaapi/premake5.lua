project "Opensegaapi"
	targetname "Opensegaapi"
	language "C++"
	kind "SharedLib"

	files
	{
		"src/**.cpp", "src/**.h",
		"deps/cpp/**.cpp", "deps/inc/**.h"
	}

	includedirs { "src" }

	postbuildcommands { }