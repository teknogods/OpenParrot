#include <StdInc.h>
#include "Utility/GameDetect.h"
#include "Utility/InitFunction.h"
#include "Functions/Global.h"
#include "Utility/Helper.h"

static bool CoinPressed;
static bool TestPressed;
static bool ServicePressed;
static bool TimeStartPressed;
static bool StartPressed;
static bool ViewPressed;
static bool HazardPressed;
static bool OverTakePressed;
static bool ShiftLeftPressed;
static bool ShiftRightPressed;
static bool SideBrakePressed;
static bool SeatSwitch1Pressed;
static bool SeatSwitch2Pressed;
static bool Gear1Pressed;
static bool Gear2Pressed;
static bool Gear3Pressed;
static bool Gear4Pressed;
static bool Gear5Pressed;
static bool Gear6Pressed;
static bool KeyPressed;
static bool init;
static bool MenuHack;
static bool MenuHackDelay;
static bool TestMode;
static bool StopPausing;
static bool init2;
bool BG4EnableTracks;

static DWORD imageBase;

extern int* wheelSection;
extern int* ffbOffset;
extern int* ffbOffset3;
extern int* ffbOffset4;
extern int* ffbOffset5;

static void BG4General(Helpers* helpers)
{
	if (!init2)
	{
		UINT8 WaitForAttract = helpers->ReadByte(0x42D964, true);

		if (WaitForAttract)
		{
			init2 = true;

			if (BG4EnableTracks)
				helpers->WriteIntPtr(0x4165C0, 0x3FFFFFFF, true);
		}
	}

	UINT8 ScreenVal = helpers->ReadByte(0x4E3668, true);

	if (ScreenVal == 0x08)
	{
		if (!StopPausing)
			StopPausing = true;
	}

	if (StopPausing)
		helpers->WriteByte(0x42D29C, 0x01, true);
}

void BG4ManualHack(Helpers* helpers) //Hack to allow us to select Manual	
{	
	INT_PTR MenuTimerBase = helpers->ReadIntPtr(0x4C2924, true);
	INT_PTR MenuTimerBaseA = helpers->ReadIntPtr(MenuTimerBase + 0x08, false);
	INT_PTR MenuTime = helpers->ReadIntPtr(MenuTimerBaseA + 0x45C, false);

	BG4General(0);

	if (MenuTime == 0x1194)
	{
		if (!MenuHack)
			MenuHack = true;
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

static void BG4ProManualHack(Helpers* helpers) //Hack to allow us to select Manual and Manual with Clutch
{	
	INT_PTR MenuTimerBase = helpers->ReadIntPtr(0x4C2924, true);
	INT_PTR MenuTimerBaseA = helpers->ReadIntPtr(MenuTimerBase + 0x08, false);
	INT_PTR MenuTime = helpers->ReadIntPtr(MenuTimerBaseA + 0x45C, false);

	INT_PTR VehicleSelectionBase = helpers->ReadIntPtr(0x42D4A0, true);
	INT_PTR VehicleSelectionOff1 = helpers->ReadIntPtr(VehicleSelectionBase + 0x78, false);
	INT_PTR VehicleSelectionOff2 = helpers->ReadIntPtr(VehicleSelectionOff1 + 0x190, false);
	UINT8 VehicleSelection = helpers->ReadByte(VehicleSelectionOff2 + 0x20, false);

	if (VehicleSelection)
	{
		if (!MenuHack)
			MenuHack = true;
	}

	if (MenuTime == 0x00)
	{
		if (MenuHack)
			MenuHack = false;
	}

	if (MenuHack)
	{
		helpers->WriteByte(MenuTimerBaseA + 0x454, 0x04, false);
		BYTE This = helpers->ReadByte(MenuTimerBaseA + 0x44C, false);

		switch (This)
		{
			case 0x02:
				helpers->WriteByte(0x42E341, 0xD0, true);  //Set Shift SEN 2 to ON or error
				break;
			case 0x03:
				helpers->WriteByte(0x42E341, 0xE0, true);  //Set Shift SEN 1 to ON or error
				break;
		}
	}
}

void BG4ProInputs(Helpers* helpers)
{
	if (!init)
	{
		imageBase = (DWORD)GetModuleHandleA(0);
		UINT8 WaitForAttract = helpers->ReadByte(0x42E386, true);

		if (WaitForAttract)
		{
			helpers->WriteByte(0x42E296, 0x01, true);
			helpers->WriteByte(0x42E295, 0x80, true);
			injector::MakeNOP(imageBase + 0x27400, 6);
			init = true;
		}
	}

	BG4General(0);
	BG4ProManualHack(0);
	
	UINT8 KeyInput = helpers->ReadByte(0x42E296, true);

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