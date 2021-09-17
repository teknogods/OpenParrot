#if __has_include(<atlstr.h>)
#include <StdInc.h>
#include "Utility/InitFunction.h"
#include "Functions/Global.h"
#include "Utility\Hooking.Patterns.h"
#include <Xinput.h>
#include <math.h>
#include <dinput.h>
#include "Functions/GlobalRegHooks.h"
#include "Utility\Hooking.Patterns.h"
#include <atlstr.h>
#include <windows.h>
#include <string>
#include <iostream>
#include <shlobj.h>
#include <fstream>
#include "d3d9.h"
#include "Utility/Helper.h"
#include "Mmsystem.h"

#if _M_IX86
#pragma optimize("", off)
#pragma comment(lib, "Ws2_32.lib")
#pragma comment( lib, "winmm.lib" )

typedef unsigned int U32;
typedef unsigned char U8;

DWORD BaseAddress9 = 0x00400000;
int horizontal9 = 0;
int vertical9 = 0;
HWND hWndRT9 = 0;

static bool button1pressed = false;
static bool button2pressed = false;
static bool button3pressed = false;
static bool button4pressed = false;
static bool STARTpressed = false;
static bool TESTpressed = false;
static bool SERVICEpressed = false;
static bool previousVolMin = false;
static bool previousVolMax = false;
static bool MenuHack = false;
static bool RiptideHack = false;
static bool MenuHackStopWriting = false;
static bool CoinPressed = false;
static bool NameEntryHack;
static BYTE GameState;
extern bool PauseEnabled;

// controls
extern int* ffbOffset;
extern int* ffbOffset2;
extern int* ffbOffset3;
extern int* ffbOffset4;

BOOL(__stdcall* original_CreateWindowExW9)(DWORD dwExStyle, LPCWSTR lpClassName, LPCWSTR lpWindowName, DWORD dwStyle, int X, int Y, int nWidth, int nHeight, HWND hWndParent, HMENU hMenu, HINSTANCE hInstance, LPVOID lpParam);
BOOL(__stdcall* original_CreateWindowExA9)(DWORD dwExStyle, LPCSTR lpClassName, LPCSTR lpWindowName, DWORD dwStyle, int X, int Y, int nWidth, int nHeight, HWND hWndParent, HMENU hMenu, HINSTANCE hInstance, LPVOID lpParam);

void __stdcall ServiceControlsPatch()
{
	// TEST
	if ((GetAsyncKeyState(VK_INSERT) & 0x8000) || (*ffbOffset & 0x400))
	{
		if (TESTpressed == false)
		{
			injector::WriteMemory<BYTE>((0x98C380), 0x01, true);
			TESTpressed = true;
		}
	}
	else
	{
		if (TESTpressed == true)
		{
			injector::WriteMemory<BYTE>((0x98C380), 0x00, true);
			TESTpressed = false;
		}
	}
	// SERVICE
	if ((GetAsyncKeyState(VK_DELETE) & 0x8000) || (*ffbOffset & 0x800))
	{
		if (SERVICEpressed == false)
		{
			injector::WriteMemory<BYTE>((0x98C384), 0x01, true);
			SERVICEpressed = true;
		}
	}
	else
	{
		if (SERVICEpressed == true)
		{
			injector::WriteMemory<BYTE>((0x98C384), 0x00, true);
			SERVICEpressed = false;
		}
	}
	// VOL+
	if (*ffbOffset & 0x1000)
	{
		if (previousVolMax == false)
		{
			injector::WriteMemory<BYTE>((0x98C388), 0x01, true);
			previousVolMax = true;
		}
	}
	else
	{
		if (previousVolMax == true)
		{
			injector::WriteMemory<BYTE>((0x98C388), 0x00, true);
			previousVolMax = false;
		}
	}
	// VOL-
	if (*ffbOffset & 0x2000)
	{
		if (previousVolMin == false)
		{
			injector::WriteMemory<BYTE>((0x98C38C), 0x01, true);
			previousVolMin = true;
		}
	}
	else
	{
		if (previousVolMin == true)
		{
			injector::WriteMemory<BYTE>((0x98C38C), 0x00, true);
			previousVolMin = false;
		}
	}

	return;
}

static char CoinDigitChar[256];
static DWORD CoinDigitAddress;
static char Digits[256];
static int Digit1CoinValueinHex;
static int Digit2CoinValueinHex;
static bool SoundFail;

