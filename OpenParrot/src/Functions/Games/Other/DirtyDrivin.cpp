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

#if _M_IX86
#pragma optimize("", off)
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
static bool SERVICEpressed = false;
static bool previousVolMin = false;
static bool previousVolMax = false;
static bool MenuHack = false;
static bool RiptideHack = false;
static bool MenuHackStopWriting = false;

// controls 
extern int* ffbOffset;
extern int* ffbOffset2;
extern int* ffbOffset3;
extern int* ffbOffset4;

BOOL(__stdcall* original_CreateWindowExW9)(DWORD dwExStyle, LPCWSTR lpClassName, LPCWSTR lpWindowName, DWORD dwStyle, int X, int Y, int nWidth, int nHeight, HWND hWndParent, HMENU hMenu, HINSTANCE hInstance, LPVOID lpParam);
BOOL(__stdcall* original_CreateWindowExA9)(DWORD dwExStyle, LPCSTR lpClassName, LPCSTR lpWindowName, DWORD dwStyle, int X, int Y, int nWidth, int nHeight, HWND hWndParent, HMENU hMenu, HINSTANCE hInstance, LPVOID lpParam);

void __stdcall ServiceControlsPatch()
{
	// TEST 
	if ((GetAsyncKeyState(VK_INSERT) & 0x8000) || (*ffbOffset & 0x400))
	{
		if (TESTpressed == false)
		{
			injector::WriteMemory<BYTE>((0x98C380), 0x01, true);
			TESTpressed = true;
		}
	}
	else
	{
		if (TESTpressed == true)
		{
			injector::WriteMemory<BYTE>((0x98C380), 0x00, true);
			TESTpressed = false;
		}
	}
	// SERVICE
	if ((GetAsyncKeyState(VK_DELETE) & 0x8000) || (*ffbOffset & 0x800))
	{
		if (SERVICEpressed == false)
		{
			injector::WriteMemory<BYTE>((0x98C384), 0x01, true);
			SERVICEpressed = true;
		}
	}
	else
	{
		if (SERVICEpressed == true)
		{
			injector::WriteMemory<BYTE>((0x98C384), 0x00, true);
			SERVICEpressed = false;
		}
	}
	// VOL+
	if (*ffbOffset & 0x1000)
	{
		if (previousVolMax == false)
		{
			injector::WriteMemory<BYTE>((0x98C388), 0x01, true);
			previousVolMax = true;
		}
	}
	else
	{
		if (previousVolMax == true)
		{
			injector::WriteMemory<BYTE>((0x98C388), 0x00, true);
			previousVolMax = false;
		}
	}
	// VOL-
	if (*ffbOffset & 0x2000)
	{
		if (previousVolMin == false)
		{
			injector::WriteMemory<BYTE>((0x98C38C), 0x01, true);
			previousVolMin = true;
		}
	}
	else
	{
		if (previousVolMin == true)
		{
			injector::WriteMemory<BYTE>((0x98C38C), 0x00, true);
			previousVolMin = false;
		}
	}

	return;
}

