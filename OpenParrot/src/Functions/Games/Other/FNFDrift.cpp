#include <StdInc.h>
#include "Utility/InitFunction.h"
#include "Functions/Global.h"
#include "Utility\Hooking.Patterns.h"
#include <Xinput.h>
#include <math.h>
#include <dinput.h>

#pragma comment(lib, "Ws2_32.lib")

#define clamp( x, xmin, xmax ) min( xmax, max( x, xmin ) )

DWORD BaseAddress = 0x00400000;
int horizontal = 0;
int vertical = 0;
HWND hWndRT = 0;
HCURSOR cursorhndle;
// controls
extern int* ffbOffset;
extern int* ffbOffset2;
extern int* ffbOffset3;
extern int* ffbOffset4;
// hooks ori
BOOL(__stdcall *original_SetWindowPos)(HWND hWnd, HWND hWndInsertAfter, int X, int Y, int cx, int cy, UINT uFlags);
BOOL(__stdcall *original_CreateWindowExA)(DWORD dwExStyle, LPCSTR lpClassName, LPCSTR lpWindowName, DWORD dwStyle, int X, int Y, int nWidth, int nHeight, HWND hWndParent, HMENU hMenu, HINSTANCE hInstance, LPVOID lpParam);

DWORD WINAPI InputRT(LPVOID lpParam)
{
	int deltaTimer = 16;
	INT_PTR keyboardBuffer = (0x41B5920 + BaseAddress);
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
		// TEST (shift+numlock TO FIX)
		if (*ffbOffset & 0x01)
		{
			// injector::WriteMemory<BYTE>((keyboardBuffer + DIK_NUMLOCK), 2, true);
			// injector::WriteMemory<BYTE>((keyboardBuffer + DIK_LSHIFT), 2, true);			
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
			injector::WriteMemory<BYTE>((keyboardBuffer + DIK_A), 2, true);
			injector::WriteMemory<BYTE>((keyboardBuffer + DIK_F1), 2, true);
		}
		// BUTTON 2/ VIEW 2
		if (*ffbOffset & 0x400)
		{
			injector::WriteMemory<BYTE>((keyboardBuffer + DIK_B), 2, true);
			injector::WriteMemory<BYTE>((keyboardBuffer + DIK_F2), 2, true);
		}
		// BUTTON 3/ VIEW 3
		if (*ffbOffset & 0x800)
		{
			injector::WriteMemory<BYTE>((keyboardBuffer + DIK_E), 2, true);
			injector::WriteMemory<BYTE>((keyboardBuffer + DIK_F3), 2, true);
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
		// WHEEL
		int iWheel = (((float)*ffbOffset2) - 128);
		float wheel = (iWheel * 0.0078125f);	
		injector::WriteMemory<float>((0x19A300 + BaseAddress), wheel, true);
		injector::WriteMemory<BYTE>((0x41B85B0 + BaseAddress), iWheel, true);
		// GAS
		BYTE gas = ((float)*ffbOffset3) / 255.0f;
		int iGas = (int)(gas * 255);
		injector::WriteMemory<BYTE>((0x41B85B1 + BaseAddress), iGas, true);
		// BRAKE
		BYTE brake = ((float)*ffbOffset4) / 255.0f;
		int iBrake = (int)(brake * 255);
		injector::WriteMemory<BYTE>((0x41B85B2 + BaseAddress), iBrake, true);
		injector::WriteMemory<float>((0x19A310 + BaseAddress), gas - brake, true);

		//DEBUG//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		//	info(true, "test value %f ", test);
		//DEBUG//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

		Sleep(deltaTimer);
	}
	
	return 0;
}

