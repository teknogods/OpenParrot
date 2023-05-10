#include <StdInc.h>
#include "Utility/InitFunction.h"
#include "Functions/Global.h"
#include "Utility\Hooking.Patterns.h"
#include "Utility/Helper.h"

extern int* ffbOffset;
extern int* ffbOffset2;
extern int* ffbOffset3;
extern int* ffbOffset4;
extern int* ffbOffset5;

static bool Coin;
static bool Coin2;
static bool Test;
static bool InTestMenu;
static bool SoundUp;
static bool SoundDown;
static bool SoundSelect;
static bool SoundCoin;
static bool SoundCoin2;
static bool MenuUp;
static bool MenuDown;
static bool MenuSelect;
static bool MenuSelect2;
static bool P1Active;
static bool P2Active;
static bool P1Continue;
static bool P2Continue;
static bool P1Trigger;
static bool P2Trigger;
static bool P1LevelSelectTrigger;
static bool P2LevelSelectTrigger;
static bool P1Reload;
static bool P2Reload;
static bool P1HighScoreTrigger;
static bool P2HighScoreTrigger;
static bool P1IsLead;
static bool P2IsLead;
static bool P1isActive;
static bool P2isActive;

static int P1TrigCount;
static int P2TrigCount;
static int MainMenuPosition;
static int ResX;
static int ResY;
static int P1Name;
static int P2Name;
static int P1Position = 1;
static int P2Position = 1;

static UINT8 P1Gun;
static UINT8 P2Gun;

extern HWND D3D9hWnd;

typedef FARPROC(WINAPI* tGetProcAddress)(HMODULE, LPCSTR);
tGetProcAddress GetProcAddressOri;

static const char* WindowTitle = "OpenParrot - Friction";

static HWND(WINAPI* CreateWindowExAOri)(DWORD dwExStyle, LPCSTR lpClassName, LPCSTR lpWindowName, DWORD dwStyle, int X, int Y, int nWidth, int nHeight, HWND hWndParent, HMENU hMenu, HINSTANCE hInstance, LPVOID lpParam);
static HWND WINAPI CreateWindowExAHook(DWORD dwExStyle, LPCSTR lpClassName, LPCSTR lpWindowName, DWORD dwStyle, int X, int Y, int nWidth, int nHeight, HWND hWndParent, HMENU hMenu, HINSTANCE hInstance, LPVOID lpParam)
{
	lpWindowName = WindowTitle;

	return CreateWindowExAOri(dwExStyle, lpClassName, lpWindowName, dwStyle, 0, 0, nWidth, nHeight, hWndParent, hMenu, hInstance, lpParam);
}

static WNDPROC OriWndProc = NULL;
static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_SETCURSOR:
		SetCursor(NULL);
		break;
	default:
		return CallWindowProc(OriWndProc, hWnd, message, wParam, lParam);
	}
	return 0;
}

static int ExitGameAfterChangingGoreSetting()
{
	TerminateProcess(GetCurrentProcess(), 0);
	return 0;
}

static int vsIOBoard_Init()
{
	info("vsIOBoard_Init");
	return 0;
}

static int vsIOBoard_Close()
{
	info("vsIOBoard_Close");
	return 0;
}

static int vsIOBoard_getVideoMode()
{
	info("vsIOBoard_getVideoMode");
	return 1;
}

static int vsIOBoard_getVSyncPolarity()
{
	info("vsIOBoard_getVSyncPolarity");
	return 0;
}

static int vsIOBoard_getHSyncPolarity()
{
	info("vsIOBoard_getHSyncPolarity");
	return 0;
}

static int vsIOBoard_AddListener()
{
	info("vsIOBoard_AddListener");
	return 0;
}

static int vsIOBoard_RemoveListener()
{
	info("vsIOBoard_RemoveListener");
	return 0;
}

static int vsIOBoard_SetCalibration()
{
	info("vsIOBoard_SetCalibration");
	return 0;
}

static int vsIOBoard_Calibrate()
{
	info("vsIOBoard_Calibrate");
	return 0;
}

static int vsIOBoard_SetUpGun()
{
	info("vsIOBoard_SetUpGun");
	return 0;
}

static int vsIOBoard_GetGunParams()
{
	info("vsIOBoard_GetGunParams");
	return 0;
}

static int  vsIOBoard_SetFlashDelay()
{
	info("vsIOBoard_SetFlashDelay");
	return 0;
}

static int vsIOBoard_SetGunAccuracy()
{
	info("vsIOBoard_SetGunAccuracy");
	return 0;
}

static FARPROC WINAPI GetProcAddressHook(HMODULE hModule, LPCSTR lpProcName)
{
	if (!strcmp(lpProcName, "vsIOBoard_Init"))
		return (FARPROC)&vsIOBoard_Init;

	if (!strcmp(lpProcName, "vsIOBoard_Close"))
		return (FARPROC)&vsIOBoard_Close;

	if (!strcmp(lpProcName, "vsIOBoard_getVideoMode"))
		return (FARPROC)&vsIOBoard_getVideoMode;

	if (!strcmp(lpProcName, "vsIOBoard_getVSyncPolarity"))
		return (FARPROC)&vsIOBoard_getVSyncPolarity;

	if (!strcmp(lpProcName, "vsIOBoard_getHSyncPolarity"))
		return (FARPROC)&vsIOBoard_getHSyncPolarity;

	if (!strcmp(lpProcName, "vsIOBoard_AddListener"))
		return (FARPROC)&vsIOBoard_AddListener;

	if (!strcmp(lpProcName, "vsIOBoard_RemoveListener"))
		return (FARPROC)&vsIOBoard_RemoveListener;

	if (!strcmp(lpProcName, "vsIOBoard_SetCalibration"))
		return (FARPROC)&vsIOBoard_SetCalibration;

	if (!strcmp(lpProcName, "vsIOBoard_Calibrate"))
		return (FARPROC)&vsIOBoard_Calibrate;

	if (!strcmp(lpProcName, "vsIOBoard_SetUpGun"))
		return (FARPROC)&vsIOBoard_SetUpGun;

	if (!strcmp(lpProcName, "vsIOBoard_GetGunParams"))
		return (FARPROC)&vsIOBoard_GetGunParams;

	if (!strcmp(lpProcName, "vsIOBoard_SetFlashDelay"))
		return (FARPROC)&vsIOBoard_SetFlashDelay;

	if (!strcmp(lpProcName, "vsIOBoard_SetGunAccuracy"))
		return (FARPROC)&vsIOBoard_SetGunAccuracy;

		return GetProcAddressOri(hModule, lpProcName);
}

static void Inputs(Helpers* helpers)
{
	DWORD CoinBase = helpers->ReadInt32(0x1450C4, true);
	DWORD PlayerBase = helpers->ReadInt32(0x145088, true);

	if (P2IsLead)
	{
		P2Active = helpers->ReadByte(PlayerBase + 0x461, false);
		P1Active = helpers->ReadByte(PlayerBase + 0x462, false);
		P2Continue = helpers->ReadByte(PlayerBase + 0x467, false);
		P1Continue = helpers->ReadByte(PlayerBase + 0x468, false);
		P2Gun = helpers->ReadByte(PlayerBase + 0x494, false);
		P1Gun = helpers->ReadByte(PlayerBase + 0x498, false);
	}
	else
	{
		P1Active = helpers->ReadByte(PlayerBase + 0x461, false);
		P2Active = helpers->ReadByte(PlayerBase + 0x462, false);
		P1Continue = helpers->ReadByte(PlayerBase + 0x467, false);
		P2Continue = helpers->ReadByte(PlayerBase + 0x468, false);
		P1Gun = helpers->ReadByte(PlayerBase + 0x494, false);
		P2Gun = helpers->ReadByte(PlayerBase + 0x498, false);
	}

	if (!D3D9hWnd)
		D3D9hWnd = FindWindowA(0, WindowTitle);

	if (OriWndProc == NULL) // Hide Cursor
	{
		if (D3D9hWnd > NULL)
			OriWndProc = (WNDPROC)::SetWindowLongPtr((HWND)D3D9hWnd, GWLP_WNDPROC, (LONG_PTR)WndProc);
	}

	if (*ffbOffset & 0x02)
	{
		if (!Coin)
		{
			Coin = true;
			
			if (CoinBase)
				++*(DWORD*)(CoinBase + 0x00);
		}
	}
	else
	{
		if (Coin)
			Coin = false;
	}

	if (*ffbOffset & 0x800)
	{
		if (!Coin2)
		{
			Coin2 = true;

			if (CoinBase)
				++*(DWORD*)(CoinBase + 0x00);
		}
	}
	else
	{
		if (Coin2)
			Coin2 = false;
	}

	if (!(*ffbOffset & 0x04))
	{
		if (P1LevelSelectTrigger)
			P1LevelSelectTrigger = false;
	}

	if (!(*ffbOffset & 0x08))
	{
		if (P2LevelSelectTrigger)
			P2LevelSelectTrigger = false;
	}
}

