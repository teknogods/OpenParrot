#ifdef _M_AMD64
#include <StdInc.h>
#include <winternl.h>
#include "GameDetect.h"
#include "InitFunction.h"
#include "PE.h"
#pragma comment(lib, "ntdll.lib")

_CONTEXT mycontext;
DWORD_PTR MyLoadLibraryA = 0;
PEStruct FilePEFile;
EXTERN_C IMAGE_DOS_HEADER __ImageBase;

static int RunTo(LPPROCESS_INFORMATION lpProcessInformation, DWORD_PTR Address, DWORD Mode, DWORD_PTR Eip)
{
	char tempbuf[4];
	if (Eip != 0)
	{
		GetThreadContext(lpProcessInformation->hThread, &mycontext);
		mycontext.Rip = Eip;
		SetThreadContext(lpProcessInformation->hThread, &mycontext);
	}
	ReadProcessMemory(lpProcessInformation->hProcess, (LPVOID)Address, tempbuf, 4, 0);
	WriteProcessMemory(lpProcessInformation->hProcess, (LPVOID)Address, "\xEB\xFE", 2, 0);
	ResumeThread(lpProcessInformation->hThread);
	while (GetThreadContext(lpProcessInformation->hThread, &mycontext))
	{
		if (Mode == 1) WriteProcessMemory(lpProcessInformation->hProcess, (LPVOID)Address, "\xEB\xFE", 2, 0);
		if (mycontext.Rip == Address) break;
		Sleep(100);
	}
	SuspendThread(lpProcessInformation->hThread);
	if (!GetThreadContext(lpProcessInformation->hThread, &mycontext)) return 0;
	WriteProcessMemory(lpProcessInformation->hProcess, (LPVOID)Address, tempbuf, 4, 0);
	return 1;
}

