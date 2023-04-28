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
extern int* ffbOffset6;
extern int* ffbOffset7;
extern int* ffbOffset8;
extern int* ffbOffset9;

static bool Init;
static bool VolInit;
static bool VolUp;
static bool VolDown;
static bool GunTrig1P;
static bool GunTrig2P;
static bool GunTrig3P;
static bool GunTrig4P;

static int VolumeUpCounter;
static int VolumeDownCounter;

static bool CoinPressed;
static bool Coin;

static DWORD imageBase;
UINT8 GaiaAttack4Volume;

static char VolPercentChar[256];
static char INIChar[256];

BOOL(__stdcall* GaiaAttack4GetKeyboardStateOri)(PBYTE lpKeyState);
BOOL WINAPI GaiaAttack4GetKeyboardStateHook(PBYTE lpKeyState)
{
	return 0;
}

HWND(WINAPI* GaiaAttack4CreateWindowExAOri)(DWORD dwExStyle, LPCSTR lpClassName, LPCSTR lpWindowName, DWORD dwStyle, int X, int Y, int nWidth, int nHeight, HWND hWndParent, HMENU hMenu, HINSTANCE hInstance, LPVOID lpParam);
HWND WINAPI GaiaAttack4CreateWindowExAHook(DWORD dwExStyle, LPCSTR lpClassName, LPCSTR lpWindowName, DWORD dwStyle, int X, int Y, int nWidth, int nHeight, HWND hWndParent, HMENU hMenu, HINSTANCE hInstance, LPVOID lpParam)
{
	DWORD windowStyle = WS_POPUP | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_MAXIMIZEBOX;

	if (strcmp(lpWindowName, "CriDSoundOutput") == 0)
		dwStyle = windowStyle;
	else
		lpWindowName = "OpenParrot - Gaia Attack 4";

	return GaiaAttack4CreateWindowExAOri(dwExStyle, lpClassName, lpWindowName, dwStyle, 0, 0, nWidth, nHeight, hWndParent, hMenu, hInstance, lpParam);
}

static void WriteVol()
{
	sprintf_s(INIChar, "%d", GaiaAttack4Volume);
	WritePrivateProfileStringA("Settings", "Volume", INIChar, ".\\OpenParrot\\Settings.ini");
}