static DWORD WINAPI RunningLoop(LPVOID lpParam)
{
	while (true)
	{
		Inputs(0);
		Sleep(16);
	}
}

static int(__fastcall* LevelSelectAxisOri)(DWORD* a1, void* EDX, int a2, int a3);
static int __fastcall LevelSelectAxisHook(DWORD* a1, void* EDX, int a2, int a3)
{
	if (P1LevelSelectTrigger)
	{
		a2 = (*ffbOffset2 / 255.0) * 640;
		a3 = (*ffbOffset3 / 255.0) * 480;
	}
	else
	{
		a2 = (*ffbOffset4 / 255.0) * 640;
		a3 = (*ffbOffset5 / 255.0) * 480;
	}

	return LevelSelectAxisOri(a1, EDX, a2, a3);
}

static int(__fastcall* HighScoreTriggerOri)(int a1, void* EDX, float a2);
static int __fastcall HighScoreTriggerHook(int a1, void* EDX, float a2)
{
	HighScoreTriggerOri(a1, EDX, a2);

	if (P2IsLead)
	{
		if (*(BYTE*)(a1 + 741))
		{
			int P1XAxis = (*ffbOffset2 / 255.0) * 640;
			int P1YAxis = (*ffbOffset3 / 255.0) * 480;

			*(WORD*)(a1 + 728) = P1XAxis; // P1 X Axis
			*(WORD*)(a1 + 736) = P1YAxis; // P1 Y Axis

			if (*ffbOffset & 0x04)
			{
				if (!P1HighScoreTrigger)
				{
					P1HighScoreTrigger = true;

					*(BYTE*)(a1 + 108) = 0x01;

					if (P1XAxis >= 0x54 && P1XAxis <= 0x87 && P1YAxis >= 0x99 && P1YAxis <= 0xCC) { P1Name = 0; }
					else if (P1XAxis >= 0x9A && P1XAxis <= 0xCD && P1YAxis >= 0x99 && P1YAxis <= 0xCC) { P1Name = 1; }
					else if (P1XAxis >= 0xE0 && P1XAxis <= 0x113 && P1YAxis >= 0x99 && P1YAxis <= 0xCC) { P1Name = 2; }
					else if (P1XAxis >= 0x126 && P1XAxis <= 0x159 && P1YAxis >= 0x99 && P1YAxis <= 0xCC) { P1Name = 3; }
					else if (P1XAxis >= 0x16C && P1XAxis <= 0x19F && P1YAxis >= 0x99 && P1YAxis <= 0xCC) { P1Name = 4; }
					else if (P1XAxis >= 0x1B2 && P1XAxis <= 0x1E5 && P1YAxis >= 0x99 && P1YAxis <= 0xCC) { P1Name = 5; }
					else if (P1XAxis >= 0x1F8 && P1XAxis <= 0x22B && P1YAxis >= 0x99 && P1YAxis <= 0xCC) { P1Name = 6; }
					else if (P1XAxis >= 0x54 && P1XAxis <= 0x87 && P1YAxis >= 0xD5 && P1YAxis <= 0x108) { P1Name = 7; }
					else if (P1XAxis >= 0x9A && P1XAxis <= 0xCD && P1YAxis >= 0xD5 && P1YAxis <= 0x108) { P1Name = 8; }
					else if (P1XAxis >= 0xE0 && P1XAxis <= 0x113 && P1YAxis >= 0xD5 && P1YAxis <= 0x108) { P1Name = 9; }
					else if (P1XAxis >= 0x126 && P1XAxis <= 0x159 && P1YAxis >= 0xD5 && P1YAxis <= 0x108) { P1Name = 10; }
					else if (P1XAxis >= 0x16C && P1XAxis <= 0x19F && P1YAxis >= 0xD5 && P1YAxis <= 0x108) { P1Name = 11; }
					else if (P1XAxis >= 0x1B2 && P1XAxis <= 0x1E5 && P1YAxis >= 0xD5 && P1YAxis <= 0x108) { P1Name = 12; }
					else if (P1XAxis >= 0x1F8 && P1XAxis <= 0x22B && P1YAxis >= 0xD5 && P1YAxis <= 0x108) { P1Name = 13; }
					else if (P1XAxis >= 0x54 && P1XAxis <= 0x87 && P1YAxis >= 0x111 && P1YAxis <= 0x144) { P1Name = 14; }
					else if (P1XAxis >= 0x9A && P1XAxis <= 0xCD && P1YAxis >= 0x111 && P1YAxis <= 0x144) { P1Name = 15; }
					else if (P1XAxis >= 0xE0 && P1XAxis <= 0x113 && P1YAxis >= 0x111 && P1YAxis <= 0x144) { P1Name = 16; }
					else if (P1XAxis >= 0x126 && P1XAxis <= 0x159 && P1YAxis >= 0x111 && P1YAxis <= 0x144) { P1Name = 17; }
					else if (P1XAxis >= 0x16C && P1XAxis <= 0x19F && P1YAxis >= 0x111 && P1YAxis <= 0x144) { P1Name = 18; }
					else if (P1XAxis >= 0x1B2 && P1XAxis <= 0x1E5 && P1YAxis >= 0x111 && P1YAxis <= 0x144) { P1Name = 19; }
					else if (P1XAxis >= 0x1F8 && P1XAxis <= 0x22B && P1YAxis >= 0x111 && P1YAxis <= 0x144) { P1Name = 20; }
					else if (P1XAxis >= 0x54 && P1XAxis <= 0x87 && P1YAxis >= 0x14D && P1YAxis <= 0x180) { P1Name = 21; }
					else if (P1XAxis >= 0x9A && P1XAxis <= 0xCD && P1YAxis >= 0x14D && P1YAxis <= 0x180) { P1Name = 22; }
					else if (P1XAxis >= 0xE0 && P1XAxis <= 0x113 && P1YAxis >= 0x14D && P1YAxis <= 0x180) { P1Name = 23; }
					else if (P1XAxis >= 0x126 && P1XAxis <= 0x159 && P1YAxis >= 0x14D && P1YAxis <= 0x180) { P1Name = 24; }
					else if (P1XAxis >= 0x16C && P1XAxis <= 0x19F && P1YAxis >= 0x14D && P1YAxis <= 0x180) { P1Name = 25; }
					else if (P1XAxis >= 0x1B2 && P1XAxis <= 0x1E5 && P1YAxis >= 0x14D && P1YAxis <= 0x180) { P1Name = 26; }
					else if (P1XAxis >= 0x1F8 && P1XAxis <= 0x22B && P1YAxis >= 0x14D && P1YAxis <= 0x180) { P1Name = 27; }
					else if (P1XAxis >= 0xCE && P1XAxis <= 0x1B1 && P1YAxis >= 0x18E && P1YAxis <= 0x1B3) { *(float*)(a1 + 44) = 0; }
					else
						P1Name = 0xFFFFFFFF;

					switch (P1Position)
					{
					case 1:
						if (P1Name < 0xFFFFFFFF && P1Name != 27)
						{
							*(DWORD*)(a1 + 88) = P1Name;
							P1Position = 2;
						}

						if (P1Name == 27)
							*(DWORD*)(a1 + 88) = 0xFFFFFFFF;
						break;
					case 2:
						if (P1Name < 0xFFFFFFFF && P1Name != 27)
						{
							*(DWORD*)(a1 + 92) = P1Name;
							P1Position = 3;
						}

						if (P1Name == 27)
						{
							*(DWORD*)(a1 + 92) = 0xFFFFFFFF;
							P1Position = 1;
						}
						break;
					case 3:
						if (P1Name < 0xFFFFFFFF && P1Name != 27)
							*(DWORD*)(a1 + 96) = P1Name;

						if (P1Name == 27)
						{
							*(DWORD*)(a1 + 96) = 0xFFFFFFFF;
							P1Position = 2;
						}
						break;
					}
				}
			}
			else
			{
				if (P1HighScoreTrigger)
					P1HighScoreTrigger = false;
			}
		}

		if (*(BYTE*)(a1 + 740))
		{
			int P2XAxis = (*ffbOffset4 / 255.0) * 640;
			int P2YAxis = (*ffbOffset5 / 255.0) * 480;

			*(WORD*)(a1 + 724) = P2XAxis; // P2 X Axis
			*(WORD*)(a1 + 732) = P2YAxis; // P2 Y Axis

			if (*ffbOffset & 0x08)
			{
				if (!P2HighScoreTrigger)
				{
					P2HighScoreTrigger = true;

					*(BYTE*)(a1 + 108) = 0x01;

					if (P2XAxis >= 0x54 && P2XAxis <= 0x87 && P2YAxis >= 0x99 && P2YAxis <= 0xCC) { P2Name = 0; }
					else if (P2XAxis >= 0x9A && P2XAxis <= 0xCD && P2YAxis >= 0x99 && P2YAxis <= 0xCC) { P2Name = 1; }
					else if (P2XAxis >= 0xE0 && P2XAxis <= 0x113 && P2YAxis >= 0x99 && P2YAxis <= 0xCC) { P2Name = 2; }
					else if (P2XAxis >= 0x126 && P2XAxis <= 0x159 && P2YAxis >= 0x99 && P2YAxis <= 0xCC) { P2Name = 3; }
					else if (P2XAxis >= 0x16C && P2XAxis <= 0x19F && P2YAxis >= 0x99 && P2YAxis <= 0xCC) { P2Name = 4; }
					else if (P2XAxis >= 0x1B2 && P2XAxis <= 0x1E5 && P2YAxis >= 0x99 && P2YAxis <= 0xCC) { P2Name = 5; }
					else if (P2XAxis >= 0x1F8 && P2XAxis <= 0x22B && P2YAxis >= 0x99 && P2YAxis <= 0xCC) { P2Name = 6; }
					else if (P2XAxis >= 0x54 && P2XAxis <= 0x87 && P2YAxis >= 0xD5 && P2YAxis <= 0x108) { P2Name = 7; }
					else if (P2XAxis >= 0x9A && P2XAxis <= 0xCD && P2YAxis >= 0xD5 && P2YAxis <= 0x108) { P2Name = 8; }
					else if (P2XAxis >= 0xE0 && P2XAxis <= 0x113 && P2YAxis >= 0xD5 && P2YAxis <= 0x108) { P2Name = 9; }
					else if (P2XAxis >= 0x126 && P2XAxis <= 0x159 && P2YAxis >= 0xD5 && P2YAxis <= 0x108) { P2Name = 10; }
					else if (P2XAxis >= 0x16C && P2XAxis <= 0x19F && P2YAxis >= 0xD5 && P2YAxis <= 0x108) { P2Name = 11; }
					else if (P2XAxis >= 0x1B2 && P2XAxis <= 0x1E5 && P2YAxis >= 0xD5 && P2YAxis <= 0x108) { P2Name = 12; }
					else if (P2XAxis >= 0x1F8 && P2XAxis <= 0x22B && P2YAxis >= 0xD5 && P2YAxis <= 0x108) { P2Name = 13; }
					else if (P2XAxis >= 0x54 && P2XAxis <= 0x87 && P2YAxis >= 0x111 && P2YAxis <= 0x144) { P2Name = 14; }
					else if (P2XAxis >= 0x9A && P2XAxis <= 0xCD && P2YAxis >= 0x111 && P2YAxis <= 0x144) { P2Name = 15; }
					else if (P2XAxis >= 0xE0 && P2XAxis <= 0x113 && P2YAxis >= 0x111 && P2YAxis <= 0x144) { P2Name = 16; }
					else if (P2XAxis >= 0x126 && P2XAxis <= 0x159 && P2YAxis >= 0x111 && P2YAxis <= 0x144) { P2Name = 17; }
					else if (P2XAxis >= 0x16C && P2XAxis <= 0x19F && P2YAxis >= 0x111 && P2YAxis <= 0x144) { P2Name = 18; }
					else if (P2XAxis >= 0x1B2 && P2XAxis <= 0x1E5 && P2YAxis >= 0x111 && P2YAxis <= 0x144) { P2Name = 19; }
					else if (P2XAxis >= 0x1F8 && P2XAxis <= 0x22B && P2YAxis >= 0x111 && P2YAxis <= 0x144) { P2Name = 20; }
					else if (P2XAxis >= 0x54 && P2XAxis <= 0x87 && P2YAxis >= 0x14D && P2YAxis <= 0x180) { P2Name = 21; }
					else if (P2XAxis >= 0x9A && P2XAxis <= 0xCD && P2YAxis >= 0x14D && P2YAxis <= 0x180) { P2Name = 22; }
					else if (P2XAxis >= 0xE0 && P2XAxis <= 0x113 && P2YAxis >= 0x14D && P2YAxis <= 0x180) { P2Name = 23; }
					else if (P2XAxis >= 0x126 && P2XAxis <= 0x159 && P2YAxis >= 0x14D && P2YAxis <= 0x180) { P2Name = 24; }
					else if (P2XAxis >= 0x16C && P2XAxis <= 0x19F && P2YAxis >= 0x14D && P2YAxis <= 0x180) { P2Name = 25; }
					else if (P2XAxis >= 0x1B2 && P2XAxis <= 0x1E5 && P2YAxis >= 0x14D && P2YAxis <= 0x180) { P2Name = 26; }
					else if (P2XAxis >= 0x1F8 && P2XAxis <= 0x22B && P2YAxis >= 0x14D && P2YAxis <= 0x180) { P2Name = 27; }
					else if (P2XAxis >= 0xCE && P2XAxis <= 0x1B1 && P2YAxis >= 0x18E && P2YAxis <= 0x1B3) { *(float*)(a1 + 44) = 0; }
					else
						P2Name = 0xFFFFFFFF;

					switch (P2Position)
					{
					case 1:
						if (P2Name < 0xFFFFFFFF && P2Name != 27)
						{
							*(DWORD*)(a1 + 76) = P2Name;
							P2Position = 2;
						}

						if (P2Name == 27)
							*(DWORD*)(a1 + 76) = 0xFFFFFFFF;
						break;
					case 2:
						if (P2Name < 0xFFFFFFFF && P2Name != 27)
						{
							*(DWORD*)(a1 + 80) = P2Name;
							P2Position = 3;
						}

						if (P2Name == 27)
						{
							*(DWORD*)(a1 + 80) = 0xFFFFFFFF;
							P2Position = 1;
						}
						break;
					case 3:
						if (P2Name < 0xFFFFFFFF && P2Name != 27)
							*(DWORD*)(a1 + 84) = P2Name;

						if (P2Name == 27)
						{
							*(DWORD*)(a1 + 84) = 0xFFFFFFFF;
							P2Position = 2;
						}
						break;
					}
				}
			}
			else
			{
				if (P2HighScoreTrigger)
					P2HighScoreTrigger = false;
			}
		}
	}
	else
	{
		if (*(BYTE*)(a1 + 740))
		{
			int P1XAxis = (*ffbOffset2 / 255.0) * 640;
			int P1YAxis = (*ffbOffset3 / 255.0) * 480;

			*(WORD*)(a1 + 724) = P1XAxis; // P1 X Axis
			*(WORD*)(a1 + 732) = P1YAxis; // P1 Y Axis

			if (*ffbOffset & 0x04)
			{
				if (!P1HighScoreTrigger)
				{
					P1HighScoreTrigger = true;

					*(BYTE*)(a1 + 108) = 0x01;

					if (P1XAxis >= 0x54 && P1XAxis <= 0x87 && P1YAxis >= 0x99 && P1YAxis <= 0xCC) { P1Name = 0; }
					else if (P1XAxis >= 0x9A && P1XAxis <= 0xCD && P1YAxis >= 0x99 && P1YAxis <= 0xCC) { P1Name = 1; }
					else if (P1XAxis >= 0xE0 && P1XAxis <= 0x113 && P1YAxis >= 0x99 && P1YAxis <= 0xCC) { P1Name = 2; }
					else if (P1XAxis >= 0x126 && P1XAxis <= 0x159 && P1YAxis >= 0x99 && P1YAxis <= 0xCC) { P1Name = 3; }
					else if (P1XAxis >= 0x16C && P1XAxis <= 0x19F && P1YAxis >= 0x99 && P1YAxis <= 0xCC) { P1Name = 4; }
					else if (P1XAxis >= 0x1B2 && P1XAxis <= 0x1E5 && P1YAxis >= 0x99 && P1YAxis <= 0xCC) { P1Name = 5; }
					else if (P1XAxis >= 0x1F8 && P1XAxis <= 0x22B && P1YAxis >= 0x99 && P1YAxis <= 0xCC) { P1Name = 6; }
					else if (P1XAxis >= 0x54 && P1XAxis <= 0x87 && P1YAxis >= 0xD5 && P1YAxis <= 0x108) { P1Name = 7; }
					else if (P1XAxis >= 0x9A && P1XAxis <= 0xCD && P1YAxis >= 0xD5 && P1YAxis <= 0x108) { P1Name = 8; }
					else if (P1XAxis >= 0xE0 && P1XAxis <= 0x113 && P1YAxis >= 0xD5 && P1YAxis <= 0x108) { P1Name = 9; }
					else if (P1XAxis >= 0x126 && P1XAxis <= 0x159 && P1YAxis >= 0xD5 && P1YAxis <= 0x108) { P1Name = 10; }
					else if (P1XAxis >= 0x16C && P1XAxis <= 0x19F && P1YAxis >= 0xD5 && P1YAxis <= 0x108) { P1Name = 11; }
					else if (P1XAxis >= 0x1B2 && P1XAxis <= 0x1E5 && P1YAxis >= 0xD5 && P1YAxis <= 0x108) { P1Name = 12; }
					else if (P1XAxis >= 0x1F8 && P1XAxis <= 0x22B && P1YAxis >= 0xD5 && P1YAxis <= 0x108) { P1Name = 13; }
					else if (P1XAxis >= 0x54 && P1XAxis <= 0x87 && P1YAxis >= 0x111 && P1YAxis <= 0x144) { P1Name = 14; }
					else if (P1XAxis >= 0x9A && P1XAxis <= 0xCD && P1YAxis >= 0x111 && P1YAxis <= 0x144) { P1Name = 15; }
					else if (P1XAxis >= 0xE0 && P1XAxis <= 0x113 && P1YAxis >= 0x111 && P1YAxis <= 0x144) { P1Name = 16; }
					else if (P1XAxis >= 0x126 && P1XAxis <= 0x159 && P1YAxis >= 0x111 && P1YAxis <= 0x144) { P1Name = 17; }
					else if (P1XAxis >= 0x16C && P1XAxis <= 0x19F && P1YAxis >= 0x111 && P1YAxis <= 0x144) { P1Name = 18; }
					else if (P1XAxis >= 0x1B2 && P1XAxis <= 0x1E5 && P1YAxis >= 0x111 && P1YAxis <= 0x144) { P1Name = 19; }
					else if (P1XAxis >= 0x1F8 && P1XAxis <= 0x22B && P1YAxis >= 0x111 && P1YAxis <= 0x144) { P1Name = 20; }
					else if (P1XAxis >= 0x54 && P1XAxis <= 0x87 && P1YAxis >= 0x14D && P1YAxis <= 0x180) { P1Name = 21; }
					else if (P1XAxis >= 0x9A && P1XAxis <= 0xCD && P1YAxis >= 0x14D && P1YAxis <= 0x180) { P1Name = 22; }
					else if (P1XAxis >= 0xE0 && P1XAxis <= 0x113 && P1YAxis >= 0x14D && P1YAxis <= 0x180) { P1Name = 23; }
					else if (P1XAxis >= 0x126 && P1XAxis <= 0x159 && P1YAxis >= 0x14D && P1YAxis <= 0x180) { P1Name = 24; }
					else if (P1XAxis >= 0x16C && P1XAxis <= 0x19F && P1YAxis >= 0x14D && P1YAxis <= 0x180) { P1Name = 25; }
					else if (P1XAxis >= 0x1B2 && P1XAxis <= 0x1E5 && P1YAxis >= 0x14D && P1YAxis <= 0x180) { P1Name = 26; }
					else if (P1XAxis >= 0x1F8 && P1XAxis <= 0x22B && P1YAxis >= 0x14D && P1YAxis <= 0x180) { P1Name = 27; }
					else if (P1XAxis >= 0xCE && P1XAxis <= 0x1B1 && P1YAxis >= 0x18E && P1YAxis <= 0x1B3) { *(float*)(a1 + 44) = 0; }
					else
						P1Name = 0xFFFFFFFF;

					switch (P1Position)
					{
					case 1:
						if (P1Name < 0xFFFFFFFF && P1Name != 27)
						{
							*(DWORD*)(a1 + 76) = P1Name;
							P1Position = 2;
						}

						if (P1Name == 27)
							*(DWORD*)(a1 + 76) = 0xFFFFFFFF;
						break;
					case 2:
						if (P1Name < 0xFFFFFFFF && P1Name != 27)
						{
							*(DWORD*)(a1 + 80) = P1Name;
							P1Position = 3;
						}

						if (P1Name == 27)
						{
							*(DWORD*)(a1 + 80) = 0xFFFFFFFF;
							P1Position = 1;
						}
						break;
					case 3:
						if (P1Name < 0xFFFFFFFF && P1Name != 27)
							*(DWORD*)(a1 + 84) = P1Name;

						if (P1Name == 27)
						{
							*(DWORD*)(a1 + 84) = 0xFFFFFFFF;
							P1Position = 2;
						}
						break;
					}
				}
			}
			else
			{
				if (P1HighScoreTrigger)
					P1HighScoreTrigger = false;
			}
		}

		if (*(BYTE*)(a1 + 741))
		{
			int P2XAxis = (*ffbOffset4 / 255.0) * 640;
			int P2YAxis = (*ffbOffset5 / 255.0) * 480;

			*(WORD*)(a1 + 728) = P2XAxis; // P2 X Axis
			*(WORD*)(a1 + 736) = P2YAxis; // P2 Y Axis

			if (*ffbOffset & 0x08)
			{
				if (!P2HighScoreTrigger)
				{
					P2HighScoreTrigger = true;

					*(BYTE*)(a1 + 108) = 0x01;

					if (P2XAxis >= 0x54 && P2XAxis <= 0x87 && P2YAxis >= 0x99 && P2YAxis <= 0xCC) { P2Name = 0; }
					else if (P2XAxis >= 0x9A && P2XAxis <= 0xCD && P2YAxis >= 0x99 && P2YAxis <= 0xCC) { P2Name = 1; }
					else if (P2XAxis >= 0xE0 && P2XAxis <= 0x113 && P2YAxis >= 0x99 && P2YAxis <= 0xCC) { P2Name = 2; }
					else if (P2XAxis >= 0x126 && P2XAxis <= 0x159 && P2YAxis >= 0x99 && P2YAxis <= 0xCC) { P2Name = 3; }
					else if (P2XAxis >= 0x16C && P2XAxis <= 0x19F && P2YAxis >= 0x99 && P2YAxis <= 0xCC) { P2Name = 4; }
					else if (P2XAxis >= 0x1B2 && P2XAxis <= 0x1E5 && P2YAxis >= 0x99 && P2YAxis <= 0xCC) { P2Name = 5; }
					else if (P2XAxis >= 0x1F8 && P2XAxis <= 0x22B && P2YAxis >= 0x99 && P2YAxis <= 0xCC) { P2Name = 6; }
					else if (P2XAxis >= 0x54 && P2XAxis <= 0x87 && P2YAxis >= 0xD5 && P2YAxis <= 0x108) { P2Name = 7; }
					else if (P2XAxis >= 0x9A && P2XAxis <= 0xCD && P2YAxis >= 0xD5 && P2YAxis <= 0x108) { P2Name = 8; }
					else if (P2XAxis >= 0xE0 && P2XAxis <= 0x113 && P2YAxis >= 0xD5 && P2YAxis <= 0x108) { P2Name = 9; }
					else if (P2XAxis >= 0x126 && P2XAxis <= 0x159 && P2YAxis >= 0xD5 && P2YAxis <= 0x108) { P2Name = 10; }
					else if (P2XAxis >= 0x16C && P2XAxis <= 0x19F && P2YAxis >= 0xD5 && P2YAxis <= 0x108) { P2Name = 11; }
					else if (P2XAxis >= 0x1B2 && P2XAxis <= 0x1E5 && P2YAxis >= 0xD5 && P2YAxis <= 0x108) { P2Name = 12; }
					else if (P2XAxis >= 0x1F8 && P2XAxis <= 0x22B && P2YAxis >= 0xD5 && P2YAxis <= 0x108) { P2Name = 13; }
					else if (P2XAxis >= 0x54 && P2XAxis <= 0x87 && P2YAxis >= 0x111 && P2YAxis <= 0x144) { P2Name = 14; }
					else if (P2XAxis >= 0x9A && P2XAxis <= 0xCD && P2YAxis >= 0x111 && P2YAxis <= 0x144) { P2Name = 15; }
					else if (P2XAxis >= 0xE0 && P2XAxis <= 0x113 && P2YAxis >= 0x111 && P2YAxis <= 0x144) { P2Name = 16; }
					else if (P2XAxis >= 0x126 && P2XAxis <= 0x159 && P2YAxis >= 0x111 && P2YAxis <= 0x144) { P2Name = 17; }
					else if (P2XAxis >= 0x16C && P2XAxis <= 0x19F && P2YAxis >= 0x111 && P2YAxis <= 0x144) { P2Name = 18; }
					else if (P2XAxis >= 0x1B2 && P2XAxis <= 0x1E5 && P2YAxis >= 0x111 && P2YAxis <= 0x144) { P2Name = 19; }
					else if (P2XAxis >= 0x1F8 && P2XAxis <= 0x22B && P2YAxis >= 0x111 && P2YAxis <= 0x144) { P2Name = 20; }
					else if (P2XAxis >= 0x54 && P2XAxis <= 0x87 && P2YAxis >= 0x14D && P2YAxis <= 0x180) { P2Name = 21; }
					else if (P2XAxis >= 0x9A && P2XAxis <= 0xCD && P2YAxis >= 0x14D && P2YAxis <= 0x180) { P2Name = 22; }
					else if (P2XAxis >= 0xE0 && P2XAxis <= 0x113 && P2YAxis >= 0x14D && P2YAxis <= 0x180) { P2Name = 23; }
					else if (P2XAxis >= 0x126 && P2XAxis <= 0x159 && P2YAxis >= 0x14D && P2YAxis <= 0x180) { P2Name = 24; }
					else if (P2XAxis >= 0x16C && P2XAxis <= 0x19F && P2YAxis >= 0x14D && P2YAxis <= 0x180) { P2Name = 25; }
					else if (P2XAxis >= 0x1B2 && P2XAxis <= 0x1E5 && P2YAxis >= 0x14D && P2YAxis <= 0x180) { P2Name = 26; }
					else if (P2XAxis >= 0x1F8 && P2XAxis <= 0x22B && P2YAxis >= 0x14D && P2YAxis <= 0x180) { P2Name = 27; }
					else if (P2XAxis >= 0xCE && P2XAxis <= 0x1B1 && P2YAxis >= 0x18E && P2YAxis <= 0x1B3) { *(float*)(a1 + 44) = 0; }
					else
						P2Name = 0xFFFFFFFF;

					switch (P2Position)
					{
					case 1:
						if (P2Name < 0xFFFFFFFF && P2Name != 27)
						{
							*(DWORD*)(a1 + 88) = P2Name;
							P2Position = 2;
						}

						if (P2Name == 27)
							*(DWORD*)(a1 + 88) = 0xFFFFFFFF;
						break;
					case 2:
						if (P2Name < 0xFFFFFFFF && P2Name != 27)
						{
							*(DWORD*)(a1 + 92) = P2Name;
							P2Position = 3;
						}

						if (P2Name == 27)
						{
							*(DWORD*)(a1 + 92) = 0xFFFFFFFF;
							P2Position = 1;
						}
						break;
					case 3:
						if (P2Name < 0xFFFFFFFF && P2Name != 27)
							*(DWORD*)(a1 + 96) = P2Name;

						if (P2Name == 27)
						{
							*(DWORD*)(a1 + 96) = 0xFFFFFFFF;
							P2Position = 2;
						}
						break;
					}
				}
			}
			else
			{
				if (P2HighScoreTrigger)
					P2HighScoreTrigger = false;
			}
		}
	}

	return 0;
}

