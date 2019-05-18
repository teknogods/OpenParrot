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
bool movable4 = false;
bool polling4 = false;

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
		injector::WriteMemory<BYTE>((0x576CF + BaseAddress4), 0x01, true);
		movable4 = true;
		break;

	case WM_LBUTTONUP:
		injector::WriteMemory<BYTE>((0x576CF + BaseAddress4), 0x00, true);
		movable4 = false;
		break;

	case WM_MOUSEMOVE:
	{
		if (movable4 == true)
		{
			injector::WriteMemory<BYTE>((0x576CF + BaseAddress4), 0x01, true);
			return 0;
		}
		else
		{
			if (polling4 == false)
			{
				return 0;
			}
			if (polling4 == true)
			{
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
				injector::WriteMemory<BYTE>((0x4C424 + BaseAddress4), 0xEB, true);
			}
			// NITRO ( = START too)
			if (*ffbOffset & 0x100)
			{
				injector::WriteMemory<BYTE>((keyboardBuffer + DIK_N), 2, true);
				injector::WriteMemory<BYTE>((keyboardBuffer + DIK_SPACE), 2, true);
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
			double fx = (float)((0 + wheel) * (65535.0f / horizontal4));
			double fy = (float)((0) *(65535.0f / vertical4));

			if (movable4 == false) // then poll ugly mouse input
			{
				polling4 = true;
				mouse_event(MOUSEEVENTF_MOVE, fx, 0, 0, 0);
				polling4 = false;
			}

			WORD vkey;
			// GAS
			vkey = 0x51; // Q key (qwerty A)
			if (*ffbOffset3 >= 5)
			{
				if (gaspressed == false)
				{
					keybd_event(vkey, MapVirtualKey(vkey, MAPVK_VK_TO_VSC), 0, 0);
					gaspressed = true;
				}
			}
			else
			{
				if (gaspressed == true)
				{
					keybd_event(vkey, MapVirtualKey(vkey, MAPVK_VK_TO_VSC), KEYEVENTF_KEYUP, 0);
					gaspressed = false;
				}
			}

			// BRAKE
			vkey = 0x57; // W key (qwerty Z)
			if (*ffbOffset4 >= 5)
			{
				if (brakepressed == false)
				{
					keybd_event(vkey, MapVirtualKey(vkey, MAPVK_VK_TO_VSC), 0, 0);
					brakepressed = true;
				}
			}
			else
			{
				if (brakepressed == true)
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
				injector::WriteMemory<BYTE>((0x576CF + BaseAddress4), 0x01, true);
				movable4 = true;
				original_SetWindowPos4(hWndRT4, HWND_BOTTOM, 0, 0, 0, 0, SWP_NOSIZE);
				ShowWindow(hWndRT4, SW_MINIMIZE);
			}
		}
		// LEFT-CLICK MOVES WINDOW FROM TOP-LEFT CORNER
		if (GetAsyncKeyState(VK_LBUTTON) & 0x8000)
		{
			HWND hWndTMP = GetForegroundWindow();
			if (hWndRT4 == 0)
			{
				hWndRT4 = FindWindowA(NULL, "Fast n Furious SuperBikes");
			}
			if (hWndTMP == hWndRT4)
			{
				injector::WriteMemory<BYTE>((0x576CF + BaseAddress4), 0x01, true);
				movable4 = true;
				POINT point;
				GetCursorPos(&point);
				RECT rect;
				GetWindowRect(hWndRT4, &rect);
				int width = rect.right - rect.left;
				int height = rect.bottom - rect.top;
				LPARAM blah = MAKELPARAM(point.x, point.y);
				int xClick = LOWORD(blah);
				int yClick = HIWORD(blah);
				original_SetWindowPos4(hWndRT4, HWND_TOP, xClick, yClick, width, height, SWP_NOSIZE);
				SetForegroundWindow(hWndRT4);
			}
		}
		else
		{
			injector::WriteMemory<BYTE>((0x576CF + BaseAddress4), 0x00, true);
			movable4 = false;
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

	MH_Initialize();
	MH_CreateHookApi(L"user32.dll", "SetCursorPos", &SetCursorPosRT4, (void**)&original_SetCursorPosRT4);
	MH_CreateHookApi(L"user32.dll", "DefWindowProcA", &DefWindowProcART4, (void**)&original_DefWindowProcA4);
	MH_EnableHook(MH_ALL_HOOKS);

	CreateThread(NULL, 0, InputRT4, NULL, 0, NULL);

	if (ToBool(config["General"]["Windowed"]))
	{
	CreateThread(NULL, 0, WindowRT4, NULL, 0, NULL);

	MH_Initialize();
	MH_CreateHookApi(L"user32.dll", "CreateWindowExA", &CreateWindowExART4, (void**)&original_CreateWindowExA4);
	MH_CreateHookApi(L"user32.dll", "SetWindowPos", &SetWindowPosRT4, (void**)&original_SetWindowPos4);
	MH_EnableHook(MH_ALL_HOOKS);
	}

}, GameID::FNFSB);
