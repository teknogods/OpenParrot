#include <StdInc.h>
#include "Utility/InitFunction.h"
#include "Functions/Global.h"
#include "Utility\Hooking.Patterns.h"

#pragma comment(lib, "Ws2_32.lib")

#ifndef _M_AMD64
extern int* wheelSection;
extern int* ffbOffset;
extern int* ffbOffset2;
extern int* ffbOffset3;
extern int* ffbOffset4;
int FFBDeadzoneMaxMin;
bool daytonaPressStart = false;
uintptr_t imageBase;
bool shiftup = false;
bool shiftdown = false;
static bool viewchange = false;
static bool keybdleft = false;
static bool keybdright = false;
static bool keybdup = false;

float Cubic(const float x, const float weight) 
{
	return weight * x * x * x + (1.0 - weight) * x;
}

float joystickCubicScaledDeadband(const float x)
{
	const float deadbandCutoff = 0.2f;
	const float weight = 0.5f;

	if (fabs(x) < deadbandCutoff)
	{
		return 0;
	}
	else 
	{
		return (Cubic(x, weight) - (fabs(x) / x) * Cubic(deadbandCutoff, weight)) / (1.0 - Cubic(deadbandCutoff, weight));
	}
}

void ShiftUp(BYTE shift)
{
	*(BYTE*)(imageBase + 0x15B468C) = shift + 1;
}

void ShiftDown(BYTE shift)
{
	*(BYTE*)(imageBase + 0x15B468C) = shift - 1;
}