static int(__fastcall* LevelSelectTriggerOri)(int a1, void* EDX, int a2);
static int __fastcall LevelSelectTriggerHook(int a1, void* EDX, int a2)
{
	LevelSelectTriggerOri(a1, EDX, a2);

	if (P1IsLead || P1isActive)
	{
		if (*ffbOffset & 0x04)
		{
			if (!P1LevelSelectTrigger)
			{
				P1LevelSelectTrigger = true;

				if (*(DWORD*)(a1 + 32))
					*(DWORD*)(a1 + 32) = 0;
				else
					*(DWORD*)(a1 + 32) = 0xFFFFFFFF;
			}
		}
		else
		{
			if (P1LevelSelectTrigger)
				P1LevelSelectTrigger = false;
		}
	}

	if (P2IsLead || P2isActive)
	{
		if (*ffbOffset & 0x08)
		{
			if (!P2LevelSelectTrigger)
			{
				P2LevelSelectTrigger = true;

				if (*(DWORD*)(a1 + 32))
					*(DWORD*)(a1 + 32) = 0;
				else
					*(DWORD*)(a1 + 32) = 0xFFFFFFFF;
			}
		}
		else
		{
			if (P2LevelSelectTrigger)
				P2LevelSelectTrigger = false;
		}
	}

	P1Position = 1;
	P2Position = 1;

	return 0;
}

