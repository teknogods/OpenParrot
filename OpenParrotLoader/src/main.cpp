#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <windows.h>
#include <winternl.h>
#include <conio.h>
#include "PE.h"
#include <string>
#include "Utils.h"
#include <filesystem>

#include <fcntl.h>
#include <io.h>

#pragma comment(lib, "ntdll.lib")
#pragma optimize("", off)
STARTUPINFO si;
PROCESS_INFORMATION pi;
_CONTEXT mycontext;
PEStruct FilePEFile;
int RunTo(DWORD_PTR Address, DWORD Mode, DWORD_PTR Eip);
#pragma comment (lib, "Advapi32.lib")

#if _M_IX86
char* LoaderExe = "OpenParrotLoader.exe";
#else
char* LoaderExe = "OpenParrotLoader64.exe";
#endif

int LoadHookDLL(const wchar_t* dllLocation, DWORD_PTR address)
{
	HMODULE kernel32Handle = GetModuleHandle(L"kernel32.dll");

	if (kernel32Handle == NULL)
	{
		wprintf(L"Failed to Load DLL! (Error 1)\n");
		return 0;
	}

	DWORD_PTR MyLoadLibraryW = (DWORD_PTR)GetProcAddress(kernel32Handle, "LoadLibraryW");
	DWORD_PTR addy = (DWORD_PTR)VirtualAllocEx(pi.hProcess, 0, 0x1000, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);

	if (addy == NULL)
	{
		wprintf(L"Failed to Load DLL! (Error 2)\n");
		return 0;
	}

	DWORD_PTR OEP = address;
	DWORD_PTR TextLocation = addy + 0x30;
	DWORD_PTR CallLoadLibraryW = MyLoadLibraryW - (addy + 10);
#ifdef _M_IX86
	DWORD addyOffset = 10;

	WriteProcessMemory(pi.hProcess, (LPVOID)addy, "\x68\x00\x00\x00\x00\xE8\x00\x00\x00\x00\xEB\xFE", 0x0C, 0);
	WriteProcessMemory(pi.hProcess, (LPVOID)(addy + 1), &TextLocation, 4, 0);
	WriteProcessMemory(pi.hProcess, (LPVOID)(addy + 6), &CallLoadLibraryW, 4, 0);
	WriteProcessMemory(pi.hProcess, (LPVOID)TextLocation, dllLocation, GetByteSizeOfWchar(dllLocation), 0);
#else
	DWORD addyOffset = 30;

	WriteProcessMemory(pi.hProcess, (LPVOID)addy, "\x48\x83\xEC\x28\x48\xB9\x00\x00\x00\x00\x00\x00\x00\x00\x48\xB8\x11\x11\x11\x11\x01\x00\x00\x00\xFF\xD0\x48\x83\xC4\x28\xEB\xFE", 32, 0);
	WriteProcessMemory(pi.hProcess, (LPVOID)(addy + 6), &TextLocation, 8, 0);
	WriteProcessMemory(pi.hProcess, (LPVOID)(addy + 16), &MyLoadLibraryW, 8, 0);
	WriteProcessMemory(pi.hProcess, (LPVOID)TextLocation, dllLocation, GetByteSizeOfWchar(dllLocation) + 1, 0);
#endif

	GetThreadContext(pi.hThread, &mycontext);
	Sleep(1000);

	if (!RunTo(addy + addyOffset, 0, addy))
	{
		wprintf(L"Failed to Load DLL! (Error 3)\n");
		return 0;
	}

#ifdef _M_IX86
	if (mycontext.Eax == 0)
#elif defined(_M_AMD64)
	if (mycontext.Rax == 0)
#endif
	{
		wprintf(L"Failed to Load DLL! (Error 4)\n");
		return 0;
	}

	GetThreadContext(pi.hThread, &mycontext);
	Sleep(100);
#ifdef _M_IX86
	mycontext.Eip = OEP;
#else
	mycontext.Rip = OEP;
#endif
	Sleep(100);
	SetThreadContext(pi.hThread, &mycontext);

	return 1;
}
int RunTo(DWORD_PTR Address, DWORD Mode, DWORD_PTR Eip)
{
	char tempbuf[4] = { 0 };
	if (Eip != 0)
	{
		GetThreadContext(pi.hThread, &mycontext);
#ifdef _M_IX86
		mycontext.Eip = Eip;
#elif defined(_M_AMD64)
		mycontext.Rip = Eip;
#endif
		SetThreadContext(pi.hThread, &mycontext);
	}
	ReadProcessMemory(pi.hProcess, (LPVOID)Address, tempbuf, 4, 0);
	WriteProcessMemory(pi.hProcess, (LPVOID)Address, "\xEB\xFE", 2, 0);
	ResumeThread(pi.hThread);
	while (GetThreadContext(pi.hThread, &mycontext))
	{
		if (Mode == 1) WriteProcessMemory(pi.hProcess, (LPVOID)Address, "\xEB\xFE", 2, 0);
#ifdef _M_IX86
		if (mycontext.Eip == Address) break;
#elif defined(_M_AMD64)
		if (mycontext.Rip == Address) break;
#endif
		Sleep(100);
	}
	SuspendThread(pi.hThread);
	if (!GetThreadContext(pi.hThread, &mycontext)) return 0;
	WriteProcessMemory(pi.hProcess, (LPVOID)Address, tempbuf, 4, 0);
	return 1;
}


