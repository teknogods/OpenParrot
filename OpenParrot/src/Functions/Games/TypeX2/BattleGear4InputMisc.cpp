#include <StdInc.h>
#include "Utility/GameDetect.h"
#include "Utility/InitFunction.h"
#include "Functions/Global.h"
#include "Utility/Helper.h"

static bool CoinPressed = false;
static bool TestPressed = false;
static bool ServicePressed = false;
static bool TimeStartPressed = false;
static bool StartPressed = false;
static bool ViewPressed = false;
static bool HazardPressed = false;
static bool OverTakePressed = false;
static bool ShiftLeftPressed = false;
static bool ShiftRightPressed = false;
static bool SideBrakePressed = false;
static bool SeatSwitch1Pressed = false;
static bool SeatSwitch2Pressed = false;
static bool Gear1Pressed = false;
static bool Gear2Pressed = false;
static bool Gear3Pressed = false;
static bool Gear4Pressed = false;
static bool Gear5Pressed = false;
static bool Gear6Pressed = false;
static bool KeyPressed = false;
static bool init = false;
static bool MenuHack = false;
static bool MenuHackDelay = false;
static bool TestMode = false;

extern int* wheelSection;
extern int* ffbOffset;
extern int* ffbOffset3;
extern int* ffbOffset4;
extern int* ffbOffset5;
static Helpers* myHelpers;

static DWORD WINAPI ChangeValues(LPVOID lpParam)
{
	Sleep(10000);

	DWORD imageBase = (DWORD)GetModuleHandleA(0);
	myHelpers->WriteByte(0x42E296, 0x01, true);
	myHelpers->WriteByte(0x42E295, 0x80, true);
	injector::MakeNOP(imageBase + 0x27400, 6);
	return 0;
}

void BG4ManualHack(Helpers* helpers)
{
	//Hack to allow us to select Manual		
	INT_PTR MenuTimerBase = helpers->ReadIntPtr(0x4C2924, true);
	INT_PTR MenuTimerBaseA = helpers->ReadIntPtr(MenuTimerBase + 0x08, false);
	INT_PTR MenuTime = helpers->ReadIntPtr(MenuTimerBaseA + 0x45C, false);

	if (MenuTime == 0x1194)
	{
		if (!MenuHack)
		{
			MenuHack = true;
		}
	}

	if (MenuTime == 0x00)
	{
		if (MenuHack)
		{
			MenuHack = false;
			MenuHackDelay = false;
		}
	}

	if (MenuHack)
	{
		if (!MenuHackDelay)
		{
			MenuHackDelay = true;
			Sleep(2500);
		}
		helpers->WriteByte(MenuTimerBaseA + 0x454, 0x03, false);
	}
}