static int(__fastcall* AxisTriggersOri)(int a1, void* EDX, int a2);
static int __fastcall AxisTriggersHook(int a1, void* EDX, int a2)
{
	AxisTriggersOri(a1, EDX, a2);

	bool P1SceenOut = (*ffbOffset2 <= 1 || *ffbOffset2 >= 254 || *ffbOffset3 <= 1 || *ffbOffset3 >= 254);
	bool P2SceenOut = (*ffbOffset4 <= 1 || *ffbOffset4 >= 254 || *ffbOffset5 <= 1 || *ffbOffset5 >= 254);

	if (*ffbOffset & 0x04)
	{
		if (!P1Trigger)
		{
			P1Trigger = true;
			*(BYTE*)(a1 + 18) = 0x01;

			if (P1SceenOut)
				*(BYTE*)(a1 + 16) = 0x01;
		}
		else
		{
			if (P1Gun == 0x01)
			{
				++P1TrigCount;

				if (P1TrigCount == 5)
				{
					P1TrigCount = 0;
					*(BYTE*)(a1 + 18) = 0x01;
				}
			}
			else
			{
				*(BYTE*)(a1 + 18) = 0x00;
				*(BYTE*)(a1 + 14) = 0x00;
			}
		}
	}
	else
	{
		if (P1Trigger)
			P1Trigger = false;

		if (P1TrigCount)
			P1TrigCount = 0;
	}

	if (*ffbOffset & 0x08)
	{
		if (!P2Trigger)
		{
			P2Trigger = true;
			*(BYTE*)(a1 + 19) = 0x01;

			if (P2SceenOut)
				*(BYTE*)(a1 + 17) = 0x01;
		}
		else
		{
			if (P2Gun == 0x01)
			{
				++P2TrigCount;

				if (P2TrigCount == 5)
				{
					P2TrigCount = 0;
					*(BYTE*)(a1 + 19) = 0x01;
				}
			}
			else
			{
				*(BYTE*)(a1 + 19) = 0x00;
				*(BYTE*)(a1 + 17) = 0x00;
			}
		}
	}
	else
	{
		if (P2Trigger)
			P2Trigger = false;

		if (P2TrigCount)
			P2TrigCount = 0;
	}

	if (*ffbOffset & 0x200) // P1 Reload Button
	{
		if (!P1Reload)
		{
			P1Reload = true;
			*(BYTE*)(a1 + 16) = 0x01;
		}
	}
	else
	{
		if (P1Reload)
		{
			P1Reload = false;
			*(BYTE*)(a1 + 16) = 0x00;
		}
	}

	if (*ffbOffset & 0x400) // P1 Reload Button
	{
		if (!P2Reload)
		{
			P2Reload = true;
			*(BYTE*)(a1 + 17) = 0x01;
		}
	}
	else
	{
		if (P2Reload)
		{
			P2Reload = false;
			*(BYTE*)(a1 + 17) = 0x00;
		}
	}

	*(WORD*)(a1 + 68) = (*ffbOffset2 / 255.0) * ResX; // P1 X Axis
	*(WORD*)(a1 + 76) = (*ffbOffset3 / 255.0) * ResY; // P1 Y Axis

	*(WORD*)(a1 + 72) = (*ffbOffset4 / 255.0) * ResX; // P2 X Axis
	*(WORD*)(a1 + 80) = (*ffbOffset5 / 255.0) * ResY; // P2 Y Axis

	return 0;
}

