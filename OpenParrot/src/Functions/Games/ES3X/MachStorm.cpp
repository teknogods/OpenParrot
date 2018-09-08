#if _M_IX86
#include <StdInc.h>
#include "Utility/InitFunction.h"
#include "Functions/Global.h"

extern LPCSTR hookPort;
static InitFunction MachStormFunc([]()
{
	DWORD imageBase = (DWORD)GetModuleHandleA(0);

	// Patch to ignore all errors
	injector::WriteMemory<DWORD>(imageBase + 0x7071E, 0x90909090, true);

	// Patch to skip extra device strange init (also delays game start on real cabinet several minutes!!!)
	injector::WriteMemory<DWORD>(imageBase + 0x76430, 0xC340C033, true);
	injector::WriteMemory<DWORD>(imageBase + 0x76870, 0xC340C033, true);
	
	// Patch test menu etc
	injector::WriteMemoryRaw(imageBase + 0x544BB0, (char*)".\\%c%s.dat", 12, true);

	hookPort = "COM3";
}, GameID::MachStorm);
#endif