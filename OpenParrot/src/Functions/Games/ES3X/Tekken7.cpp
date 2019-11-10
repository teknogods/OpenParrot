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

		// Force Offline mode
		//safeJMP(imageBase + 0xACB190, ReturnFalse);
	}, GameID::Tekken7);
#endif