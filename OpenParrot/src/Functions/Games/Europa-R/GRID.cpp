#include <StdInc.h>
#include "Utility/InitFunction.h"
#include "Functions/Global.h"
#include "Utility\Hooking.Patterns.h"
#include "Utility/GameDetect.h"

#pragma comment(lib, "Ws2_32.lib")
#ifndef _M_AMD64
extern int* wheelSection;
extern int* ffbOffset;
extern int* ffbOffset2;
extern int* ffbOffset3;
extern int* ffbOffset4;

DWORD jOffset = 0x0081D64E;
DWORD myPointer = 0;

typedef struct
{
	DWORD *SomeAddress;
	float ControlValue;
} ControlStruct, *PControlStruct;

void __declspec(naked) FixMissingData()
{
	__asm
	{
		mov ebx, [edx]
		cmp ebx, 0xFFFFFFFF
		jne skip
		mov ebx, 0
		skip:
		mov[ecx], ebx
		sub eax, 1
		jmp jOffset
	}
}

void __declspec(naked) GetControlOffset()
{
	__asm
	{
		CMP myPointer, 0
		JNE skip
		CMP[ECX - 0x1C], 0x580
		JNE skip
		mov myPointer, ecx
		sub myPointer, 0x18
		skip:
		RET 4
	}
}

bool inputInitialized = false;

float toFloatGRID(uint8_t x)
{
	return x / 255.0;
}

DWORD WINAPI XInputGetStateGRID
(
	__in  DWORD         dwUserIndex,						// Index of the gamer associated with the device
	__out DWORD* pState								// Receives the current state
)
{
	if (myPointer != 0)
	{
		// Write control injector here
		if (!inputInitialized)
		{
			//info(true, "The Page is at: %08X", myPointer);
			inputInitialized = true;
		}

		BYTE wheel = *ffbOffset2;
		BYTE gas = *ffbOffset3;
		BYTE brake = *ffbOffset4;

		float wheelFloat = toFloatGRID(wheel);
		float gasFloat = toFloatGRID(gas);
		float brakeFloat = toFloatGRID(brake);

		// Wheel Float
		memcpy((void *)(myPointer + 0x484), &wheelFloat, 4);

		// Gas Float
		memcpy((void *)(myPointer + 0x49C), &gasFloat, 4);

		// Brake Float
		memcpy((void *)(myPointer + 0x494), &brakeFloat, 4);

		// FFB Wheel Float
		memcpy((void*)(myPointer + 0x4), &wheelFloat, 4);
	}
	if (GameDetect::currentGame == GameID::GRID)
	{
		return ERROR_SUCCESS;
	}
	else
	{
		return ERROR_DEVICE_NOT_CONNECTED;
	}
}

static bool InputsInit;
static int InputsCount;

static DWORD StartButton;
static DWORD ViewButton;
static DWORD ShiftUpButton;
static DWORD ShiftDownButton;
static DWORD ResetButton;
static DWORD LookBehindButton;
static DWORD LookBehind2Button;
static DWORD PauseButton;
static DWORD ReplayButton;
static DWORD LookLeftButton;
static DWORD LookRightButton;
static DWORD SelectButton;
static DWORD Gear1Button;
static DWORD Gear2Button;
static DWORD Gear3Button;
static DWORD Gear4Button;
static DWORD Gear5Button;
static DWORD Gear6Button;
static DWORD GearReverseButton;

