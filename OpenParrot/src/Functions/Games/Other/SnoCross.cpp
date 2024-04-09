#include <StdInc.h>
#include "Utility/InitFunction.h"
#include "Functions/Global.h"
#include "Utility\Hooking.Patterns.h"
#include <dinput.h>

#if _M_IX86
static DWORD mainModuleBase = 0x00400000;

static bool previousLeft = false;
static bool previousRight = false;
static bool previousUp = false;
static bool previousDown = false;
static bool button1pressed = false;
static bool button2pressed = false;
static bool button3pressed = false;
static bool button4pressed = false;
static bool PINpressed = false;
static bool NUMpressed = false;
static bool NUMpressed0 = false;
static bool NUMpressed1 = false;
static bool NUMpressed2 = false;
static bool NUMpressed3 = false;
static bool NUMpressed4 = false;
static bool NUMpressed5 = false;
static bool NUMpressed6 = false;
static bool NUMpressed7 = false;
static bool NUMpressed8 = false;
static bool NUMpressed9 = false;

// controls
extern int* ffbOffset;
extern int* ffbOffset2;
extern int* ffbOffset3;
extern int* ffbOffset4;

DWORD WINAPI InputRT7(LPVOID lpParam)
{
	int deltaTimer = 16;
	INT_PTR keyboardBuffer = (0x5CA700 + mainModuleBase);
	INT_PTR keyboardBuffer2 = (0x5CA1F0 + mainModuleBase);

	while (true)
	{
		if (GetAsyncKeyState(VK_NUMPAD1) & 0x0001)
		{
			if (NUMpressed1 == false)
			{
				injector::WriteMemoryRaw((0x1A634 + mainModuleBase), "\xB8\x01\x00\x00\x00\x90", 6, true);
				NUMpressed1 = true;
			}
		}
		else
		{
			if (NUMpressed1 == true)
			{
				injector::WriteMemoryRaw((0x1A634 + mainModuleBase), "\x8D\x87\x80\xFF\xFE\xFF", 6, true);
				NUMpressed1 = false;
			}
		}
		if (GetAsyncKeyState(VK_NUMPAD2) & 0x0001)
		{
			if (NUMpressed2 == false)
			{
				injector::WriteMemoryRaw((0x1A634 + mainModuleBase), "\xB8\x02\x00\x00\x00\x90", 6, true);
				NUMpressed2 = true;
			}
		}
		else
		{
			if (NUMpressed2 == true)
			{
				injector::WriteMemoryRaw((0x1A634 + mainModuleBase), "\x8D\x87\x80\xFF\xFE\xFF", 6, true);
				NUMpressed2 = false;
			}
		}
		if (GetAsyncKeyState(VK_NUMPAD3) & 0x0001)
		{
			if (NUMpressed3 == false)
			{
				injector::WriteMemoryRaw((0x1A634 + mainModuleBase), "\xB8\x03\x00\x00\x00\x90", 6, true);
				NUMpressed3 = true;
			}
		}
		else
		{
			if (NUMpressed3 == true)
			{
				injector::WriteMemoryRaw((0x1A634 + mainModuleBase), "\x8D\x87\x80\xFF\xFE\xFF", 6, true);
				NUMpressed3 = false;
			}
		}
		if (GetAsyncKeyState(VK_NUMPAD4) & 0x0001)
		{
			if (NUMpressed4 == false)
			{
				injector::WriteMemoryRaw((0x1A634 + mainModuleBase), "\xB8\x04\x00\x00\x00\x90", 6, true);
				NUMpressed4 = true;
			}
		}
		else
		{
			if (NUMpressed4 == true)
			{
				injector::WriteMemoryRaw((0x1A634 + mainModuleBase), "\x8D\x87\x80\xFF\xFE\xFF", 6, true);
				NUMpressed4 = false;
			}
		}
		if (GetAsyncKeyState(VK_NUMPAD5) & 0x0001)
		{
			if (NUMpressed5 == false)
			{
				injector::WriteMemoryRaw((0x1A634 + mainModuleBase), "\xB8\x05\x00\x00\x00\x90", 6, true);
				NUMpressed5 = true;
			}
		}
		else
		{
			if (NUMpressed5 == true)
			{
				injector::WriteMemoryRaw((0x1A634 + mainModuleBase), "\x8D\x87\x80\xFF\xFE\xFF", 6, true);
				NUMpressed5 = false;
			}
		}
		if (GetAsyncKeyState(VK_NUMPAD6) & 0x0001)
		{
			if (NUMpressed6 == false)
			{
				injector::WriteMemoryRaw((0x1A634 + mainModuleBase), "\xB8\x06\x00\x00\x00\x90", 6, true);
				NUMpressed6 = true;
			}
		}
		else
		{
			if (NUMpressed6 == true)
			{
				injector::WriteMemoryRaw((0x1A634 + mainModuleBase), "\x8D\x87\x80\xFF\xFE\xFF", 6, true);
				NUMpressed6 = false;
			}
		}
		if (GetAsyncKeyState(VK_NUMPAD7) & 0x0001)
		{
			if (NUMpressed7 == false)
			{
				injector::WriteMemoryRaw((0x1A634 + mainModuleBase), "\xB8\x07\x00\x00\x00\x90", 6, true);
				NUMpressed7 = true;
			}
		}
		else
		{
			if (NUMpressed7 == true)
			{
				injector::WriteMemoryRaw((0x1A634 + mainModuleBase), "\x8D\x87\x80\xFF\xFE\xFF", 6, true);
				NUMpressed7 = false;
			}
		}
		if (GetAsyncKeyState(VK_NUMPAD8) & 0x0001)
		{
			if (NUMpressed8 == false)
			{
				injector::WriteMemoryRaw((0x1A634 + mainModuleBase), "\xB8\x08\x00\x00\x00\x90", 6, true);
				NUMpressed8 = true;
			}
		}
		else
		{
			if (NUMpressed8 == true)
			{
				injector::WriteMemoryRaw((0x1A634 + mainModuleBase), "\x8D\x87\x80\xFF\xFE\xFF", 6, true);
				NUMpressed8 = false;
			}
		}
		if (GetAsyncKeyState(VK_NUMPAD9) & 0x0001)
		{
			if (NUMpressed9 == false)
			{
				injector::WriteMemoryRaw((0x1A634 + mainModuleBase), "\xB8\x09\x00\x00\x00\x90", 6, true);
				NUMpressed9 = true;
			}
		}
		else
		{
			if (NUMpressed9 == true)
			{
				injector::WriteMemoryRaw((0x1A634 + mainModuleBase), "\x8D\x87\x80\xFF\xFE\xFF", 6, true);
				NUMpressed9 = false;
			}
		}
		if (GetAsyncKeyState(VK_NUMPAD0) & 0x0001)
		{
			if (NUMpressed0 == false)
			{
				injector::WriteMemoryRaw((0x1A634 + mainModuleBase), "\xB8\x00\x00\x00\x00\x90", 6, true);
				NUMpressed0 = true;
			}
		}
		else
		{
			if (NUMpressed0 == true)
			{
				injector::WriteMemoryRaw((0x1A634 + mainModuleBase), "\x8D\x87\x80\xFF\xFE\xFF", 6, true);
				NUMpressed0 = false;
			}
		}
		if (GetAsyncKeyState(VK_RETURN) & 0x0001)
		{
			if (NUMpressed == false)
			{
				injector::WriteMemoryRaw((0x1A634 + mainModuleBase), "\xB8\x0A\x00\x00\x00\x90", 6, true);
				injector::WriteMemoryRaw((0x1A77D + mainModuleBase), "\x90\x90", 2, true);
				NUMpressed = true;
			}
		}
		else
		{
			if (NUMpressed == true)
			{
				injector::WriteMemoryRaw((0x1A634 + mainModuleBase), "\x8D\x87\x80\xFF\xFE\xFF", 6, true);
				injector::WriteMemoryRaw((0x1A77D + mainModuleBase), "\x7C\x49", 2, true);
				NUMpressed = false;
			}
		}

		// buttons see bitwise values in TPui//RawThrills.cs
		// START
		if (*ffbOffset & 0x08)
		{
			injector::WriteMemory<BYTE>((keyboardBuffer + 4 * 0x00), 2, true);
			if (NUMpressed == false)
			{
				injector::WriteMemoryRaw((0x1A634 + mainModuleBase), "\xB8\x0A\x00\x00\x00\x90", 6, true);
				injector::WriteMemoryRaw((0x1A77D + mainModuleBase), "\x90\x90", 2, true);
				NUMpressed = true;
			}
		}
		else
		{
			if (NUMpressed == true)
			{
				injector::WriteMemoryRaw((0x1A634 + mainModuleBase), "\x8D\x87\x80\xFF\xFE\xFF", 6, true);
				injector::WriteMemoryRaw((0x1A77D + mainModuleBase), "\x7C\x49", 2, true);
				NUMpressed = false;
			}
		}
		// TEST
		if (*ffbOffset & 0x01)
		{
			injector::WriteMemory<BYTE>((keyboardBuffer + 4 * 0x0B), 2, true);
		}
		// NITRO
		if (*ffbOffset & 0x100)
		{
			if (button1pressed == false)
			{
				injector::WriteMemory<BYTE>((keyboardBuffer + 4 * 0x00), 2, true);
				button1pressed = true;
			}
		}
		else
		{
			if (button1pressed == true)
			{
				button1pressed = false;
			}
		}
		// BUTTON 1/ VIEW
		if (*ffbOffset & 0x200)
		{
			if (button2pressed == false)
			{
				injector::WriteMemory<BYTE>((keyboardBuffer + 4 * 0x04), 2, true);
				button2pressed = true;
			}
		}
		else
		{
			if (button2pressed == true)
			{
				button2pressed = false;
			}
		}
		// BUTTON 2/ SNOWBALLS
		if (*ffbOffset & 0x400)
		{
			if (button3pressed == false)
			{
				injector::WriteMemory<BYTE>((keyboardBuffer + 4 * 0x01), 2, true);
				button3pressed = true;
			}
		}
		else
		{
			if (button3pressed == true)
			{
				button3pressed = false;
			}
		}
		// BUTTON 3/ CREDITS
		if (*ffbOffset & 0x800)
		{
			if (button4pressed == false)
			{
				injector::WriteMemory<BYTE>((keyboardBuffer2 + 0x2C), 3, true);
				button4pressed = true;
			}
		}
		else
		{
			if (button4pressed == true)
			{
				button4pressed = false;
			}
		}
		// BUTTON ENTER-PIN
		if (*ffbOffset & 0x20)
		{
			if (PINpressed == false)
			{
				injector::WriteMemoryRaw((0x26614 + mainModuleBase), "\x90\x90", 2, true);
				injector::WriteMemoryRaw((0x2661F + mainModuleBase), "\x90\x90", 2, true);
				injector::WriteMemoryRaw((0x26638 + mainModuleBase), "\x90\x90", 2, true);

				injector::WriteMemoryRaw((0x28BE2 + mainModuleBase), "\x90\x90", 2, true);
				injector::WriteMemoryRaw((0x28BED + mainModuleBase), "\x90\x90", 2, true);
				injector::WriteMemoryRaw((0x28C08 + mainModuleBase), "\x90\x90", 2, true);

				injector::WriteMemoryRaw((0x2A5A4 + mainModuleBase), "\x90\x90", 2, true);
				injector::WriteMemoryRaw((0x2A5AF + mainModuleBase), "\x90\x90", 2, true);
				injector::WriteMemoryRaw((0x2A5C8 + mainModuleBase), "\x90\x90", 2, true);
				PINpressed = true;
			}
		}
		else
		{
			if (PINpressed == true)
			{
				injector::WriteMemoryRaw((0x26614 + mainModuleBase), "\x75\x3F", 2, true);
				injector::WriteMemoryRaw((0x2661F + mainModuleBase), "\x74\x34", 2, true);
				injector::WriteMemoryRaw((0x26638 + mainModuleBase), "\x75\x07", 2, true);

				injector::WriteMemoryRaw((0x28BE2 + mainModuleBase), "\x75\x41", 2, true);
				injector::WriteMemoryRaw((0x28BED + mainModuleBase), "\x74\x36", 2, true);
				injector::WriteMemoryRaw((0x28C08 + mainModuleBase), "\x75\x07", 2, true);

				injector::WriteMemoryRaw((0x2A5A4 + mainModuleBase), "\x75\x3F", 2, true);
				injector::WriteMemoryRaw((0x2A5AF + mainModuleBase), "\x74\x34", 2, true);
				injector::WriteMemoryRaw((0x2A5C8 + mainModuleBase), "\x75\x07", 2, true);
				PINpressed = false;
			}
		}
		// MENU LEFT
		if (*ffbOffset & 0x4000)
		{
			if (previousLeft == false)
			{
				injector::WriteMemory<BYTE>((keyboardBuffer + DIK_LEFT), 2, true);
				previousLeft = true;
			}
		}
		else
		{
			if (previousLeft == true)
			{
				previousLeft = false;
			}
		}
		// MENU RIGHT
		if (*ffbOffset & 0x8000)
		{
			if (previousRight == false)
			{
				injector::WriteMemory<BYTE>((keyboardBuffer + DIK_RIGHT), 2, true);
				previousRight = true;
			}
		}
		else
		{
			if (previousRight == true)
			{
				previousRight = false;
			}
		}

		// WHEEL
		int iWheel0 = (((float)*ffbOffset2) - 128);
		float wheel = (iWheel0 * 0.0078125f);
		int iWheel = (int)(2047.5 + 2047.5 * wheel);
		injector::WriteMemory<INT32>(((0x5CA300 + mainModuleBase) + 4 * 0x20), iWheel, true);
		//// GAS
		float gas = (float)*ffbOffset3 / 255.0f;
		int iGas = (int)(gas * 4095);
		injector::WriteMemory<INT32>(((0x5CA300 + mainModuleBase) + 4 * 0x21), iGas, true);
		//// BRAKE
		float brake = (float)*ffbOffset4 / 255.0f;
		int iBrake = (int)(brake * 4095);
		injector::WriteMemory<INT32>(((0x5CA300 + mainModuleBase) + 4 * 0x22), iBrake, true);

		//DEBUG//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		//	info(true, "test value %f ", test);
		//DEBUG//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

		Sleep(deltaTimer);
	}

	return 0;
}

