#include <StdInc.h>
#include "Utility/InitFunction.h"
#include "Functions/Global.h"
#include "Utility\Hooking.Patterns.h"

#pragma comment(lib, "Ws2_32.lib")

#ifdef _M_AMD64
extern LPCSTR hookPort;

static int PokkenGetSerial(int, int, int, int, char* dst)
{
	OutputDebugStringA("Getting Dongle Serial...");
	memset(dst, 0x00, 0x628);
	memcpy(dst + 0x428, L"274713190000\0\0", 26);
	OutputDebugStringA("Got Dongle Serial!");
	return 1;
}

static int ReturnTrue()
{
	return 1;
}

static InitFunction PokkenFunc([]()
{
	hookPort = "COM3";
	uintptr_t imageBase = (uintptr_t)GetModuleHandleA(0);

	// force windowed
	// BE 01 00 00 00 8B CE -0x8 // ok 10-24
	// 18: imageBase + 0x5A5A2A
	char* windowedPattern = hook::get_pattern<char>("BE 01 00 00 00 8B CE", -0x8);
	if (ToBool(config["General"]["Windowed"]))
	{
		injector::MakeNOP(windowedPattern, 8);
		injector::WriteMemory<BYTE>(windowedPattern + 0x48, 0x00, true);
	}

	// Remove BlockInput
	injector::MakeNOP(windowedPattern+8, 13);

	// make english into japanese
	// TODO: other system locales
	// 65 6E 00 00 6A 61 00 00 // ok 10-24
	// 18: imageBase + 0x9AC1FC
	char* localePattern = hook::get_pattern<char>("65 6E 00 00 6A 61 00 00");
	injector::WriteMemory<char>(localePattern, 'j', true);
	injector::WriteMemory<char>(localePattern + 0x1, 'a', true);


	// dongle
	// 41 B8 28 06 00 00 49 8B CE -0x42 // ok 10-24
	// 18: imageBase + 0x318610
	safeJMP(hook::get_pattern("41 B8 28 06 00 00 49 8B CE", -0x42), PokkenGetSerial);

	// icmp pinging
	// 44 89 4C 24 20 53 41 54 48 83 EC 58 // ok 10-24
	// 18: imageBase + 0x609450
	safeJMP(hook::get_pattern("44 89 4C 24 20 53 41 54 48 83 EC 58"), ReturnTrue);

	// subnet check (don't force 192.168.123.xxx)
	// 48 8B C4 55 48 8D 68 A1 48 81 EC B0 00 00 00 48 C7 45 D7 FE // ok 10-24
	// 18: imageBase + 0x60A5E0
	safeJMP(hook::get_pattern("48 8B C4 55 48 8D 68 A1 48 81 EC B0 00 00 00 48 C7 45 D7 FE"), ReturnTrue);

	// no cpuid detection (crashes on Core 2?)
	// 48 89 5C 24 08 4C 8B C9 C7 41 08 FF FF FF FF // ok 10-24
	// 18: imageBase + 0x6F7C80
	injector::MakeRET(hook::get_pattern("48 89 5C 24 08 4C 8B C9 C7 41 08 FF FF FF FF"));

	// don't give usb controller error #2
	// 48 8D 8F F8 00 00 00 88 9F 05 01 00 00 -0x2 // ok 10-24
	// 18: imageBase + 0x661820
	injector::MakeNOP(hook::get_pattern("48 8D 8F F8 00 00 00 88 9F 05 01 00 00", -0x2), 2);

	auto chars = { 'F', 'G', 'J' };

	for (auto cha : chars)
	{
		auto patterns = hook::pattern(va("%02X 3A 5C", cha));

		if (patterns.size() > 0)
		{
			for (int i = 0; i < patterns.size(); i++)
			{
				char* text = patterns.get(i).get<char>(0);
				std::string text_str(text);

				std::string to_replace = va("%c:\\", cha);
				std::string replace_with = va(".\\%c", cha);

				std::string replaced = text_str.replace(0, to_replace.length(), replace_with);

				injector::WriteMemoryRaw(text, (char*)replaced.c_str(), replaced.length() + 1, true);
			}
		}
	}
}, GameID::PokkenTournament);
#endif