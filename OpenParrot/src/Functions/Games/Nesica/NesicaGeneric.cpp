#include <StdInc.h>
#include "Utility/GameDetect.h"
#include "Utility/InitFunction.h"
#include "Functions/Nesica_Libs/CryptoPipe.h"
#include "Functions/Nesica_Libs/FastIoEmu.h"
#include "Functions/Nesica_Libs/RfidEmu.h"
#include "Functions/Nesica_Libs/NesysEmu.h"
#include "Functions/Nesica_Libs/RegHooks.h"
#include "Utility/Hooking.Patterns.h"

static InitFunction initFunction([]()
{
	init_FastIoEmu();
	init_RfidEmu();
	init_RegHooks();
	if(GameDetect::enableNesysEmu)
		init_NesysEmu();
#if _M_IX86
	init_CryptoPipe(GameDetect::NesicaKey);
#endif
}, GameID::Nesica);

static int ReturnTrue()
{
	return 1;
}

static InitFunction initFunction_USF4([]()
{
	init_FastIoEmu();
	init_RfidEmu();
	init_RegHooks();
	if (GameDetect::enableNesysEmu)
		init_NesysEmu();
#if _M_IX86
	init_CryptoPipe(GameDetect::NesicaKey);
#endif
}, GameID::UltraStreetFighterIVDevExe);

static InitFunction initFunction_GC2([]()
{
	uintptr_t imageBase = (uintptr_t)GetModuleHandleA(0);
	init_FastIoEmu();
	init_RfidEmu();
	init_RegHooks();
	init_NesysEmu();

	// Patch D: references
	//D:
	injector::WriteMemoryRaw(imageBase + 0x33D344, ".", 2, true);
	// D:/garbage%d.txt
	injector::WriteMemoryRaw(imageBase + 0x2B6B08, "./garbage%d.txt", 16, true);
	// D:/country.dat
	injector::WriteMemoryRaw(imageBase + 0x2B4B68, "./country.dat", 14, true);
	injector::WriteMemoryRaw(imageBase + 0x2B4B54, "./country.dat", 14, true);
	// D:/NesysQueue_Error_%04d_%02d_%02d_%02d_%02d_%02d.txt
	injector::WriteMemoryRaw(imageBase + 0x2B205C, "./NesysQueue_Error_%04d_%02d_%02d_%02d_%02d_%02d.txt", 53, true);
	// D:/count.csv
	injector::WriteMemoryRaw(imageBase + 0x2B1024, "./count.csv", 12, true);
	injector::WriteMemoryRaw(imageBase + 0x2B0F40, "./count.csv", 12, true);
	// D:\\%s/
	injector::WriteMemoryRaw(imageBase + 0x27AD80, "./%s/", 6, true);
	// D:\\%s/*
	injector::WriteMemoryRaw(imageBase + 0x27AD78, "./%s/*", 7, true);
	// "D:\\%s/%s"
	injector::WriteMemoryRaw(imageBase + 0x27AD6C, "./%s/%s", 8, true);
	// "D:\\"
	injector::WriteMemoryRaw(imageBase + 0x27AC44, "./", 3, true);
	// D:\\%s%04d%02d%02d_%02d%02d%02d_
	injector::WriteMemoryRaw(imageBase + 0x27AC00, "./%s%04d%02d%02d_%02d%02d%02d_", 31, true);
	// D:\\%s/%s/*
	injector::WriteMemoryRaw(imageBase + 0x27AD60, "./%s/%s/*", 10, true);
	// D:/PlayData/
	injector::WriteMemoryRaw(imageBase + 0x2A9CB8, "./PlayData/", 12, true);

	// C:\\TypeXZEROTemp.dat check
	safeJMP(imageBase + 0xF81B0, ReturnTrue);

	// Ignore stupid OutputDebugStringA Scheluder crap
	injector::MakeNOP(imageBase + 0x2008CC, 7);

	// Unstuck the game from some dumb mouse scanner func
	injector::MakeNOP(imageBase + 0xA3FF6, 2);

	// Patch dongle spam on RFID port
	injector::MakeNOP(imageBase + 0xF90F6, 5);
#if _M_IX86
	init_CryptoPipe(GameDetect::NesicaKey);
#endif
}, GameID::GrooveCoaster2);

