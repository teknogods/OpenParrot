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

extern int Player1Active;
extern int Player2Active;
extern int BorderThickness;
extern int BezelPixelWidth;
extern int BezelPixelHeight;
static int FrontWindowCount;
static int WindowWidth, WindowHeight;
static int TitleCount;
static int TitleCount2;
static int TitleCount3;
static int VibrationSleepCount;
static int VibrationCount;
static int VibrationTime;
static int VibrationPower;
static int oldffbOffset2;
static int oldffbOffset3;
static int oldffbOffset4;
static int oldffbOffset5;
static int Attract1Count;
static int Attract2Count;

extern float EADPRenderWidth;
extern float EADPRenderHeight;
extern float ScaleBezelX;
extern float ScaleBezelY;
static float EADPRenderWidthOri;
static float EADPRenderHeightOri;
static float TaitoLogo;
static float currentEADPRenderWidth;
static float currentEADPRenderHeight;
static float p1X;
static float p1Y;
static float p2X;
static float p2Y;
float DoorFloatLeft;
float DoorFloatRight;
float currentDoorFloatLeft;
float currentDoorFloatRight;

extern bool EnableD3D9Crosshairs;
extern bool EnableD3D9Bezel;
extern bool EnableD3D9Border;
static bool Init;
static bool Windowed;
static bool GameFrontWindow;
static bool GetOrigRenderRes;
static bool VolUP;
static bool VolDown;
static bool P1ReadyStart;
static bool P2ReadyStart;
static bool HideShootTheMainDisplay;
static bool AttractVideo;
static bool AttractionDoors;
static bool PlayVibrationEffect;
static bool VibrationEffect;
static bool ScaleTestMenu;
static bool MultiScreen;
bool EADPCenter2D;
bool EADPCenter3D;
bool EADPNameEntry;
bool EADPAttractVidPlay;

extern DWORD resWidthD3D9;
extern DWORD resHeightD3D9;
static DWORD imageBase;
static DWORD P1Health;
static DWORD P2Health;

static UINT8 GameContinue;
UINT8 EADPVolume;

static RECT rect;

static char INIChar[256];

static void WriteVol()
{
	sprintf_s(INIChar, "%d", EADPVolume);
	WritePrivateProfileStringA("Settings", "Volume", INIChar, ".\\OpenParrot\\Settings.ini");
}

static void VolumeSetting(Helpers* helpers)
{
	DWORD Base = helpers->ReadInt32(0x212CDC, true);
	EADPVolume = helpers->ReadByte(Base + 0x49, false);

	if (*ffbOffset & 0x10)
	{
		if (!VolUP)
		{
			VolUP = true;

			helpers->WriteByte(Base + 0x49, EADPVolume + 0x03, false);
			EADPVolume = helpers->ReadByte(Base + 0x49, false);
			WriteVol();
		}
	}
	else
	{
		if (VolUP)
			VolUP = false;
	}

	if (*ffbOffset & 0x20)
	{
		if (!VolDown)
		{
			VolDown = true;

			helpers->WriteByte(Base + 0x49, EADPVolume - 0x03, false);
			EADPVolume = helpers->ReadByte(Base + 0x49, false);
			WriteVol();
		}
	}
	else
	{
		if (VolDown)
			VolDown = false;
	}
}

int(__fastcall* ResultsCenterOri)(void* ECX, void* EDX, int a2, int a3);
int __fastcall ResultsCenterHook(void* ECX, void* EDX, int a2, int a3)
{
	EADPCenter2D = true;
	return ResultsCenterOri(ECX, EDX, a2, a3);
}

static void AlignTest2D(Helpers* helpers)
{
	helpers->WriteIntPtr(0xCDE7D, resWidthD3D9, true); // Align Test Menu
}

int(__cdecl* AttractionDoorOri)(float a1, float a2, float a3);
int __cdecl AttractionDoorHook(float a1, float a2, float a3)
{
	DoorFloatLeft = a1; currentDoorFloatLeft = a1;
	DoorFloatRight = a2; currentDoorFloatRight = a2;

	return AttractionDoorOri(a1, a2, a3);
}

