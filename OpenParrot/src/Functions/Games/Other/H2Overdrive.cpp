#include <StdInc.h>
#include "Utility/InitFunction.h"
#include "Functions/Global.h"
#include "Utility\Hooking.Patterns.h"
#include <Xinput.h>
#include <math.h>
#include <dinput.h>

#pragma comment(lib, "Ws2_32.lib")
#if _M_IX86
typedef unsigned int U32;
typedef unsigned char U8;

DWORD BaseAddress10 = 0x00400000;
HWND hWndRT10 = 0;

static bool button1pressed = false;
static bool button2pressed = false;
static bool button3pressed = false;
static bool button4pressed = false;
static bool STARTpressed = false;

// controls
extern int* ffbOffset;
extern int* ffbOffset2;
extern int* ffbOffset3;
extern int* ffbOffset4;

CREATE_PROCESS_DEBUG_INFO gProcDbgInfo = { 0 };

LPVOID GetTranslatedOffset(int offset)
{
	return reinterpret_cast<LPVOID>((int)gProcDbgInfo.lpBaseOfImage + offset);
}

DWORD WINAPI InputRT10(LPVOID lpParam)
{
	int deltaTimer = 16;

	while (true)
	{

		// ESCAPE QUITS GAME 
		if (GetAsyncKeyState(VK_ESCAPE) & 0x8000)
		{
			exit(0);
		}

		// REAL NUMERIC KEYPAD
		int keys[] = { VK_NUMPAD1, VK_NUMPAD2, VK_NUMPAD3, VK_NUMPAD4, VK_NUMPAD5, VK_NUMPAD6, VK_NUMPAD7, VK_NUMPAD8, VK_NUMPAD9, VK_MULTIPLY, VK_NUMPAD0, VK_DIVIDE };
		for (int i = 0; i < sizeof(keys) / sizeof(int); i++)
		{
			SHORT res = GetAsyncKeyState(keys[i]);
			if (res)
			{
				injector::WriteMemory<INT32>((0x2BAA04 + i * 4 + BaseAddress10), 1, true);
				if (res & 1)
				{
					injector::WriteMemory<INT32>((0x392BA8 + BaseAddress10), 1, true);
					injector::WriteMemory<INT32>((0x392B70 + 0x10 + BaseAddress10), i, true);
				}
			}
		}
		// FAKE KEYPAD FROM NUMBERS
		int fakekeys[] = { 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, VK_RETURN, 0x30, VK_BACK };
		for (int i = 0; i < sizeof(fakekeys) / sizeof(int); i++)
		{
			SHORT res2 = GetAsyncKeyState(fakekeys[i]);
			if (res2)
			{
				injector::WriteMemory<INT32>((0x2BAA04 + i * 4 + BaseAddress10), 1, true);
				if (res2 & 1)
				{
					injector::WriteMemory<INT32>((0x392BA8 + BaseAddress10), 1, true);
					injector::WriteMemory<INT32>((0x392B70 + 0x10 + BaseAddress10), i, true);
				}
			}
		}

		// buttons see bitwise values in TPui//RawThrills.cs
		// START+NITRO
		if (*ffbOffset & 0x08)
		{
			if (STARTpressed == false)
			{
				injector::WriteMemory<INT32>((0x398CB0 + BaseAddress10), 1, true);
				injector::WriteMemory<INT32>((0x398CB4 + BaseAddress10), 0, true);
				STARTpressed = true;
			}
		}
		else
		{
			if (STARTpressed == true)
			{
				injector::WriteMemory<INT32>((0x398CB0 + BaseAddress10), 0, true);
				injector::WriteMemory<INT32>((0x398CB4 + BaseAddress10), 1, true);
				STARTpressed = false;
			}
		}

		// BUTTON 1 / START+NITRO
		if (*ffbOffset & 0x100)
		{
			if (button1pressed == false)
			{
				injector::WriteMemory<INT32>((0x398CB0 + BaseAddress10), 1, true);
				injector::WriteMemory<INT32>((0x398CB4 + BaseAddress10), 0, true);
				button1pressed = true;
			}
		}
		else
		{
			if (button1pressed == true)
			{
				injector::WriteMemory<INT32>((0x398CB0 + BaseAddress10), 0, true);
				injector::WriteMemory<INT32>((0x398CB4 + BaseAddress10), 1, true);
				button1pressed = false;
			}
		}
		
		// BUTTON 3/ VIEW
		int ptr = *(DWORD*)(0x38C2D0 + BaseAddress10);
		if (*ffbOffset & 0x400)
		{
			if (button3pressed == false)
			{
				injector::WriteMemory<INT32>((0x398CB8 + BaseAddress10), 1, true);
				injector::WriteMemory<INT32>((0x398CBC + BaseAddress10), 0, true);
				button3pressed = true;

				if (0 != ptr)
				{
					injector::WriteMemory<INT32>((ptr + 0x840), 1, true);
				}
			}
		}
		else
		{
			if (button3pressed == true)
			{
				injector::WriteMemory<INT32>((0x398CB8 + BaseAddress10), 0, true);
				injector::WriteMemory<INT32>((0x398CBC + BaseAddress10), 1, true);
				button3pressed = false;
			}
		}

		// WHEEL
		int iWheel = (((float)* ffbOffset2) - 128);
		float wheel = (iWheel * 0.0078125f);
		injector::WriteMemory<float>((0x398CC8 + BaseAddress10), wheel * 0.5f + 0.5f, true); // MENU WHEEL
		injector::WriteMemory<float>((0x29D4D8 + BaseAddress10), wheel, true); // GAME WHEEL
		//// GAS
		float gas = (float)* ffbOffset3 / 255.0f;
		injector::WriteMemory<float>((0x398CD0 + BaseAddress10), gas, true);

		//DEBUG//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		//	info(true, "test value %08X", ptr);
		//DEBUG//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

		Sleep(deltaTimer);
	}

	return 0;
}


