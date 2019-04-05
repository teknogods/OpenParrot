#include <StdInc.h>
#include "Utility/InitFunction.h"
#include "Functions/Global.h"
#if _M_IX86
DWORD mainModuleBase;
int __cdecl IgnoreFunc(int a1, int a2)
{
	int result; // eax

	result = a2;
	*(BYTE *)(a2 + 8) = 1;

	// Enable Input
	*(BYTE *)(mainModuleBase + 0x67E75B) = 1;
	return result;
}

extern int* ffbOffset;
extern int* ffbOffset2;
extern int* ffbOffset3;
extern int* ffbOffset4;


float toFloat(uint8_t x)
{
	return x / 255.0;
}

float toFloatWheel(uint8_t x)
{
	return x / 255.0 * 2.0 - 1.0;
}

static int InjectKeys()
{
	int notButtons = ~*(WORD *)(mainModuleBase + 0x67E784);
	int buttons = *ffbOffset;
	DWORD wheel = *ffbOffset2;
	DWORD gas = *ffbOffset3;
	DWORD brake = *ffbOffset4;

	BYTE wheelVal = (wheel - 0xFF) / 0x100;
	BYTE gasVal = (gas - 0xFF) / 0x100;
	BYTE brakeVal = (brake - 0xFF) / 0x100;

	float wheelFloat = toFloatWheel(wheelVal);
	float gasFloat = toFloat(gasVal);
	float brakeFloat = toFloat(brakeVal);

	if (wheelVal < 0x60)
		buttons |= 0x200;

	if (wheelVal > 0xA0)
		buttons |= 0x400;

	if (gasVal > 0xA0)
		buttons |= 0x20;

	if (brakeVal > 0xA0)
		buttons |= 0x40;

	// Digital Inputs
	*(WORD *)(mainModuleBase + 0x67E784) = buttons;

	// Wheel Test Menu
	*(WORD *)(mainModuleBase + 0x518C8C) = *ffbOffset2;

	// Gas Test Menu
	*(WORD *)(mainModuleBase + 0x518C90) = *ffbOffset3;

	// Brake Test Menu
	*(WORD *)(mainModuleBase + 0x518C94) = *ffbOffset4;

	// Wheel Float
	memcpy((void *)(mainModuleBase + 0x518C80), &wheelFloat, 4);

	// Gas Float
	memcpy((void *)(mainModuleBase + 0x518C84), &gasFloat, 4);

	// Brake Float
	memcpy((void *)(mainModuleBase + 0x518C88), &brakeFloat, 4);

	*(DWORD*)0xA7E788 = buttons & notButtons;

	return 1;
}

static int(*g_origac_io_hbhi_COMUNICATE)();

signed int __cdecl ac_io_hbhi_COMUNICATE()
{
	InjectKeys();

	return g_origac_io_hbhi_COMUNICATE();
}

static InitFunction GtiClub3Func([]()
{

	mainModuleBase = (DWORD)GetModuleHandle(0);

	DWORD funcPtr = (DWORD)(void *)IgnoreFunc;

	CreateDirectoryA("EUP", nullptr);
	CreateDirectoryA("FUP", nullptr);

	injector::WriteMemory<DWORD>(mainModuleBase + 0x4C0E0C, funcPtr, true);
	injector::WriteMemory<DWORD>(mainModuleBase + 0x4C0E14, funcPtr, true);
	injector::WriteMemory<DWORD>(mainModuleBase + 0x4C0E1C, funcPtr, true);
	injector::WriteMemory<DWORD>(mainModuleBase + 0x4C0E3C, funcPtr, true);

	injector::WriteMemory<DWORD>(mainModuleBase + 0x357CB4, 0x55465C2E, true);
	injector::WriteMemory<DWORD>(mainModuleBase + 0x357C80, 0x55455C2E, true);

	MH_Initialize();
	MH_CreateHook((void*)(mainModuleBase + 0x409C0), ac_io_hbhi_COMUNICATE, (void**)&g_origac_io_hbhi_COMUNICATE); // ?
	MH_EnableHook(MH_ALL_HOOKS);

}, GameID::GTIClub3);
#endif