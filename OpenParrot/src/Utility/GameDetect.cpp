#include <StdInc.h>
#include "GameDetect.h"
#include <filesystem>
#pragma optimize("", off)
bool GameDetect::isNesica = false;
bool GameDetect::enableNesysEmu = true;
NesicaKey GameDetect::NesicaKey;
X2Type GameDetect::X2Type = X2Type::None;
static char newCrc[0x400];

void GameDetect::DetectCurrentGame()
{
	uint32_t crcResult = GetCRC32(GetModuleHandle(nullptr), 0x400);
	NesicaKey = NesicaKey::None;
	switch (crcResult)
	{
#if _M_IX86
	case 0xb8f2836d:
		currentGame = GameID::Daytona3;
		break;
	case 0xd3f62267:
		currentGame = GameID::Nesica;
		NesicaKey = NesicaKey::None;
		isNesica = true;
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
		NesicaKey = NesicaKey::BlazBlueCentralFriction;
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
		// SF 3rd Strike
	case 0x9369715e:
		currentGame = GameID::Nesica;
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
		currentGame = GameID::Nesica;
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
		currentGame = GameID::Nesica;
		NesicaKey = NesicaKey::None;
		isNesica = true;
		break;
	case 0x19ba6a0c: // Vampire Savior - The Lord of Vampire
	case 0x7cc54950: // Test.exe
		currentGame = GameID::Nesica;
		NesicaKey = NesicaKey::None;
		isNesica = true;
		GameDetect::enableNesysEmu = false;
		break;
	case 0xf6e33d2b: // Exception
		currentGame = GameID::Nesica;
		NesicaKey = NesicaKey::None;
		isNesica = true;
		break;
	case 0x6496a142: // Hyper Street Fighter II
	case 0xb686d3ac: // Test.exe
		currentGame = GameID::Nesica;
		NesicaKey = NesicaKey::None;
		isNesica = true;
		GameDetect::enableNesysEmu = false;
		break;
	case 0x6e5c8b5b: // Street Fighter Zero 3
	case 0x609d8b35: // Test.exe
		currentGame = GameID::Nesica;
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
		currentGame = GameID::Nesica;
		NesicaKey = NesicaKey::None;
		isNesica = true;
		break;
	case 0xe378e178: // Senko no Ronde DUO
		currentGame = GameID::Nesica;
		NesicaKey = NesicaKey::None;
		isNesica = true;
		break;
	case 0x5503983f: // Trouble Witches AC - Amalgam no Joutachi
		currentGame = GameID::Nesica;
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
		X2Type = X2Type::Generic;
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
	case 0xef843fd1: // Super Street Fighter IV Arcade Edition Ver. 2012
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
	case 0x14eb3c8a: // Battle Gear 4 Tuned Professional
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
				break;
			}
		}
#else
		// X64
		// School of Ragnarok
		if (*(uint32_t *)(moduleBase + 0x396E) == 0x2180B841)
		{
			currentGame = GameID::SchoolOfRagnarok;
			isNesica = true;
		}
		if (*(uint32_t*)(moduleBase + 0x1C04) == 0x7401C3F6)
		{
			currentGame = GameID::PokkenTournament;
			break;
		}
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
		switch (newCrcResult)
		{
		case 0xfe7afff4:
			currentGame = GameID::FNFSB2;
			break;
		case 0x72c27333:
			currentGame = GameID::JLeague;
			break;
		default:
#ifdef _DEBUG
			info(true, "---------------------------------");
			info(true, "New CRC: %08x not implemented", newCrcResult);
			info(true, "---------------------------------");
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
	case GameID::KOFMIRA:
	case GameID::KOFSkyStage:
	case GameID::KOFXII:
	case GameID::KOFXIII:
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
		return true;
	default:
		return false;
	}
}
GameID GameDetect::currentGame;
#pragma optimize("", on)