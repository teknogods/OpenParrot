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
uintptr_t imageBase;
bool shiftup = false;
bool shiftdown = false;

void ShiftUp(BYTE shift)
{
	*(BYTE *)(imageBase + 0x15B468C) = shift + 1;	
}

void ShiftDown(BYTE shift)
{
	*(BYTE *)(imageBase + 0x15B468C) = shift - 1;
}

static void InjectKeys()
{
	DWORD buttons2 = *wheelSection;
	DWORD buttons = *ffbOffset;
	BYTE wheel = *ffbOffset2;
	BYTE gas = *ffbOffset3;
	BYTE brake = *ffbOffset4;
	BYTE gamestate = *(BYTE *)(imageBase + 0x15B5744);
	BYTE gear = *(BYTE *)(imageBase + 0x15B468C);
	
	*(BYTE *)(imageBase + 0x15B4679) = gas;
	*(BYTE *)(imageBase + 0x15B467A) = brake;
	DWORD track = *(DWORD *)(imageBase + 0x011B0148);
	if (track != 0)
	{
		BYTE track1 = *(BYTE *)(track + 0x4);
		if (track1 != 0)
		{
			info(true, "%02X %02X", track1, gamestate);
			if ((track1 == 2 || track1 == 4) && (gamestate == 0x16))
			{
				BYTE reverse = wheel * 0xFF;
				if (reverse == 0x00)
					*(BYTE *)(imageBase + 0x15B4678) = 0xFF;
				else
					*(BYTE *)(imageBase + 0x15B4678) = reverse;
				info(true, "Reverse wheel");
			}
			else
			{
				*(BYTE *)(imageBase + 0x15B4678) = wheel;
				info(true, "Normal wheel1");
			}
		}
	}
	else
	{
		*(BYTE *)(imageBase + 0x15B4678) = wheel;
		info(true, "Normal wheel2");
	}
	

	
	if (wheel <= 0x40)
	{
		//Menu Left
		DWORD p = *(DWORD *)(imageBase + 0x1AAFFF0);
		if (p != 0)
		{
			DWORD p1 = *(DWORD *)(p + 0x00);
			if (p1 != 0)
			{
				*(BYTE *)(p1 + 0x25) = 0xFF;
			}
		}
	}
	else if (wheel >= 0xC0)
	{
		//Menu Right
		DWORD p = *(DWORD *)(imageBase + 0x1AAFFF0);
		if (p != 0)
		{
			DWORD p1 = *(DWORD *)(p + 0x00);
			if (p1 != 0)
			{
				*(BYTE *)(p1 + 0x27) = 0xFF;
			}
		}
	}
	else
	{
		DWORD p = *(DWORD *)(imageBase + 0x1AAFFF0);
		if (p != 0)
		{
			DWORD p1 = *(DWORD *)(p + 0x00);
			if (p1 != 0)
			{
				*(BYTE *)(p1 + 0x25) = 0x00;
				*(BYTE *)(p1 + 0x27) = 0x00;
			}
		}
	}

	if (buttons == 0x01)
	{
		//Inject Start
		DWORD p = *(DWORD *)(imageBase + 0x1AB0010);
		if (p != 0)
		{
			DWORD p1 = *(DWORD *)(p + 0x00);
			if (p1 != 0)
			{
				*(DWORD *)(p1 + 0x08) = 0x01;
			}
		}
	}
	else
	{
		DWORD p = *(DWORD *)(imageBase + 0x1AB0010);
		if (p != 0)
		{
			DWORD p1 = *(DWORD *)(p + 0x00);
			if (p1 != 0)
			{
				*(DWORD *)(p1 + 0x08) = 0x00;
			}
		}
	}

	if ((buttons == 0x02) && (gear < 0x03))
	{
		if (!shiftup)
		{
			shiftup = true;
			ShiftUp(*(BYTE *)(imageBase + 0x15B468C));
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
			ShiftDown(*(BYTE *)(imageBase + 0x15B468C));
		}
	}
	else
	{
		shiftdown = false;
	}

	if (buttons2 == 0x01)
	{
		//Gear Change 1
		*(BYTE *)(imageBase + 0x15B468C) = 0x00;
	}

	if (buttons2 == 0x02)
	{
		//Gear Change 2
		*(BYTE *)(imageBase + 0x15B468C) = 0x01;
	}

	if (buttons2 == 0x04)
	{
		//Gear Change 3
		*(BYTE *)(imageBase + 0x15B468C) = 0x02;
	}

	if (buttons2 == 0x08)
	{
		//Gear Change 4
		*(BYTE *)(imageBase + 0x15B468C) = 0x03;
	}

	if (buttons2 == 0x10)
	{
		//View Change 1
		{
			*(DWORD *)(imageBase + 0x15B5DB0) = 0x00;
		}
	}

	if (buttons2 == 0x20)
	{
		//View Change 2
		{
			*(DWORD *)(imageBase + 0x15B5DB0) = 0x01;
		}
	}

	if (buttons2 == 0x40)
	{
		//View Change 3
		{
			*(DWORD *)(imageBase + 0x15B5DB0) = 0x02;
		}
	}

	if (buttons2 == 0x80)
	{
		//View Change 4
		{
			*(DWORD *)(imageBase + 0x15B5DB0) = 0x03;
		}
	}

	if (buttons2 == 0xF0)
	{
		//View Change 5
		{
			*(DWORD *)(imageBase + 0x15B5DB0) = 0x04;
		}
	}
}

static InitFunction Daytona3Func([]()
{
    imageBase = (uintptr_t)GetModuleHandleA(0);
	injector::WriteMemoryRaw(imageBase + 0xDD697, "\x90\x90\x90\x90\x90\x90\x38\x05\xC8\xF9\x5A\x01\x90\x90\x90\x90\x90\x90", 18, true);
	injector::MakeNOP(imageBase + 0x1DDDFA, 5);
	injector::MakeNOP(imageBase + 0x1DDE1E, 6);
	injector::MakeNOP(imageBase + 0x1DDE45, 6);
	injector::MakeNOP(imageBase + 0x1DE10D, 6); 
	injector::MakeNOP(imageBase + 0x29B481, 3);
	injector::MakeNOP(imageBase + 0x29B513, 4);
	
	injector::MakeCALL(imageBase + 0x1DDDFA, InjectKeys);
}, GameID::Daytona3);
#endif