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

static bool previousLeft = false;
static bool previousRight = false;
static bool previousUp = false;
static bool previousDown = false;
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
BOOL(__stdcall* original_SetWindowPos)(HWND hWnd, HWND hWndInsertAfter, int X, int Y, int cx, int cy, UINT uFlags);
BOOL(__stdcall* original_CreateWindowExA)(DWORD dwExStyle, LPCSTR lpClassName, LPCSTR lpWindowName, DWORD dwStyle, int X, int Y, int nWidth, int nHeight, HWND hWndParent, HMENU hMenu, HINSTANCE hInstance, LPVOID lpParam);

DWORD WINAPI InputRT(LPVOID lpParam)
{
	int deltaTimer = 16;
	INT_PTR keyboardBuffer = (0x41B5920 + BaseAddress);

	while (true)
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
			injector::WriteMemory<BYTE>((0x57B33 + BaseAddress), 0xEB, true);
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
		// BUTTON 1/ VIEW 1
		if (*ffbOffset & 0x200)
		{
			if (button2pressed == false)
			{
				injector::WriteMemory<BYTE>((keyboardBuffer + DIK_A), 2, true);
				injector::WriteMemory<BYTE>((keyboardBuffer + DIK_F1), 2, true);
				button2pressed = true;
			}
		}
		else
		{
			if (button2pressed == true)
			{
				button2pressed = false;
			}
		}
		// BUTTON 2/ VIEW 2
		if (*ffbOffset & 0x400)
		{
			if (button3pressed == false)
			{
				injector::WriteMemory<BYTE>((keyboardBuffer + DIK_B), 2, true);
				injector::WriteMemory<BYTE>((keyboardBuffer + DIK_F2), 2, true);
				button3pressed = true;
			}
		}
		else
		{
			if (button3pressed == true)
			{
				button3pressed = false;
			}
		}
		// BUTTON 3/ VIEW 3
		if (*ffbOffset & 0x800)
		{
			if (button4pressed == false)
			{
				injector::WriteMemory<BYTE>((keyboardBuffer + DIK_E), 2, true);
				injector::WriteMemory<BYTE>((keyboardBuffer + DIK_F3), 2, true);
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

DWORD WINAPI CreateWindowExART(DWORD dwExStyle, LPCSTR lpClassName, LPCSTR lpWindowName, DWORD dwStyle, int X, int Y, int nWidth, int nHeight, HWND hWndParent, HMENU hMenu, HINSTANCE hInstance, LPVOID lpParam)
{
	return original_CreateWindowExA(dwExStyle, lpClassName, "FNF Drift", 0x96C60000, X, Y, (nWidth + 16), (nHeight + 39), hWndParent, hMenu, hInstance, lpParam);
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

static DWORD genericRetZero()
{
	return 0;
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

		// FIX file write on D:
		injector::WriteMemoryRaw((0x1411F0 + BaseAddress), "\x2E\x5C\x65\x72\x72\x6F\x72\x6C\x6F\x67\x2E\x74\x78\x74\x00", 15, true);

		// TEST KEY FIX (uses BACKSPACE now)
		injector::MakeNOP((0x57B35 + BaseAddress), 14);
		injector::WriteMemory<BYTE>((0x57B44 + BaseAddress), DIK_BACK, true);

		// REMOVE ESC BOX
		injector::MakeNOP((0x455FC8), 5, true);

		CreateThread(NULL, 0, InputRT, NULL, 0, NULL);

		safeJMP(0x4c7c60, genericRetZero);

		if (ToBool(config["General"]["Windowed"]))
		{
			// CURSOR NOT HIDDEN
			injector::WriteMemory<BYTE>((0x63F6F + BaseAddress), 0x01, true);
			// SETWINDOWPOS DISABLE
			injector::WriteMemory<BYTE>((0xCB596 + BaseAddress), 0x74, true);

			MH_Initialize();
			MH_CreateHookApi(L"user32.dll", "CreateWindowExA", &CreateWindowExART, (void**)&original_CreateWindowExA);
			MH_CreateHookApi(L"user32.dll", "SetCursorPos", &SetCursorPosRT, NULL);
			MH_CreateHookApi(L"user32.dll", "SetCursor", &SetCursorRT, NULL);
			MH_CreateHookApi(L"user32.dll", "SetWindowPos", &SetWindowPosRT, (void**)&original_SetWindowPos);
			MH_EnableHook(MH_ALL_HOOKS);
		}

		// MACHINE ID setting
		if ((strcmp(config["Network"]["MachineID"].c_str(), "2") == 0))
		{
			injector::WriteMemory<DWORD>((0x137F88 + BaseAddress), 0x01, true);
		}
		else if ((strcmp(config["Network"]["MachineID"].c_str(), "3") == 0))
		{
			injector::WriteMemory<DWORD>((0x137F88 + BaseAddress), 0x02, true);
		}
		else if ((strcmp(config["Network"]["MachineID"].c_str(), "4") == 0))
		{
			injector::WriteMemory<DWORD>((0x137F88 + BaseAddress), 0x03, true);
		}
		else // MACHINE ID = 1
		{
			injector::WriteMemory<DWORD>((0x137F88 + BaseAddress), 0x00, true);
		}

	}, GameID::FNFDrift);
