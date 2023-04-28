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
UINT8 HauntedMuseum2Volume;

BOOL(__stdcall* HauntedMuseum2GetKeyboardStateOri)(PBYTE lpKeyState);
BOOL WINAPI HauntedMuseum2GetKeyboardStateHook(PBYTE lpKeyState)
{
	return 0;
}

HWND(WINAPI* HauntedMuseum2CreateWindowExAOri)(DWORD dwExStyle, LPCSTR lpClassName, LPCSTR lpWindowName, DWORD dwStyle, int X, int Y, int nWidth, int nHeight, HWND hWndParent, HMENU hMenu, HINSTANCE hInstance, LPVOID lpParam);
HWND WINAPI HauntedMuseum2CreateWindowExAHook(DWORD dwExStyle, LPCSTR lpClassName, LPCSTR lpWindowName, DWORD dwStyle, int X, int Y, int nWidth, int nHeight, HWND hWndParent, HMENU hMenu, HINSTANCE hInstance, LPVOID lpParam)
{
	DWORD windowStyle = WS_POPUP | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_MAXIMIZEBOX;

	if (strcmp(lpWindowName, "CriDSoundOutput") == 0)
		dwStyle = windowStyle;
	else
		lpWindowName = "OpenParrot - Haunted Museum II";

	return HauntedMuseum2CreateWindowExAOri(dwExStyle, lpClassName, lpWindowName, dwStyle, 0, 0, nWidth, nHeight, hWndParent, hMenu, hInstance, lpParam);
}

static void WriteVol()
{
	sprintf_s(INIChar, "%d", HauntedMuseum2Volume);
	WritePrivateProfileStringA("Settings", "Volume", INIChar, ".\\OpenParrot\\Settings.ini");
}