static int(__fastcall* ButtonsOri)(BYTE* a1, void* EDX, float a2);
static int __fastcall ButtonsHook(BYTE* a1, void* EDX, float a2)
{
	ButtonsOri(a1, EDX, a2);

	if (*ffbOffset & 0x20) // P1 Start
	{
		if (!P1Active && !P2Active && !P1Continue && !P2Continue)
		{
			a1[401] = 1;
			P2IsLead = false;
			P1IsLead = true;
		}
		else
			P1isActive = true;
	}

	if (*ffbOffset & 0x40) // P2 Start
	{
		if (!P1Active && !P2Active && !P1Continue && !P2Continue)
		{
			a1[402] = 1;
			P2IsLead = true;
			P1IsLead = false;
		}
		else
			P2isActive = true;
	}

	if (P1isActive)
	{
		if (!P1Active)
			a1[403] = 1;
		else
			P1isActive = false;
	}

	if (P2isActive)
	{
		if (!P2Active)
			a1[404] = 1;
		else
			P2isActive = false;
	}

	if (*ffbOffset & 0x01)
	{
		if (!Test)
		{
			Test = true;
			a1[136] = 1; // Test
			a1[128] = 1; // Sound
		}
	}
	else
	{
		if (Test)
			Test = false;
	}

	if (*ffbOffset & 0x02)
	{
		if (!SoundCoin)
		{
			SoundCoin = true;
			a1[128] = 1;
		}
	}
	else
	{
		if (SoundCoin)
			SoundCoin = false;
	}

	if (*ffbOffset & 0x800)
	{
		if (!SoundCoin2)
		{
			SoundCoin2 = true;
			a1[128] = 1;
		}
	}
	else
	{
		if (SoundCoin2)
			SoundCoin2 = false;
	}

	if (InTestMenu)
	{
		if (*ffbOffset & 0x80)
		{
			if (!SoundUp)
			{
				SoundUp = true;
				a1[128] = 1;
			}
		}
		else
		{
			if (SoundUp)
				SoundUp = false;
		}

		if (*ffbOffset & 0x100)
		{
			if (!SoundDown)
			{
				SoundDown = true;
				a1[128] = 1;
			}
		}
		else
		{
			if (SoundDown)
				SoundDown = false;
		}

		if (*ffbOffset & 0x10)
		{
			if (!SoundSelect)
			{
				SoundSelect = true;
				a1[128] = 1;
			}
		}
		else
		{
			if (SoundSelect)
				SoundSelect = false;
		}
	}

	return 0;
}

