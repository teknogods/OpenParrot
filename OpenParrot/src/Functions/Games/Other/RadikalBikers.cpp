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
			*(BYTE*)(imageBase + 0x668822C) -= 0x04;
		}
	}
	else
	{
		if (Service)
		{
			Service = false;
			*(BYTE*)(imageBase + 0x668822C) += 0x04;
		}
	}

	if (*ffbOffset & 0x400) //Coin
	{
		if (!Coin)
		{
			Coin = true;
			*(BYTE*)(imageBase + 0x668822C) -= 0x01;
		}
	}
	else
	{
		if (Coin)
		{
			Coin = false;
			*(BYTE*)(imageBase + 0x668822C) += 0x01;
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

static void DebugBreakFunc(){}

static InitFunction RadikalBikersFunc([]()
	{
		//Find imagebase address
		imageBase = (DWORD)GetModuleHandleA(0);

		//Disable native inputs etc
		injector::MakeRET(0x4027E0);

		//Set buttons to default values
		injector::WriteMemory<BYTE>(imageBase + 0x668822C, 0xEF, false);
		injector::WriteMemory<BYTE>(imageBase + 0x668820D, 0xFF, false);

		//Fix crash on medium stage
		MH_Initialize();
		MH_CreateHookApi(L"kernelbase.dll", "DebugBreak", DebugBreakFunc, NULL);
		MH_EnableHook(MH_ALL_HOOKS);

		//Create thread for inputs etc
		CreateThread(NULL, 0, RunningLoop, NULL, 0, NULL);

	}, GameID::RadikalBikers);
#endif