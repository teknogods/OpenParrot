#include <StdInc.h>
#include "Utility/InitFunction.h"
#include "Functions/Global.h"
#include "Utility\Hooking.Patterns.h"
#include <math.h>

#pragma comment(lib, "Ws2_32.lib")

static const char* EnterYourName = ("ENTER YOUR NAME!");
static const char* HighScoreName = ("PLAYER BEST TIMES");

typedef unsigned int U32;
typedef unsigned char U8;

DWORD BaseAddress10 = 0x00400000;
HWND hWndRT10 = 0;

static bool button1pressed = false;
static bool button1pressed2 = false;
static bool button2pressed = false;
static bool button3pressed = false;
static bool button4pressed = false;
static bool button5pressed = false;
static bool button6pressed = false;
static bool STARTpressed = false;
static bool STARTpressed2 = false;
static bool NameChoosing = false;
static bool NameViewButton = false;
static int ViewCount;
static int View2Count;

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

BOOL(__stdcall* original_CreateWindowExA10)(DWORD dwExStyle, LPCSTR lpClassName, LPCSTR lpWindowName, DWORD dwStyle, int X, int Y, int nWidth, int nHeight, HWND hWndParent, HMENU hMenu, HINSTANCE hInstance, LPVOID lpParam);

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

		// FIX FOR NAME CHOOSING
		if (strcmp((char*)0x3CB4F8 + BaseAddress10, EnterYourName) == 0 || strcmp((char*)0x3CB4FE + BaseAddress10, HighScoreName) == 0 || strcmp((char*)0x3CB4FF + BaseAddress10, HighScoreName) == 0)
			NameChoosing = true;
		else
			NameChoosing = false;

		BYTE ViewName = *(BYTE*)(0x398CB8 + BaseAddress10);

		if (NameChoosing && NameViewButton)
		{
			if (ViewName)
			{
				++ViewCount;
				if (ViewCount == 2)
				{
					injector::WriteMemory<INT32>((0x398CB8 + BaseAddress10), 0, true);
					ViewCount = 0;
				}

			}
			else
				if (ViewCount > 0)
					ViewCount = 0;
		}

		if (*ffbOffset & 0x200)
		{
			if (!NameChoosing && !ViewName)
			{
				++View2Count;
				if (View2Count == 2)
				{
					injector::WriteMemory<INT32>((0x398CB8 + BaseAddress10), 1, true);
					View2Count = 0;
				}
			}
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
			else
			{
				if (NameChoosing && !STARTpressed2)
				{
					injector::WriteMemory<INT32>((0x398CB0 + BaseAddress10), 0, true);
					injector::WriteMemory<INT32>((0x398CB4 + BaseAddress10), 1, true);
					STARTpressed2 = true;
				}
			}
		}
		else
		{
			if (STARTpressed == true)
			{
				injector::WriteMemory<INT32>((0x398CB0 + BaseAddress10), 0, true);
				injector::WriteMemory<INT32>((0x398CB4 + BaseAddress10), 1, true);
				STARTpressed = false;
				STARTpressed2 = false;
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
			else
			{
				if (NameChoosing && !button1pressed2)
				{
					injector::WriteMemory<INT32>((0x398CB0 + BaseAddress10), 0, true);
					injector::WriteMemory<INT32>((0x398CB4 + BaseAddress10), 1, true);
					button1pressed2 = true;
				}
			}
		}
		else
		{
			if (button1pressed == true)
			{
				injector::WriteMemory<INT32>((0x398CB0 + BaseAddress10), 0, true);
				injector::WriteMemory<INT32>((0x398CB4 + BaseAddress10), 1, true);
				button1pressed = false;
				button1pressed2 = false;
			}
		}
		
		// BUTTON 2/ VIEW
		int ptr = *(DWORD*)(0x38C2D0 + BaseAddress10);
		if (*ffbOffset & 0x200)
		{
			if (button2pressed == false)
			{
				injector::WriteMemory<INT32>((0x398CBC + BaseAddress10), 1, true);
				button2pressed = true;

				if (0 != ptr)
				{
					injector::WriteMemory<INT32>((ptr + 0x840), 1, true);
				}

				if (NameChoosing && !NameViewButton)
				{
					NameViewButton = true;
					injector::WriteMemory<INT32>((0x398CB8 + BaseAddress10), 1, true);
				}
			}
			else if (button2pressed == true)
			{
				injector::WriteMemory<INT32>((0x398CBC + BaseAddress10), 0, true);
			}
		}
		else
		{
			if (button2pressed == true)
			{
				injector::WriteMemory<INT32>((0x398CBC + BaseAddress10), 0, true);
				button2pressed = false;

				if (0 != ptr)
				{
					injector::WriteMemory<INT32>((ptr + 0x840), 0, true);
				}

				if (NameViewButton)
				{
					NameViewButton = false;
				}

				if (!NameChoosing && ViewName)
					injector::WriteMemory<INT32>((0x398CB8 + BaseAddress10), 0, true);
			}
		}

		// BUTTON 3/ TEST
		if (*ffbOffset & 0x400)
		{
			if (button3pressed == false)
			{
				keybd_event(0x2E, MapVirtualKey(0x2E, MAPVK_VK_TO_VSC), 0, 0);
				button3pressed = true;
			}
		}
		else
		{
			if (button3pressed == true)
			{
				keybd_event(0x2E, MapVirtualKey(0x2E, MAPVK_VK_TO_VSC), KEYEVENTF_KEYUP, 0);
				button3pressed = false;
			}
		}
		// BUTTON 4/ SERVICE
		if (*ffbOffset & 0x800)
		{
			if (button4pressed == false)
			{
				keybd_event(0x2D, MapVirtualKey(0x2D, MAPVK_VK_TO_VSC), 0, 0);
				button4pressed = true;
			}
		}
		else
		{
			if (button4pressed == true)
			{
				keybd_event(0x2D, MapVirtualKey(0x2D, MAPVK_VK_TO_VSC), KEYEVENTF_KEYUP, 0);
				button4pressed = false;
			}
		}
		// BUTTON 5/ VOL/MENU UP
		if (*ffbOffset & 0x10)
		{
			if (button5pressed == false)
			{
				keybd_event(0x21, MapVirtualKey(0x21, MAPVK_VK_TO_VSC), 0, 0);
				button5pressed = true;
			}
		}
		else
		{
			if (button5pressed == true)
			{
				keybd_event(0x21, MapVirtualKey(0x21, MAPVK_VK_TO_VSC), KEYEVENTF_KEYUP, 0);
				button5pressed = false;
			}
		}
		// BUTTON 6/ VOL/MENU DOWN
		if (*ffbOffset & 0x20)
		{
			if (button6pressed == false)
			{
				keybd_event(0x22, MapVirtualKey(0x22, MAPVK_VK_TO_VSC), 0, 0);
				button6pressed = true;
			}
		}
		else
		{
			if (button6pressed == true)
			{
				keybd_event(0x22, MapVirtualKey(0x22, MAPVK_VK_TO_VSC), KEYEVENTF_KEYUP, 0);
				button6pressed = false;
			}
		}

		// WHEEL
		int iWheel = (((float)* ffbOffset2) - 128);
		float wheel = (iWheel * 0.0078125f);
		injector::WriteMemory<float>((0x398CC8 + BaseAddress10), wheel * 0.5f + 0.5f, true); // MENU WHEEL
		injector::WriteMemory<float>((0x3D2CD4 + BaseAddress10), wheel, true); // GAME WHEEL
		//// GAS
		float gas = (float)* ffbOffset3 / 255.0f;
		float brake = (float)*ffbOffset4 / 255.0f;
	//	injector::WriteMemory<float>((0x398CD0 + BaseAddress10), gas, true);
		injector::WriteMemory<float>((0x398CD0 + BaseAddress10), gas - brake, true);

		//DEBUG//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		//	info(true, "test value %f", wheel);
		//DEBUG//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

		Sleep(deltaTimer);
	}

	return 0;
}

