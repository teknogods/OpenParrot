#if __has_include(<atlstr.h>)
#include <StdInc.h>
#include "Utility/InitFunction.h"
#include "Functions/Global.h"
#include "Utility\Hooking.Patterns.h"
#include <Xinput.h>
#include <winbase.h>
#include <math.h>
#include <string>
#include <atlstr.h>
#include <dinput.h>

#pragma comment(lib, "Ws2_32.lib")
#if _M_IX86
#define clamp( x, xmin, xmax ) min( xmax, max( x, xmin ) )
typedef unsigned int U32;
typedef unsigned char U8;

static DWORD BaseAddress8 = 0x00400000;
static int horizontal8 = 0;
static int vertical8 = 0;
static HWND hWndRT8 = 0;

static bool previousLeft = false;
static bool previousRight = false;
static bool previousUp = false;
static bool previousDown = false;

static bool TESTpressed = false;
static bool COIN1pressed = false;
static bool COIN2pressed = false;

// controls
extern int* ffbOffset;
extern int* ffbOffset2;
extern int* ffbOffset3;
extern int* ffbOffset4;
// hooks ori
BOOL(__stdcall* original_SetWindowPos8)(HWND hWnd, HWND hWndInsertAfter, int X, int Y, int cx, int cy, UINT uFlags);
BOOL(__stdcall* original_CreateWindowExA8)(DWORD dwExStyle, LPCSTR lpClassName, LPCSTR lpWindowName, DWORD dwStyle, int X, int Y, int nWidth, int nHeight, HWND hWndParent, HMENU hMenu, HINSTANCE hInstance, LPVOID lpParam);
BOOL(__stdcall* original_GetPrivateProfileStringA8)(LPCSTR lpAppName, LPCSTR lpKeyName, LPCSTR lpDefault,LPSTR lpReturnedString,DWORD nSize, LPCSTR lpFileName);


DWORD WINAPI InputRT8(LPVOID lpParam)
{
	int deltaTimer = 16;

	while (true)
	{
		// regular buttons are emulated by XINPUTEMU
		// TEST
		if (*ffbOffset & 0x01)
		{
			if (TESTpressed == false)
			{
				keybd_event(0x4F, MapVirtualKey(0x4F, MAPVK_VK_TO_VSC), 0, 0);
				TESTpressed = true;
			}
		}
		else
		{
			if (TESTpressed == true)
			{
				keybd_event(0x4F, MapVirtualKey(0x4F, MAPVK_VK_TO_VSC), KEYEVENTF_KEYUP, 0);
				TESTpressed = false;
			}
		}
		Sleep(deltaTimer);
	}

	return 0;
}

DWORD WINAPI WindowRT8(LPVOID lpParam)
{
	while (true)
	{
		// LEFT-CLICK MOVES WINDOW FROM TOP-LEFT CORNER
		if (GetAsyncKeyState(VK_LBUTTON) & 0x8000)
		{
			HWND hWndTMP = GetForegroundWindow();
			if (hWndRT8 == 0)
			{
				hWndRT8 = FindWindowA(NULL, "Justice League");
			}
			if (hWndTMP == hWndRT8)
			{
				POINT point;
				GetCursorPos(&point);
				RECT rect;
				GetWindowRect(hWndRT8, &rect);
				int width = rect.right - rect.left;
				int height = rect.bottom - rect.top;
				LPARAM blah = MAKELPARAM(point.x, point.y);
				int xClick = LOWORD(blah);
				int yClick = HIWORD(blah);
				if ((xClick + (width/2)) > horizontal8)
				{
					xClick = (horizontal8 - width);
				}
				if ((yClick + (height/2)) > vertical8)
				{
					yClick = (vertical8 - height);
				}
				original_SetWindowPos8(hWndRT8, HWND_TOP, xClick, yClick, 1360, 768, SWP_NOSIZE);
				SetForegroundWindow(hWndRT8);

			}
		}
		// RIGHT-CLICK MINIMIZES WINDOW
		if (GetAsyncKeyState(VK_RBUTTON) & 0x8000)
		{
			HWND hWndTMP = GetForegroundWindow();
			if (hWndRT8 == 0)
			{
				hWndRT8 = FindWindowA(NULL, "Justice League");
			}
			if (hWndTMP == hWndRT8)
			{
				RECT rect;
				GetWindowRect(hWndRT8, &rect);
				int currentwidth = rect.right - rect.left;
				int currentheight = rect.bottom - rect.top;
				original_SetWindowPos8(hWndRT8, HWND_BOTTOM, 0, 0, 1360, 768, SWP_NOSIZE);
				ShowWindow(hWndRT8, SW_MINIMIZE);
			}
			else ShowWindow(hWndRT8, SW_SHOWDEFAULT);
		}
	}
}

