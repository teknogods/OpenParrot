#include <StdInc.h>
#include "Utility/GameDetect.h"
#include "Utility/InitFunction.h"
#include "Functions/Nesica_Libs/CryptoPipe.h"
#include "Functions/Nesica_Libs/FastIoEmu.h"
#include "Functions/Nesica_Libs/RfidEmu.h"
#include "Functions/Nesica_Libs/NesysEmu.h"
#include "Functions/Nesica_Libs/RegHooks.h"

static InitFunction initFunction([]()
{
	init_FastIoEmu();
	init_RfidEmu();
	init_RegHooks();
	if(GameDetect::enableNesysEmu)
		init_NesysEmu();
#if _M_IX86
	init_CryptoPipe(GameDetect::NesicaKey);
#endif
}, GameID::Nesica);

static int ReturnTrue()
{
	return 1;
}

static InitFunction initFunction_GC2([]()
{
	uintptr_t imageBase = (uintptr_t)GetModuleHandleA(0);
	init_FastIoEmu();
	init_RfidEmu();
	init_RegHooks();
	init_NesysEmu();

	// C:\\TypeXZEROTemp.dat check
	safeJMP(imageBase + 0xF81B0, ReturnTrue);
#if _M_IX86
	init_CryptoPipe(GameDetect::NesicaKey);
#endif
}, GameID::GrooveCoaster2);

static InitFunction initFunction_PB([]()
{
	uintptr_t imageBase = (uintptr_t)GetModuleHandleA(0);
	init_FastIoEmu();
	init_RfidEmu();
	init_RegHooks();
	init_NesysEmu();

	// Nesys error workaround
	injector::WriteMemoryRaw(imageBase + 0xA77B, "\xA3\xEC\x0D\x4F\x00\x90", 6, true);
}, GameID::PuzzleBobble);

static InitFunction initFunction_MB([]()
{
	uintptr_t imageBase = (uintptr_t)GetModuleHandleA(0);
	init_FastIoEmu();
	init_RfidEmu();
	init_RegHooks();
	init_NesysEmu();
	init_CryptoPipe(GameDetect::NesicaKey);

	// Skip Initilization wait time.
	injector::MakeNOP(imageBase + 0x56B21, 2);
}, GameID::MagicalBeat);

static InitFunction initFunction_SOR([]()
{
	uintptr_t imageBase = (uintptr_t)GetModuleHandleA(0);
	init_FastIoEmu();
	init_RfidEmu();
	// TODO: DOCUMENT PATCHES
	safeJMP(imageBase + 0xFA350, ReturnTrue);
	safeJMP(imageBase + 0xF8FC0, ReturnTrue);
	// Patch data dir to game dir pls D:/ -> .\\
	//
	injector::WriteMemory<DWORD>(imageBase + 0x21B9AC0, 0x2F002E002E, true);
	init_NesysEmu();
	if (ToBool(config["General"]["Windowed"]))
	{
		// TODO: DOCUMENT PATCHES
		injector::WriteMemory<LONGLONG>(imageBase + 0xFF703C, 0xF633C1FFC1FFC933, true);
		injector::WriteMemory<DWORD>(imageBase + 0xFF703C+0x08, 0xC6FFC6FF, true);
	}
}, GameID::SchoolOfRagnarok);