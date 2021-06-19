#include <StdInc.h>
#include "Utility/InitFunction.h"
#include "Functions/Global.h"
#include "Utility\Hooking.Patterns.h"

#pragma comment(lib, "Ws2_32.lib")

#ifdef _M_AMD64
extern LPCSTR hookPort;

static int PokkenGetSerial(int, int, int, int, char* dst)
{
	OutputDebugStringA("Getting Dongle Serial...");
	memset(dst, 0x00, 0x628);
	memcpy(dst + 0x428, L"274713190000\0\0", 26);
	OutputDebugStringA("Got Dongle Serial!");
	return 1;
}

static int ReturnTrue()
{
	return 1;
}

DWORD windowStyle = WS_VISIBLE | WS_POPUP | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_MAXIMIZEBOX;
int centeredX;
int centeredY;

static HWND WINAPI CreateWindowExAHook(DWORD dwExStyle, LPCSTR lpClassName, LPCSTR lpWindowName, DWORD dwStyle, int X, int Y, int nWidth, int nHeight, HWND hWndParent, HMENU hMenu, HINSTANCE hInstance, LPVOID lpParam)
{
	centeredX = (GetSystemMetrics(SM_CXSCREEN) - nWidth) / 2;
	centeredY = (GetSystemMetrics(SM_CYSCREEN) - nHeight) / 2;

	lpWindowName = "OpenParrot - Pokken Tournament";

	return CreateWindowExA(dwExStyle, lpClassName, lpWindowName, windowStyle, centeredX, centeredY, nWidth, nHeight, hWndParent, hMenu, hInstance, lpParam);
}

static HWND WINAPI CreateWindowExWHook(DWORD dwExStyle, LPCWSTR lpClassName, LPCWSTR lpWindowName, DWORD dwStyle, int X, int Y, int nWidth, int nHeight, HWND hWndParent, HMENU hMenu, HINSTANCE hInstance, LPVOID lpParam)
{
	centeredX = (GetSystemMetrics(SM_CXSCREEN) - nWidth) / 2;
	centeredY = (GetSystemMetrics(SM_CYSCREEN) - nHeight) / 2;

	lpWindowName = L"OpenParrot - Pokken Tournament";

	return CreateWindowExW(dwExStyle, lpClassName, lpWindowName, windowStyle, centeredX, centeredY, nWidth, nHeight, hWndParent, hMenu, hInstance, lpParam);
}

enum class INIT_STATE : int32_t
{
	CHECKING = 0,
	OK = 1,
	NG = 2,
	NOT_CHECKED = 3,
	OFFLINE = 4,
	DASH = 5
};

class InitData
{
public:
	uint64_t StateFunctionCurrent; //0x0000
	uint64_t StateFunctionNext; //0x0008
	uint32_t StateFunctionRV; //0x0010
	INIT_STATE StateSN; //0x0014
	INIT_STATE StateIO; //0x0018
	INIT_STATE StateBackupMemory; //0x001C
	INIT_STATE StateCardReader; //0x0020
	INIT_STATE StateUsbController; //0x0024
	INIT_STATE StateLocalNetwork; //0x0028
	INIT_STATE StateAllNet; //0x002C
	INIT_STATE StateGameServer; //0x0030
	INIT_STATE StateMatchingServer; //0x0034
	INIT_STATE StateVersion; //0x0038
	INIT_STATE StateClock; //0x003C
	uint32_t Timer1; //0x0040
	uint32_t Timer2; //0x0044
	bool Done; //0x0048
	char SerialNumber[13]; //0x0049
}; //Size: 0x0056

static uintptr_t imageBase;

static __int64 __fastcall StateFunctionSN(InitData* a1)
{
	a1->StateSN = INIT_STATE::OK;
	sprintf(a1->SerialNumber, "TEKNOPARROT");
	a1->StateFunctionNext = imageBase + 0x5902E0;

	return 1;
}

