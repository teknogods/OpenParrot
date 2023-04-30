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

extern HWND D3D9hWnd;

static bool Init;
static bool VolInit;
static bool GunTrig1P;
static bool GunTrig2P;
static bool VolUp;
static bool VolDown;
static bool CoinPressed;
static bool Coin;

static int VolumeUpCounter;
static int VolumeDownCounter;

static DWORD imageBase;

static char INIChar[256];
UINT8 HauntedMuseumVolume;

static const char* HauntedMuseumName = "OpenParrot - Haunted Museum";

BOOL(__stdcall* HauntedMuseumGetKeyboardStateOri)(PBYTE lpKeyState);
BOOL WINAPI HauntedMuseumGetKeyboardStateHook(PBYTE lpKeyState)
{
	return 0;
}

HWND(WINAPI* HauntedMuseumCreateWindowExAOri)(DWORD dwExStyle, LPCSTR lpClassName, LPCSTR lpWindowName, DWORD dwStyle, int X, int Y, int nWidth, int nHeight, HWND hWndParent, HMENU hMenu, HINSTANCE hInstance, LPVOID lpParam);
HWND WINAPI HauntedMuseumCreateWindowExAHook(DWORD dwExStyle, LPCSTR lpClassName, LPCSTR lpWindowName, DWORD dwStyle, int X, int Y, int nWidth, int nHeight, HWND hWndParent, HMENU hMenu, HINSTANCE hInstance, LPVOID lpParam)
{
	DWORD windowStyle = WS_POPUP | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_MAXIMIZEBOX;

		if (strcmp(lpWindowName, "CriDSoundOutput") == 0)
			dwStyle = windowStyle;
		else
			lpWindowName = HauntedMuseumName;

	return HauntedMuseumCreateWindowExAOri(dwExStyle, lpClassName, lpWindowName, dwStyle, 0, 0, nWidth, nHeight, hWndParent, hMenu, hInstance, lpParam);
}

static void WriteVol()
{
	sprintf_s(INIChar, "%d", HauntedMuseumVolume);
	WritePrivateProfileStringA("Settings", "Volume", INIChar, ".\\OpenParrot\\Settings.ini");
}

void HauntedMuseumInputs(Helpers* helpers)
{
	if (!Init)
	{
		Init = true;
		imageBase = (DWORD)GetModuleHandleA(0);	
	}

	if (!D3D9hWnd)
		D3D9hWnd = FindWindowA(0, HauntedMuseumName);

	*(DWORD*)(imageBase + 0x32797C) = 0x02; // Gun Board Connected
	*(DWORD*)(imageBase + 0x32796C) = 0xEEEEEEEE;

	DWORD CoinLimit = helpers->ReadByte(0x98B3E8, true);

	if (CoinLimit == 9)
		helpers->WriteByte(0x98B3E8, 0, true);

	DWORD VolBase = helpers->ReadInt32(0x1E2702C, true);
	UINT8 VolRead = helpers->ReadByte(VolBase + 0x21, false);

	if (!VolInit)
	{
		if (VolBase)
		{
			VolInit = true;
			helpers->WriteByte(VolBase + 0x21, HauntedMuseumVolume, false);
		}
	}

	UINT8 CoinButtonPressed = helpers->ReadByte(0x1E27169, true);

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
			*(BYTE*)(imageBase + 0x1E27169) = 0x08;
		}
	}

	if (*ffbOffset & 0x10)
	{
		++VolumeUpCounter;

		if (VolumeUpCounter == 2)
		{
			VolumeUpCounter = 0;

			if (VolRead < 0xFF)
				++HauntedMuseumVolume;

			WriteVol();

			if (VolBase)
				helpers->WriteByte(VolBase + 0x21, HauntedMuseumVolume, false);
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
				--HauntedMuseumVolume;

			WriteVol();

			if (VolBase)
				helpers->WriteByte(VolBase + 0x21, HauntedMuseumVolume, false);
		}
	}
	else
	{
		if (VolumeDownCounter)
			VolumeDownCounter = 0;
	}

	bool P1SceenOut = (*ffbOffset2 <= 1 || *ffbOffset2 >= 254 || *ffbOffset3 <= 1 || *ffbOffset3 >= 254);
	bool P2SceenOut = (*ffbOffset4 <= 1 || *ffbOffset4 >= 254 || *ffbOffset5 <= 1 || *ffbOffset5 >= 254);

	P1SceenOut ? *(BYTE*)(imageBase + 0x98B416) = 0x01 : *(BYTE*)(imageBase + 0x98B416) = 0x00; // P1 Offscreen
	P2SceenOut ? *(BYTE*)(imageBase + 0x98B41E) = 0x01 : *(BYTE*)(imageBase + 0x98B41E) = 0x00; // P2 Offscreen

	(*ffbOffset & 0x01) ? *(BYTE*)(imageBase + 0x98B444) = 0x01 : *(BYTE*)(imageBase + 0x98B444) = 0x00; // P1 Trigger
	(*ffbOffset & 0x04) ? *(BYTE*)(imageBase + 0x98B448) = 0x01 : *(BYTE*)(imageBase + 0x98B448) = 0x00; // P2 Trigger

	(*ffbOffset & 0x01) ? *(BYTE*)(imageBase + 0x98B41C) = 0x01 : *(BYTE*)(imageBase + 0x98B41C) = 0x00; // P1 Auto Fire
	(*ffbOffset & 0x04) ? *(BYTE*)(imageBase + 0x98B426) = 0x01 : *(BYTE*)(imageBase + 0x98B426) = 0x00; // P2 Auto Fire

	if (*ffbOffset & 0x01)
	{
		if (!GunTrig1P)
		{
			GunTrig1P = true;
			*(BYTE*)(imageBase + 0x98B43C) = 0x01;
		}
		else
			*(BYTE*)(imageBase + 0x98B43C) = 0x00;
	}
	else
	{
		if (GunTrig1P)
		{
			GunTrig1P = false;
			*(BYTE*)(imageBase + 0x98B43C) = 0x00;
		}
	}

	if (*ffbOffset & 0x04)
	{
		if (!GunTrig2P)
		{
			GunTrig2P = true;
			*(BYTE*)(imageBase + 0x98B440) = 0x01;
		}
		else
			*(BYTE*)(imageBase + 0x98B440) = 0x00;
	}
	else
	{
		if (GunTrig2P)
		{
			GunTrig2P = false;
			*(BYTE*)(imageBase + 0x98B440) = 0x00;
		}
	}

	*(WORD*)(imageBase + 0x98B418) = (*ffbOffset2 / 255.0) * 16384; // P1 X Axis
	*(WORD*)(imageBase + 0x98B41A) = (*ffbOffset3 / 255.0) * 16384; // P1 Y Axis

	*(WORD*)(imageBase + 0x98B422) = (*ffbOffset4 / 255.0) * 16384; // P2 X Axis
	*(WORD*)(imageBase + 0x98B424) = (*ffbOffset5 / 255.0) * 16384; // P2 Y Axis
}