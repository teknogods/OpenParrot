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

// controls
extern int* ffbOffset;
extern int* ffbOffset2;
extern int* ffbOffset3;
extern int* ffbOffset4;
// hooks ori
BOOL(__stdcall *original_SetWindowPos4)(HWND hWnd, HWND hWndInsertAfter, int X, int Y, int cx, int cy, UINT uFlags);
BOOL(__stdcall *original_CreateWindowExA4)(DWORD dwExStyle, LPCSTR lpClassName, LPCSTR lpWindowName, DWORD dwStyle, int X, int Y, int nWidth, int nHeight, HWND hWndParent, HMENU hMenu, HINSTANCE hInstance, LPVOID lpParam);
BOOL(__stdcall *original_DefWindowProcA4)(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
BOOL(__stdcall *original_SetCursorPosRT4)(int X, int Y);

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
	INT_PTR keyboardBuffer = (0x31943C8 + BaseAddress4);

	while (true)
	{
		// ESCAPE QUITS GAME 
		if (GetAsyncKeyState(VK_ESCAPE) & 0x8000)
		{
			exit(0);
		}

		// buttons see bitwise values in TPui//RawThrills.cs
		// START 
		if (*ffbOffset & 0x08)
		{
			injector::WriteMemory<BYTE>((keyboardBuffer + DIK_SPACE), 2, true);
		}
		// TEST
		if (*ffbOffset & 0x01)
		{
			injector::WriteMemory<BYTE>((0x4C424 + BaseAddress4), 0xEB, true);
		}
		// NITRO ( = START too)
		if (*ffbOffset & 0x100)
		{
			if (button1pressed == false)
			{
				injector::WriteMemory<BYTE>((keyboardBuffer + DIK_N), 2, true);
				injector::WriteMemory<BYTE>((keyboardBuffer + DIK_SPACE), 2, true);
				button1pressed = true;
			}
		}
		else
		{
			if (button1pressed == true)
			{
				button1pressed = false;
			}
		}
		// SHIFT DOWN
		if (*ffbOffset & 0x2000)
		{
			if (previousDown == false)
			{
				injector::WriteMemory<BYTE>((keyboardBuffer + DIK_DOWN), 2, true);
				previousDown = true;
			}
		}
		else
		{
			if (previousDown == true)
			{
				previousDown = false;
			}
		}
		// SHIFT UP
		if (*ffbOffset & 0x1000)
		{
			if (previousUp == false)
			{
				injector::WriteMemory<BYTE>((keyboardBuffer + DIK_UP), 2, true);
				previousUp = true;
			}
		}
		else
		{
			if (previousUp == true)
			{
				previousUp = false;
			}
		}
		// BUTTON 1/ VIEW
		if (*ffbOffset & 0x200)
		{
			if (button2pressed == false)
			{
				injector::MakeNOP((0x35BA5 + BaseAddress4), 2);
				injector::WriteMemory<BYTE>((keyboardBuffer + DIK_F), 2, true);
				keybd_event(0x70, MapVirtualKey(0x70, MAPVK_VK_TO_VSC), 0, 0);
				button2pressed = true;
			}
		}
		else
		{
			if (button2pressed == true)
			{
				injector::WriteMemoryRaw((0x35BA5 + BaseAddress4), "\x74\x46", 2, true);
				keybd_event(0x70, MapVirtualKey(0x70, MAPVK_VK_TO_VSC), KEYEVENTF_KEYUP, 0);
				button2pressed = false;
			}
		}
		// BUTTON 2/ MUSIC
		if (*ffbOffset & 0x400)
		{
			if (button3pressed == false)
			{
				injector::WriteMemory<BYTE>((keyboardBuffer + DIK_B), 2, true);
				keybd_event(0xDC, MapVirtualKey(0xDC, MAPVK_VK_TO_VSC), 0, 0);
				button3pressed = true;
			}
		}
		else
		{
			if (button3pressed == true)
			{
				keybd_event(0xDC, MapVirtualKey(0xDC, MAPVK_VK_TO_VSC), KEYEVENTF_KEYUP, 0);
				button3pressed = false;
			}
		}
		// BUTTON 3/ OTHER
		if (*ffbOffset & 0x800)
		{
			if (button4pressed == false)
			{
				injector::WriteMemory<BYTE>((keyboardBuffer + DIK_E), 2, true);
				injector::WriteMemory<BYTE>((keyboardBuffer + DIK_J), 2, true);
				button4pressed = true;
			}
		}
		else
		{
			if (button4pressed == true)
			{
				button4pressed = false;
			}
		}
		// MENU LEFT
		if (*ffbOffset & 0x4000)
		{
			if (previousLeft == false)
			{
				injector::WriteMemory<BYTE>((keyboardBuffer + DIK_LEFT), 2, true);
				previousLeft = true;
			}
		}
		else
		{
			if (previousLeft == true)
			{
				previousLeft = false;
			}
		}
		// MENU RIGHT
		if (*ffbOffset & 0x8000)
		{
			if (previousRight == false)
			{
				injector::WriteMemory<BYTE>((keyboardBuffer + DIK_RIGHT), 2, true);
				previousRight = true;
			}
		}
		else
		{
			if (previousRight == true)
			{
				previousRight = false;
			}
		}

		RECT rect;
		GetWindowRect(hWndRT4, &rect);
		int width = rect.right - rect.left;
		int height = rect.bottom - rect.top;
		int windowcenterx = (rect.left + (width * 0.5));
		int windowcentery = (rect.top + (height * 0.5));
		// WHEEL
		int iWheel0 = (((float)*ffbOffset2) - 128);
		float wheel = (iWheel0 * 0.0078125f);
		int iWheel = (int)((width - 20) * 0.5 * wheel);
		double fx = (float)((wheel) * (65535.0f / horizontal4));

		if (movable4 == false)
		{
			polling4 = true;
			mouse_event(MOUSEEVENTF_MOVE, fx, 0, 0, 0);
			polling4 = false;
		}

		// GAS
		if (*ffbOffset3 >= 5)
		{
			if (gaspressed == false)
			{
				keybd_event(0x51, MapVirtualKey(0x51, MAPVK_VK_TO_VSC), 0, 0); // Q key (qwerty A)
				keybd_event(0x41, MapVirtualKey(0x41, MAPVK_VK_TO_VSC), 0, 0); // A key
				gaspressed = true;
			}
		}
		else
		{
			if (gaspressed == true)
			{
				keybd_event(0x51, MapVirtualKey(0x51, MAPVK_VK_TO_VSC), KEYEVENTF_KEYUP, 0);
				keybd_event(0x41, MapVirtualKey(0x41, MAPVK_VK_TO_VSC), KEYEVENTF_KEYUP, 0);
				gaspressed = false;
			}
		}

		// BRAKE
		if (*ffbOffset4 >= 5)
		{
			if (brakepressed == false)
			{
				keybd_event(0x57, MapVirtualKey(0x57, MAPVK_VK_TO_VSC), 0, 0); // W key (qwerty Z)
				keybd_event(0x5A, MapVirtualKey(0x5A, MAPVK_VK_TO_VSC), 0, 0); // Z key
				brakepressed = true;
			}
		}
		else
		{
			if (brakepressed == true)
			{
				keybd_event(0x57, MapVirtualKey(0x57, MAPVK_VK_TO_VSC), KEYEVENTF_KEYUP, 0);
				keybd_event(0x5A, MapVirtualKey(0x5A, MAPVK_VK_TO_VSC), KEYEVENTF_KEYUP, 0);
				brakepressed = false;
			}
		}

		//DEBUG//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		//	info(true, "test values *ffbOffset2=0x%02X /  *iWheel=%d / *wheel=%f ", *ffbOffset2, iWheel, fx);
		//DEBUG//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

		SleepEx(deltaTimer,1);
	}
//	return 0;
}


