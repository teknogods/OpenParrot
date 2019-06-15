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

DWORD BaseAddress6 = 0x00400000;
int horizontal6 = 0;
int vertical6 = 0;
HWND hWndRT6 = 0;

extern int* ffbOffset;
extern int* ffbOffset3;
extern int* ffbOffset4;

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
