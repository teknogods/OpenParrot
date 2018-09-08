#include <StdInc.h>
#include "Utility/InitFunction.h"
#include "Functions/Global.h"
#if _M_IX86
extern int* ffbOffset;

int __stdcall Sr3FfbFunc(DWORD device, DWORD data)
{
	*ffbOffset = data;
	return 0;
}

static InitFunction sr3Func([]()
{
	DWORD oldprot = 0;
	DWORD oldprot2 = 0;
	VirtualProtect((LPVOID)0x401000, 0x273000, 0x40, &oldprot);
	// force controller init
	//injector::MakeJMP(0x57B2F0, ReturnTrue);
	memcpy((LPVOID)0x57B2F0, "\x33\xC0\x40\xC3", 4);

	// disable checks for controller pointer
	memset((LPVOID)0x57B670, 0x90, 15);

	// dereference
	memset((LPVOID)0x57B684, 0x90, 3);

	// Hook FFB
	// Remove loading of inpout32.dll
	injector::MakeNOP(0x006582A8, 0x17);
	// Give our own pointer to the FFB func
	injector::WriteMemory<uint8_t>(0x006582A8, 0xB8, true);
	injector::WriteMemory<DWORD>(0x006582A9, (DWORD)Sr3FfbFunc, true);
	//

	// ReadFile call
	static DWORD source = (DWORD)(LPVOID)&ReadFileHooked;
	*(DWORD *)0x57B696 = (DWORD)(LPVOID)&source;
	VirtualProtect((LPVOID)0x401000, 0x273000, oldprot, &oldprot2);

}, GameID::SR3);
#endif