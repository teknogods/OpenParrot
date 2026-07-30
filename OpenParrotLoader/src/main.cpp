#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <windows.h>
#include <winternl.h>
#include <TlHelp32.h>
#include <conio.h>
#include "PE.h"
#include <iterator>
#include <string>
#include "Utils.h"
#include <filesystem>

#include <fcntl.h>
#include <io.h>
#include <Shlwapi.h>

#pragma comment(lib, "ntdll.lib")
#pragma comment(lib, "Shlwapi.lib")
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

static bool ShouldUseRemoteThread()
{
	wchar_t envVar[256] = { 0 };
	DWORD result = GetEnvironmentVariable(
		L"TP_REMOTETHREAD", envVar, static_cast<DWORD>(std::size(envVar)));
	return (result > 0);
}

static DWORD PostStartRemoteThreadDelay()
{
	wchar_t envVar[32] = { 0 };
	DWORD result = GetEnvironmentVariable(
		L"TP_POSTSTART_REMOTETHREAD_MS", envVar,
		static_cast<DWORD>(std::size(envVar)));
	if (result == 0 || result >= std::size(envVar))
		return 0;

	wchar_t* end = nullptr;
	unsigned long delay = wcstoul(envVar, &end, 10);
	if (end == envVar || *end != L'\0')
		return 0;
	if (delay < 1)
		delay = 1;
	if (delay > 5000)
		delay = 5000;
	return static_cast<DWORD>(delay);
}

static DWORD EntryPointRemoteThreadDelay()
{
	wchar_t envVar[32] = { 0 };
	DWORD result = GetEnvironmentVariable(
		L"TP_ENTRYPOINT_REMOTETHREAD_MS", envVar,
		static_cast<DWORD>(std::size(envVar)));
	if (result == 0 || result >= std::size(envVar))
		return 0;

	wchar_t* end = nullptr;
	unsigned long delay = wcstoul(envVar, &end, 10);
	if (end == envVar || *end != L'\0')
		return 0;
	if (delay < 100)
		delay = 100;
	if (delay > 10000)
		delay = 10000;
	return static_cast<DWORD>(delay);
}

static bool ShouldUseLoaderManagedInit()
{
	wchar_t envVar[256] = { 0 };
	DWORD result = GetEnvironmentVariable(
		L"TP_LOADER_MANAGED_INIT", envVar,
		static_cast<DWORD>(std::size(envVar)));
	return result > 0;
}

