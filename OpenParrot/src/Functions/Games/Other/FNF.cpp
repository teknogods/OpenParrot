#include <StdInc.h>
#include "Utility/InitFunction.h"
#include "Functions/Global.h"
#include "Utility\Hooking.Patterns.h"
#include <Xinput.h>
#include <math.h>
#include <dinput.h>

#pragma comment(lib, "Ws2_32.lib")

#define clamp( x, xmin, xmax ) min( xmax, max( x, xmin ) )

DWORD BaseAddress3 = 0x00400000;
int horizontal3 = 0;
int vertical3 = 0;
HWND hWndRT3 = 0;
// controls
extern int* ffbOffset;
extern int* ffbOffset2;
extern int* ffbOffset3;
extern int* ffbOffset4;
// hooks ori
BOOL(__stdcall *original_SetWindowPos3)(HWND hWnd, HWND hWndInsertAfter, int X, int Y, int cx, int cy, UINT uFlags);
BOOL(__stdcall *original_CreateWindowExA3)(DWORD dwExStyle, LPCSTR lpClassName, LPCSTR lpWindowName, DWORD dwStyle, int X, int Y, int nWidth, int nHeight, HWND hWndParent, HMENU hMenu, HINSTANCE hInstance, LPVOID lpParam);
BOOL(__stdcall *original_DefWindowProcA3)(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

DWORD WINAPI DefWindowProcART3(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	static int xClick;
	static int yClick;

		switch (message)
		{
		case WM_LBUTTONDOWN:

			SetCapture(hWnd);
			xClick = LOWORD(lParam);
			yClick = HIWORD(lParam);
			break;

		case WM_LBUTTONUP:
			ReleaseCapture();
			break;

		case WM_MOUSEMOVE:
		{
			if (GetCapture() == hWnd)
			{
				RECT rcWindow;
				GetWindowRect(hWnd, &rcWindow);
				int xMouse = LOWORD(lParam);
				int yMouse = HIWORD(lParam);
				int xWindow = rcWindow.left + xMouse - xClick;
				int yWindow = rcWindow.top + yMouse - yClick;
				original_SetWindowPos3(hWnd, HWND_TOP, xWindow, yWindow, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
			}
			break;
		}

		}
	return original_DefWindowProcA3(hWnd, message, wParam, lParam);
}

DWORD WINAPI InputRT3(LPVOID lpParam)
{
	int deltaTimer = 2;
	INT_PTR keyboardBuffer = (0x31B6988 + BaseAddress3);
	bool previousLeft = false;
	bool previousRight = false;
	bool previousUp = false;
	bool previousDown = false;

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
				injector::WriteMemory<BYTE>((0x41599 + BaseAddress3), 0xEB, true);
			}
			// NITRO ( = START too)
			if (*ffbOffset & 0x100)
			{
				injector::WriteMemory<BYTE>((keyboardBuffer + DIK_N), 2, true);
				injector::WriteMemory<BYTE>((keyboardBuffer + DIK_SPACE), 2, true);
			}
			// SHIFT DOWN
			if (*ffbOffset & 0x2000)
			{
				if (!previousDown)
				{
					injector::WriteMemory<BYTE>((keyboardBuffer + DIK_DOWN), 2, true);
					previousDown = true;
				}

				else
				{
					previousDown = false;
				}
			}
			// SHIFT UP
			if (*ffbOffset & 0x1000)
			{
				if (!previousUp)
				{
					injector::WriteMemory<BYTE>((keyboardBuffer + DIK_UP), 2, true);
					previousUp = true;
				}

				else
				{
					previousUp = false;
				}
			}
			// BUTTON 1/ VIEW 1
			if (*ffbOffset & 0x200)
			{
				injector::WriteMemory<BYTE>((keyboardBuffer + DIK_F1), 2, true);
				injector::WriteMemory<BYTE>((keyboardBuffer + DIK_A), 2, true);
			}
			// BUTTON 2/ VIEW 2
			if (*ffbOffset & 0x400)
			{
				injector::WriteMemory<BYTE>((keyboardBuffer + DIK_F2), 2, true);
				injector::WriteMemory<BYTE>((keyboardBuffer + DIK_B), 2, true);
			}
			// BUTTON 3/ VIEW 3
			if (*ffbOffset & 0x800)
			{
				injector::WriteMemory<BYTE>((keyboardBuffer + DIK_E), 2, true);
				injector::WriteMemory<BYTE>((keyboardBuffer + DIK_W), 2, true);
			}
			// MENU LEFT
			if (*ffbOffset & 0x4000)
			{
				if (!previousLeft)
				{
					injector::WriteMemory<BYTE>((keyboardBuffer + DIK_LEFT), 2, true);
					previousLeft = true;
				}

				else
				{
					previousLeft = false;
				}
			}
			// MENU RIGHT
			if (*ffbOffset & 0x8000)
			{
				if (!previousRight)
				{
					injector::WriteMemory<BYTE>((keyboardBuffer + DIK_RIGHT), 2, true);
					previousRight = true;
				}

				else
				{
					previousRight = false;
				}
			}

			INPUT input[4];
			WORD vkey;
			// WHEEL
			if (*ffbOffset2 >= 0x85) // RIGHT
			{
				vkey = 0x66; // NUMPAD6 key down
				input[0].type = INPUT_KEYBOARD;
				input[0].ki.wScan = MapVirtualKey(vkey, MAPVK_VK_TO_VSC);
				input[0].ki.time = 0;
				input[0].ki.dwExtraInfo = 0;
				input[0].ki.wVk = vkey;
				input[0].ki.dwFlags = 0;
				vkey = 0x64; // NUMPAD4 key up
				input[1].type = INPUT_KEYBOARD;
				input[1].ki.wScan = MapVirtualKey(vkey, MAPVK_VK_TO_VSC);
				input[1].ki.time = 0;
				input[1].ki.dwExtraInfo = 0;
				input[1].ki.wVk = vkey;
				input[1].ki.dwFlags = KEYEVENTF_KEYUP;
			}
			else if (*ffbOffset2 <= 0x75) // LEFT
			{
				vkey = 0x66; // NUMPAD6 key up
				input[0].type = INPUT_KEYBOARD;
				input[0].ki.wScan = MapVirtualKey(vkey, MAPVK_VK_TO_VSC);
				input[0].ki.time = 0;
				input[0].ki.dwExtraInfo = 0;
				input[0].ki.wVk = vkey;
				input[0].ki.dwFlags = KEYEVENTF_KEYUP;
				vkey = 0x64; // NUMPAD4 key down
				input[1].type = INPUT_KEYBOARD;
				input[1].ki.wScan = MapVirtualKey(vkey, MAPVK_VK_TO_VSC);
				input[1].ki.time = 0;
				input[1].ki.dwExtraInfo = 0;
				input[1].ki.wVk = vkey;
				input[1].ki.dwFlags = 0;

			}
			else // CENTER
			{
				vkey = 0x66; // NUMPAD6 key up
				input[0].type = INPUT_KEYBOARD;
				input[0].ki.wScan = MapVirtualKey(vkey, MAPVK_VK_TO_VSC);
				input[0].ki.time = 0;
				input[0].ki.dwExtraInfo = 0;
				input[0].ki.wVk = vkey;
				input[0].ki.dwFlags = KEYEVENTF_KEYUP;
				vkey = 0x64; // NUMPAD4 key up
				input[1].type = INPUT_KEYBOARD;
				input[1].ki.wScan = MapVirtualKey(vkey, MAPVK_VK_TO_VSC);
				input[1].ki.time = 0;
				input[1].ki.dwExtraInfo = 0;
				input[1].ki.wVk = vkey;
				input[1].ki.dwFlags = KEYEVENTF_KEYUP;
			}

			// GAS
			if (*ffbOffset3 >= 5)
			{
				vkey = 0x51; // Q key (qwerty A)
				input[2].type = INPUT_KEYBOARD;
				input[2].ki.wScan = MapVirtualKey(vkey, MAPVK_VK_TO_VSC);
				input[2].ki.time = 0;
				input[2].ki.dwExtraInfo = 0;
				input[2].ki.wVk = vkey;
				input[2].ki.dwFlags = 0;
			}
			else
			{
				vkey = 0x51; // Q key (qwerty A)
				input[2].type = INPUT_KEYBOARD;
				input[2].ki.wScan = MapVirtualKey(vkey, MAPVK_VK_TO_VSC);
				input[2].ki.time = 0;
				input[2].ki.dwExtraInfo = 0;
				input[2].ki.wVk = vkey;
				input[2].ki.dwFlags = KEYEVENTF_KEYUP;
			}
		
			// BRAKE
			if (*ffbOffset4 >= 5)
			{
				vkey = 0x57; // W key (qwerty Z)
				input[3].type = INPUT_KEYBOARD;
				input[3].ki.wScan = MapVirtualKey(vkey, MAPVK_VK_TO_VSC);
				input[3].ki.time = 0;
				input[3].ki.dwExtraInfo = 0;
				input[3].ki.wVk = vkey;
				input[3].ki.dwFlags = 0;
			}
			else
			{
				vkey = 0x57; // W key (qwerty Z)
				input[3].type = INPUT_KEYBOARD;
				input[3].ki.wScan = MapVirtualKey(vkey, MAPVK_VK_TO_VSC);
				input[3].ki.time = 0;
				input[3].ki.dwExtraInfo = 0;
				input[3].ki.wVk = vkey;
				input[3].ki.dwFlags = KEYEVENTF_KEYUP;
			}

			SendInput(4, input, sizeof(INPUT));

			//DEBUG//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
			//	info(true, "test values *ffbOffset2=0x%02X /  *ffbOffset3=0x%02X / *ffbOffset4=0x%02X ", *ffbOffset2, *ffbOffset3, *ffbOffset4);
			//DEBUG//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

			Sleep(deltaTimer);
		}
	return 0;
}

