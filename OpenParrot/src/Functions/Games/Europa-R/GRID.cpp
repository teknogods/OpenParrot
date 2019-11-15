#include <StdInc.h>
#include "Utility/InitFunction.h"
#include "Functions/Global.h"
#include "Utility\Hooking.Patterns.h"

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

void SetButton(DWORD address, bool pressed)
{
	if (pressed)
		*(DWORD*)address = 0x3F800000;
	else
		*(DWORD*)address = 0;
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

		DWORD buttons2 = *wheelSection;
		DWORD buttons = *ffbOffset;
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

		SetButton(myPointer + 0x4, buttons & 0x1);
		SetButton(myPointer + 0x1C, buttons & 0x2);
		SetButton(myPointer + 0x24, buttons & 0x4);

		SetButton(myPointer + 0xC, buttons2 & 0x1);
		SetButton(myPointer + 0x14, buttons2 & 0x2);
		SetButton(myPointer + 0x4C, buttons2 & 0x4);
		SetButton(myPointer + 0x414, buttons2 & 0x8);
		SetButton(myPointer + 0x404, buttons2 & 0x10);
		SetButton(myPointer + 0x3C, buttons2 & 0x20);
		SetButton(myPointer + 0x34, buttons2 & 0x40);
		SetButton(myPointer + 0x40c, buttons2 & 0x80);
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

	// Hook XInput to send our controls
	MH_Initialize();
	MH_CreateHookApi(L"xinput1_3.dll", "XInputGetState", &XInputGetStateGRID, NULL);
	MH_EnableHook(MH_ALL_HOOKS);

}, GameID::GRID);
#endif