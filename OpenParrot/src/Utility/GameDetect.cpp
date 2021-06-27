#include <StdInc.h>
#include "GameDetect.h"
#include <filesystem>
#pragma optimize("", off)
bool GameDetect::isNesica = false;
bool GameDetect::enableNesysEmu = true;
NesicaKey GameDetect::NesicaKey;
X2Type GameDetect::X2Type = X2Type::None;
static char newCrc[0x400];
static char errorBuffer[256];

#if _M_IX86
void GameDetect::DetectCurrentLinuxGame()
{
}
#endif

void GameDetect::DetectCurrentGame()
{
	// TODO: move all game detection bound to crcResult immediately below to use the newCrcResult switch at end with its new CRC instead.
	uint32_t crcResult = GetCRC32(GetModuleHandle(nullptr), 0x400);
	NesicaKey = NesicaKey::None;
	switch (crcResult)
	{
#if _M_IX86
	case 0xb8f2836d:
		currentGame = GameID::Daytona3;
		break;
	case 0x4bcfbc4f:
		currentGame = GameID::GrooveCoaster2;
		isNesica = true;
		break;
	case 0xcb4ab9d6:
		currentGame = GameID::Nesica;
		NesicaKey = NesicaKey::USF4;
		isNesica = true;
		break;
	case 0x385ad385:
		currentGame = GameID::UltraStreetFighterIVDevExe;
		NesicaKey = NesicaKey::USF4;
		isNesica = true;
		break;
	case 0x1d396bf2:
		currentGame = GameID::MagicalBeat;
		NesicaKey = NesicaKey::MagicalBeat;
		isNesica = true;
		break;
	case 0x2809c7c0: // Ikaruga
	case 0xef4c3efd:
		currentGame = GameID::Nesica;
		NesicaKey = NesicaKey::None;
		isNesica = true;
		break;
	case 0x6e67076f:
		currentGame = GameID::Nesica;
		NesicaKey = NesicaKey::BlazBlueCentralFiction;
		isNesica = true;
		break;
	case 0xd98bed41: // Crimzon Clover
		currentGame = GameID::CrimzonClover;
		NesicaKey = NesicaKey::None;
		isNesica = true;
		break;
	case 0xd32f9ec1: // Goketsuji Ichizoku - Matsuri Senzo Kuyo
		currentGame = GameID::Nesica;
		NesicaKey = NesicaKey::None;
		isNesica = true;
		break;
	case 0xde1b6797:
	case 0xaff18f9b:
	case 0x8f4d9755: // 302510 I/O ERROR
		currentGame = GameID::Nesica;
		NesicaKey = NesicaKey::BlazBlueChronoPhantasma;
		isNesica = true;
		break;
	case 0x5c095b93: // Daemon Bride
		currentGame = GameID::Nesica;
		NesicaKey = NesicaKey::BlazBlueChronoPhantasma;
		isNesica = true;
		break;
	case 0x05e53803: // KOF XIII Climax
		currentGame = GameID::Nesica;
		NesicaKey = NesicaKey::KOFXIIIClimax;
		isNesica = true;
		break;
	case 0x949efa20:
		currentGame = GameID::Nesica;
		NesicaKey = NesicaKey::Persona4Arena;
		GameDetect::enableNesysEmu = false;
		isNesica = true;
		break;
	case 0x6b233485:
	case 0xec97cef0:
		currentGame = GameID::Nesica;
		NesicaKey = NesicaKey::Persona4Ultimix;
		isNesica = true;
		break;
	case 0xffe46764: // Akai Katana Shin
	case 0x48362a6a:
	case 0x2e65f7ad:
		currentGame = GameID::Nesica;
		NesicaKey = NesicaKey::None;
		isNesica = true;
		break;
	case 0x9369715e: // SF 3rd Strike
	case 0xd3f62267: // test.exe
		currentGame = GameID::StreetFighter3rdStrike;
		NesicaKey = NesicaKey::None;
		isNesica = true;
		GameDetect::enableNesysEmu = false;
		break;
	case 0xae41d855:
		currentGame = GameID::Nesica;
		NesicaKey = NesicaKey::None;
		isNesica = true;
		break;
	case 0x28503c4c: // Nitroplus Blasterz 303910
	case 0x6673e73b:
		currentGame = GameID::Nesica;
		NesicaKey = NesicaKey::None;
		isNesica = true;
		break;
	case 0x57c889e6: // Space Invaders
		currentGame = GameID::Nesica;
		NesicaKey = NesicaKey::None;
		isNesica = true;
		break;
	case 0x62c933d9: // Strania - The Stella Machina
		currentGame = GameID::Nesica;
		NesicaKey = NesicaKey::None;
		isNesica = true;
		break;
	case 0x549627f4: // Aquapazza Aquaplus Dream Match
		currentGame = GameID::Nesica;
		NesicaKey = NesicaKey::None;
		isNesica = true;
		break;
	case 0xc0cbafd3: // Do Not Fall - Run for Your Drink
		currentGame = GameID::DoNotFall;
		NesicaKey = NesicaKey::None;
		isNesica = true;
		break;
	case 0x87670e0f: // Elevator Action
		currentGame = GameID::Nesica;
		NesicaKey = NesicaKey::None;
		isNesica = true;
		break;
	case 0x29025abc: // En-Eins Perfektewelt
		currentGame = GameID::Nesica;
		NesicaKey = NesicaKey::None;
		isNesica = true;
		break;
	case 0x3a998b11: // Rastan Saga
		currentGame = GameID::Nesica;
		NesicaKey = NesicaKey::None;
		isNesica = true;
		break;
	case 0x358ec3ff: // Puzzle Bobble
		currentGame = GameID::PuzzleBobble;
		NesicaKey = NesicaKey::None;
		isNesica = true;
		break;
	case 0xabc41e0a: // Homura
		currentGame = GameID::HomuraNesica;
		NesicaKey = NesicaKey::None;
		isNesica = true;
		break;
	case 0x19ba6a0c: // Vampire Savior - The Lord of Vampire
	case 0x7cc54950: // Test.exe
		currentGame = GameID::VampireSavior;
		NesicaKey = NesicaKey::None;
		isNesica = true;
		break;
	case 0xf6e33d2b: // Exception
		currentGame = GameID::Exception;
		NesicaKey = NesicaKey::None;
		isNesica = true;
		break;
	case 0x6496a142: // Hyper Street Fighter II
	case 0xb686d3ac: // Test.exe
		currentGame = GameID::HyperStreetFighterII;
		NesicaKey = NesicaKey::None;
		isNesica = true;
		GameDetect::enableNesysEmu = false;
		break;
	case 0x6e5c8b5b: // Street Fighter Zero 3
	case 0x609d8b35: // Test.exe
		currentGame = GameID::StreetFigherZero3;
		NesicaKey = NesicaKey::None;
		isNesica = true;
		GameDetect::enableNesysEmu = false;
		break;
	case 0xdd275161: // Sugoi! Arcana Heart 2 (Nesica)
		currentGame = GameID::Nesica;
		NesicaKey = NesicaKey::None;
		isNesica = true;
		break;
	case 0x42243e28: // Arcana Heart 2
		currentGame = GameID::Nesica;
		NesicaKey = NesicaKey::None;
		isNesica = true;
		break;
	case 0x47f8fb43: // Arcana Heart 3 - LOVE MAX SIX STARS!!!!!!
		currentGame = GameID::Nesica;
		NesicaKey = NesicaKey::None;
		isNesica = true;
		break;
	case 0x5bb8a4ca: // Raiden III
		currentGame = GameID::Nesica;
		NesicaKey = NesicaKey::None;
		isNesica = true;
		break;
	case 0xd0fea58c: // Raiden IV
		currentGame = GameID::RaidenIVNesica;
		NesicaKey = NesicaKey::None;
		isNesica = true;
		break;
	case 0xe378e178: // Senko no Ronde DUO
		currentGame = GameID::SenkoNoRondeDuoNesica;
		NesicaKey = NesicaKey::None;
		isNesica = true;
		break;
	case 0x5503983f: // Trouble Witches AC - Amalgam no Joutachi
		currentGame = GameID::TroubleWitchesNesica;
		NesicaKey = NesicaKey::None;
		isNesica = true;
		break;
	case 0x6DA10E60:
		currentGame = GameID::FordRacing;
		break;
	case 0x2B41A7EA:
		currentGame = GameID::SR3;
		break;
	case 0x42b00124:
		currentGame = GameID::MB4;
		X2Type = X2Type::MB4;
		break;
	case 0x47da80e5: // Spica Adventure
		currentGame = GameID::SpicaAdventure;
		X2Type = X2Type::Generic;
		break;
	case 0xf9297ecb: // BlazBlue
		currentGame = GameID::BlazBlue;
		X2Type = X2Type::BlazBlue;
		break;
	case 0xee568daa: // BlazBlue Continuum Shift II
		currentGame = GameID::BlazBlueCS2;
		X2Type = X2Type::Generic;
		break;
	case 0x4414288e: // BlazBlue Continuum Shift
		currentGame = GameID::BlazBlueCS;
		X2Type = X2Type::Generic;
		break;
	case 0xe064ed48: // Giga Wing Generations
		currentGame = GameID::GigaWingGenerations;
		X2Type = X2Type::Generic;
		break;
	case 0xc336abc8: // King of Fighter 98 Unlimited Match
		currentGame = GameID::KOF98UM;
		X2Type = X2Type::Generic;
		break;
	case 0x1782f027: // KOF MIRA
		currentGame = GameID::KOFMIRA;
		X2Type = X2Type::Generic;
		break;
	case 0x15c690de: // KOF SkyStage
	case 0xc0e5e71c: // KOF SkyStage
		currentGame = GameID::KOFSkyStage;
		X2Type = X2Type::Generic;
		break;
	case 0x9603cd6e: // KOF XII
		currentGame = GameID::KOFXII;
		X2Type = X2Type::Generic;
		break;
	case 0x3de0a15a: // KOF XIII
		currentGame = GameID::KOFXIII;
		NesicaKey = NesicaKey::KOFXIIIClimax;
		X2Type = X2Type::Generic;
		break;
	case 0x777df862: // Power Instinct V
		currentGame = GameID::PowerInstinctV;
		X2Type = X2Type::Generic;
		break;
	case 0x4aafd08c: // Raiden III
		currentGame = GameID::RaidenIII;
		X2Type = X2Type::Generic;
		break;
	case 0x6c2d2657: // Senko No Ronde Duo
		currentGame = GameID::SenkoNoRondeDuo;
		X2Type = X2Type::Generic;
		break;
	case 0xb0e19321: // Shigami 3
		currentGame = GameID::Shigami3;
		X2Type = X2Type::Generic;
		break;
	case 0xddd19e50: // Street Fighter IV
		currentGame = GameID::SF4;
		X2Type = X2Type::Generic;
		break;
	case 0x5a5526b2: // Street Fighter IV Arcade Edition
		currentGame = GameID::SSFAE;
		X2Type = X2Type::Generic;
		break;
	case 0x282de00f: // Street Fighter IV Arcade Edition EXP
		currentGame = GameID::SSFAE_EXP;
		X2Type = X2Type::Generic;
		break;
	case 0xef843fd1: // Super Street Fighter IV Arcade Edition Ver. 2012 (NESYS 1.1)
		currentGame = GameID::SSFAE2012;
		X2Type = X2Type::Generic;
		break;
	case 0xcb2c2568: // Trouble Witches
		currentGame = GameID::TroubleWitches;
		X2Type = X2Type::Generic;
		break;
	case 0xf795d548: // Tetris The Grand Master 3 Terror Instinct
		currentGame = GameID::TetrisGM3;
		X2Type = X2Type::Generic;
		break;
	case 0xdca0c880: // Chaos Breaker
		currentGame = GameID::ChaosBreaker;
		X2Type = X2Type::Generic;
		break;
	case 0xf2d38452: // Taisen Hot Gimmick 5
		currentGame = GameID::TaisenHotGimmick5;
		X2Type = X2Type::Generic;
		break;
	case 0x9646805d: // Samurai Spirits Sen
		currentGame = GameID::SamuraiSpiritsSen;
		X2Type = X2Type::Generic;
		break;
	case 0x185fd019: // Wacky Races
		currentGame = GameID::WackyRaces;
		X2Type = X2Type::Generic;
		break;
	case 0x1f2f9497: // Chase HQ 2
		currentGame = GameID::ChaseHq2;
		X2Type = X2Type::Generic;
		break;
	case 0x45121869: // Virtua R-Limit
		currentGame = GameID::VirtuaRLimit;
		X2Type = X2Type::VRL;
		break;
	case 0x7b43b0dc: // Raiden IV
		currentGame = GameID::RaidenIV;
		X2Type = X2Type::Raiden4;
		break;
	case 0x14eb3c8a: // Battle Gear 4 Tuned Professional (2.08)
		currentGame = GameID::BG4;
		X2Type = X2Type::BG4;
		break;
	case 0xf5d90f01: // Music GunGun 2
		currentGame = GameID::TypeXGeneric;
		break;
	case 0xabcebc43: // Lupin 3
		currentGame = GameID::TypeXGeneric;
		X2Type = X2Type::Lupin3;
		break;
	case 0xcdf01376: // Battle Fantasia
		currentGame = GameID::TypeXGeneric;
		X2Type = X2Type::BattleFantasia;
		break;
	case 0x521d6765: // Arcana Heart 3
		currentGame = GameID::ExBoardGeneric;
		break;
	case 0x581aa812: // Daemon Bride
		currentGame = GameID::ExBoardGeneric;
		break;
	case 0xbb359a1a: // Suggoi! Arcana Heart 2
		currentGame = GameID::ExBoardGeneric;
		break;
	case 0x4966e11b:
		currentGame = GameID::GRID;
		break;
	case 0x326ebcd6:
		currentGame = GameID::GTIClub3;
		break;
	case 0xe158763c:
		currentGame = GameID::TypeXGeneric;
		X2Type = X2Type::Wontertainment;
		break;
	case 0x47922b80: // FNF Drift
		currentGame = GameID::FNFDrift;
		break;
	case 0x4929f3ca: // FNF SuperCars
		currentGame = GameID::FNFSC;
		break;
	case 0x93888a27: // FNF
		currentGame = GameID::FNF;
		break;
	case 0x6449d9b1: // FNF SuperBikes
		currentGame = GameID::FNFSB;
		break;
	case 0x6214f7ec: // Guitar Hero Arcade
		currentGame = GameID::GHA;
		break;
		//case 0xea1984ff:
		//	currentGame = GameID::ExBoardGeneric;
		//	break;
#endif
	default:
		auto moduleBase = (uintptr_t)GetModuleHandle(nullptr);
		// SPECIAL BYTE PATTERNS (IF PE CHANGES DUE TO DYNAMIC IMAGEBASE)
#if _M_IX86
		if (*(uint32_t*)(moduleBase + 0x2182) == 0xE995C969)
		{
			currentGame = GameID::MachStorm;
			break;
		}
		if (*(uint32_t*)(moduleBase + 0xABB7) == 0x00E445C7 &&
			*(uint32_t*)(moduleBase + 0xABB7 + 4) == 0xC7000100)
		{
			currentGame = GameID::Daytona3;
			break;
		}
#if !_DEBUG
		// IF GAME = JusticeLeague (if workingdir\JLA.exe exists) , AVOID THIS CHECK (note: darius checked offset is beyond JLA exe limits and TP crashes...)
		char working_directory[MAX_PATH + 1];
		GetCurrentDirectoryA(sizeof(working_directory), working_directory);
		std::string JLAexestr0 = working_directory;
		std::string JLAexestr = JLAexestr0 + "\\JLA.exe";
		bool JLAexists(std::filesystem::exists(JLAexestr.c_str()));
		if (JLAexists == false)
		{
			if (*(uint32_t*)(moduleBase + 0x2CC751) == 0x6B75C084)
			{
				currentGame = GameID::DariusBurst;
				NesicaKey = NesicaKey::DariusBurst;
				break;
			}
			if (*(uint32_t*)(moduleBase + 0x302741) == 0x7075C084)
			{
				currentGame = GameID::DariusBurst116;
				NesicaKey = NesicaKey::DariusBurst;
				break;
			}
		}
#endif
#else
		// X64
		// School of Ragnarok
		//if (*(uint32_t *)(moduleBase + 0x396E) == 0x2180B841)
		//{
		//	currentGame = GameID::SchoolOfRagnarok;
		//	isNesica = true;
		//}
		// PATCHES 0-9
		if (*(uint32_t*)(moduleBase + 0x2F00) == 0xFFCB8B48)
		{
			currentGame = GameID::WMMT5;
			break;
		}
		// PATCHES 10-21
		if (*(uint32_t*)(moduleBase + 0x2F00) == 0x084AFF48)
		{
			currentGame = GameID::WMMT5;
			break;
		}
#endif
		// Craft CRC detection without virtual address
		memcpy(newCrc, GetModuleHandle(nullptr), 0x400);
		DWORD pePTR = *(DWORD*)(newCrc + 0x3C);

		// Overwrite ImageBase with 8 bytes of 0
		*(DWORD*)(newCrc + pePTR + 0x18) = 0x00000000;
		*(DWORD*)(newCrc + pePTR + 0x18 + 4) = 0x00000000;
#ifdef _AMD64_
		* (DWORD*)(newCrc + pePTR + 50) = 0x00000000;
#endif
		* (DWORD*)(newCrc + pePTR + 54) = 0x00000000;
		uint32_t newCrcResult = GetCRC32(newCrc, 0x400);

		// Pokken update 08-26
		if (*(uint32_t*)(moduleBase + 0x1C04) == 0x7401C3F6)
		{
			if (newCrcResult == 0x4D4F1EBF)
				currentGame = GameID::PokkenTournament26;
			else
				currentGame = GameID::PokkenTournament;

			break;
		}

		switch (newCrcResult)
		{
		case 0xfe7afff4:
			currentGame = GameID::FNFSB2;
			break;
		case 0x72c27333:
			currentGame = GameID::JLeague;
			break;
		case 0x43de771f: // 3D Cosplay Mahjong (runs in windowed mode permanently for some reason)
			currentGame = GameID::Nesica;
			NesicaKey = NesicaKey::None;
			isNesica = true;
			break;
		case 0x04ce5f18: // Akai Katana Shin for NESiCAxLive
			currentGame = GameID::Nesica;
			NesicaKey = NesicaKey::None;
			isNesica = true;
			break;
		case 0xaf3d84cc: //Battle Fantasia Network Edition for NESiCAxLive (I/O not working)
			currentGame = GameID::Nesica;
			NesicaKey = NesicaKey::None;
			isNesica = true;
			break;
		case 0xac86efec: //Chaos Code v1.03
			currentGame = GameID::ChaosCode;
			NesicaKey = NesicaKey::None;
			isNesica = true;
			break;
		case 0xa742a607: //Chaos Code v2.11
			currentGame = GameID::ChaosCode;
			NesicaKey = NesicaKey::None;
			isNesica = true;
			break;
		case 0xb208e63a: //Dragon Dance game_liong.exe (game.exe just crashes but starts this program anyway)
			currentGame = GameID::Nesica;
			NesicaKey = NesicaKey::None;
			isNesica = true;
			break;
		case 0xbe9c0407: //Skullgirls 2nd Encore
			currentGame = GameID::SkullGirls;
			NesicaKey = NesicaKey::NoNet;
			isNesica = true;
			break;
		case 0x1046a695: //Spica Adventure for NXL (honestly no difference i can find from OG version, X2 emu works fine for it)
			currentGame = GameID::SpicaAdventureNXL;
			X2Type = X2Type::Generic;
			break;
		case 0xbd516d7b: // KOFXIII Climax
			currentGame = GameID::KOFXIIIClimax;
			NesicaKey = NesicaKey::KOFXIIIClimax;
			X2Type = X2Type::Generic;
			break;
		case 0x3806e8f4: //KoF98 (has files that need to be deleted to run)
			currentGame = GameID::KOF98Nesica;
			NesicaKey = NesicaKey::None;
			isNesica = true;
			break;
		case 0x545b0d0c: // Super Street Fighter IV Arcade Edition Ver. 2012 (NESYS 1.3)
			currentGame = GameID::SSFAE2012;
			X2Type = X2Type::Generic;
			break;
		case 0xa202d660: //Yatagarasu - Attack on Cataclysm
			currentGame = GameID::Yatagarasu;
			NesicaKey = NesicaKey::None;
			isNesica = true;
			break;
		case 0xbd4c12a5: // Rumble Fish 2
			currentGame = GameID::RumbleFish2;
			NesicaKey = NesicaKey::None;
			isNesica = true;
			break;
		case 0xc5715d2e: //Ikaruga NXL
			currentGame = GameID::Nesica;
			NesicaKey = NesicaKey::None;
			isNesica = true;
			break;
		case 0xafe8fc50: // KOF2002
			currentGame = GameID::KOF2002;
			NesicaKey = NesicaKey::None;
			isNesica = true;
			break;
		case 0xed9b5740:
			currentGame = GameID::H2Overdrive;
			break;
		case 0x8456EEC1:
			currentGame = GameID::DirtyDrivin;
			break;
		case 0x4D91A27A:
		case 0x31a4f2d0:
			currentGame = GameID::SnoCross;
			break;
		case 0x28b99e8d:
			currentGame = GameID::RadikalBikers; //3rd Party using Aaron Giles Emulator
			break;
		case 0xbd8c984d: // Battle Gear 4 English Ver (2.03) 
			currentGame = GameID::BG4_Eng;
			X2Type = X2Type::BG4_Eng;
			break;
		case 0x74f898ae: // KOF SkyStage
			currentGame = GameID::KOFSkyStage100J;
			X2Type = X2Type::Generic;
			break;
		case 0xd9221042: // Power Instinct V
			currentGame = GameID::PowerInstinctV;
			X2Type = X2Type::Generic;
			break;
		case 0xee586431: // Chaos Breaker for NesicaxLive
			currentGame = GameID::ChaosBreakerNXL;
			NesicaKey = NesicaKey::None;
			isNesica = true;
			break;
		case 0x486e885c: // Dark Awake - The King Has No Name
			currentGame = GameID::DarkAwake;
			NesicaKey = NesicaKey::None;
			isNesica = true;
			break;
		case 0x31e72d72: // BlazBlue Central Fiction 2.01
			currentGame = GameID::BlazBlueCF201;
			NesicaKey = NesicaKey::BlazBlueCentralFiction;
			isNesica = true;
			break;
#ifdef _AMD64_
		case 0x80ebd207:
			currentGame = GameID::Theatrhythm;
			break;
		case 0xdb9c3a90:
			currentGame = GameID::TER;
			break;
		case 0xf3d3f699:
			currentGame = GameID::StarWarsEs3XLauncher;
			break;
		case 0x5424a6d8:
			currentGame = GameID::StarWarsJapEs3XLauncher;
			break;
		case 0x8505c794:
			currentGame = GameID::StarWarsEs3X;
			break;
		case 0xe1e9e32c: // JPN
			currentGame = GameID::StarWarsJapEs3X;
			break;
		case 0x30F676AD:
			currentGame = GameID::SchoolOfRagnarok;
			isNesica = true;
			break;
		case 0x00ed2300:
			currentGame = GameID::Tekken7Update12;
			break;
		case 0xce9718fd:
			currentGame = GameID::Tekken7Update00;
			break;
		case 0xC017F0BE: // 00 doesn't work, broken dump?
		case 0x17059CF3: // 01
		case 0xE325036F: // 02
		case 0x652FEE7D: // 03
		case 0x246B5F7E: // 04
		case 0x94D16CCC: // 05
		case 0x3CC1BE43: // 06
		case 0x247B6F8C: // 07
			currentGame = GameID::PokkenTournament;
			break;
		case 0xcb9c4353:
			currentGame = GameID::Pengoe5_Test;
			break;
		case 0x0f98a7a2:
			currentGame = GameID::Pengoe5;
			break;
		case 0x6fc27eed: // Original
		case 0x3b3fc3ab: // Other exe
			currentGame = GameID::VF5Esports;
			break;
		case 0x1ab0f981:
			currentGame = GameID::GoonyaFighter;
			break;
		case 0x8c30fa5a:
			currentGame = GameID::PuyoPuyoEsports;
			break;
#endif
		default:
#ifdef _DEBUG
			info(true, "---------------------------------");
			info(true, "New CRC: %08x not implemented", newCrcResult);
			info(true, "---------------------------------");
#else
			memset(errorBuffer, 0, 256);
			sprintf_s(errorBuffer, 256, "Unsupported Executable, NEW CRC: %08X!", newCrcResult);
			MessageBoxA(0, errorBuffer, "Error", MB_ICONERROR);
			ExitProcess(0);
#endif
			break;
		}
		break;
	}
}
bool GameDetect::IsNesicaGame()
{
	return isNesica;
}