static InitFunction initFunction_DariusBurst([]()
{
	init_FastIoEmu();
	init_RegHooks();
	init_NesysEmu(true);

	uintptr_t imageBase = (uintptr_t)GetModuleHandleA(0);

	// Ignore cryptopipe check.
	// NOTE: This could be cause for the non-working TEST MODE. No time to analyze since dump was released and we want to give instant support.
	//injector::WriteMemory<BYTE>(imageBase + 0x2CC753, 0xEB, true);

	// D:
	injector::WriteMemoryRaw(imageBase + 0x482F38, "\x2E\x5C\x44", 3, true); // D:\%s%04d%02d%02d_%02d%02d%02d_
	injector::WriteMemoryRaw(imageBase + 0x4830A0, "\x2E\x5C\x44", 3, true); // D:\%s/%s/*
	injector::WriteMemoryRaw(imageBase + 0x4830AC, "\x2E\x5C\x44", 3, true); // D:\%s/%s
	injector::WriteMemoryRaw(imageBase + 0x4830B3, "\x2E\x5C\x44", 3, true); // D:\%s/*
	injector::WriteMemoryRaw(imageBase + 0x49FC90, "\x2E\x5C\x44", 3, true); // D:\%s
	injector::WriteMemoryRaw(imageBase + 0x4A269C, "\x2E\x5C\x44", 3, true); // EDData
	injector::WriteMemoryRaw(imageBase + 0x4AA168, "\x2E\x5C\x44", 3, true);
	injector::WriteMemoryRaw(imageBase + 0x4D460C, "\x2E\x5C\x44", 3, true);
	injector::WriteMemoryRaw(imageBase + 0x4D46A8, "\x2E\x5C\x44", 3, true); // Proclog

	// D:/
	injector::WriteMemoryRaw(imageBase + 0x4D44B4, "\x2E\x5C\x44", 3, true);

	// Disable invertion of 2nd screen area
	// NOTE: Nezarn is pro
	injector::WriteMemoryRaw(imageBase + 0x4D4E34, "\x30\x2E\x30\x66\x20\x20\x20\x20", 8, true); // 0.0f
	injector::WriteMemoryRaw(imageBase + 0x4D4E4C, "\x2A\x20\x30\x2E\x30\x66\x20\x20\x20\x20\x20\x2D", 12, true); // * 0.0f -
#if _M_IX86
	init_CryptoPipe(GameDetect::NesicaKey);
#endif
}, GameID::DariusBurst);