DWORD WINAPI GetPrivateProfileStringART8(LPCSTR lpAppName, LPCSTR lpKeyName, LPCSTR lpDefault, LPSTR lpReturnedString, DWORD nSize, LPCSTR lpFileName)
{
	char buffer[256];
	LPCSTR fs = "true";
	LPCSTR resX = itoa(horizontal8, buffer, 10);
	LPCSTR resY = itoa(vertical8, buffer, 10);
	if (ToBool(config["General"]["Windowed"]))
	{
		fs = "false";
		resX = "1360";
		resY = "768";
	}
	
	if (_stricmp(lpKeyName, "Fullscreen") == 0)
	{	
		WritePrivateProfileStringA(lpAppName, lpKeyName, fs, lpFileName);
	}
	if (_stricmp(lpKeyName, "ScreenResolutionX") == 0)
	{
		WritePrivateProfileStringA(lpAppName, lpKeyName, resX, lpFileName);
	}
	if (_stricmp(lpKeyName, "ScreenResolutionY") == 0)
	{
		WritePrivateProfileStringA(lpAppName, lpKeyName, resY, lpFileName);
	}
	if (_stricmp(lpKeyName, "DisplayWidth") == 0)
	{
		WritePrivateProfileStringA(lpAppName, lpKeyName, resX, lpFileName);
	}
	if (_stricmp(lpKeyName, "DisplayHeight") == 0)
	{
		WritePrivateProfileStringA(lpAppName, lpKeyName, resY, lpFileName);
	}

	return original_GetPrivateProfileStringA8(lpAppName, lpKeyName, lpDefault, lpReturnedString, nSize, lpFileName);
}

DWORD WINAPI CreateWindowExART8(DWORD dwExStyle, LPCSTR lpClassName, LPCSTR lpWindowName, DWORD dwStyle, int X, int Y, int nWidth, int nHeight, HWND hWndParent, HMENU hMenu, HINSTANCE hInstance, LPVOID lpParam)
{
	return original_CreateWindowExA8(dwExStyle, lpClassName, "Justice League", 0x96000000, 0, 0, nWidth, nHeight, hWndParent, hMenu, hInstance, lpParam);
}

DWORD WINAPI SetWindowPosRT8(HWND hWnd, HWND hWndInsertAfter, int X, int Y, int cx, int cy, UINT uFlags)
{
	return original_SetWindowPos8(hWnd, hWndInsertAfter, X, Y, cx, cy, uFlags);
}

static InitFunction JLeagueFunc([]()
{
	GetDesktopResolution(horizontal8, vertical8);

	CreateThread(NULL, 0, InputRT8, NULL, 0, NULL);

	MH_Initialize();
	MH_CreateHookApi(L"kernel32.dll", "GetPrivateProfileStringA", &GetPrivateProfileStringART8, (void**)& original_GetPrivateProfileStringA8);
	MH_CreateHookApi(L"user32.dll", "CreateWindowExA", &CreateWindowExART8, (void**)& original_CreateWindowExA8);
	MH_CreateHookApi(L"user32.dll", "SetWindowPos", &SetWindowPosRT8, (void**)& original_SetWindowPos8);
	MH_EnableHook(MH_ALL_HOOKS);

	if (ToBool(config["General"]["Windowed"]))
	{
		CreateThread(NULL, 0, WindowRT8, NULL, 0, NULL);
	}

}, GameID::JLeague);
#endif
#endif