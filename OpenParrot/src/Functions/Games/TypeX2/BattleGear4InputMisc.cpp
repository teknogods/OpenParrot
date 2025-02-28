#include <StdInc.h>
#include "Utility/GameDetect.h"
#include "Utility/InitFunction.h"
#include "Functions/Global.h"
#include "Utility/Helper.h"

static bool init;
static bool StopPausing;
static bool BG4EnableTracks;

void BG4General(Helpers* helpers)
{
	if (!init)
	{
		UINT8 WaitForAttract = helpers->ReadByte(0x42D964, true);

		if (WaitForAttract)
		{
			init = true;

			if (BG4EnableTracks)
				helpers->WriteIntPtr(0x4165C0, 0x3FFFFFFF, true);
		}
	}

	UINT8 ScreenVal = helpers->ReadByte(0x4E3668, true);

	if (ScreenVal == 0x08)
	{
		if (!StopPausing)
			StopPausing = true;
	}

	if (StopPausing)
		helpers->WriteByte(0x42D29C, 0x01, true);
}