static FARPROC ResolveRemoteProcedureAddress(DWORD processId, FARPROC localAddress)
{
	if (localAddress == nullptr)
		return nullptr;

	MEMORY_BASIC_INFORMATION memoryInfo = {};
	if (VirtualQuery(reinterpret_cast<LPCVOID>(localAddress), &memoryInfo,
		sizeof(memoryInfo)) == 0 || memoryInfo.AllocationBase == nullptr)
	{
		wprintf(L"Failed to resolve local procedure owner (0x%X)\n", GetLastError());
		return nullptr;
	}

	HMODULE localModule = static_cast<HMODULE>(memoryInfo.AllocationBase);
	wchar_t localModulePath[MAX_PATH] = {};
	if (GetModuleFileNameW(localModule, localModulePath,
		static_cast<DWORD>(std::size(localModulePath))) == 0)
	{
		wprintf(L"Failed to resolve local procedure module (0x%X)\n", GetLastError());
		return nullptr;
	}

	const wchar_t* localModuleName = PathFindFileNameW(localModulePath);
	const uintptr_t procedureOffset = reinterpret_cast<uintptr_t>(localAddress) -
		reinterpret_cast<uintptr_t>(localModule);

	FARPROC remoteAddress = nullptr;
	for (unsigned int attempt = 0; attempt < 100 && remoteAddress == nullptr; ++attempt)
	{
		HANDLE snapshot = CreateToolhelp32Snapshot(
			TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, processId);
		if (snapshot != INVALID_HANDLE_VALUE)
		{
			MODULEENTRY32W moduleEntry = {};
			moduleEntry.dwSize = sizeof(moduleEntry);
			if (Module32FirstW(snapshot, &moduleEntry))
			{
				do
				{
					if (_wcsicmp(moduleEntry.szModule, localModuleName) == 0)
					{
						remoteAddress = reinterpret_cast<FARPROC>(
							reinterpret_cast<uintptr_t>(moduleEntry.modBaseAddr) +
							procedureOffset);
						break;
					}
				} while (Module32NextW(snapshot, &moduleEntry));
			}
			CloseHandle(snapshot);
		}

		if (remoteAddress == nullptr)
		{
			if (WaitForSingleObject(pi.hProcess, 0) != WAIT_TIMEOUT)
				break;
			Sleep(25);
		}
	}

	if (remoteAddress == nullptr)
	{
		// Standard x86 RemoteThread mode deliberately keeps the target's
		// primary thread suspended until OpenParrot has been injected. Under
		// Wine/Box86, Toolhelp cannot enumerate kernel32 in that state even
		// though Wine maps its system DLLs at the same addresses in both x86
		// processes. Preserve the long-standing x86 loader behavior as a
		// narrow fallback; arbitrary DLL exports must still be resolved from
		// the target module list.
#if defined(_M_IX86)
		if (_wcsicmp(localModuleName, L"kernel32.dll") == 0)
		{
			wprintf(L"Target kernel32 is not enumerable while suspended; "
				L"using the Wine x86 local procedure address %p.\n",
				localAddress);
			return localAddress;
		}
#endif
		wprintf(L"Failed to find %ls in target process %lu\n",
			localModuleName, processId);
		HANDLE snapshot = CreateToolhelp32Snapshot(
			TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, processId);
		if (snapshot != INVALID_HANDLE_VALUE)
		{
			MODULEENTRY32W moduleEntry = {};
			moduleEntry.dwSize = sizeof(moduleEntry);
			if (Module32FirstW(snapshot, &moduleEntry))
			{
				wprintf(L"Target modules visible to Toolhelp:\n");
				do
				{
					wprintf(L"  %ls at %p\n", moduleEntry.szModule,
						moduleEntry.modBaseAddr);
				} while (Module32NextW(snapshot, &moduleEntry));
			}
			CloseHandle(snapshot);
		}
		return nullptr;
	}

	wprintf(L"Resolved target %ls procedure: local=%p remote=%p rva=0x%llX\n",
		localModuleName, localAddress, remoteAddress,
		static_cast<unsigned long long>(procedureOffset));
	return remoteAddress;
}

