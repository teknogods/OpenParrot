#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <windows.h>
#include <winternl.h>
#include <conio.h>
#include "PE.h"

#pragma comment(lib, "ntdll.lib")
#pragma optimize("", off)
STARTUPINFO si;
PROCESS_INFORMATION pi;
_CONTEXT mycontext;
PEStruct FilePEFile;
int RunTo(DWORD_PTR Address, DWORD Mode, DWORD_PTR Eip);
DWORD_PTR MyLoadLibraryA = 0;
#pragma comment (lib, "Advapi32.lib")
int LoadHookDLL(char *dllLocation, DWORD_PTR address)
{
	MyLoadLibraryA = (DWORD_PTR)GetProcAddress(GetModuleHandle("kernel32.dll"), "LoadLibraryA");
	DWORD_PTR addy = (DWORD_PTR)VirtualAllocEx(pi.hProcess, 0, 0x1000, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
	//printf("addy: %08X", addy);
	DWORD_PTR OEP = address;
	DWORD_PTR TextLocation = addy + 0x30;
	DWORD_PTR CallLoadLibraryA = MyLoadLibraryA - (addy + 10);
#ifdef _M_IX86
	DWORD addyOffset = 10;
	//ReadProcessMemory(pi.hProcess, (LPVOID)OEP, backbuf, 256, 0);
	WriteProcessMemory(pi.hProcess, (LPVOID)addy, "\x68\x00\x00\x00\x00\xE8\x00\x00\x00\x00\xEB\xFE", 0x0C, 0);
	WriteProcessMemory(pi.hProcess, (LPVOID)(addy + 1), &TextLocation, 4, 0);
	WriteProcessMemory(pi.hProcess, (LPVOID)(addy + 6), &CallLoadLibraryA, 4, 0);
	//printf("%s\n", dllLocation);
	WriteProcessMemory(pi.hProcess, (LPVOID)TextLocation, dllLocation, strlen(dllLocation)+1, 0);
#else
	DWORD addyOffset = 30;
	WriteProcessMemory(pi.hProcess, (LPVOID)addy, "\x48\x83\xEC\x28\x48\xB9\x00\x00\x00\x00\x00\x00\x00\x00\x48\xB8\x11\x11\x11\x11\x01\x00\x00\x00\xFF\xD0\x48\x83\xC4\x28\xEB\xFE", 32, 0);
	WriteProcessMemory(pi.hProcess, (LPVOID)(addy + 6), &TextLocation, 8, 0);
	WriteProcessMemory(pi.hProcess, (LPVOID)(addy + 16), &MyLoadLibraryA, 8, 0);
	//printf("%s\n", dllLocation);
	WriteProcessMemory(pi.hProcess, (LPVOID)TextLocation, dllLocation, strlen(dllLocation) + 1, 0);
#endif
	GetThreadContext(pi.hThread, &mycontext);
	Sleep(1000);
	if (!RunTo(addy + addyOffset, 0, addy))
	{
		printf("Failed to Load DLL!");
		return 0;
	}
#ifdef _M_IX86
	if (mycontext.Eax == 0)
#elif defined(_M_AMD64)
	if (mycontext.Rax == 0)
#endif
	{
		printf("Failed to Load DLL!");
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
	Sleep(100);
	//WriteProcessMemory(pi.hProcess, (LPVOID)OEP, backbuf, 256, 0);
	Sleep(100);
	printf("DLL Loaded!\n");// %08x\n", mycontext.Eax);
	return 1;
}
int RunTo(DWORD_PTR Address, DWORD Mode, DWORD_PTR Eip)
{
	char tempbuf[4];
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
void lowercase(char string[])
{
	int  i = 0;

	while (string[i])
	{
		string[i] = tolower(string[i]);
		i++;
	}

	return;
}

DWORD GetVal(HKEY hKey, LPCTSTR lpValue)
{
	DWORD data;		DWORD size = sizeof(data);	DWORD type = REG_DWORD;
	LONG nError = RegQueryValueEx(hKey, lpValue, NULL, &type, (LPBYTE)&data, &size);

	if (nError == ERROR_FILE_NOT_FOUND)
		data = 0; // The value will be created and set to data next time SetVal() is called.
	else if (nError)
		printf("Error: Could not get registry value %s", lpValue);

	return data;
}

int main(int argc, char *argv[])
{
	if (argc == 1 || argc > 4)
	{
		printf("Please use the following format: ");
#if _M_IX86
		printf("OpenParrotLoader.exe <DLL> c:\\games\\id6\\id6.exe <param>");
#else
		printf("OpenParrotLoader64.exe <DLL> c:\\games\\id6\\id6.exe <param>");
#endif		
		return 0;
	}

	char dirBuf[256];
	strcpy(dirBuf, argv[2]);
	char *p = strrchr(dirBuf, '\\');
	if (p) p[0] = 0;

	WIN32_FIND_DATA filedata;
	HANDLE hFind;
	HANDLE hFindDll;
	WIN32_FIND_DATA filedata2;
	HANDLE hFind2;
	char szDir[512];
	char szDir2[512];
	printf("OpenParrot Loader\n");
	printf("https://www.teknogods.com - Modding Gurus!\n");
	printf("Supported OS: Windows 7 x86 / x64, Windows 8 / 8.1 x86 / x64, Windows 10 x86 / x64\n");
	printf("Code: Reaver, NTAuthority, avail and the community\n");
	GetCurrentDirectoryA(400, szDir);
	sprintf_s(szDir2, "%s\\%s.dll", szDir, argv[1]);
	hFind = FindFirstFile(argv[2], &filedata);
	hFind2 = FindFirstFile(szDir2, &filedata2);

	if (hFind == INVALID_HANDLE_VALUE)
	{
		printf("Unable to find %s", argv[2]);
		_getch();
		return 0;
	}

	if (hFind2 == INVALID_HANDLE_VALUE)
	{
		printf("Unable to find %s", szDir2);
		_getch();
		return 0;
	}

	FilePEFile = getPEFileInformation(argv[2]);
	if (argc == 4)
	{
		char bb[256];
		sprintf(bb, "%s %s", argv[2], argv[3]);
		if (!CreateProcess(NULL, // No module name (use command line). 
			bb,			  // Command line.
			NULL,             // Process handle not inheritable. 
			NULL,             // Thread handle not inheritable. 
			FALSE,            // Set handle inheritance to FALSE. 
			CREATE_SUSPENDED | CREATE_NEW_PROCESS_GROUP, // suspended creation flags. 
			NULL,             // Use parent's environment block. 
			dirBuf,             // Use parent's starting directory. 
			&si,              // Pointer to STARTUPINFO structure.
			&pi)             // Pointer to PROCESS_INFORMATION structure.
			)
		{
			printf("Failed to load process!");
			_getch();
			return 1;
		}
	}
	else if (argc == 3)
	{
		if (!CreateProcess(argv[2], // No module name (use command line). 
			"",			  // Command line.
			NULL,             // Process handle not inheritable. 
			NULL,             // Thread handle not inheritable. 
			FALSE,            // Set handle inheritance to FALSE. 
			CREATE_SUSPENDED | CREATE_NEW_PROCESS_GROUP, // suspended creation flags. 
			NULL,             // Use parent's environment block. 
			dirBuf,             // Use parent's starting directory. 
			&si,              // Pointer to STARTUPINFO structure.
			&pi)             // Pointer to PROCESS_INFORMATION structure.
			)
		{
			printf("Failed to load process!");
			_getch();
			return 1;
		}
	}

	mycontext.ContextFlags = 0x00010000 + 1 + 2 + 4 + 8 + 0x10;
	GetThreadContext(pi.hThread, &mycontext);

	PROCESS_BASIC_INFORMATION pbi;
	DWORD pbiSize = sizeof(pbi);

	if (!NT_SUCCESS(NtQueryInformationProcess(pi.hProcess, ProcessBasicInformation, &pbi, pbiSize, &pbiSize)))
	{
		printf("Failed to get process information!");
		_getch();
		return 1;
	}

	DWORD_PTR baseAddress;
	SIZE_T read = 0;

	ReadProcessMemory(pi.hProcess, (void*)((DWORD_PTR)pbi.PebBaseAddress + (sizeof(DWORD_PTR) * 2)), &baseAddress, sizeof(baseAddress), &read);

	if (read != sizeof(DWORD_PTR))
	{
		printf("Failed to get process environment!");
		_getch();
		return 1;
	}

	Sleep(1000);

	if (!RunTo(baseAddress + FilePEFile.image_nt_headers.OptionalHeader.AddressOfEntryPoint, 1, 0))
	{
		printf("Failed to run the process\n");
		TerminateProcess(pi.hProcess, 0);
		_getch();
		return 1;
	}

	if (!LoadHookDLL(szDir2, baseAddress + FilePEFile.image_nt_headers.OptionalHeader.AddressOfEntryPoint))
	{
		TerminateProcess(pi.hProcess, 0);
		_getch();
		return 0;
	}
	printf("Resuming game!\n");
	Sleep(2000);
	ResumeThread(pi.hThread);
	while (GetThreadContext(pi.hThread, &mycontext)) Sleep(2000);
	return 1;
}