DWORD WINAPI WindowRT3(LPVOID lpParam)
{
	while (true)
	{
			// RIGHT-CLICK MINIMIZES WINDOW
			if (GetAsyncKeyState(VK_RBUTTON) & 0x8000)
			{
				HWND hWndTMP = GetForegroundWindow();
				if (hWndRT3 == 0)
				{
					hWndRT3 = FindWindowA(NULL, "Fast n Furious");
				}
				if (hWndTMP == hWndRT3)
				{
					original_SetWindowPos3(hWndRT3, HWND_BOTTOM, 0, 0, 0, 0, SWP_NOSIZE);
					ShowWindow(hWndRT3, SW_MINIMIZE);
				}
			}
	}
}

DWORD WINAPI CreateWindowExART3(DWORD dwExStyle, LPCSTR lpClassName, LPCSTR lpWindowName, DWORD dwStyle, int X, int Y, int nWidth, int nHeight, HWND hWndParent, HMENU hMenu, HINSTANCE hInstance, LPVOID lpParam)
{
	return original_CreateWindowExA3(dwExStyle, lpClassName, "Fast n Furious", 0x96C60000, X, Y, (nWidth+16), (nHeight+39), hWndParent, hMenu, hInstance, lpParam);
}

DWORD WINAPI SetCursorPosRT3(int X, int Y)
{
	return 1;
}

