#include <StdInc.h>
#include "Utility/GameDetect.h"
#include "Utility/InitFunction.h"
#include "Functions/Global.h"
#include "Utility/Helper.h"

void KOFSkyStageInputs(Helpers* helpers)
{
	UINT8 JVSInput1 = helpers->ReadByte(0x2DF304, true);
	UINT8 JVSInput1A = helpers->ReadByte(0x2DF305, true);
	UINT8 Joystick1Up = helpers->ReadByte(0x37085A, true);
	UINT8 Joystick1Down = helpers->ReadByte(0x370860, true);
	UINT8 Joystick1Left = helpers->ReadByte(0x370866, true);
	UINT8 Joystick1Right = helpers->ReadByte(0x37086C, true);
	UINT8 Joystick1Start = helpers->ReadByte(0x37088A, true);
	UINT8 Joystick1Button1 = helpers->ReadByte(0x370890, true);
	UINT8 Joystick1Button2 = helpers->ReadByte(0x370896, true);
	UINT8 Joystick1Button3 = helpers->ReadByte(0x37089C, true);
	UINT8 JVSInput2 = helpers->ReadByte(0x2DF306, true);
	UINT8 JVSInput2A = helpers->ReadByte(0x2DF307, true);
	UINT8 Joystick2Up = helpers->ReadByte(0x370950, true);
	UINT8 Joystick2Down = helpers->ReadByte(0x370956, true);
	UINT8 Joystick2Left = helpers->ReadByte(0x37095C, true);
	UINT8 Joystick2Right = helpers->ReadByte(0x370962, true);
	UINT8 Joystick2Start = helpers->ReadByte(0x370980, true);
	UINT8 Joystick2Button1 = helpers->ReadByte(0x370986, true);
	UINT8 Joystick2Button2 = helpers->ReadByte(0x37098C, true);
	UINT8 Joystick2Button3 = helpers->ReadByte(0x370992, true);

	if (!(JVSInput1 & 0x20))
	{
		if (Joystick1Up == 0x01)
		{
			helpers->WriteByte(0x37085A, 0x00, true);
		}
	}

	if (!(JVSInput1 & 0x10))
	{
		if (Joystick1Down == 0x01)
		{
			helpers->WriteByte(0x370860, 0x00, true);
		}
	}

	if (!(JVSInput1 & 0x08))
	{
		if (Joystick1Left == 0x01)
		{
			helpers->WriteByte(0x370866, 0x00, true);
		}
	}

	if (!(JVSInput1 & 0x04))
	{
		if (Joystick1Right == 0x01)
		{
			helpers->WriteByte(0x37086C, 0x00, true);
		}
	}

	if (!(JVSInput1 & 0x80))
	{
		if (Joystick1Start == 0x01)
		{
			helpers->WriteByte(0x37088A, 0x00, true);
		}
	}

	if (!(JVSInput1 & 0x02))
	{
		if (Joystick1Button1 == 0x01)
		{
			helpers->WriteByte(0x370890, 0x00, true);
			helpers->WriteByte(0x370872, 0x00, true);
		}
	}

	if (!(JVSInput1 & 0x01))
	{
		if (Joystick1Button2 == 0x01)
		{
			helpers->WriteByte(0x370896, 0x00, true);
			helpers->WriteByte(0x370878, 0x00, true);
		}
	}

	if (!(JVSInput1A & 0x80))
	{
		if (Joystick1Button3 == 0x01)
		{
			helpers->WriteByte(0x37089C, 0x00, true);
			helpers->WriteByte(0x370884, 0x00, true);
		}
	}

	if (!(JVSInput2 & 0x20))
	{
		if (Joystick2Up == 0x01)
		{
			helpers->WriteByte(0x370950, 0x00, true);
		}
	}

	if (!(JVSInput2 & 0x10))
	{
		if (Joystick2Down == 0x01)
		{
			helpers->WriteByte(0x370956, 0x00, true);
		}
	}

	if (!(JVSInput2 & 0x08))
	{
		if (Joystick2Left == 0x01)
		{
			helpers->WriteByte(0x37095C, 0x00, true);
		}
	}

	if (!(JVSInput2 & 0x04))
	{
		if (Joystick2Right == 0x01)
		{
			helpers->WriteByte(0x370962, 0x00, true);
		}
	}

	if (!(JVSInput2 & 0x80))
	{
		if (Joystick2Start == 0x01)
		{
			helpers->WriteByte(0x370980, 0x00, true);
		}
	}

	if (!(JVSInput2 & 0x02))
	{
		if (Joystick2Button1 == 0x01)
		{
			helpers->WriteByte(0x370986, 0x00, true);
			helpers->WriteByte(0x370968, 0x00, true);
		}
	}

	if (!(JVSInput2 & 0x01))
	{
		if (Joystick2Button2 == 0x01)
		{
			helpers->WriteByte(0x37098C, 0x00, true);
			helpers->WriteByte(0x37096E, 0x00, true);
		}
	}

	if (!(JVSInput2A & 0x80))
	{
		if (Joystick2Button3 == 0x01)
		{
			helpers->WriteByte(0x370992, 0x00, true);
			helpers->WriteByte(0x37097A, 0x00, true);
		}
	}
}