#include <StdInc.h>
#include "Utility/InitFunction.h"
#include "Functions/Global.h"
#include "MinHook.h"
#include <Utility/Hooking.Patterns.h>
#include <thread>
#ifdef _M_AMD64
#pragma optimize("", off)
#pragma comment(lib, "Ws2_32.lib")

extern LPCSTR hookPort;

static InitFunction Tekken7Func([]()
	{
		hookPort = ("COM3");
		uintptr_t imageBase = (uintptr_t)GetModuleHandleA(0);

		// Skip all errors
		injector::MakeNOP(imageBase + 0x25E791, 5);

		// First Check Error
		injector::MakeNOP(imageBase + 0x268E00, 5);

		// Windowed
		if (ToBool(config["General"]["Windowed"]))
		{
			// TODO: DOCUMENT PATCHES
			injector::WriteMemory<LONGLONG>(imageBase + 0x115E757, 0xF633C1FFC1FFC933, true);
			injector::WriteMemory<DWORD>(imageBase + 0x115E757 + 0x08, 0xC6FFC6FF, true);
		}
	}, GameID::TER);
#endif