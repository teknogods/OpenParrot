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


// Lan fix funcs, fixes not yet committed.
static int ReturnFalse()
{
	return 0;
}

static int ReturnTrue()
{
	return 1;
}

static int ReturnsTwo()
{
	return 2;
}

static InitFunction Tekken7Func([]()
	{
		hookPort = ("COM3");
		uintptr_t imageBase = (uintptr_t)GetModuleHandleA(0);

		// Force Offline mode
		safeJMP(imageBase + 0x204AF0, ReturnTrue);
	
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

static InitFunction Tekken7Update00Func([]()
	{
		hookPort = "COM3";
		uintptr_t imageBase = (uintptr_t)GetModuleHandleA(0);

		// Skip all errors
		injector::MakeNOP(imageBase + 0x16D3D1, 5);

		// Force Offline mode
		safeJMP(imageBase + 0xACB190, ReturnTrue);

		// First Check Error
		injector::MakeNOP(imageBase + 0x17B400, 5);

		// workaround string out of range crash
		// still wrong, but at least no crash
		injector::WriteMemory<BYTE>(imageBase + 0x102C34, 0xEB, true);

		// Windowed
		if (ToBool(config["General"]["Windowed"]))
		{
			// doesn't seem to work?
			injector::WriteMemory<LONGLONG>(imageBase + 0xBCB2D3, 0xF633C1FFC1FFC933, true);
			injector::WriteMemory<DWORD>(imageBase + 0xBCB2D3 + 0x08, 0xC6FFC6FF, true);
		}

	}, GameID::Tekken7Update00);

static InitFunction Tekken7Upd12Func([]()
	{
		hookPort = "COM3";
		uintptr_t imageBase = (uintptr_t)GetModuleHandleA(0);

		// Skip all errors
		injector::MakeNOP(imageBase + 0x16BC41, 5);

		// Force Offline mode
		safeJMP(imageBase + 0x147380, ReturnTrue);

		// First Check Error
		injector::MakeNOP(imageBase + 0x179050, 5);

		// Windowed
		if (ToBool(config["General"]["Windowed"]))
		{
			injector::WriteMemory<LONGLONG>(imageBase + 0xBF0F33, 0xF633C1FFC1FFC933, true);
			injector::WriteMemory<DWORD>(imageBase + 0xBF0F33 + 0x08, 0xC6FFC6FF, true);
		}

	}, GameID::Tekken7Update12);
#endif