static void CoinInput(Helpers* helpers)
{
	UINT8 CoinValue = helpers->ReadByte(0x4947AC, true);

	INT_PTR CoinDigitBase = helpers->ReadIntPtr(0x494064, true);
	INT_PTR CoinDigitA = helpers->ReadIntPtr(CoinDigitBase + 0x10, false);
	INT_PTR CoinDigitB = helpers->ReadIntPtr(CoinDigitA + 0x600, false);
	INT_PTR CoinDigitC = helpers->ReadIntPtr(CoinDigitB + 0x1A0, false);

	if (*ffbOffset & 0x04)
	{
		if (!CoinPressed)
		{
			CoinPressed = true;

			if (!SoundFail)
			{
				LPCWSTR Stop = L"stop .\\Coin\\Coin.mp3";
				int Stopping = mciSendString(Stop, NULL, 0, 0);

				LPCWSTR Play = L"play .\\Coin\\Coin.mp3 from 0";
				int Playing = mciSendString(Play, NULL, 0, 0);

				if (Playing != 0 && Stopping != 0)
				{
					SoundFail = true;
				}
			}

			helpers->WriteByte(0x8947AC, ++CoinValue, false);
		}
	}
	else
	{
		if (CoinPressed)
		{
			CoinPressed = false;
		}
	}

	if (CoinDigitC != NULL)
	{
		memset(CoinDigitChar, 0, 256);
		sprintf(CoinDigitChar, "%p", CoinDigitC + 0xE70);
		CoinDigitAddress = strtoul(CoinDigitChar, NULL, 16);

		helpers->WriteIntPtr(0x4947B0, 0x0000, true);

		if (CoinValue > 99)
			helpers->WriteByte(0x8947AC, 0x63, false);

		if (CoinValue >= 0x0A)
		{
			injector::WriteMemoryRaw(CoinDigitAddress + 0x03, "\x00\x20\x00\x20\x00\x43\x00\x52\x00\x45\x00\x44\x00\x49\x00\x54\x00\x53\x00\x00\x00\x00\x00\x00\x00\x00", 27, false);

			if (CoinValue >= 99)
			{
				itoa(0x39, Digits, 16);
				Digit1CoinValueinHex = 0x39;
			}
			else if (CoinValue >= 90)
			{
				itoa(CoinValue - 42, Digits, 16);
				Digit1CoinValueinHex = 0x39;
			}
			else if (CoinValue >= 80)
			{
				itoa(CoinValue - 32, Digits, 16);
				Digit1CoinValueinHex = 0x38;
			}
			else if (CoinValue >= 70)
			{
				itoa(CoinValue - 22, Digits, 16);
				Digit1CoinValueinHex = 0x37;
			}
			else if (CoinValue >= 60)
			{
				itoa(CoinValue - 12, Digits, 16);
				Digit1CoinValueinHex = 0x36;
			}
			else if (CoinValue >= 50)
			{
				itoa(CoinValue - 2, Digits, 16);
				Digit1CoinValueinHex = 0x35;
			}
			else if (CoinValue >= 40)
			{
				itoa(CoinValue + 8, Digits, 16);
				Digit1CoinValueinHex = 0x34;
			}
			else if (CoinValue >= 30)
			{
				itoa(CoinValue + 18, Digits, 16);
				Digit1CoinValueinHex = 0x33;
			}
			else if (CoinValue >= 20)
			{
				itoa(CoinValue + 28, Digits, 16);
				Digit1CoinValueinHex = 0x32;
			}
			else
			{
				itoa(CoinValue + 38, Digits, 16);
				Digit1CoinValueinHex = 0x31;
			}

			Digit2CoinValueinHex = strtoul(Digits, NULL, 16);

			helpers->WriteByte(CoinDigitC + 0xE70, Digit1CoinValueinHex, false);
			helpers->WriteByte(CoinDigitC + 0xE71, 0x00, false);
			helpers->WriteByte(CoinDigitC + 0xE72, Digit2CoinValueinHex, false);
		}
		else
		{
			injector::WriteMemoryRaw(CoinDigitAddress + 0x01, "\x00\x20\x00\x20\x00\x43\x00\x52\x00\x45\x00\x44\x00\x49\x00\x54\x00\x53\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00", 29, false);

			itoa(48 + CoinValue, Digits, 16);

			Digit1CoinValueinHex = strtoul(Digits, NULL, 16);

			helpers->WriteByte(CoinDigitC + 0xE70, Digit1CoinValueinHex, false);
		}
	}
}