DWORD WINAPI InputRT9(LPVOID lpParam)
{
	int deltaTimer = 16;

	while (true)
	{
		BYTE GameState = *(BYTE*)(0x570190 + BaseAddress9);
		BYTE Chosen = *(BYTE*)(0x5705E8 + BaseAddress9);

		if (GameState == 0x05)
		{
			if (!MenuHack)
			{
				MenuHack = true;
				injector::MakeNOP((0x78A27 + BaseAddress9), 6, true);
			}
		}
		else if (GameState == 0x06 || GameState == 0x08 || GameState == 0x12)
		{
			if (MenuHack)
			{
				MenuHack = false;
				MenuHackStopWriting = false;

				injector::WriteMemory((0x78A27 + BaseAddress9), 0x03448689, true);
				injector::WriteMemory((0x78A2B + BaseAddress9), 0x8E890000, true);
			}
		}

		if (MenuHack)
		{
			if (Chosen == 0x01)
			{
				MenuHackStopWriting = true;
			}

			if (!MenuHackStopWriting)
			{
				if (*ffbOffset2 >= 0xEE)
				{
					*(BYTE*)(0x570234 + BaseAddress9) = 0x0A;
				}
				else if (*ffbOffset2 >= 0xDD)
				{
					*(BYTE*)(0x570234 + BaseAddress9) = 0x0C;
				}
				else if (*ffbOffset2 >= 0xCC)
				{
					*(BYTE*)(0x570234 + BaseAddress9) = 0x08;
				}
				else if (*ffbOffset2 >= 0xBB)
				{
					*(BYTE*)(0x570234 + BaseAddress9) = 0x0D;
				}
				else if (*ffbOffset2 >= 0xAA)
				{
					*(BYTE*)(0x570234 + BaseAddress9) = 0x0E;
				}
				else if (*ffbOffset2 >= 0x99)
				{
					*(BYTE*)(0x570234 + BaseAddress9) = 0x09;
				}
				else if (*ffbOffset2 >= 0x88)
				{
					*(BYTE*)(0x570234 + BaseAddress9) = 0x0B;
				}
				else if (*ffbOffset2 >= 0x77)
				{
					*(BYTE*)(0x570234 + BaseAddress9) = 0x02;
				}
				else if (*ffbOffset2 >= 0x66)
				{
					*(BYTE*)(0x570234 + BaseAddress9) = 0x00;
				}
				else if (*ffbOffset2 >= 0x55)
				{
					*(BYTE*)(0x570234 + BaseAddress9) = 0x04;
				}
				else if (*ffbOffset2 >= 0x44)
				{
					*(BYTE*)(0x570234 + BaseAddress9) = 0x06;
				}
				else if (*ffbOffset2 >= 0x33)
				{
					*(BYTE*)(0x570234 + BaseAddress9) = 0x05;
				}
				else if (*ffbOffset2 >= 0x22)
				{
					*(BYTE*)(0x570234 + BaseAddress9) = 0x03;
				}
				else if (*ffbOffset2 >= 0x11)
				{
					*(BYTE*)(0x570234 + BaseAddress9) = 0x01;
				}
				else if (*ffbOffset2 >= 0x00)
				{
					*(BYTE*)(0x570234 + BaseAddress9) = 0x10;
				}
			}
		}

		if (ToBool(config["General"]["Windowed"]))
		{
			if (hWndRT9 == 0)
			{
				hWndRT9 = FindWindowW(NULL, TEXT("Dirty Drivin'"));
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
			HWND hWndTMP = GetForegroundWindow();
			if (hWndRT9 == 0)
			{
				hWndRT9 = FindWindowW(NULL, TEXT("Dirty Drivin'"));
			}
			if (hWndTMP == hWndRT9)
			{
				exit(0);
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

		// BUTTON 1/ CRANK BACKWARD 
		// !!! NOTE: CRANK FORWARD ONLY USED FOR GAS WHEN CONVERTING FROM H2Overdrive CABINET !!! //
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

		// BUTTON 2/ VIEW 
		if (*ffbOffset & 0x200)
		{
			if (button2pressed == false)
			{
				keybd_event(0x56, 0, 0, 0);
				button2pressed = true;
			}
		}
		else
		{
			if (button2pressed == true)
			{
				keybd_event(0x56, 0, KEYEVENTF_KEYUP, 0);
				button2pressed = false;
			}
		}

		// WHEEL 
		if ((GameState == 0x06) && (*ffbOffset2 > 0x60 && *ffbOffset2 < 0x70))
		{
			if (!RiptideHack)
			{
				RiptideHack = true;
				*(BYTE*)(0x44BAD0 + BaseAddress9) = 0x08;
				injector::WriteMemory<float>((0x4AD0FC + BaseAddress9), -1.0f, true);
			}	
		}
		else
		{
			if (RiptideHack)
			{
				RiptideHack = false;
				*(BYTE*)(0x44BAD0 + BaseAddress9) = 0x0B;
			}
			
			int iWheel = (((float)*ffbOffset2) - 128);
			float wheel = (iWheel * 0.0078125f);
			injector::WriteMemory<float>((0x4AD0FC + BaseAddress9), wheel, true);
		}
		//// GAS 
		float gas = (float)*ffbOffset3 / 255.0f;
		float brake = (float)*ffbOffset4 / 255.0f;
	//	injector::WriteMemory<float>((0x4AD0F8 + BaseAddress9), gas, true);
		// BRAKE BUTTON HACK = if brake pressed gas is reduced
		injector::WriteMemory<float>((0x4AD0F8 + BaseAddress9), gas - brake, true);
		
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

		//TVVS CONTROLS PATCH
		injector::MakeNOP((0x506C57), 6, true);
		injector::MakeNOP((0x506C5D), 6, true);
		injector::MakeNOP((0x506C63), 6, true);
		injector::MakeNOP((0x506C69), 6, true);
		injector::MakeNOP((0x506C6F), 6, true);
		injector::MakeNOP((0x506C75), 6, true);
		injector::MakeNOP((0x506C7B), 6, true);

		injector::MakeNOP((0x506883), 2, true);
		injector::MakeNOP((0x54DDE0), 2, true);
		injector::MakeNOP((0x54DD3B), 2, true);

		//tvvs controls patch #1
		injector::MakeJMP(0x506EEF, 0x729F00);
		injector::MakeCALL(0x729F00, ServiceControlsPatch);
		injector::WriteMemoryRaw(0x729F05, "\xBF\x00\x00\x00\x00", 5, true);
		injector::WriteMemoryRaw(0x729F0A, "\x39\x3D\x30\xC3\x98\x00", 6, true);
		injector::MakeJMP(0x729F10, 0x506EF5);
		//tvvs controls patch #2
		injector::MakeJMP(0x500A70, 0x729F20);
		injector::MakeCALL(0x729F20, ServiceControlsPatch);
		injector::WriteMemoryRaw(0x729F25, "\x83\x3D\x80\xC3\x98\x00\x00", 7, true);
		injector::WriteMemoryRaw(0x729F2C, "\x0F\x84\xB9\x6B\xDD\xFF", 6, true);
		injector::MakeJMP(0x729F32, 0x500A79);

		CreateThread(NULL, 0, InputRT9, NULL, 0, NULL);

		// auto accl off 
		if (ToBool(config["General"]["AutoAcclOff"]))
		{
			injector::MakeNOP((0xAA6E6 + BaseAddress9), 6, true);
			// alternative patch
		//	injector::WriteMemoryRaw((0x96fd70), "\x00", 1, true);
		}

		if (ToBool(config["General"]["Windowed"]))
		{
			injector::MakeJMP(0x63B332, 0x729EF0);
			injector::MakeCALL(0x729EF0, D3D9CreateParamPatch);
			injector::MakeJMP(0x729EF5, 0x63B332);
		}

	}, GameID::DirtyDrivin);
#endif
#pragma optimize("", on)