DWORD WINAPI SetWindowPosRT3(HWND hWnd, HWND hWndInsertAfter, int X, int Y, int cx, int cy, UINT uFlags)
{
	return 1;
}

static InitFunction FNFFunc([]()
{
	GetDesktopResolution(horizontal3, vertical3);

	// REMOVE ERROR MESSAGEBOX ON CLOSE
	injector::WriteMemory<BYTE>((0x4CC2A + BaseAddress3), 0xEB, true);

	// REPLACE SPACE KEY WITH ESC TO PREVENT EXITING LEVEL PREMATURELY
	injector::WriteMemory<BYTE>((0x421B2 + BaseAddress3), DIK_ESCAPE, true);

	// FIX file write on D:
	injector::WriteMemoryRaw((0xD3E6C + BaseAddress3), "\x2E\x5C\x65\x72\x72\x6F\x72\x6C\x6F\x67\x2E\x74\x78\x74\x00", 15, true);

	// TEST KEY FIX (uses BACKSPACE now)
	injector::MakeNOP((0x4159B + BaseAddress3), 14);
	injector::WriteMemory<BYTE>((0x415AA + BaseAddress3), DIK_BACK, true);

	CreateThread(NULL, 0, InputRT3, NULL, 0, NULL);

	if (ToBool(config["General"]["Windowed"]))
	{
	// CURSOR NOT HIDDEN
	injector::WriteMemory<BYTE>((0x4BF1F + BaseAddress3), 0x01, true);
		
	CreateThread(NULL, 0, WindowRT3, NULL, 0, NULL);

	MH_Initialize();
	MH_CreateHookApi(L"user32.dll", "CreateWindowExA", &CreateWindowExART3, (void**)&original_CreateWindowExA3);
	MH_CreateHookApi(L"user32.dll", "SetCursorPos", &SetCursorPosRT3, NULL);
	MH_CreateHookApi(L"user32.dll", "SetWindowPos", &SetWindowPosRT3, (void**)&original_SetWindowPos3);
	MH_CreateHookApi(L"user32.dll", "DefWindowProcA", &DefWindowProcART3, (void**)&original_DefWindowProcA3);
	MH_EnableHook(MH_ALL_HOOKS);
	}

}, GameID::FNF);
