#ifdef _M_AMD64
#include <StdInc.h>
#include <winternl.h>
#include <TlHelp32.h>
#include "GameDetect.h"
#include "InitFunction.h"
#include "PE.h"
#pragma comment(lib, "ntdll.lib")

_CONTEXT mycontext;
DWORD_PTR MyLoadLibraryA = 0;
PEStruct FilePEFile;
EXTERN_C IMAGE_DOS_HEADER __ImageBase;
extern "C" void InitializeASI();

static bool ShouldUseChildPrimaryThreadInit()
{
	char value[8] = {};
	return GetEnvironmentVariableA(
		"TP_CHILD_PRIMARY_THREAD_INIT", value, sizeof(value)) > 0;
}

static FARPROC ResolveChildProcedureAddress(
	LPPROCESS_INFORMATION lpProcessInformation, FARPROC localAddress)
{
	if (localAddress == nullptr)
		return nullptr;

	MEMORY_BASIC_INFORMATION memoryInfo = {};
	if (VirtualQuery(reinterpret_cast<LPCVOID>(localAddress), &memoryInfo,
		sizeof(memoryInfo)) == 0 || memoryInfo.AllocationBase == nullptr)
	{
		printf("Failed to resolve child procedure owner (0x%lX).\n",
			GetLastError());
		return nullptr;
	}

	HMODULE localModule = static_cast<HMODULE>(memoryInfo.AllocationBase);
	wchar_t localModulePath[MAX_PATH] = {};
	if (GetModuleFileNameW(localModule, localModulePath, MAX_PATH) == 0)
	{
		printf("Failed to resolve child procedure module (0x%lX).\n",
			GetLastError());
		return nullptr;
	}

	const wchar_t* localModuleName = wcsrchr(localModulePath, L'\\');
	localModuleName = localModuleName == nullptr
		? localModulePath
		: localModuleName + 1;
	const uintptr_t procedureOffset =
		reinterpret_cast<uintptr_t>(localAddress) -
		reinterpret_cast<uintptr_t>(localModule);

	for (unsigned int attempt = 0; attempt < 200; ++attempt)
	{
		HANDLE snapshot = CreateToolhelp32Snapshot(
			TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32,
			lpProcessInformation->dwProcessId);
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
						FARPROC remoteAddress = reinterpret_cast<FARPROC>(
							reinterpret_cast<uintptr_t>(moduleEntry.modBaseAddr) +
							procedureOffset);
						CloseHandle(snapshot);
						return remoteAddress;
					}
				} while (Module32NextW(snapshot, &moduleEntry));
			}
			CloseHandle(snapshot);
		}

		if (WaitForSingleObject(lpProcessInformation->hProcess, 0) != WAIT_TIMEOUT)
			break;
		Sleep(25);
	}

	printf("Failed to find the child %ls module.\n", localModuleName);
	return nullptr;
}

