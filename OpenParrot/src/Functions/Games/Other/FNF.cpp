#include <StdInc.h>
#include "Utility/InitFunction.h"
#include "Functions/Global.h"
#include "Utility\Hooking.Patterns.h"
#include <Xinput.h>
#include <math.h>
#include <dinput.h>

#pragma comment(lib, "Ws2_32.lib")

#define clamp( x, xmin, xmax ) min( xmax, max( x, xmin ) )

#define WINDOW_TITLE "Fast n Furious"
#define KEYBOARD_ADDR 0x31B6988

DWORD BaseAddress3 = 0x00400000;
int horizontal3 = 0;
int vertical3 = 0;
HWND hWndRT3 = 0;
bool movable = false;
bool polling = false;

static bool previousLeft = false;
static bool previousRight = false;
static bool previousUp = false;
static bool previousDown = false;
static bool gaspressed = false;
static bool brakepressed = false;

// controls
extern int* ffbOffset;
extern int* ffbOffset2;
extern int* ffbOffset3;
extern int* ffbOffset4;
// hooks ori
BOOL(__stdcall *original_SetWindowPos3)(HWND hWnd, HWND hWndInsertAfter, int X, int Y, int cx, int cy, UINT uFlags);
BOOL(__stdcall *original_CreateWindowExA3)(DWORD dwExStyle, LPCSTR lpClassName, LPCSTR lpWindowName, DWORD dwStyle, int X, int Y, int nWidth, int nHeight, HWND hWndParent, HMENU hMenu, HINSTANCE hInstance, LPVOID lpParam);
BOOL(__stdcall *original_DefWindowProcA3)(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
BOOL(__stdcall *original_SetCursorPosRT3)(int X, int Y);

DWORD WINAPI DefWindowProcART3(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	static int xClick;
	static int yClick;

	switch (message)
	{
	case WM_LBUTTONDOWN:
		// CURSOR NOT HIDDEN
		injector::WriteMemory<BYTE>((0x4BF1F + BaseAddress3), 0x01, true);

		SetCapture(hWnd);
		xClick = LOWORD(lParam);
		yClick = HIWORD(lParam);
		movable = true;
		break;

	case WM_LBUTTONUP:
		ReleaseCapture();
		movable = false;
		break;

	case WM_MOUSEMOVE:
	{
		if (movable)
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
		else
		{
			if (!polling)
			{
				return 0;
			}
			if (polling)
			{
				RECT rect;
				GetWindowRect(hWndRT3, &rect);
				int width = rect.right - rect.left;
				int iWheel0 = (((float)*ffbOffset2) - 128);
				float wheel = (iWheel0 * 0.0078125f);
				int iWheel = (int)(horizontal3 * 0.5 * wheel);
				int xPad = (int)(iWheel + (horizontal3 * 0.5));
				lParam = MAKELPARAM(xPad, (vertical3 * 0.5));
				break;
			}
		}
	}
	}
	return original_DefWindowProcA3(hWnd, message, wParam, lParam);
}

DWORD WINAPI InputRT3(LPVOID lpParam)
{
	int deltaTimer = 16;
	INT_PTR keyboardBuffer = (KEYBOARD_ADDR + BaseAddress3);


	while (true)
	{
		// ESCAPE QUITS GAME 
		if (GetAsyncKeyState(VK_ESCAPE) & 0x8000)
		{
			exit(0);
		}

		if (hWndRT3 == 0)
		{
			hWndRT3 = FindWindowA(NULL, WINDOW_TITLE);
		}
		HWND hWndTMP = GetForegroundWindow();
		if (hWndTMP == hWndRT3)
		{
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
			}
			else
			{
				if (previousDown)
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
			}
			else
			{
				if (previousUp)
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
			}
			else
			{
				if (previousLeft)
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
			}
			else
			{
				if (previousRight)
				{
					previousRight = false;
				}
			}

			if (!movable) // then poll ugly mouse input
			{
				polling = true;
				mouse_event(MOUSEEVENTF_MOVE, 0, 0, 0, 0);
				polling = false;
			}

			WORD vkey;
			// GAS
			vkey = 0x51; // Q key (qwerty A)
			if (*ffbOffset3 >= 5)
			{
				if (!gaspressed)
				{
					keybd_event(vkey, MapVirtualKey(vkey, MAPVK_VK_TO_VSC), 0, 0);
					gaspressed = true;
				}
			}
			else
			{
				if (gaspressed)
				{
					keybd_event(vkey, MapVirtualKey(vkey, MAPVK_VK_TO_VSC), KEYEVENTF_KEYUP, 0);
					gaspressed = false;
				}
			}

			// BRAKE
			vkey = 0x57; // W key (qwerty Z)
			if (*ffbOffset4 >= 5)
			{
				if (!brakepressed)
				{
					keybd_event(vkey, MapVirtualKey(vkey, MAPVK_VK_TO_VSC), 0, 0);
					brakepressed = true;
				}
			}
			else
			{
				if (brakepressed)
				{
					keybd_event(vkey, MapVirtualKey(vkey, MAPVK_VK_TO_VSC), KEYEVENTF_KEYUP, 0);
					brakepressed = false;
				}
			}
				
			//DEBUG//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
			//	info(true, "test values *ffbOffset2=0x%02X /  *iWheel=%d / *wheel=%f ", *ffbOffset2, iWheel, fx);
			//DEBUG//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

			Sleep(deltaTimer);
		}
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
			if (hWndRT3 == 0)
			{
				hWndRT3 = FindWindowA(NULL, WINDOW_TITLE);
			}
			HWND hWndTMP = GetForegroundWindow();
			if (hWndTMP == hWndRT3)
			{
				ShowCursor(TRUE);
				movable = true;
				original_SetWindowPos3(hWndRT3, HWND_BOTTOM, 0, 0, 0, 0, SWP_NOSIZE);
				ShowWindow(hWndRT3, SW_MINIMIZE);
			}
		}
	}
}

