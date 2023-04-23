#include <StdInc.h>
#include "Utility/GameDetect.h"
#include "Utility/InitFunction.h"
#include "Functions/Global.h"
#include "Utility/Helper.h"

extern int* ffbOffset;
extern int* ffbOffset2;
extern int* ffbOffset3;
extern int* ffbOffset4;
extern int* ffbOffset5;

static bool Init;

static int VolumeUpCounter;
static int VolumeDownCounter;

static bool CoinPressed;
static bool Coin;

static DWORD imageBase;
UINT8 MusicGunGun2Volume;

static char VolPercentChar[256];
static char INIChar[256];

static void WriteVol()
{
	sprintf_s(INIChar, "%d", MusicGunGun2Volume);
	WritePrivateProfileStringA("Settings", "Volume", INIChar, ".\\OpenParrot\\Settings.ini");
}

static void VolumeSetting(Helpers* helpers)
{
	if (*ffbOffset & 0x10)
	{
		++VolumeUpCounter;

		if (VolumeUpCounter == 6)
		{
			VolumeUpCounter = 0;

			if (MusicGunGun2Volume < 100)
				++MusicGunGun2Volume;

			WriteVol();
		}
	}
	else
	{
		if (VolumeUpCounter)
			VolumeUpCounter = 0;
	}

	if (*ffbOffset & 0x20)
	{
		++VolumeDownCounter;

		if (VolumeDownCounter == 6)
		{
			VolumeDownCounter = 0;

			if (MusicGunGun2Volume > 0)
				--MusicGunGun2Volume;

			WriteVol();
		}
	}
	else
	{
		if (VolumeDownCounter)
			VolumeDownCounter = 0;
	}
}

static void VolChange(Helpers* helpers)
{
	DWORD VolBase = helpers->ReadInt32(0x2DBEEC, true);
	DWORD VolOff0 = helpers->ReadInt32(VolBase + 0x2C, false);
	DWORD VolOff1 = helpers->ReadInt32(VolOff0 + 0x10, false);

	sprintf_s(VolPercentChar, "%d%%", MusicGunGun2Volume);

	if (VolOff1)
		strcpy((char*)VolOff1 + 0x24C, VolPercentChar);

	DWORD VolOff2 = helpers->ReadInt32(VolOff1 + 0x1C, false);
	DWORD VolOff3 = helpers->ReadInt32(VolOff2 + 0x2C, false);
	DWORD VolOff4 = helpers->ReadInt32(VolOff3 + 0x3C, false);

	if (VolOff4)
		strcpy((char*)VolOff4 + 0x344, VolPercentChar);

}

char* (__cdecl* MusicGunGun2strncpyOri)(char* Destination, const char* Source, size_t Count);
char* __cdecl MusicGunGun2strncpy(char* Destination, const char* Source, size_t Count)
{
	MusicGunGun2strncpyOri(Destination, Source, Count);

	VolChange(0);

	return 0;
}

int(__cdecl* MusicGunGun2VolumeSetupOri)(float a1);
int __cdecl MusicGunGun2VolumeSetup(float a1)
{
	VolumeSetting(0);

	a1 = MusicGunGun2Volume / 100.0;

	if (a1 == 0.0)
		a1 = 0.001;

	return MusicGunGun2VolumeSetupOri(a1);
}

void MusicGunGun2Inputs(Helpers* helpers)
{
	if (!Init)
	{
		Init = true;
		imageBase = (DWORD)GetModuleHandleA(0);
	}

	*(BYTE*)(imageBase + 0x2B8128) = 0x02; // Enable Inputs
	*(BYTE*)(imageBase + 0x2B3708) = 0x03; // Force JVS Type

	DWORD CoinButtonBase = helpers->ReadInt32(0x2B36F8, true);
	UINT8 CoinButtonPressed = helpers->ReadByte(CoinButtonBase + 0x29, false);

	DWORD CoinCountBase = helpers->ReadInt32(0x2B36F8, true);

	if (CoinButtonPressed & 0x01)
	{
		if (!Coin)
			Coin = true;
	}
	else
	{
		if (CoinPressed)
			CoinPressed = false;

		if (Coin)
			Coin = false;
	}

	if (Coin)
	{
		if (!CoinPressed)
		{
			CoinPressed = true;
			++*(BYTE*)(CoinCountBase + 0x18);
		}
	}

	bool P1SceenIn = !(*ffbOffset2 <= 1 || *ffbOffset2 >= 254 || *ffbOffset3 <= 1 || *ffbOffset3 >= 254);
	bool P2SceenIn = !(*ffbOffset4 <= 1 || *ffbOffset4 >= 254 || *ffbOffset5 <= 1 || *ffbOffset5 >= 254);

	(*ffbOffset & 0x01) ? *(BYTE*)(imageBase + 0x2B8108) = 0x01 : *(BYTE*)(imageBase + 0x2B8108) = 0x00; // P1 Trigger
	(*ffbOffset & 0x04) ? *(BYTE*)(imageBase + 0x2B8112) = 0x01 : *(BYTE*)(imageBase + 0x2B8112) = 0x00; // P2 Trigger

	*(WORD*)(imageBase + 0x2B8104) = (*ffbOffset2 / 255.0) * 16384; // P1 X Axis
	*(WORD*)(imageBase + 0x2B8106) = (*ffbOffset3 / 255.0) * 16384; // P1 Y Axis

	*(WORD*)(imageBase + 0x2B810E) = (*ffbOffset4 / 255.0) * 16384; // P2 X Axis
	*(WORD*)(imageBase + 0x2B8110) = (*ffbOffset5 / 255.0) * 16384; // P2 Y Axis
}