static int ThreadLoop()
{
	DWORD buttons2 = *wheelSection;
	DWORD buttons = *ffbOffset;
	BYTE wheel = *ffbOffset2;
	BYTE gas = *ffbOffset3;
	BYTE brake = *ffbOffset4;

	BYTE gamestate = *(BYTE*)(imageBase + 0x15B5744);
	BYTE gear = *(BYTE*)(imageBase + 0x15B468C);
	BYTE viewread = *(BYTE*)(imageBase + 0x15B5DB0);

	*(BYTE*)(imageBase + 0x15B4679) = gas;
	*(BYTE*)(imageBase + 0x15B467A) = brake;

	HWND hWnd = FindWindowA(0, ("Daytona Championship USA"));

	if (ToBool(config["General"]["Cubic Scaled Deadband"]))
	{
		float joycubicdeadband = joystickCubicScaledDeadband((*ffbOffset2 - 128) / 128.0);
		wheel = 128 + (joycubicdeadband * 128.0);

		if (wheel >= 251)
			wheel = 255;
	}
	
	//Menu Movement & Game Initial Screen
	if (gamestate == 18 || gamestate == 30)
	{
		if ((wheel <= 0x75) && (wheel > 0x50))
		{
			//Menu Left
			if (!keybdleft)
			{
				keybdleft = true;
				SendMessage(hWnd, WM_KEYDOWN, VK_LEFT, 0);
			}
			else
			{
				SendMessage(hWnd, WM_KEYUP, VK_LEFT, 0);
			}
		}
		else if (wheel <= 0x50)
		{
			SendMessage(hWnd, WM_KEYDOWN, VK_LEFT, 0);
		}
		else if ((wheel >= 0x89) && (wheel < 0xAE))
		{
			//Menu Right
			if (!keybdright)
			{
				keybdright = true;
				SendMessage(hWnd, WM_KEYDOWN, VK_RIGHT, 0);
			}
			else
			{
				SendMessage(hWnd, WM_KEYUP, VK_RIGHT, 0);
			}
		}
		else if (wheel >= 0xAE)
		{
			SendMessage(hWnd, WM_KEYDOWN, VK_RIGHT, 0);
		}
		else
		{
			keybdleft = false;
			keybdright = false;
			SendMessage(hWnd, WM_KEYUP, VK_RIGHT, 0);
			SendMessage(hWnd, WM_KEYUP, VK_LEFT, 0);
		}
		keybdup = false;
	}
	else
	{
		if (!keybdup)
		{
			keybdup = true;
			keybdleft = false;
			keybdright = false;
			SendMessage(hWnd, WM_KEYUP, VK_RIGHT, 0);
			SendMessage(hWnd, WM_KEYUP, VK_LEFT, 0);
		}
	}

	DWORD track = *(DWORD*)(imageBase + 0x011B0148);
	if (track != 0)
	{
		BYTE track1 = *(BYTE*)(track + 0x4);
#ifdef _DEBUG
		info(true, "%02X %02X", track1, gamestate);
#endif
		if ((track1 == 2 || track1 == 4) && (gamestate == 0x16))
		{
			BYTE reverse = wheel * 0xFF;
			if (reverse == 0x00)
				*(BYTE*)(imageBase + 0x15B4678) = 0xFF;
			else
				*(BYTE*)(imageBase + 0x15B4678) = reverse;
#ifdef _DEBUG
			info(true, "Reverse wheel");
#endif
		}
		else
		{
			*(BYTE*)(imageBase + 0x15B4678) = wheel;
#ifdef _DEBUG
			info(true, "Normal wheel1");
#endif
		}
	}
	else
	{
		*(BYTE*)(imageBase + 0x15B4678) = wheel;
#ifdef _DEBUG
		info(true, "Normal wheel2");
#endif
	}

	if (gas >= 0x40)
	{
		daytonaPressStart = true;
	}
	else
	{
		daytonaPressStart = false;
	}

	if (buttons == 0x01)
	{
		//Inject Start
		DWORD p = *(DWORD*)(imageBase + 0x1AB0010);
		if (p != 0)
		{
			DWORD p1 = *(DWORD*)(p + 0x00);
			if (p1 != 0)
			{
				*(DWORD*)(p1 + 0x08) = 0x01;
			}
		}
	}
	else
	{
		DWORD p = *(DWORD*)(imageBase + 0x1AB0010);
		if (p != 0)
		{
			DWORD p1 = *(DWORD*)(p + 0x00);
			if (p1 != 0)
			{
				*(DWORD*)(p1 + 0x08) = 0x00;
			}
		}
	}

	if ((buttons == 0x02) && (gear < 0x03))
	{
		if (!shiftup)
		{
			shiftup = true;
			ShiftUp(*(BYTE*)(imageBase + 0x15B468C));
		}
	}
	else
	{
		shiftup = false;
	}

	if ((buttons == 0x04) && (gear > 0x00))
	{
		if (!shiftdown)
		{
			shiftdown = true;
			ShiftDown(*(BYTE*)(imageBase + 0x15B468C));
		}
	}
	else
	{
		shiftdown = false;
	}

	if (buttons2 == 0x01)
	{
		//Gear Change 1
		*(BYTE*)(imageBase + 0x15B468C) = 0x00;
	}

	if (buttons2 == 0x02)
	{
		//Gear Change 2
		*(BYTE*)(imageBase + 0x15B468C) = 0x01;
	}

	if (buttons2 == 0x04)
	{
		//Gear Change 3
		*(BYTE*)(imageBase + 0x15B468C) = 0x02;
	}

	if (buttons2 == 0x08)
	{
		//Gear Change 4
		*(BYTE*)(imageBase + 0x15B468C) = 0x03;
	}

	if (ToBool(config["General"]["View Change 1 To Cycle All Views"]))
	{
		if ((buttons2 == 0x10) && (viewread < 4))  //INCLUDES SPECIAL VIEW
		{
			//View Change 1
			if (!viewchange)
			{
				viewchange = true;
				*(BYTE*)(imageBase + 0x15B5DB0) = ++viewread;
			}
		}
		else if ((buttons2 == 0x10) && (viewread == 4))
		{
			if (!viewchange)
			{
				viewchange = true;
				*(BYTE*)(imageBase + 0x15B5DB0) = 0x00;
			}
		}
		else
		{
			viewchange = false;
		}
	}
	else
	{
		if (buttons2 == 0x10)
		{
			//View Change 1
			*(DWORD*)(imageBase + 0x15B5DB0) = 0x00;
		}

		if (buttons2 == 0x20)
		{
			//View Change 2
			*(DWORD*)(imageBase + 0x15B5DB0) = 0x01;
		}

		if (buttons2 == 0x40)
		{
			//View Change 3
			*(DWORD*)(imageBase + 0x15B5DB0) = 0x02;
		}

		if (buttons2 == 0x80)
		{
			//View Change 4
			*(DWORD*)(imageBase + 0x15B5DB0) = 0x03;
		}
	}
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

static InitFunction Daytona3Func([]()
	{
		imageBase = (uintptr_t)GetModuleHandleA(0);
		injector::WriteMemoryRaw(imageBase + 0xDD697, "\x90\x90\x90\x90\x90\x90\x38\x05\xC8\xF9\x5A\x01\x90\x90\x90\x90\x90\x90", 18, true);
		injector::WriteMemoryRaw(imageBase + 0x12958F, "\x33\xC0\x8A\x45\x08\x90\x90\x90\x90\x72\x08\x66\xA3\x46\xFC\x5A\x01\xEB\x06\x66\xA3\x44\xFC\x5A\x01\x31\xFF\x31\xF6\x47\xBE\x0F\x00\x00\x00\xEB\x4C\x90\x90\x90\x90", 41, true);
		injector::WriteMemoryRaw(imageBase + 0xD8616C, "\x2E\x2E\x2F\x73\x68\x65\x6C\x6C\x00\x00\x00\x00\x00\x00\x00\x00\x2E\x2E\x2F\x73\x68\x65\x6C\x6C\x5C\x49\x6E\x70\x57\x72\x61\x70\x70\x65\x72\x2E\x65\x78\x65\x00\x00\x00\x00\x00\x00\x00\x00\x00", 48, true);
		injector::WriteMemoryRaw(imageBase + 0x7D087, "\x85\xFF\x0F\x84\xE9\x01\x00\x00\x6A\x00\x8B\xCF\xE8\xC8\xD2\x21\x00\x8D\x80\x00\x80\x00\x00\x89\x46\x40\x6A\x05\x8B\xCF\xE8\xB6\xD2\x21\x00\xC1\xE0\x08\x89\x46\x44\x6A\x04\x8B\xCF\xE8\xA7\xD2\x21\x00\xC1\xE0\x08\x89\x46\x48\x90\xEB\x2E\xE8\xE9\xD4\x21\x00\x8B\x5E\x04\x8B\xF8\x8B\xCF\xE8\x2D\xD5\x21\x00\x8D\x4B\x01\x3B\xC1\x0F\x8C\x9A\x01\x00\x00\x53\x8B\xCF\xE8\xDA\xD4\x21\x00\x8B\xF8\xEB\x9D\x90\x90\x90\x90\x90\x90", 105, true);
		injector::MakeNOP(imageBase + 0x1DDDFA, 5);
		injector::MakeNOP(imageBase + 0x1DDE1E, 6);
		injector::MakeNOP(imageBase + 0x1DDE45, 6);
		injector::MakeNOP(imageBase + 0x1DE10D, 6);
		injector::MakeNOP(imageBase + 0x29B481, 3);
		injector::MakeNOP(imageBase + 0x29B513, 4);

		if (ToBool(config["General"]["MSAA4X Disable"]))
			injector::WriteMemoryRaw(imageBase + 0x17CD3D, "\x00", 1, true);

		if (ToBool(config["General"]["Hide Cursor"]))
			ShowCursor(false);

		std::string FFBDeadzoneString = config["General"]["FFB Deadzone Percent"];
		int FFBDeadzone = std::stoi(FFBDeadzoneString);
		FFBDeadzoneMaxMin = (128 * FFBDeadzone) / 100.0;

		CreateThread(NULL, 0, RunningLoop, NULL, 0, NULL);

	}, GameID::Daytona3);
#endif