static int(__fastcall* TestMenuMainOri)(DWORD* a1, void* EDX);
static int __fastcall TestMenuMainHook(DWORD* a1, void* EDX)
{
	TestMenuMainOri(a1, EDX);

	MainMenuPosition = a1[14];

	if (MainMenuPosition < 7)
		InTestMenu = true;

	if (*ffbOffset & 0x100)
	{ 
		if (!MenuDown)
		{
			MenuDown = true;

			if (a1[14] < 7)
				++a1[14];
			else
				a1[14] = 1;
		}
	}
	else
	{
		if (MenuDown)
			MenuDown = false;
	}

	if (*ffbOffset & 0x80)
	{
		if (!MenuUp)
		{
			MenuUp = true;

			if (a1[14] > 1)
				--a1[14];
			else
				a1[14] = 7;
		}
	}
	else
	{
		if (MenuUp)
			MenuUp = false;
	}

	return 0;
}

static int(__fastcall* TestMenuCoinSettingsOri)(DWORD* a1, void* EDX);
static int __fastcall TestMenuCoinSettingsHook(DWORD* a1, void* EDX)
{
	TestMenuCoinSettingsOri(a1, EDX);

	if (*ffbOffset & 0x10)
	{
		if (!MenuSelect2)
		{
			MenuSelect2 = true;

			switch (a1[13])
			{
			case 0x01: // Currency Type	
				if (a1[14] < 0x02)
					++a1[14];
				else
					a1[14] = 0x00;
				break;
			case 0x02: // Coins Per Play
				if (a1[15] < 0xFF)
					++a1[15];
				else
					a1[15] = 0x00;
				break;
			case 0x03: // Coins Per Continue
				if (a1[16] < 0xFF)
					++a1[16];
				else
					a1[16] = 0x00;
				break;
			case 0x04: // Coins Per Mech 1
				if (a1[17] < 0xFF)
					++a1[17];
				else
					a1[17] = 0x00;
				break;
			case 0x05: // Coins Per Mech 2
				if (a1[18] < 0xFF)
					++a1[18];
				else
					a1[18] = 0x00;
				break;
			case 0x06: // Service Credits
				if (a1[19] < 0xFF)
					++a1[19];
				else
					a1[19] = 0x00;
				break;
			case 0x07: // Exit and Save Settings
				break;
			}
		}
	}
	else
	{
		if (MenuSelect2)
			MenuSelect2 = false;
	}

	if (*ffbOffset & 0x100)
	{
		if (!MenuDown)
		{
			MenuDown = true;

			if (a1[13] < 7)
				++a1[13];
			else
				a1[13] = 1;
		}
	}
	else
	{
		if (MenuDown)
			MenuDown = false;
	}

	if (*ffbOffset & 0x80)
	{
		if (!MenuUp)
		{
			MenuUp = true;

			if (a1[13] > 1)
				--a1[13];
			else
				a1[13] = 7;
		}
	}
	else
	{
		if (MenuUp)
			MenuUp = false;
	}

	return 0;
}

static int(__fastcall* TestMenuGameSettingsOri)(DWORD* a1, void* EDX);
static int __fastcall TestMenuGameSettingsHook(DWORD* a1, void* EDX)
{
	TestMenuGameSettingsOri(a1, EDX);

	DWORD Menu = ((DWORD)a1 + 52);

	if (*ffbOffset & 0x10)
	{
		if (!MenuSelect2)
		{
			MenuSelect2 = true;
			switch (*(BYTE*)(Menu))
			{
			case 0x01: // Game Volume
				if (*(BYTE*)(a1 + 16) < 0x64)
					++* (BYTE*)(a1 + 16);
				else
					*(BYTE*)(a1 + 16) = 0x00;
				break;
			case 0x02: // Attract Volume
				if (*(BYTE*)(a1 + 17) < 0x64)
					++* (BYTE*)(a1 + 17);
				else
					*(BYTE*)(a1 + 17) = 0x00;
				break;
			case 0x03: // Reset High Scores
				if (*(BYTE*)(a1 + 18) < 9)
					++* (BYTE*)(a1 + 18);
				else
					*(BYTE*)(a1 + 18) = 0x00;
				break;
			case 0x04: // Brightness
				if (*(BYTE*)(a1 + 19) < 255)
					++* (BYTE*)(a1 + 19);
				else
					*(BYTE*)(a1 + 19) = 0x00;
				break;
			case 0x05: // Gore
				if (*(BYTE*)(a1 + 14))
					*(BYTE*)(a1 + 14) = 0x00;
				else
					*(BYTE*)(a1 + 14) = 0x01;
				break;
			case 0x06: // Reset Settings
				if (*(WORD*)(a1 + 20))
					*(WORD*)(a1 + 20) = 0x00;
				else
					*(WORD*)(a1 + 20) = 0x100;
				break;
			case 0x07: // Exit and Save Settings
				break;
			}
		}
	}
	else
	{
		if (MenuSelect2)
			MenuSelect2 = false;
	}
	
	if (*ffbOffset & 0x100)
	{
		if (!MenuDown)
		{
			MenuDown = true;

			if (*(BYTE*)(Menu) < 7)
				++*(BYTE*)(Menu);
			else
				*(BYTE*)(Menu) = 1;
		}
	}
	else
	{
		if (MenuDown)
			MenuDown = false;
	}

	if (*ffbOffset & 0x80)
	{
		if (!MenuUp)
		{
			MenuUp = true;

			if (*(BYTE*)(Menu) > 1)
				--*(BYTE*)(Menu);
			else
				*(BYTE*)(Menu) = 7;
		}
	}
	else
	{
		if (MenuUp)
			MenuUp = false;
	}

	return 0;
}

static int(__fastcall* TestMenuClockSettingsOri)(DWORD* a1, void* EDX);
static int __fastcall TestMenuClockSettingsHook(DWORD* a1, void* EDX)
{
	TestMenuClockSettingsOri(a1, EDX);

	if (*ffbOffset & 0x10)
	{
		if (!MenuSelect2)
		{
			MenuSelect2 = true;
			switch (a1[13])
			{
			case 0x01:
				break;
			case 0x02:
				break;
			case 0x03:
				break;
			case 0x04: // Daily Maintenance
				if (a1[14] < 0x18)
					++a1[14];
				else
					a1[14] = 0;
				break;
			case 0x05: // Exit and Save Settings
				break;
			}
		}
	}
	else
	{
		if (MenuSelect2)
			MenuSelect2 = false;
	}

	if (*ffbOffset & 0x100)
	{
		if (!MenuDown)
		{
			MenuDown = true;

			if (a1[13] < 5)
				++a1[13];
			else
				a1[13] = 1;
		}
	}
	else
	{
		if (MenuDown)
			MenuDown = false;
	}

	if (*ffbOffset & 0x80)
	{
		if (!MenuUp)
		{
			MenuUp = true;

			if (a1[13] > 1)
				--a1[13];
			else
				a1[13] = 5;
		}
	}
	else
	{
		if (MenuUp)
			MenuUp = false;
	}

	return 0;
}

static int(__fastcall* TestMenuGameStatisticsOri)(DWORD* a1, void* EDX);
static int __fastcall TestMenuGameStatisticsHook(DWORD* a1, void* EDX)
{
	TestMenuGameStatisticsOri(a1, EDX);

	DWORD Select = ((DWORD)a1 + 49);
	DWORD Menu = ((DWORD)a1 + 52);

	if (*ffbOffset & 0x10)
	{
		if (!MenuSelect2)
		{
			MenuSelect2 = true;
			switch (*(BYTE*)(Menu))
			{
			case 0x01: // Reset Statistics
				if (*(BYTE*)(Select))
					*(BYTE*)(Select) = 0x00;
				else
					*(BYTE*)(Select) = 0x01;
				break;
			case 0x02: // Exit and Save Settings
				break;
			}
		}
	}
	else
	{
		if (MenuSelect2)
			MenuSelect2 = false;
	}

	if (*ffbOffset & 0x100)
	{
		if (!MenuDown)
		{
			MenuDown = true;

			if (*(BYTE*)(Menu) < 2)
				++*(BYTE*)(Menu);
			else
				*(BYTE*)(Menu) = 1;
		}
	}
	else
	{
		if (MenuDown)
			MenuDown = false;
	}

	if (*ffbOffset & 0x80)
	{
		if (!MenuUp)
		{
			MenuUp = true;

			if (*(BYTE*)(Menu) > 1)
				--*(BYTE*)(Menu);
			else
				*(BYTE*)(Menu) = 2;
		}
	}
	else
	{
		if (MenuUp)
			MenuUp = false;
	}

	return 0;
}