static void NameScreenInput(Helpers* helpers) //"Fix" stupid name not allowing all characters to be selected
{
	if (!NameEntryHack)
	{
		NameEntryHack = true;
		injector::MakeNOP(0x7CC26 + BaseAddress9, 6);
		injector::MakeNOP(0x7CC2C + BaseAddress9, 6);
		injector::MakeNOP(0x76864 + BaseAddress9, 3);
		injector::MakeNOP(0x76867 + BaseAddress9, 5);
	}

	INT_PTR NameBase = helpers->ReadIntPtr(0x5AE500, true);
	INT_PTR NameBaseOff1 = helpers->ReadIntPtr(NameBase + 0x10, false);
	UINT8 NameNum = helpers->ReadByte(NameBaseOff1 + 0x388, false);
	UINT8 ViewNum = helpers->ReadByte(0x57D618, true);

	DWORD LetterAddr{};

	helpers->WriteFloat32(0x4AD0FC, 0.0, true);

	switch (NameNum)
	{
	case 0:
		injector::WriteMemoryRaw((0x57057D + BaseAddress9), "\x00\x00\x00\x00\x00\x00", 6, true);
		LetterAddr = 0x57057C;
		break;
	case 1:
		LetterAddr = 0x57057D;
		break;
	case 2:
		LetterAddr = 0x57057E;
		break;
	case 3:
		LetterAddr = 0x57057F;
		break;
	case 4:
		LetterAddr = 0x570580;
		break;
	case 5:
		LetterAddr = 0x570581;
		break;
	case 6:
		helpers->WriteFloat32(0x4AD0FC, 1.0, true);
		LetterAddr = 0x570582;
		break;
	}

	if (*ffbOffset2 >= 0xF3)
	{
		helpers->WriteFloat32(0x4AD0FC, 1.0, true);
		switch (ViewNum)
		{
		case 0:
			*(BYTE*)(0x5705A0 + BaseAddress9) = 0x10;
			*(BYTE*)(0x5705A4 + BaseAddress9) = 0x1B;
			*(BYTE*)(LetterAddr + BaseAddress9) = 0x10;
			break;
		case 1:
			*(BYTE*)(0x5705A0 + BaseAddress9) = 0x10;
			*(BYTE*)(0x5705A4 + BaseAddress9) = 0x1A;
			*(BYTE*)(LetterAddr + BaseAddress9) = 0x10;
			break;
		}
	}
	else if (*ffbOffset2 >= 0xEA)
	{
		switch (ViewNum)
		{
		case 0:
			*(BYTE*)(0x5705A0 + BaseAddress9) = 0x5A;
			*(BYTE*)(0x5705A4 + BaseAddress9) = 0x1A;
			*(BYTE*)(LetterAddr + BaseAddress9) = 0x5A;
			break;
		case 1:
			helpers->WriteFloat32(0x4AD0FC, 1.0, true);
			*(BYTE*)(0x5705A0 + BaseAddress9) = 0x10;
			*(BYTE*)(0x5705A4 + BaseAddress9) = 0x1A;
			*(BYTE*)(LetterAddr + BaseAddress9) = 0x10;
			break;
		}
	}
	else if (*ffbOffset2 >= 0xE1)
	{
		*(BYTE*)(0x5705A4 + BaseAddress9) = 0x19;
		switch (ViewNum)
		{
		case 0:
			*(BYTE*)(0x5705A0 + BaseAddress9) = 0x59;
			*(BYTE*)(LetterAddr + BaseAddress9) = 0x59;
			break;
		case 1:
			*(BYTE*)(0x5705A0 + BaseAddress9) = 0x7D;
			*(BYTE*)(LetterAddr + BaseAddress9) = 0x7D;
			break;
		}
	}
	else if (*ffbOffset2 >= 0xD8)
	{
		*(BYTE*)(0x5705A4 + BaseAddress9) = 0x18;
		switch (ViewNum)
		{
		case 0:
			*(BYTE*)(0x5705A0 + BaseAddress9) = 0x58;
			*(BYTE*)(LetterAddr + BaseAddress9) = 0x58;
			break;
		case 1:
			*(BYTE*)(0x5705A0 + BaseAddress9) = 0x7B;
			*(BYTE*)(LetterAddr + BaseAddress9) = 0x7B;
			break;
		}
	}
	else if (*ffbOffset2 >= 0xCF)
	{
		*(BYTE*)(0x5705A4 + BaseAddress9) = 0x17;
		switch (ViewNum)
		{
		case 0:
			*(BYTE*)(0x5705A0 + BaseAddress9) = 0x57;
			*(BYTE*)(LetterAddr + BaseAddress9) = 0x57;
			break;
		case 1:
			*(BYTE*)(0x5705A0 + BaseAddress9) = 0x5E;
			*(BYTE*)(LetterAddr + BaseAddress9) = 0x5E;
			break;
		}
	}
	else if (*ffbOffset2 >= 0xC6)
	{
		*(BYTE*)(0x5705A4 + BaseAddress9) = 0x16;
		switch (ViewNum)
		{
		case 0:
			*(BYTE*)(0x5705A0 + BaseAddress9) = 0x56;
			*(BYTE*)(LetterAddr + BaseAddress9) = 0x56;
			break;
		case 1:
			*(BYTE*)(0x5705A0 + BaseAddress9) = 0x5D;
			*(BYTE*)(LetterAddr + BaseAddress9) = 0x5D;
			break;
		}
	}
	else if (*ffbOffset2 >= 0xBD)
	{
		*(BYTE*)(0x5705A4 + BaseAddress9) = 0x15;
		switch (ViewNum)
		{
		case 0:
			*(BYTE*)(0x5705A0 + BaseAddress9) = 0x55;
			*(BYTE*)(LetterAddr + BaseAddress9) = 0x55;
			break;
		case 1:
			*(BYTE*)(0x5705A0 + BaseAddress9) = 0x5B;
			*(BYTE*)(LetterAddr + BaseAddress9) = 0x5B;
			break;
		}
	}
	else if (*ffbOffset2 >= 0xB4)
	{
		*(BYTE*)(0x5705A4 + BaseAddress9) = 0x14;
		switch (ViewNum)
		{
		case 0:
			*(BYTE*)(0x5705A0 + BaseAddress9) = 0x54;
			*(BYTE*)(LetterAddr + BaseAddress9) = 0x54;
			break;
		case 1:
			*(BYTE*)(0x5705A0 + BaseAddress9) = 0x2F;
			*(BYTE*)(LetterAddr + BaseAddress9) = 0x2F;
			break;
		}
	}
	else if (*ffbOffset2 >= 0xAB)
	{
		*(BYTE*)(0x5705A4 + BaseAddress9) = 0x13;
		switch (ViewNum)
		{
		case 0:
			*(BYTE*)(0x5705A0 + BaseAddress9) = 0x53;
			*(BYTE*)(LetterAddr + BaseAddress9) = 0x53;
			break;
		case 1:
			*(BYTE*)(0x5705A0 + BaseAddress9) = 0x3E;
			*(BYTE*)(LetterAddr + BaseAddress9) = 0x3E;
			break;
		}
	}
	else if (*ffbOffset2 >= 0xA2)
	{
		*(BYTE*)(0x5705A4 + BaseAddress9) = 0x12;
		switch (ViewNum)
		{
		case 0:
			*(BYTE*)(0x5705A0 + BaseAddress9) = 0x52;
			*(BYTE*)(LetterAddr + BaseAddress9) = 0x52;
			break;
		case 1:
			*(BYTE*)(0x5705A0 + BaseAddress9) = 0x3C;
			*(BYTE*)(LetterAddr + BaseAddress9) = 0x3C;
			break;
		}
	}
	else if (*ffbOffset2 >= 0x99)
	{
		*(BYTE*)(0x5705A4 + BaseAddress9) = 0x11;
		switch (ViewNum)
		{
		case 0:
			*(BYTE*)(0x5705A0 + BaseAddress9) = 0x51;
			*(BYTE*)(LetterAddr + BaseAddress9) = 0x51;
			break;
		case 1:
			*(BYTE*)(0x5705A0 + BaseAddress9) = 0x40;
			*(BYTE*)(LetterAddr + BaseAddress9) = 0x40;
			break;
		}
	}
	else if (*ffbOffset2 >= 0x90)
	{
		*(BYTE*)(0x5705A4 + BaseAddress9) = 0x10;
		switch (ViewNum)
		{
		case 0:
			*(BYTE*)(0x5705A0 + BaseAddress9) = 0x50;
			*(BYTE*)(LetterAddr + BaseAddress9) = 0x50;
			break;
		case 1:
			*(BYTE*)(0x5705A0 + BaseAddress9) = 0x5F;
			*(BYTE*)(LetterAddr + BaseAddress9) = 0x5F;
			break;
		}
	}
	else if (*ffbOffset2 >= 0x87)
	{
		*(BYTE*)(0x5705A4 + BaseAddress9) = 0x0F;
		switch (ViewNum)
		{
		case 0:
			*(BYTE*)(0x5705A0 + BaseAddress9) = 0x4F;
			*(BYTE*)(LetterAddr + BaseAddress9) = 0x4F;
			break;
		case 1:
			*(BYTE*)(0x5705A0 + BaseAddress9) = 0x2D;
			*(BYTE*)(LetterAddr + BaseAddress9) = 0x2D;
			break;
		}
	}
	else if (*ffbOffset2 >= 0x7E)
	{
		*(BYTE*)(0x5705A4 + BaseAddress9) = 0x0E;
		switch (ViewNum)
		{
		case 0:
			*(BYTE*)(0x5705A0 + BaseAddress9) = 0x4E;
			*(BYTE*)(LetterAddr + BaseAddress9) = 0x4E;
			break;
		case 1:
			*(BYTE*)(0x5705A0 + BaseAddress9) = 0x21;
			*(BYTE*)(LetterAddr + BaseAddress9) = 0x21;
			break;
		}
	}
	else if (*ffbOffset2 >= 0x75)
	{
		*(BYTE*)(0x5705A4 + BaseAddress9) = 0x0D;
		switch (ViewNum)
		{
		case 0:
			*(BYTE*)(0x5705A0 + BaseAddress9) = 0x4D;
			*(BYTE*)(LetterAddr + BaseAddress9) = 0x4D;
			break;
		case 1:
			*(BYTE*)(0x5705A0 + BaseAddress9) = 0x3F;
			*(BYTE*)(LetterAddr + BaseAddress9) = 0x3F;
			break;
		}
	}
	else if (*ffbOffset2 >= 0x6C)
	{
		*(BYTE*)(0x5705A4 + BaseAddress9) = 0x0C;
		switch (ViewNum)
		{
		case 0:
			*(BYTE*)(0x5705A0 + BaseAddress9) = 0x4C;
			*(BYTE*)(LetterAddr + BaseAddress9) = 0x4C;
			break;
		case 1:
			*(BYTE*)(0x5705A0 + BaseAddress9) = 0x2E;
			*(BYTE*)(LetterAddr + BaseAddress9) = 0x2E;
			break;
		}
	}
	else if (*ffbOffset2 >= 0x63)
	{
		*(BYTE*)(0x5705A4 + BaseAddress9) = 0x0B;
		switch (ViewNum)
		{
		case 0:
			*(BYTE*)(0x5705A0 + BaseAddress9) = 0x4B;
			*(BYTE*)(LetterAddr + BaseAddress9) = 0x4B;
			break;
		case 1:
			*(BYTE*)(0x5705A0 + BaseAddress9) = 0x20;
			*(BYTE*)(LetterAddr + BaseAddress9) = 0x20;
			break;
		}
	}
	else if (*ffbOffset2 >= 0x5A)
	{
		*(BYTE*)(0x5705A4 + BaseAddress9) = 0x0A;
		switch (ViewNum)
		{
		case 0:
			*(BYTE*)(0x5705A0 + BaseAddress9) = 0x4A;
			*(BYTE*)(LetterAddr + BaseAddress9) = 0x4A;
			break;
		case 1:
			*(BYTE*)(0x5705A0 + BaseAddress9) = 0x39;
			*(BYTE*)(LetterAddr + BaseAddress9) = 0x39;
			break;
		}
	}
	else if (*ffbOffset2 >= 0x51)
	{
		*(BYTE*)(0x5705A4 + BaseAddress9) = 0x09;
		switch (ViewNum)
		{
		case 0:
			*(BYTE*)(0x5705A0 + BaseAddress9) = 0x49;
			*(BYTE*)(LetterAddr + BaseAddress9) = 0x49;
			break;
		case 1:
			*(BYTE*)(0x5705A0 + BaseAddress9) = 0x38;
			*(BYTE*)(LetterAddr + BaseAddress9) = 0x38;
			break;
		}
	}
	else if (*ffbOffset2 >= 0x48)
	{
		*(BYTE*)(0x5705A4 + BaseAddress9) = 0x08;
		switch (ViewNum)
		{
		case 0:
			*(BYTE*)(0x5705A0 + BaseAddress9) = 0x48;
			*(BYTE*)(LetterAddr + BaseAddress9) = 0x48;
			break;
		case 1:
			*(BYTE*)(0x5705A0 + BaseAddress9) = 0x37;
			*(BYTE*)(LetterAddr + BaseAddress9) = 0x37;
			break;
		}
	}
	else if (*ffbOffset2 >= 0x3F)
	{
		*(BYTE*)(0x5705A4 + BaseAddress9) = 0x07;
		switch (ViewNum)
		{
		case 0:
			*(BYTE*)(0x5705A0 + BaseAddress9) = 0x47;
			*(BYTE*)(LetterAddr + BaseAddress9) = 0x47;
			break;
		case 1:
			*(BYTE*)(0x5705A0 + BaseAddress9) = 0x36;
			*(BYTE*)(LetterAddr + BaseAddress9) = 0x36;
			break;
		}

	}
	else if (*ffbOffset2 >= 0x36)
	{
		*(BYTE*)(0x5705A4 + BaseAddress9) = 0x06;
		switch (ViewNum)
		{
		case 0:
			*(BYTE*)(0x5705A0 + BaseAddress9) = 0x46;
			*(BYTE*)(LetterAddr + BaseAddress9) = 0x46;
			break;
		case 1:
			*(BYTE*)(0x5705A0 + BaseAddress9) = 0x35;
			*(BYTE*)(LetterAddr + BaseAddress9) = 0x35;
			break;
		}
	}
	else if (*ffbOffset2 >= 0x2D)
	{
		*(BYTE*)(0x5705A4 + BaseAddress9) = 0x05;
		switch (ViewNum)
		{
		case 0:
			*(BYTE*)(0x5705A0 + BaseAddress9) = 0x45;
			*(BYTE*)(LetterAddr + BaseAddress9) = 0x45;
			break;
		case 1:
			*(BYTE*)(0x5705A0 + BaseAddress9) = 0x34;
			*(BYTE*)(LetterAddr + BaseAddress9) = 0x34;
			break;
		}
	}
	else if (*ffbOffset2 >= 0x24)
	{
		*(BYTE*)(0x5705A4 + BaseAddress9) = 0x04;
		switch (ViewNum)
		{
		case 0:
			*(BYTE*)(0x5705A0 + BaseAddress9) = 0x44;
			*(BYTE*)(LetterAddr + BaseAddress9) = 0x44;
			break;
		case 1:
			*(BYTE*)(0x5705A0 + BaseAddress9) = 0x33;
			*(BYTE*)(LetterAddr + BaseAddress9) = 0x33;
			break;
		}
	}
	else if (*ffbOffset2 >= 0x1B)
	{
		*(BYTE*)(0x5705A4 + BaseAddress9) = 0x03;
		switch (ViewNum)
		{
		case 0:
			*(BYTE*)(0x5705A0 + BaseAddress9) = 0x43;
			*(BYTE*)(LetterAddr + BaseAddress9) = 0x43;
			break;
		case 1:
			*(BYTE*)(0x5705A0 + BaseAddress9) = 0x32;
			*(BYTE*)(LetterAddr + BaseAddress9) = 0x32;
			break;
		}
	}
	else if (*ffbOffset2 >= 0x12)
	{
		*(BYTE*)(0x5705A4 + BaseAddress9) = 0x02;
		switch (ViewNum)
		{
		case 0:
			*(BYTE*)(0x5705A0 + BaseAddress9) = 0x42;
			*(BYTE*)(LetterAddr + BaseAddress9) = 0x42;
			break;
		case 1:
			*(BYTE*)(0x5705A0 + BaseAddress9) = 0x31;
			*(BYTE*)(LetterAddr + BaseAddress9) = 0x31;
			break;
		}
	}
	else if (*ffbOffset2 >= 0x09)
	{
		*(BYTE*)(0x5705A4 + BaseAddress9) = 0x01;
		switch (ViewNum)
		{
		case 0:
			*(BYTE*)(0x5705A0 + BaseAddress9) = 0x41;
			*(BYTE*)(LetterAddr + BaseAddress9) = 0x41;
			break;
		case 1:
			*(BYTE*)(0x5705A0 + BaseAddress9) = 0x30;
			*(BYTE*)(LetterAddr + BaseAddress9) = 0x30;
			break;
		}
	}
	else if (*ffbOffset2 >= 0x00)
	{
		helpers->WriteFloat32(0x4AD0FC, -1.0, true);
		*(BYTE*)(0x5705A0 + BaseAddress9) = 0x08;
		*(BYTE*)(0x5705A4 + BaseAddress9) = 0x00;
		*(BYTE*)(LetterAddr + BaseAddress9) = 0x00;
	}
}