void BG4ProInputs(Helpers* helpers)
{
	if (!init)
	{
		init = true;
		myHelpers = helpers;
		CreateThread(NULL, 0, ChangeValues, NULL, 0, NULL);
	}

	//Hack to allow us to select Manual and Manual with Clutch		
	INT_PTR MenuTimerBase = helpers->ReadIntPtr(0x4C2924, true);
	INT_PTR MenuTimerBaseA = helpers->ReadIntPtr(MenuTimerBase + 0x08, false);
	INT_PTR MenuTime = helpers->ReadIntPtr(MenuTimerBaseA + 0x45C, false);

	if (MenuTime == 0x1194)
	{
		if (!MenuHack)
		{
			MenuHack = true;
		}
	}

	if (MenuTime == 0x00)
	{
		if (MenuHack)
		{
			MenuHack = false;
			MenuHackDelay = false;
		}
	}

	if (MenuHack)
	{
		if (!MenuHackDelay)
		{
			MenuHackDelay = true;
			Sleep(2500);
		}
		helpers->WriteByte(MenuTimerBaseA + 0x454, 0x04, false);
		BYTE This = helpers->ReadByte(MenuTimerBaseA + 0x44C, false);

		if (This == 0x02)
		{
			helpers->WriteByte(0x42E341, 0xD0, true);  //Set Shift SEN 2 to ON or error
		}
		else if (This == 0x03)
		{
			helpers->WriteByte(0x42E341, 0xE0, true);  //Set Shift SEN 1 to ON or error
		}
	}

	DWORD imageBase = (DWORD)GetModuleHandleA(0);
	UINT8 KeyInput = helpers->ReadByte(imageBase + 0x42E296, false);

	if (*ffbOffset & 0x01) //Test
	{
		if (!TestPressed)
		{
			TestPressed = true;
			TestMode = true;
			*(BYTE*)(imageBase + 0x42E297) += 0x80;
		}
	}
	else
	{
		if (TestPressed)
		{
			TestPressed = false;
			*(BYTE*)(imageBase + 0x42E297) -= 0x80;
		}
	}

	if (*ffbOffset & 0x02) //Service
	{
		if (!ServicePressed)
		{
			ServicePressed = true;
			*(BYTE*)(imageBase + 0x42E296) += 0x40;
		}
	}
	else
	{
		if (ServicePressed)
		{
			ServicePressed = false;
			*(BYTE*)(imageBase + 0x42E296) -= 0x40;
		}
	}

	if (*ffbOffset & 0x04) //Coin Chute 1
	{
		if (!CoinPressed)
		{
			CoinPressed = true;
			*(BYTE*)(imageBase + 0x42E294) += 0x40;
			if (!TestMode) //Let's get that sweet sweet sexy coin sound
			{
				*(BYTE*)(imageBase + 0x42E296) += 0x40;
			}
		}
	}
	else
	{
		if (CoinPressed)
		{
			CoinPressed = false;
			*(BYTE*)(imageBase + 0x42E294) -= 0x40;
			if (!TestMode)
			{
				*(BYTE*)(imageBase + 0x42E296) -= 0x40;
			}
		}
	}

	if (*ffbOffset & 0x08) //Start
	{
		if (!StartPressed)
		{
			StartPressed = true;
			*(BYTE*)(imageBase + 0x42E296) += 0x80;
		}
	}
	else
	{
		if (StartPressed)
		{
			StartPressed = false;
			*(BYTE*)(imageBase + 0x42E296) -= 0x80;
		}
	}

	if (*ffbOffset & 0x10) //Key
	{
		if (!KeyPressed)
		{
			KeyPressed = true;
			if (!(KeyInput & 0x04))
			{
				*(BYTE*)(imageBase + 0x42E296) += 0x04;
			}
			else
			{
				*(BYTE*)(imageBase + 0x42E296) -= 0x04;
			}
		}
	}
	else
	{
		if (KeyPressed)
		{
			KeyPressed = false;
		}
	}

	if (*ffbOffset & 0x20) //View
	{
		if (!ViewPressed)
		{
			ViewPressed = true;
			*(BYTE*)(imageBase + 0x42E296) += 0x20;
		}
	}
	else
	{
		if (ViewPressed)
		{
			ViewPressed = false;
			*(BYTE*)(imageBase + 0x42E296) -= 0x20;
		}
	}

	if (*ffbOffset & 0x40) //Hazard
	{
		if (!HazardPressed)
		{
			HazardPressed = true;
			*(BYTE*)(imageBase + 0x42E296) += 0x10;
		}
	}
	else
	{
		if (HazardPressed)
		{
			HazardPressed = false;
			*(BYTE*)(imageBase + 0x42E296) -= 0x10;
		}
	}

	if (*ffbOffset & 0x80) //Overtake
	{
		if (!OverTakePressed)
		{
			OverTakePressed = true;
			*(BYTE*)(imageBase + 0x42E296) += 0x08;
		}
	}
	else
	{
		if (OverTakePressed)
		{
			OverTakePressed = false;
			*(BYTE*)(imageBase + 0x42E296) -= 0x08;
		}
	}

	if (*ffbOffset & 0x100) //SideBrake
	{
		if (!SideBrakePressed)
		{
			SideBrakePressed = true;
			*(BYTE*)(imageBase + 0x42E296) += 0x02;
		}
	}
	else
	{
		if (SideBrakePressed)
		{
			SideBrakePressed = false;
			*(BYTE*)(imageBase + 0x42E296) -= 0x02;
		}
	}

	if (*ffbOffset & 0x200) //Shift Right
	{
		if (!ShiftRightPressed)
		{
			ShiftRightPressed = true;
			*(BYTE*)(imageBase + 0x42E341) -= 0x40;
		}
	}
	else
	{
		if (ShiftRightPressed)
		{
			ShiftRightPressed = false;
			*(BYTE*)(imageBase + 0x42E341) += 0x40;
		}
	}

	if (*ffbOffset & 0x400) //Shift Left
	{
		if (!ShiftLeftPressed)
		{
			ShiftLeftPressed = true;
			*(BYTE*)(imageBase + 0x42E341) -= 0x80;
		}
	}
	else
	{
		if (ShiftLeftPressed)
		{
			ShiftLeftPressed = false;
			*(BYTE*)(imageBase + 0x42E341) += 0x80;
		}
	}

	if (*ffbOffset & 0x800) //Timer Start
	{
		if (!TimeStartPressed)
		{
			TimeStartPressed = true;
			*(BYTE*)(imageBase + 0x42E295) += 0x01;
		}
	}
	else
	{
		if (TimeStartPressed)
		{
			TimeStartPressed = false;
			*(BYTE*)(imageBase + 0x42E295) -= 0x01;
		}
	}

	if (*ffbOffset & 0x1000) //Seat Switch 1
	{
		if (!SeatSwitch1Pressed)
		{
			SeatSwitch1Pressed = true;
			*(BYTE*)(imageBase + 0x42E295) += 0x04;
		}
	}
	else
	{
		if (SeatSwitch1Pressed)
		{
			SeatSwitch1Pressed = false;
			*(BYTE*)(imageBase + 0x42E295) -= 0x04;
		}
	}

	if (*ffbOffset & 0x2000) //Seat Switch 2
	{
		if (!SeatSwitch2Pressed)
		{
			SeatSwitch2Pressed = true;
			*(BYTE*)(imageBase + 0x42E295) += 0x02;
		}
	}
	else
	{
		if (SeatSwitch2Pressed)
		{
			SeatSwitch2Pressed = false;
			*(BYTE*)(imageBase + 0x42E295) -= 0x02;
		}
	}

	if (*ffbOffset & 0x4000) //Gear 1
	{
		if ((!Gear1Pressed) && (!Gear2Pressed) && (!Gear3Pressed) && (!Gear4Pressed) && (!Gear5Pressed) && (!Gear6Pressed))
		{
			Gear1Pressed = true;
			*(BYTE*)(imageBase + 0x42E295) -= 0x80;
			*(BYTE*)(imageBase + 0x42E341) -= 0x80;
		}
	}
	else
	{
		if (Gear1Pressed)
		{
			Gear1Pressed = false;
			*(BYTE*)(imageBase + 0x42E295) += 0x80;
			*(BYTE*)(imageBase + 0x42E341) += 0x80;
		}
	}

	if (*ffbOffset & 0x8000) //Gear 2
	{
		if ((!Gear2Pressed) && (!Gear1Pressed) && (!Gear3Pressed) && (!Gear4Pressed) && (!Gear5Pressed) && (!Gear6Pressed))
		{
			Gear2Pressed = true;
			*(BYTE*)(imageBase + 0x42E296) -= 0x01;
			*(BYTE*)(imageBase + 0x42E341) -= 0x80;
		}
	}
	else
	{
		if (Gear2Pressed)
		{
			Gear2Pressed = false;
			*(BYTE*)(imageBase + 0x42E296) += 0x01;
			*(BYTE*)(imageBase + 0x42E341) += 0x80;
		}
	}

	if (*ffbOffset & 0x10000) //Gear 3 / Shift Up
	{
		if ((!Gear3Pressed) && (!Gear2Pressed) && (!Gear1Pressed) && (!Gear4Pressed) && (!Gear5Pressed) && (!Gear6Pressed))
		{
			Gear3Pressed = true;
			*(BYTE*)(imageBase + 0x42E296) -= 0x01;
		}
	}
	else
	{
		if (Gear3Pressed)
		{
			Gear3Pressed = false;
			*(BYTE*)(imageBase + 0x42E296) += 0x01;
		}
	}

	if (*ffbOffset & 0x20000) //Gear 4 / Shift Down
	{
		if ((!Gear4Pressed) && (!Gear2Pressed) && (!Gear3Pressed) && (!Gear1Pressed) && (!Gear5Pressed) && (!Gear6Pressed))
		{
			Gear4Pressed = true;
			*(BYTE*)(imageBase + 0x42E295) -= 0x80;
		}
	}
	else
	{
		if (Gear4Pressed)
		{
			Gear4Pressed = false;
			*(BYTE*)(imageBase + 0x42E295) += 0x80;
		}
	}

	if (*ffbOffset & 0x40000) //Gear 5
	{
		if ((!Gear5Pressed) && (!Gear2Pressed) && (!Gear3Pressed) && (!Gear4Pressed) && (!Gear1Pressed) && (!Gear6Pressed))
		{
			Gear5Pressed = true;
			*(BYTE*)(imageBase + 0x42E295) -= 0x80;
			*(BYTE*)(imageBase + 0x42E341) -= 0x40;
		}
	}
	else
	{
		if (Gear5Pressed)
		{
			Gear5Pressed = false;
			*(BYTE*)(imageBase + 0x42E295) += 0x80;
			*(BYTE*)(imageBase + 0x42E341) += 0x40;
		}
	}

	if (*ffbOffset & 0x80000) //Gear 6/Reverse
	{
		if ((!Gear6Pressed) && (!Gear2Pressed) && (!Gear3Pressed) && (!Gear4Pressed) && (!Gear5Pressed) && (!Gear1Pressed))
		{
			Gear6Pressed = true;
			*(BYTE*)(imageBase + 0x42E296) -= 0x01;
			*(BYTE*)(imageBase + 0x42E341) -= 0x40;
		}
	}
	else
	{
		if (Gear6Pressed)
		{
			Gear6Pressed = false;
			*(BYTE*)(imageBase + 0x42E296) += 0x01;
			*(BYTE*)(imageBase + 0x42E341) += 0x40;
		}
	}

	*(float*)(imageBase + 0x42D994) = (*wheelSection - 128) / 128.0; // Steering
	*(float*)(imageBase + 0x42D998) = (*ffbOffset3) / 255.0; // Gas
	*(float*)(imageBase + 0x42D99C) = (*ffbOffset4) / 255.0; // Brake
	*(float*)(imageBase + 0x42D9A0) = (*ffbOffset5) / 255.0; // Clutch
}