int(__cdecl* VibrationDoorOri)(float a1, int a2, int a3);
int __cdecl VibrationDoorHook(float a1, int a2, int a3)
{
	VibrationEffect = true;

	VibrationPower = a2;
	VibrationTime = a3;

	return VibrationDoorOri(a1, a2, a3);
}

int(__fastcall* EADPVolumeSetupOri)(void* ECX, void* EDX, float a2);
int __fastcall EADPVolumeSetup(void* ECX, void* EDX, float a2)
{
	VolumeSetting(0);
	a2 = EADPVolume / 255.0;

	return EADPVolumeSetupOri(ECX, EDX, a2);
}

int(__fastcall* AttractVideoCenterOri)(void* ECX, void* EDX);
int __fastcall AttractVideoCenterHook(void* ECX, void* EDX)
{
	AttractVideoCenterOri(ECX, EDX);

	if (AttractVideo)
	{
		if (MultiScreen)
		{
			if (*(float*)((int)ECX + 352) == -1280.5)
			{
				++Attract1Count;

				switch (Attract1Count)
				{
				case 0x01:
					*(float*)((int)ECX + 352) = *(float*)((int)ECX + 352) + ((resWidthD3D9 / 2.0) - 360.0);
					break;
				case 0x02:
					*(float*)((int)ECX + 352) = *(float*)((int)ECX + 352) + ((resHeightD3D9 / 2.0) - 640.0);
					break;
				case 0x03:
					Attract1Count = 0;
					*(float*)((int)ECX + 352) = *(float*)((int)ECX + 352) + ((resWidthD3D9 / 2.0) - 360.0);
					break;
				}
			}

			if (*(float*)((int)ECX + 352) == -512.5)
			{
				++Attract2Count;

				switch (Attract2Count)
				{
				case 0x03:
					Attract2Count = 0;
					*(float*)((int)ECX + 352) = *(float*)((int)ECX + 352) + ((resWidthD3D9 / 2.0) - 360.0);
					break;
				}
			}
		}
		else
			*(float*)((int)ECX + 352) = *(float*)((int)ECX + 352) + ((resWidthD3D9 / 2.0) - 360.0);
	}	

	return 0;
}

int(__fastcall* TestMenuCenterOri)(void* ECX, void* EDX, int a2);
int __fastcall TestMenuCenterHook(void* ECX, void* EDX, int a2)
{
	TestMenuCenterOri(ECX, EDX, a2);

	*(float*)((int)ECX + 12) = resWidthD3D9 / 2.0;

	if (ScaleTestMenu)
	{
		float ResAdjust = 5120.0 / resWidthD3D9;

		*(float*)((int)ECX + 16) = resHeightD3D9 / ResAdjust;

		*(double*)(imageBase + 0x212708) = 0.162 * ResAdjust;
	}

	AlignTest2D(0);

	return 0;
}

int(__fastcall* EADP3DCenterOri)(void* ECX, void* EDX, float a2, float a3, float a4, float a5);
int __fastcall EADP3DCenterHook(void* ECX, void* EDX, float a2, float a3, float a4, float a5)
{
	if (!GameContinue && P1Health <= 10 && P2Health <= 10 && -TaitoLogo == 0 || EADPCenter3D)
	{
		if (EADPCenter3D)
		{
			if (a2 != (resWidthD3D9 / 2.0) - (EADPRenderWidthOri / 2.0))
				a2 = a2 + ((resWidthD3D9 / 2.0) - 360.0);
		}
		else
			a2 = a2 + ((resWidthD3D9 / 2.0) - 360.0);

		if (!EADPCenter3D)
			a3 = a3 + ((resHeightD3D9 / 2.0) - 640.0);
	}

	if (MultiScreen)
	{
		if (a4 == 1024.0 && a5 == 576.0)
		{
			a2 = 0.0;
			a3 = 0.0;
		}

		if (-TaitoLogo)
		{
			if (a4 == 720.0 && a5 == 1280.0)
			{
				a2 = a2 + ((resWidthD3D9 / 2.0) - 360.0);
				a3 = a3 + ((resHeightD3D9 / 2.0) - 640.0);
			}
		}
		else
		{
			if (a4 == 720.0 && a5 == 1280.0 && (P1Health >= 10 && P2Health >= 10 || GameContinue))
			{
				a2 = a2 + ((resWidthD3D9 / 2.0) - 360.0);
				a3 = a3 + ((resHeightD3D9 / 2.0) - 640.0);
			}
		}
	}

	return EADP3DCenterOri(ECX, EDX, a2, a3, a4, a5);
}