DWORD WINAPI InputRT9(LPVOID lpParam)
{
	int deltaTimer = 16;

	while (!PauseEnabled)
	{
		GameState = *(BYTE*)(0x570190 + BaseAddress9);
		BYTE Chosen = *(BYTE*)(0x5705E8 + BaseAddress9);

		if (GameState == 0x05)
		{
			if (!MenuHack)
			{
				MenuHack = true;
				injector::MakeNOP((0x78A27 + BaseAddress9), 6, true);
			}
		}
		else if (GameState == 0x06 || GameState == 0x08 || GameState == 0x12)
		{
			if (MenuHack)
			{
				MenuHack = false;
				MenuHackStopWriting = false;

				injector::WriteMemory((0x78A27 + BaseAddress9), 0x03448689, true);
				injector::WriteMemory((0x78A2B + BaseAddress9), 0x8E890000, true);
			}
		}

		if (MenuHack)
		{
			if (Chosen == 0x01)
			{
				MenuHackStopWriting = true;
			}

			if (!MenuHackStopWriting)
			{
				if (*ffbOffset2 >= 0xEE)
				{
					*(BYTE*)(0x570234 + BaseAddress9) = 0x0A;
				}
				else if (*ffbOffset2 >= 0xDD)
				{
					*(BYTE*)(0x570234 + BaseAddress9) = 0x0C;
				}
				else if (*ffbOffset2 >= 0xCC)
				{
					*(BYTE*)(0x570234 + BaseAddress9) = 0x08;
				}
				else if (*ffbOffset2 >= 0xBB)
				{
					*(BYTE*)(0x570234 + BaseAddress9) = 0x0D;
				}
				else if (*ffbOffset2 >= 0xAA)
				{
					*(BYTE*)(0x570234 + BaseAddress9) = 0x0E;
				}
				else if (*ffbOffset2 >= 0x99)
				{
					*(BYTE*)(0x570234 + BaseAddress9) = 0x09;
				}
				else if (*ffbOffset2 >= 0x88)
				{
					*(BYTE*)(0x570234 + BaseAddress9) = 0x0B;
				}
				else if (*ffbOffset2 >= 0x77)
				{
					*(BYTE*)(0x570234 + BaseAddress9) = 0x02;
				}
				else if (*ffbOffset2 >= 0x66)
				{
					*(BYTE*)(0x570234 + BaseAddress9) = 0x00;
				}
				else if (*ffbOffset2 >= 0x55)
				{
					*(BYTE*)(0x570234 + BaseAddress9) = 0x04;
				}
				else if (*ffbOffset2 >= 0x44)
				{
					*(BYTE*)(0x570234 + BaseAddress9) = 0x06;
				}
				else if (*ffbOffset2 >= 0x33)
				{
					*(BYTE*)(0x570234 + BaseAddress9) = 0x05;
				}
				else if (*ffbOffset2 >= 0x22)
				{
					*(BYTE*)(0x570234 + BaseAddress9) = 0x03;
				}
				else if (*ffbOffset2 >= 0x11)
				{
					*(BYTE*)(0x570234 + BaseAddress9) = 0x01;
				}
				else if (*ffbOffset2 >= 0x00)
				{
					*(BYTE*)(0x570234 + BaseAddress9) = 0x10;
				}
			}
		}

		if (GameState == 0x09)
			NameScreenInput(0);
		else
		{
			if (NameEntryHack)
			{
				NameEntryHack = false;
				injector::WriteMemoryRaw((0x7CC26 + BaseAddress9), "\x89\x83\xB4\x06\x00\x00", 6, true);
				injector::WriteMemoryRaw((0x7CC2C + BaseAddress9), "\x89\xB3\xB0\x06\x00\x00", 6, true);
				injector::WriteMemoryRaw((0x76864 + BaseAddress9), "\x88\x0C\x06", 3, true);
				injector::WriteMemoryRaw((0x76867 + BaseAddress9), "\xC6\x44\x06\x01\x00", 5, true);
			}
		}

		if (ToBool(config["General"]["Windowed"]))
		{
			if (hWndRT9 == 0)
			{
				hWndRT9 = FindWindowW(NULL, TEXT("Dirty Drivin'"));
			}
			else if ((GetWindowLongPtrA(hWndRT9, GWL_STYLE) != 0x14CA0000))
			{
				SetWindowLongPtr(hWndRT9, GWL_STYLE, 0x14CA0000);
				SetWindowLongPtr(hWndRT9, GWL_EXSTYLE, 0x00000000);
				SetWindowPos(hWndRT9, HWND_TOPMOST, 0, 0, 1366, 797, SWP_SHOWWINDOW | SWP_NOZORDER);
			}
		}

		if (!ToBool(config["General"]["Free Play"]))
		{
			CoinInput(0);
		}

		// buttons see bitwise values in TPui//RawThrills.cs
		// START
		if (*ffbOffset & 0x08)
		{
			if (STARTpressed == false)
			{
				keybd_event(VK_SPACE, 0, 0, 0);
				STARTpressed = true;
			}
		}
		else
		{
			if (STARTpressed == true)
			{
				keybd_event(VK_SPACE, 0, KEYEVENTF_KEYUP, 0);
				STARTpressed = false;
			}
		}

		// BUTTON 1/ CRANK BACKWARD
		// !!! NOTE: CRANK FORWARD ONLY USED FOR GAS WHEN CONVERTING FROM H2Overdrive CABINET !!! //
		if (*ffbOffset & 0x100)
		{
			if (button1pressed == false)
			{
				keybd_event(VK_SPACE, 0, 0, 0);
				button1pressed = true;
			}
		}
		else
		{
			if (button1pressed == true)
			{
				keybd_event(VK_SPACE, 0, KEYEVENTF_KEYUP, 0);
				button1pressed = false;
			}
		}

		// BUTTON 2/ VIEW
		if (*ffbOffset & 0x200)
		{
			if (button2pressed == false)
			{
				keybd_event(0x56, 0, 0, 0);
				button2pressed = true;
			}
		}
		else
		{
			if (button2pressed == true)
			{
				keybd_event(0x56, 0, KEYEVENTF_KEYUP, 0);
				button2pressed = false;
			}
		}

		// WHEEL
		if ((GameState == 0x06) && (*ffbOffset2 > 0x60 && *ffbOffset2 < 0x70))
		{
			if (!RiptideHack)
			{
				RiptideHack = true;
				*(BYTE*)(0x44BAD0 + BaseAddress9) = 0x08;
				injector::WriteMemory<float>((0x4AD0FC + BaseAddress9), -1.0f, true);
			}
		}
		else
		{
			if (RiptideHack)
			{
				RiptideHack = false;
				*(BYTE*)(0x44BAD0 + BaseAddress9) = 0x0B;
			}

			if (GameState != 9)
			{
				int iWheel = (((float)*ffbOffset2) - 128);
				float wheel = (iWheel * 0.0078125f);
				injector::WriteMemory<float>((0x4AD0FC + BaseAddress9), wheel, true);
			}
		}
		//// GAS
		float gas = (float)*ffbOffset3 / 255.0f;
		float brake = (float)*ffbOffset4 / 255.0f;
		// injector::WriteMemory<float>((0x4AD0F8 + BaseAddress9), gas, true);
		// BRAKE BUTTON HACK = if brake pressed gas is reduced
		injector::WriteMemory<float>((0x4AD0F8 + BaseAddress9), gas - brake, true);

		//DEBUG//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		// info(true, "test value %f %f ", *ffbOffset2, x);
		//DEBUG//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

		Sleep(deltaTimer);
	}

	Sleep(deltaTimer);
	InputRT9(0);

	return 0;
}