static int(__fastcall* ButtonInputsOri)(int a1, __int64 a2);
static int __fastcall ButtonInputsHook(int a1, __int64 a2)
{
	ButtonInputsOri(a1, a2);

	unsigned int ValueRead{};
	int Count{};
	int InputsAddress{};

	if (!InputsInit)
	{
		++InputsCount;

		int BaseRead = *(DWORD*)(a1 + 68);

		if (BaseRead)
			ValueRead = (*(DWORD*)(a1 + 72) - BaseRead) >> 2;
		else
			ValueRead = 0;

		if (ValueRead)
		{
			InputsAddress = *(DWORD*)(*(DWORD*)(a1 + 68) + 4 * Count);
			++Count;
		}

		switch (InputsCount)
		{
		case 1:
			PauseButton = InputsAddress + 16;
			break;
		case 3:
			ResetButton = InputsAddress + 16;
			break;
		case 18:
			ViewButton = InputsAddress + 16;
			break;
		case 19:
			ShiftUpButton = InputsAddress + 16;
			break;
		case 20:
			ShiftDownButton = InputsAddress + 16;
			break;
		case 21:
			Gear1Button = InputsAddress + 16;
			break;
		case 22:
			Gear2Button = InputsAddress + 16;
			break;
		case 23:
			Gear3Button = InputsAddress + 16;
			break;
		case 24:
			Gear4Button = InputsAddress + 16;
			break;
		case 25:
			Gear5Button = InputsAddress + 16;
			break;
		case 26:
			Gear6Button = InputsAddress + 16;
			break;
		case 27:
			GearReverseButton = InputsAddress + 16;
			break;
		case 28:
			LookLeftButton = InputsAddress + 16;
			break;
		case 29:
			LookRightButton = InputsAddress + 16;
			break;
		case 31:
			LookBehind2Button = InputsAddress + 16;
			break;
		case 32:
			LookBehindButton = InputsAddress + 16;
			break;
		case 33:
			ResetButton = InputsAddress + 16;
			break;
		case 38:
			SelectButton = InputsAddress + 16;
			break;
		case 39:
			StartButton = InputsAddress + 16;
			break;
		case 46:
			InputsInit = true;
			break;
		}
	}

	if (StartButton)
		*ffbOffset & 0x01 ? *(DWORD*)(StartButton) = 0x01 : *(DWORD*)(StartButton) = 0x00;
	if (ShiftUpButton)
		*ffbOffset & 0x02 ? *(DWORD*)(ShiftUpButton) = 0x01 : *(DWORD*)(ShiftUpButton) = 0x00;
	if (ShiftDownButton)
		*ffbOffset & 0x04 ? *(DWORD*)(ShiftDownButton) = 0x01 : *(DWORD*)(ShiftDownButton) = 0x00;
	if (ViewButton)
		*wheelSection & 0x01 ? *(DWORD*)(ViewButton) = 0x01 : *(DWORD*)(ViewButton) = 0x00;
	if (ResetButton)
		*wheelSection & 0x02 ? *(DWORD*)(ResetButton) = 0x01 : *(DWORD*)(ResetButton) = 0x00;
	if (LookBehindButton)
		*wheelSection & 0x04 ? *(DWORD*)(LookBehindButton) = 0x01 : *(DWORD*)(LookBehindButton) = 0x00;
	if (LookLeftButton)
		*wheelSection & 0x08 ? *(DWORD*)(LookLeftButton) = 0x01 : *(DWORD*)(LookLeftButton) = 0x00;
	if (LookRightButton)
		*wheelSection & 0x10 ? *(DWORD*)(LookRightButton) = 0x01 : *(DWORD*)(LookRightButton) = 0x00;
	if (PauseButton)
		*wheelSection & 0x20 ? *(DWORD*)(PauseButton) = 0x01 : *(DWORD*)(PauseButton) = 0x00;
	if (ReplayButton)
		*wheelSection & 0x40 ? *(DWORD*)(ReplayButton) = 0x01 : *(DWORD*)(ReplayButton) = 0x00;
	if (LookBehind2Button)
		*wheelSection & 0x80 ? *(DWORD*)(LookBehind2Button) = 0x01 : *(DWORD*)(LookBehind2Button) = 0x00;
	if (Gear1Button)
		*wheelSection & 0x100 ? *(DWORD*)(Gear1Button) = 0x01 : *(DWORD*)(Gear1Button) = 0x00;
	if (Gear2Button)
		*wheelSection & 0x200 ? *(DWORD*)(Gear2Button) = 0x01 : *(DWORD*)(Gear2Button) = 0x00;
	if (Gear3Button)
		*wheelSection & 0x400 ? *(DWORD*)(Gear3Button) = 0x01 : *(DWORD*)(Gear3Button) = 0x00;
	if (Gear4Button)
		*wheelSection & 0x800 ? *(DWORD*)(Gear4Button) = 0x01 : *(DWORD*)(Gear4Button) = 0x00;
	if (Gear5Button)
		*wheelSection & 0x1000 ? *(DWORD*)(Gear5Button) = 0x01 : *(DWORD*)(Gear5Button) = 0x00;
	if (Gear6Button)
		*wheelSection & 0x2000 ? *(DWORD*)(Gear6Button) = 0x01 : *(DWORD*)(Gear6Button) = 0x00;
	if (GearReverseButton)
		*wheelSection & 0x4000 ? *(DWORD*)(GearReverseButton) = 0x01 : *(DWORD*)(GearReverseButton) = 0x00;

	if (SelectButton)
	{
		if (*ffbOffset3 > 0x10)
			*(DWORD*)(SelectButton) = 0x01;
		else
			*(DWORD*)(SelectButton) = 0x00;
	}

	return 0;
}


static InitFunction GRIDFunc([]()
{
	// Crash fix
	injector::MakeNOP(0x005AB9E5, 2);

	// Fix stupid "let's put 0xFFFFFFFF in stuff we miss to cause crashes"
	injector::MakeJMP(0x0081D647, FixMissingData, true);
	injector::WriteMemory<DWORD>(0x00AEA6DD, 0x00000000, true);

	// Get Control Offset
	injector::MakeJMP(0x00A878E0, GetControlOffset, true);

	// Button mashing on
	injector::WriteMemory<BYTE>(0xEAF304, 0x01, true);

	// Prevent overwriting of button mashing by xml
	injector::MakeNOP(0x005F44B8, 6);
	injector::MakeNOP(0x00AEC695, 5);

	// Force button mashing on
	injector::MakeNOP(0x00B76560, 6);

	// Button Inputs
	injector::MakeNOP(0xB8CECD, 3);

	MH_Initialize();
	MH_CreateHook((void*)0xB8CDD0, ButtonInputsHook, (void**)&ButtonInputsOri);
	MH_CreateHookApi(L"xinput1_3.dll", "XInputGetState", &XInputGetStateGRID, NULL);
	MH_EnableHook(MH_ALL_HOOKS);

}, GameID::GRID);
#endif