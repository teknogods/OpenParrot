#include <StdInc.h>
#include "Utility/InitFunction.h"
#include "Functions/Global.h"
#include "Utility\Hooking.Patterns.h"
#include <Xinput.h>
#include <math.h>
#include <dinput.h>

// Helper to clean up code
#define Check2Keys(key1, key2) (GetAsyncKeyState(key1) & 0x8000 || GetAsyncKeyState(key2) & 0x8000)

static DWORD mainModuleBase = 0x00400000;
int horizontal3 = 0;
int vertical3 = 0;
static bool brakepressed = false;
static bool brakepressedlastframe = false;

// controls
extern int* ffbOffset;
extern int* ffbOffset2;
extern int* ffbOffset3;
extern int* ffbOffset4;
extern int* ffbOffset5; // for the shifter

static std::string newCommandLine = "";

static LPSTR(WINAPI* GetCommandLineAOriginal)();
static LPSTR WINAPI GetCommandLineAHook() {
	return const_cast<LPSTR>(newCommandLine.c_str());
};

static DWORD WINAPI InputRT3(LPVOID lpParam)
{
	// We write directly into this struct in memory to bypass having to use debug stuff
	DWORD* keypadStruct = (DWORD*)(0x31df2e0 + mainModuleBase);
	bool keypadPressed[12] = {};

	while (true)
	{
		// TEST
		if (*ffbOffset & 0x01)
		{
			injector::WriteMemory<BYTE>((0x41599 + mainModuleBase), 0xEB, true);
		}

		// I wish this was a bit cleaner but for now this will suffice
		if (Check2Keys('0', VK_NUMPAD0)) {
			if (!keypadPressed[10]) {
				keypadStruct[10] = 1;
				keypadPressed[10] = true;
			}
		}
		else {
			keypadStruct[10] = 0;
			keypadPressed[10] = false;
		}

		if (Check2Keys(VK_OEM_MINUS, VK_DECIMAL)) {
			if (!keypadPressed[9]) {
				keypadStruct[9] = 1;
				keypadPressed[9] = true;
			}
		}
		else {
			keypadStruct[9] = 0;
			keypadPressed[9] = false;
		}

		if (Check2Keys(VK_OEM_PLUS, VK_RETURN)) {
			if (!keypadPressed[11]) {
				keypadStruct[11] = 1;
				keypadPressed[11] = true;
			}
		}
		else {
			keypadStruct[11] = 0;
			keypadPressed[11] = false;
		}

		for (int key = 1; key <= 9; ++key) {
			if (Check2Keys('0' + key, VK_NUMPAD0 + key)) {
				if (!keypadPressed[key - 1]) {
					keypadStruct[key - 1] = 1;
					keypadPressed[key - 1] = true;
				}
			}
			else {
				keypadStruct[key - 1] = 0;
				keypadPressed[key - 1] = false;
			}
		}

		// WHEEL
		int iWheel = (*ffbOffset2 - 128);
		float wheel = (iWheel * 0.0078125f);
		//injector::WriteMemory<float>((0x31b672c + mainModuleBase), wheel, true); // THIS MESSES UP GAS AND IS NOT NEEDED, DO NOT UNCOMMENT
		// add 128 back to iWheel, somehow the game wants 0 to 255 for the byte
		injector::WriteMemory<BYTE>((0x31df220 + mainModuleBase), iWheel + 128, true);
		// GAS
		float gas = ((float)*ffbOffset3) / 255.0f;
		injector::WriteMemory<float>((0x1aafc8 + mainModuleBase), gas, true);
		injector::WriteMemory<BYTE>((0x31df221 + mainModuleBase), *ffbOffset3, true);

		if (*ffbOffset4 > 40)
		{
			brakepressed = true;
		}
		else {
			brakepressed = false;
			brakepressedlastframe = false;
		}

		//DEBUG//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		//info("test values *ffbOffset2=0x%02X /  *iWheel=%d / *wheel=%f ", *ffbOffset2, iWheel, wheel);
		//info("test values *ffbOffset3=0x%02X / *gas=%f ", *ffbOffset3, gas);
		//DEBUG//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

		Sleep(16);
	}
	return 0;
}