static InitFunction PokkenFunc([]()
{
	hookPort = "COM3";
	imageBase = (uintptr_t)GetModuleHandleA(0);

	// force windowed
	// BE 01 00 00 00 8B CE -0x8 // ok 00-24
	// 18: imageBase + 0x5A5A2A
	char* windowedPattern = hook::get_pattern<char>("BE 01 00 00 00 8B CE", -0x8);
	if (ToBool(config["General"]["Windowed"]))
	{
		injector::MakeNOP(windowedPattern, 8);
		injector::WriteMemory<BYTE>(windowedPattern + 0x48, 0x00, true);

		iatHook("user32.dll", CreateWindowExAHook, "CreateWindowExA");
		iatHook("user32.dll", CreateWindowExWHook, "CreateWindowExW");
	}

	// Remove BlockInput
	injector::MakeNOP(windowedPattern+8, 13);

	// make english into japanese
	// TODO: other system locales
	// 65 6E 00 00 6A 61 00 00 // ok 00-24
	// 18: imageBase + 0x9AC1FC
	char* localePattern = hook::get_pattern<char>("65 6E 00 00 6A 61 00 00");
	injector::WriteMemory<char>(localePattern, 'j', true);
	injector::WriteMemory<char>(localePattern + 0x1, 'a', true);


	// dongle
	// 41 B8 28 06 00 00 49 8B CE -0x42 // ok 00-24
	// 18: imageBase + 0x318610
	safeJMP(hook::get_pattern("41 B8 28 06 00 00 49 8B CE", -0x42), PokkenGetSerial);

	// icmp pinging
	// 44 89 4C 24 20 53 41 54 48 83 EC 58 // ok 00-24
	// 18: imageBase + 0x609450
	safeJMP(hook::get_pattern("44 89 4C 24 20 53 41 54 48 83 EC 58"), ReturnTrue);

	// subnet check (don't force 192.168.123.xxx)
	// 48 8B C4 55 48 8D 68 A1 48 81 EC B0 00 00 00 48 C7 45 D7 FE // ok 00-24
	// 18: imageBase + 0x60A5E0
	safeJMP(hook::get_pattern("48 8B C4 55 48 8D 68 A1 48 81 EC B0 00 00 00 48 C7 45 D7 FE"), ReturnTrue);

	// no cpuid detection (crashes on Core 2?)
	// 48 89 5C 24 08 4C 8B C9 C7 41 08 FF FF FF FF // ok 00-24
	// 18: imageBase + 0x6F7C80
	injector::MakeRET(hook::get_pattern("48 89 5C 24 08 4C 8B C9 C7 41 08 FF FF FF FF"));

	// don't give usb controller error #2
	// 48 8D 8F F8 00 00 00 88 9F 05 01 00 00 -0x2 // ok 00-24
	// 18: imageBase + 0x661820
	injector::MakeNOP(hook::get_pattern("48 8D 8F F8 00 00 00 88 9F 05 01 00 00", -0x2), 2);

	auto chars = { 'F', 'G', 'J' };

	for (auto cha : chars)
	{
		auto patterns = hook::pattern(va("%02X 3A 5C", cha));

		if (patterns.size() > 0)
		{
			for (int i = 0; i < patterns.size(); i++)
			{
				char* text = patterns.get(i).get<char>(0);
				std::string text_str(text);

				std::string to_replace = va("%c:\\", cha);
				std::string replace_with = va(".\\%c", cha);

				std::string replaced = text_str.replace(0, to_replace.length(), replace_with);

				injector::WriteMemoryRaw(text, (char*)replaced.c_str(), replaced.length() + 1, true);
			}
		}
	}
}, GameID::PokkenTournament);

static InitFunction PokkenFunc26([]()
{
	InitFunction::RunFunctions(GameID::PokkenTournament);
	safeJMP(imageBase + 0x590320, StateFunctionSN);
}, GameID::PokkenTournament26);
#endif