static InitFunction SnoCrossFunc([]()
{
	// PATCHING EXE AT RUNTIME (reboots, network, filepath, config, CRC...36DF3D
	injector::WriteMemoryRaw((0x36DF3D + mainModuleBase), "\x75\x0D", 2, true); // Really cursed start() patch, what the fuck?
	//injector::WriteMemoryRaw((0x6C7EB + mainModuleBase), "\xF0", 1, true);
	injector::WriteMemoryRaw((0x6CAF0 + mainModuleBase), "\xC3\x90\x90", 3, true); // ret 0 on ioboard stuff?
	injector::WriteMemoryRaw((0x6D020 + mainModuleBase), "\xC3", 1, true); // again?
	injector::WriteMemoryRaw((0x6F6D0 + mainModuleBase), "\xE9\xCB\x00\x00\x00\x90", 6, true); // dont reboot
	injector::WriteMemoryRaw((0x6FFE0 + mainModuleBase), "\xC3\x90", 2, true); // skip port talk ffb stuff
	injector::WriteMemoryRaw((0x70DE0 + mainModuleBase), "\x90\x90", 2, true); // more maintenance stuff
	injector::WriteMemoryRaw((0x71499 + mainModuleBase), "\xE9\x9A\x00\x00\x00\x90", 6, true); // more io board checks
	injector::WriteMemoryRaw((0x3783AD + mainModuleBase), "\x31\xC0\x40", 3, true); // ???????? this exe is cursed for sure. 
	//injector::WriteMemoryRaw(0x3A8F90 + mainModuleBase, ".\\cops2.hst\0", 12, true);
	//injector::WriteMemoryRaw(0x3A9D48 + mainModuleBase, ".\\audbak\0", 9, true);
	//injector::WriteMemoryRaw(0x3A9D5C + mainModuleBase, ".\\audits\0", 9, true);
	//injector::WriteMemoryRaw(0x3AA054 + mainModuleBase, ".\\AuditLog%.3d.txt\0", 19, true);
	//injector::WriteMemoryRaw(0x3ACE14 + mainModuleBase, ".\\prefsbak\0", 11, true);
	//injector::WriteMemoryRaw(0x3ACE28 + mainModuleBase, ".\\prefs\0", 8, true);
	//injector::WriteMemoryRaw(0x3AF5C8 + mainModuleBase, ".\\mp.pdt\0", 9, true);
	//injector::WriteMemoryRaw(0x3AAB00 + mainModuleBase, ".\\\0", 3, true); // C:\rawart
	//injector::WriteMemoryRaw(0x3AE224 + mainModuleBase, ".\\erlg0000.txt\0", 15, true);
	//injector::WriteMemoryRaw(0x3AE294 + mainModuleBase, ".\\HighScores.html\0", 18, true);
	//injector::WriteMemoryRaw(0x3AAAF0 + mainModuleBase, ".\\version.txt\0", 14, true);
	//injector::WriteMemoryRaw(0x3AB6C0 + mainModuleBase, ".\\1stboot.txt\0", 14, true);
	//injector::WriteMemoryRaw(0x3AE200 + mainModuleBase, ".\\errorlog.txt\0", 15, true);
	//injector::WriteMemoryRaw(0x3AE210 + mainModuleBase, ".\\Minidump_%d.dmp\0", 18, true);
	//injector::WriteMemoryRaw(0x3B03A4 + mainModuleBase, ".\\Minidump\0", 11, true);
	//injector::WriteMemoryRaw(0x3AE268 + mainModuleBase, "%c\\AuditLog%04i.txt\0", 20, true);
	//injector::WriteMemoryRaw(0x3AE23C + mainModuleBase, "%c\\erlg%.4d.txt\0", 16, true);
	//injector::WriteMemoryRaw(0x3AE2B0 + mainModuleBase, "%c\\scores%04i.html\0", 19, true);
	//injector::WriteMemoryRaw(0x3AE280 + mainModuleBase, "%c\\audit%04i.txt\0", 17, true);

	injector::WriteMemoryRaw(0x7ae250, "audit0000.txt", 14, true);
	injector::WriteMemoryRaw(0x7ae294, "HighScores.html", 16, true);
	injector::WriteMemoryRaw(0x7ae224, "erlg0000.txt", 13, true);

	// some %c string fix
	//injector::WriteMemory<BYTE>(0xA2A6A + mainModuleBase, 0x2E, true);
	//injector::WriteMemory<BYTE>(0xA298A + mainModuleBase, 0x2E, true);
	//injector::WriteMemory<BYTE>(0xA2BEA + mainModuleBase, 0x2E, true);

	// REPLACE SPACE KEY WITH ESC TO PREVENT EXITING LEVEL PREMATURELY
	injector::WriteMemory<BYTE>((0x77328 + mainModuleBase), DIK_ESCAPE, true);

	// Run from relative paths instead of absolute
	safeJMP(0x4befc0, genericRetZero, true);

	injector::MakeNOP(0x46c7b0, 7, true);

	// REMOVE ESC BOX
	injector::MakeNOP((0x476999), 5, true);

	CreateThread(NULL, 0, InputRT7, NULL, 0, NULL);

	if (ToBool(config["General"]["Windowed"]))
	{
		// NO HIDE CURSOR
		//injector::WriteMemory<BYTE>((0x14A9F + mainModuleBase), 0x01, true);
		//injector::MakeNOP(mainModuleBase + 0x14B44, 8, true);

		static const char* title = "OpenParrot - X-Games SnoCross";
		injector::WriteMemory<DWORD>(mainModuleBase + 0x14DC5, (DWORD)title, true);

		windowHooks hooks = { 0 };
		hooks.createWindowExA = mainModuleBase + 0x3A32F4;
		hooks.adjustWindowRect = mainModuleBase + 0x3A332C;
		hooks.adjustWindowRectEx = mainModuleBase + 0x3A32F8;
		hooks.setWindowPos = mainModuleBase + 0x3A3328;
		hooks.setCursorPos = mainModuleBase + 0x3A3300;

		init_windowHooks(&hooks);
	}

	// MACHINE ID setting
	if ((strcmp(config["Network"]["MachineID"].c_str(), "2") == 0))
	{
		injector::WriteMemory<DWORD>((0x4A4490 + mainModuleBase), 0x01, true);
	}
	else if ((strcmp(config["Network"]["MachineID"].c_str(), "3") == 0))
	{
		injector::WriteMemory<DWORD>((0x4A4490 + mainModuleBase), 0x02, true);
	}
	else if ((strcmp(config["Network"]["MachineID"].c_str(), "4") == 0))
	{
		injector::WriteMemory<DWORD>((0x4A4490 + mainModuleBase), 0x03, true);
	}
	else // MACHINE ID = 1
	{
		injector::WriteMemory<DWORD>((0x4A4490 + mainModuleBase), 0x00, true);
	}

	// FPS Fix
	if ((strcmp(config["General"]["FPSFix"].c_str(), "0") != 0))
	{
		std::string FPSstring = (LPCSTR)(config["General"]["FPSFix"].c_str());
		int FPSvalue = std::stoi(FPSstring);
		if (FPSvalue > 255)
		{
			FPSvalue = 0;
		}
		if (FPSvalue < 0)
		{
			FPSvalue = 0;
		}
		injector::WriteMemoryRaw((0xF0140 + mainModuleBase), "\x6A", 1, true);
		injector::WriteMemory<BYTE>((0xF0141 + mainModuleBase), FPSvalue, true);
		injector::WriteMemoryRaw((0xF0142 + mainModuleBase), "\xFF\x15\x7C\x71\x71\x00", 6, true);
		injector::WriteMemoryRaw((0xF0148 + mainModuleBase), "\xC3", 1, true);
	}

	// redirect messages.txt
	injector::WriteMemoryRaw(mainModuleBase + 0x4A9EF4, "./messages.txt", 14, true);

	//graphics crashfix
	injector::WriteMemory<BYTE>(mainModuleBase + 0xC7FE2, 0xEB, true);
	injector::MakeNOP(mainModuleBase + 0xC7FDB, 2, true);

}, GameID::SnoCross);
#endif