static void TaitoLogoWrite(Helpers* helpers)
{
	DWORD TaitoBase = helpers->ReadInt32(0x212CA4, true);
	helpers->WriteFloat32(TaitoBase + 0x3C0, -360.0, false);
	helpers->WriteFloat32(TaitoBase + 0x3C4, -640.0, false);
}

static void Random2DRead(Helpers* helpers)
{
	DWORD ReadyBase = helpers->ReadInt32(0x212C80, true);
	DWORD ReadyOff0 = helpers->ReadInt32(ReadyBase + 0x10, false);
	DWORD ReadyOff1 = helpers->ReadInt32(ReadyOff0 + 0x60, false);
	P1ReadyStart = helpers->ReadByte(ReadyOff1 + 0x8D1, false);
	P2ReadyStart = helpers->ReadByte(ReadyOff1 + 0x9A1, false);

	if (P1ReadyStart || P2ReadyStart)
	{
		helpers->WriteByte(ReadyOff1 + 0x8D1, 0x01, false);
		helpers->WriteByte(ReadyOff1 + 0x9A1, 0x01, false);
	}

	DWORD HealthBase1P = helpers->ReadInt32(0x212CD8, true);
	DWORD Health1POff0 = helpers->ReadInt32(HealthBase1P + 0x08, false);
	DWORD Health1POff1 = helpers->ReadInt32(Health1POff0 + 0x08, false);
	P1Health = helpers->ReadInt32(Health1POff1 + 0xC0, false);

	DWORD HealthBase2P = helpers->ReadInt32(0x2124A4, true);
	DWORD Health2POff0 = helpers->ReadInt32(HealthBase2P + 0x04, false);
	DWORD Health2POff1 = helpers->ReadInt32(Health2POff0 + 0x04, false);
	DWORD Health2POff2 = helpers->ReadInt32(Health2POff1 + 0x9F0, false);
	P2Health = helpers->ReadInt32(Health2POff2 + 0xC0, false);

	DWORD ContinueBase = helpers->ReadInt32(0x212CC4, true);
	DWORD ContinueOff0 = helpers->ReadInt32(ContinueBase + 0x04, false);
	DWORD ContinueOff1 = helpers->ReadInt32(ContinueOff0 + 0x40, false);
	GameContinue = helpers->ReadInt32(ContinueOff1 + 0x3C, false);
}