static void loadTeknoIoDll(std::filesystem::path gamePathTP)
{
	std::string currentPath = std::filesystem::current_path().string();
	std::string sdl2Path = currentPath + "\\libs\\sdl2.dll";
	bool sdlLoaded = false;
	if (!std::filesystem::exists(sdl2Path))
	{
		return;
	}
	std::string teknoioPath = currentPath + "\\libs\\teknoio.dll";
	if (!std::filesystem::exists(teknoioPath))
	{
		return;
	}
	HMODULE sdl2 = LoadLibraryA(sdl2Path.c_str());
	if (sdl2 == NULL)
	{
		wprintf(L"Problem loading SDL2.dll\n");
		return;
	}
	HMODULE teknoio = LoadLibraryA(teknoioPath.c_str());
	if (teknoio == NULL)
	{
		wprintf(L"Problem loading TeknoIO.dll\n");
		return;
	}

	//pass the game directory into the teknoio.dll
	typedef void (*TeknoIoInit)(const char* gameDir);
	TeknoIoInit teknoIoInit = (TeknoIoInit)GetProcAddress(teknoio, "TeknoIoInit");
	if (teknoIoInit == NULL)
	{
		wprintf(L"Problem loading TeknoIoInit\n");
		return;
	}
	//pass in gamePathTP
	teknoIoInit(gamePathTP.string().c_str());
	wprintf(L"Loaded TeknoIO - FFB + Outputs are GO!\n");
}