static bool InstallChildPrimaryThreadBootstrap(
	LPPROCESS_INFORMATION lpProcessInformation, DWORD_PTR entryPoint,
	const BYTE* originalEntryPoint, const char* dllLocation)
{
	HMODULE kernel32Handle = GetModuleHandleW(L"kernel32.dll");
	FARPROC localLoadLibraryA = kernel32Handle == nullptr
		? nullptr
		: GetProcAddress(kernel32Handle, "LoadLibraryA");
	FARPROC remoteLoadLibraryA = ResolveChildProcedureAddress(
		lpProcessInformation, localLoadLibraryA);
	if (remoteLoadLibraryA == nullptr)
		return false;

	if (SuspendThread(lpProcessInformation->hThread) == static_cast<DWORD>(-1))
	{
		printf("Failed to suspend the parked child process (0x%lX).\n",
			GetLastError());
		return false;
	}

	DWORD oldProtect = 0;
	if (!VirtualProtectEx(lpProcessInformation->hProcess,
		reinterpret_cast<LPVOID>(entryPoint), 20, PAGE_EXECUTE_READWRITE,
		&oldProtect))
	{
		printf("Failed to make the child entry point writable (0x%lX).\n",
			GetLastError());
		return false;
	}

	LPVOID remoteBootstrap = VirtualAllocEx(lpProcessInformation->hProcess,
		nullptr, 0x1000, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
	if (remoteBootstrap == nullptr)
	{
		printf("Failed to allocate the child bootstrap (0x%lX).\n",
			GetLastError());
		return false;
	}

	// The launcher process already contains this DLL. LoadLibraryA returns the
	// child module base, so adding InitializeASI's local RVA gives the correct
	// child export without assuming identical Wine/Box64 module addresses.
	const uintptr_t initializeRva =
		reinterpret_cast<uintptr_t>(&InitializeASI) -
		reinterpret_cast<uintptr_t>(&__ImageBase);
	const uintptr_t remoteDllPath =
		reinterpret_cast<uintptr_t>(remoteBootstrap) + 0x200;
	const uintptr_t remoteLoadLibraryAddress =
		reinterpret_cast<uintptr_t>(remoteLoadLibraryA);
	const uintptr_t entryPointAddress = static_cast<uintptr_t>(entryPoint);

	// Preserve the primary thread's startup state, load OpenParrot64.dll, call
	// InitializeASI outside the loader lock, restore the game's original entry
	// bytes, and jump there without rewriting RIP. The fixed offsets below are
	// asserted by the byte count and are covered by the x64 build gate.
	BYTE bootstrap[] = {
		0x9C,                                             // pushfq
		0x50, 0x51, 0x52,                               // push rax, rcx, rdx
		0x41, 0x50, 0x41, 0x51,                         // push r8, r9
		0x41, 0x52, 0x41, 0x53,                         // push r10, r11
		0x48, 0x83, 0xEC, 0x28,                         // sub rsp, 28h
		0x48, 0xB9, 0, 0, 0, 0, 0, 0, 0, 0,           // mov rcx, DLL path
		0x48, 0xB8, 0, 0, 0, 0, 0, 0, 0, 0,           // mov rax, LoadLibraryA
		0xFF, 0xD0,                                     // call rax
		0x48, 0x85, 0xC0,                               // test rax, rax
		0x74, 0x0F,                                     // jz skip InitializeASI
		0x48, 0xBA, 0, 0, 0, 0, 0, 0, 0, 0,           // mov rdx, InitializeASI RVA
		0x48, 0x01, 0xD0,                               // add rax, rdx
		0xFF, 0xD0,                                     // call rax
		0x48, 0x83, 0xC4, 0x28,                         // add rsp, 28h
		0x48, 0xB8, 0, 0, 0, 0, 0, 0, 0, 0,           // mov rax, entry point
		0x48, 0xBA, 0, 0, 0, 0, 0, 0, 0, 0,           // mov rdx, original[0..7]
		0x48, 0x89, 0x10,                               // mov [rax], rdx
		0x48, 0xBA, 0, 0, 0, 0, 0, 0, 0, 0,           // mov rdx, original[8..15]
		0x48, 0x89, 0x50, 0x08,                         // mov [rax+8], rdx
		0x41, 0xBA, 0, 0, 0, 0,                         // mov r10d, original[16..19]
		0x44, 0x89, 0x50, 0x10,                         // mov [rax+16], r10d
		0x41, 0x5B, 0x41, 0x5A,                         // pop r11, r10
		0x41, 0x59, 0x41, 0x58,                         // pop r9, r8
		0x5A, 0x59, 0x58, 0x9D,                         // pop rdx, rcx, rax, popfq
		0xFF, 0x25, 0, 0, 0, 0,                         // jmp [rip+0]
		0, 0, 0, 0, 0, 0, 0, 0                        // entry point address
	};
	static_assert(sizeof(bootstrap) == 135,
		"Unexpected child bootstrap size");
	memcpy(&bootstrap[18], &remoteDllPath, sizeof(remoteDllPath));
	memcpy(&bootstrap[28], &remoteLoadLibraryAddress,
		sizeof(remoteLoadLibraryAddress));
	memcpy(&bootstrap[45], &initializeRva, sizeof(initializeRva));
	memcpy(&bootstrap[64], &entryPointAddress, sizeof(entryPointAddress));
	memcpy(&bootstrap[74], originalEntryPoint, 8);
	memcpy(&bootstrap[87], originalEntryPoint + 8, 8);
	memcpy(&bootstrap[101], originalEntryPoint + 16, 4);
	memcpy(&bootstrap[127], &entryPointAddress, sizeof(entryPointAddress));

	SIZE_T written = 0;
	const SIZE_T dllPathSize = strlen(dllLocation) + 1;
	if (!WriteProcessMemory(lpProcessInformation->hProcess,
		reinterpret_cast<LPVOID>(remoteDllPath), dllLocation, dllPathSize,
		&written) || written != dllPathSize)
	{
		printf("Failed to write the child DLL path (0x%lX).\n",
			GetLastError());
		return false;
	}

	written = 0;
	if (!WriteProcessMemory(lpProcessInformation->hProcess, remoteBootstrap,
		bootstrap, sizeof(bootstrap), &written) || written != sizeof(bootstrap))
	{
		printf("Failed to write the child bootstrap (0x%lX).\n",
			GetLastError());
		return false;
	}

	BYTE entryJump[] = {
		0x48, 0xB8, 0, 0, 0, 0, 0, 0, 0, 0,           // mov rax, bootstrap
		0xFF, 0xE0                                      // jmp rax
	};
	const uintptr_t remoteBootstrapAddress =
		reinterpret_cast<uintptr_t>(remoteBootstrap);
	memcpy(&entryJump[2], &remoteBootstrapAddress,
		sizeof(remoteBootstrapAddress));
	written = 0;
	if (!WriteProcessMemory(lpProcessInformation->hProcess,
		reinterpret_cast<LPVOID>(entryPoint), entryJump, sizeof(entryJump),
		&written) || written != sizeof(entryJump))
	{
		printf("Failed to install the child entry jump (0x%lX).\n",
			GetLastError());
		return false;
	}

	FlushInstructionCache(lpProcessInformation->hProcess, remoteBootstrap,
		sizeof(bootstrap));
	FlushInstructionCache(lpProcessInformation->hProcess,
		reinterpret_cast<LPVOID>(entryPoint), sizeof(entryJump));
	printf("Installed Wine-safe primary-thread child bootstrap at %p.\n",
		remoteBootstrap);
	return true;
}

static bool StartChildWithPrimaryThreadInit(
	LPPROCESS_INFORMATION lpProcessInformation, DWORD_PTR entryPoint,
	const BYTE* originalEntryPoint, const char* dllLocation)
{
	const BYTE spinLoop[2] = { 0xEB, 0xFE };
	SIZE_T written = 0;
	if (!WriteProcessMemory(lpProcessInformation->hProcess,
		reinterpret_cast<LPVOID>(entryPoint), spinLoop, sizeof(spinLoop),
		&written) || written != sizeof(spinLoop))
	{
		printf("Failed to park the child entry point (0x%lX).\n",
			GetLastError());
		return false;
	}
	FlushInstructionCache(lpProcessInformation->hProcess,
		reinterpret_cast<LPVOID>(entryPoint), sizeof(spinLoop));

	if (ResumeThread(lpProcessInformation->hThread) == static_cast<DWORD>(-1))
	{
		printf("Failed to start the parked child process (0x%lX).\n",
			GetLastError());
		return false;
	}

	if (!InstallChildPrimaryThreadBootstrap(lpProcessInformation, entryPoint,
		originalEntryPoint, dllLocation))
		return false;

	if (ResumeThread(lpProcessInformation->hThread) == static_cast<DWORD>(-1))
	{
		printf("Failed to resume the bootstrapped child process (0x%lX).\n",
			GetLastError());
		return false;
	}
	return true;
}

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
	if (!myProc)
		return FALSE;

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

	if (ShouldUseChildPrimaryThreadInit())
	{
		BYTE originalEntryPoint[20] = {};
		const DWORD_PTR entryPoint = baseAddress +
			FilePEFile.image_nt_headers.OptionalHeader.AddressOfEntryPoint;
		read = 0;
		if (!ReadProcessMemory(lpProcessInformation->hProcess,
			reinterpret_cast<LPVOID>(entryPoint), originalEntryPoint,
			sizeof(originalEntryPoint), &read) ||
			read != sizeof(originalEntryPoint))
		{
			printf("Failed to read the child entry point (0x%lX).\n",
				GetLastError());
			TerminateProcess(lpProcessInformation->hProcess, 0);
			return FALSE;
		}

		char childDllPath[MAX_PATH] = {};
		if (GetModuleFileNameA(reinterpret_cast<HINSTANCE>(&__ImageBase),
			childDllPath, MAX_PATH) == 0 ||
			!StartChildWithPrimaryThreadInit(lpProcessInformation, entryPoint,
				originalEntryPoint, childDllPath))
		{
			TerminateProcess(lpProcessInformation->hProcess, 0);
			return FALSE;
		}
		return TRUE;
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
	return myProc;
}
#endif
