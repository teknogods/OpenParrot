#if _M_IX86
#include <StdInc.h>
#include "Utility/InitFunction.h"
#include "Functions/Global.h"

typedef unsigned char U8;
typedef unsigned int U32;
typedef uint16_t U16;

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

	if (ToBool(config["General"]["Dome Hack"]))
	{
		U16 *buffer = new U16[1920 * 1080 * 2];

		for (int y = 0; y < 1080; y++)
		{
			for (int x = 0; x < 1920; x++)
			{
				float d = 0.5f;
				float xf = x / 1920.0f;
				float yf = y / 1080.0f;
				xf = 2.0f * xf - 1.0f;
				yf = 2.0f * yf - 1.0f;
				if (0.0f > xf)
				{
					float k = 0.95f * -xf / (-xf + d);
					xf = -k;
					yf = yf * (1.0f - k);
				}
				else
				{
					float k = 0.95f * xf / (xf + d);
					xf = k;
					yf = yf * (1.0f - k);
				}
				xf = 0.5f * xf + 0.5f;
				yf = -0.5f * yf - 0.5f;
				buffer[(y * 1920 + x) * 2] = 65535 * xf;
				buffer[(y * 1920 + x) * 2 + 1] = 65535 * yf;
			}
		}

		injector::WriteMemory<U8>(imageBase + 0x003D2FEE, 0x45, true);
		injector::WriteMemory<U8>(imageBase + 0x003D3637, 0xB8, true);
		injector::WriteMemory<U32>(imageBase + 0x003D3638, (U32)buffer, true);
		injector::WriteMemory<U32>(imageBase + 0x003D363C, 0x0C244489, true);
	}

	hookPort = "COM3";
}, GameID::MachStorm);
#endif