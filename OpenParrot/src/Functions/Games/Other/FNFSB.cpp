#if __has_include(<atlstr.h>)
#include <StdInc.h>
#include "Utility/InitFunction.h"
#include "Functions/Global.h"
#include "Utility\Hooking.Patterns.h"
#include <Xinput.h>
#include <math.h>
#include <dinput.h>
#include <string>
#include <atlstr.h>

// Helper to clean up code
#define Check2Keys(key1, key2) (GetAsyncKeyState(key1) & 0x8000 || GetAsyncKeyState(key2) & 0x8000)

static DWORD mainModuleBase = 0x00400000;
int horizontal4 = 0;
int vertical4 = 0;
HWND hWndRT4 = 0;
bool movable4 = false;
bool polling4 = false;

static bool previousLeft = false;
static bool previousRight = false;
static bool previousUp = false;
static bool previousDown = false;
static bool gaspressed = false;
static bool brakepressed = false;
static bool button1pressed = false;
static bool button2pressed = false;
static bool button3pressed = false;
static bool button4pressed = false;
static bool brakepressedlastframe = false;

// controls
extern int* ffbOffset;
extern int* ffbOffset2;
extern int* ffbOffset3;
extern int* ffbOffset4;

// hooks ori
BOOL(__stdcall* original_SetWindowPos4)(HWND hWnd, HWND hWndInsertAfter, int X, int Y, int cx, int cy, UINT uFlags);
BOOL(__stdcall* original_CreateWindowExA4)(DWORD dwExStyle, LPCSTR lpClassName, LPCSTR lpWindowName, DWORD dwStyle, int X, int Y, int nWidth, int nHeight, HWND hWndParent, HMENU hMenu, HINSTANCE hInstance, LPVOID lpParam);
BOOL(__stdcall* original_DefWindowProcA4)(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
BOOL(__stdcall* original_SetCursorPosRT4)(int X, int Y);

DWORD WINAPI DefWindowProcART4(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	static int xClick;
	static int yClick;

	switch (message)
	{
	case WM_LBUTTONDOWN:
		ShowCursor(TRUE);
		movable4 = true;
		xClick = LOWORD(lParam);
		yClick = HIWORD(lParam);
		break;

	case WM_LBUTTONUP:
		ShowCursor(NULL);
		movable4 = false;
		break;

	case WM_MOUSEMOVE:
	{
		if ((movable4 == true) && (ToBool(config["General"]["Windowed"])))
		{
			ShowCursor(TRUE);
			RECT rcWindow;
			GetWindowRect(hWnd, &rcWindow);
			int width = rcWindow.right - rcWindow.left;
			int height = rcWindow.bottom - rcWindow.top;
			int xMouse = LOWORD(lParam);
			int yMouse = HIWORD(lParam);
			int xWindow = rcWindow.left + xMouse - xClick;
			int yWindow = rcWindow.top + yMouse - yClick;
			if (xWindow >= (horizontal4 - 100))
				xWindow = 0;
			if (yWindow >= (vertical4 - 100))
				yWindow = 0;
			original_SetWindowPos4(hWnd, NULL, xWindow, yWindow, width, height, SWP_NOSIZE | SWP_NOZORDER);
			return 0;
		}
		else
		{
			if (polling4 == false)
			{
				ShowCursor(NULL);
				return 0;
			}
			if (polling4 == true)
			{
				ShowCursor(NULL);
				break;
			}
		}
	}
	}
	return original_DefWindowProcA4(hWnd, message, wParam, lParam);
}

DWORD WINAPI InputRT4(LPVOID lpParam)
{
	// We write directly into this struct in memory to bypass having to use debug stuff
	DWORD* keypadStruct = (DWORD*)(0x3597688);
	bool keypadPressed[12] = {};

	int deltaTimer = 16;
	while (true)
	{
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
		injector::WriteMemory<BYTE>(0x35975c8, iWheel + 128, true);
		//// GAS
		float gas = ((float)*ffbOffset3) / 255.0f;
		injector::WriteMemory<float>(0x57eaec, gas, true);
		injector::WriteMemory<BYTE>(0x35975c9, *ffbOffset3, true);

		if (*ffbOffset4 > 40)
		{
			brakepressed = true;
		}
		else {
			brakepressed = false;
			brakepressedlastframe = false;
		}

		SleepEx(deltaTimer, 1);
	}
}

DWORD WINAPI CreateWindowExART4(DWORD dwExStyle, LPCSTR lpClassName, LPCSTR lpWindowName, DWORD dwStyle, int X, int Y, int nWidth, int nHeight, HWND hWndParent, HMENU hMenu, HINSTANCE hInstance, LPVOID lpParam)
{
	return original_CreateWindowExA4(dwExStyle, lpClassName, "Fast n Furious SuperBikes", 0x96C60000, X, Y, (nWidth + 16), (nHeight + 39), hWndParent, hMenu, hInstance, lpParam);
}

DWORD WINAPI SetCursorPosRT4(int X, int Y)
{
	return 0;
}

DWORD WINAPI SetWindowPosRT4(HWND hWnd, HWND hWndInsertAfter, int X, int Y, int cx, int cy, UINT uFlags)
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

typedef struct {
	bool pressed;
	int ffbOffset;
} fnfButtonState;

std::unordered_map<FNF_BUTTONS, fnfButtonState> buttonStates = {
	{FNF_START, {false, 0x08}},
	{FNF_BUTTON1, {false, 0x200}}, // VIEW
	{FNF_GEAR1, {false, 0x400}}, // Shift Up
	{FNF_BUTTON3, {false, 0x800}},
	{FNF_TUNES, {false, 0x100}},
	{FNF_GEAR2, {false, 0xFFFF}},
	{FNF_GEAR3, {false, 0xFFFF}},
	{FNF_GEAR4, {true, 0xFFFF}},
	{FNF_COIN1, {false, 0x04}},
	{FNF_COIN2, {false, 0xFFFF}},
	{FNF_TEST, {false, 0x01}},
	{FNF_SERVICE, {false, 0x02}},
	{FNF_BILL, {false, 0xFFFF}},
	{FNF_VOLUP, {false, 0x4000}},
	{FNF_VOLDOWN, {false, 0x8000}},
	{FNF_HANDBRAKE, {false, 0xFFFF}}, // Braking also shifts down... weird game
};

static void inline SetButtonState(int buttonCode, bool newState)
{
	auto button = buttonStates.find(static_cast<FNF_BUTTONS>(buttonCode));
	if (button != buttonStates.end())
	{
		button->second.pressed = newState;
	}
}

static bool inline GetButtonState(int buttonIndex)
{
	auto button = buttonStates.find(static_cast<FNF_BUTTONS>(buttonIndex));
	if (button != buttonStates.end())
	{
		return button->second.pressed;
	}
	else
	{
		return false;
	}
}

static int inline GetButtonFFBOffset(int buttonIndex)
{
	auto button = buttonStates.find(static_cast<FNF_BUTTONS>(buttonIndex));
	if (button != buttonStates.end())
	{
		return button->second.ffbOffset;
	}
	else
	{
		return 0xFFFF;
	}
}

static void ResetButtons()
{
	for (auto const& [key, value] : buttonStates)
	{
		if (value.ffbOffset == 0xFFFF) {
			continue;
		}

		if (!(*ffbOffset & value.ffbOffset))
		{

			SetButtonState(key, false);
		}
	}
}

static int _cdecl GetInputPressOnce(unsigned int buttonCode)
{
	// Clear the button state cache
	ResetButtons();

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

static InitFunction FNFSBFunc([]()
	{
		GetDesktopResolution(horizontal4, vertical4);
		uintptr_t mainModuleBase = (uintptr_t)GetModuleHandleA(0);
		// DISABLE CURSOR RESET
		injector::WriteMemory<BYTE>((0x57C19 + mainModuleBase), 0xEB, true);

		// REMOVE ERROR MESSAGEBOX ON CLOSE
		injector::WriteMemory<BYTE>((0x584AA + mainModuleBase), 0xEB, true);

		// REPLACE SPACE KEY WITH ESC TO PREVENT EXITING LEVEL PREMATURELY
		injector::WriteMemory<BYTE>((0x4D1C5 + mainModuleBase), DIK_ESCAPE, true);

		// FIX file write on D:
		injector::WriteMemoryRaw((0x125B50 + mainModuleBase), "\x2E\x5C\x65\x72\x72\x6F\x72\x6C\x6F\x67\x2E\x74\x78\x74\x00", 15, true);
		injector::WriteMemoryRaw((0x11FAD4 + mainModuleBase), "\x2E\x5C\x76\x65\x72\x73\x69\x6F\x6E\x2E\x74\x78\x74\x00", 14, true);

		// TEST KEY FIX (uses BACKSPACE now)
		injector::MakeNOP((0x4C426 + mainModuleBase), 14);
		injector::WriteMemory<BYTE>((0x4C435 + mainModuleBase), DIK_BACK, true);

		// REMOVE ESC BOX
		injector::MakeNOP((0x44B4B5), 5, true);

		// IO stuff here :3
		// Disable debug inputs
		injector::WriteMemory<BYTE>(0x554504, 0x1013, true);
		injector::MakeNOP(0x44aaf6, 11, true);
		injector::MakeNOP(0x44aadc, 10, true);

		// always poll inputs
		injector::MakeNOP(0x4c4cfd, 4, true);
		safeJMP(0x4c47b0, genericRetZero, true);

		// stop the game from overwriting gas values
		injector::WriteMemoryRaw(0x45b234, "\xb8\xc8\xaf\x5a\x00\x89\x86\xf0\x00\x00\x00\xEB\x17\x90", 14, true);

		// IO OK
		injector::WriteMemory<DWORD>(0x35944d8, 0x1000, true);
		injector::MakeNOP(0x4b7949, 2, true); // always treat IO as working

		// overwrite generic button check function
		safeJMP(0x4b7f30, GetInputPress);

		// debounced button handling or something :thinking: 
		safeJMP(0x4b8060, GetInputPressOnce);

		// Run from any path
		safeJMP(0x4b37a0, genericRetZero, true);

		MH_Initialize();
		MH_CreateHookApi(L"user32.dll", "SetCursorPos", &SetCursorPosRT4, (void**)&original_SetCursorPosRT4);
		MH_CreateHookApi(L"user32.dll", "DefWindowProcA", &DefWindowProcART4, (void**)&original_DefWindowProcA4);
		MH_CreateHookApi(L"user32.dll", "SetWindowPos", &SetWindowPosRT4, (void**)&original_SetWindowPos4);
		MH_EnableHook(MH_ALL_HOOKS);

		CreateThread(NULL, 0, InputRT4, NULL, 0, NULL);

		if (ToBool(config["General"]["Windowed"]))
		{
			MH_Initialize();
			MH_CreateHookApi(L"user32.dll", "CreateWindowExA", &CreateWindowExART4, (void**)&original_CreateWindowExA4);
			MH_EnableHook(MH_ALL_HOOKS);
		}

		else if (ToBool(config["General"]["HDPatch"]))
		{
			// BROKEN RESOLUTION PATCH WHEN FULLSCREEN
			injector::WriteMemory<DWORD>((0x1522F8 + mainModuleBase), horizontal4, true);
			injector::WriteMemory<DWORD>((0x1522FC + mainModuleBase), vertical4, true);
		}

		// MACHINE ID setting
		if ((strcmp(config["Network"]["MachineID"].c_str(), "2") == 0))
		{
			injector::WriteMemory<DWORD>((0x11FA18 + mainModuleBase), 0x01, true);
		}
		else if ((strcmp(config["Network"]["MachineID"].c_str(), "3") == 0))
		{
			injector::WriteMemory<DWORD>((0x11FA18 + mainModuleBase), 0x02, true);
		}
		else if ((strcmp(config["Network"]["MachineID"].c_str(), "4") == 0))
		{
			injector::WriteMemory<DWORD>((0x11FA18 + mainModuleBase), 0x03, true);
		}
		else // MACHINE ID = 1
		{
			injector::WriteMemory<DWORD>((0x11FA18 + mainModuleBase), 0x00, true);
		}

	}, GameID::FNFSB);
#endif