void HauntedMuseum2Inputs(Helpers* helpers)
{
	if (!Init)
	{
		Init = true;
		imageBase = (DWORD)GetModuleHandleA(0);
	}

	if (GameDetect::currentGame == GameID::HauntedMuseum2100)
	{
		*(BYTE*)(imageBase + 0x3BB448) = 0x02; // Gun Board Connected

		DWORD VolBase = helpers->ReadInt32(0x2B140D0, true);
		UINT8 VolRead = helpers->ReadByte(VolBase + 0x25, false);

		if (!VolInit)
		{
			if (VolBase)
			{
				VolInit = true;
				helpers->WriteByte(VolBase + 0x25, HauntedMuseum2Volume, false);
			}
		}

		UINT8 CoinButtonPressed = helpers->ReadByte(0x2B142B1, true);

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
				*(BYTE*)(imageBase + 0x2B142B1) = 0x08;
			}
		}

		if (*ffbOffset & 0x10)
		{
			++VolumeUpCounter;

			if (VolumeUpCounter == 2)
			{
				VolumeUpCounter = 0;

				if (VolRead < 0xFF)
					++HauntedMuseum2Volume;

				WriteVol();

				if (VolBase)
					helpers->WriteByte(VolBase + 0x25, HauntedMuseum2Volume, false);
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
					--HauntedMuseum2Volume;

				WriteVol();

				if (VolBase)
					helpers->WriteByte(VolBase + 0x25, HauntedMuseum2Volume, false);
			}
		}
		else
		{
			if (VolumeDownCounter)
				VolumeDownCounter = 0;
		}

		bool P1SceenOut = (*ffbOffset2 <= 1 || *ffbOffset2 >= 254 || *ffbOffset3 <= 1 || *ffbOffset3 >= 254);
		bool P2SceenOut = (*ffbOffset4 <= 1 || *ffbOffset4 >= 254 || *ffbOffset5 <= 1 || *ffbOffset5 >= 254);

		P1SceenOut ? *(BYTE*)(imageBase + 0xC4E4F8) = 0x01 : *(BYTE*)(imageBase + 0xC4E4F8) = 0x00; // P1 Offscreen
		P2SceenOut ? *(BYTE*)(imageBase + 0xC4E502) = 0x01 : *(BYTE*)(imageBase + 0xC4E502) = 0x00; // P2 Offscreen

		(*ffbOffset & 0x01) ? *(BYTE*)(imageBase + 0xC4E580) = 0x01 : *(BYTE*)(imageBase + 0xC4E580) = 0x00; // P1 Trigger
		(*ffbOffset & 0x04) ? *(BYTE*)(imageBase + 0xC4E584) = 0x01 : *(BYTE*)(imageBase + 0xC4E584) = 0x00; // P2 Trigger

		if (*ffbOffset & 0x01)
		{
			if (!GunTrig1P)
			{
				GunTrig1P = true;
				*(BYTE*)(imageBase + 0xC4E570) = 0x01;
			}
			else
				*(BYTE*)(imageBase + 0xC4E570) = 0x00;
		}
		else
		{
			if (GunTrig1P)
			{
				GunTrig1P = false;
				*(BYTE*)(imageBase + 0xC4E570) = 0x00;
			}
		}

		if (*ffbOffset & 0x04)
		{
			if (!GunTrig2P)
			{
				GunTrig2P = true;
				*(BYTE*)(imageBase + 0xC4E574) = 0x01;
			}
			else
				*(BYTE*)(imageBase + 0xC4E574) = 0x00;
		}
		else
		{
			if (GunTrig2P)
			{
				GunTrig2P = false;
				*(BYTE*)(imageBase + 0xC4E574) = 0x00;
			}
		}

		*(WORD*)(imageBase + 0xC4E4FC) = (*ffbOffset2 / 255.0) * 16384; // P1 X Axis
		*(WORD*)(imageBase + 0xC4E4FE) = (*ffbOffset3 / 255.0) * 16384; // P1 Y Axis

		*(WORD*)(imageBase + 0xC4E506) = (*ffbOffset4 / 255.0) * 16384; // P2 X Axis
		*(WORD*)(imageBase + 0xC4E508) = (*ffbOffset5 / 255.0) * 16384; // P2 Y Axis
	}

	if (GameDetect::currentGame == GameID::HauntedMuseum2101J)
	{
		*(BYTE*)(imageBase + 0x32F048) = 0x02; // Gun Board Connected

		DWORD VolBase = helpers->ReadInt32(0x29AAFCC, true);
		UINT8 VolRead = helpers->ReadByte(VolBase + 0x25, false);

		if (!VolInit)
		{
			if (VolBase)
			{
				VolInit = true;
				helpers->WriteByte(VolBase + 0x25, HauntedMuseum2Volume, false);
			}
		}

		UINT8 CoinButtonPressed = helpers->ReadByte(0x29AB189, true);

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
				*(BYTE*)(imageBase + 0x29AB189) = 0x08;
			}
		}

		if (*ffbOffset & 0x10)
		{
			++VolumeUpCounter;

			if (VolumeUpCounter == 2)
			{
				VolumeUpCounter = 0;

				if (VolRead < 0xFF)
					++HauntedMuseum2Volume;

				WriteVol();

				if (VolBase)
					helpers->WriteByte(VolBase + 0x25, HauntedMuseum2Volume, false);
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
					--HauntedMuseum2Volume;

				WriteVol();

				if (VolBase)
					helpers->WriteByte(VolBase + 0x25, HauntedMuseum2Volume, false);
			}
		}
		else
		{
			if (VolumeDownCounter)
				VolumeDownCounter = 0;
		}

		bool P1SceenOut = (*ffbOffset2 <= 1 || *ffbOffset2 >= 254 || *ffbOffset3 <= 1 || *ffbOffset3 >= 254);
		bool P2SceenOut = (*ffbOffset4 <= 1 || *ffbOffset4 >= 254 || *ffbOffset5 <= 1 || *ffbOffset5 >= 254);

		P1SceenOut ? *(BYTE*)(imageBase + 0xAE5430) = 0x01 : *(BYTE*)(imageBase + 0xAE5430) = 0x00; // P1 Offscreen
		P2SceenOut ? *(BYTE*)(imageBase + 0xAE543A) = 0x01 : *(BYTE*)(imageBase + 0xAE543A) = 0x00; // P2 Offscreen

		(*ffbOffset & 0x01) ? *(BYTE*)(imageBase + 0xAE54B8) = 0x01 : *(BYTE*)(imageBase + 0xAE54B8) = 0x00; // P1 Trigger
		(*ffbOffset & 0x04) ? *(BYTE*)(imageBase + 0xAE54BC) = 0x01 : *(BYTE*)(imageBase + 0xAE54BC) = 0x00; // P2 Trigger

		if (*ffbOffset & 0x01)
		{
			if (!GunTrig1P)
			{
				GunTrig1P = true;
				*(BYTE*)(imageBase + 0xAE54A8) = 0x01;
			}
			else
				*(BYTE*)(imageBase + 0xAE54A8) = 0x00;
		}
		else
		{
			if (GunTrig1P)
			{
				GunTrig1P = false;
				*(BYTE*)(imageBase + 0xAE54A8) = 0x00;
			}
		}

		if (*ffbOffset & 0x04)
		{
			if (!GunTrig2P)
			{
				GunTrig2P = true;
				*(BYTE*)(imageBase + 0xAE54AC) = 0x01;
			}
			else
				*(BYTE*)(imageBase + 0xAE54AC) = 0x00;
		}
		else
		{
			if (GunTrig2P)
			{
				GunTrig2P = false;
				*(BYTE*)(imageBase + 0xAE54AC) = 0x00;
			}
		}

		*(WORD*)(imageBase + 0xAE5434) = (*ffbOffset2 / 255.0) * 16384; // P1 X Axis
		*(WORD*)(imageBase + 0xAE5436) = (*ffbOffset3 / 255.0) * 16384; // P1 Y Axis

		*(WORD*)(imageBase + 0xAE543E) = (*ffbOffset4 / 255.0) * 16384; // P2 X Axis
		*(WORD*)(imageBase + 0xAE5440) = (*ffbOffset5 / 255.0) * 16384; // P2 Y Axis
	}
}