int LoadHookDLL(const wchar_t* dllLocation, DWORD_PTR address)
{
	HMODULE kernel32Handle = GetModuleHandle(L"kernel32.dll");

	if (kernel32Handle == NULL)
	{
		wprintf(L"Failed to Load DLL! (Error 1)\n");
		return 0;
	}

	FARPROC localLoadLibraryW = GetProcAddress(kernel32Handle, "LoadLibraryW");
	FARPROC remoteLoadLibraryW = ResolveRemoteProcedureAddress(
		pi.dwProcessId, localLoadLibraryW);
	if (remoteLoadLibraryW == nullptr)
	{
		wprintf(L"Failed to Load DLL! (Error 1b)\n");
		return 0;
	}
	DWORD_PTR MyLoadLibraryW = reinterpret_cast<DWORD_PTR>(remoteLoadLibraryW);
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

int LoadHookDLLRemoteThread(const wchar_t* dllLocation)
{
	HMODULE kernel32Handle = GetModuleHandle(L"kernel32.dll");

	if (kernel32Handle == nullptr)
	{
		wprintf(L"Failed to Load DLL via RemoteThread! (Error 1)\n");
		return 0;
	}

	FARPROC localLoadLibraryW = GetProcAddress(kernel32Handle, "LoadLibraryW");
	if (localLoadLibraryW == nullptr)
	{
		wprintf(L"Failed to Load DLL via RemoteThread! (Error 2)\n");
		return 0;
	}

	FARPROC remoteLoadLibraryW = ResolveRemoteProcedureAddress(
		pi.dwProcessId, localLoadLibraryW);
	if (remoteLoadLibraryW == nullptr)
	{
		wprintf(L"Failed to Load DLL via RemoteThread! (Error 2b)\n");
		return 0;
	}

	// In case of stubborn executables, might not need this anymore?
	HANDLE processHandle = OpenProcess(PROCESS_VM_OPERATION | PROCESS_VM_WRITE | PROCESS_CREATE_THREAD | PROCESS_QUERY_INFORMATION, FALSE, pi.dwProcessId);
	if (processHandle == nullptr)
	{
		wprintf(L"Failed to Load DLL via RemoteThread! (Error 3 - OpenProcess failed: 0x%X)\n", GetLastError());
		return 0;
	}

	size_t dllPathSize = (wcslen(dllLocation) + 1) * sizeof(wchar_t);
	LPVOID remoteDllPath = VirtualAllocEx(processHandle, nullptr, dllPathSize, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);

	if (remoteDllPath == nullptr)
	{
		wprintf(L"Failed to Load DLL via RemoteThread! (Error 4 - VirtualAllocEx failed: 0x%X)\n", GetLastError());
		CloseHandle(processHandle);
		return 0;
	}

	if (!WriteProcessMemory(processHandle, remoteDllPath, dllLocation, dllPathSize, nullptr))
	{
		wprintf(L"Failed to Load DLL via RemoteThread! (Error 5 - WriteProcessMemory failed: 0x%X)\n", GetLastError());
		VirtualFreeEx(processHandle, remoteDllPath, 0, MEM_RELEASE);
		CloseHandle(processHandle);
		return 0;
	}

	HANDLE remoteThread = CreateRemoteThread(processHandle, nullptr, 0,
		reinterpret_cast<LPTHREAD_START_ROUTINE>(remoteLoadLibraryW),
		remoteDllPath, 0, NULL);

	if (remoteThread == nullptr)
	{
		wprintf(L"Failed to Load DLL via RemoteThread! (Error 6 - CreateRemoteThread failed: 0x%X)\n", GetLastError());
		VirtualFreeEx(processHandle, remoteDllPath, 0, MEM_RELEASE);
		CloseHandle(processHandle);
		return 0;
	}

	WaitForSingleObject(remoteThread, INFINITE);

	DWORD exitCode = 0;
	GetExitCodeThread(remoteThread, &exitCode);
	CloseHandle(remoteThread);
	VirtualFreeEx(processHandle, remoteDllPath, 0, MEM_RELEASE);
	CloseHandle(processHandle);

	if (exitCode == 0)
	{
		wprintf(L"Failed to Load DLL via RemoteThread! (Error 7 - LoadLibraryW returned NULL)\n");
		return 0;
	}

	return 1;
}

static FARPROC ResolveRemoteDllExport(
	const wchar_t* dllLocation, const char* exportName)
{
	HMODULE localModule = LoadLibraryExW(
		dllLocation, nullptr, DONT_RESOLVE_DLL_REFERENCES);
	if (localModule == nullptr)
	{
		wprintf(L"Failed to inspect remote DLL export! (Error 1 - 0x%X)\n",
			GetLastError());
		return nullptr;
	}

	FARPROC localExport = GetProcAddress(localModule, exportName);
	if (localExport == nullptr)
	{
		wprintf(L"Failed to find remote DLL export! (Error 2 - 0x%X)\n",
			GetLastError());
		FreeLibrary(localModule);
		return nullptr;
	}

	FARPROC remoteExport = ResolveRemoteProcedureAddress(
		pi.dwProcessId, localExport);
	FreeLibrary(localModule);
	if (remoteExport == nullptr)
	{
		wprintf(L"Failed to resolve remote DLL export! (Error 3)\n");
		return nullptr;
	}
	return remoteExport;
}

static int InstallEntryPointInitializer(const wchar_t* dllLocation,
	LPVOID entryPoint, const BYTE* originalEntryPoint)
{
#ifdef _M_AMD64
	FARPROC remoteInitialize = ResolveRemoteDllExport(
		dllLocation, "InitializeASI");
	if (remoteInitialize == nullptr)
		return 0;

	DWORD oldProtect = 0;
	if (!VirtualProtectEx(pi.hProcess, entryPoint, 20,
		PAGE_EXECUTE_READWRITE, &oldProtect))
	{
		wprintf(L"Failed to make parked entry point writable! (Error 4 - 0x%X)\n",
			GetLastError());
		return 0;
	}

	// Run InitializeASI on the game's primary thread, restore the 20 original
	// entry-point bytes in place, then jump back to the real entry point. This
	// avoids Wine/Box64's unreliable arbitrary-procedure CreateRemoteThread path.
	BYTE trampoline[] = {
		0x48, 0x83, 0xEC, 0x28,                         // sub rsp, 28h
		0x48, 0xB8, 0, 0, 0, 0, 0, 0, 0, 0,           // mov rax, InitializeASI
		0xFF, 0xD0,                                     // call rax
		0x48, 0x83, 0xC4, 0x28,                         // add rsp, 28h
		0x48, 0xB8, 0, 0, 0, 0, 0, 0, 0, 0,           // mov rax, entry point
		0x48, 0xBA, 0, 0, 0, 0, 0, 0, 0, 0,           // mov rdx, original[0..7]
		0x48, 0x89, 0x10,                               // mov [rax], rdx
		0x48, 0xBA, 0, 0, 0, 0, 0, 0, 0, 0,           // mov rdx, original[8..15]
		0x48, 0x89, 0x50, 0x08,                         // mov [rax+8], rdx
		0x41, 0xBA, 0, 0, 0, 0,                         // mov r10d, original[16..19]
		0x44, 0x89, 0x50, 0x10,                         // mov [rax+16], r10d
		0xFF, 0xE0                                      // jmp rax
	};
	static_assert(sizeof(trampoline) == 69, "Unexpected entry trampoline size");

	DWORD_PTR initializeAddress = reinterpret_cast<DWORD_PTR>(remoteInitialize);
	DWORD_PTR entryPointAddress = reinterpret_cast<DWORD_PTR>(entryPoint);
	memcpy(&trampoline[6], &initializeAddress, sizeof(initializeAddress));
	memcpy(&trampoline[22], &entryPointAddress, sizeof(entryPointAddress));
	memcpy(&trampoline[32], originalEntryPoint, 8);
	memcpy(&trampoline[45], originalEntryPoint + 8, 8);
	memcpy(&trampoline[59], originalEntryPoint + 16, 4);

	LPVOID remoteTrampoline = VirtualAllocEx(pi.hProcess, nullptr, 0x1000,
		MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
	if (remoteTrampoline == nullptr)
	{
		wprintf(L"Failed to allocate entry-point initializer! (Error 5 - 0x%X)\n",
			GetLastError());
		return 0;
	}

	SIZE_T written = 0;
	if (!WriteProcessMemory(pi.hProcess, remoteTrampoline, trampoline,
		sizeof(trampoline), &written) || written != sizeof(trampoline))
	{
		wprintf(L"Failed to write entry-point initializer! (Error 6 - 0x%X)\n",
			GetLastError());
		return 0;
	}

	BYTE entryJump[] = {
		0x48, 0xB8, 0, 0, 0, 0, 0, 0, 0, 0,           // mov rax, trampoline
		0xFF, 0xE0                                      // jmp rax
	};
	DWORD_PTR trampolineAddress = reinterpret_cast<DWORD_PTR>(remoteTrampoline);
	memcpy(&entryJump[2], &trampolineAddress, sizeof(trampolineAddress));
	written = 0;
	if (!WriteProcessMemory(pi.hProcess, entryPoint, entryJump,
		sizeof(entryJump), &written) || written != sizeof(entryJump))
	{
		wprintf(L"Failed to install entry-point initializer! (Error 7 - 0x%X)\n",
			GetLastError());
		return 0;
	}

	FlushInstructionCache(pi.hProcess, remoteTrampoline, sizeof(trampoline));
	FlushInstructionCache(pi.hProcess, entryPoint, sizeof(entryJump));
	wprintf(L"Primary-thread OpenParrot initializer installed at %p.\n",
		remoteTrampoline);
	return 1;
#elif defined(_M_IX86)
	FARPROC remoteInitialize = ResolveRemoteDllExport(
		dllLocation, "InitializeASI");
	if (remoteInitialize == nullptr)
		return 0;

	DWORD oldProtect = 0;
	if (!VirtualProtectEx(pi.hProcess, entryPoint, 20,
		PAGE_EXECUTE_READWRITE, &oldProtect))
	{
		wprintf(L"Failed to make parked entry point writable! (Error 4 - 0x%X)\n",
			GetLastError());
		return 0;
	}

	// Preserve the complete x86 entry register state while InitializeASI runs,
	// restore the original 20 bytes, then jump back to the real entry point.
	// This gives Wine/Box86 time to publish kernel32 without allowing the game
	// to execute before OpenParrot has installed its hooks.
	BYTE trampoline[] = {
		0x9C,                                           // pushfd
		0x60,                                           // pushad
		0xB8, 0, 0, 0, 0,                               // mov eax, InitializeASI
		0xFF, 0xD0,                                     // call eax
		0xB8, 0, 0, 0, 0,                               // mov eax, entry point
		0xBA, 0, 0, 0, 0,                               // mov edx, original[0..3]
		0x89, 0x10,                                     // mov [eax], edx
		0xBA, 0, 0, 0, 0,                               // mov edx, original[4..7]
		0x89, 0x50, 0x04,                               // mov [eax+4], edx
		0xBA, 0, 0, 0, 0,                               // mov edx, original[8..11]
		0x89, 0x50, 0x08,                               // mov [eax+8], edx
		0xBA, 0, 0, 0, 0,                               // mov edx, original[12..15]
		0x89, 0x50, 0x0C,                               // mov [eax+12], edx
		0xBA, 0, 0, 0, 0,                               // mov edx, original[16..19]
		0x89, 0x50, 0x10,                               // mov [eax+16], edx
		0x61,                                           // popad
		0x9D,                                           // popfd
		0xE9, 0, 0, 0, 0                                // jmp entry point
	};
	static_assert(sizeof(trampoline) == 60, "Unexpected x86 entry trampoline size");

	DWORD initializeAddress = reinterpret_cast<DWORD>(remoteInitialize);
	DWORD entryPointAddress = reinterpret_cast<DWORD>(entryPoint);
	memcpy(&trampoline[3], &initializeAddress, sizeof(initializeAddress));
	memcpy(&trampoline[10], &entryPointAddress, sizeof(entryPointAddress));
	memcpy(&trampoline[15], originalEntryPoint, 4);
	memcpy(&trampoline[22], originalEntryPoint + 4, 4);
	memcpy(&trampoline[30], originalEntryPoint + 8, 4);
	memcpy(&trampoline[38], originalEntryPoint + 12, 4);
	memcpy(&trampoline[46], originalEntryPoint + 16, 4);

	LPVOID remoteTrampoline = VirtualAllocEx(pi.hProcess, nullptr, 0x1000,
		MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
	if (remoteTrampoline == nullptr)
	{
		wprintf(L"Failed to allocate entry-point initializer! (Error 5 - 0x%X)\n",
			GetLastError());
		return 0;
	}

	DWORD trampolineAddress = reinterpret_cast<DWORD>(remoteTrampoline);
	DWORD returnDisplacement = entryPointAddress -
		(trampolineAddress + static_cast<DWORD>(sizeof(trampoline)));
	memcpy(&trampoline[56], &returnDisplacement, sizeof(returnDisplacement));

	SIZE_T written = 0;
	if (!WriteProcessMemory(pi.hProcess, remoteTrampoline, trampoline,
		sizeof(trampoline), &written) || written != sizeof(trampoline))
	{
		wprintf(L"Failed to write entry-point initializer! (Error 6 - 0x%X)\n",
			GetLastError());
		return 0;
	}

	BYTE entryJump[] = { 0xE9, 0, 0, 0, 0 };
	DWORD entryDisplacement = trampolineAddress - (entryPointAddress + 5);
	memcpy(&entryJump[1], &entryDisplacement, sizeof(entryDisplacement));
	written = 0;
	if (!WriteProcessMemory(pi.hProcess, entryPoint, entryJump,
		sizeof(entryJump), &written) || written != sizeof(entryJump))
	{
		wprintf(L"Failed to install entry-point initializer! (Error 7 - 0x%X)\n",
			GetLastError());
		return 0;
	}

	FlushInstructionCache(pi.hProcess, remoteTrampoline, sizeof(trampoline));
	FlushInstructionCache(pi.hProcess, entryPoint, sizeof(entryJump));
	wprintf(L"Primary-thread x86 OpenParrot initializer installed at %p.\n",
		remoteTrampoline);
	return 1;
#else
	(void)dllLocation;
	(void)entryPoint;
	(void)originalEntryPoint;
	wprintf(L"Loader-managed entry-point initialization is unsupported.\n");
	return 0;
#endif
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
	wchar_t* loaderPathW = new wchar_t[wcslen(loaderPath.wstring().c_str()) + 1] { 0 };
	wcscpy(loaderPathW, loaderPath.wstring().c_str());

	wchar_t* corePathW = new wchar_t[wcslen(corePath.wstring().c_str()) + 1] { 0 };
	wcscpy(corePathW, corePath.wstring().c_str());

	wchar_t* gamePathW = new wchar_t[wcslen(gamePath.wstring().c_str()) + 1] { 0 };
	wcscpy(gamePathW, gamePath.wstring().c_str());

	std::wstring gameFolder = gamePath.parent_path().wstring();
	wchar_t requestedGameFolder[32768] = {};
	DWORD requestedGameFolderLength = GetEnvironmentVariableW(
		L"TP_GAME_WORKING_DIRECTORY", requestedGameFolder,
		static_cast<DWORD>(std::size(requestedGameFolder)));
	if (requestedGameFolderLength > 0 &&
		requestedGameFolderLength < std::size(requestedGameFolder))
	{
		DWORD requestedGameFolderAttributes = GetFileAttributesW(requestedGameFolder);
		if (requestedGameFolderAttributes != INVALID_FILE_ATTRIBUTES &&
			(requestedGameFolderAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0)
		{
			gameFolder = requestedGameFolder;
		}
	}
	wchar_t* gameFolderW = new wchar_t[gameFolder.length() + 1] { 0 };
	wcscpy(gameFolderW, gameFolder.c_str());

	// Print paths
	wprintf(L"Loader: %ls (%ls)\n", loaderPathW, GetFileVersion(loaderPathW));
	wprintf(L"Core:   %ls (%ls)\n", corePathW, GetFileVersion(corePathW));
	wprintf(L"Game:   %ls (%ls)\n", gamePathW, GetFileVersion(gamePathW));
	wprintf(L"Working directory: %ls\n", gameFolderW);

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

	DWORD postStartRemoteThreadDelay = PostStartRemoteThreadDelay();
	DWORD entryPointRemoteThreadDelay = EntryPointRemoteThreadDelay();
	bool loaderManagedInit = ShouldUseLoaderManagedInit();
	bool useRemoteThread = ShouldUseRemoteThread() ||
		postStartRemoteThreadDelay != 0 || entryPointRemoteThreadDelay != 0;
	if (useRemoteThread)
	{
		if (entryPointRemoteThreadDelay != 0)
			wprintf(L"Using parked-entry-point RemoteThread injection method (%lu ms)...\n",
				entryPointRemoteThreadDelay);
		else if (postStartRemoteThreadDelay != 0)
			wprintf(L"Using post-start RemoteThread injection method (%lu ms)...\n",
				postStartRemoteThreadDelay);
		else
			wprintf(L"Using RemoteThread injection method...\n");
	}
	if (!useRemoteThread || entryPointRemoteThreadDelay != 0) {
		FilePEFile = getPEFileInformation(gamePathW);
	}

	// With arguments
	if (argc == 4)
	{
		size_t cmdSize = wcslen(gamePathW) + wcslen(argv[3]) + 4;
		wchar_t* cmdW = new wchar_t[cmdSize] { 0 };
		swprintf(cmdW, cmdSize, L"\"%ls\" %ls", gamePathW, argv[3]);

		if (!CreateProcess(gamePathW, // No module name (use command line). 
			cmdW,			  // Command line.
			NULL,             // Process handle not inheritable. 
			NULL,             // Thread handle not inheritable. 
			FALSE,            // Set handle inheritance to FALSE. 
			CREATE_SUSPENDED | CREATE_NEW_PROCESS_GROUP,    // creation flags. 
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
			CREATE_SUSPENDED | CREATE_NEW_PROCESS_GROUP,    // creation flags. 
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

	DWORD_PTR baseAddress = 0;
	LPVOID parkedEntryPoint = nullptr;
	BYTE parkedOriginalEntryPoint[20] = {};
	bool entryPointParked = false;

	if (useRemoteThread)
	{
		if (entryPointRemoteThreadDelay != 0)
		{
			PROCESS_BASIC_INFORMATION pbi = { 0 };
			DWORD pbiSize = sizeof(pbi);
			if (!NT_SUCCESS(NtQueryInformationProcess(pi.hProcess,
				ProcessBasicInformation, &pbi, pbiSize, &pbiSize)))
			{
				wprintf(L"Failed to get process information for entry-point parking!\n");
				TerminateProcess(pi.hProcess, 0);
				return 1;
			}

			SIZE_T read = 0;
			ReadProcessMemory(pi.hProcess,
				reinterpret_cast<void*>(reinterpret_cast<DWORD_PTR>(pbi.PebBaseAddress) +
					(sizeof(DWORD_PTR) * 2)),
				&baseAddress, sizeof(baseAddress), &read);
			if (read != sizeof(baseAddress) || baseAddress == 0)
			{
				wprintf(L"Failed to get process image base for entry-point parking!\n");
				TerminateProcess(pi.hProcess, 0);
				return 1;
			}

			parkedEntryPoint = reinterpret_cast<LPVOID>(baseAddress +
				FilePEFile.image_nt_headers.OptionalHeader.AddressOfEntryPoint);
			read = 0;
			if (!ReadProcessMemory(pi.hProcess, parkedEntryPoint,
				parkedOriginalEntryPoint, sizeof(parkedOriginalEntryPoint), &read) ||
				read != sizeof(parkedOriginalEntryPoint))
			{
				wprintf(L"Failed to read game entry point for parking! (0x%X)\n",
					GetLastError());
				TerminateProcess(pi.hProcess, 0);
				return 1;
			}

			const BYTE spinLoop[2] = { 0xEB, 0xFE };
			SIZE_T written = 0;
			if (!WriteProcessMemory(pi.hProcess, parkedEntryPoint, spinLoop,
				sizeof(spinLoop), &written) || written != sizeof(spinLoop))
			{
				wprintf(L"Failed to park game entry point! (0x%X)\n", GetLastError());
				TerminateProcess(pi.hProcess, 0);
				return 1;
			}
			FlushInstructionCache(pi.hProcess, parkedEntryPoint, sizeof(spinLoop));
			entryPointParked = true;

			if (ResumeThread(pi.hThread) == static_cast<DWORD>(-1))
			{
				wprintf(L"Failed to start process for entry-point parking! (0x%X)\n",
					GetLastError());
				TerminateProcess(pi.hProcess, 0);
				return 1;
			}
			Sleep(entryPointRemoteThreadDelay);
			wprintf(L"Game entry point is parked; target modules are ready.\n");
		}
		else if (postStartRemoteThreadDelay != 0)
		{
			// Wine/Box64 must begin mapping and relocating large PE images before
			// LoadLibrary runs in a remote thread.  Do not suspend the primary thread
			// again: it may still own ntdll's loader lock.  CreateRemoteThread will
			// naturally wait behind that lock and load OpenParrot once relocation is
			// complete.
			if (ResumeThread(pi.hThread) == static_cast<DWORD>(-1))
			{
				wprintf(L"Failed to start process before RemoteThread injection! (0x%X)\n",
					GetLastError());
				TerminateProcess(pi.hProcess, 0);
				return 1;
			}
			Sleep(postStartRemoteThreadDelay);
		}
		else
		{
			// Standard RemoteThread mode injects while the primary thread remains
			// suspended, preserving the long-standing Windows/Linux behaviour.
			Sleep(1000);
		}
		wprintf(L"Success!\n");
	}
	else
	{
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
	}

	wprintf(L"Loading core...\n");

	WCHAR iniPath[MAX_PATH];
	wcscpy_s(iniPath, gamePathW);
	PathRemoveFileSpecW(iniPath);
	wcscat_s(iniPath, L"\\teknoparrot.ini");

	if (GetPrivateProfileInt(L"FFB Blaster", L"Enable", 0, iniPath))
	{
		std::filesystem::path workingDirectory = std::filesystem::current_path();
		std::filesystem::path ffbBlasterPath;

#ifdef _M_IX86
		ffbBlasterPath = workingDirectory / L"FFBBlaster" / L"x86" / L"FFBBlaster.dll";
#else
		ffbBlasterPath = workingDirectory / L"FFBBlaster" / L"x64" / L"FFBBlaster64.dll";
#endif

		wprintf(L"Looking for FFB Blaster at: %ls\n", ffbBlasterPath.wstring().c_str());

		if (std::filesystem::exists(ffbBlasterPath))
		{
			wchar_t* ffbBlasterPathW = new wchar_t[wcslen(ffbBlasterPath.wstring().c_str()) + 1] { 0 };
			wcscpy(ffbBlasterPathW, ffbBlasterPath.wstring().c_str());

			wprintf(L"FFB Blaster found: %ls\n", ffbBlasterPathW);

			bool ffbSuccess = false;
			if (useRemoteThread)
			{
				ffbSuccess = LoadHookDLLRemoteThread(ffbBlasterPathW);
			}
			else
			{
				ffbSuccess = LoadHookDLL(ffbBlasterPathW, baseAddress + FilePEFile.image_nt_headers.OptionalHeader.AddressOfEntryPoint);
			}

			if (ffbSuccess)
			{
				wprintf(L"FFB Blaster loaded successfully!\n");
			}
			else
			{
				wprintf(L"Failed to load FFB Blaster. Continuing with core DLL only...\n");
			}

			delete[] ffbBlasterPathW;
		}
		else
		{
			wprintf(L"FFB Blaster not found at: %ls\n", ffbBlasterPath.wstring().c_str());
		}
	}

	bool coreSuccess = false;
	if (useRemoteThread)
	{
		coreSuccess = LoadHookDLLRemoteThread(corePathW);
	}
	else
	{
		coreSuccess = LoadHookDLL(corePathW, baseAddress + FilePEFile.image_nt_headers.OptionalHeader.AddressOfEntryPoint);
	}

	if (!coreSuccess)
	{
		TerminateProcess(pi.hProcess, 0);
		(void)_getch();
		return 0;
	}

	// Load optional title-scoped compatibility code only after the core has
	// completed its normal LoadLibrary initialization. The target's primary
	// thread is still suspended (or parked) here, so the helper is active before
	// game code resumes without perturbing OpenParrot's startup ordering.
	wchar_t preloadValue[32768] = {};
	const DWORD preloadValueLength = GetEnvironmentVariableW(
		L"TP_PRELOAD_DLL", preloadValue,
		static_cast<DWORD>(std::size(preloadValue)));
	if (preloadValueLength > 0 &&
		preloadValueLength < std::size(preloadValue))
	{
		std::filesystem::path preloadPath = preloadValue;
		if (preloadPath.is_relative())
			preloadPath = std::filesystem::absolute(preloadPath);

		wprintf(L"Loading requested compatibility DLL: %ls\n",
			preloadPath.wstring().c_str());
		if (!std::filesystem::is_regular_file(preloadPath))
		{
			wprintf(L"Requested compatibility DLL does not exist.\n");
			TerminateProcess(pi.hProcess, 0);
			return 0;
		}

		const bool preloadSuccess = useRemoteThread
			? LoadHookDLLRemoteThread(preloadPath.wstring().c_str())
			: LoadHookDLL(
				preloadPath.wstring().c_str(),
				baseAddress +
					FilePEFile.image_nt_headers.OptionalHeader.AddressOfEntryPoint);
		if (!preloadSuccess)
		{
			wprintf(L"Failed to load requested compatibility DLL.\n");
			TerminateProcess(pi.hProcess, 0);
			return 0;
		}
		wprintf(L"Requested compatibility DLL loaded successfully.\n");
	}

	if (entryPointParked && loaderManagedInit)
	{
		// LoadLibrary has now returned, so Wine's target loader lock is free and
		// the primary thread is spinning at the real game entry point. Install a
		// primary-thread trampoline instead of starting another remote thread.
		if (SuspendThread(pi.hThread) == static_cast<DWORD>(-1))
		{
			wprintf(L"Failed to suspend parked game entry point! (0x%X)\n",
				GetLastError());
			TerminateProcess(pi.hProcess, 0);
			return 0;
		}

		if (!InstallEntryPointInitializer(corePathW, parkedEntryPoint,
			parkedOriginalEntryPoint))
		{
			TerminateProcess(pi.hProcess, 0);
			return 0;
		}
		entryPointParked = false;
	}

	wprintf(L"Success!\n");

	wprintf(L"\nHave fun :)\n");

	Sleep(2000);
	
	if (useRemoteThread)
	{
		if (postStartRemoteThreadDelay == 0)
			ResumeThread(pi.hThread);
		WaitForSingleObject(pi.hProcess, INFINITE);
	}
	else
	{
		ResumeThread(pi.hThread);
		while (GetThreadContext(pi.hThread, &mycontext)) Sleep(2000);
	}
	
	DWORD lpExitCode = 1;
	::GetExitCodeThread(pi.hThread, &lpExitCode);

	return lpExitCode;
}
