#if __has_include(<atlstr.h>)
#include <StdInc.h>
#include "Utility/InitFunction.h"
#include "Functions/Global.h"
#include "Functions/GlobalRegHooks.h"
#include "Utility\Hooking.Patterns.h"
#include <atlstr.h>
#include <windows.h>
#include <string>
#include <iostream>
#include <shlobj.h>
#include <fstream>
using namespace std;
#include "Functions/XInputEmu.h"

#pragma comment(lib, "Ws2_32.lib")

static uintptr_t imageBase;
int horizontal6 = 0;
int vertical6 = 0;
HWND hWndRT6 = 0;

extern int* ffbOffset;
extern int* ffbOffset3;
extern int* ffbOffset4;
extern XINPUT_GAMEPAD gamepadState;

static bool GHAStart;
static bool GHAStart2;
static bool GHAUP;
static bool GHADOWN;
static bool GHALEFT;
static bool GHARIGHT;
static bool GHAUP2;
static bool GHADOWN2;
static bool GHALEFT2;
static bool GHARIGHT2;
static bool GHAA;
static bool GHAB;
static bool GHAX;
static bool GHAY;
static bool GHALB;
static bool GHARB;

static bool init;

// hooks ori
BOOL(__stdcall *original_SetWindowPos6)(HWND hWnd, HWND hWndInsertAfter, int X, int Y, int cx, int cy, UINT uFlags);
BOOL(__stdcall *original_CreateWindowExA6)(DWORD dwExStyle, LPCSTR lpClassName, LPCSTR lpWindowName, DWORD dwStyle, int X, int Y, int nWidth, int nHeight, HWND hWndParent, HMENU hMenu, HINSTANCE hInstance, LPVOID lpParam);
BOOL(__stdcall *original_DefWindowProcA6)(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
BOOL(__stdcall *original_SetCursorPosRT6)(int X, int Y);
BOOL(__stdcall *original_SetWindowTextWRT6)(HWND hWnd, LPCWSTR lpString);
BOOL(__stdcall *original_XInputGetStateGHA)(DWORD dwUserIndex, XINPUT_STATE* pState);

DWORD WINAPI InputRT6(LPVOID lpParam)
{
	while (true)
	{
		// ESCAPE QUITS GAME 
		if (GetAsyncKeyState(VK_ESCAPE) & 0x8000)
		{
			WinExec("taskkill /f /im GHA.exe", SW_HIDE);
		}
		//DEBUG//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		//info(true, "test values *ffbOffset3=0x%02X / *ffbOffset4=0x%02X ", *ffbOffset3, *ffbOffset4);
		//DEBUG//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	}
	return 0;
}

DWORD WINAPI WindowRT6(LPVOID lpParam)
{
	while (true)
	{
		// RIGHT-CLICK MINIMIZES WINDOW
		if (GetAsyncKeyState(VK_RBUTTON) & 0x8000)
		{
			HWND hWndTMP = GetForegroundWindow();
			if (hWndRT6 == 0)
			{
				hWndRT6 = FindWindowA(NULL, "Guitar Hero Arcade");
			}
			if (hWndTMP == hWndRT6)
			{
				RECT rect;
				GetWindowRect(hWndRT6, &rect);
				int currentwidth = rect.right - rect.left;
				int currentheight = rect.bottom - rect.top;
				original_SetWindowPos6(hWndRT6, HWND_BOTTOM, 0, 0, 1360, 768, SWP_NOSIZE);
				ShowWindow(hWndRT6, SW_MINIMIZE);
			}
		}
	}
}

void GHAInputs()
{
	if (!init)
	{
		BYTE Modify = *(BYTE*)(imageBase + 0x13B3DC4);

		if (Modify)
		{
			init = true;
			*(BYTE*)(imageBase + 0x857AE0) = 0x00;
		}	
	}
	
	BYTE Active = *(BYTE*)(imageBase + 0x857AE0);

	if (*ffbOffset & XINPUT_GAMEPAD_START) // START KEY MACRO (When not playing song)
	{
		if (!GHAStart)
		{
			GHAStart = true;
			GHAStart2 = true;
			if (!Active)
			{
				gamepadState.wButtons += 0xF000;
				gamepadState.bLeftTrigger += 0xFF;
				gamepadState.bRightTrigger += 0xFF;
			}
		}
		else
		{
			if (GHAStart2)
			{
				GHAStart2 = false;
				if (!Active)
				{
					gamepadState.wButtons -= 0xF000;
					gamepadState.bLeftTrigger -= 0xFF;
					gamepadState.bRightTrigger -= 0xFF;

				}
			}
		}
	}
	else
	{
		if (GHAStart)
		{
			GHAStart = false;

			if (Active)
			{
				if (gamepadState.wButtons == 0xF000 && gamepadState.bLeftTrigger == 0xFF && gamepadState.bRightTrigger == 0xFF)
				{
					gamepadState.wButtons -= 0xF000;
					gamepadState.bLeftTrigger -= 0xFF;
					gamepadState.bRightTrigger -= 0xFF;
				}
			}
		}
	}

	if (*ffbOffset & XINPUT_GAMEPAD_X) // GREEN KEY MACRO
	{
		if (!GHAX)
		{
			GHAX = true;
			gamepadState.bLeftTrigger += 0xFF;
		}
	}
	else
	{
		if (GHAX)
		{
			GHAX = false;
			gamepadState.bLeftTrigger -= 0xFF;
		}
	}

	if (*ffbOffset & XINPUT_GAMEPAD_Y) // BLUE KEY MACRO
	{
		if (!GHAY)
		{
			GHAY = true;
			gamepadState.bRightTrigger += 0xFF;
		}
	}
	else
	{
		if (GHAY)
		{
			GHAY = false;
			gamepadState.bRightTrigger -= 0xFF;
		}
	}

	if (*ffbOffset & XINPUT_GAMEPAD_A)
	{
		if (!GHAA)
		{
			GHAA = true;
			gamepadState.wButtons += XINPUT_GAMEPAD_A;
		}
	}
	else
	{
		if (GHAA)
		{
			GHAA = false;
			gamepadState.wButtons -= XINPUT_GAMEPAD_A;
		}
	}

	if (*ffbOffset & XINPUT_GAMEPAD_B)
	{
		if (!GHAB)
		{
			GHAB = true;
			gamepadState.wButtons += XINPUT_GAMEPAD_B;
		}
	}
	else
	{
		if (GHAB)
		{
			GHAB = false;
			gamepadState.wButtons -= XINPUT_GAMEPAD_B;
		}
	}

	if (*ffbOffset & XINPUT_GAMEPAD_DPAD_UP)
	{
		if (!GHAUP)
		{
			GHAUP = true;
			GHAUP2 = true;
			gamepadState.wButtons += XINPUT_GAMEPAD_DPAD_UP;
		}
		else
		{
			if (GHAUP2)
			{
				GHAUP2 = false;
				gamepadState.wButtons -= XINPUT_GAMEPAD_DPAD_UP;
			}
		}
	}
	else
	{
		if (GHAUP)
			GHAUP = false;
	}

	if (*ffbOffset & XINPUT_GAMEPAD_DPAD_DOWN)
	{
		if (!GHADOWN)
		{
			GHADOWN = true;
			GHADOWN2 = true;
			gamepadState.wButtons += XINPUT_GAMEPAD_DPAD_DOWN;
		}
		else
		{
			if (GHADOWN2)
			{
				GHADOWN2 = false;
				gamepadState.wButtons -= XINPUT_GAMEPAD_DPAD_DOWN;
			}
		}
	}
	else
	{
		if (GHADOWN)
			GHADOWN = false;
	}

	if (*ffbOffset & XINPUT_GAMEPAD_DPAD_LEFT)
	{
		if (!GHALEFT)
		{
			GHALEFT = true;
			GHALEFT2 = true;
			gamepadState.wButtons += XINPUT_GAMEPAD_DPAD_LEFT;
		}
		else
		{
			if (GHALEFT2)
			{
				GHALEFT2 = false;
				gamepadState.wButtons -= XINPUT_GAMEPAD_DPAD_LEFT;
			}
		}
	}
	else
	{
		if (GHALEFT)
			GHALEFT = false;
	}

	if (*ffbOffset & XINPUT_GAMEPAD_DPAD_RIGHT)
	{
		if (!GHARIGHT)
		{
			GHARIGHT = true;
			GHARIGHT2 = true;
			gamepadState.wButtons += XINPUT_GAMEPAD_DPAD_RIGHT;
		}
		else
		{
			if (GHARIGHT2)
			{
				GHARIGHT2 = false;
				gamepadState.wButtons -= XINPUT_GAMEPAD_DPAD_RIGHT;
			}
		}
	}
	else
	{
		if (GHARIGHT)
			GHARIGHT = false;
	}

	if (*ffbOffset & XINPUT_GAMEPAD_LEFT_SHOULDER)
	{
		if (!GHALB)
		{
			GHALB = true;
			gamepadState.wButtons += XINPUT_GAMEPAD_LEFT_SHOULDER;
		}
	}
	else
	{
		if (GHALB)
		{
			GHALB = false;
			gamepadState.wButtons -= XINPUT_GAMEPAD_LEFT_SHOULDER;
		}
	}

	if (*ffbOffset & XINPUT_GAMEPAD_RIGHT_SHOULDER)
	{
		if (!GHARB)
		{
			GHARB = true;
			gamepadState.wButtons += XINPUT_GAMEPAD_RIGHT_SHOULDER;
		}
	}
	else
	{
		if (GHARB)
		{
			GHARB = false;
			gamepadState.wButtons -= XINPUT_GAMEPAD_RIGHT_SHOULDER;
		}
	}
}

DWORD WINAPI DefWindowProcART6(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
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
			if (xWindow >= (horizontal6 - 100))
				xWindow = 0;
			if (yWindow >= (vertical6 - 100))
				yWindow = 0;
			original_SetWindowPos6(hWnd, NULL, xWindow, yWindow, 1360, 768, SWP_NOSIZE | SWP_NOZORDER);
		}
		break;
	}

	}
	return original_DefWindowProcA6(hWnd, message, wParam, lParam);
}