static int LoadHookDLL(LPPROCESS_INFORMATION lpProcessInformation, char *dllLocation, DWORD_PTR address)
{
	MyLoadLibraryA = (DWORD_PTR)GetProcAddress(GetModuleHandleA("kernel32.dll"), "LoadLibraryA");
	DWORD_PTR addy = (DWORD_PTR)VirtualAllocEx(lpProcessInformation->hProcess, 0, 0x1000, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
	//printf("addy: %08X", addy);
	DWORD_PTR OEP = address;
	DWORD_PTR TextLocation = addy + 0x30;
	DWORD_PTR CallLoadLibraryA = MyLoadLibraryA - (addy + 10);
	DWORD addyOffset = 30;
	WriteProcessMemory(lpProcessInformation->hProcess, (LPVOID)addy, "\x48\x83\xEC\x28\x48\xB9\x00\x00\x00\x00\x00\x00\x00\x00\x48\xB8\x11\x11\x11\x11\x01\x00\x00\x00\xFF\xD0\x48\x83\xC4\x28\xEB\xFE", 32, 0);
	WriteProcessMemory(lpProcessInformation->hProcess, (LPVOID)(addy + 6), &TextLocation, 8, 0);
	WriteProcessMemory(lpProcessInformation->hProcess, (LPVOID)(addy + 16), &MyLoadLibraryA, 8, 0);
	//printf("%s\n", dllLocation);
	WriteProcessMemory(lpProcessInformation->hProcess, (LPVOID)TextLocation, dllLocation, strlen(dllLocation) + 1, 0);
	GetThreadContext(lpProcessInformation->hThread, &mycontext);
	Sleep(1000);
	if (!RunTo(lpProcessInformation, addy + addyOffset, 0, addy))
	{
		printf("Failed to Load DLL!");
		return 0;
	}
	if (mycontext.Rax == 0)
	{
		printf("Failed to Load DLL!");
		return 0;
	}
	GetThreadContext(lpProcessInformation->hThread, &mycontext);
	Sleep(100);
	mycontext.Rip = OEP;
	Sleep(100);
	SetThreadContext(lpProcessInformation->hThread, &mycontext);
	Sleep(100);
	//WriteProcessMemory(pi.hProcess, (LPVOID)OEP, backbuf, 256, 0);
	Sleep(100);
	return 1;
}

BOOL(WINAPI* g_origCreateProcessW)(LPCWSTR lpApplicationName,
	LPWSTR                lpCommandLine,
	LPSECURITY_ATTRIBUTES lpProcessAttributes,
	LPSECURITY_ATTRIBUTES lpThreadAttributes,
	BOOL                  bInheritHandles,
	DWORD                 dwCreationFlags,
	LPVOID                lpEnvironment,
	LPCWSTR               lpCurrentDirectory,
	LPSTARTUPINFOW        lpStartupInfo,
	LPPROCESS_INFORMATION lpProcessInformation);

char nonWideDll[256];
char currentDir[256];

BOOL WINAPI CreateProcessWHook(
	LPCWSTR               lpApplicationName,
	LPWSTR                lpCommandLine,
	LPSECURITY_ATTRIBUTES lpProcessAttributes,
	LPSECURITY_ATTRIBUTES lpThreadAttributes,
	BOOL                  bInheritHandles,
	DWORD                 dwCreationFlags,
	LPVOID                lpEnvironment,
	LPCWSTR               lpCurrentDirectory,
	LPSTARTUPINFOW        lpStartupInfo,
	LPPROCESS_INFORMATION lpProcessInformation
)
{
	memset(nonWideDll, 0, 256);
	memset(currentDir, 0, 256);
	GetCurrentDirectoryA(256, currentDir);
	sprintf(nonWideDll, "%s\\%ws", currentDir, lpCommandLine);
	for (int i = 0; i < 256; i++)
	{
		// In case no args
		if (nonWideDll[i] == 0)
		{
			break;
		}

		// Remove args from behind
		if (nonWideDll[i] == '.'
			&& nonWideDll[i + 1] == 'e'
			&& nonWideDll[i + 2] == 'x'
			&& nonWideDll[i + 3] == 'e')
		{
			nonWideDll[i + 4] = 0x00;
			break;
		}
	}
	FilePEFile = getPEFileInformation(nonWideDll);

	printf("%s", nonWideDll);
	printf("%08X", FilePEFile.image_nt_headers.OptionalHeader.AddressOfEntryPoint);
	auto myProc = g_origCreateProcessW(lpApplicationName, lpCommandLine, lpProcessAttributes, lpThreadAttributes, bInheritHandles, CREATE_SUSPENDED | CREATE_NEW_PROCESS_GROUP, lpEnvironment, lpCurrentDirectory, lpStartupInfo, lpProcessInformation);

	GetThreadContext(lpProcessInformation->hThread, &mycontext);

	mycontext.ContextFlags = 0x00010000 + 1 + 2 + 4 + 8 + 0x10;
	GetThreadContext(lpProcessInformation->hThread, &mycontext);

	PROCESS_BASIC_INFORMATION pbi;
	DWORD pbiSize = sizeof(pbi);

	if (!NT_SUCCESS(NtQueryInformationProcess(lpProcessInformation->hProcess, ProcessBasicInformation, &pbi, pbiSize, &pbiSize)))
	{
		MessageBoxA(0, "Failed to inject in to a new process!", "Error", 0);
		TerminateProcess(lpProcessInformation->hProcess, 0);
		ExitProcess(0);
	}

	DWORD_PTR baseAddress;
	SIZE_T read = 0;

	ReadProcessMemory(lpProcessInformation->hProcess, (void*)((DWORD_PTR)pbi.PebBaseAddress + (sizeof(DWORD_PTR) * 2)), &baseAddress, sizeof(baseAddress), &read);

	if (read != sizeof(DWORD_PTR))
	{
		MessageBoxA(0, "Failed to get process environment!", "Error", 0);
		TerminateProcess(lpProcessInformation->hProcess, 0);
		ExitProcess(0);
	}

	Sleep(1000);

	if (!RunTo(lpProcessInformation, baseAddress + FilePEFile.image_nt_headers.OptionalHeader.AddressOfEntryPoint, 1, 0))
	{
		MessageBoxA(0, "Failed to run the process", "Error", 0);
		TerminateProcess(lpProcessInformation->hProcess, 0);
		ExitProcess(0);
	}

	char DllPath[MAX_PATH] = { 0 };
	GetModuleFileNameA((HINSTANCE)&__ImageBase, DllPath, MAX_PATH);

	if (!LoadHookDLL(lpProcessInformation, (char *)DllPath, baseAddress + FilePEFile.image_nt_headers.OptionalHeader.AddressOfEntryPoint))
	{
		TerminateProcess(lpProcessInformation->hProcess, 0);
		ExitProcess(0);
	}
	Sleep(500);
	ResumeThread(lpProcessInformation->hThread);
}
#endif