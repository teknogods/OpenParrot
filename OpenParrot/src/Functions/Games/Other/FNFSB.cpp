#include <StdInc.h>
#include "Utility/InitFunction.h"
#include "Functions/Global.h"
#include "Utility\Hooking.Patterns.h"
#include <Xinput.h>
#include <math.h>
#include <dinput.h>

#pragma comment(lib, "Ws2_32.lib")

#define clamp( x, xmin, xmax ) min( xmax, max( x, xmin ) )

DWORD BaseAddress4 = 0x00400000;
int horizontal4 = 0;
int vertical4 = 0;
HWND hWndRT4 = 0;
// controls
extern int* ffbOffset;
extern int* ffbOffset2;
extern int* ffbOffset3;
extern int* ffbOffset4;
// hooks ori
BOOL(__stdcall *original_SetWindowPos4)(HWND hWnd, HWND hWndInsertAfter, int X, int Y, int cx, int cy, UINT uFlags);
BOOL(__stdcall *original_CreateWindowExA4)(DWORD dwExStyle, LPCSTR lpClassName, LPCSTR lpWindowName, DWORD dwStyle, int X, int Y, int nWidth, int nHeight, HWND hWndParent, HMENU hMenu, HINSTANCE hInstance, LPVOID lpParam);
BOOL(__stdcall *original_DefWindowProcA4)(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

DWORD WINAPI DefWindowProcART4(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
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
				original_SetWindowPos4(hWnd, HWND_TOP, xWindow, yWindow, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
			}
			break;
		}

		}
	return original_DefWindowProcA4(hWnd, message, wParam, lParam);
}

DWORD WINAPI InputRT4(LPVOID lpParam)
{
	int deltaTimer = 16;
	INT_PTR keyboardBuffer = (0x31943C8 + BaseAddress4);
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
				injector::WriteMemory<BYTE>((0x41599 + BaseAddress4), 0xEB, true);
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
				if (previousLeft == false)
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
				if (previousRight == false)
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
	return 1;
}

DWORD WINAPI SetWindowPosRT4(HWND hWnd, HWND hWndInsertAfter, int X, int Y, int cx, int cy, UINT uFlags)
{
	return 1;
}

static InitFunction FNFSBFunc([]()
{
	GetDesktopResolution(horizontal4, vertical4);

	// REMOVE ERROR MESSAGEBOX ON CLOSE
	injector::WriteMemory<BYTE>((0x584AA + BaseAddress4), 0xEB, true);

	// REPLACE SPACE KEY WITH ESC TO PREVENT EXITING LEVEL PREMATURELY
	injector::WriteMemory<BYTE>((0x4D1C5 + BaseAddress4), DIK_ESCAPE, true);

	// FIX file write on D:
	injector::WriteMemoryRaw((0x125B50 + BaseAddress4), "\x2E\x5C\x65\x72\x72\x6F\x72\x6C\x6F\x67\x2E\x74\x78\x74\x00", 15, true);

	// TEST KEY FIX (uses BACKSPACE now)
	injector::MakeNOP((0x4C426 + BaseAddress4), 14);
	injector::WriteMemory<BYTE>((0x4C435 + BaseAddress4), DIK_BACK, true);

	CreateThread(NULL, 0, InputRT4, NULL, 0, NULL);

	if (ToBool(config["General"]["Windowed"]))
	{
	// CURSOR NOT HIDDEN
	injector::WriteMemory<BYTE>((0x576CF + BaseAddress4), 0x01, true);
		
	CreateThread(NULL, 0, WindowRT4, NULL, 0, NULL);

	MH_Initialize();
	MH_CreateHookApi(L"user32.dll", "CreateWindowExA", &CreateWindowExART4, (void**)&original_CreateWindowExA4);
	MH_CreateHookApi(L"user32.dll", "SetCursorPos", &SetCursorPosRT4, NULL);
	MH_CreateHookApi(L"user32.dll", "SetWindowPos", &SetWindowPosRT4, (void**)&original_SetWindowPos4);
	MH_CreateHookApi(L"user32.dll", "DefWindowProcA", &DefWindowProcART4, (void**)&original_DefWindowProcA4);
	MH_EnableHook(MH_ALL_HOOKS);
	}

}, GameID::FNFSB);
