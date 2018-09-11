#include <Windows.h>
#include "libavs-win32.h"

typedef signed(__stdcall *dll_entry_main)();
typedef signed(__stdcall *dll_entry_init)();

#include <stdio.h>
#include <iostream>

inline bool file_exists(const std::string& name)
{
	struct stat buffer;
	return (stat(name.c_str(), &buffer) == 0);
}

int main(int argc, char* argv[])
{
	if (argc < 2)
	{
		printf("ERROR: param needs to be target dll\n");
		printf("USAGE: OpenParrotKonamiLoader.exe path/to/some.dll\n");
		return 0;
	}

	if (!file_exists(argv[1]))
	{
		printf("ERROR: file does not exist.\n");
		return 0;
	}

	init_libavs();

	Sleep(1000);

	auto gameBinary = LoadLibraryA(argv[1]);

	dll_entry_main dllMain = (dll_entry_main)GetProcAddress(gameBinary, "dll_entry_main");

	dll_entry_init dllInit = (dll_entry_init)GetProcAddress(gameBinary, "dll_entry_init");

	dllInit();
	dllMain();

	while (true)
		Sleep(100);

	return 1;
}