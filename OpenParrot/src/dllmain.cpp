#include <StdInc.h>
#include <Utility/InitFunction.h>
#include <Utility/GameDetect.h>
linb::ini config;
#include <Functions/Global.h>

#pragma optimize("", off)

static void RunMain();

static BYTE originalCode[20];
extern "C" PBYTE originalEP = 0;

#ifdef _M_AMD64
extern "C" void Main_DoResume();
#endif

static void Main_DoInit()
{
	RunMain();

	DWORD oldProtect;
	VirtualProtect(originalEP, 20, PAGE_EXECUTE_READWRITE, &oldProtect);

	memcpy(originalEP, &originalCode, sizeof(originalCode));

	VirtualProtect(originalEP, 20, oldProtect, &oldProtect);

#if _M_IX86
	__asm jmp originalEP
#elif defined(_M_AMD64)
	Main_DoResume();
#endif
}

static void Main_SetSafeInit()
{
	// find the entry point for the executable process, set page access, and replace the EP
	HMODULE hModule = GetModuleHandle(NULL);

	if (hModule)
	{
		PIMAGE_DOS_HEADER header = (PIMAGE_DOS_HEADER)hModule;
		PIMAGE_NT_HEADERS ntHeader = (PIMAGE_NT_HEADERS)((DWORD_PTR)hModule + header->e_lfanew);

		// back up original code
		PBYTE ep = (PBYTE)((DWORD_PTR)hModule + ntHeader->OptionalHeader.AddressOfEntryPoint);
		memcpy(originalCode, ep, sizeof(originalCode));

		DWORD oldProtect;
		VirtualProtect(ep, 20, PAGE_EXECUTE_READWRITE, &oldProtect);

#ifdef _M_IX86
		// patch to call our EP
		int newEP = (int)Main_DoInit - ((int)ep + 5);
		ep[0] = 0xE9; // for some reason this doesn't work properly when run under the debugger
		memcpy(&ep[1], &newEP, 4);
#elif defined(_M_AMD64)
		ep[0] = 0x48;
		ep[1] = 0xB8;
		*(uint64_t *)(ep + 2) = (uint64_t)Main_DoInit;
		ep[10] = 0xFF;
		ep[11] = 0xE0;
#endif

		VirtualProtect(ep, 20, oldProtect, &oldProtect);

		originalEP = ep;
	}
}

static void RunMain()
{
	static bool initialized;

	if (initialized)
	{
		return;
	}

	initialized = true;

	if (!config.load_file("teknoparrot.ini"))
	{
		//MessageBoxA(NULL, V("Failed to open config.ini"), V("TeknoParrot",) MB_OK);
		//std::_Exit(0);
	}

	GameDetect::DetectCurrentGame();
	InitFunction::RunFunctions(GameID::Global);
	InitFunction::RunFunctions(GameDetect::currentGame);
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved)
{
	if (ul_reason_for_call == DLL_PROCESS_ATTACH)
	{
		// SpiceTools detection
		if (GameDetect::file_exists("spice.exe") || GameDetect::file_exists("spice64.exe"))
		{
			GameDetect::DetectCurrentGame(true);
			InitFunction::RunFunctions(GameID::Global);
			InitFunction::RunFunctions(GameDetect::currentGame);
			return TRUE;
		}
#ifdef DEVMODE
		RunMain();
#else
		Main_SetSafeInit();
#endif
	}
	return TRUE; // false
}

extern "C" __declspec(dllexport) void InitializeASI()
{
	RunMain();
}
#pragma optimize("", on)