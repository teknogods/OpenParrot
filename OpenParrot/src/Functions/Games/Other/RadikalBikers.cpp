#include <StdInc.h>
#include "Utility/InitFunction.h"
#include "Functions/Global.h"
#include "Utility\Hooking.Patterns.h"

#pragma comment(lib, "Ws2_32.lib")

#if _M_IX86
extern int* ffbOffset;
extern int* ffbOffset2;

DWORD imageBase;

static bool Accl;
static bool Brake;
static bool View;
static bool HandleUp;
static bool Service;
static bool Test;
static bool VolUp;
static bool VolDown;
static bool Start;
static bool Coin;

static void ReturnKey()
{
	INPUT ip;

	ip.type = INPUT_KEYBOARD;
	ip.ki.wScan = 0;
	ip.ki.time = 0;
	ip.ki.dwExtraInfo = 0;

	ip.ki.wVk = 0x0D;
	ip.ki.dwFlags = 0;
	SendInput(1, &ip, sizeof(INPUT));
}

static int ThreadLoop()
{
	if (!(ToBool(config["General"]["Setup Screen"])))
	{
		HWND hWnd = FindWindowA(0, ("Radikal Bikers Setup"));
		if (hWnd > NULL)
		{
			SetForegroundWindow(hWnd);
			ReturnKey();
		}
	}

	*(BYTE*)(imageBase + 0xB1CA5A4) = 0x00; //Force Mouse Mode Off

	if (*ffbOffset & 0x01) //Handle Up
	{
		if (!HandleUp)
		{
			HandleUp = true;
			*(BYTE*)(imageBase + 0x668820D) -= 0x08;
		}
	}
	else
	{
		if (HandleUp)
		{
			HandleUp = false;
			*(BYTE*)(imageBase + 0x668820D) += 0x08;
		}
	}

	if (*ffbOffset & 0x02) //Accl
	{
		if (!Accl)
		{
			Accl = true;
			*(BYTE*)(imageBase + 0x668820D) -= 0x40;
		}
	}
	else
	{
		if (Accl)
		{
			Accl = false;
			*(BYTE*)(imageBase + 0x668820D) += 0x40;
		}
	}

	if (*ffbOffset & 0x04) //Brake
	{
		if (!Brake)
		{
			Brake = true;
			*(BYTE*)(imageBase + 0x668820D) -= 0x20;
		}
	}
	else
	{
		if (Brake)
		{
			Brake = false;
			*(BYTE*)(imageBase + 0x668820D) += 0x20;
		}
	}

	if (*ffbOffset & 0x08) //View
	{
		if (!View)
		{
			View = true;
			*(BYTE*)(imageBase + 0x668820D) -= 0x10;
		}
	}
	else
	{
		if (View)
		{
			View = false;
			*(BYTE*)(imageBase + 0x668820D) += 0x10;
		}
	}

	if (*ffbOffset & 0x100) //Test
	{
		if (!Test)
		{
			Test = true;
			*(BYTE*)(imageBase + 0x715FC28) += 0x10;
		}
	}
	else
	{
		if (Test)
		{
			Test = false;
		}
	}

	if (*ffbOffset & 0x200) //Service
	{
		if (!Service)
		{
			Service = true;
			*(BYTE*)(imageBase + 0x715FC28) += 0x20;
		}
	}
	else
	{
		if (Service)
		{
			Service = false;
		}
	}

	if (*ffbOffset & 0x400) //Coin
	{
		if (!Coin)
		{
			Coin = true;
			*(BYTE*)(imageBase + 0x715FC28) += 0x20;
		}
	}
	else
	{
		if (Coin)
		{
			Coin = false;
		}
	}

	if (*ffbOffset & 0x1000) //Start
	{
		if (!Start)
		{
			Start = true;
			*(BYTE*)(imageBase + 0x668820D) -= 0x80;
		}
	}
	else
	{
		if (Start)
		{
			Start = false;
			*(BYTE*)(imageBase + 0x668820D) += 0x80;
		}
	}

	if (*ffbOffset & 0x2000) //Vol Up
	{
		if (!VolUp)
		{
			VolUp = true;
			*(BYTE*)(imageBase + 0x668820D) -= 0x01;
		}
	}
	else
	{
		if (VolUp)
		{
			VolUp = false;
			*(BYTE*)(imageBase + 0x668820D) += 0x01;
		}
	}

	if (*ffbOffset & 0x4000) //Vol Down
	{
		if (!VolDown)
		{
			VolDown = true;
			*(BYTE*)(imageBase + 0x668820D) -= 0x02;
		}
	}
	else
	{
		if (VolDown)
		{
			VolDown = false;
			*(BYTE*)(imageBase + 0x668820D) += 0x02;
		}
	}

	*(BYTE*)(imageBase + 0x83148) = *ffbOffset2; //Handle

	return 0;
}

static DWORD WINAPI RunningLoop(LPVOID lpParam)
{
	while (true)
	{
		ThreadLoop();
		Sleep(16);
	}
}

static BOOL(__stdcall* original_DebugBreak)();

static void DebugBreakFunc()
{
#if _DEBUG
	OutputDebugStringA("DEBUG BREAK CALLED");
#endif
}

static InitFunction RadikalBikersFunc([]()
	{
		imageBase = (DWORD)GetModuleHandleA(0);

		//Disable Native Handle
		injector::MakeNOP(imageBase + 0x28FC, 5);
		injector::MakeNOP(imageBase + 0x28DA, 5);
		injector::MakeNOP(imageBase + 0x29B7, 10);
		injector::MakeNOP(imageBase + 0x2907, 10);
		injector::MakeNOP(imageBase + 0x29A1, 10);
		injector::MakeNOP(imageBase + 0x28E5, 10);

		//Disable Native Buttons
		injector::MakeNOP(imageBase + 0x27E5, 5);

		//Buttons 0xFF by default
		injector::WriteMemory<BYTE>(imageBase + 0x668820D, 0xFF, false);

		//Disable Native Keyboard Inputs
		injector::WriteMemoryRaw(imageBase + 0x2811, "\x00", 1, true);
		injector::WriteMemoryRaw(imageBase + 0x2821, "\x00", 1, true);
		injector::WriteMemoryRaw(imageBase + 0x2834, "\x00", 1, true);
		injector::WriteMemoryRaw(imageBase + 0x2847, "\x00", 1, true);
		injector::WriteMemoryRaw(imageBase + 0x2857, "\x00", 1, true);
		injector::WriteMemoryRaw(imageBase + 0x2867, "\x00", 1, true);
		injector::WriteMemoryRaw(imageBase + 0x2877, "\x00", 1, true);
		injector::WriteMemoryRaw(imageBase + 0x2887, "\x00", 1, true);

		//Fix Crash on Medium Stage
		MH_Initialize();
		MH_CreateHookApi(L"kernelbase.dll", "DebugBreak", &DebugBreakFunc, (void**)&original_DebugBreak);
		MH_EnableHook(MH_ALL_HOOKS);

		//Create Thread For Inputs etc
		CreateThread(NULL, 0, RunningLoop, NULL, 0, NULL);

	}, GameID::RadikalBikers);
#endif