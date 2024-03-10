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

#pragma comment(lib, "Ws2_32.lib")

DWORD BaseAddress4 = 0x00400000;
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
	int deltaTimer = 16;
	//INT_PTR keyboardBuffer = (0x31943C8 + BaseAddress4);

	while (true)
	{
		// buttons see bitwise values in TPui//RawThrills.cs
		// START 
		//if (*ffbOffset & 0x08)
		//{
		//	injector::WriteMemory<BYTE>((keyboardBuffer + DIK_SPACE), 2, true);
		//}
		//// TEST
		//if (*ffbOffset & 0x01)
		//{
		//	injector::WriteMemory<BYTE>((0x4C424 + BaseAddress4), 0xEB, true);
		//}
		//// NITRO ( = START too)
		//if (*ffbOffset & 0x100)
		//{
		//	if (button1pressed == false)
		//	{
		//		injector::WriteMemory<BYTE>((keyboardBuffer + DIK_N), 2, true);
		//		injector::WriteMemory<BYTE>((keyboardBuffer + DIK_SPACE), 2, true);
		//		button1pressed = true;
		//	}
		//}
		//else
		//{
		//	if (button1pressed == true)
		//	{
		//		button1pressed = false;
		//	}
		//}
		//// SHIFT DOWN
		//if (*ffbOffset & 0x2000)
		//{
		//	if (previousDown == false)
		//	{
		//		injector::WriteMemory<BYTE>((keyboardBuffer + DIK_DOWN), 2, true);
		//		previousDown = true;
		//	}
		//}
		//else
		//{
		//	if (previousDown == true)
		//	{
		//		previousDown = false;
		//	}
		//}
		//// SHIFT UP
		//if (*ffbOffset & 0x1000)
		//{
		//	if (previousUp == false)
		//	{
		//		injector::WriteMemory<BYTE>((keyboardBuffer + DIK_UP), 2, true);
		//		previousUp = true;
		//	}
		//}
		//else
		//{
		//	if (previousUp == true)
		//	{
		//		previousUp = false;
		//	}
		//}
		//// BUTTON 1/ VIEW
		//if (*ffbOffset & 0x200)
		//{
		//	if (button2pressed == false)
		//	{
		//		injector::MakeNOP((0x35BA5 + BaseAddress4), 2);
		//		injector::WriteMemory<BYTE>((keyboardBuffer + DIK_F), 2, true);
		//		keybd_event(0x70, MapVirtualKey(0x70, MAPVK_VK_TO_VSC), 0, 0);
		//		button2pressed = true;
		//	}
		//}
		//else
		//{
		//	if (button2pressed == true)
		//	{
		//		injector::WriteMemoryRaw((0x35BA5 + BaseAddress4), "\x74\x46", 2, true);
		//		keybd_event(0x70, MapVirtualKey(0x70, MAPVK_VK_TO_VSC), KEYEVENTF_KEYUP, 0);
		//		button2pressed = false;
		//	}
		//}
		//// BUTTON 2/ MUSIC
		//if (*ffbOffset & 0x400)
		//{
		//	if (button3pressed == false)
		//	{
		//		injector::WriteMemory<BYTE>((keyboardBuffer + DIK_B), 2, true);
		//		keybd_event(0xDC, MapVirtualKey(0xDC, MAPVK_VK_TO_VSC), 0, 0);
		//		button3pressed = true;
		//	}
		//}
		//else
		//{
		//	if (button3pressed == true)
		//	{
		//		keybd_event(0xDC, MapVirtualKey(0xDC, MAPVK_VK_TO_VSC), KEYEVENTF_KEYUP, 0);
		//		button3pressed = false;
		//	}
		//}
		//// BUTTON 3/ OTHER
		//if (*ffbOffset & 0x800)
		//{
		//	if (button4pressed == false)
		//	{
		//		injector::WriteMemory<BYTE>((keyboardBuffer + DIK_E), 2, true);
		//		injector::WriteMemory<BYTE>((keyboardBuffer + DIK_J), 2, true);
		//		button4pressed = true;
		//	}
		//}
		//else
		//{
		//	if (button4pressed == true)
		//	{
		//		button4pressed = false;
		//	}
		//}
		//// MENU LEFT
		//if (*ffbOffset & 0x4000)
		//{
		//	if (previousLeft == false)
		//	{
		//		injector::WriteMemory<BYTE>((keyboardBuffer + DIK_LEFT), 2, true);
		//		previousLeft = true;
		//	}
		//}
		//else
		//{
		//	if (previousLeft == true)
		//	{
		//		previousLeft = false;
		//	}
		//}
		//// MENU RIGHT
		//if (*ffbOffset & 0x8000)
		//{
		//	if (previousRight == false)
		//	{
		//		injector::WriteMemory<BYTE>((keyboardBuffer + DIK_RIGHT), 2, true);
		//		previousRight = true;
		//	}
		//}
		//else
		//{
		//	if (previousRight == true)
		//	{
		//		previousRight = false;
		//	}
		//}

		//RECT rect;
		//GetWindowRect(hWndRT4, &rect);
		//int width = rect.right - rect.left;
		//int height = rect.bottom - rect.top;
		//int windowcenterx = (rect.left + (width * 0.5));
		//int windowcentery = (rect.top + (height * 0.5));
		//// WHEEL
		//int iWheel0 = (((float)*ffbOffset2) - 128);
		//float wheel = (iWheel0 * 0.0078125f);
		//int iWheel = (int)((width - 20) * 0.5 * wheel);
		//double fx = (float)((wheel) * (65535.0f / horizontal4));

		//if (movable4 == false)
		//{
		//	polling4 = true;
		//	mouse_event(MOUSEEVENTF_MOVE, fx, 0, 0, 0);
		//	polling4 = false;
		//}

		//// GAS
		//if (*ffbOffset3 >= 5)
		//{
		//	if (gaspressed == false)
		//	{
		//		keybd_event(0x51, MapVirtualKey(0x51, MAPVK_VK_TO_VSC), 0, 0); // Q key (qwerty A)
		//		keybd_event(0x41, MapVirtualKey(0x41, MAPVK_VK_TO_VSC), 0, 0); // A key
		//		gaspressed = true;
		//	}
		//}
		//else
		//{
		//	if (gaspressed == true)
		//	{
		//		keybd_event(0x51, MapVirtualKey(0x51, MAPVK_VK_TO_VSC), KEYEVENTF_KEYUP, 0);
		//		keybd_event(0x41, MapVirtualKey(0x41, MAPVK_VK_TO_VSC), KEYEVENTF_KEYUP, 0);
		//		gaspressed = false;
		//	}
		//}

		//// BRAKE
		//if (*ffbOffset4 >= 5)
		//{
		//	if (brakepressed == false)
		//	{
		//		keybd_event(0x57, MapVirtualKey(0x57, MAPVK_VK_TO_VSC), 0, 0); // W key (qwerty Z)
		//		keybd_event(0x5A, MapVirtualKey(0x5A, MAPVK_VK_TO_VSC), 0, 0); // Z key
		//		brakepressed = true;
		//	}
		//}
		//else
		//{
		//	if (brakepressed == true)
		//	{
		//		keybd_event(0x57, MapVirtualKey(0x57, MAPVK_VK_TO_VSC), KEYEVENTF_KEYUP, 0);
		//		keybd_event(0x5A, MapVirtualKey(0x5A, MAPVK_VK_TO_VSC), KEYEVENTF_KEYUP, 0);
		//		brakepressed = false;
		//	}
		//}


		// WHEEL
		int iWheel = (*ffbOffset2 - 128);
		float wheel = (iWheel * 0.0078125f);
		////injector::WriteMemory<float>((0x31b672c + mainModuleBase), wheel, true); // THIS MESSES UP GAS AND IS NOT NEEDED, DO NOT UNCOMMENT
		//// add 128 back to iWheel, somehow the game wants 0 to 255 for the byte
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

		//DEBUG//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		//	info(true, "test values *ffbOffset2=0x%02X /  *iWheel=%d / *wheel=%f ", *ffbOffset2, iWheel, fx);
		//DEBUG//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

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
		// BUTTON VIEW 1 HACK
		//injector::WriteMemory<BYTE>((0x3A51E + BaseAddress4), DIK_F1, true);

		//// BUTTON VIEW 3 HACK
		//injector::WriteMemory<BYTE>((0xB7BC9 + BaseAddress4), DIK_F4, true);

		// DISABLE CURSOR RESET
		injector::WriteMemory<BYTE>((0x57C19 + BaseAddress4), 0xEB, true);

		// REMOVE ERROR MESSAGEBOX ON CLOSE
		injector::WriteMemory<BYTE>((0x584AA + BaseAddress4), 0xEB, true);

		// REPLACE SPACE KEY WITH ESC TO PREVENT EXITING LEVEL PREMATURELY
		injector::WriteMemory<BYTE>((0x4D1C5 + BaseAddress4), DIK_ESCAPE, true);

		// FIX file write on D:
		injector::WriteMemoryRaw((0x125B50 + BaseAddress4), "\x2E\x5C\x65\x72\x72\x6F\x72\x6C\x6F\x67\x2E\x74\x78\x74\x00", 15, true);
		injector::WriteMemoryRaw((0x11FAD4 + BaseAddress4), "\x2E\x5C\x76\x65\x72\x73\x69\x6F\x6E\x2E\x74\x78\x74\x00", 14, true);

		// TEST KEY FIX (uses BACKSPACE now)
		injector::MakeNOP((0x4C426 + BaseAddress4), 14);
		injector::WriteMemory<BYTE>((0x4C435 + BaseAddress4), DIK_BACK, true);

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
			injector::WriteMemory<DWORD>((0x1522F8 + BaseAddress4), horizontal4, true);
			injector::WriteMemory<DWORD>((0x1522FC + BaseAddress4), vertical4, true);
		}

		// MACHINE ID setting
		if ((strcmp(config["Network"]["MachineID"].c_str(), "2") == 0))
		{
			injector::WriteMemory<DWORD>((0x11FA18 + BaseAddress4), 0x01, true);
		}
		else if ((strcmp(config["Network"]["MachineID"].c_str(), "3") == 0))
		{
			injector::WriteMemory<DWORD>((0x11FA18 + BaseAddress4), 0x02, true);
		}
		else if ((strcmp(config["Network"]["MachineID"].c_str(), "4") == 0))
		{
			injector::WriteMemory<DWORD>((0x11FA18 + BaseAddress4), 0x03, true);
		}
		else // MACHINE ID = 1
		{
			injector::WriteMemory<DWORD>((0x11FA18 + BaseAddress4), 0x00, true);
		}

	}, GameID::FNFSB);
#endif