D3DPRESENT_PARAMETERS* pPresentationParameters_RT9;
uintptr_t d3dcall;
void __stdcall D3D9CreateParamPatch()
{
#if _M_IX86
	__asm mov d3dcall, edx
	__asm mov edx, [ebp + 0x1c]
		__asm mov pPresentationParameters_RT9, edx
	pPresentationParameters_RT9->Windowed = TRUE;
	pPresentationParameters_RT9->FullScreen_RefreshRateInHz = 0;
	pPresentationParameters_RT9->hDeviceWindow = NULL;
	injector::WriteMemoryRaw(0x63B332, "\xFF\xD2\x3D\x68\x08", 5, true);
	__asm mov edx, pPresentationParameters_RT9
	__asm mov[ebp + 0x1c], edx
	__asm mov edx, d3dcall
#endif
	return;
}

static InitFunction DirtyDrivinFunc([]()
	{
		// PATCHING EXE AT RUNTIME (reboots, network, filepath, config, CRC...
		injector::WriteMemoryRaw((0x335DD4 + BaseAddress9), "\x44\x69\x72\x74\x79\x20\x44\x72\x69\x76\x69\x6E\x27\x00", 14, true); // edit window caption text
		injector::WriteMemoryRaw((0x3B00 + BaseAddress9), "\xEB", 1, true);
		if (ToBool(config["General"]["Free Play"]))
		{
			injector::WriteMemoryRaw((0x43B88 + BaseAddress9), "\xC6\x05\x6C\xFE\x96\x00\x01\x90\x90\xC6\x05\x58\x4A\x88\x00\x01\xB8\x58\x2A\x74\x00\xEB", 22, true);
		}
		else
		{
			injector::WriteMemoryRaw((0x43B88 + BaseAddress9), "\x83\x3D\x6C\xFE\x96\x00\x00\x74\x0E\x83\x3D\x58\x4A\x88\x00\x00\xB8\x58\x2A\x74\x00\x75", 22, true);

			LPCWSTR Open = L"open .\\Coin\\Coin.mp3 type mpegvideo";
			int Opening = mciSendString(Open, NULL, 0, 0);
			if (Opening != 0)
			{
				SoundFail = true;
			}
		}
		injector::WriteMemoryRaw((0x65447 + BaseAddress9), "\xEB\x0B\x90\x90", 4, true);
		injector::WriteMemoryRaw((0xBC9E8 + BaseAddress9), "\xEB", 1, true);
		injector::WriteMemoryRaw((0x1B5BF5 + BaseAddress9), "\x90\x90", 2, true);
		injector::WriteMemoryRaw((0x239B5C + BaseAddress9), "\xEB", 1, true);
		injector::WriteMemoryRaw((0x2F236E + BaseAddress9), "\x90\x90\x90", 3, true);
		injector::WriteMemoryRaw((0x2FB88F + BaseAddress9), "\x90\x90", 2, true);
		injector::WriteMemoryRaw((0x337A07 + BaseAddress9), "\x35", 1, true);

		//CONTROLS PATCH
		injector::MakeNOP((0x5159A + BaseAddress9), 6, true);
		injector::MakeNOP((0x515AC + BaseAddress9), 6, true);
		injector::MakeNOP((0x514BB + BaseAddress9), 4, true);
		injector::MakeNOP((0x514C0 + BaseAddress9), 3, true);

		//TVVS CONTROLS PATCH
		injector::MakeNOP((0x506C57), 6, true);
		injector::MakeNOP((0x506C5D), 6, true);
		injector::MakeNOP((0x506C63), 6, true);
		injector::MakeNOP((0x506C69), 6, true);
		injector::MakeNOP((0x506C6F), 6, true);
		injector::MakeNOP((0x506C75), 6, true);
		injector::MakeNOP((0x506C7B), 6, true);

		injector::MakeNOP((0x506883), 2, true);
		injector::MakeNOP((0x54DDE0), 2, true);
		injector::MakeNOP((0x54DD3B), 2, true);

		//tvvs controls patch #1
		injector::MakeJMP(0x506EEF, 0x729F00);
		injector::MakeCALL(0x729F00, ServiceControlsPatch);
		injector::WriteMemoryRaw(0x729F05, "\xBF\x00\x00\x00\x00", 5, true);
		injector::WriteMemoryRaw(0x729F0A, "\x39\x3D\x30\xC3\x98\x00", 6, true);
		injector::MakeJMP(0x729F10, 0x506EF5);
		//tvvs controls patch #2
		injector::MakeJMP(0x500A70, 0x729F20);
		injector::MakeCALL(0x729F20, ServiceControlsPatch);
		injector::WriteMemoryRaw(0x729F25, "\x83\x3D\x80\xC3\x98\x00\x00", 7, true);
		injector::WriteMemoryRaw(0x729F2C, "\x0F\x84\xB9\x6B\xDD\xFF", 6, true);
		injector::MakeJMP(0x729F32, 0x500A79);

		CreateThread(NULL, 0, InputRT9, NULL, 0, NULL);

		// auto accl off
		if (ToBool(config["General"]["AutoAcclOff"]))
		{
			injector::MakeNOP((0xAA6E6 + BaseAddress9), 6, true);
			// alternative patch
			// injector::WriteMemoryRaw((0x96fd70), "\x00", 1, true);
		}

		if (ToBool(config["General"]["Windowed"]))
		{
			injector::MakeJMP(0x63B332, 0x729EF0);
			injector::MakeCALL(0x729EF0, D3D9CreateParamPatch);
			injector::MakeJMP(0x729EF5, 0x63B332);
		}

	}, GameID::DirtyDrivin);
#endif
#pragma optimize("", on)
#endif