DWORD WINAPI WindowRT(LPVOID lpParam)
{
	while (true)
	{
			// LEFT-CLICK MOVES WINDOW FROM TOP-LEFT CORNER
		if (GetAsyncKeyState(VK_LBUTTON) & 0x8000)
		{
			HWND hWndTMP = GetForegroundWindow();
			if (hWndRT == 0)
			{
				hWndRT = FindWindowA(NULL, "FNF Drift");
			}
			if (hWndTMP == hWndRT)
			{
				POINT point;
				GetCursorPos(&point);
				RECT rect;
				GetWindowRect(hWndRT, &rect);
				int width = rect.right - rect.left;
				int height = rect.bottom - rect.top;
				LPARAM blah = MAKELPARAM(point.x, point.y);
				int xClick = LOWORD(blah) + 10;
				int yClick = HIWORD(blah);
				if ((xClick + width) > horizontal)
				{
					xClick = ((horizontal - width) + 10);
				}
				if ((yClick + height) > vertical)
				{
					yClick = (vertical - height);
				}
				original_SetWindowPos(hWndRT, HWND_TOP, xClick, yClick, width, height, SWP_NOSIZE);
				SetForegroundWindow(hWndRT);

			}
		}
			// RIGHT-CLICK MINIMIZES WINDOW
			if (GetAsyncKeyState(VK_RBUTTON) & 0x8000)
			{
				HWND hWndTMP = GetForegroundWindow();
				if (hWndRT == 0)
				{
					hWndRT = FindWindowA(NULL, "FNF Drift");
				}
				if (hWndTMP == hWndRT)
				{
					original_SetWindowPos(hWndRT, HWND_BOTTOM, 0, 0, 0, 0, SWP_NOSIZE);
					ShowWindow(hWndRT, SW_MINIMIZE);
				}
			}
	}
}

DWORD WINAPI CreateWindowExART(DWORD dwExStyle, LPCSTR lpClassName, LPCSTR lpWindowName, DWORD dwStyle, int X, int Y, int nWidth, int nHeight, HWND hWndParent, HMENU hMenu, HINSTANCE hInstance, LPVOID lpParam)
{
	return original_CreateWindowExA(dwExStyle, lpClassName, "FNF Drift", 0x96C60000, X, Y, (nWidth+16), (nHeight+39), hWndParent, hMenu, hInstance, lpParam);
}

HCURSOR WINAPI SetCursorRT(int X, int Y)
{
	return cursorhndle;
}

DWORD WINAPI SetCursorPosRT(int X, int Y)
{
	return 1;
}

DWORD WINAPI SetWindowPosRT(HWND hWnd, HWND hWndInsertAfter, int X, int Y, int cx, int cy, UINT uFlags)
{
	return 1;
}

static InitFunction FNFDriftFunc([]()
{
	GetDesktopResolution(horizontal, vertical);
	cursorhndle = GetCursor();

	// DISABLE MOUSE AND JOYSTICK
	injector::WriteMemory<BYTE>((0x553FD + BaseAddress), 0x13, true);

	// PATCH CODE FOR GAS AND BRAKE
	injector::WriteMemoryRaw((0x67E44 + BaseAddress), "\xA1\x10\xA3\x59\x00\x89\x86\xD4\x00\x00\x00\xEB\x17", 13, true);
	injector::MakeNOP((0x67E85 + BaseAddress), 10);
	injector::MakeNOP((0x67EB8 + BaseAddress), 6);

	// FIX FOCUS CRASH
	injector::MakeNOP((0x63E17 + BaseAddress), 6);

	// REPLACE SPACE KEY WITH ESC TO PREVENT EXITING LEVEL PREMATURELY
	injector::WriteMemory<BYTE>((0x58E8A + BaseAddress), DIK_ESCAPE, true);

	// REMOVE ERROR MESSAGEBOX ON CLOSE
	injector::WriteMemory<BYTE>((0x6431D + BaseAddress), 0xEB, true);

	CreateThread(NULL, 0, InputRT, NULL, 0, NULL);

	if (ToBool(config["General"]["Windowed"]))
	{
	// CURSOR NOT HIDDEN
	injector::WriteMemory<BYTE>((0x63F6F + BaseAddress), 0x01, true);
	// SETWINDOWPOS DISABLE
	injector::WriteMemory<BYTE>((0xCB596 + BaseAddress), 0x74, true);
		
	CreateThread(NULL, 0, WindowRT, NULL, 0, NULL);

	MH_Initialize();
	MH_CreateHookApi(L"user32.dll", "CreateWindowExA", &CreateWindowExART, (void**)&original_CreateWindowExA);
	MH_CreateHookApi(L"user32.dll", "SetCursorPos", &SetCursorPosRT, NULL);
	MH_CreateHookApi(L"user32.dll", "SetCursor", &SetCursorRT, NULL);
	MH_CreateHookApi(L"user32.dll", "SetWindowPos", &SetWindowPosRT, (void**)&original_SetWindowPos);
	MH_EnableHook(MH_ALL_HOOKS);
	}

}, GameID::FNFDrift);