static int(__fastcall* TestMenuDiagnosticsOri)(DWORD* a1, void* EDX);
static int __fastcall TestMenuDiagnosticsHook(DWORD* a1, void* EDX)
{
	TestMenuDiagnosticsOri(a1, EDX);

	DWORD Menu = ((DWORD)a1 + 64);

	if (*ffbOffset & 0x100)
	{
		if (!MenuDown)
		{
			MenuDown = true;

			if (*(BYTE*)(Menu) < 3)
				++*(BYTE*)(Menu);
			else
				*(BYTE*)(Menu) = 1;
		}
	}
	else
	{
		if (MenuDown)
			MenuDown = false;
	}

	if (*ffbOffset & 0x80)
	{
		if (!MenuUp)
		{
			MenuUp = true;

			if (*(BYTE*)(Menu) > 1)
				--*(BYTE*)(Menu);
			else
				*(BYTE*)(Menu) = 3;
		}
	}
	else
	{
		if (MenuUp)
			MenuUp = false;
	}

	return 0;
}

static int(__fastcall* TestMenuControlsOri)(DWORD* a1, void* EDX);
static int __fastcall TestMenuControlsHook(DWORD* a1, void* EDX)
{
	TestMenuControlsOri(a1, EDX);

	DWORD GunTrigger1 = ((DWORD)a1 + 49);
	DWORD GunTrigger2 = ((DWORD)a1 + 50);
	DWORD Coin1 = ((DWORD)a1 + 51);
	DWORD Coin2 = ((DWORD)a1 + 52);
	DWORD StartButton1 = ((DWORD)a1 + 53);
	DWORD StartButton2 = ((DWORD)a1 + 54);

	(*ffbOffset & 0x04) ? *(BYTE*)(GunTrigger1) = 1 : *(BYTE*)(GunTrigger1) = 0;
	(*ffbOffset & 0x08) ? *(BYTE*)(GunTrigger2) = 1 : *(BYTE*)(GunTrigger2) = 0;
	(*ffbOffset & 0x02) ? *(BYTE*)(Coin1) = 1 : *(BYTE*)(Coin1) = 0;
	(*ffbOffset & 0x800) ? *(BYTE*)(Coin2) = 1 : *(BYTE*)(Coin2) = 0;
	(*ffbOffset & 0x20) ? *(BYTE*)(StartButton1) = 1 : *(BYTE*)(StartButton1) = 0;
	(*ffbOffset & 0x40) ? *(BYTE*)(StartButton2) = 1 : *(BYTE*)(StartButton2) = 0;

	return 0;
}

static int(__fastcall* TestMenuGunCalibrationOri)(DWORD* a1, void* EDX);
static int __fastcall TestMenuGunCalibrationHook(DWORD* a1, void* EDX)
{
	TestMenuGunCalibrationOri(a1, EDX);

	DWORD Menu = ((DWORD)a1 + 52);

	if (*ffbOffset & 0x100)
	{
		if (!MenuDown)
		{
			MenuDown = true;

			if (*(BYTE*)(Menu) < 4)
				++*(BYTE*)(Menu);
			else
				*(BYTE*)(Menu) = 1;
		}
	}
	else
	{
		if (MenuDown)
			MenuDown = false;
	}

	if (*ffbOffset & 0x80)
	{
		if (!MenuUp)
		{
			MenuUp = true;

			if (*(BYTE*)(Menu) > 1)
				--*(BYTE*)(Menu);
			else
				*(BYTE*)(Menu) = 4;
		}
	}
	else
	{
		if (MenuUp)
			MenuUp = false;
	}

	return 0;
}

static int(__fastcall* TestMenuGunSettingsOri)(DWORD* a1, void* EDX);
static int __fastcall TestMenuGunSettingsHook(DWORD* a1, void* EDX)
{
	TestMenuGunSettingsOri(a1, EDX);

	DWORD Menu = ((DWORD)a1 + 52);
	DWORD FlashDelay = ((DWORD)a1 + 72);
	DWORD GunAccuracy = ((DWORD)a1 + 80);

	if (*ffbOffset & 0x10)
	{
		if (!MenuSelect2)
		{
			MenuSelect2 = true;
			switch (*(BYTE*)(Menu))
			{
			case 0x01: // Flash  Delay
				if (*(BYTE*)(FlashDelay) < 0xFF)
					++*(BYTE*)(FlashDelay);
				else
					*(BYTE*)(FlashDelay) = 0;
				break;
			case 0x02: // Gun Accuracy
				if (*(BYTE*)(GunAccuracy) < 0xFF)
					++*(BYTE*)(GunAccuracy);
				else
					*(BYTE*)(GunAccuracy) = 0;
				break;
			case 0x03: // Exit and Save Settings
				break;
			}
		}
	}
	else
	{
		if (MenuSelect2)
			MenuSelect2 = false;
	}

	if (*ffbOffset & 0x100)
	{
		if (!MenuDown)
		{
			MenuDown = true;

			if (*(BYTE*)(Menu) < 3)
				++*(BYTE*)(Menu);
			else
				*(BYTE*)(Menu) = 1;
		}
	}
	else
	{
		if (MenuDown)
			MenuDown = false;
	}

	if (*ffbOffset & 0x80)
	{
		if (!MenuUp)
		{
			MenuUp = true;

			if (*(BYTE*)(Menu) > 1)
				--*(BYTE*)(Menu);
			else
				*(BYTE*)(Menu) = 3;
		}
	}
	else
	{
		if (MenuUp)
			MenuUp = false;
	}

	return 0;
}

static int(__fastcall* TestMenuMainSelectOri)(BYTE* a1, void* EDX, float a2);
static int __fastcall TestMenuMainSelectHook(BYTE* a1, void* EDX, float a2)
{
	TestMenuMainSelectOri(a1, EDX, a2);

	if (*ffbOffset & 0x10)
	{
		if (!MenuSelect)
		{
			MenuSelect = true;

			if (MainMenuPosition == 7)
				InTestMenu = false;

			a1[50] = 1;
		}
	}
	else
	{
		if (MenuSelect)
		{
			MenuSelect = false;
			a1[50] = 0;
		}
	}

	return 0;
}

static int(__fastcall* TestMenuGameStatisticsSelectOri)(BYTE* a1, void* EDX, float a2);
static int __fastcall TestMenuGameStatisticsSelectHook(BYTE* a1, void* EDX, float a2)
{
	TestMenuGameStatisticsSelectOri(a1, EDX, a2);

	if (*ffbOffset & 0x10)
	{
		if (!MenuSelect)
		{
			MenuSelect = true;
			a1[48] = 1;
		}
	}
	else
	{
		if (MenuSelect)
		{
			MenuSelect = false;
			a1[48] = 0;
		}
	}

	return 0;
}

static int(__fastcall* TestMenuGameSettingsSelectOri)(BYTE* a1, void* EDX, float a2);
static int __fastcall TestMenuGameSettingsSelectHook(BYTE* a1, void* EDX, float a2)
{
	TestMenuGameSettingsSelectOri(a1, EDX, a2);

	if (*ffbOffset & 0x10)
	{
		if (!MenuSelect)
		{
			MenuSelect = true;
			a1[48] = 1;
		}
	}
	else
	{
		if (MenuSelect)
		{
			MenuSelect = false;
			a1[48] = 0;
		}
	}

	return 0;
}

static int(__fastcall* TestMenuCoinSettingsSelectOri)(BYTE* a1, void* EDX, float a2);
static int __fastcall TestMenuCoinSettingsSelectHook(BYTE* a1, void* EDX, float a2)
{
	TestMenuCoinSettingsSelectOri(a1, EDX, a2);

	if (*ffbOffset & 0x10)
	{
		if (!MenuSelect)
		{
			MenuSelect = true;
			a1[49] = 1;
		}
	}
	else
	{
		if (MenuSelect)
		{
			MenuSelect = false;
			a1[49] = 0;
		}
	}

	return 0;
}

static int(__fastcall* TestMenuClockSettingsSelectOri)(BYTE* a1, void* EDX, float a2);
static int __fastcall TestMenuClockSettingsSelectHook(BYTE* a1, void* EDX, float a2)
{
	TestMenuClockSettingsSelectOri(a1, EDX, a2);

	if (*ffbOffset & 0x10)
	{
		if (!MenuSelect)
		{
			MenuSelect = true;
			a1[49] = 1;
		}
	}
	else
	{
		if (MenuSelect)
		{
			MenuSelect = false;
			a1[49] = 0;
		}
	}

	return 0;
}