DWORD WINAPI CreateWindowExART3(DWORD dwExStyle, LPCSTR lpClassName, LPCSTR lpWindowName, DWORD dwStyle, int X, int Y, int nWidth, int nHeight, HWND hWndParent, HMENU hMenu, HINSTANCE hInstance, LPVOID lpParam)
{
	return original_CreateWindowExA3(dwExStyle, lpClassName, WINDOW_TITLE, 0x96C60000, X, Y, (nWidth+16), (nHeight+39), hWndParent, hMenu, hInstance, lpParam);
}

DWORD WINAPI SetCursorPosRT3(int X, int Y)
{
	RECT rect;
	GetWindowRect(hWndRT3, &rect);
	int width = rect.right - rect.left;
	int height = rect.bottom - rect.top;
	int windowcenterx = (rect.left + (width * 0.5));
	int windowcentery = (rect.top + (height * 0.5));
	return original_SetCursorPosRT3(windowcenterx, windowcentery);
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

	MH_Initialize();
	MH_CreateHookApi(L"user32.dll", "SetCursorPos", &SetCursorPosRT3, (void**)&original_SetCursorPosRT3);
	MH_CreateHookApi(L"user32.dll", "DefWindowProcA", &DefWindowProcART3, (void**)&original_DefWindowProcA3);
	MH_EnableHook(MH_ALL_HOOKS);

	CreateThread(NULL, 0, InputRT3, NULL, 0, NULL);

	if (ToBool(config["General"]["Windowed"]))
	{
		// CURSOR NOT HIDDEN
		//injector::WriteMemory<BYTE>((0x4BF1F + BaseAddress3), 0x01, true);
		
		CreateThread(NULL, 0, WindowRT3, NULL, 0, NULL);

		MH_Initialize();
		MH_CreateHookApi(L"user32.dll", "CreateWindowExA", &CreateWindowExART3, (void**)&original_CreateWindowExA3);
		MH_CreateHookApi(L"user32.dll", "SetWindowPos", &SetWindowPosRT3, (void**)&original_SetWindowPos3);
		MH_EnableHook(MH_ALL_HOOKS);
	}

}, GameID::FNF);