DWORD WINAPI CreateWindowExART10(DWORD dwExStyle, LPCSTR lpClassName, LPCSTR lpWindowName, DWORD dwStyle, int X, int Y, int nWidth, int nHeight, HWND hWndParent, HMENU hMenu, HINSTANCE hInstance, LPVOID lpParam)
{
	return original_CreateWindowExA10(dwExStyle, lpClassName, lpWindowName, 0x14CF0000, X, Y, nWidth, nHeight, hWndParent, hMenu, hInstance, lpParam);
}

static InitFunction H2OverdriveFunc([]()
	{
		// PATCHING EXE AT RUNTIME (reboots, network, filepath, config, CRC...
	//	injector::MakeNOP((0x0DBE3A + BaseAddress10), 2, true); // force DEV SETTINGS BOOT DISPLAY
		injector::WriteMemoryRaw((0x24713C + BaseAddress10), "\x48\x32\x4F\x76\x65\x72\x64\x72\x69\x76\x65", 11, true);
		injector::WriteMemoryRaw((0x004DB1 + BaseAddress10), "\xEB", 1, true);
		injector::WriteMemoryRaw((0x0B423B + BaseAddress10), "\xEB", 1, true); // IO-Board check
		injector::WriteMemoryRaw((0x101B70 + BaseAddress10), "\xEB", 1, true); // ATTRACT check
		injector::WriteMemoryRaw((0x12AB6F + BaseAddress10), "\xEB", 1, true); 
		injector::WriteMemoryRaw((0x19C240 + BaseAddress10), "\xC2\x04\x00\x90\x90\x90", 6, true);
		injector::WriteMemoryRaw((0x251744 + BaseAddress10), "\x2E\x5C\x00", 3, true); // PATH PATCH
		// UNUSED PATCHES
	//	injector::WriteMemoryRaw((0x24F6B8 + BaseAddress10), "\x2E\x5C", 2, true);
	//	injector::WriteMemoryRaw((0x250B68 + BaseAddress10), "\x2E\x5C", 2, true);
				
	//CONTROLS PATCH
		// BOOST BUTTON
		injector::MakeNOP((0x151594 + BaseAddress10), 5, true); // PATCH BUTTON IN MENU
		injector::MakeNOP((0x1515BB + BaseAddress10), 5, true);
		U8 patchBoostButton[] = { 0xA1, 0xB0, 0x8C, 0x79, 0x00, 0xC3 };
		LPVOID* ptr3 = (LPVOID*)& patchBoostButton[1];
		*ptr3 = GetTranslatedOffset(0x398CB0 + BaseAddress10);
		injector::WriteMemoryRaw((0x109BA3 + BaseAddress10), patchBoostButton, sizeof(patchBoostButton), true); // PATCH BOOST BUTTON IN GAME
		injector::WriteMemory<U32>((0x257B94 + 0x2C + BaseAddress10), (int)GetTranslatedOffset(0x109BA3 + BaseAddress10), true); // PATCH JUMP TABLE FOR BOOST BUTTON
		// VIEW BUTTON
		injector::MakeNOP((0x151599 + BaseAddress10), 6, true); // PATCH VIEW BUTTON IN MENU
		injector::MakeNOP((0x1515DC + BaseAddress10), 5, true);
		U8 patchViewButton[] = { 0xA1, 0xB8, 0x8C, 0x79, 0x00, 0xC3 };
		LPVOID* ptr4 = (LPVOID*)& patchViewButton[1];
		*ptr4 = GetTranslatedOffset(0x398CB8 + BaseAddress10);
		injector::WriteMemoryRaw((0x109BA9 + BaseAddress10), patchViewButton, sizeof(patchViewButton), true); // PATCH VIEW BUTTON IN GAME
		injector::WriteMemory<U32>((0x257B94 + 0x44 + BaseAddress10), (int)GetTranslatedOffset(0x109BA9 + BaseAddress10), true); // PATCH JUMP TABLE FOR VIEW BUTTON
		// WHEEL AND PEDAL
		injector::MakeNOP((0x151698 + BaseAddress10), 10, true); // PATCH WHEEL WRITE IN MENU
		U8 patchPedal[] = { 0xD9, 0x05, 0xD0, 0x8C, 0x79, 0x00, 0xC2, 0x04, 0x00 };
		LPVOID* ptrPedal = (LPVOID*)&patchPedal[2];
		*ptrPedal = GetTranslatedOffset(0x00398CD0 + BaseAddress10);
		injector::WriteMemoryRaw((0x000859B7 + BaseAddress10), patchPedal, sizeof(patchPedal), true); // PATCH PEDAL
		injector::WriteMemory<U32>((0x257B94 + 0x20 + BaseAddress10), (int)GetTranslatedOffset(0x000859B7 + BaseAddress10), true); // PATCH JUMP TABLE FOR GAS
		U8 patchWheel[] = { 0xD9, 0x05, 0xD4, 0x2C, 0x7D, 0x00, 0xC2, 0x04, 0x00 };
		LPVOID* ptrWheel = (LPVOID*)&patchWheel[2];
		*ptrWheel = GetTranslatedOffset(0x3D2CD4 + BaseAddress10);
		injector::WriteMemoryRaw((0x000859C7 + BaseAddress10), patchWheel, sizeof(patchWheel), true); // PATCH WHEEL
		injector::WriteMemory<U32>((0x257B94 + 0x1C + BaseAddress10), (int)GetTranslatedOffset(0x000859C7 + BaseAddress10), true); // PATCH JUMP TABLE FOR WHEEL
		// OLD WHEEL+PEDAL PATCH
//		U8 patchWheelAndPedal[] = { 0xD9, 0x05, 0xD0, 0x8C, 0x79, 0x00, 0xC2, 0x04, 0x00,  0x83, 0x7C, 0x24, 0x04, 0x00, 0x75, 0xF0, 0xD9, 0x05, 0xD8, 0xD4, 0x69, 0x00, 0xC2, 0x04, 0x00 };
//		LPVOID* ptr1 = (LPVOID*)&patchWheelAndPedal[2];
//		*ptr1 = GetTranslatedOffset(0x00398CD0 + BaseAddress10);
//		LPVOID* ptr2 = (LPVOID*)&patchWheelAndPedal[18];
//		*ptr2 = GetTranslatedOffset(0x0029D4D8 + BaseAddress10);
//		injector::WriteMemoryRaw(0x000859B7 + BaseAddress10, patchWheelAndPedal, sizeof(patchWheelAndPedal),true); // PATCH COMMON CODE USED BY PEDAL AND WHEEL

		//TEST REMAP to DELETE
		injector::WriteMemoryRaw((0x146E68 + BaseAddress10), "\x6A\x2E", 2, true);
		//SERVICE REMAP to INSERT
		injector::WriteMemoryRaw((0x146EB2 + BaseAddress10), "\x6A\x2D", 2, true);
		//VOL+- REMAP to Page Up/Down
		injector::WriteMemoryRaw((0x146F03 + BaseAddress10), "\x6A\x21", 2, true);
		injector::WriteMemoryRaw((0x146F54 + BaseAddress10), "\x6A\x22", 2, true);
		//ENABLE KEYPAD
		injector::WriteMemoryRaw((0x186302 + BaseAddress10), "\xEB", 1, true);
		//KEYPAD HACK
		U8 patchKeypadButton[] = { 0x8B, 0x44, 0x24, 0x04, 0x8B, 0x04, 0x85, 0x04, 0xAA, 0x6B, 0x00 ,0xC2, 0x04, 0x00 };
		injector::WriteMemoryRaw((0x0023CE37 + BaseAddress10), patchKeypadButton, sizeof(patchKeypadButton), true);
		injector::WriteMemory<INT32>((0x257B94 + 0x78 + BaseAddress10), (int)GetTranslatedOffset(0x23CE37 + BaseAddress10), true); // PATCH JUMP TABLE FOR KEYPAD BUTTON

		if (ToBool(config["General"]["Windowed"]))
		{
			MH_Initialize();
			MH_CreateHookApi(L"user32.dll", "CreateWindowExA", &CreateWindowExART10, (void**)&original_CreateWindowExA10);
			MH_EnableHook(MH_ALL_HOOKS);
		}

		if (ToBool(config["Network"]["Enable"]))
		{
			injector::MakeNOP(0xF78AE + BaseAddress10, 6); // Stop game writing to cabinet id values
			injector::MakeNOP(0x1457F3 + BaseAddress10, 5); // Stop game adding certain ips if user has specific adapter
			injector::MakeNOP(0x1457FB + BaseAddress10, 5); // Stop game adding certain ips if user has specific adapter

			int PC1 = FetchDwordInformation("Network", "Cabinet 1 IP X.X.X.???", 256);
			int PC2 = FetchDwordInformation("Network", "Cabinet 2 IP X.X.X.???", 256);
			int PC3 = FetchDwordInformation("Network", "Cabinet 3 IP X.X.X.???", 256);
			int PC4 = FetchDwordInformation("Network", "Cabinet 4 IP X.X.X.???", 256);
			int PC5 = FetchDwordInformation("Network", "Cabinet 5 IP X.X.X.???", 256);
			int PC6 = FetchDwordInformation("Network", "Cabinet 6 IP X.X.X.???", 256);
			int PC7 = FetchDwordInformation("Network", "Cabinet 7 IP X.X.X.???", 256);
			int PC8 = FetchDwordInformation("Network", "Cabinet 8 IP X.X.X.???", 256);

			for (int i = 0; i < 255; i++) {
				injector::WriteMemory<BYTE>((0x329728 + BaseAddress10) + i, 0x00, true);
			}

			if (PC1 > 0 && PC1 < 256) injector::WriteMemory<BYTE>((0x329728 + BaseAddress10) + PC1, 0x01, true);
			if (PC2 > 0 && PC2 < 256) injector::WriteMemory<BYTE>((0x329728 + BaseAddress10) + PC2, 0x02, true);
			if (PC3 > 0 && PC3 < 256) injector::WriteMemory<BYTE>((0x329728 + BaseAddress10) + PC3, 0x03, true);
			if (PC4 > 0 && PC4 < 256) injector::WriteMemory<BYTE>((0x329728 + BaseAddress10) + PC4, 0x04, true);
			if (PC5 > 0 && PC5 < 256) injector::WriteMemory<BYTE>((0x329728 + BaseAddress10) + PC5, 0x05, true);
			if (PC6 > 0 && PC6 < 256) injector::WriteMemory<BYTE>((0x329728 + BaseAddress10) + PC6, 0x06, true);
			if (PC7 > 0 && PC7 < 256) injector::WriteMemory<BYTE>((0x329728 + BaseAddress10) + PC7, 0x07, true);
			if (PC8 > 0 && PC8 < 256) injector::WriteMemory<BYTE>((0x329728 + BaseAddress10) + PC8, 0x08, true);
		}

		CreateThread(NULL, 0, InputRT10, NULL, 0, NULL);

	}, GameID::H2Overdrive);