static BOOL(__stdcall* original_CreateWindowExA3)(DWORD dwExStyle, LPCSTR lpClassName, LPCSTR lpWindowName, DWORD dwStyle, int X, int Y, int nWidth, int nHeight, HWND hWndParent, HMENU hMenu, HINSTANCE hInstance, LPVOID lpParam);
static DWORD WINAPI CreateWindowExART3(DWORD dwExStyle, LPCSTR lpClassName, LPCSTR lpWindowName, DWORD dwStyle, int X, int Y, int nWidth, int nHeight, HWND hWndParent, HMENU hMenu, HINSTANCE hInstance, LPVOID lpParam)
{

	dwStyle = WS_OVERLAPPEDWINDOW | WS_VISIBLE;

	if (!ToBool(config["General"]["HDPatch"]))
	{
		nWidth = 640;
		nHeight = 480;
	}

	std::tie(nWidth, nHeight) = CalculateWindowSize(nWidth, nHeight, dwStyle, dwExStyle);
	std::tie(X, Y) = CalculateWindowCenterPosition(nWidth, nHeight);
	return original_CreateWindowExA3(dwExStyle, lpClassName, "Fast n Furious", dwStyle, X, Y, nWidth, nHeight, hWndParent, hMenu, hInstance, lpParam);
}

static BOOL(__stdcall* SetCursorPosOriginal)(int X, int Y);
static DWORD WINAPI SetCursorPosHook(int X, int Y)
{
	return 0;
}

static BOOL(__stdcall* SetWindowPosOriginal)(HWND hWnd, HWND hWndInsertAfter, int X, int Y, int cx, int cy, UINT uFlags);
static DWORD WINAPI SetWindowPosHook(HWND hWnd, HWND hWndInsertAfter, int X, int Y, int cx, int cy, UINT uFlags)
{
	return 1;
}

enum FNF_BUTTONS : int
{
	FNF_START = 0x1000,
	FNF_BUTTON1 = 0x1001,
	FNF_BUTTON2 = 0x1002,
	FNF_BUTTON3 = 0x1003,
	FNF_TUNES = 0x1004,
	FNF_GEAR1 = 0x1005,
	FNF_GEAR2 = 0x1006,
	FNF_GEAR3 = 0x1007,
	FNF_GEAR4 = 0x1008,
	FNF_COIN1 = 0x1009,
	FNF_COIN2 = 0x100A,
	FNF_TEST = 0x100B,
	FNF_SERVICE = 0x100C,
	FNF_BILL = 0x100D,
	FNF_VOLUP = 0x100E,
	FNF_VOLDOWN = 0x100F,
	FNF_HANDBRAKE = 0x1010,
};

static std::unordered_map<FNF_BUTTONS, bool> buttonStates =
{
	{ FNF_START, false},
	{ FNF_BUTTON1, false},
	{ FNF_BUTTON2, false},
	{ FNF_BUTTON3, false},
	{ FNF_TUNES, false},
	{ FNF_GEAR1, false},
	{ FNF_GEAR2, false},
	{ FNF_GEAR3, false},
	{ FNF_GEAR4, true},
	{ FNF_COIN1, false},
	{ FNF_COIN2, false},
	{ FNF_TEST, false},
	{ FNF_SERVICE, false},
	{ FNF_BILL, false},
	{ FNF_VOLUP, false},
	{ FNF_VOLDOWN, false},
	{ FNF_HANDBRAKE, false},
};

