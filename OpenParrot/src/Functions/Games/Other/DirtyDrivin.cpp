#include <StdInc.h> 
#include "Utility/InitFunction.h" 
#include "Functions/Global.h" 
#include "Utility\Hooking.Patterns.h" 
#include <Xinput.h> 
#include <math.h> 
#include <dinput.h> 
#include "Functions/GlobalRegHooks.h" 
#include "Utility\Hooking.Patterns.h" 
#include <atlstr.h> 
#include <windows.h> 
#include <string> 
#include <iostream> 
#include <shlobj.h> 
#include <fstream> 
#include "d3d9.h"

#pragma comment(lib, "Ws2_32.lib") 

typedef unsigned int U32;
typedef unsigned char U8;

DWORD BaseAddress9 = 0x00400000;
int horizontal9 = 0;
int vertical9 = 0;
HWND hWndRT9 = 0;

static bool button1pressed = false;
static bool button2pressed = false;
static bool button3pressed = false;
static bool button4pressed = false;
static bool STARTpressed = false;
static bool TESTpressed = false;

// controls 
extern int* ffbOffset;
extern int* ffbOffset2;
extern int* ffbOffset3;
extern int* ffbOffset4;

BOOL(__stdcall* original_CreateWindowExW9)(DWORD dwExStyle, LPCWSTR lpClassName, LPCWSTR lpWindowName, DWORD dwStyle, int X, int Y, int nWidth, int nHeight, HWND hWndParent, HMENU hMenu, HINSTANCE hInstance, LPVOID lpParam);
BOOL(__stdcall* original_CreateWindowExA9)(DWORD dwExStyle, LPCSTR lpClassName, LPCSTR lpWindowName, DWORD dwStyle, int X, int Y, int nWidth, int nHeight, HWND hWndParent, HMENU hMenu, HINSTANCE hInstance, LPVOID lpParam);

DWORD WINAPI InputRT9(LPVOID lpParam)
{
	int deltaTimer = 16;

	while (true)
	{
		if (ToBool(config["General"]["Windowed"]))
		{
			if (hWndRT9 == 0)
			{
				hWndRT9 = FindWindowW(NULL, TEXT("Dirty Drivin'"));
		//		hWndRT9 = FindWindowEx(NULL, NULL, L"Ghost", L"Dirty Drivin");
		//	}
		//	else if	(hWndRT9 != FindWindowEx(NULL, NULL, L"Ghost", L"Dirty Drivin"))
		//	{
		//		hWndRT9 = FindWindowEx(NULL, NULL, L"Direct3DWindowClass", L"Dirty Drivin");
			}
			else if ((GetWindowLongPtrA(hWndRT9, GWL_STYLE) != 0x14CA0000))
			{
				SetWindowLongPtr(hWndRT9, GWL_STYLE, 0x14CA0000);
				SetWindowLongPtr(hWndRT9, GWL_EXSTYLE, 0x00000000);
				SetWindowPos(hWndRT9, HWND_TOPMOST, 0, 0, 1366, 797, SWP_SHOWWINDOW | SWP_NOZORDER);
			}
		}

			// ESCAPE QUITS GAME  
		if (GetAsyncKeyState(VK_ESCAPE) & 0x8000)
		{
			exit(0);
		}

		// TEST MENU 
		if (GetAsyncKeyState(VK_BACK) & 0x8000)
		{
			if (TESTpressed == false)
			{
				TESTpressed = true;
			}
		}
		else
		{
			if (TESTpressed == true)
			{
				TESTpressed = false;
			}
		}

		// buttons see bitwise values in TPui//RawThrills.cs 
		// START	 
		if (*ffbOffset & 0x08)
		{
			if (STARTpressed == false)
			{
				keybd_event(VK_SPACE, 0, 0, 0);
				STARTpressed = true;
			}
		}
		else
		{
			if (STARTpressed == true)
			{
				keybd_event(VK_SPACE, 0, KEYEVENTF_KEYUP, 0);
				STARTpressed = false;
			}
		}

		// BUTTON 1/ CRANK UP 
		if (*ffbOffset & 0x100)
		{
			if (button1pressed == false)
			{
				keybd_event(VK_SPACE, 0, 0, 0);
				button1pressed = true;
			}
		}
		else
		{
			if (button1pressed == true)
			{
				keybd_event(VK_SPACE, 0, KEYEVENTF_KEYUP, 0);
				button1pressed = false;
			}
		}

		// BUTTON 3/ VIEW 
		if (*ffbOffset & 0x400)
		{
			if (button3pressed == false)
			{
				keybd_event(0x56, 0, 0, 0);
				button3pressed = true;
			}
		}
		else
		{
			if (button3pressed == true)
			{
				keybd_event(0x56, 0, KEYEVENTF_KEYUP, 0);
				button3pressed = false;
			}
		}

		// WHEEL 
		int iWheel = (((float)*ffbOffset2) - 128);
		float wheel = (iWheel * 0.0078125f);
		injector::WriteMemory<float>((0x4AD0FC + BaseAddress9), wheel, true);
		//// GAS 
		float gas = (float)*ffbOffset3 / 255.0f;
		injector::WriteMemory<float>((0x4AD0F8 + BaseAddress9), gas, true);

		//DEBUG////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////// 
		//	info(true, "test value %f  %f ", *ffbOffset2, x); 
		//DEBUG////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////// 

		Sleep(deltaTimer);
	}

	return 0;
}