DWORD WINAPI WindowRT4(LPVOID lpParam)
{
	while (true)
	{
		// RIGHT-CLICK MINIMIZES WINDOW
		if (GetAsyncKeyState(VK_RBUTTON) & 0x8000)
		{
			HWND hWndTMP = GetForegroundWindow();
			if (hWndRT4 == 0)
			{
				hWndRT4 = FindWindowA(NULL, "Fast n Furious SuperBikes");
			}
			if (hWndTMP == hWndRT4)
			{
				ShowCursor(TRUE);
				original_SetWindowPos4(hWndRT4, HWND_BOTTOM, 0, 0, 0, 0, SWP_NOSIZE);
				ShowWindow(hWndRT4, SW_MINIMIZE);
			}
		}
	}
}

DWORD WINAPI CreateWindowExART4(DWORD dwExStyle, LPCSTR lpClassName, LPCSTR lpWindowName, DWORD dwStyle, int X, int Y, int nWidth, int nHeight, HWND hWndParent, HMENU hMenu, HINSTANCE hInstance, LPVOID lpParam)
{
	return original_CreateWindowExA4(dwExStyle, lpClassName, "Fast n Furious SuperBikes", 0x96C60000, X, Y, (nWidth+16), (nHeight+39), hWndParent, hMenu, hInstance, lpParam);
}

DWORD WINAPI SetCursorPosRT4(int X, int Y)
{
	return 0;
}

DWORD WINAPI SetWindowPosRT4(HWND hWnd, HWND hWndInsertAfter, int X, int Y, int cx, int cy, UINT uFlags)
{
	return 1;
}

static InitFunction FNFSBFunc([]()
{
	GetDesktopResolution(horizontal4, vertical4);

	// BUTTON VIEW 1 HACK
	injector::WriteMemory<BYTE>((0x3A51E + BaseAddress4), DIK_F1, true);

	// BUTTON VIEW 3 HACK
	injector::WriteMemory<BYTE>((0xB7BC9 + BaseAddress4), DIK_F4, true);

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

	MH_Initialize();
	MH_CreateHookApi(L"user32.dll", "SetCursorPos", &SetCursorPosRT4, (void**)&original_SetCursorPosRT4);
	MH_CreateHookApi(L"user32.dll", "DefWindowProcA", &DefWindowProcART4, (void**)&original_DefWindowProcA4);
	MH_CreateHookApi(L"user32.dll", "SetWindowPos", &SetWindowPosRT4, (void**)&original_SetWindowPos4);
	MH_EnableHook(MH_ALL_HOOKS);

	CreateThread(NULL, 0, InputRT4, NULL, 0, NULL);

	if (ToBool(config["General"]["Windowed"]))
	{
	CreateThread(NULL, 0, WindowRT4, NULL, 0, NULL);

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