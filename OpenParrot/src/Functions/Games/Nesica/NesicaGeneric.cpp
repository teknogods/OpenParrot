#include <StdInc.h>
#include "Utility/GameDetect.h"
#include "Utility/InitFunction.h"
#include "Functions/Nesica_Libs/CryptoPipe.h"
#include "Functions/Nesica_Libs/FastIoEmu.h"
#include "Functions/Nesica_Libs/RfidEmu.h"
#include "Functions/Nesica_Libs/NesysEmu.h"

static InitFunction initFunction([]()
{
	init_FastIoEmu();
	init_RfidEmu(X2Type::RFID);
	init_NesysEmu();
#if _M_IX86
	if(GameDetect::NesicaKey != NesicaKey::None)
		init_CryptoPipe(GameDetect::NesicaKey);
#endif
}, GameID::Nesica);

static int ReturnTrue()
{
	return 1;
}

static InitFunction initFunction_SOR([]()
{
	uintptr_t imageBase = (uintptr_t)GetModuleHandleA(0);
	init_FastIoEmu();
	init_RfidEmu(X2Type::RFID);
	// TODO: DOCUMENTS PATCHES
	safeJMP(imageBase + 0xFA350, ReturnTrue);
	safeJMP(imageBase + 0xF8FC0, ReturnTrue);
	// Patch data dir to game dir pls D:/ -> .\\
	//
	injector::WriteMemory<DWORD>(imageBase + 0x21B9AC0, 0x2F002E002E, true);
	init_NesysEmu();
	if (ToBool(config["General"]["Windowed"]))
	{
		injector::WriteMemory<LONGLONG>(imageBase + 0xFF703C, 0xF633C1FFC1FFC933, true);
		injector::WriteMemory<DWORD>(imageBase + 0xFF703C+0x08, 0xC6FFC6FF, true);
	}
}, GameID::SchoolOfRagnarok);