int(__fastcall* EADP2DCenterOri)(void* ECX, void* EDX);
int __fastcall EADP2DCenterHook(void* ECX, void* EDX)
{
	Random2DRead(0);

	EADP2DCenterOri(ECX, EDX);

	if (HideShootTheMainDisplay)
	{
		if (*(float*)((int)ECX + 20) == 108.0 && *(float*)((int)ECX + 24) == 29.0 && -TaitoLogo == 0 && !GameContinue)
			*(float*)((int)ECX + 20) = *(float*)((int)ECX + 20) - 99999999999.0;
	}

	if (*(float*)((int)ECX + 20) == 236.5 && -TaitoLogo == 0)
		*(float*)((int)ECX + 20) = *(float*)((int)ECX + 20) - ((resWidthD3D9 / 2.0) - 360.0);

	if (*(float*)((int)ECX + 20) == 211.0 && (P1Health > 10 || P2Health > 10))
		*(float*)((int)ECX + 20) = *(float*)((int)ECX + 20) - ((resWidthD3D9 / 2.0) - 360.0);

	if (!GameContinue && EADPCenter2D && -TaitoLogo == 0 || !GameContinue && EADPNameEntry && -TaitoLogo == 0)
		*(float*)((int)ECX + 20) = *(float*)((int)ECX + 20) - ((resWidthD3D9 / 2.0) - 360.0);

	if (EADPNameEntry)
	{
		if (EnableD3D9Crosshairs)
		{
			if (oldffbOffset2 && oldffbOffset2 != *ffbOffset2 && *ffbOffset2 && oldffbOffset3 && oldffbOffset3 != *ffbOffset3 && *ffbOffset3)
				Player1Active = true;
			if (oldffbOffset4 && oldffbOffset4 != *ffbOffset4 && *ffbOffset4 && oldffbOffset5 && oldffbOffset5 != *ffbOffset5 && *ffbOffset5)
				Player2Active = true;
		}

		*(float*)(imageBase + 0x21224C) = (p1X / 255.0) * 768.0; // P1 X Axis
		*(float*)(imageBase + 0x212250) = (p1Y / 255.0) * resHeightD3D9; // P1 Y Axis

		*(float*)(imageBase + 0x2122A4) = (p2X / 255.0) * 768.0; // P2 X Axis
		*(float*)(imageBase + 0x2122A8) = (p2Y / 255.0) * resHeightD3D9; // P2 Y Axis
	}

	if (-TaitoLogo)
	{
		if (*(float*)((int)ECX + 20) == 135.0)
		{
			++TitleCount;

			if (P1ReadyStart && P2ReadyStart)
			{
				TitleCount = 0;
				++TitleCount3;
				
				switch (TitleCount3)
				{
				case 0x01:
					*(float*)((int)ECX + 20) = *(float*)((int)ECX + 20) - ((resWidthD3D9 / 2.0) - 360.0);
					break;
				case 0x02:
					*(float*)((int)ECX + 20) = *(float*)((int)ECX + 20) + ((resWidthD3D9 / 2.0) - 360.0);
					TitleCount3 = 0;
					break;
				}
			}
			else
			{
				TitleCount3 = 0;

				switch (TitleCount)
				{
				case 0x01:
					if ((P1ReadyStart && P2ReadyStart) || P1ReadyStart)
						*(float*)((int)ECX + 20) = *(float*)((int)ECX + 20) + ((resWidthD3D9 / 2.0) - 360.0);
					else if (P2ReadyStart)
						*(float*)((int)ECX + 20) = *(float*)((int)ECX + 20) + ((resWidthD3D9 / 2.0) - 360.0);
					else
						*(float*)((int)ECX + 20) = *(float*)((int)ECX + 20) - ((resWidthD3D9 / 2.0) - 360.0);
					break;
				case 0x02:
					if ((P1ReadyStart && P2ReadyStart) || P2ReadyStart)
						*(float*)((int)ECX + 20) = *(float*)((int)ECX + 20) - ((resWidthD3D9 / 2.0) - 360.0);
					else if (P1ReadyStart)
						*(float*)((int)ECX + 20) = *(float*)((int)ECX + 20) - ((resWidthD3D9 / 2.0) - 360.0);
					else
						*(float*)((int)ECX + 20) = *(float*)((int)ECX + 20) + ((resWidthD3D9 / 2.0) - 360.0);
					TitleCount = 0;
					break;
				}
			}
		}
		else if (*(float*)((int)ECX + 20) == 137.0)
		{
			++TitleCount2;

			switch (TitleCount2)
			{
			case 0x01:
				*(float*)((int)ECX + 20) = *(float*)((int)ECX + 20) - ((resWidthD3D9 / 2.0) - 360.0);
				break;
			case 0x02:
				*(float*)((int)ECX + 20) = *(float*)((int)ECX + 20) + ((resWidthD3D9 / 2.0) - 360.0);
				TitleCount2 = 0;
				break;
			}
		}
		else
			*(float*)((int)ECX + 20) = *(float*)((int)ECX + 20) - ((resWidthD3D9 / 2.0) - 360.0);

		*(float*)((int)ECX + 24) = *(float*)((int)ECX + 24) - ((resHeightD3D9 / 2.0) - 640.0);

		TaitoLogoWrite(0);
	}

	return 0;
}

