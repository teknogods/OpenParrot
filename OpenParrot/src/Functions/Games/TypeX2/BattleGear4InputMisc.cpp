#include <StdInc.h>
#include "Utility/GameDetect.h"
#include "Utility/InitFunction.h"
#include "Functions/Global.h"
#include "Utility/Helper.h"

bool BG4EnableTracks;

static bool _init;
static bool _stopPausing;

void BG4General(Helpers* helpers)
{
	if (!_init)
	{
		UINT8 WaitForAttract = helpers->ReadByte(0x42D964, true);

		if (WaitForAttract)
		{
			_init = true;

			if (BG4EnableTracks)
				helpers->WriteIntPtr(0x4165C0, 0x3FFFFFFF, true);
		}
	}

	UINT8 ScreenVal = helpers->ReadByte(0x4E3668, true);

	if (ScreenVal == 0x08)
	{
		if (!_stopPausing)
			_stopPausing = true;
	}

	if (_stopPausing)
		helpers->WriteByte(0x42D29C, 0x01, true);
}