static InitFunction initFunction_DariusBurst116([]()
{
	init_FastIoEmu();
	init_RegHooks();
	init_NesysEmu(true);

	uintptr_t imageBase = (uintptr_t)GetModuleHandleA(0);

	// Ignore cryptopipe check.
	// NOTE: This could be cause for the non-working TEST MODE. No time to analyze since dump was released and we want to give instant support.
	//injector::WriteMemory<BYTE>(imageBase + 0x302743, 0xEB, true);

	// D:
	injector::WriteMemoryRaw(imageBase + 0x4EEF68, "\x2E\x5C\x44", 3, true); // D:\%s%04d%02d%02d_%02d%02d%02d_
	injector::WriteMemoryRaw(imageBase + 0x4EF0D0, "\x2E\x5C\x44", 3, true); // D:\%s/%s/*
	injector::WriteMemoryRaw(imageBase + 0x4EF0DC, "\x2E\x5C\x44", 3, true); // D:\%s/%s
	injector::WriteMemoryRaw(imageBase + 0x4EF0E8, "\x2E\x5C\x44", 3, true); // D:\%s/*
	injector::WriteMemoryRaw(imageBase + 0x4EF0F0, "\x2E\x5C\x44", 3, true); // D:\%s
	injector::WriteMemoryRaw(imageBase + 0x50E980, "\x2E\x5C\x44", 3, true); // D:\EDData
	injector::WriteMemoryRaw(imageBase + 0x50EB58, "\x2E\x5C\x44", 3, true); // D:\EDData
	injector::WriteMemoryRaw(imageBase + 0x5145E0, "\x2E\x5C\x44", 3, true); // D:\EDData
	injector::WriteMemoryRaw(imageBase + 0x539190, "\x2E\x5C\x44", 3, true); // D:\EDData
	injector::WriteMemoryRaw(imageBase + 0x539240, "\x2E\x5C\x44", 3, true); // Proclog
	injector::WriteMemoryRaw(imageBase + 0x50DD84, "\x2E\x5C\x44", 3, true); // D:\EDData\event000.pxk
	injector::WriteMemoryRaw(imageBase + 0x50DD9C, "\x2E\x5C\x44", 3, true); // D:\EDData\ev
	injector::WriteMemoryRaw(imageBase + 0x50E8EC, "\x2E\x5C\x44", 3, true); // D:\EDData\ev
	injector::WriteMemoryRaw(imageBase + 0x50E8FC, "\x2E\x5C\x44", 3, true); // D:\EDData\ev\event000.sxr
	injector::WriteMemoryRaw(imageBase + 0x50EB64, "\x2E\x5C\x44", 3, true); // D:\EDData\ev\event000.sxr
	injector::WriteMemoryRaw(imageBase + 0x50EBE8, "\x2E\x5C\x44", 3, true); // D:\EDData\ev
	injector::WriteMemoryRaw(imageBase + 0x50EBF8, "\x2E\x5C\x44", 3, true); // D:\EDData\ev\event000.sxr
	injector::WriteMemoryRaw(imageBase + 0x517464, "\x2E\x5C\x44", 3, true); // D:\EDData\news000.tx2
	injector::WriteMemoryRaw(imageBase + 0x51747C, "\x2E\x5C\x44", 3, true); // D:\EDData\news000.tx2
	injector::WriteMemoryRaw(imageBase + 0x517494, "\x2E\x5C\x44", 3, true); // D:\EDData\news000.tx2
	injector::WriteMemoryRaw(imageBase + 0x518524, "\x2E\x5C\x44", 3, true); // D:\EDData\ev
	injector::WriteMemoryRaw(imageBase + 0x5660FC, "\x2E\x5C\x44", 3, true); // D:\EDData\ev
	injector::WriteMemoryRaw(imageBase + 0x56610C, "\x2E\x5C\x44", 3, true); // D:\EDData\ev\event000.sxr

	// D:/
	injector::WriteMemoryRaw(imageBase + 0x539020, "\x2E\x5C\x44", 3, true);

	// Disable invertion of 2nd screen area
	injector::WriteMemoryRaw(imageBase + 0x5399CC, "\x30\x2E\x30\x66\x20\x20\x20\x20", 8, true); // 0.0f
	injector::WriteMemoryRaw(imageBase + 0x5399E4, "\x2A\x20\x30\x2E\x30\x66\x20\x20\x20\x20\x20\x2D", 12, true); // * 0.0f -
#if _M_IX86
	init_CryptoPipe(GameDetect::NesicaKey);
#endif
}, GameID::DariusBurst116);

static InitFunction initFunction_PB([]()
{
	uintptr_t imageBase = (uintptr_t)GetModuleHandleA(0);
	init_FastIoEmu();
	init_RfidEmu();
	init_RegHooks();
	init_NesysEmu();

	// Nesys error workaround
	injector::WriteMemoryRaw(imageBase + 0xA77B, "\xA3\xEC\x0D\x4F\x00\x90", 6, true);
}, GameID::PuzzleBobble);

static InitFunction initFunction_MB([]()
{
	uintptr_t imageBase = (uintptr_t)GetModuleHandleA(0);
	init_FastIoEmu();
	init_RfidEmu();
	init_RegHooks();
	init_NesysEmu();
#if _M_IX86
	init_CryptoPipe(GameDetect::NesicaKey);
#endif
	// Skip Initilization wait time.
	injector::MakeNOP(imageBase + 0x56B21, 2);
}, GameID::MagicalBeat);

static InitFunction initFunction_CC([]()
{
	uintptr_t imageBase = (uintptr_t)GetModuleHandleA(0);
	init_FastIoEmu();
	init_RfidEmu();
	init_RegHooks();
	init_NesysEmu();
#if _M_IX86
	init_CryptoPipe(GameDetect::NesicaKey);
#endif
	// Skip stuck on Warning screen. NESYS emu must be improved for this to work properly!
	injector::MakeNOP(imageBase + 0x1015E7, 2);
}, GameID::CrimzonClover);

