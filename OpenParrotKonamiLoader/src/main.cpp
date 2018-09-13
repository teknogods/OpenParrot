#include <Windows.h>
#include "libavs-win32.h"
#include "libacio.h"
#include <MinHook.h>

typedef signed(__stdcall *dll_entry_main)();
typedef signed(__stdcall *dll_entry_init)();

#include <stdio.h>
#include <iostream>
int __cdecl bt_get_ikey_status(unsigned char *a1)
{
	memset(a1, 0, 28);
	a1[4] = 1;
	a1[6] = 1;
	a1[12] = -1;
	a1[20] = 1;
	a1[22] = 1;
	a1[28] = -1;
	return 0;
}

int bt_fcheck_main()
{
	return 0;
}

void bt_fcheck_init()
{
	// Do nothing
}

void bt_fcheck_finish()
{
	// Do nothing
}

HMODULE (WINAPI *OrigGetModuleHandleA)(
	_In_opt_ LPCTSTR lpModuleName
);

HMODULE WINAPI GetModuleHandleAWrap(
	_In_opt_ LPCTSTR lpModuleName
)
{
	if ((DWORD)lpModuleName == 0x00)
	{
		return OrigGetModuleHandleA(lpModuleName);
	}

	if(strlen(lpModuleName) != 7)
	{
		return OrigGetModuleHandleA(lpModuleName);
	}

	if(strcmp(lpModuleName, "kbt.dll") == 0)
	{
		return (HMODULE)0x8001;
	}

	if(strcmp(lpModuleName, "kld.dll") == 0)
	{
		return (HMODULE)0x8002;
	}

	return OrigGetModuleHandleA(lpModuleName);
}

FARPROC(WINAPI *OrigGetProcAddress)(
	_In_ HMODULE hModule,
	_In_ LPCSTR  lpProcName
	);

FARPROC WINAPI GetProcAddressWrap(
	_In_ HMODULE hModule,
	_In_ LPCSTR  lpProcName
)
{
	if(hModule == (HMODULE)0x8001)
	{
		// kbt.dll
		if(strcmp(lpProcName, "bt_get_ikey_status") == 0)
		{
			return (FARPROC)bt_get_ikey_status;
		}
	}

	if(hModule == (HMODULE)0x8002)
	{
		// kld.dll
		if(strcmp(lpProcName, "bt_fcheck_init") == 0)
		{
			return (FARPROC)bt_fcheck_init;
		}
		if(strcmp(lpProcName, "bt_fcheck_main") == 0)
		{
			return (FARPROC)bt_fcheck_main;
		}
		if (strcmp(lpProcName, "bt_fcheck_finish") == 0)
		{
			return (FARPROC)bt_fcheck_finish;
		}
	}

	return OrigGetProcAddress(hModule, lpProcName);
}

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
	init_libacioHooks();


	// Hook dongle stuff without needing any lame dlls
	MH_Initialize();
	MH_CreateHookApi(L"kernel32.dll", "GetModuleHandleA", GetModuleHandleAWrap, (void **)&OrigGetModuleHandleA);
	MH_CreateHookApi(L"kernel32.dll", "GetProcAddress", GetProcAddressWrap, (void **)&OrigGetProcAddress);
	MH_EnableHook(MH_ALL_HOOKS);

	auto gameBinary = LoadLibraryA(argv[1]);

	dll_entry_main dllMain = (dll_entry_main)GetProcAddress(gameBinary, "dll_entry_main");

	dll_entry_init dllInit = (dll_entry_init)GetProcAddress(gameBinary, "dll_entry_init");

	dllInit();
	dllMain();

	while (true)
		Sleep(100);

	return 1;
}