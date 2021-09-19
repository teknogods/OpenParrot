#include <StdInc.h>
#include "Utility/GameDetect.h"
#include "Utility/InitFunction.h"
#include "Functions/Global.h"
#include "Utility/Helper.h"

static DWORD imageBase;
static bool PauseGameFixInit;
static bool PerformSingleFix;

void PauseGameFixes(Helpers* helpers)
{
	if (!PauseGameFixInit)
	{
		PauseGameFixInit = true;
		imageBase = (DWORD)GetModuleHandleA(0);
	}

	if (!PerformSingleFix)
	{
		PerformSingleFix = true;

		if (GameDetect::currentGame == GameID::SR3)
			injector::MakeNOP(0x58CE6A, 6);
	}
}

void ResetPauseGameFixes(Helpers* helpers)
{
	PerformSingleFix = false;

	if (GameDetect::currentGame == GameID::SR3)
		injector::WriteMemoryRaw(imageBase + 0x18CE6A, "\x01\x05\x08\x4A\x9C\x00", 6, true);
}