static InitFunction initFunction_SOR([]()
{
	uintptr_t imageBase = (uintptr_t)GetModuleHandleA(0);
	init_FastIoEmu();
	init_RfidEmu();
	// TODO: DOCUMENT PATCHES
	safeJMP(imageBase + 0xFA350, ReturnTrue);
	safeJMP(imageBase + 0xF8FC0, ReturnTrue);
	// Patch data dir to game dir pls D:/ -> .\\
	//
	injector::WriteMemory<DWORD>(imageBase + 0x21B9AC0, 0x2F002E002E, true);
	init_NesysEmu();
	if (ToBool(config["General"]["Windowed"]))
	{
		// TODO: DOCUMENT PATCHES
		injector::WriteMemory<LONGLONG>(imageBase + 0xFF703C, 0xF633C1FFC1FFC933, true);
		injector::WriteMemory<DWORD>(imageBase + 0xFF703C+0x08, 0xC6FFC6FF, true);
	}
}, GameID::SchoolOfRagnarok);

static InitFunction initFunction_HyperSF4([]()
{
	init_FastIoEmu();
	init_RfidEmu();
	init_RegHooks();
	//init_NesysEmu();
#if _M_IX86
	init_CryptoPipe(GameDetect::NesicaKey);
#endif
}, GameID::HyperStreetFighterII);

static InitFunction initFunction_StreetFighterZero3([]()
{
	init_FastIoEmu();
	init_RfidEmu();
	init_RegHooks();
	//init_NesysEmu();
#if _M_IX86
	init_CryptoPipe(GameDetect::NesicaKey);
#endif
}, GameID::StreetFigherZero3);

static InitFunction initFunction_StreetFighter3rdStrike([]()
{
	init_FastIoEmu();
	init_RfidEmu();
	init_RegHooks();
	//init_NesysEmu();
#if _M_IX86
	init_CryptoPipe(GameDetect::NesicaKey);
#endif
}, GameID::StreetFighter3rdStrike);

static InitFunction initFunction_RumbleFish2([]()
{
	init_FastIoEmu();
	init_RfidEmu();
	init_RegHooks();
	init_NesysEmu();
#if _M_IX86
	init_CryptoPipe(GameDetect::NesicaKey);
#endif
}, GameID::RumbleFish2);

static InitFunction initFunction_KOF98Nesica([]()
{
	init_FastIoEmu();
	init_RfidEmu();
	init_RegHooks();
	init_NesysEmu();
#if _M_IX86
	init_CryptoPipe(GameDetect::NesicaKey);
#endif
}, GameID::KOF98Nesica);

static InitFunction initFunction_Theatrhythm([]()
	{
		uintptr_t imageBase = (uintptr_t)GetModuleHandleA(0);

		static std::string modPath;

		if (modPath.empty())
		{
			char exeName[512];
			GetModuleFileNameA(GetModuleHandle(L"OpenParrot64.dll"), exeName, sizeof(exeName));

			char* exeBaseName = strrchr(exeName, '\\');
			exeBaseName[0] = L'\0';

			modPath = exeName;
			modPath += "\\";

			GetFullPathNameA(modPath.c_str(), sizeof(exeName), exeName, nullptr);

			modPath = exeName;
			modPath += "\\";
		}

		std::string idmac = modPath + "idmacdrv64.dll";

		uintptr_t idmacbase = (uintptr_t)LoadLibraryA(idmac.c_str());

		if (idmacbase == NULL)
			ExitProcess(0);

		init_FastIoEmu();
	
	}, GameID::Theatrhythm);

#if _M_IX86
static InitFunction initFunction_KOFXIIIClimax([]()
{
	init_CryptoPipe(GameDetect::NesicaKey);
}, GameID::KOFXIIIClimax);

static InitFunction initFunction_KOFXIII([]()
{
		init_CryptoPipe(GameDetect::NesicaKey);
}, GameID::KOFXIII);
#endif