D3DPRESENT_PARAMETERS* pPresentationParameters_RT9;
uintptr_t d3dcall;
void __stdcall D3D9CreateParamPatch() 
{
	#if _M_IX86
	__asm mov d3dcall , edx
	__asm mov edx, [ebp + 0x1c]
	__asm mov pPresentationParameters_RT9 , edx
	pPresentationParameters_RT9->Windowed = TRUE;
	pPresentationParameters_RT9->FullScreen_RefreshRateInHz = 0;
	pPresentationParameters_RT9->hDeviceWindow = NULL;
	injector::WriteMemoryRaw(0x63B332, "\xFF\xD2\x3D\x68\x08", 5, true);
	__asm mov edx , pPresentationParameters_RT9
	__asm mov [ebp + 0x1c], edx
	__asm mov edx , d3dcall
	#endif
	return;
}

static InitFunction DirtyDrivinFunc([]()
	{
		// PATCHING EXE AT RUNTIME (reboots, network, filepath, config, CRC... 
		injector::WriteMemoryRaw((0x335DD4 + BaseAddress9), "\x44\x69\x72\x74\x79\x20\x44\x72\x69\x76\x69\x6E\x27\x00", 14, true); // edit window caption text
		injector::WriteMemoryRaw((0x3B00 + BaseAddress9), "\xEB", 1, true);
		injector::WriteMemoryRaw((0x43B88 + BaseAddress9), "\xC6\x05\x6C\xFE\x96\x00\x01\x90\x90\xC6\x05\x58\x4A\x88\x00\x01\xB8\x58\x2A\x74\x00\xEB", 22, true);
		injector::WriteMemoryRaw((0x65447 + BaseAddress9), "\xEB\x0B\x90\x90", 4, true);
		injector::WriteMemoryRaw((0xBC9E8 + BaseAddress9), "\xEB", 1, true);
		injector::WriteMemoryRaw((0x1B5BF5 + BaseAddress9), "\x90\x90", 2, true);
		injector::WriteMemoryRaw((0x239B5C + BaseAddress9), "\xEB", 1, true);
		injector::WriteMemoryRaw((0x2F236E + BaseAddress9), "\x90\x90\x90", 3, true);
		injector::WriteMemoryRaw((0x2FB88F + BaseAddress9), "\x90\x90", 2, true);
		injector::WriteMemoryRaw((0x337A07 + BaseAddress9), "\x35", 1, true);

		//CONTROLS PATCH 
		injector::MakeNOP((0x5159A + BaseAddress9), 6, true);
		injector::MakeNOP((0x515AC + BaseAddress9), 6, true);
		injector::MakeNOP((0x514BB + BaseAddress9), 4, true);
		injector::MakeNOP((0x514C0 + BaseAddress9), 3, true);

		CreateThread(NULL, 0, InputRT9, NULL, 0, NULL);

		// auto accl off 
		if (ToBool(config["General"]["AutoAcclOff"]))
		{
			injector::MakeNOP((0xAA6E6 + BaseAddress9), 6, true);
		}

		if (ToBool(config["General"]["Windowed"]))
		{
			injector::MakeJMP(0x63B332, 0x729EF0);
			injector::MakeCALL(0x729EF0, D3D9CreateParamPatch);
			injector::MakeJMP(0x729EF5, 0x63B332);
		}

	}, GameID::DirtyDrivin);