static InitFunction H2OverdriveFunc([]()
	{
		// PATCHING EXE AT RUNTIME (reboots, network, filepath, config, CRC...
		injector::WriteMemoryRaw((0x24713C + BaseAddress10), "\x48\x32\x4F\x76\x65\x72\x64\x72\x69\x76\x65", 11, true);
		injector::WriteMemoryRaw((0x004DB1 + BaseAddress10), "\xEB", 1, true);
		injector::WriteMemoryRaw((0x0B423B + BaseAddress10), "\xEB", 1, true);
		injector::WriteMemoryRaw((0x101B70 + BaseAddress10), "\xEB", 1, true);
		injector::WriteMemoryRaw((0x12AB6F + BaseAddress10), "\xEB", 1, true);
		injector::WriteMemoryRaw((0x19C240 + BaseAddress10), "\xC2\x04\x00\x90\x90\x90", 6, true);
		injector::WriteMemoryRaw((0x251744 + BaseAddress10), "\x2E\x5C\x00", 3, true);

	//CONTROLS PATCH
		// BOOST BUTTON
		injector::MakeNOP((0x151594 + BaseAddress10), 5, true); // PATCH BUTTON IN MENU
		injector::MakeNOP((0x1515BB + BaseAddress10), 5, true);
		U8 patchBoostButton[] = { 0xA1, 0xB0, 0x8C, 0x79, 0x00, 0xC3 };
		LPVOID* ptr3 = (LPVOID*)& patchBoostButton[1];
		*ptr3 = GetTranslatedOffset(0x398CB0 + BaseAddress10);
		injector::WriteMemoryRaw((0x109BA3 + BaseAddress10), patchBoostButton, sizeof(patchBoostButton), true); // PATCH BOOST BUTTON IN GAME
		injector::WriteMemory<INT32>((0x257B94 + 0x2C + BaseAddress10), (int)GetTranslatedOffset(0x109BA3 + BaseAddress10), true); // PATCH JUMP TABLE FOR BOOST BUTTON
		// VIEW BUTTON
		injector::MakeNOP((0x151599 + BaseAddress10), 6, true); // PATCH VIEW BUTTON IN MENU
		injector::MakeNOP((0x1515DC + BaseAddress10), 5, true);
		U8 patchViewButton[] = { 0xA1, 0xB8, 0x8C, 0x79, 0x00, 0xC3 };
		LPVOID* ptr4 = (LPVOID*)& patchViewButton[1];
		*ptr4 = GetTranslatedOffset(0x398CB8 + BaseAddress10);
		injector::WriteMemoryRaw((0x109BA9 + BaseAddress10), patchViewButton, sizeof(patchViewButton), true); // PATCH VIEW BUTTON IN GAME
		injector::WriteMemory<INT32>((0x257B94 + 0x44 + BaseAddress10), (int)GetTranslatedOffset(0x109BA9 + BaseAddress10), true); // PATCH JUMP TABLE FOR VIEW BUTTON
		// WHEEL AND PEDAL
		injector::MakeNOP((0x151698 + BaseAddress10), 10, true); // PATCH WHEEL WRITE IN MENU
		U8 patchPedal[] = { 0xD9, 0x05, 0xD0, 0x8C, 0x79, 0x00, 0xC2, 0x04, 0x00 };
		LPVOID* ptrPedal = (LPVOID*)& patchPedal[2];
		*ptrPedal = GetTranslatedOffset(0x00398CD0 + BaseAddress10);
		injector::WriteMemoryRaw((0x000859B7 + BaseAddress10), patchPedal, sizeof(patchPedal), true); // PATCH PEDAL
		injector::WriteMemory<INT32>((0x257B94 + 0x20 + BaseAddress10), (int)GetTranslatedOffset(0x000859B7 + BaseAddress10), true); // PATCH JUMP TABLE FOR GAS
		U8 patchWheel[] = { 0xD9, 0x05, 0xD8, 0xD4, 0x69, 0x00, 0xC2, 0x04, 0x00 };
		LPVOID* ptrWheel = (LPVOID*)& patchWheel[2];
		*ptrWheel = GetTranslatedOffset(0x0029D4D8 + BaseAddress10);
		injector::WriteMemoryRaw((0x000859C7 + BaseAddress10), patchWheel, sizeof(patchWheel), true); // PATCH WHEEL
		injector::WriteMemory<INT32>((0x257B94 + 0x1C + BaseAddress10), (int)GetTranslatedOffset(0x000859C7 + BaseAddress10), true); // PATCH JUMP TABLE FOR WHEEL

		//TEST REMAP to DELETE
		injector::WriteMemoryRaw((0x146E68 + BaseAddress10), "\x6A\x2E", 2, true);
		//SERVICE REMAP to INSERT
		injector::WriteMemoryRaw((0x146EB2 + BaseAddress10), "\x6A\x2D", 2, true);
		//VOL+- REMAP to UP/DOWN
		injector::WriteMemoryRaw((0x146F03 + BaseAddress10), "\x6A\x26", 2, true);
		injector::WriteMemoryRaw((0x146F54 + BaseAddress10), "\x6A\x28", 2, true);
		//ENABLE KEYPAD
		injector::WriteMemoryRaw((0x186302 + BaseAddress10), "\xEB", 1, true);
		//KEYPAD HACK
		U8 patchKeypadButton[] = { 0x8B, 0x44, 0x24, 0x04, 0x8B, 0x04, 0x85, 0x04, 0xAA, 0x6B, 0x00 ,0xC2, 0x04, 0x00 };
		injector::WriteMemoryRaw((0x0023CE37 + BaseAddress10), patchKeypadButton, sizeof(patchKeypadButton), true);
		injector::WriteMemory<INT32>((0x257B94 + 0x78 + BaseAddress10), (int)GetTranslatedOffset(0x23CE37 + BaseAddress10), true); // PATCH JUMP TABLE FOR KEYPAD BUTTON

		CreateThread(NULL, 0, InputRT10, NULL, 0, NULL);

	}, GameID::H2Overdrive);
#endif