void EADPInputs(Helpers* helpers)
{
	if (!Init)
	{
		Init = true;
		imageBase = (DWORD)GetModuleHandleA(0);
		Windowed = ToBool(config["General"]["Windowed"]);
		AttractionDoors = ToBool(config["Attraction Doors"]["Enable"]);
		HideShootTheMainDisplay = ToBool(config["General"]["Hide Shoot the display msg"]);
		ScaleTestMenu = ToBool(config["General"]["Scale Test Menu"]);
		MultiScreen = ToBool(config["General"]["Multi Screen"]);
	}

	*(BYTE*)(imageBase + 0x201C10) = 0x02; // Enable Inputs

	DWORD VolBase = helpers->ReadInt32(0x212CDC, true);

	if (EADPVolume == 0)
		VolumeSetting(0);

	helpers->WriteByte(VolBase + 0x49, EADPVolume, false);

	if (Windowed)
	{ 
		helpers->WriteIntPtr(0x5F5290, 720, false); // Force Res
		helpers->WriteIntPtr(0x5F5294, 1280, false); // Force Res
	}
	else
	{
		DWORD CheckVideoBase = helpers->ReadInt32(0x209C38, true);
		UINT8 CheckVideo1 = helpers->ReadByte(CheckVideoBase + 0x8AB, false);
		UINT8 CheckVideo2 = helpers->ReadByte(CheckVideoBase + 0x95B, false);

		if (-TaitoLogo && CheckVideo1 && CheckVideo2 && EADPAttractVidPlay)
		{
			if (!AttractVideo)
				AttractVideo = true;
		}
		else
		{
			if (AttractVideo)
				AttractVideo = false;
		}
	}

	if (AttractionDoors)
	{
		if (VibrationEffect)
		{
			++VibrationCount;
			++VibrationSleepCount;

			float Power = (float)VibrationPower / 200.0;

			if (VibrationSleepCount == 2)
			{
				VibrationSleepCount = 0;

				if (!PlayVibrationEffect)
					PlayVibrationEffect = true;
				else
					PlayVibrationEffect = false;
			}

			if (PlayVibrationEffect)
			{
				DoorFloatLeft += Power;
				DoorFloatRight += Power;
			}
			else
			{
				DoorFloatLeft -= Power;
				DoorFloatRight -= Power;
			}

			if (DoorFloatLeft > 1.0)
				DoorFloatLeft = 1.0;

			if (DoorFloatRight > 1.0)
				DoorFloatRight = 1.0;

			if (DoorFloatLeft < 0.0)
				DoorFloatLeft = 0.0;

			if (DoorFloatRight < 0.0)
				DoorFloatRight = 0.0;

			if (VibrationCount >= VibrationTime / 16.0)
			{
				VibrationEffect = false;
				DoorFloatLeft = currentDoorFloatLeft;
				DoorFloatRight = currentDoorFloatRight;
			}
		}
		else
		{
			if (VibrationCount)
				VibrationCount = 0;
		}
	}

	DWORD TaitoBase = helpers->ReadInt32(0x212CA4, true);
	TaitoLogo = helpers->ReadFloat32(TaitoBase + 0x3C0, false);

	DWORD RenderBase = helpers->ReadInt32(0x212C80, true);
	currentEADPRenderWidth = helpers->ReadFloat32(RenderBase + 0x94, false);
	currentEADPRenderHeight = helpers->ReadFloat32(RenderBase + 0x98, false);

	if ((int)currentEADPRenderWidth % 2 != 0)
		helpers->WriteFloat32(RenderBase + 0x94, currentEADPRenderWidth + 1.0, false);

	if (currentEADPRenderWidth)
		EADPRenderWidth = currentEADPRenderWidth;

	if (currentEADPRenderHeight)
		EADPRenderHeight = currentEADPRenderHeight;

	float LeftMaxWidth = (int)round((float)resWidthD3D9 / 2.0) - ((float)EADPRenderWidth / 2.0);
	float RightMaxWidth = (int)round((float)resWidthD3D9 / 2.0) + ((float)EADPRenderWidth / 2.0);
	float TopMaxHeight = (int)round((float)resHeightD3D9 / 2.0) - ((float)EADPRenderHeight / 2.0);
	float BottomMaxHeight = (int)round((float)resHeightD3D9 / 2.0) + ((float)EADPRenderHeight / 2.0);

	float P1XAxis = (*ffbOffset2 / 255.0) * resWidthD3D9;
	float P1YAxis = (*ffbOffset3 / 255.0) * resHeightD3D9;

	float P2XAxis = (*ffbOffset4 / 255.0) * resWidthD3D9;
	float P2YAxis = (*ffbOffset5 / 255.0) * resHeightD3D9;

	bool P1SceenOut = (P1XAxis <= LeftMaxWidth || P1XAxis >= RightMaxWidth || P1YAxis <= TopMaxHeight || P1YAxis >= BottomMaxHeight);
	bool P2SceenOut = (P2XAxis <= LeftMaxWidth || P2XAxis >= RightMaxWidth || P2YAxis <= TopMaxHeight || P2YAxis >= BottomMaxHeight);

	P1SceenOut ? *(BYTE*)(imageBase + 0x201BEA) = 0x01 : *(BYTE*)(imageBase + 0x201BEA) = 0x00; // P1 Out
	P2SceenOut ? *(BYTE*)(imageBase + 0x201BF4) = 0x01 : *(BYTE*)(imageBase + 0x201BF4) = 0x00; // P2 Out

	(*ffbOffset & 0x01) ? *(BYTE*)(imageBase + 0x201BF0) = 0x01 : *(BYTE*)(imageBase + 0x201BF0) = 0x00; // P1 Trigger
	(*ffbOffset & 0x02) ? *(BYTE*)(imageBase + 0x212235) = 0x01 : *(BYTE*)(imageBase + 0x212235) = 0x00; // P1 Grenade

	(*ffbOffset & 0x04) ? *(BYTE*)(imageBase + 0x201BFA) = 0x01 : *(BYTE*)(imageBase + 0x201BFA) = 0x00; // P2 Trigger
	(*ffbOffset & 0x08) ? *(BYTE*)(imageBase + 0x21228D) = 0x01 : *(BYTE*)(imageBase + 0x21228D) = 0x00; // P2 Grenade

	float xMin = 255.0 * LeftMaxWidth / resWidthD3D9;
	float xMax = 255.0 * RightMaxWidth / resWidthD3D9;
	float yMin = 255.0 * TopMaxHeight / resHeightD3D9;
	float yMax = 255.0 * BottomMaxHeight / resHeightD3D9;

	p1X = (float)*ffbOffset2;
	p1Y = (float)*ffbOffset3;
	p2X = (float)*ffbOffset4;
	p2Y = (float)*ffbOffset5;

	p1X = fmin(fmax(p1X, xMin), xMax);
	p1X = round((p1X - xMin) / (xMax - xMin) * 255.0);

	p1Y = fmin(fmax(p1Y, yMin), yMax);
	p1Y = round((p1Y - yMin) / (yMax - yMin) * 255.0);

	p2X = fmin(fmax(p2X, xMin), xMax);
	p2X = round((p2X - xMin) / (xMax - xMin) * 255.0);

	p2Y = fmin(fmax(p2Y, yMin), yMax);
	p2Y = round((p2Y - yMin) / (yMax - yMin) * 255.0);

	*(WORD*)(imageBase + 0x201BEC) = (p1X / 255.0) * 16384; // P1 X Axis
	*(WORD*)(imageBase + 0x201BEE) = (p1Y / 255.0) * 16384; // P1 Y Axis

	*(WORD*)(imageBase + 0x201BF6) = (p2X / 255.0) * 16384; // P2 X Axis
	*(WORD*)(imageBase + 0x201BF8) = (p2Y / 255.0) * 16384; // P2 Y Axis

	if (!GetOrigRenderRes && EADPRenderWidth && EADPRenderHeight)
	{
		GetOrigRenderRes = true;
		EADPRenderWidthOri = EADPRenderWidth;
		EADPRenderHeightOri = EADPRenderHeight;
	}

	if (EnableD3D9Bezel || EnableD3D9Border)
	{
		if (EnableD3D9Bezel && EnableD3D9Border)
		{
			helpers->WriteFloat32(RenderBase + 0x94, EADPRenderWidthOri - ((BorderThickness / 2.0) + BezelPixelWidth), false);
			helpers->WriteFloat32(RenderBase + 0x98, EADPRenderHeightOri - ((BorderThickness / 2.0) + BezelPixelHeight), false);
		}
		else if (EnableD3D9Border)
		{
			helpers->WriteFloat32(RenderBase + 0x94, EADPRenderWidthOri - (BorderThickness / 2.0), false);
			helpers->WriteFloat32(RenderBase + 0x98, EADPRenderHeightOri - (BorderThickness / 2.0), false);
		}
		else
		{
			float W = helpers->WriteFloat32(RenderBase + 0x94, EADPRenderWidthOri - BezelPixelWidth, false);
			helpers->WriteFloat32(RenderBase + 0x98, EADPRenderHeightOri - BezelPixelHeight, false);

			if ((int)W % 2 != 0)
				helpers->WriteFloat32(RenderBase + 0x94, W + 1.0, false);
		}
	}

	if (EnableD3D9Crosshairs)
	{
		DWORD TimerBase = helpers->ReadInt32(0x210A80, true);

		float Start = helpers->ReadFloat32(TimerBase + 0x17B28, false);
		UINT8 P1Active = helpers->ReadByte(TimerBase + 0x1794C, false);
		UINT8 P2Active = helpers->ReadByte(TimerBase + 0x17A38, false);

		if (Start && !EADPNameEntry)
		{
			if (P1Active)
				Player1Active = true;
			else
				Player1Active = false;

			if (P2Active)
				Player2Active = true;
			else
				Player2Active = false;
		}
		else
		{
			if (-TaitoLogo > 0)
			{
				if (oldffbOffset2 && oldffbOffset2 != *ffbOffset2 && *ffbOffset2 && oldffbOffset3 && oldffbOffset3 != *ffbOffset3 && *ffbOffset3)
					Player1Active = true;
				if (oldffbOffset4 && oldffbOffset4 != *ffbOffset4 && *ffbOffset4 && oldffbOffset5 && oldffbOffset5 != *ffbOffset5 && *ffbOffset5)
					Player2Active = true;
			}
			else
			{
				if (!EADPNameEntry)
				{
					if (Player1Active)
						Player1Active = false;

					if (Player2Active)
						Player2Active = false;
				}
			}
		}
		oldffbOffset2 = *ffbOffset2;
		oldffbOffset3 = *ffbOffset3;
		oldffbOffset4 = *ffbOffset4;
		oldffbOffset5 = *ffbOffset5;
	}

	HWND Game = FindWindowA("Eva", "OpenParrot - Elevator Action: Death Parade");

	if (Game)
	{
		if (GetWindowRect(Game, &rect))
		{
			WindowWidth = rect.right - rect.left;
			WindowHeight = rect.bottom - rect.top;
		}

		resWidthD3D9 = WindowWidth;
		resHeightD3D9 = WindowHeight;


		if (P1SceenOut)
		{
			*(float*)(imageBase + 0x21224C) = (*ffbOffset2 / 255.0) * resWidthD3D9; // P1 X Axis
			*(float*)(imageBase + 0x212250) = (*ffbOffset3 / 255.0) * resHeightD3D9; // P1 Y Axis
		}

		if (P2SceenOut)
		{
			*(float*)(imageBase + 0x2122A4) = (*ffbOffset4 / 255.0) * resWidthD3D9; // P2 X Axis
			*(float*)(imageBase + 0x2122A8) = (*ffbOffset5 / 255.0) * resHeightD3D9; // P2 Y Axis
		}
	}
}