int wmain(int argc, wchar_t* argv[])
{
	// Set stdout to wide chars, as a result you can only use wprintf!
	(void)_setmode(_fileno(stdout), _O_U16TEXT);

	wprintf(L"  _______   _                _____                     _   \n");
	wprintf(L" |__   __| | |              |  __ \\                   | |  \n");
	wprintf(L"    | | ___| | ___ __   ___ | |__) |_ _ _ __ _ __ ___ | |_ \n");
	wprintf(L"    | |/ _ \\ |/ / '_ \\ / _ \\|  ___/ _` | '__| '__/ _ \\| __|\n");
	wprintf(L"    | |  __/   <| | | | (_) | |  | (_| | |  | | | (_) | |_ \n");
	wprintf(L"    |_|\\___|_|\\_\\_| |_|\\___/|_|   \\__,_|_|  |_|  \\___/ \\__|\n\n");

	if (argc == 1 || argc > 4)
	{
		wprintf(L"Please use the following format:\n");
#if _M_IX86
		wprintf(L"OpenParrotLoader.exe <DLL> <EXE> <param>\n");
#else
		wprintf(L"OpenParrotLoader64.exe <DLL> <EXE> <param>\n");
#endif
		(void)_getch();
		return 0;
	}

	// Parse paths
	std::filesystem::path loaderPath;
	std::filesystem::path corePath;
	std::filesystem::path gamePath;

	loaderPath = std::filesystem::absolute(LoaderExe);
	corePath = std::filesystem::absolute(argv[1]);
	corePath += ".dll";
	gamePath = argv[2];

	// Prepare wchars
	wchar_t* loaderPathW = new wchar_t[wcslen(loaderPath.wstring().c_str()) + 1]{ 0 };
	wcscpy(loaderPathW, loaderPath.wstring().c_str());

	wchar_t* corePathW = new wchar_t[wcslen(corePath.wstring().c_str()) + 1]{ 0 };
	wcscpy(corePathW, corePath.wstring().c_str());

	wchar_t* gamePathW = new wchar_t[wcslen(gamePath.wstring().c_str()) + 1]{ 0 };
	wcscpy(gamePathW, gamePath.wstring().c_str());

	wchar_t* gameFolderW = new wchar_t[wcslen(gamePath.parent_path().wstring().c_str()) + 1]{ 0 };
	wcscpy(gameFolderW, gamePath.parent_path().wstring().c_str());

	// Print paths
	wprintf(L"Loader: %ls (%ls)\n", loaderPathW, GetFileVersion(loaderPathW));
	wprintf(L"Core:   %ls (%ls)\n", corePathW, GetFileVersion(corePathW));
	wprintf(L"Game:   %ls (%ls)\n", gamePathW, GetFileVersion(gamePathW));

	loadTeknoIoDll(gamePath);

	if (argc == 4)
		wprintf(L"Arguments: %ls\n", argv[3]);

	// Check paths
	if (!std::filesystem::exists(corePath))
	{
		wprintf(L"Unable to find core DLL!\n");
		(void)_getch();
		return 0;
	}

	if (!std::filesystem::exists(gamePath))
	{
		wprintf(L"Unable to find game EXE!\n");
		(void)_getch();
		return 0;
	}

	wprintf(L"\nLoading game...\n");

	FilePEFile = getPEFileInformation(gamePathW);

	// With arguments
	if (argc == 4)
	{
		size_t cmdSize = wcslen(gamePathW) + wcslen(argv[3]) + 4;
		wchar_t* cmdW = new wchar_t[cmdSize]{ 0 };
		swprintf(cmdW, cmdSize, L"\"%ls\" %ls", gamePathW, argv[3]);

		if (!CreateProcess(gamePathW, // No module name (use command line). 
			cmdW,			  // Command line.
			NULL,             // Process handle not inheritable. 
			NULL,             // Thread handle not inheritable. 
			FALSE,            // Set handle inheritance to FALSE. 
			CREATE_SUSPENDED | CREATE_NEW_PROCESS_GROUP, // suspended creation flags. 
			NULL,             // Use parent's environment block. 
			gameFolderW,      // Use parent's starting directory. 
			&si,              // Pointer to STARTUPINFO structure.
			&pi)              // Pointer to PROCESS_INFORMATION structure.
			)
		{
			wprintf(L"Failed to load process!\n");
			wprintf(L"Error: %ls(0x%X)\n", GetLastErrorAsString(), GetLastError());
			(void)_getch();
			return 1;
		}
	}
	// Without arguments
	else if (argc == 3)
	{
		if (!CreateProcess(gamePathW, // No module name (use command line). 
			L"",			  // Command line.
			NULL,             // Process handle not inheritable. 
			NULL,             // Thread handle not inheritable. 
			FALSE,            // Set handle inheritance to FALSE. 
			CREATE_SUSPENDED | CREATE_NEW_PROCESS_GROUP, // suspended creation flags. 
			NULL,             // Use parent's environment block. 
			gameFolderW,      // Use parent's starting directory. 
			&si,              // Pointer to STARTUPINFO structure.
			&pi)              // Pointer to PROCESS_INFORMATION structure.
			)
		{
			wprintf(L"Failed to load process!\n");
			wprintf(L"Error: %ls(0x%X)\n", GetLastErrorAsString(), GetLastError());
			(void)_getch();
			return 1;
		}
	}

	mycontext.ContextFlags = 0x00010000 + 1 + 2 + 4 + 8 + 0x10;
	GetThreadContext(pi.hThread, &mycontext);

	PROCESS_BASIC_INFORMATION pbi = { 0 };
	DWORD pbiSize = sizeof(pbi);

	if (!NT_SUCCESS(NtQueryInformationProcess(pi.hProcess, ProcessBasicInformation, &pbi, pbiSize, &pbiSize)))
	{
		wprintf(L"Failed to get process information!\n");
		(void)_getch();
		return 1;
	}

	DWORD_PTR baseAddress = 0;
	SIZE_T read = 0;

	ReadProcessMemory(pi.hProcess, (void*)((DWORD_PTR)pbi.PebBaseAddress + (sizeof(DWORD_PTR) * 2)), &baseAddress, sizeof(baseAddress), &read);

	if (read != sizeof(DWORD_PTR))
	{
		wprintf(L"Failed to get process environment!\n");
		(void)_getch();
		return 1;
	}

	Sleep(1000);

	if (!RunTo(baseAddress + FilePEFile.image_nt_headers.OptionalHeader.AddressOfEntryPoint, 1, 0))
	{
		wprintf(L"Failed to run the process\n");
		TerminateProcess(pi.hProcess, 0);
		(void)_getch();
		return 1;
	}
	wprintf(L"Success!\n");

	wprintf(L"Loading core...\n");

	if (!LoadHookDLL(corePathW, baseAddress + FilePEFile.image_nt_headers.OptionalHeader.AddressOfEntryPoint))
	{
		TerminateProcess(pi.hProcess, 0);
		(void)_getch();
		return 0;
	}
	wprintf(L"Success!\n");

	wprintf(L"\nHave fun :)\n");

	Sleep(2000);
	ResumeThread(pi.hThread);
	while (GetThreadContext(pi.hThread, &mycontext)) Sleep(2000);
	DWORD lpExitCode = 1;
	::GetExitCodeThread(pi.hThread, &lpExitCode);

	return lpExitCode;
}
