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

#include "xinput.h"
#include "Utility/Helper.h"

#pragma comment(lib, "Ws2_32.lib")

static uintptr_t imageBase;
int horizontal6 = 0;
int vertical6 = 0;
HWND hWndRT6 = 0;

extern int* ffbOffset;
extern int* ffbOffset2;
extern int* ffbOffset3;

// hooks ori
BOOL(__stdcall *original_SetWindowPos6)(HWND hWnd, HWND hWndInsertAfter, int X, int Y, int cx, int cy, UINT uFlags);
BOOL(__stdcall *original_CreateWindowExA6)(DWORD dwExStyle, LPCSTR lpClassName, LPCSTR lpWindowName, DWORD dwStyle, int X, int Y, int nWidth, int nHeight, HWND hWndParent, HMENU hMenu, HINSTANCE hInstance, LPVOID lpParam);
BOOL(__stdcall *original_DefWindowProcA6)(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
BOOL(__stdcall *original_SetCursorPosRT6)(int X, int Y);
BOOL(__stdcall *original_SetWindowTextWRT6)(HWND hWnd, LPCWSTR lpString);

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

static bool init;
static bool LeftStart;
static bool LeftStrumUp;
static bool LeftStrumDown;
static bool LeftGreen;
static bool LeftRed;
static bool LeftYellow;
static bool LeftBlue;
static bool LeftOrange;
static bool RightStart;
static bool RightStrumUp;
static bool RightStrumDown;
static bool RightGreen;
static bool RightRed;
static bool RightYellow;
static bool RightBlue;
static bool RightOrange;

void GHAInputs(Helpers* helpers)
{
	DWORD Buttons = helpers->ReadInt32(0x746AB0, true);
	BYTE Active = *(BYTE*)(imageBase + 0x857AE0);

	if (!init)
	{
		BYTE Modify = *(BYTE*)(imageBase + 0x13B3DC4);

		if (Modify)
		{
			init = true;
			*(BYTE*)(imageBase + 0x857AE0) = 0x00;
		}
	}
	else
	{
		if (*ffbOffset & 0x01) // Left Start
		{
			if (!LeftStart)
			{
				LeftStart = true;

				if (!Active)
					*(BYTE*)(Buttons + 0x85) = 0x08;
			}
			else if (!Active)
				*(BYTE*)(Buttons + 0x85) = 0x00;
		}
		else
		{
			if (LeftStart)
			{
				LeftStart = false;

				if (!Active)
					*(BYTE*)(Buttons + 0x85) = 0x00;
			}
		}

		if (*ffbOffset & 0x02) // Left Strum Up
		{
			if (!LeftStrumUp)
			{
				LeftStrumUp = true;
				*(BYTE*)(Buttons + 0x75) += 0x10;

				if (Active)
					*(BYTE*)(Buttons + 0x84) = 0x01;
			}
			else if (Active)
				*(BYTE*)(Buttons + 0x84) = 0x00;
		}
		else
		{
			if (LeftStrumUp)
			{
				LeftStrumUp = false;
				*(BYTE*)(Buttons + 0x75) -= 0x10;

				if (Active)
					*(BYTE*)(Buttons + 0x84) = 0x00;
			}
		}

		if (*ffbOffset & 0x04) // Left Strum Down
		{
			if (!LeftStrumDown)
			{
				LeftStrumDown = true;
				*(BYTE*)(Buttons + 0x75) += 0x40;

				if (Active)
					*(BYTE*)(Buttons + 0x84) = 0x01;
			}
			else if (Active)
				*(BYTE*)(Buttons + 0x84) = 0x00;
		}
		else
		{
			if (LeftStrumDown)
			{
				LeftStrumDown = false;
				*(BYTE*)(Buttons + 0x75) -= 0x40;

				if (Active)
					*(BYTE*)(Buttons + 0x84) = 0x00;
			}
		}

		if (*ffbOffset & 0x08) // Left Green Fret
		{
			if (!LeftGreen)
			{
				LeftGreen = true;
				*(BYTE*)(Buttons + 0x52) += 0xFF;

				if (!Active)
					*(BYTE*)(Buttons + 0x84) = 0x40;
			}
			else if (!Active)
				*(BYTE*)(Buttons + 0x84) = 0x00;
		}
		else
		{
			if (LeftGreen)
			{
				LeftGreen = false;
				*(BYTE*)(Buttons + 0x52) -= 0xFF;

				if (!Active)
					*(BYTE*)(Buttons + 0x84) = 0x00;
			}
		}

		if (*ffbOffset & 0x10) // Left Red Fret
		{
			if (!LeftRed)
			{
				LeftRed = true;
				*(BYTE*)(Buttons + 0x50) += 0xFF;

				if (!Active)
					*(BYTE*)(Buttons + 0x85) = 0x01;
			}
			else if (!Active)
				*(BYTE*)(Buttons + 0x85) = 0x00;
		}
		else
		{
			if (LeftRed)
			{
				LeftRed = false;
				*(BYTE*)(Buttons + 0x50) -= 0xFF;

				if (!Active)
					*(BYTE*)(Buttons + 0x85) = 0x00;
			}
		}

		if (*ffbOffset & 0x20) // Left Yellow Fret
		{
			if (!LeftYellow)
			{
				LeftYellow = true;
				*(BYTE*)(Buttons + 0x51) += 0xFF;
			}
		}
		else
		{
			if (LeftYellow)
			{
				LeftYellow = false;
				*(BYTE*)(Buttons + 0x51) -= 0xFF;
			}
		}

		if (*ffbOffset & 0x40) // Left Blue Fret
		{
			if (!LeftBlue)
			{
				LeftBlue = true;
				*(BYTE*)(Buttons + 0x53) += 0xFF;
			}
		}
		else
		{
			if (LeftBlue)
			{
				LeftBlue = false;
				*(BYTE*)(Buttons + 0x53) -= 0xFF;
			}
		}

		if (*ffbOffset & 0x80) // Left Orange Fret
		{
			if (!LeftOrange)
			{
				LeftOrange = true;
				*(BYTE*)(Buttons + 0x4E) += 0xFF;
			}
		}
		else
		{
			if (LeftOrange)
			{
				LeftOrange = false;
				*(BYTE*)(Buttons + 0x4E) -= 0xFF;
			}
		}

		if (*ffbOffset & 0x100) // Right Start
		{
			if (!RightStart)
			{
				RightStart = true;

				if (!Active)
					*(BYTE*)(Buttons + 0x121) = 0x08;
			}
			else if (!Active)
				*(BYTE*)(Buttons + 0x121) = 0x00;
		}
		else
		{
			if (RightStart)
			{
				RightStart = false;

				if (!Active)
					*(BYTE*)(Buttons + 0x121) = 0x00;
			}
		}

		if (*ffbOffset & 0x200) // Right Strum Up
		{
			if (!RightStrumUp)
			{
				RightStrumUp = true;
				*(BYTE*)(Buttons + 0x111) += 0x10;

				if (Active)
					*(BYTE*)(Buttons + 0x120) = 0x01;
			}
			else if (Active)
				*(BYTE*)(Buttons + 0x120) = 0x00;
		}
		else
		{
			if (RightStrumUp)
			{
				RightStrumUp = false;
				*(BYTE*)(Buttons + 0x111) -= 0x10;

				if (Active)
					*(BYTE*)(Buttons + 0x120) = 0x00;
			}
		}

		if (*ffbOffset & 0x400) // Right Strum Down
		{
			if (!RightStrumDown)
			{
				RightStrumDown = true;
				*(BYTE*)(Buttons + 0x111) += 0x40;

				if (Active)
					*(BYTE*)(Buttons + 0x120) = 0x01;
			}
			else if (Active)
				*(BYTE*)(Buttons + 0x120) = 0x00;
		}
		else
		{
			if (RightStrumDown)
			{
				RightStrumDown = false;
				*(BYTE*)(Buttons + 0x111) -= 0x40;

				if (Active)
					*(BYTE*)(Buttons + 0x120) = 0x00;
			}
		}

		if (*ffbOffset & 0x800) // Right Green Fret
		{
			if (!RightGreen)
			{
				RightGreen = true;
				*(BYTE*)(Buttons + 0xEE) += 0xFF;

				if (!Active)
					*(BYTE*)(Buttons + 0x120) = 0x40;
			}
			else if (!Active)
				*(BYTE*)(Buttons + 0x120) = 0x00;
		}
		else
		{
			if (RightGreen)
			{
				RightGreen = false;
				*(BYTE*)(Buttons + 0xEE) -= 0xFF;

				if (!Active)
					*(BYTE*)(Buttons + 0x120) = 0x00;
			}
		}

		if (*ffbOffset & 0x1000) // Right Red Fret
		{
			if (!RightRed)
			{
				RightRed = true;
				*(BYTE*)(Buttons + 0xEC) += 0xFF;

				if (!Active)
					*(BYTE*)(Buttons + 0x121) = 0x01;
			}
			else if (!Active)
				*(BYTE*)(Buttons + 0x121) = 0x00;
		}
		else
		{
			if (RightRed)
			{
				RightRed = false;
				*(BYTE*)(Buttons + 0xEC) -= 0xFF;

				if (!Active)
					*(BYTE*)(Buttons + 0x121) = 0x00;
			}
		}

		if (*ffbOffset & 0x2000) // Right Yellow Fret
		{
			if (!RightYellow)
			{
				RightYellow = true;
				*(BYTE*)(Buttons + 0xED) += 0xFF;
			}
		}
		else
		{
			if (RightYellow)
			{
				RightYellow = false;
				*(BYTE*)(Buttons + 0xED) -= 0xFF;
			}
		}

		if (*ffbOffset & 0x4000) // Right Blue Fret
		{
			if (!RightBlue)
			{
				RightBlue = true;
				*(BYTE*)(Buttons + 0xEF) += 0xFF;
			}
		}
		else
		{
			if (RightBlue)
			{
				RightBlue = false;
				*(BYTE*)(Buttons + 0xEF) -= 0xFF;
			}
		}

		if (*ffbOffset & 0x8000) // Right Orange Fret
		{
			if (!RightOrange)
			{
				RightOrange = true;
				*(BYTE*)(Buttons + 0xEA) += 0xFF;
			}
		}
		else
		{
			if (RightOrange)
			{
				RightOrange = false;
				*(BYTE*)(Buttons + 0xEA) -= 0xFF;
			}
		}

		*(BYTE*)(Buttons + 0x66) = *ffbOffset2; // Left Guitar Tilt
		*(BYTE*)(Buttons + 0x102) = *ffbOffset3; // Right Guitar Tilt
	}
}

static DWORD WINAPI RunningLoop(LPVOID lpParam)
{
	while (true)
	{
		GHAInputs(0);
		Sleep(16);
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

	// Disable Xinput Inputs
	injector::MakeNOP(imageBase + 0x132168, 4);
	injector::MakeNOP(imageBase + 0x13215C, 4);
	injector::MakeNOP(imageBase + 0x1089F0, 3);
	injector::MakeNOP(imageBase + 0x108A01, 3);
	injector::MakeNOP(imageBase + 0x108A07, 3);
	injector::MakeNOP(imageBase + 0x108AEE, 3);

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

	CreateThread(NULL, 0, RunningLoop, NULL, 0, NULL);

}, GameID::GHA);
#endif