bool GameDetect::IsTypeX()
{
	switch (GameDetect::currentGame)
	{
	case GameID::TypeXGeneric:
	case GameID::SpicaAdventure:
	case GameID::BlazBlue:
	case GameID::BlazBlueCS:
	case GameID::BlazBlueCS2:
	case GameID::GigaWingGenerations:
	case GameID::KOF98UM:
	case GameID::KOFXIIIClimax:
	case GameID::KOFMIRA:
	case GameID::KOFSkyStage:
	case GameID::KOFXII:
	case GameID::KOFXIII:
	case GameID::KOFSkyStage100J:
	case GameID::PowerInstinctV:
	case GameID::RaidenIII:
	case GameID::SenkoNoRondeDuo:
	case GameID::Shigami3:
	case GameID::SF4:
	case GameID::SSFAE:
	case GameID::SSFAE_EXP:
	case GameID::SSFAE2012:
	case GameID::TroubleWitches:
	case GameID::TetrisGM3:
	case GameID::ChaosBreaker:
	case GameID::TaisenHotGimmick5:
	case GameID::SamuraiSpiritsSen:
	case GameID::WackyRaces:
	case GameID::ChaseHq2:
	case GameID::BG4:
	case GameID::RaidenIV:
	case GameID::VirtuaRLimit:
	case GameID::MB4:
	case GameID::BG4_Eng:
		return true;
	default:
		return false;
	}
}
GameID GameDetect::currentGame;
#pragma optimize("", on)