static std::unordered_map<FNF_BUTTONS, int> buttonOffsets =
{
	{ FNF_START, 0x08},
	{ FNF_BUTTON1, 0x200},
	{ FNF_BUTTON2, 0x400},
	{ FNF_BUTTON3, 0x800},
	{ FNF_TUNES, 0x100},
	{ FNF_GEAR1, 0xFFFF}, // TODO: assign new pipe slots for these
	{ FNF_GEAR2, 0xFFFF},
	{ FNF_GEAR3, 0xFFFF},
	{ FNF_GEAR4, 0xFFFF},
	{ FNF_COIN1, 0x04},
	{ FNF_COIN2, 0xFFFF}, // not needed
	{ FNF_TEST, 0x01},
	{ FNF_SERVICE,  0x02},
	{ FNF_BILL, 0xFFFF}, // don't need that
	{ FNF_VOLUP, 0x4000},
	{ FNF_VOLDOWN, 0x8000},
	{ FNF_HANDBRAKE, 0xFFFF}, // lets handle this seperate by converting our analog pedal into the digital button RT wants
};

static void inline SetButtonState(int buttonCode, bool newState)
{
	auto button = buttonStates.find(static_cast<FNF_BUTTONS>(buttonCode));
	if (button != buttonStates.end())
	{
		button->second = newState;
	}
}

static bool inline GetButtonState(int buttonIndex)
{
	auto button = buttonStates.find(static_cast<FNF_BUTTONS>(buttonIndex));
	if (button != buttonStates.end())
	{
		return button->second;
	}
	else
	{
		return false;
	}
}

static int inline GetButtonFFBOffset(int buttonIndex)
{
	auto button = buttonOffsets.find(static_cast<FNF_BUTTONS>(buttonIndex));
	if (button != buttonOffsets.end())
	{
		return button->second;
	}
	else
	{
		return 0xFFFF;
	}
}

static void ResetButtons()
{
	for (auto const& [key, value] : buttonOffsets)
	{
		if (value == 0xFFFF) {
			continue;
		}

		if (!(*ffbOffset & value))
		{

			SetButtonState(key, false);
		}

		if (!(*ffbOffset5 & 0x01))
		{
			SetButtonState(FNF_GEAR1, false);
		}

		if (!(*ffbOffset5 & 0x02))
		{
			SetButtonState(FNF_GEAR2, false);
		}

		if (!(*ffbOffset5 & 0x04))
		{
			SetButtonState(FNF_GEAR3, false);
		}

		if (!(*ffbOffset5 & 0x08))
		{
			SetButtonState(FNF_GEAR4, false);
		}
	}
}

static int _cdecl GetInputPressOnce(unsigned int buttonCode)
{
	// Clear the button state cache
	ResetButtons();

	if (buttonCode == FNF_GEAR1)
	{
		if (*ffbOffset5 & 0x1 && !GetButtonState(buttonCode))
		{
			SetButtonState(buttonCode, true);
			return 1;
		}
		else {
			return 0;
		}
	}
	else if (buttonCode == FNF_GEAR2)
	{
		if (*ffbOffset5 & 0x2 && !GetButtonState(buttonCode))
		{
			SetButtonState(buttonCode, true);
			return 1;
		}
		else {
			return 0;
		}
	}
	else if (buttonCode == FNF_GEAR3)
	{
		if (*ffbOffset5 & 0x4 && !GetButtonState(buttonCode))
		{
			SetButtonState(buttonCode, true);
			return 1;
		}
		else {
			return 0;
		}
	}
	else if (buttonCode == FNF_GEAR4)
	{
		if (*ffbOffset5 & 0x8 && !GetButtonState(buttonCode))
		{
			SetButtonState(buttonCode, true);
			return 1;
		}
		else {
			return 0;
		}
	}

	// Map analog brake to digital button ingame because lol Raw Thrills
	if (buttonCode == FNF_HANDBRAKE)
	{
		if (brakepressed)
		{
			if (brakepressedlastframe) {
				return 0;
			}
			else {
				brakepressedlastframe = true;
				return 1;
			}
		}
		else {
			return 0;
		}
	}

	// check if button is pressed && button State is not pressed
	// if so: return 1, then set state to true, otherwise return 0
	int buttonOffset = GetButtonFFBOffset(buttonCode);

	// unmapped buttons should be at FFFF
	if (buttonOffset == 0xFFFF)
	{
		return 0;
	}

	if (*ffbOffset & buttonOffset && !GetButtonState(buttonCode))
	{
		SetButtonState(buttonCode, true);
		return 1;
	}
	else {
		return 0;
	}
}

