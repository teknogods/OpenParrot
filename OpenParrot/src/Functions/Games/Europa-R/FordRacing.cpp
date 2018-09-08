#include <StdInc.h>
#include "Utility/InitFunction.h"
#include "Functions/Global.h"
#if _M_IX86
signed int FordRacingControlInit()
{
	// start input thread
	((void(*)(int, int, int))0x4E0803)(0x4DE030, 0, 0);

	// set input initialized
	*(DWORD*)0x6B02A0 = 1;

	// device count
	*(DWORD*)0x6B0290 = 1;

	// set joystick
	*(char**)0x6A9AF4 = "JOYSTICK";

	// create mutex
	*(DWORD*)0x6A9A74 = (DWORD)CreateMutex(NULL, FALSE, NULL);

	return 1;
}
extern int* ffbOffset;

int __stdcall FordRacingFfbFunc(DWORD device, DWORD data)
{
	if(device == 0x378)
	{
		*ffbOffset = data;
	}
	return 0;
}

static InitFunction fordRacingFunc([]()
{

	DWORD oldprot = 0;
	DWORD oldprot2 = 0;
	VirtualProtect((LPVOID)0x401000, 0xEE000, 0x40, &oldprot);

	// readfile call
	static DWORD source = (DWORD)(LPVOID)&ReadFileHooked;
	*(DWORD *)0x4DE005 = (DWORD)(LPVOID)&source;
	VirtualProtect((LPVOID)0x401000, 0xEE000, oldprot, &oldprot2);

	// replace input init
	injector::MakeJMP(0x4DE1F0, FordRacingControlInit);
	// Hook FFB calls
	injector::MakeJMP(0x00467C9A, FordRacingFfbFunc);

}, GameID::FordRacing);
#endif