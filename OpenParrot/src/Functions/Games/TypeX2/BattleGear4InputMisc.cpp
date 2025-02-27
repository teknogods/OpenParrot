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

void BG4General(Helpers* helpers)
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