static int __cdecl GetInputPress(unsigned int buttonCode)
{
	int buttonOffset = GetButtonFFBOffset(buttonCode);

	if (buttonCode == FNF_GEAR1)
	{
		return *ffbOffset5 & 0x01;
	}
	else if (buttonCode == FNF_GEAR2)
	{
		return *ffbOffset5 & 0x02;
	}
	else if (buttonCode == FNF_GEAR3)
	{
		return *ffbOffset5 & 0x04;
	}
	else if (buttonCode == FNF_GEAR4)
	{
		return *ffbOffset5 & 0x08;
	}

	// Map analog brake to digital button ingame because lol Raw Thrills
	if (buttonCode == FNF_HANDBRAKE)
	{
		return brakepressed;
	}

	// unmapped buttons should be at FFFF
	if (buttonOffset == 0xFFFF)
	{
		return 0;
	}

	if (*ffbOffset & buttonOffset)
	{
		return 1;
	}
	else {
		return 0;
	}
}

static void(__cdecl* DrawControlIconsOrig)(float* wheelIcon, float* pedalIcon, float scaleMultiplier, float* a4, float a5, float a6);
static void __cdecl DrawControlIconsHook(float* wheelIcon, float* pedalIcon, float scaleMultiplier, float* a4, float a5, float a6)
{
#ifdef _DEBUG
	info("Tried drawing menu control icons");
#endif
	scaleMultiplier *= 2.5;
	wheelIcon[0] *= 2.5;
	wheelIcon[1] *= 2.5;
	pedalIcon[0] *= 2.5;
	pedalIcon[1] *= 2.5;
	return DrawControlIconsOrig(wheelIcon, pedalIcon, scaleMultiplier, a4, a5, a6);
}

