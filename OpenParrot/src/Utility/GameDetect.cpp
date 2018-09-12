#include <StdInc.h>
#include "GameDetect.h"
#pragma optimize("", off)
bool GameDetect::isNesica = false;
bool GameDetect::enableNesysEmu = true;
NesicaKey GameDetect::NesicaKey;
X2Type GameDetect::X2Type = X2Type::None;

void GameDetect::DetectCurrentGame()
{
	uint32_t crcResult = GetCRC32(GetModuleHandle(nullptr), 0x400);
	NesicaKey = NesicaKey::None;
	switch (crcResult)
	{
#if _M_IX86
	case 0xcb4ab9d6:
		currentGame = GameID::Nesica;
		NesicaKey = NesicaKey::USF4;
		isNesica = true;
		break;
	case 0x1d396bf2:
		currentGame = GameID::Nesica;
		NesicaKey = NesicaKey::MagicalBeat;
		isNesica = true;
		break;
	case 0x6e67076f:
		currentGame = GameID::Nesica;
		NesicaKey = NesicaKey::BlazBlueCentralFriction;
		isNesica = true;
		break;
	case 0xde1b6797:
	case 0xaff18f9b:
		currentGame = GameID::Nesica;
		NesicaKey = NesicaKey::BlazBlueChronoPhantasma;
		isNesica = true;
		break;
	case 0x05e53803:
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
		currentGame = GameID::Nesica;
		NesicaKey = NesicaKey::None;
		isNesica = true;
		break;
		// SF 3rd Strike
	case 0x9369715e:
		currentGame = GameID::Nesica;
		NesicaKey = NesicaKey::None;
		isNesica = true;
		break;
	case 0xae41d855:
		currentGame = GameID::Nesica;
		NesicaKey = NesicaKey::None;
		isNesica = true;
		break;
	case 0x28503c4c: // Nitroplus Blasterz 303910
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
#endif

#ifdef _DEBUG
		info(true, "%08x not implemented", crcResult);
#endif
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
			return true;
		default:
			return false;
	}
}
GameID GameDetect::currentGame;
#pragma optimize("", on)