void GaiaAttack4Inputs(Helpers* helpers)
{
	if (!Init)
	{
		Init = true;
		imageBase = (DWORD)GetModuleHandleA(0);
	}

	*(BYTE*)(imageBase + 0x32F068) = 0x02; // Gun Board Connected
	*(BYTE*)(imageBase + 0xB3B820) = 0x04; // Set 4 Players

	DWORD VolBase = helpers->ReadInt32(0x1FE720C, true);
	UINT8 VolRead = helpers->ReadByte(VolBase + 0x21, false);

	if (!VolInit)
	{
		if (VolBase)
		{
			VolInit = true;
			helpers->WriteByte(VolBase + 0x21, GaiaAttack4Volume, false);
		}
	}

	UINT8 CoinButtonPressed = helpers->ReadByte(0x1FE7351, true);

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
			*(BYTE*)(imageBase + 0x1FE7351) = 0x08;
		}
	}

	if (*ffbOffset & 0x10)
	{
		++VolumeUpCounter;

		if (VolumeUpCounter == 2)
		{
			VolumeUpCounter = 0;

			if (VolRead < 0xFF)
				++GaiaAttack4Volume;

			WriteVol();

			if (VolBase)
				helpers->WriteByte(VolBase + 0x21, GaiaAttack4Volume, false);
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

		if (VolumeDownCounter == 2)
		{
			VolumeDownCounter = 0;

			if (VolRead > 0x00)
				--GaiaAttack4Volume;

			WriteVol();

			if (VolBase)
				helpers->WriteByte(VolBase + 0x21, GaiaAttack4Volume, false);
		}
	}
	else
	{
		if (VolumeDownCounter)
			VolumeDownCounter = 0;
	}

	bool P1SceenOut = (*ffbOffset2 <= 1 || *ffbOffset2 >= 254 || *ffbOffset3 <= 1 || *ffbOffset3 >= 254);
	bool P2SceenOut = (*ffbOffset4 <= 1 || *ffbOffset4 >= 254 || *ffbOffset5 <= 1 || *ffbOffset5 >= 254);
	bool P3SceenOut = (*ffbOffset6 <= 1 || *ffbOffset6 >= 254 || *ffbOffset7 <= 1 || *ffbOffset7 >= 254);
	bool P4SceenOut = (*ffbOffset8 <= 1 || *ffbOffset8 >= 254 || *ffbOffset9 <= 1 || *ffbOffset9 >= 254);

	(*ffbOffset & 0x01) ? *(BYTE*)(imageBase + 0xB3B890) = 0x01 : *(BYTE*)(imageBase + 0xB3B890) = 0x00; // P1 Trigger
	(*ffbOffset & 0x04) ? *(BYTE*)(imageBase + 0xB3B894) = 0x01 : *(BYTE*)(imageBase + 0xB3B894) = 0x00; // P2 Trigger
	(*ffbOffset & 0x40) ? *(BYTE*)(imageBase + 0xB3B898) = 0x01 : *(BYTE*)(imageBase + 0xB3B898) = 0x00; // P3 Trigger
	(*ffbOffset & 0x80) ? *(BYTE*)(imageBase + 0xB3B89C) = 0x01 : *(BYTE*)(imageBase + 0xB3B89C) = 0x00; // P4 Trigger

	(*ffbOffset & 0x01) ? *(BYTE*)(imageBase + 0xB3B838) = 0x01 : *(BYTE*)(imageBase + 0xB3B838) = 0x00; // P1 Auto Fire
	(*ffbOffset & 0x04) ? *(BYTE*)(imageBase + 0xB3B842) = 0x01 : *(BYTE*)(imageBase + 0xB3B842) = 0x00; // P2 Auto Fire
	(*ffbOffset & 0x40) ? *(BYTE*)(imageBase + 0xB3B84C) = 0x01 : *(BYTE*)(imageBase + 0xB3B84C) = 0x00; // P3 Auto Fire
	(*ffbOffset & 0x80) ? *(BYTE*)(imageBase + 0xB3B856) = 0x01 : *(BYTE*)(imageBase + 0xB3B856) = 0x00; // P4 Auto Fire

	P1SceenOut ? *(BYTE*)(imageBase + 0xB3B830) = 0x01 : *(BYTE*)(imageBase + 0xB3B830) = 0x00; // P1 Offscreen
	P2SceenOut ? *(BYTE*)(imageBase + 0xB3B83A) = 0x01 : *(BYTE*)(imageBase + 0xB3B83A) = 0x00; // P2 Offscreen
	P3SceenOut ? *(BYTE*)(imageBase + 0xB3B844) = 0x01 : *(BYTE*)(imageBase + 0xB3B844) = 0x00; // P3 Offscreen
	P4SceenOut ? *(BYTE*)(imageBase + 0xB3B84E) = 0x01 : *(BYTE*)(imageBase + 0xB3B84E) = 0x00; // P4 Offscreen

	if (*ffbOffset & 0x01)
	{
		if (!GunTrig1P)
		{
			GunTrig1P = true;
			*(BYTE*)(imageBase + 0xB3B880) = 0x01;
		}
		else
			*(BYTE*)(imageBase + 0xB3B880) = 0x00;
	}
	else
	{
		if (GunTrig1P)
		{
			GunTrig1P = false;
			*(BYTE*)(imageBase + 0xB3B880) = 0x00;
		}
	}

	if (*ffbOffset & 0x04)
	{
		if (!GunTrig2P)
		{
			GunTrig2P = true;
			*(BYTE*)(imageBase + 0xB3B884) = 0x01;
		}
		else
			*(BYTE*)(imageBase + 0xB3B884) = 0x00;
	}
	else
	{
		if (GunTrig2P)
		{
			GunTrig2P = false;
			*(BYTE*)(imageBase + 0xB3B884) = 0x00;
		}
	}

	if (*ffbOffset & 0x40)
	{
		if (!GunTrig3P)
		{
			GunTrig3P = true;
			*(BYTE*)(imageBase + 0xB3B888) = 0x01;
		}
		else
			*(BYTE*)(imageBase + 0xB3B888) = 0x00;
	}
	else
	{
		if (GunTrig3P)
		{
			GunTrig3P = false;
			*(BYTE*)(imageBase + 0xB3B888) = 0x00;
		}
	}

	if (*ffbOffset & 0x80)
	{
		if (!GunTrig4P)
		{
			GunTrig4P = true;
			*(BYTE*)(imageBase + 0xB3B88C) = 0x01;
		}
		else
			*(BYTE*)(imageBase + 0xB3B88C) = 0x00;
	}
	else
	{
		if (GunTrig4P)
		{
			GunTrig4P = false;
			*(BYTE*)(imageBase + 0xB3B88C) = 0x00;
		}
	}

	*(WORD*)(imageBase + 0xB3B834) = (*ffbOffset2 / 255.0) * 16384; // P1 Raw X Axis
	*(WORD*)(imageBase + 0xB3B950) = (*ffbOffset2 / 255.0) * 16384; // P1 X Axis
	*(WORD*)(imageBase + 0xB3B836) = (*ffbOffset3 / 255.0) * 16384; // P1 Raw Y Axis
	*(WORD*)(imageBase + 0xB3B960) = (*ffbOffset3 / 255.0) * 16384; // P1 Y Axis

	*(WORD*)(imageBase + 0xB3B83E) = (*ffbOffset4 / 255.0) * 16384; // P2 Raw X Axis
	*(WORD*)(imageBase + 0xB3B954) = (*ffbOffset4 / 255.0) * 16384; // P2 X Axis
	*(WORD*)(imageBase + 0xB3B840) = (*ffbOffset5 / 255.0) * 16384; // P2 Raw Y Axis
	*(WORD*)(imageBase + 0xB3B964) = (*ffbOffset5 / 255.0) * 16384; // P2 Y Axis

	*(WORD*)(imageBase + 0xB3B848) = (*ffbOffset6 / 255.0) * 16384; // P3 Raw X Axis
	*(WORD*)(imageBase + 0xB3B958) = (*ffbOffset6 / 255.0) * 16384; // P3 X Axis
	*(WORD*)(imageBase + 0xB3B84A) = (*ffbOffset7 / 255.0) * 16384; // P3 Raw Y Axis
	*(WORD*)(imageBase + 0xB3B968) = (*ffbOffset7 / 255.0) * 16384; // P3 Y Axis

	*(WORD*)(imageBase + 0xB3B852) = (*ffbOffset8 / 255.0) * 16384; // P4 Raw X Axis
	*(WORD*)(imageBase + 0xB3B95C) = (*ffbOffset8 / 255.0) * 16384; // P4 X Axis
	*(WORD*)(imageBase + 0xB3B854) = (*ffbOffset9 / 255.0) * 16384; // P4 Raw Y Axis
	*(WORD*)(imageBase + 0xB3B96C) = (*ffbOffset9 / 255.0) * 16384; // P4 Y Axis
}