static InitFunction FNFFunc([]()
	{
		GetDesktopResolution(horizontal3, vertical3);
		LPSTR lpCmdLine = GetCommandLineA();
		newCommandLine = lpCmdLine;
		if (ToBool(config["General"]["Enable Anti Aliasing"]))
		{
#ifdef _DEBUG
			info("Anti Aliasing enabled");
#endif
			newCommandLine += " -fsaa8";
		}

		// at this point a bunch of these hacks aren't needed but 
		// this game is chipping away at my sanity so ill leave them here while they work

		// Disable debug inputs
		injector::WriteMemory<BYTE>((mainModuleBase + 0x135994), 0x1013, true);
		injector::MakeNOP((mainModuleBase + 0x40240), 6, true);
		injector::MakeNOP((mainModuleBase + 0x4c224), 25, true);

		// always poll inputs
		injector::MakeNOP((mainModuleBase + 0x86d0d), 4, true);
		injector::MakeNOP((mainModuleBase + 0x831a7), 8, true);

		// PATCH CODE FOR GAS AND BRAKE
		injector::WriteMemoryRaw((0x4f6cc + mainModuleBase), "\xb8\xc8\xaf\x5a\x00\x89\x86\xf0\x00\x00\x00\xEB\x17\x90", 14, true);
		injector::MakeNOP((0x4f70d + mainModuleBase), 10);
		injector::MakeNOP((0x4f740 + mainModuleBase), 6);
		injector::MakeNOP(mainModuleBase + 0x834b9, 2, true); // always treat IO as working

		injector::MakeNOP(mainModuleBase + 0x86ae5, 6, true); // don't change gas byte
		injector::MakeNOP(mainModuleBase + 0x4132c, 6, true); // don't change gas float
		injector::MakeNOP(mainModuleBase + 0x4f70d, 10, true);

		// overwrite generic button check function
		safeJMP(mainModuleBase + 0x83aa0, GetInputPress);

		// debounced button handling or something :thinking: 
		safeJMP(mainModuleBase + 0x83bd0, GetInputPressOnce);

		// IO OK
		injector::WriteMemory<DWORD>(mainModuleBase + 0x31b6a98, 0x1000, true);

		// REMOVE ERROR MESSAGEBOX ON CLOSE
		injector::WriteMemory<BYTE>((mainModuleBase + 0x4CC2A), 0xEB, true);

		// REPLACE SPACE KEY WITH ESC TO PREVENT EXITING LEVEL PREMATURELY
		injector::WriteMemory<BYTE>((mainModuleBase + 0x421B2), DIK_ESCAPE, true);

		// FIX file write on D:
		injector::WriteMemoryRaw((mainModuleBase + 0xD3E6C), "\x2E\x5C\x65\x72\x72\x6F\x72\x6C\x6F\x67\x2E\x74\x78\x74\x00", 15, true);

		// TEST KEY FIX (uses BACKSPACE now)
		injector::MakeNOP((mainModuleBase + 0x4159B), 14);
		injector::WriteMemory<BYTE>((mainModuleBase + 0x415AA), DIK_BACK, true);

		// REMOVE ESC BOX
		injector::MakeNOP((0x440A0B), 5, true);

		// WIP/experiment regarding res scaling

		MH_Initialize();
		//MH_CreateHook((void*)(mainModuleBase + 0x37620), DrawControlIconsHook, (void**)&DrawControlIconsOrig);
		MH_CreateHookApi(L"Kernel32.dll", "GetCommandLineA", &GetCommandLineAHook, (void**)&GetCommandLineAOriginal);
		MH_CreateHookApi(L"user32.dll", "SetCursorPos", &SetCursorPosHook, (void**)&SetCursorPosOriginal);
		MH_CreateHookApi(L"user32.dll", "SetWindowPos", &SetWindowPosHook, (void**)&SetWindowPosOriginal);
		MH_EnableHook(MH_ALL_HOOKS);

		CreateThread(NULL, 0, InputRT3, NULL, 0, NULL);

		if (ToBool(config["General"]["Windowed"]))
		{
			MH_Initialize();
			MH_CreateHookApi(L"user32.dll", "CreateWindowExA", &CreateWindowExART3, (void**)&original_CreateWindowExA3);
			MH_EnableHook(MH_ALL_HOOKS);
		}
		else if (ToBool(config["General"]["HDPatch"]))
		{
			// BROKEN RESOLUTION PATCH WHEN FULLSCREEN
			injector::WriteMemory<DWORD>((0x135954 + mainModuleBase), horizontal3, true);
			injector::WriteMemory<DWORD>((0x135958 + mainModuleBase), vertical3, true);
		}

		// MACHINE ID setting
		if ((strcmp(config["Network"]["MachineID"].c_str(), "2") == 0))
		{
			injector::WriteMemory<DWORD>((0xCD808 + mainModuleBase), 0x01, true);
		}
		else if ((strcmp(config["Network"]["MachineID"].c_str(), "3") == 0))
		{
			injector::WriteMemory<DWORD>((0xCD808 + mainModuleBase), 0x02, true);
		}
		else if ((strcmp(config["Network"]["MachineID"].c_str(), "4") == 0))
		{
			injector::WriteMemory<DWORD>((0xCD808 + mainModuleBase), 0x03, true);
		}
		else // MACHINE ID = 1
		{
			injector::WriteMemory<DWORD>((0xCD808 + mainModuleBase), 0x00, true);
		}

	}, GameID::FNF);
