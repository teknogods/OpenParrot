#include <StdInc.h>
#include "Utility/GameDetect.h"
#include "Utility/InitFunction.h"
#include "Functions/Global.h"
#include "Utility/Helper.h"

bool PauseGameFixInit;
static DWORD TimerValue;

void PauseGameFixes(Helpers* helpers)
{
	if (GameDetect::currentGame == GameID::SR3)
	{
		if (!PauseGameFixInit)
		{
			PauseGameFixInit = true;
			TimerValue = helpers->ReadIntPtr(0x9C4A07, false);
		}
		helpers->WriteIntPtr(0x9C4A07, TimerValue, false);
	}
}