DWORD WINAPI CreateWindowExART6(DWORD dwExStyle, LPCSTR lpClassName, LPCSTR lpWindowName, DWORD dwStyle, int X, int Y, int nWidth, int nHeight, HWND hWndParent, HMENU hMenu, HINSTANCE hInstance, LPVOID lpParam)
{
	return original_CreateWindowExA6(dwExStyle, lpClassName, "Guitar Hero Arcade", 0x94000000, X, Y, 1360, 768, hWndParent, hMenu, hInstance, lpParam);
}

DWORD WINAPI SetCursorPosRT6(int X, int Y)
{
	return 1;
}

DWORD WINAPI SetWindowPosRT6(HWND hWnd, HWND hWndInsertAfter, int X, int Y, int cx, int cy, UINT uFlags)
{
	return 1;
}

DWORD WINAPI SetWindowTextWRT6(HWND hWnd, LPCWSTR lpString)
{
	return original_SetWindowTextWRT6(hWnd, CStringW("Guitar Hero Arcade"));
}

static InitFunction GHAFunc([]()
{
	imageBase = (uintptr_t)GetModuleHandleA(0);

	init_GlobalRegHooks();
	GetDesktopResolution(horizontal6, vertical6);

	// CONFIG FILE HANDLING
	char working_directory[MAX_PATH + 1];
	GetCurrentDirectoryA(sizeof(working_directory), working_directory);
	std::string s7 = working_directory;
	std::string s8 = s7 + "\\AspyrConfig.xml";

	ofstream file;
	file.open(s8.c_str());

	string strXML = "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n<r>\n	<s id=\"Video.Width\">";

	std::string h = std::to_string(horizontal6);
	std::string v = std::to_string(vertical6);
	
	if (ToBool(config["General"]["Windowed"]))
	{
	strXML += "1360";
	}
	else if (ToBool(config["General"]["HDPatch"]))
	{
		strXML += h;
	}
	else 
	{
		strXML += "1360";
	}

	strXML += "</s>\n	<s id=\"Video.Height\">";

	if (ToBool(config["General"]["Windowed"]))
	{
		strXML += "768";
	}
	else if (ToBool(config["General"]["HDPatch"]))
	{
		strXML += v;
	}
	else 
	{
		strXML += "768";
	}
	
	strXML += "</s> \n	<s id=\"Options.GraphicsQuality\">0</s>\n	<s id=\"Options.Crowd\">1</s>\n	<s id=\"Options.Physics\">1</s>\n	<s id=\"Options.Flares\">1</s>\n	<s id=\"Options.FrontRowCamera\">0</s>\n	<s id=\"6f1d2b61d5a011cfbfc7444553540000\">328 221 340 343 267 264 999 219 235 331 304 999 310</s>\n</r>\n";
	file << strXML;
	file.close();

	char user[MAX_PATH];
	SHGetFolderPathA(NULL, CSIDL_LOCAL_APPDATA, NULL, 0, user);
	std::string s11 = user;
	std::string s12 = s11 + "\\Aspyr\\Guitar Hero III\\AspyrConfig.xml";
	SetFileAttributesA(s12.c_str(), FILE_ATTRIBUTE_NORMAL);
	Sleep(100);
	CopyFileA(s8.c_str(), s12.c_str(), FALSE);
	Sleep(100);
	SetFileAttributesA(s12.c_str(), FILE_ATTRIBUTE_READONLY);
	Sleep(100);

	// UNLOCK MAIN MENU
	std::string s21 = s7 + "\\DATA\\PAK\\qb.pab.xen";
	std::string s22 = s7 + "\\DATA\\PAK\\qb.pak.xen";
	std::string s23 = s7 + "\\DATA\\PAK\\qb.pab.ATTRACTv4.xen";
	std::string s24 = s7 + "\\DATA\\PAK\\qb.pak.ATTRACTv4.xen";
	std::string s25 = s7 + "\\DATA\\PAK\\qb.pab.MENUv4.xen";
	std::string s26 = s7 + "\\DATA\\PAK\\qb.pak.MENUv4.xen";

	if (ToBool(config["General"]["UnlockMainMenu"]))
	{
		CopyFileA(s25.c_str(), s21.c_str(), FALSE);
		CopyFileA(s26.c_str(), s22.c_str(), FALSE);
		Sleep(100);
	}
	else
	{
		CopyFileA(s23.c_str(), s21.c_str(), FALSE);
		CopyFileA(s24.c_str(), s22.c_str(), FALSE);
		Sleep(100);
	}

	CreateThread(NULL, 0, InputRT6, NULL, 0, NULL);

	MH_Initialize();
	MH_CreateHookApi(L"user32.dll", "SetWindowPos", &SetWindowPosRT6, (void**)&original_SetWindowPos6);
	MH_CreateHookApi(L"user32.dll", "SetWindowTextW", &SetWindowTextWRT6, (void**)&original_SetWindowTextWRT6);
	MH_EnableHook(MH_ALL_HOOKS);

	if (ToBool(config["General"]["Windowed"]))
	{
		CreateThread(NULL, 0, WindowRT6, NULL, 0, NULL);

		MH_Initialize();
		MH_CreateHookApi(L"user32.dll", "CreateWindowExA", &CreateWindowExART6, (void**)&original_CreateWindowExA6);
		MH_CreateHookApi(L"user32.dll", "DefWindowProcA", &DefWindowProcART6, (void**)&original_DefWindowProcA6);
		MH_CreateHookApi(L"user32.dll", "SetCursorPos", &SetCursorPosRT6, NULL);
		MH_EnableHook(MH_ALL_HOOKS);
	}

}, GameID::GHA);
#endif