static int(__fastcall* TestMenuCollectionsSelectOri)(BYTE* a1, void* EDX, float a2);
static int __fastcall TestMenuCollectionsSelectHook(BYTE* a1, void* EDX, float a2)
{
	TestMenuCollectionsSelectOri(a1, EDX, a2);

	if (*ffbOffset & 0x10)
	{
		if (!MenuSelect)
		{
			MenuSelect = true;
			a1[48] = 1;
		}
	}
	else
	{
		if (MenuSelect)
		{
			MenuSelect = false;
			a1[48] = 0;
		}
	}

	return 0;
}

static int(__fastcall* TestMenuDiagnosticsSelectOri)(BYTE* a1, void* EDX, float a2);
static int __fastcall TestMenuDiagnosticsSelectHook(BYTE* a1, void* EDX, float a2)
{
	TestMenuDiagnosticsSelectOri(a1, EDX, a2);

	if (*ffbOffset & 0x10)
	{
		if (!MenuSelect)
		{
			MenuSelect = true;
			a1[49] = 1;
		}
	}
	else
	{
		if (MenuSelect)
		{
			MenuSelect = false;
			a1[49] = 0;
		}
	}

	return 0;
}

static int(__fastcall* TestMenuControlsSelectOri)(BYTE* a1, void* EDX, float a2);
static int __fastcall TestMenuControlsSelectHook(BYTE* a1, void* EDX, float a2)
{
	TestMenuControlsSelectOri(a1, EDX, a2);

	if (*ffbOffset & 0x10)
	{
		if (!MenuSelect)
		{
			MenuSelect = true;
			a1[48] = 1;
		}
	}
	else
	{
		if (MenuSelect)
		{
			MenuSelect = false;
			a1[48] = 0;
		}
	}

	return 0;
}

static int(__fastcall* TestMenuGunCalibrationSelectOri)(BYTE* a1, void* EDX, float a2);
static int __fastcall TestMenuGunCalibrationSelectHook(BYTE* a1, void* EDX, float a2)
{
	TestMenuGunCalibrationSelectOri(a1, EDX, a2);

	if (*ffbOffset & 0x10)
	{
		if (!MenuSelect)
		{
			MenuSelect = true;

			a1[49] = 1;
		}
	}
	else
	{
		if (MenuSelect)
		{
			MenuSelect = false;

			a1[49] = 0;
		}
	}

	return 0;
}

static int(__fastcall* TestMenuGunSettingsSelectOri)(BYTE* a1, void* EDX, float a2);
static int __fastcall TestMenuGunSettingsSelectHook(BYTE* a1, void* EDX, float a2)
{
	TestMenuGunSettingsSelectOri(a1, EDX, a2);

	if (*ffbOffset & 0x10)
	{
		if (!MenuSelect)
		{
			MenuSelect = true;
			a1[49] = 1;
		}
	}
	else
	{
		if (MenuSelect)
		{
			MenuSelect = false;
			a1[49] = 0;
		}
	}

	return 0;
}

static InitFunction FrictionFunc([]()
	{
		DWORD imageBase = (DWORD)GetModuleHandleA(0);

		injector::MakeNOP(imageBase + 0x1018, 6); // Remove Sleep when game first launched

		injector::MakeJMP(imageBase + 0x4BFF3, ExitGameAfterChangingGoreSetting); // Exit game when changing gore settings etc, usually windows reboots!!

		injector::WriteMemoryRaw(imageBase + 0x1180B4, "\x4F\x70\x65\x6E\x50\x61\x72\x72\x6F\x74\x2E\x64\x6C\x6C", 14, true); // Set dll name to OpenParrot.dll
		injector::WriteMemoryRaw(imageBase + 0x114FAC, "\x47\x41\x4D\x45\x20\x57\x49\x4C\x4C\x20\x45\x58\x49\x54\x20\x49\x46\x20\x47\x4F\x52\x45\x20\x53\x45\x54\x54\x49\x4E\x47\x20\x49\x53\x20\x43\x48\x41\x4E\x47\x45\x44", 41, true); // Change Gore Message
		
		MH_Initialize();
		MH_CreateHookApi(L"Kernel32.dll", "GetProcAddress", GetProcAddressHook, (void**)&GetProcAddressOri);
		MH_CreateHookApi(L"user32.dll", "CreateWindowExA", CreateWindowExAHook, (void**)&CreateWindowExAOri);
		MH_CreateHook((void*)(imageBase + 0x4D2E),	ButtonsHook, (void**)&ButtonsOri);
		MH_CreateHook((void*)(imageBase + 0x29EBD), AxisTriggersHook, (void**)&AxisTriggersOri);
		MH_CreateHook((void*)(imageBase + 0x23CC5), LevelSelectTriggerHook, (void**)&LevelSelectTriggerOri);
		MH_CreateHook((void*)(imageBase + 0x1DC8C), LevelSelectAxisHook, (void**)&LevelSelectAxisOri);
		MH_CreateHook((void*)(imageBase + 0x1E388), HighScoreTriggerHook, (void**)&HighScoreTriggerOri);
		MH_CreateHook((void*)(imageBase + 0x228F5), TestMenuMainHook, (void**)&TestMenuMainOri);
		MH_CreateHook((void*)(imageBase + 0x22D76), TestMenuMainSelectHook, (void**)&TestMenuMainSelectOri);
		MH_CreateHook((void*)(imageBase + 0x352B9), TestMenuCoinSettingsHook, (void**)&TestMenuCoinSettingsOri);
		MH_CreateHook((void*)(imageBase + 0x35C94), TestMenuCoinSettingsSelectHook, (void**)&TestMenuCoinSettingsSelectOri);
		MH_CreateHook((void*)(imageBase + 0x32E4C), TestMenuGameSettingsHook, (void**)&TestMenuGameSettingsOri);
		MH_CreateHook((void*)(imageBase + 0x33A12), TestMenuGameSettingsSelectHook, (void**)&TestMenuGameSettingsSelectOri);
		MH_CreateHook((void*)(imageBase + 0x3464E), TestMenuClockSettingsHook, (void**)&TestMenuClockSettingsOri);
		MH_CreateHook((void*)(imageBase + 0x34D4F), TestMenuClockSettingsSelectHook, (void**)&TestMenuClockSettingsSelectOri);
		MH_CreateHook((void*)(imageBase + 0x321E5), TestMenuGameStatisticsHook, (void**)&TestMenuGameStatisticsOri);
		MH_CreateHook((void*)(imageBase + 0x32839), TestMenuGameStatisticsSelectHook, (void**)&TestMenuGameStatisticsSelectOri);
		MH_CreateHook((void*)(imageBase + 0x33EA6), TestMenuCollectionsSelectHook, (void**)&TestMenuCollectionsSelectOri);
		MH_CreateHook((void*)(imageBase + 0x31A4F), TestMenuDiagnosticsHook, (void**)&TestMenuDiagnosticsOri);
		MH_CreateHook((void*)(imageBase + 0x31DB5), TestMenuDiagnosticsSelectHook, (void**)&TestMenuDiagnosticsSelectOri);
		MH_CreateHook((void*)(imageBase + 0x3C0B2), TestMenuControlsHook, (void**)&TestMenuControlsOri);
		MH_CreateHook((void*)(imageBase + 0x3C08D), TestMenuControlsSelectHook, (void**)&TestMenuControlsSelectOri);
		MH_CreateHook((void*)(imageBase + 0x3C997), TestMenuGunCalibrationHook, (void**)&TestMenuGunCalibrationOri);
		MH_CreateHook((void*)(imageBase + 0x3CD65), TestMenuGunCalibrationSelectHook, (void**)&TestMenuGunCalibrationSelectOri);
		MH_CreateHook((void*)(imageBase + 0x3DD8F), TestMenuGunSettingsHook, (void**)&TestMenuGunSettingsOri);
		MH_CreateHook((void*)(imageBase + 0x3DD18), TestMenuGunSettingsSelectHook, (void**)&TestMenuGunSettingsSelectOri);
		MH_EnableHook(MH_ALL_HOOKS);

		if (ToBool(config["General"]["Windowed"]))
			injector::WriteMemory<BYTE>(imageBase + 0x41366, 0, true);

		ResX = FetchDwordInformation("Resolution", "Width", 640);
		ResY = FetchDwordInformation("Resolution", "Height", 480);

		injector::WriteMemory<DWORD>(imageBase + 0x4140E, ResX, true);
		injector::WriteMemory<DWORD>(imageBase + 0x41407, ResY, true);

		DisableProcessWindowsGhosting(); // Stop window not responding on loading startup if clicked on

		CreateThread(NULL, 0, RunningLoop, NULL, 0, NULL);

	}, GameID::Friction);