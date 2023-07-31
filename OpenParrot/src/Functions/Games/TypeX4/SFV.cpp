#if __has_include(<atlstr.h>)
#include <StdInc.h>
#include "Utility/InitFunction.h"
#include "Functions/Global.h"
#include <Functions\Nesica_Libs\NesysEmu.h> 
#include <Functions\Nesica_Libs\RegHooks.h> 
#include <windows.h>
#include <atlstr.h> 
#include <string>
#include <iostream>
#include <shlobj.h>
#include <fstream>
#include <sstream>
#include <filesystem>
#include <stdint.h>

#if _M_AMD64
#pragma optimize("", off)
static DWORD_PTR imageBase = 0;
static wchar_t FullPathWithExeName[MAX_PATH];
static wchar_t ResX[16];
static wchar_t ResY[16];
extern int* ffbOffset;

std::string to_hex_string(DWORD_PTR i) {
	std::stringstream s;
	s << std::hex << i;
	return s.str();
}

// TODO: The emulation code should be moved to a separate file and this code should be used for all nxl2 games.

__int64 (WINAPI* g_origUIO_AMIC_Close)();
__int64 (WINAPI* g_origUIO_AMIC_GetDeviceVersion)();
__int64 (WINAPI* g_origUIO_AMIC_GetStatus)();
__int64 (WINAPI* g_origUIO_AMIC_IsPadMode)();
__int64 (WINAPI* g_origUIO_AMIC_Open)();
__int64 (WINAPI* g_origUIO_AMIC_Update)(__int64 a1);
__int64 (WINAPI* g_origUIO_AMIC_WdtClear)();
__int64 (WINAPI* g_origUIO_AMIC_WdtDisable)();
__int64 (WINAPI* g_origUIO_AMIC_WdtEnable)(unsigned __int16 a1, __int64 a2);
__int64 (WINAPI* g_origUIO_AMIC_WdtGetDIP)();

__int64 (WINAPI* g_origNESiCAUtilLibIsOnlineMode)();
__int64 (WINAPI* g_origNESiCAUtilLibGetStatus)();
__int64 (WINAPI* g_origNESiCAUtilLibDelete)();
__int64 (WINAPI* g_origNESiCAUtilLibGetCenterInformation)();
__int64 (WINAPI* g_origNESiCAUtilLibGetInformation)();
__int64 (WINAPI* g_origNESiCAUtilLibGetPlayerAskInformation)();
__int64 (WINAPI* g_origNESiCAUtilLibGetPlayerInformation)();
__int64 (WINAPI* g_origNESiCAUtilLibGetPlayerTime)();
__int64 (WINAPI* g_origNESiCAUtilLibGetStatusStringUTF16)();
__int64 (WINAPI* g_origNESiCAUtilLibInit)();
__int64 (WINAPI* g_origNESiCAUtilLibIsExistInstance)();
__int64 (WINAPI* g_origNESiCAUtilLibUpdate)();
__int64 (WINAPI* g_origNESiCAUtilLib_NESiCA_BData_Get)();
__int64 (WINAPI* g_origNESiCAUtilLib_NESiCA_BData_Set)();
__int64 (WINAPI* g_origNESiCAUtilLib_NESiCA_BData_SetDefault)();
__int64 (WINAPI* g_origNESiCAUtilLib_NESiCA_TData_GetBuf)();
__int64 (WINAPI* g_origNESiCAUtilLib_NESiCA_TData_GetSize)();
__int64 (WINAPI* g_origBeginPlay)();
__int64 (WINAPI* g_origContinuePlay)();
__int64 (WINAPI* g_origEndPlay)();
__int64 (WINAPI* g_origGetGlobalIP)();
__int64 (WINAPI* g_origGetKeyConfig)();
__int64 (WINAPI* g_origGetLanInterface)();
__int64 (WINAPI* g_origGetLibraryVersion)();
__int64 (WINAPI* g_origGetNWInfo)();
__int64 (WINAPI* g_origGetNewsImagePath)();
__int64 (WINAPI* g_origGetRanking)(signed int a1, __int64 a2, signed int a3, DWORD_PTR a4, DWORD_PTR a5);
__int64 (WINAPI* g_origGetResponseTime)();
__int64 (WINAPI* g_origGetServerInfo)();
__int64 (WINAPI* g_origGetServiceVersion)();
__int64 (WINAPI* g_origGetTenpoInfo)();
__int64 (WINAPI* g_origIsBeginningProcessFinished)();
__int64 (WINAPI* g_origIsDisactive)();
__int64 (WINAPI* g_origIsEndingProcessFinished)();
__int64 (WINAPI* g_origIsEventMode)();
__int64 (WINAPI* g_origIsReadyDownloadRanking)();
__int64 (WINAPI* g_origIsTestMode)();
__int64 (WINAPI* g_origIsUploaded)();
__int64 (WINAPI* g_origLockProcess)();
__int64 (WINAPI* g_origNESiCA_Card_GetDetailCharacterBattlePointMonth)();
__int64 (WINAPI* g_origNESiCA_Card_GetDetailCharacterBattlePointTotal)();
__int64 (WINAPI* g_origNESiCA_Card_GetDetailCharacterDrawCountMonth)();
__int64 (WINAPI* g_origNESiCA_Card_GetDetailCharacterDrawCountTotal)();
__int64 (WINAPI* g_origNESiCA_Card_GetDetailCharacterLastColorID)();
__int64 (WINAPI* g_origNESiCA_Card_GetDetailCharacterLastComment)();
__int64 (WINAPI* g_origNESiCA_Card_GetDetailCharacterLastCostumeID)();
__int64 (WINAPI* g_origNESiCA_Card_GetDetailCharacterLastVTrigger)();
__int64 (WINAPI* g_origNESiCA_Card_GetDetailCharacterLoseCountMonth)();
__int64 (WINAPI* g_origNESiCA_Card_GetDetailCharacterLoseCountTotal)();
__int64 (WINAPI* g_origNESiCA_Card_GetDetailCharacterUseCountMonth)();
__int64 (WINAPI* g_origNESiCA_Card_GetDetailCharacterUseCountTotal)();
__int64 (WINAPI* g_origNESiCA_Card_GetDetailCharacterWinCountMonth)();
__int64 (WINAPI* g_origNESiCA_Card_GetDetailCharacterWinCountTotal)();
__int64 (WINAPI* g_origNESiCA_Card_GetDetailControlType)();
__int64 (WINAPI* g_origNESiCA_Card_GetDetailCurrentWins)();
__int64 (WINAPI* g_origNESiCA_Card_GetDetailHighScore)();
__int64 (WINAPI* g_origNESiCA_Card_GetDetailHighScoreCharacterID)();
__int64 (WINAPI* g_origNESiCA_Card_GetDetailKeyConfigHK)();
__int64 (WINAPI* g_origNESiCA_Card_GetDetailKeyConfigHP)();
__int64 (WINAPI* g_origNESiCA_Card_GetDetailKeyConfigHPHK)();
__int64 (WINAPI* g_origNESiCA_Card_GetDetailKeyConfigK3)();
__int64 (WINAPI* g_origNESiCA_Card_GetDetailKeyConfigLK)();
__int64 (WINAPI* g_origNESiCA_Card_GetDetailKeyConfigLKMK)();
__int64 (WINAPI* g_origNESiCA_Card_GetDetailKeyConfigLP)();
__int64 (WINAPI* g_origNESiCA_Card_GetDetailKeyConfigLPLK)();
__int64 (WINAPI* g_origNESiCA_Card_GetDetailKeyConfigLPMK)();
__int64 (WINAPI* g_origNESiCA_Card_GetDetailKeyConfigLPMP)();
__int64 (WINAPI* g_origNESiCA_Card_GetDetailKeyConfigMK)();
__int64 (WINAPI* g_origNESiCA_Card_GetDetailKeyConfigMP)();
__int64 (WINAPI* g_origNESiCA_Card_GetDetailKeyConfigMPLK)();
__int64 (WINAPI* g_origNESiCA_Card_GetDetailKeyConfigMPMK)();
__int64 (WINAPI* g_origNESiCA_Card_GetDetailKeyConfigP3)();
__int64 (WINAPI* g_origNESiCA_Card_GetDetailLastCharacterID)();
__int64 (WINAPI* g_origNESiCA_Card_GetDetailLeaguePoint)();
__int64 (WINAPI* g_origNESiCA_Card_GetDetailMaxWins)();
__int64 (WINAPI* g_origNESiCA_Card_GetDetailPlayerPointMonth)();
__int64 (WINAPI* g_origNESiCA_Card_GetDetailPlayerPointTotal)();
__int64 (WINAPI* g_origNESiCA_Card_GetPlayerComment)();
__int64 (WINAPI* g_origNESiCA_Card_GetPlayerCommentUTF16)();
__int64 (WINAPI* g_origNESiCA_Card_GetPlayerMyStoreID)();
__int64 (WINAPI* g_origNESiCA_Card_GetPlayerName)();
__int64 (WINAPI* g_origNESiCA_Card_GetPlayerNameUTF16)();
__int64 (WINAPI* g_origNESiCA_Card_GetPlayerTitleID)();
__int64 (WINAPI* g_origNESiCA_Card_SetDetailCharacterBattlePointMonth)();
__int64 (WINAPI* g_origNESiCA_Card_SetDetailCharacterBattlePointTotal)();
__int64 (WINAPI* g_origNESiCA_Card_SetDetailCharacterDrawCountMonth)();
__int64 (WINAPI* g_origNESiCA_Card_SetDetailCharacterDrawCountTotal)();
__int64 (WINAPI* g_origNESiCA_Card_SetDetailCharacterLastColorID)();
__int64 (WINAPI* g_origNESiCA_Card_SetDetailCharacterLastComment)();
__int64 (WINAPI* g_origNESiCA_Card_SetDetailCharacterLastCostumeID)();
__int64 (WINAPI* g_origNESiCA_Card_SetDetailCharacterLastVTrigger)();
__int64 (WINAPI* g_origNESiCA_Card_SetDetailCharacterLoseCountMonth)();
__int64 (WINAPI* g_origNESiCA_Card_SetDetailCharacterLoseCountTotal)();
__int64 (WINAPI* g_origNESiCA_Card_SetDetailCharacterUseCountMonth)();
__int64 (WINAPI* g_origNESiCA_Card_SetDetailCharacterUseCountTotal)();
__int64 (WINAPI* g_origNESiCA_Card_SetDetailCharacterWinCountMonth)();
__int64 (WINAPI* g_origNESiCA_Card_SetDetailCharacterWinCountTotal)();
__int64 (WINAPI* g_origNESiCA_Card_SetDetailControlType)();
__int64 (WINAPI* g_origNESiCA_Card_SetDetailCurrentWins)();
__int64 (WINAPI* g_origNESiCA_Card_SetDetailHighScore)();
__int64 (WINAPI* g_origNESiCA_Card_SetDetailHighScoreCharacterID)();
__int64 (WINAPI* g_origNESiCA_Card_SetDetailKeyConfigHK)();
__int64 (WINAPI* g_origNESiCA_Card_SetDetailKeyConfigHP)();
__int64 (WINAPI* g_origNESiCA_Card_SetDetailKeyConfigHPHK)();
__int64 (WINAPI* g_origNESiCA_Card_SetDetailKeyConfigK3)();
__int64 (WINAPI* g_origNESiCA_Card_SetDetailKeyConfigLK)();
__int64 (WINAPI* g_origNESiCA_Card_SetDetailKeyConfigLKMK)();
__int64 (WINAPI* g_origNESiCA_Card_SetDetailKeyConfigLP)();
__int64 (WINAPI* g_origNESiCA_Card_SetDetailKeyConfigLPLK)();
__int64 (WINAPI* g_origNESiCA_Card_SetDetailKeyConfigLPMK)();
__int64 (WINAPI* g_origNESiCA_Card_SetDetailKeyConfigLPMP)();
__int64 (WINAPI* g_origNESiCA_Card_SetDetailKeyConfigMK)();
__int64 (WINAPI* g_origNESiCA_Card_SetDetailKeyConfigMP)();
__int64 (WINAPI* g_origNESiCA_Card_SetDetailKeyConfigMPLK)();
__int64 (WINAPI* g_origNESiCA_Card_SetDetailKeyConfigMPMK)();
__int64 (WINAPI* g_origNESiCA_Card_SetDetailKeyConfigP3)();
__int64 (WINAPI* g_origNESiCA_Card_SetDetailLastCharacterID)();
__int64 (WINAPI* g_origNESiCA_Card_SetDetailLeaguePoint)();
__int64 (WINAPI* g_origNESiCA_Card_SetDetailMaxWins)();
__int64 (WINAPI* g_origNESiCA_Card_SetDetailPlayerPointMonth)();
__int64 (WINAPI* g_origNESiCA_Card_SetDetailPlayerPointTotal)();
__int64 (WINAPI* g_origNESiCA_Card_SetPlayerComment)();
__int64 (WINAPI* g_origNESiCA_Card_SetPlayerCommentUTF16)();
__int64 (WINAPI* g_origNESiCA_Card_SetPlayerMyStoreID)();
__int64 (WINAPI* g_origNESiCA_Card_SetPlayerName)();
__int64 (WINAPI* g_origNESiCA_Card_SetPlayerNameUTF16)();
__int64 (WINAPI* g_origNESiCA_Card_SetPlayerTitleID)();
__int64 (WINAPI* g_origNESiCA_GetNESiCAID)();
__int64 (WINAPI* g_origNESiCA_GetNESiCAType)();
__int64 (WINAPI* g_origNESiCA_IsCardPlayer)();
__int64 (WINAPI* g_origNESiCA_Rank_GetBattlePointRank)();
__int64 (WINAPI* g_origNESiCA_Rank_GetBattlePointRankPref)();
__int64 (WINAPI* g_origNESiCA_Rank_GetBattlePointRankPrefId)();
__int64 (WINAPI* g_origNESiCA_Rank_GetBattlePointRankPrefName)();
__int64 (WINAPI* g_origNESiCA_Rank_GetBattlePointRankPrefNameUTF16)();
__int64 (WINAPI* g_origReconnect)();
__int64 (WINAPI* g_origSaveOnTheWay)();
__int64 (WINAPI* g_origSetCharacterID)();
__int64 (WINAPI* g_origSetEventMode)();
__int64 (WINAPI* g_origSetEventModeResult)();
__int64 (WINAPI* g_origSetOpenCloseTime)();
__int64 (WINAPI* g_origStartTestMode)();
__int64 (WINAPI* g_origTypeXGetTelop)();
__int64 (WINAPI* g_origTypeXMatching_ChangeWinLimit)();
__int64 (WINAPI* g_origTypeXMatching_CreateInstance)();
__int64 (WINAPI* g_origTypeXMatching_DeleteInstance)();
__int64 (WINAPI* g_origTypeXMatching_GetEnemyAddr)();
__int64 (WINAPI* g_origTypeXMatching_GetEnemyEntryId)();
__int64 (WINAPI* g_origTypeXMatching_GetEnemyOption)();
__int64 (WINAPI* g_origTypeXMatching_GetEnemyPlayerName)();
__int64 (WINAPI* g_origTypeXMatching_GetEnemyPlayerNameUTF16)();
__int64 (WINAPI* g_origTypeXMatching_GetEnemyTenpoName)();
__int64 (WINAPI* g_origTypeXMatching_GetEnemyTenpoNameUTF16)();
__int64 (WINAPI* g_origTypeXMatching_GetFighterInfoInStore)();
__int64 (WINAPI* g_origTypeXMatching_GetMyAddr)();
__int64 (WINAPI* g_origTypeXMatching_GetMyEntryId)();
__int64 (WINAPI* g_origTypeXMatching_GetRTT)();
__int64 (WINAPI* g_origTypeXMatching_GetStatus)();
__int64 (WINAPI* g_origTypeXMatching_GetWinLimit)();
__int64 (WINAPI* g_origTypeXMatching_GetWins)();
__int64 (WINAPI* g_origTypeXMatching_GetWinsAll)();
__int64 (WINAPI* g_origTypeXMatching_IsMatchingDesignatedFailed)();
__int64 (WINAPI* g_origTypeXMatching_Register_NESiCAUtilLibCancelRequestRFID)();
__int64 (WINAPI* g_origTypeXMatching_Register_NESiCAUtilLibGetRFIDID)();
__int64 (WINAPI* g_origTypeXMatching_Register_NESiCAUtilLibIsRFIDRead)();
__int64 (WINAPI* g_origTypeXMatching_Register_NESiCAUtilLibIsRFIDReadSuccess)();
__int64 (WINAPI* g_origTypeXMatching_Register_NESiCAUtilLibRequestRFID)();
__int64 (WINAPI* g_origTypeXMatching_ResetWins)();
__int64 (WINAPI* g_origTypeXMatching_Resume)();
__int64 (WINAPI* g_origTypeXMatching_StartMatchingDesignated)();
__int64 (WINAPI* g_origTypeXMatching_StartMathingInternetOnly)();
__int64 (WINAPI* g_origTypeXMatching_StartMathingStoreOnly)();
__int64 (WINAPI* g_origTypeXMatching_StartMathingStorePriority)();
__int64 (WINAPI* g_origTypeXMatching_StopMatching)();
__int64 (WINAPI* g_origTypeXMatching_Suspend)();
__int64 (WINAPI* g_origTypeXMatching_Update)();
__int64 (WINAPI* g_origTypeXMatching_UpdateWins)();
__int64 (WINAPI* g_origTypeXPlayRecordAddToSend)();
__int64 (WINAPI* g_origTypeXPlayRecordReset)();
__int64 (WINAPI* g_origTypeXPlayRecordResetRevenge)();
__int64 (WINAPI* g_origTypeXPlayRecordSetPlayerInfo)();
__int64 (WINAPI* g_origTypeXPlayRecordSetRoundResult)();
__int64 (WINAPI* g_origTypeXSafeFileIsFinished)();
__int64 (WINAPI* g_origTypeXSafeFileRead)();
__int64 (WINAPI* g_origTypeXSafeFileWrite)();
__int64 (WINAPI* g_origTypeXSafeFileWriteUpdate)();
__int64 (WINAPI* g_origUnlockProcess)();

__int64 UIO_AMIC_Close()
{
//	auto result = g_origUIO_AMIC_Close();
//	info(true, "UIO_AMIC_Close result: %08X", result);
	return 0;
}

__int64 UIO_AMIC_GetDeviceVersion()
{
//	auto result = g_origUIO_AMIC_GetDeviceVersion();
//	info(true, "UIO_AMIC_GetDeviceVersion result: %08X", result);
	return 0x66;
}

__int64 UIO_AMIC_GetStatus()
{
//	auto result = g_origUIO_AMIC_GetStatus();
//	info(true, "UIO_AMIC_GetStatus result: %08X", result);
	return 0x200;
}

__int64 UIO_AMIC_IsPadMode()
{
//	auto result = g_origUIO_AMIC_IsPadMode();
//	info(true, "UIO_AMIC_IsPadMode result: %08X", result);
	return 0;
}

__int64 UIO_AMIC_Open()
{
//	auto result = g_origUIO_AMIC_Open();
//	info(true, "UIO_AMIC_Open result: %08X", result);
	return 0;
}

__int64 UIO_AMIC_Update(__int64 a1)
{
//	auto result = g_origUIO_AMIC_Update(a1);
//	info(true, "UIO_AMIC_Update result: %08X", result);

	BYTE ButtonStateByte1 = 0;
	BYTE ButtonStateByte2 = 0;
	BYTE ButtonStateByte3 = 7;

	if (*ffbOffset & 0x80) // START
	{
		ButtonStateByte1 += 0x04;
	}
	if (*ffbOffset & 0x400000) // TEST
	{
		ButtonStateByte1 += 0x10;
	}
	if (*ffbOffset & 0x100) // COIN
	{
		ButtonStateByte1 += 0x20;
	}
	if (*ffbOffset & 0x20) // Button UP
	{
		ButtonStateByte2 += 0x01;
	}
	if (*ffbOffset & 0x10) // Button DOWN
	{
		ButtonStateByte2 += 0x02;
	}
	if (*ffbOffset & 0x08) // Button LEFT
	{
		ButtonStateByte2 += 0x04;
	}
	if (*ffbOffset & 0x04) // Button RIGHT
	{
		ButtonStateByte2 += 0x08;
	}
	if (*ffbOffset & 0x02) // Button#1
	{
		ButtonStateByte2 += 0x10;
	}
	if (*ffbOffset & 0x01) // Button#2
	{
		ButtonStateByte2 += 0x20;
	}
	if (*ffbOffset & 0x8000) // Button#3
	{
		ButtonStateByte2 += 0x40;
	}
	if (*ffbOffset & 0x4000) // Button#4
	{
		ButtonStateByte2 += 0x80;
	}
	if (*ffbOffset & 0x2000) // Button#5
	{
		ButtonStateByte1 += 0x01;
	}
	if (*ffbOffset & 0x1000) // Button#6
	{
		ButtonStateByte1 += 0x02;
	}
	if (*ffbOffset & 0x20000000) // Button#EX1
	{
		ButtonStateByte3 = 6;
	}
	if (*ffbOffset & 0x10000000) // Button#EX2
	{
		ButtonStateByte3 ^= 2;
	}
	if (*ffbOffset & 0x40)// GAMEPAD STATUS BYTE: 0=OFF , 1=ON
	{
		ButtonStateByte3 ^= 4;
	}

	injector::WriteMemory<BYTE>((a1 + 16), ButtonStateByte1, true);
	injector::WriteMemory<BYTE>((a1 + 18), ButtonStateByte2, true);
	injector::WriteMemory<BYTE>((a1 + 19), ButtonStateByte3, true);
	return 0;
}

__int64 UIO_AMIC_WdtClear()
{
//	auto result = g_origUIO_AMIC_WdtClear();
//	info(true, "UIO_AMIC_WdtClear result: %08X", result);
	return 1;
}

__int64 UIO_AMIC_WdtDisable()
{
//	auto result = g_origUIO_AMIC_WdtDisable();
//	info(true, "UIO_AMIC_WdtDisable result: %08X", result);
	return 1;
}

__int64 UIO_AMIC_WdtEnable(unsigned __int16 a1, __int64 a2)
{
//	auto result = g_origUIO_AMIC_WdtEnable(a1, a2);
//	info(true, "UIO_AMIC_WdtEnable result: %08X", result);
	return 1;
}

__int64 UIO_AMIC_WdtGetDIP()
{
//	auto result = g_origUIO_AMIC_WdtGetDIP();
//	info(true, "UIO_AMIC_WdtGetDIP result: %08X", result);
	return 0xABCD;
}

#ifndef _DEBUG
static void info(...)
{
  
}
#endif

__int64 NESiCAUtilLibIsOnlineMode()
{
	auto result = g_origNESiCAUtilLibIsOnlineMode();
	info("NESiCAUtilLibIsOnlineMode result: %08X", result);
	return result;
}

__int64 NESiCAUtilLibGetStatus()
{
	auto result = g_origNESiCAUtilLibGetStatus();
	info("NESiCAUtilLibGetStatus result: %08X", result);
	return 1; // can be anything thats not 0?
}

__int64 NESiCAUtilLibDelete()
{
	auto result = g_origNESiCAUtilLibDelete();
	info("NESiCAUtilLibDelete result: %08X", result);
	return result;
}

__int64 NESiCAUtilLibGetCenterInformation()
{
	auto result = g_origNESiCAUtilLibGetCenterInformation();
	info("NESiCAUtilLibGetCenterInformation result: %08X", result);
	return result;
}

__int64 NESiCAUtilLibGetInformation()
{
	auto result = g_origNESiCAUtilLibGetInformation();
	info("NESiCAUtilLibGetInformation result: %08X", result);
	return result;
}

__int64 NESiCAUtilLibGetPlayerAskInformation()
{
	auto result = g_origNESiCAUtilLibGetPlayerAskInformation();
	info("NESiCAUtilLibGetPlayerAskInformation result: %08X", result);
	return result;
}

__int64 NESiCAUtilLibGetPlayerInformation()
{
	auto result = g_origNESiCAUtilLibGetPlayerInformation();
	info("NESiCAUtilLibGetPlayerInformation result: %08X", result);
	return result;
}

__int64 NESiCAUtilLibGetPlayerTime()
{
	auto result = g_origNESiCAUtilLibGetPlayerTime();
	info("NESiCAUtilLibGetPlayerTime result: %08X", result);
	return result;
}

__int64 NESiCAUtilLibGetStatusStringUTF16()
{
	auto result = g_origNESiCAUtilLibGetStatusStringUTF16();
	info("NESiCAUtilLibGetStatusStringUTF16 result: %08X", result);
	return result;
}

__int64 NESiCAUtilLibInit()
{
	auto result = g_origNESiCAUtilLibInit();
	info("NESiCAUtilLibInit result: %08X", result);
	return result;
}

__int64 NESiCAUtilLibIsExistInstance()
{
	auto result = g_origNESiCAUtilLibIsExistInstance();
	info("NESiCAUtilLibIsExistInstance result: %08X", result);
	return result;
}

__int64 NESiCAUtilLibUpdate()
{
	auto result = g_origNESiCAUtilLibUpdate();
	info("NESiCAUtilLibUpdate result: %08X", result);
	return result;
}

__int64 NESiCAUtilLib_NESiCA_BData_Get()
{
	auto result = g_origNESiCAUtilLib_NESiCA_BData_Get();
	info("NESiCAUtilLib_NESiCA_BData_Get result: %08X", result);
	return result;
}

__int64 NESiCAUtilLib_NESiCA_BData_Set()
{
	auto result = g_origNESiCAUtilLib_NESiCA_BData_Set();
	info("NESiCAUtilLib_NESiCA_BData_Set result: %08X", result);
	return result;
}

__int64 NESiCAUtilLib_NESiCA_BData_SetDefault()
{
	auto result = g_origNESiCAUtilLib_NESiCA_BData_SetDefault();
	info("NESiCAUtilLib_NESiCA_BData_SetDefault result: %08X", result);
	return result;
}

__int64 NESiCAUtilLib_NESiCA_TData_GetBuf()
{
	auto result = g_origNESiCAUtilLib_NESiCA_TData_GetBuf();
	info("NESiCAUtilLib_NESiCA_TData_GetBuf result: %08X", result);
	return result;
}

__int64 NESiCAUtilLib_NESiCA_TData_GetSize()
{
	auto result = g_origNESiCAUtilLib_NESiCA_TData_GetSize();
	info("NESiCAUtilLib_NESiCA_TData_GetSize result: %08X", result);
	return result;
}

__int64 BeginPlay()
{
	auto result = g_origBeginPlay();
	info("BeginPlay result: %08X", result);
	return result;
}

__int64 ContinuePlay()
{
	auto result = g_origContinuePlay();
	info("ContinuePlay result: %08X", result);
	return result;
}

__int64 EndPlay()
{
	auto result = g_origEndPlay();
	info("EndPlay result: %08X", result);
	return result;
}

__int64 GetGlobalIP()
{
	auto result = g_origGetGlobalIP();
	info("GetGlobalIP result: %08X", result);
	return result;
}

__int64 GetKeyConfig()
{
	auto result = g_origGetKeyConfig();
	info("GetKeyConfig result: %08X", result);
	return result;
}

__int64 GetLanInterface()
{
	auto result = g_origGetLanInterface();
	info("GetLanInterface result: %08X", result);
	return result;
}

__int64 GetLibraryVersion()
{
	auto result = g_origGetLibraryVersion();
	info("GetLibraryVersion result: %08X", result);
	return result;
}

__int64 GetNWInfo()
{
	auto result = g_origGetNWInfo();
	info("GetNWInfo result: %08X", result);
	return result;
}

__int64 GetNewsImagePath()
{
	auto result = g_origGetNewsImagePath();
	info("GetNewsImagePath result: %08X", result);
	return result;
}

__int64 GetRanking(signed int a1, __int64 a2, signed int a3, DWORD_PTR a4, DWORD_PTR a5)
{
	auto result = g_origGetRanking(a1, a2, a3, a4, a5);
	info("GetRanking result: %08X", result);
	return result;
}

__int64 GetResponseTime()
{
	auto result = g_origGetResponseTime();
	info("GetResponseTime result: %08X", result);
	return result;
}

__int64 GetServerInfo()
{
	auto result = g_origGetServerInfo();
	info("GetServerInfo result: %08X", result);
	return result;
}

__int64 GetServiceVersion()
{
	auto result = g_origGetServiceVersion();
	info("GetServiceVersion result: %08X", result);
	return result;
}

__int64 GetTenpoInfo()
{
	auto result = g_origGetTenpoInfo();
	info("GetTenpoInfo result: %08X", result);
	return result;
}

__int64 IsBeginningProcessFinished()
{
	auto result = g_origIsBeginningProcessFinished();
	info("IsBeginningProcessFinished result: %08X", result);
	return result;
}

__int64 IsDisactive()
{
	auto result = g_origIsDisactive();
	info("IsDisactive result: %08X", result);
	return result;
}

__int64 IsEndingProcessFinished()
{
	auto result = g_origIsEndingProcessFinished();
	info("IsEndingProcessFinished result: %08X", result);
	return result;
}

__int64 IsEventMode()
{
	auto result = g_origIsEventMode();
	info("IsEventMode result: %08X", result);
	return result;
}

__int64 IsReadyDownloadRanking()
{
	auto result = g_origIsReadyDownloadRanking();
	info("IsReadyDownloadRanking result: %08X", result);
	return result;
}

__int64 IsTestMode()
{
	auto result = g_origIsTestMode();
	info("IsTestMode result: %08X", result);
	return result;
}

__int64 IsUploaded()
{
	auto result = g_origIsUploaded();
	info("IsUploaded result: %08X", result);
	return result;
}

__int64 LockProcess()
{
	auto result = g_origLockProcess();
	info("LockProcess result: %08X", result);
	return result;
}

__int64 NESiCA_Card_GetDetailCharacterBattlePointMonth()
{
	auto result = g_origNESiCA_Card_GetDetailCharacterBattlePointMonth();
	info("NESiCA_Card_GetDetailCharacterBattlePointMonth result: %08X", result);
	return result;
}

__int64 NESiCA_Card_GetDetailCharacterBattlePointTotal()
{
	auto result = g_origNESiCA_Card_GetDetailCharacterBattlePointTotal();
	info("NESiCA_Card_GetDetailCharacterBattlePointTotal result: %08X", result);
	return result;
}

__int64 NESiCA_Card_GetDetailCharacterDrawCountMonth()
{
	auto result = g_origNESiCA_Card_GetDetailCharacterDrawCountMonth();
	info("NESiCA_Card_GetDetailCharacterDrawCountMonth result: %08X", result);
	return result;
}

__int64 NESiCA_Card_GetDetailCharacterDrawCountTotal()
{
	auto result = g_origNESiCA_Card_GetDetailCharacterDrawCountTotal();
	info("NESiCA_Card_GetDetailCharacterDrawCountTotal result: %08X", result);
	return result;
}

__int64 NESiCA_Card_GetDetailCharacterLastColorID()
{
	auto result = g_origNESiCA_Card_GetDetailCharacterLastColorID();
	info("NESiCA_Card_GetDetailCharacterLastColorID result: %08X", result);
	return result;
}

__int64 NESiCA_Card_GetDetailCharacterLastComment()
{
	auto result = g_origNESiCA_Card_GetDetailCharacterLastComment();
	info("NESiCA_Card_GetDetailCharacterLastComment result: %08X", result);
	return result;
}

__int64 NESiCA_Card_GetDetailCharacterLastCostumeID()
{
	auto result = g_origNESiCA_Card_GetDetailCharacterLastCostumeID();
	info("NESiCA_Card_GetDetailCharacterLastCostumeID result: %08X", result);
	return result;
}

__int64 NESiCA_Card_GetDetailCharacterLastVTrigger()
{
	auto result = g_origNESiCA_Card_GetDetailCharacterLastVTrigger();
	info("NESiCA_Card_GetDetailCharacterLastVTrigger result: %08X", result);
	return result;
}

__int64 NESiCA_Card_GetDetailCharacterLoseCountMonth()
{
	auto result = g_origNESiCA_Card_GetDetailCharacterLoseCountMonth();
	info("NESiCA_Card_GetDetailCharacterLoseCountMonth result: %08X", result);
	return result;
}

__int64 NESiCA_Card_GetDetailCharacterLoseCountTotal()
{
	auto result = g_origNESiCA_Card_GetDetailCharacterLoseCountTotal();
	info("NESiCA_Card_GetDetailCharacterLoseCountTotal result: %08X", result);
	return result;
}

__int64 NESiCA_Card_GetDetailCharacterUseCountMonth()
{
	auto result = g_origNESiCA_Card_GetDetailCharacterUseCountMonth();
	info("NESiCA_Card_GetDetailCharacterUseCountMonth result: %08X", result);
	return result;
}

__int64 NESiCA_Card_GetDetailCharacterUseCountTotal()
{
	auto result = g_origNESiCA_Card_GetDetailCharacterUseCountTotal();
	info("NESiCA_Card_GetDetailCharacterUseCountTotal result: %08X", result);
	return result;
}

__int64 NESiCA_Card_GetDetailCharacterWinCountMonth()
{
	auto result = g_origNESiCA_Card_GetDetailCharacterWinCountMonth();
	info("NESiCA_Card_GetDetailCharacterWinCountMonth result: %08X", result);
	return result;
}

__int64 NESiCA_Card_GetDetailCharacterWinCountTotal()
{
	auto result = g_origNESiCA_Card_GetDetailCharacterWinCountTotal();
	info("NESiCA_Card_GetDetailCharacterWinCountTotal result: %08X", result);
	return result;
}

__int64 NESiCA_Card_GetDetailControlType()
{
	auto result = g_origNESiCA_Card_GetDetailControlType();
	info("NESiCA_Card_GetDetailControlType result: %08X", result);
	return result;
}

__int64 NESiCA_Card_GetDetailCurrentWins()
{
	auto result = g_origNESiCA_Card_GetDetailCurrentWins();
	info("NESiCA_Card_GetDetailCurrentWins result: %08X", result);
	return result;
}

__int64 NESiCA_Card_GetDetailHighScore()
{
	auto result = g_origNESiCA_Card_GetDetailHighScore();
	info("NESiCA_Card_GetDetailHighScore result: %08X", result);
	return result;
}

__int64 NESiCA_Card_GetDetailHighScoreCharacterID()
{
	auto result = g_origNESiCA_Card_GetDetailHighScoreCharacterID();
	info("NESiCA_Card_GetDetailHighScoreCharacterID result: %08X", result);
	return result;
}

__int64 NESiCA_Card_GetDetailKeyConfigHK()
{
	auto result = g_origNESiCA_Card_GetDetailKeyConfigHK();
	info("NESiCA_Card_GetDetailKeyConfigHK result: %08X", result);
	return result;
}

__int64 NESiCA_Card_GetDetailKeyConfigHP()
{
	auto result = g_origNESiCA_Card_GetDetailKeyConfigHP();
	info("NESiCA_Card_GetDetailKeyConfigHP result: %08X", result);
	return result;
}

__int64 NESiCA_Card_GetDetailKeyConfigHPHK()
{
	auto result = g_origNESiCA_Card_GetDetailKeyConfigHPHK();
	info("NESiCA_Card_GetDetailKeyConfigHPHK result: %08X", result);
	return result;
}

__int64 NESiCA_Card_GetDetailKeyConfigK3()
{
	auto result = g_origNESiCA_Card_GetDetailKeyConfigK3();
	info("NESiCA_Card_GetDetailKeyConfigK3 result: %08X", result);
	return result;
}

__int64 NESiCA_Card_GetDetailKeyConfigLK()
{
	auto result = g_origNESiCA_Card_GetDetailKeyConfigLK();
	info("NESiCA_Card_GetDetailKeyConfigLK result: %08X", result);
	return result;
}

__int64 NESiCA_Card_GetDetailKeyConfigLKMK()
{
	auto result = g_origNESiCA_Card_GetDetailKeyConfigLKMK();
	info("NESiCA_Card_GetDetailKeyConfigLKMK result: %08X", result);
	return result;
}

__int64 NESiCA_Card_GetDetailKeyConfigLP()
{
	auto result = g_origNESiCA_Card_GetDetailKeyConfigLP();
	info("NESiCA_Card_GetDetailKeyConfigLP result: %08X", result);
	return result;
}

__int64 NESiCA_Card_GetDetailKeyConfigLPLK()
{
	auto result = g_origNESiCA_Card_GetDetailKeyConfigLPLK();
	info("NESiCA_Card_GetDetailKeyConfigLPLK result: %08X", result);
	return result;
}

__int64 NESiCA_Card_GetDetailKeyConfigLPMK()
{
	auto result = g_origNESiCA_Card_GetDetailKeyConfigLPMK();
	info("NESiCA_Card_GetDetailKeyConfigLPMK result: %08X", result);
	return result;
}

__int64 NESiCA_Card_GetDetailKeyConfigLPMP()
{
	auto result = g_origNESiCA_Card_GetDetailKeyConfigLPMP();
	info("NESiCA_Card_GetDetailKeyConfigLPMP result: %08X", result);
	return result;
}

__int64 NESiCA_Card_GetDetailKeyConfigMK()
{
	auto result = g_origNESiCA_Card_GetDetailKeyConfigMK();
	info("NESiCA_Card_GetDetailKeyConfigMK result: %08X", result);
	return result;
}

__int64 NESiCA_Card_GetDetailKeyConfigMP()
{
	auto result = g_origNESiCA_Card_GetDetailKeyConfigMP();
	info("NESiCA_Card_GetDetailKeyConfigMP result: %08X", result);
	return result;
}

__int64 NESiCA_Card_GetDetailKeyConfigMPLK()
{
	auto result = g_origNESiCA_Card_GetDetailKeyConfigMPLK();
	info("NESiCA_Card_GetDetailKeyConfigMPLK result: %08X", result);
	return result;
}

__int64 NESiCA_Card_GetDetailKeyConfigMPMK()
{
	auto result = g_origNESiCA_Card_GetDetailKeyConfigMPMK();
	info("NESiCA_Card_GetDetailKeyConfigMPMK result: %08X", result);
	return result;
}

__int64 NESiCA_Card_GetDetailKeyConfigP3()
{
	auto result = g_origNESiCA_Card_GetDetailKeyConfigP3();
	info("NESiCA_Card_GetDetailKeyConfigP3 result: %08X", result);
	return result;
}

__int64 NESiCA_Card_GetDetailLastCharacterID()
{
	auto result = g_origNESiCA_Card_GetDetailLastCharacterID();
	info("NESiCA_Card_GetDetailLastCharacterID result: %08X", result);
	return result;
}

__int64 NESiCA_Card_GetDetailLeaguePoint()
{
	auto result = g_origNESiCA_Card_GetDetailLeaguePoint();
	info("NESiCA_Card_GetDetailLeaguePoint result: %08X", result);
	return result;
}

__int64 NESiCA_Card_GetDetailMaxWins()
{
	auto result = g_origNESiCA_Card_GetDetailMaxWins();
	info("NESiCA_Card_GetDetailMaxWins result: %08X", result);
	return result;
}

__int64 NESiCA_Card_GetDetailPlayerPointMonth()
{
	auto result = g_origNESiCA_Card_GetDetailPlayerPointMonth();
	info("NESiCA_Card_GetDetailPlayerPointMonth result: %08X", result);
	return result;
}

__int64 NESiCA_Card_GetDetailPlayerPointTotal()
{
	auto result = g_origNESiCA_Card_GetDetailPlayerPointTotal();
	info("NESiCA_Card_GetDetailPlayerPointTotal result: %08X", result);
	return result;
}

__int64 NESiCA_Card_GetPlayerComment()
{
	auto result = g_origNESiCA_Card_GetPlayerComment();
	info("NESiCA_Card_GetPlayerComment result: %08X", result);
	return result;
}

__int64 NESiCA_Card_GetPlayerCommentUTF16()
{
	auto result = g_origNESiCA_Card_GetPlayerCommentUTF16();
	info("NESiCA_Card_GetPlayerCommentUTF16 result: %08X", result);
	return result;
}

__int64 NESiCA_Card_GetPlayerMyStoreID()
{
	auto result = g_origNESiCA_Card_GetPlayerMyStoreID();
	info("NESiCA_Card_GetPlayerMyStoreID result: %08X", result);
	return result;
}

__int64 NESiCA_Card_GetPlayerName()
{
	auto result = g_origNESiCA_Card_GetPlayerName();
	info("NESiCA_Card_GetPlayerName result: %08X", result);
	return result;
}

__int64 NESiCA_Card_GetPlayerNameUTF16()
{
	auto result = g_origNESiCA_Card_GetPlayerNameUTF16();
	info("NESiCA_Card_GetPlayerNameUTF16 result: %08X", result);
	return result;
}

__int64 NESiCA_Card_GetPlayerTitleID()
{
	auto result = g_origNESiCA_Card_GetPlayerTitleID();
	info("NESiCA_Card_GetPlayerTitleID result: %08X", result);
	return result;
}

__int64 NESiCA_Card_SetDetailCharacterBattlePointMonth()
{
	auto result = g_origNESiCA_Card_SetDetailCharacterBattlePointMonth();
	info("NESiCA_Card_SetDetailCharacterBattlePointMonth result: %08X", result);
	return result;
}

__int64 NESiCA_Card_SetDetailCharacterBattlePointTotal()
{
	auto result = g_origNESiCA_Card_SetDetailCharacterBattlePointTotal();
	info("NESiCA_Card_SetDetailCharacterBattlePointTotal result: %08X", result);
	return result;
}

__int64 NESiCA_Card_SetDetailCharacterDrawCountMonth()
{
	auto result = g_origNESiCA_Card_SetDetailCharacterDrawCountMonth();
	info("NESiCA_Card_SetDetailCharacterDrawCountMonth result: %08X", result);
	return result;
}

__int64 NESiCA_Card_SetDetailCharacterDrawCountTotal()
{
	auto result = g_origNESiCA_Card_SetDetailCharacterDrawCountTotal();
	info("NESiCA_Card_SetDetailCharacterDrawCountTotal result: %08X", result);
	return result;
}

__int64 NESiCA_Card_SetDetailCharacterLastColorID()
{
	auto result = g_origNESiCA_Card_SetDetailCharacterLastColorID();
	info("NESiCA_Card_SetDetailCharacterLastColorID result: %08X", result);
	return result;
}

__int64 NESiCA_Card_SetDetailCharacterLastComment()
{
	auto result = g_origNESiCA_Card_SetDetailCharacterLastComment();
	info("NESiCA_Card_SetDetailCharacterLastComment result: %08X", result);
	return result;
}

__int64 NESiCA_Card_SetDetailCharacterLastCostumeID()
{
	auto result = g_origNESiCA_Card_SetDetailCharacterLastCostumeID();
	info("NESiCA_Card_SetDetailCharacterLastCostumeID result: %08X", result);
	return result;
}

__int64 NESiCA_Card_SetDetailCharacterLastVTrigger()
{
	auto result = g_origNESiCA_Card_SetDetailCharacterLastVTrigger();
	info("NESiCA_Card_SetDetailCharacterLastVTrigger result: %08X", result);
	return result;
}

__int64 NESiCA_Card_SetDetailCharacterLoseCountMonth()
{
	auto result = g_origNESiCA_Card_SetDetailCharacterLoseCountMonth();
	info("NESiCA_Card_SetDetailCharacterLoseCountMonth result: %08X", result);
	return result;
}

__int64 NESiCA_Card_SetDetailCharacterLoseCountTotal()
{
	auto result = g_origNESiCA_Card_SetDetailCharacterLoseCountTotal();
	info("NESiCA_Card_SetDetailCharacterLoseCountTotal result: %08X", result);
	return result;
}

__int64 NESiCA_Card_SetDetailCharacterUseCountMonth()
{
	auto result = g_origNESiCA_Card_SetDetailCharacterUseCountMonth();
	info("NESiCA_Card_SetDetailCharacterUseCountMonth result : % 08X", result);
	return result;
}

__int64 NESiCA_Card_SetDetailCharacterUseCountTotal()
{
	auto result = g_origNESiCA_Card_SetDetailCharacterUseCountTotal();
	info("NESiCA_Card_SetDetailCharacterUseCountTotal result: %08X", result);
	return result;
}

__int64 NESiCA_Card_SetDetailCharacterWinCountMonth()
{
	auto result = g_origNESiCA_Card_SetDetailCharacterWinCountMonth();
	info("NESiCA_Card_SetDetailCharacterWinCountMonth result: %08X", result);
	return result;
}

__int64 NESiCA_Card_SetDetailCharacterWinCountTotal()
{
	auto result = g_origNESiCA_Card_SetDetailCharacterWinCountTotal();
	info("NESiCA_Card_SetDetailCharacterWinCountTotal result: %08X", result);
	return result;
}

__int64 NESiCA_Card_SetDetailControlType()
{
	auto result = g_origNESiCA_Card_SetDetailControlType();
	info("NESiCA_Card_SetDetailControlType result: %08X", result);
	return result;
}

__int64 NESiCA_Card_SetDetailCurrentWins()
{
	auto result = g_origNESiCA_Card_SetDetailCurrentWins();
	info("NESiCA_Card_SetDetailCurrentWins result: %08X", result);
	return result;
}

__int64 NESiCA_Card_SetDetailHighScore()
{
	auto result = g_origNESiCA_Card_SetDetailHighScore();
	info("NESiCA_Card_SetDetailHighScore result: %08X", result);
	return result;
}

__int64 NESiCA_Card_SetDetailHighScoreCharacterID()
{
	auto result = g_origNESiCA_Card_SetDetailHighScoreCharacterID();
	info("NESiCA_Card_SetDetailHighScoreCharacterID result: %08X", result);
	return result;
}

__int64 NESiCA_Card_SetDetailKeyConfigHK()
{
	auto result = g_origNESiCA_Card_SetDetailKeyConfigHK();
	info("NESiCA_Card_SetDetailKeyConfigHK result: %08X", result);
	return result;
}

__int64 NESiCA_Card_SetDetailKeyConfigHP()
{
	auto result = g_origNESiCA_Card_SetDetailKeyConfigHP();
	info("NESiCA_Card_SetDetailKeyConfigHP result: %08X", result);
	return result;
}

__int64 NESiCA_Card_SetDetailKeyConfigHPHK()
{
	auto result = g_origNESiCA_Card_SetDetailKeyConfigHPHK();
	info("NESiCA_Card_SetDetailKeyConfigHPHK result: %08X", result);
	return result;
}

__int64 NESiCA_Card_SetDetailKeyConfigK3()
{
	auto result = g_origNESiCA_Card_SetDetailKeyConfigK3();
	info("NESiCA_Card_SetDetailKeyConfigK3 result: %08X", result);
	return result;
}

__int64 NESiCA_Card_SetDetailKeyConfigLK()
{
	auto result = g_origNESiCA_Card_SetDetailKeyConfigLK();
	info("NESiCA_Card_SetDetailKeyConfigLK result: %08X", result);
	return result;
}

__int64 NESiCA_Card_SetDetailKeyConfigLKMK()
{
	auto result = g_origNESiCA_Card_SetDetailKeyConfigLKMK();
	info("NESiCA_Card_SetDetailKeyConfigLKMK result: %08X", result);
	return result;
}

__int64 NESiCA_Card_SetDetailKeyConfigLP()
{
	auto result = g_origNESiCA_Card_SetDetailKeyConfigLP();
	info("NESiCA_Card_SetDetailKeyConfigLP result: %08X", result);
	return result;
}

__int64 NESiCA_Card_SetDetailKeyConfigLPLK()
{
	auto result = g_origNESiCA_Card_SetDetailKeyConfigLPLK();
	info("NESiCA_Card_SetDetailKeyConfigLPLK result: %08X", result);
	return result;
}

__int64 NESiCA_Card_SetDetailKeyConfigLPMK()
{
	auto result = g_origNESiCA_Card_SetDetailKeyConfigLPMK();
	info("NESiCA_Card_SetDetailKeyConfigLPMK result: %08X", result);
	return result;
}

__int64 NESiCA_Card_SetDetailKeyConfigLPMP()
{
	auto result = g_origNESiCA_Card_SetDetailKeyConfigLPMP();
	info("NESiCA_Card_SetDetailKeyConfigLPMP result: %08X", result);
	return result;
}

__int64 NESiCA_Card_SetDetailKeyConfigMK()
{
	auto result = g_origNESiCA_Card_SetDetailKeyConfigMK();
	info("NESiCA_Card_SetDetailKeyConfigMK result: %08X", result);
	return result;
}

__int64 NESiCA_Card_SetDetailKeyConfigMP()
{
	auto result = g_origNESiCA_Card_SetDetailKeyConfigMP();
	info("NESiCA_Card_SetDetailKeyConfigMP result: %08X", result);
	return result;
}

__int64 NESiCA_Card_SetDetailKeyConfigMPLK()
{
	auto result = g_origNESiCA_Card_SetDetailKeyConfigMPLK();
	info("NESiCA_Card_SetDetailKeyConfigMPLK result: %08X", result);
	return result;
}

__int64 NESiCA_Card_SetDetailKeyConfigMPMK()
{
	auto result = g_origNESiCA_Card_SetDetailKeyConfigMPMK();
	info("NESiCA_Card_SetDetailKeyConfigMPMK result: %08X", result);
	return result;
}

__int64 NESiCA_Card_SetDetailKeyConfigP3()
{
	auto result = g_origNESiCA_Card_SetDetailKeyConfigP3();
	info("NESiCA_Card_SetDetailKeyConfigP3 result: %08X", result);
	return result;
}

__int64 NESiCA_Card_SetDetailLastCharacterID()
{
	auto result = g_origNESiCA_Card_SetDetailLastCharacterID();
	info("NESiCA_Card_SetDetailLastCharacterID result: %08X", result);
	return result;
}

__int64 NESiCA_Card_SetDetailLeaguePoint()
{
	auto result = g_origNESiCA_Card_SetDetailLeaguePoint();
	info("NESiCA_Card_SetDetailLeaguePoint result: %08X", result);
	return result;
}

__int64 NESiCA_Card_SetDetailMaxWins()
{
	auto result = g_origNESiCA_Card_SetDetailMaxWins();
	info("NESiCA_Card_SetDetailMaxWins result: %08X", result);
	return result;
}

__int64 NESiCA_Card_SetDetailPlayerPointMonth()
{
	auto result = g_origNESiCA_Card_SetDetailPlayerPointMonth();
	info("NESiCA_Card_SetDetailPlayerPointMonth result: %08X", result);
	return result;
}

__int64 NESiCA_Card_SetDetailPlayerPointTotal()
{
	auto result = g_origNESiCA_Card_SetDetailPlayerPointTotal();
	info("NESiCA_Card_SetDetailPlayerPointTotal result: %08X", result);
	return result;
}

__int64 NESiCA_Card_SetPlayerComment()
{
	auto result = g_origNESiCA_Card_SetPlayerComment();
	info("NESiCA_Card_SetPlayerComment result: %08X", result);
	return result;
}

__int64 NESiCA_Card_SetPlayerCommentUTF16()
{
	auto result = g_origNESiCA_Card_SetPlayerCommentUTF16();
	info("NESiCA_Card_SetPlayerCommentUTF16 result: %08X", result);
	return result;
}

__int64 NESiCA_Card_SetPlayerMyStoreID()
{
	auto result = g_origNESiCA_Card_SetPlayerMyStoreID();
	info("NESiCA_Card_SetPlayerMyStoreID result: %08X", result);
	return result;
}

__int64 NESiCA_Card_SetPlayerName()
{
	auto result = g_origNESiCA_Card_SetPlayerName();
	info("NESiCA_Card_SetPlayerName result: %08X", result);
	return result;
}

__int64 NESiCA_Card_SetPlayerNameUTF16()
{
	auto result = g_origNESiCA_Card_SetPlayerNameUTF16();
	info("NESiCA_Card_SetPlayerNameUTF16 result: %08X", result);
	return result;
}

__int64 NESiCA_Card_SetPlayerTitleID()
{
	auto result = g_origNESiCA_Card_SetPlayerTitleID();
	info("NESiCA_Card_SetPlayerTitleID result: %08X", result);
	return result;
}

__int64 NESiCA_GetNESiCAID()
{
	auto result = g_origNESiCA_GetNESiCAID();
	info("NESiCA_GetNESiCAID result: %08X", result);
	return result;
}

__int64 NESiCA_GetNESiCAType()
{
	auto result = g_origNESiCA_GetNESiCAType();
	info("NESiCA_GetNESiCAType result: %08X", result);
	return result;
}

__int64 NESiCA_IsCardPlayer()
{
	auto result = g_origNESiCA_IsCardPlayer();
	info("NESiCA_IsCardPlayer result: %08X", result);
	return result;
}

__int64 NESiCA_Rank_GetBattlePointRank()
{
	auto result = g_origNESiCA_Rank_GetBattlePointRank();
	info("NESiCA_Rank_GetBattlePointRank result: %08X", result);
	return result;
}

__int64 NESiCA_Rank_GetBattlePointRankPref()
{
	auto result = g_origNESiCA_Rank_GetBattlePointRankPref();
	info("NESiCA_Rank_GetBattlePointRankPref result: %08X", result);
	return result;
}

__int64 NESiCA_Rank_GetBattlePointRankPrefId()
{
	auto result = g_origNESiCA_Rank_GetBattlePointRankPrefId();
	info("NESiCA_Rank_GetBattlePointRankPrefId result: %08X", result);
	return result;
}

__int64 NESiCA_Rank_GetBattlePointRankPrefName()
{
	auto result = g_origNESiCA_Rank_GetBattlePointRankPrefName();
	info("NESiCA_Rank_GetBattlePointRankPrefName result: %08X", result);
	return result;
}

__int64 NESiCA_Rank_GetBattlePointRankPrefNameUTF16()
{
	auto result = g_origNESiCA_Rank_GetBattlePointRankPrefNameUTF16();
	info("NESiCA_Rank_GetBattlePointRankPrefNameUTF16 result: %08X", result);
	return result;
}

__int64 Reconnect()
{
	auto result = g_origReconnect();
	info("Reconnect result: %08X", result);
	return result;
}

__int64 SaveOnTheWay()
{
	auto result = g_origSaveOnTheWay();
	info("SaveOnTheWay result: %08X", result);
	return result;
}

__int64 SetCharacterID()
{
	auto result = g_origSetCharacterID();
	info("SetCharacterID result: %08X", result);
	return result;
}

__int64 SetEventMode()
{
	auto result = g_origSetEventMode();
	info("SetEventMode result: %08X", result);
	return result;
}

__int64 SetEventModeResult()
{
	auto result = g_origSetEventModeResult();
	info("SetEventModeResult result: %08X", result);
	return result;
}

__int64 SetOpenCloseTime()
{
	auto result = g_origSetOpenCloseTime();
	info("SetOpenCloseTime result: %08X", result);
	return result;
}

__int64 StartTestMode()
{
	auto result = g_origStartTestMode();
	info("StartTestMode result: %08X", result);
	return result;
}

__int64 TypeXGetTelop()
{
	auto result = g_origTypeXGetTelop();
	info("TypeXGetTelop result: %08X", result);
	return result;
}

__int64 TypeXMatching_ChangeWinLimit()
{
	auto result = g_origTypeXMatching_ChangeWinLimit();
	info("TypeXMatching_ChangeWinLimit result: %08X", result);
	return result;
}

__int64 TypeXMatching_CreateInstance()
{
	auto result = g_origTypeXMatching_CreateInstance();
	info("TypeXMatching_CreateInstance result: %08X", result);
	return result;
}

__int64 TypeXMatching_DeleteInstance()
{
	auto result = g_origTypeXMatching_DeleteInstance();
	info("TypeXMatching_DeleteInstance result: %08X", result);
	return result;
}

__int64 TypeXMatching_GetEnemyAddr()
{
	auto result = g_origTypeXMatching_GetEnemyAddr();
	info("TypeXMatching_GetEnemyAddr result: %08X", result);
	return result;
}

__int64 TypeXMatching_GetEnemyEntryId()
{
	auto result = g_origTypeXMatching_GetEnemyEntryId();
	info("TypeXMatching_GetEnemyEntryId result: %08X", result);
	return result;
}

__int64 TypeXMatching_GetEnemyOption()
{
	auto result = g_origTypeXMatching_GetEnemyOption();
	info("TypeXMatching_GetEnemyOption result: %08X", result);
	return result;
}

__int64 TypeXMatching_GetEnemyPlayerName()
{
	auto result = g_origTypeXMatching_GetEnemyPlayerName();
	info("TypeXMatching_GetEnemyPlayerName result: %08X", result);
	return result;
}

__int64 TypeXMatching_GetEnemyPlayerNameUTF16()
{
	auto result = g_origTypeXMatching_GetEnemyPlayerNameUTF16();
	info("TypeXMatching_GetEnemyPlayerNameUTF16 result: %08X", result);
	return result;
}

__int64 TypeXMatching_GetEnemyTenpoName()
{
	auto result = g_origTypeXMatching_GetEnemyTenpoName();
	info("TypeXMatching_GetEnemyTenpoName result: %08X", result);
	return result;
}

__int64 TypeXMatching_GetEnemyTenpoNameUTF16()
{
	auto result = g_origTypeXMatching_GetEnemyTenpoNameUTF16();
	info("TypeXMatching_GetEnemyTenpoNameUTF16 result: %08X", result);
	return result;
}

__int64 TypeXMatching_GetFighterInfoInStore()
{
	auto result = g_origTypeXMatching_GetFighterInfoInStore();
	info("TypeXMatching_GetFighterInfoInStore result: %08X", result);
	return result;
}

__int64 TypeXMatching_GetMyAddr()
{
	auto result = g_origTypeXMatching_GetMyAddr();
	info("TypeXMatching_GetMyAddr result: %08X", result);
	return result;
}

__int64 TypeXMatching_GetMyEntryId()
{
	auto result = g_origTypeXMatching_GetMyEntryId();
	info("TypeXMatching_GetMyEntryId result: %08X", result);
	return result;
}

__int64 TypeXMatching_GetRTT()
{
	auto result = g_origTypeXMatching_GetRTT();
	info("TypeXMatching_GetRTT result: %08X", result);
	return result;
}

__int64 TypeXMatching_GetStatus()
{
	auto result = g_origTypeXMatching_GetStatus();
	info("TypeXMatching_GetStatus result: %08X", result);
	return result;
}

__int64 TypeXMatching_GetWinLimit()
{
	auto result = g_origTypeXMatching_GetWinLimit();
	info("TypeXMatching_GetWinLimit result: %08X", result);
	return result;
}

__int64 TypeXMatching_GetWins()
{
	auto result = g_origTypeXMatching_GetWins();
	info("TypeXMatching_GetWins result: %08X", result);
	return result;
}

__int64 TypeXMatching_GetWinsAll()
{
	auto result = g_origTypeXMatching_GetWinsAll();
	info("TypeXMatching_GetWinsAll result: %08X", result);
	return result;
}

__int64 TypeXMatching_IsMatchingDesignatedFailed()
{
	auto result = g_origTypeXMatching_IsMatchingDesignatedFailed();
	info("TypeXMatching_IsMatchingDesignatedFailed result: %08X", result);
	return result;
}

__int64 TypeXMatching_Register_NESiCAUtilLibCancelRequestRFID()
{
	auto result = g_origTypeXMatching_Register_NESiCAUtilLibCancelRequestRFID();
	info("TypeXMatching_Register_NESiCAUtilLibCancelRequestRFID result: %08X", result);
	return result;
}

__int64 TypeXMatching_Register_NESiCAUtilLibGetRFIDID()
{
	auto result = g_origTypeXMatching_Register_NESiCAUtilLibGetRFIDID();
	info("TypeXMatching_Register_NESiCAUtilLibGetRFIDID result: %08X", result);
	return result;
}

__int64 TypeXMatching_Register_NESiCAUtilLibIsRFIDRead()
{
	auto result = g_origTypeXMatching_Register_NESiCAUtilLibIsRFIDRead();
	info("TypeXMatching_Register_NESiCAUtilLibIsRFIDRead result: %08X", result);
	return result;
}

__int64 TypeXMatching_Register_NESiCAUtilLibIsRFIDReadSuccess()
{
	auto result = g_origTypeXMatching_Register_NESiCAUtilLibIsRFIDReadSuccess();
	info("TypeXMatching_Register_NESiCAUtilLibIsRFIDReadSuccess result: %08X", result);
	return result;
}

__int64 TypeXMatching_Register_NESiCAUtilLibRequestRFID()
{
	auto result = g_origTypeXMatching_Register_NESiCAUtilLibRequestRFID();
	info("TypeXMatching_Register_NESiCAUtilLibRequestRFID result: %08X", result);
	return result;
}

__int64 TypeXMatching_ResetWins()
{
	auto result = g_origTypeXMatching_ResetWins();
	info("TypeXMatching_ResetWins result: %08X", result);
	return result;
}

__int64 TypeXMatching_Resume()
{
	auto result = g_origTypeXMatching_Resume();
	info("TypeXMatching_Resume result: %08X", result);
	return result;
}

__int64 TypeXMatching_StartMatchingDesignated()
{
	auto result = g_origTypeXMatching_StartMatchingDesignated();
	info("TypeXMatching_StartMatchingDesignated result: %08X", result);
	return result;
}

__int64 TypeXMatching_StartMathingInternetOnly()
{
	auto result = g_origTypeXMatching_StartMathingInternetOnly();
	info("TypeXMatching_StartMathingInternetOnly result: %08X", result);
	return result;
}

__int64 TypeXMatching_StartMathingStoreOnly()
{
	auto result = g_origTypeXMatching_StartMathingStoreOnly();
	info("TypeXMatching_StartMathingStoreOnly result: %08X", result);
	return result;
}

__int64 TypeXMatching_StartMathingStorePriority()
{
	auto result = g_origTypeXMatching_StartMathingStorePriority();
	info("TypeXMatching_StartMathingStorePriority result: %08X", result);
	return result;
}

__int64 TypeXMatching_StopMatching()
{
	auto result = g_origTypeXMatching_StopMatching();
	info("TypeXMatching_StopMatching result: %08X", result);
	return result;
}

__int64 TypeXMatching_Suspend()
{
	auto result = g_origTypeXMatching_Suspend();
	info("TypeXMatching_Suspend result: %08X", result);
	return result;
}

__int64 TypeXMatching_Update()
{
	auto result = g_origTypeXMatching_Update();
	info("TypeXMatching_Update result: %08X", result);
	return result;
}

__int64 TypeXMatching_UpdateWins()
{
	auto result = g_origTypeXMatching_UpdateWins();
	info("TypeXMatching_UpdateWins result: %08X", result);
	return result;
}

__int64 TypeXPlayRecordAddToSend()
{
	auto result = g_origTypeXPlayRecordAddToSend();
	info("TypeXPlayRecordAddToSend result: %08X", result);
	return result;
}

__int64 TypeXPlayRecordReset()
{
	auto result = g_origTypeXPlayRecordReset();
	info("TypeXPlayRecordReset result: %08X", result);
	return result;
}

__int64 TypeXPlayRecordResetRevenge()
{
	auto result = g_origTypeXPlayRecordResetRevenge();
	info("TypeXPlayRecordResetRevenge result: %08X", result);
	return result;
}

__int64 TypeXPlayRecordSetPlayerInfo()
{
	auto result = g_origTypeXPlayRecordSetPlayerInfo();
	info("TypeXPlayRecordSetPlayerInfo result: %08X", result);
	return result;
}

__int64 TypeXPlayRecordSetRoundResult()
{
	auto result = g_origTypeXPlayRecordSetRoundResult();
	info("TypeXPlayRecordSetRoundResult result: %08X", result);
	return result;
}

__int64 TypeXSafeFileIsFinished()
{
	auto result = g_origTypeXSafeFileIsFinished();
	info("TypeXSafeFileIsFinished result: %08X", result);
	return result;
}

__int64 TypeXSafeFileRead()
{
	auto result = g_origTypeXSafeFileRead();
	info("TypeXSafeFileRead result: %08X", result);
	return result;
}

__int64 TypeXSafeFileWrite()
{
	auto result = g_origTypeXSafeFileWrite();
	info("TypeXSafeFileWrite result: %08X", result);
	return result;
}

__int64 TypeXSafeFileWriteUpdate()
{
	auto result = g_origTypeXSafeFileWriteUpdate();
	info("TypeXSafeFileWriteUpdate result: %08X", result);
	return result;
}

__int64 UnlockProcess()
{
	auto result = g_origUnlockProcess();
	info("UnlockProcess result: %08X", result);
	return result;
}

static wchar_t FullPathWithArgsFinal[MAX_PATH + 74] = { 0 };
LPWSTR(__stdcall* original_GetCommandLineWSFV)();
LPWSTR WINAPI GetCommandLineWSFV()
{
	return FullPathWithArgsFinal;
}

void ReplaceStringInPlaceWSFV(std::wstring& subject, const std::wstring& search, const std::wstring& replace)
{
	size_t pos = 0;
	while ((pos = subject.find(search, pos)) != std::wstring::npos)
	{
		subject.replace(pos, search.length(), replace);
		pos += replace.length();
	}
}

HRESULT(__cdecl* original_SHGetFolderPathWSFV)(HWND hwnd, int csidl, HANDLE hToken, DWORD dwFlags, LPWSTR pszPath);
HRESULT __cdecl SHGetFolderPathWSFV(HWND hwnd, int csidl, HANDLE hToken, DWORD dwFlags, LPWSTR pszPath)
{
	HRESULT result = original_SHGetFolderPathWSFV(hwnd, csidl, hToken, dwFlags, pszPath);
	if (csidl == 0x1C)
	{
		std::wstring tempPath = pszPath;
		std::wstring exePath = FullPathWithExeName;
		ReplaceStringInPlaceWSFV(exePath, L"\\StreetFighterV.exe", L"\\AppData\\");
		ReplaceStringInPlaceWSFV(exePath, L"\\", L"/");
		wcscpy(pszPath, exePath.c_str());
	}

	return result;
}

HANDLE(__stdcall* original_CreateFileWSFV)(LPCWSTR lpFileName, DWORD dwDesiredAccess, DWORD dwShareMode, LPSECURITY_ATTRIBUTES lpSecurityAttributes, DWORD dwCreationDisposition, DWORD dwFlagsAndAttributes, HANDLE hTemplateFile);
HANDLE WINAPI CreateFileWSFV(LPCWSTR lpFileName, DWORD dwDesiredAccess, DWORD dwShareMode, LPSECURITY_ATTRIBUTES lpSecurityAttributes, DWORD dwCreationDisposition, DWORD dwFlagsAndAttributes, HANDLE hTemplateFile)
{
	if (wcsncmp(lpFileName, L"D:\\db", 5) == 0)
	{
		std::wstring tempFileName = lpFileName;
		std::wstring exePath = FullPathWithExeName;
		ReplaceStringInPlaceWSFV(exePath, L"\\StreetFighterV.exe", L"\\OpenParrot\\");
		ReplaceStringInPlaceWSFV(exePath, L"\\", L"/");
		ReplaceStringInPlaceWSFV(tempFileName, L"D:\\", exePath);
		LPCWSTR finalFileName = tempFileName.c_str();
//		MessageBoxW(0, finalFileName, L"nesica db file", 0);
		return original_CreateFileWSFV(finalFileName, dwDesiredAccess, dwShareMode, lpSecurityAttributes, dwCreationDisposition, dwFlagsAndAttributes, hTemplateFile);
	}

	if (wcsncmp(lpFileName, L"Z:", 2) == 0)
	{
		std::wstring tempFileName = lpFileName;
		std::wstring exePath = FullPathWithExeName;
		ReplaceStringInPlaceWSFV(exePath, L"\\StreetFighterV.exe", L"\\");
	//	ReplaceStringInPlaceWSFV(exePath, VMProtectDecryptStringW(L"\\"), VMProtectDecryptStringW(L"/"));
		ReplaceStringInPlaceWSFV(tempFileName, L"Z:\\", exePath);
		LPCWSTR finalFileName = tempFileName.c_str();
		return original_CreateFileWSFV(finalFileName, dwDesiredAccess, dwShareMode, lpSecurityAttributes, dwCreationDisposition, dwFlagsAndAttributes, hTemplateFile);
	}

	return original_CreateFileWSFV(lpFileName, dwDesiredAccess, dwShareMode, lpSecurityAttributes, dwCreationDisposition, dwFlagsAndAttributes, hTemplateFile);
}

BOOL(__stdcall* original_DeleteFileWSFV)(LPCWSTR lpFileName);
BOOL WINAPI DeleteFileWSFV(LPCWSTR lpFileName)
{
	if (wcsncmp(lpFileName, L"D:\\db", 5) == 0)
	{
		std::wstring tempFileName = lpFileName;
		std::wstring exePath = FullPathWithExeName;
		ReplaceStringInPlaceWSFV(exePath, L"\\StreetFighterV.exe", L"\\OpenParrot\\");
		ReplaceStringInPlaceWSFV(exePath, L"\\", L"/");
		ReplaceStringInPlaceWSFV(tempFileName, L"D:\\", exePath);
		LPCWSTR finalFileName = tempFileName.c_str();
		return original_DeleteFileWSFV(finalFileName);
	}

	if (wcsncmp(lpFileName, L"Z:", 2) == 0)
	{
		std::wstring tempFileName = lpFileName;
		std::wstring exePath = FullPathWithExeName;
		ReplaceStringInPlaceWSFV(exePath, L"\\StreetFighterV.exe", L"\\");
	//	ReplaceStringInPlaceWSFV(exePath, VMProtectDecryptStringW(L"\\"), VMProtectDecryptStringW(L"/"));
		ReplaceStringInPlaceWSFV(tempFileName, L"Z:\\", exePath);
		LPCWSTR finalFileName = tempFileName.c_str();
		return original_DeleteFileWSFV(finalFileName);
	}

	return original_DeleteFileWSFV(lpFileName);
}

HANDLE(__stdcall* original_FindFirstFileWSFV)(LPCWSTR lpFileName, LPWIN32_FIND_DATAW lpFindFileData);
HANDLE WINAPI FindFirstFileWSFV(LPCWSTR lpFileName, LPWIN32_FIND_DATAW lpFindFileData)
{
	if (wcsncmp(lpFileName, L"D:\\db", 5) == 0)
	{
		std::wstring tempFileName = lpFileName;
		std::wstring exePath = FullPathWithExeName;
		ReplaceStringInPlaceWSFV(exePath, L"\\StreetFighterV.exe", L"\\OpenParrot\\");
		ReplaceStringInPlaceWSFV(tempFileName, L"D:", exePath);
		LPCWSTR finalFileName = tempFileName.c_str();
		return original_FindFirstFileWSFV(finalFileName, lpFindFileData);
	}

	return original_FindFirstFileWSFV(lpFileName, lpFindFileData);
}

static BOOL(__stdcall* g_origCreateDirectoryW)(LPCWSTR lpPathName, LPSECURITY_ATTRIBUTES lpSecurityAttributes);
static BOOL __stdcall CreateDirectoryWWrap(
	LPCWSTR                lpPathName,
	LPSECURITY_ATTRIBUTES lpSecurityAttributes
)
{
	if (wcsncmp(lpPathName, L"D:\\db", 5) == 0)
	{
		std::wstring tempFileName = lpPathName;
		std::wstring exePath = FullPathWithExeName;
		ReplaceStringInPlaceWSFV(exePath, L"\\StreetFighterV.exe", L"\\OpenParrot\\");
		ReplaceStringInPlaceWSFV(exePath, L"\\", L"/");
		ReplaceStringInPlaceWSFV(tempFileName, L"D:\\", exePath);
		LPCWSTR finalFileName = tempFileName.c_str();
		return g_origCreateDirectoryW(finalFileName, lpSecurityAttributes);
	}
	return g_origCreateDirectoryW(lpPathName, lpSecurityAttributes);
}

static DWORD(__stdcall* g_origGetFileAttributesExW)(LPCWSTR lpFileName, GET_FILEEX_INFO_LEVELS fInfoLevelId, LPVOID lpFileInformation);
static DWORD __stdcall GetFileAttributesExWWrap(LPCWSTR lpFileName, GET_FILEEX_INFO_LEVELS fInfoLevelId, LPVOID lpFileInformation)
{
	if (wcsncmp(lpFileName, L"D:\\db", 5) == 0)
	{
		std::wstring tempFileName = lpFileName;
		std::wstring exePath = FullPathWithExeName;
		ReplaceStringInPlaceWSFV(exePath, L"\\StreetFighterV.exe", L"\\OpenParrot\\");
		ReplaceStringInPlaceWSFV(tempFileName, L"D:", exePath);
		LPCWSTR finalFileName = tempFileName.c_str();
		return g_origGetFileAttributesExW(finalFileName, fInfoLevelId, lpFileInformation);
	}
	return g_origGetFileAttributesExW(lpFileName, fInfoLevelId, lpFileInformation);
}

UINT (WINAPI *original_GetRawInputDeviceListSFV)(PRAWINPUTDEVICELIST pRawInputDeviceList, UINT* puiNumDevices, UINT cbSize);
UINT WINAPI GetRawInputDeviceListSFV(PRAWINPUTDEVICELIST pRawInputDeviceList, UINT* puiNumDevices, UINT cbSize) {
	*puiNumDevices = 0;
	return 0;
}

DWORD WINAPI XInputGetStateSFV(__int64 a1, __int64 a2)
{
	return ERROR_DEVICE_NOT_CONNECTED;
}

bool replace(std::string& str, const std::string& from, const std::string& to) {
	size_t start_pos = str.find(from);
	if (start_pos == std::string::npos)
		return false;
	str.replace(start_pos, from.length(), to);
	return true;
}

void FixJapaneseFont()
{
	char working_directory[MAX_PATH + 1];
	GetCurrentDirectoryA(sizeof(working_directory), working_directory);
	std::string strDIR = working_directory;
	std::string strCFG = strDIR + "\\AppData\\StreetFighterV\\Saved\\Config\\WindowsNoEditor\\Engine.ini";
	std::string strToFind = "[Internationalization]\nCulture=ja\n";
	std::string strInsertBefore = "[WindowsApplication.Accessibility]\n";
	std::string strToInsert = "[Internationalization]\nCulture=ja\n\n[WindowsApplication.Accessibility]\n";
	//OPEN FILE AS STRING
	std::ifstream inFile;
	inFile.open(strCFG.c_str(), std::ifstream::in);
	std::stringstream strStream;
	strStream << inFile.rdbuf();
	std::string CFGread = strStream.str(); 
	inFile.close();
	// EDIT STRING
	std::size_t found;
	found = CFGread.find(strToFind);
	if (found != std::string::npos)
	{
		if (!ToBool(config["General"]["FixJapaneseFont"]))
			replace(CFGread, strToFind, "");
	}
	else
	{
		if (ToBool(config["General"]["FixJapaneseFont"]))
			replace(CFGread, strInsertBefore, strToInsert);
	}
	//SAVE STRING AS FILE
	std::ofstream outFile;
	outFile.open(strCFG.c_str(), std::ofstream::out | std::ofstream::trunc); 
	outFile << CFGread;
	outFile.close();
	return;
}

void SetCommandLineSFV()
{
	std::wstring FullPath = FullPathWithExeName;
	std::wstring ResX = L"1280";
	std::wstring ResY = L"720";
	std::wstring Args;

	if (ToBool(config["General"]["CustomResolution"]))
	{
		std::string resWidth = config["General"]["CustomWidth"];
		std::string resHeight = config["General"]["CustomHeight"];
		std::wstring resWidthW(resWidth.begin(), resWidth.end());
		std::wstring resHeightW(resHeight.begin(), resHeight.end());
		ResX = resWidthW;
		ResY = resHeightW;
	}

	if (ToBool(config["General"]["Windowed"]))
	{
		Args = L"\" -ResX=" + ResX + L" -ResY=" + ResY + L" -WINDOWED\0";
	}
	else
	{
		Args = L"\" -ResX=" + ResX + L" -ResY=" + ResY + L" -FULLSCREEN\0";
	}
	std::wstring FullPathWithArgs = L"\"" + FullPath + Args;
	LPWSTR FullPathWithArgsTemp = (LPWSTR)(FullPathWithArgs.c_str());
	wcscpy(FullPathWithArgsFinal, FullPathWithArgsTemp);
	return;
}

LRESULT(WINAPI* g_origWndProc)
(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
static LRESULT Hook_WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	if (msg == WM_KEYDOWN || msg == WM_KEYUP)
	{
		// lets swallow key presses.
		return 0;
	}

	return g_origWndProc(hWnd, msg, wParam, lParam);
}

// Check the drive tp/the game is on, instead of D:/
static BOOL(__stdcall* g_origGetDiskFreeSpaceExW)(LPCWSTR lpDirectoryName, PULARGE_INTEGER lpFreeBytesAvailableToCaller, PULARGE_INTEGER lpTotalNumberOfBytes, PULARGE_INTEGER lpTotalNumberOfFreeBytes);
static BOOL __stdcall GetDiskFreeSpaceExWWrap(LPCWSTR lpDirectoryName, PULARGE_INTEGER lpFreeBytesAvailableToCaller, PULARGE_INTEGER lpTotalNumberOfBytes, PULARGE_INTEGER lpTotalNumberOfFreeBytes)
{
	return g_origGetDiskFreeSpaceExW(NULL, lpFreeBytesAvailableToCaller, lpTotalNumberOfBytes, lpTotalNumberOfFreeBytes);
}

static InitFunction SFVFunc([]()
	{
		imageBase = (DWORD_PTR)GetModuleHandleA(0);
		CreateDirectoryA("OpenParrot", nullptr);
		DWORD_PTR iobase = (DWORD_PTR)LoadLibraryA("..\\..\\Plugins\\USBIOPlugin\\UIO_AMIC.dll");
		MH_Initialize();
		MH_CreateHookApi(L"UIO_AMIC.dll", "UIO_AMIC_Close", UIO_AMIC_Close, (void**)&g_origUIO_AMIC_Close);
		MH_CreateHookApi(L"UIO_AMIC.dll", "UIO_AMIC_GetDeviceVersion", UIO_AMIC_GetDeviceVersion, (void**)&g_origUIO_AMIC_GetDeviceVersion);
		MH_CreateHookApi(L"UIO_AMIC.dll", "UIO_AMIC_GetStatus", UIO_AMIC_GetStatus, (void**)&g_origUIO_AMIC_GetStatus);
		MH_CreateHookApi(L"UIO_AMIC.dll", "UIO_AMIC_IsPadMode", UIO_AMIC_IsPadMode, (void**)&g_origUIO_AMIC_IsPadMode);
		MH_CreateHookApi(L"UIO_AMIC.dll", "UIO_AMIC_Open", UIO_AMIC_Open, (void**)&g_origUIO_AMIC_Open);
		MH_CreateHookApi(L"UIO_AMIC.dll", "UIO_AMIC_Update", UIO_AMIC_Update, (void**)&g_origUIO_AMIC_Update);
		MH_CreateHookApi(L"UIO_AMIC.dll", "UIO_AMIC_WdtClear", UIO_AMIC_WdtClear, (void**)&g_origUIO_AMIC_WdtClear);
		MH_CreateHookApi(L"UIO_AMIC.dll", "UIO_AMIC_WdtDisable", UIO_AMIC_WdtDisable, (void**)&g_origUIO_AMIC_WdtDisable);
		MH_CreateHookApi(L"UIO_AMIC.dll", "UIO_AMIC_WdtEnable", UIO_AMIC_WdtEnable, (void**)&g_origUIO_AMIC_WdtEnable);
		MH_CreateHookApi(L"UIO_AMIC.dll", "UIO_AMIC_WdtGetDIP", UIO_AMIC_WdtGetDIP, (void**)&g_origUIO_AMIC_WdtGetDIP);
		MH_EnableHook(MH_ALL_HOOKS);

		DWORD_PTR nesysbase = (DWORD_PTR)LoadLibraryA("..\\..\\Plugins\\NESYSUtil\\\NESiCAUtilLib.dll");
		MH_Initialize();
	//	MH_CreateHookApi(L"NESiCAUtilLib.dll", "NESiCAUtilLibIsOnlineMode", NESiCAUtilLibIsOnlineMode, (void**)&g_origNESiCAUtilLibIsOnlineMode);
	//	MH_CreateHookApi(L"NESiCAUtilLib.dll", "NESiCAUtilLibGetStatus", NESiCAUtilLibGetStatus, (void**)&g_origNESiCAUtilLibGetStatus);
	//	MH_CreateHookApi(L"NESiCAUtilLib.dll", "NESiCAUtilLibDelete", NESiCAUtilLibDelete, (void**)&g_origNESiCAUtilLibDelete);
	//	MH_CreateHookApi(L"NESiCAUtilLib.dll", "NESiCAUtilLibGetCenterInformation", NESiCAUtilLibGetCenterInformation, (void**)&g_origNESiCAUtilLibGetCenterInformation);
	//	MH_CreateHookApi(L"NESiCAUtilLib.dll", "NESiCAUtilLibGetInformation", NESiCAUtilLibGetInformation, (void**)&g_origNESiCAUtilLibGetInformation);
	//	MH_CreateHookApi(L"NESiCAUtilLib.dll", "NESiCAUtilLibGetPlayerAskInformation", NESiCAUtilLibGetPlayerAskInformation, (void**)&g_origNESiCAUtilLibGetPlayerAskInformation);
	//	MH_CreateHookApi(L"NESiCAUtilLib.dll", "NESiCAUtilLibGetPlayerInformation", NESiCAUtilLibGetPlayerInformation, (void**)&g_origNESiCAUtilLibGetPlayerInformation);
	//	MH_CreateHookApi(L"NESiCAUtilLib.dll", "NESiCAUtilLibGetPlayerTime", NESiCAUtilLibGetPlayerTime, (void**)&g_origNESiCAUtilLibGetPlayerTime);
	//	MH_CreateHookApi(L"NESiCAUtilLib.dll", "NESiCAUtilLibGetStatusStringUTF16", NESiCAUtilLibGetStatusStringUTF16, (void**)&g_origNESiCAUtilLibGetStatusStringUTF16);
	//	MH_CreateHookApi(L"NESiCAUtilLib.dll", "NESiCAUtilLibInit", NESiCAUtilLibInit, (void**)&g_origNESiCAUtilLibInit);
	//	MH_CreateHookApi(L"NESiCAUtilLib.dll", "NESiCAUtilLibIsExistInstance", NESiCAUtilLibIsExistInstance, (void**)&g_origNESiCAUtilLibIsExistInstance);
	//	MH_CreateHookApi(L"NESiCAUtilLib.dll", "NESiCAUtilLibUpdate", NESiCAUtilLibUpdate, (void**)&g_origNESiCAUtilLibUpdate);
	//	MH_CreateHookApi(L"NESiCAUtilLib.dll", "NESiCAUtilLib_NESiCA_BData_Get", NESiCAUtilLib_NESiCA_BData_Get, (void**)&g_origNESiCAUtilLib_NESiCA_BData_Get);
	//	MH_CreateHookApi(L"NESiCAUtilLib.dll", "NESiCAUtilLib_NESiCA_BData_Set", NESiCAUtilLib_NESiCA_BData_Set, (void**)&g_origNESiCAUtilLib_NESiCA_BData_Set);
	//	MH_CreateHookApi(L"NESiCAUtilLib.dll", "NESiCAUtilLib_NESiCA_BData_SetDefault", NESiCAUtilLib_NESiCA_BData_SetDefault, (void**)&g_origNESiCAUtilLib_NESiCA_BData_SetDefault);
	//	MH_CreateHookApi(L"NESiCAUtilLib.dll", "NESiCAUtilLib_NESiCA_TData_GetBuf", NESiCAUtilLib_NESiCA_TData_GetBuf, (void**)&g_origNESiCAUtilLib_NESiCA_TData_GetBuf);
	//	MH_CreateHookApi(L"NESiCAUtilLib.dll", "NESiCAUtilLib_NESiCA_TData_GetSize", NESiCAUtilLib_NESiCA_TData_GetSize, (void**)&g_origNESiCAUtilLib_NESiCA_TData_GetSize);
	//	MH_CreateHookApi(L"NESiCAUtilLib.dll", "BeginPlay", BeginPlay, (void**)&g_origBeginPlay);
	//	MH_CreateHookApi(L"NESiCAUtilLib.dll", "ContinuePlay", ContinuePlay, (void**)&g_origContinuePlay);
	//	MH_CreateHookApi(L"NESiCAUtilLib.dll", "EndPlay", EndPlay, (void**)&g_origEndPlay);
	//	MH_CreateHookApi(L"NESiCAUtilLib.dll", "GetGlobalIP", GetGlobalIP, (void**)&g_origGetGlobalIP);
	//	MH_CreateHookApi(L"NESiCAUtilLib.dll", "GetKeyConfig", GetKeyConfig, (void**)&g_origGetKeyConfig);
	//	MH_CreateHookApi(L"NESiCAUtilLib.dll", "GetLanInterface", GetLanInterface, (void**)&g_origGetLanInterface);
	//	MH_CreateHookApi(L"NESiCAUtilLib.dll", "GetLibraryVersion", GetLibraryVersion, (void**)&g_origGetLibraryVersion);
	//	MH_CreateHookApi(L"NESiCAUtilLib.dll", "GetNWInfo", GetNWInfo, (void**)&g_origGetNWInfo);
	//	MH_CreateHookApi(L"NESiCAUtilLib.dll", "GetNewsImagePath", GetNewsImagePath, (void**)&g_origGetNewsImagePath);
		//MH_CreateHookApi(L"NESiCAUtilLib.dll", "GetRanking", GetRanking, (void**)&g_origGetRanking);
	//	MH_CreateHookApi(L"NESiCAUtilLib.dll", "GetResponseTime", GetResponseTime, (void**)&g_origGetResponseTime);
	//	MH_CreateHookApi(L"NESiCAUtilLib.dll", "GetServerInfo", GetServerInfo, (void**)&g_origGetServerInfo);
	//	MH_CreateHookApi(L"NESiCAUtilLib.dll", "GetServiceVersion", GetServiceVersion, (void**)&g_origGetServiceVersion);
	//	MH_CreateHookApi(L"NESiCAUtilLib.dll", "GetTenpoInfo", GetTenpoInfo, (void**)&g_origGetTenpoInfo);
	//	MH_CreateHookApi(L"NESiCAUtilLib.dll", "IsBeginningProcessFinished", IsBeginningProcessFinished, (void**)&g_origIsBeginningProcessFinished);
	//	MH_CreateHookApi(L"NESiCAUtilLib.dll", "IsDisactive", IsDisactive, (void**)&g_origIsDisactive);
	//	MH_CreateHookApi(L"NESiCAUtilLib.dll", "IsEndingProcessFinished", IsEndingProcessFinished, (void**)&g_origIsEndingProcessFinished);
	//	MH_CreateHookApi(L"NESiCAUtilLib.dll", "IsEventMode", IsEventMode, (void**)&g_origIsEventMode);
	//	MH_CreateHookApi(L"NESiCAUtilLib.dll", "IsReadyDownloadRanking", IsReadyDownloadRanking, (void**)&g_origIsReadyDownloadRanking);
	//	MH_CreateHookApi(L"NESiCAUtilLib.dll", "IsTestMode", IsTestMode, (void**)&g_origIsTestMode);
	//	MH_CreateHookApi(L"NESiCAUtilLib.dll", "IsUploaded", IsUploaded, (void**)&g_origIsUploaded);
	//	MH_CreateHookApi(L"NESiCAUtilLib.dll", "LockProcess", LockProcess, (void**)&g_origLockProcess);
	//	MH_CreateHookApi(L"NESiCAUtilLib.dll", "NESiCA_Card_GetDetailCharacterBattlePointMonth", NESiCA_Card_GetDetailCharacterBattlePointMonth, (void**)&g_origNESiCA_Card_GetDetailCharacterBattlePointMonth);
	//	MH_CreateHookApi(L"NESiCAUtilLib.dll", "NESiCA_Card_GetDetailCharacterBattlePointTotal", NESiCA_Card_GetDetailCharacterBattlePointTotal, (void**)&g_origNESiCA_Card_GetDetailCharacterBattlePointTotal);
	//	MH_CreateHookApi(L"NESiCAUtilLib.dll", "NESiCA_Card_GetDetailCharacterDrawCountMonth", NESiCA_Card_GetDetailCharacterDrawCountMonth, (void**)&g_origNESiCA_Card_GetDetailCharacterDrawCountMonth);
	//	MH_CreateHookApi(L"NESiCAUtilLib.dll", "NESiCA_Card_GetDetailCharacterDrawCountTotal", NESiCA_Card_GetDetailCharacterDrawCountTotal, (void**)&g_origNESiCA_Card_GetDetailCharacterDrawCountTotal);
	//	MH_CreateHookApi(L"NESiCAUtilLib.dll", "NESiCA_Card_GetDetailCharacterLastColorID", NESiCA_Card_GetDetailCharacterLastColorID, (void**)&g_origNESiCA_Card_GetDetailCharacterLastColorID);
	//	MH_CreateHookApi(L"NESiCAUtilLib.dll", "NESiCA_Card_GetDetailCharacterLastComment", NESiCA_Card_GetDetailCharacterLastComment, (void**)&g_origNESiCA_Card_GetDetailCharacterLastComment);
	//	MH_CreateHookApi(L"NESiCAUtilLib.dll", "NESiCA_Card_GetDetailCharacterLastCostumeID", NESiCA_Card_GetDetailCharacterLastCostumeID, (void**)&g_origNESiCA_Card_GetDetailCharacterLastCostumeID);
	//	MH_CreateHookApi(L"NESiCAUtilLib.dll", "NESiCA_Card_GetDetailCharacterLastVTrigger", NESiCA_Card_GetDetailCharacterLastVTrigger, (void**)&g_origNESiCA_Card_GetDetailCharacterLastVTrigger);
	//	MH_CreateHookApi(L"NESiCAUtilLib.dll", "NESiCA_Card_GetDetailCharacterLoseCountMonth", NESiCA_Card_GetDetailCharacterLoseCountMonth, (void**)&g_origNESiCA_Card_GetDetailCharacterLoseCountMonth);
	//	MH_CreateHookApi(L"NESiCAUtilLib.dll", "NESiCA_Card_GetDetailCharacterLoseCountTotal", NESiCA_Card_GetDetailCharacterLoseCountTotal, (void**)&g_origNESiCA_Card_GetDetailCharacterLoseCountTotal);
	//	MH_CreateHookApi(L"NESiCAUtilLib.dll", "NESiCA_Card_GetDetailCharacterUseCountMonth", NESiCA_Card_GetDetailCharacterUseCountMonth, (void**)&g_origNESiCA_Card_GetDetailCharacterUseCountMonth);
	//	MH_CreateHookApi(L"NESiCAUtilLib.dll", "NESiCA_Card_GetDetailCharacterUseCountTotal", NESiCA_Card_GetDetailCharacterUseCountTotal, (void**)&g_origNESiCA_Card_GetDetailCharacterUseCountTotal);
	//	MH_CreateHookApi(L"NESiCAUtilLib.dll", "NESiCA_Card_GetDetailCharacterWinCountMonth", NESiCA_Card_GetDetailCharacterWinCountMonth, (void**)&g_origNESiCA_Card_GetDetailCharacterWinCountMonth);
	//	MH_CreateHookApi(L"NESiCAUtilLib.dll", "NESiCA_Card_GetDetailCharacterWinCountTotal", NESiCA_Card_GetDetailCharacterWinCountTotal, (void**)&g_origNESiCA_Card_GetDetailCharacterWinCountTotal);
	//	MH_CreateHookApi(L"NESiCAUtilLib.dll", "NESiCA_Card_GetDetailControlType", NESiCA_Card_GetDetailControlType, (void**)&g_origNESiCA_Card_GetDetailControlType);
	//	MH_CreateHookApi(L"NESiCAUtilLib.dll", "NESiCA_Card_GetDetailCurrentWins", NESiCA_Card_GetDetailCurrentWins, (void**)&g_origNESiCA_Card_GetDetailCurrentWins);
	//	MH_CreateHookApi(L"NESiCAUtilLib.dll", "NESiCA_Card_GetDetailHighScore", NESiCA_Card_GetDetailHighScore, (void**)&g_origNESiCA_Card_GetDetailHighScore);
	//	MH_CreateHookApi(L"NESiCAUtilLib.dll", "NESiCA_Card_GetDetailHighScoreCharacterID", NESiCA_Card_GetDetailHighScoreCharacterID, (void**)&g_origNESiCA_Card_GetDetailHighScoreCharacterID);
	//	MH_CreateHookApi(L"NESiCAUtilLib.dll", "NESiCA_Card_GetDetailKeyConfigHK", NESiCA_Card_GetDetailKeyConfigHK, (void**)&g_origNESiCA_Card_GetDetailKeyConfigHK);
	//	MH_CreateHookApi(L"NESiCAUtilLib.dll", "NESiCA_Card_GetDetailKeyConfigHP", NESiCA_Card_GetDetailKeyConfigHP, (void**)&g_origNESiCA_Card_GetDetailKeyConfigHP);
	//	MH_CreateHookApi(L"NESiCAUtilLib.dll", "NESiCA_Card_GetDetailKeyConfigHPHK", NESiCA_Card_GetDetailKeyConfigHPHK, (void**)&g_origNESiCA_Card_GetDetailKeyConfigHPHK);
	//	MH_CreateHookApi(L"NESiCAUtilLib.dll", "NESiCA_Card_GetDetailKeyConfigK3", NESiCA_Card_GetDetailKeyConfigK3, (void**)&g_origNESiCA_Card_GetDetailKeyConfigK3);
	//	MH_CreateHookApi(L"NESiCAUtilLib.dll", "NESiCA_Card_GetDetailKeyConfigLK", NESiCA_Card_GetDetailKeyConfigLK, (void**)&g_origNESiCA_Card_GetDetailKeyConfigLK);
	//	MH_CreateHookApi(L"NESiCAUtilLib.dll", "NESiCA_Card_GetDetailKeyConfigLKMK", NESiCA_Card_GetDetailKeyConfigLKMK, (void**)&g_origNESiCA_Card_GetDetailKeyConfigLKMK);
	//	MH_CreateHookApi(L"NESiCAUtilLib.dll", "NESiCA_Card_GetDetailKeyConfigLP", NESiCA_Card_GetDetailKeyConfigLP, (void**)&g_origNESiCA_Card_GetDetailKeyConfigLP);
	//	MH_CreateHookApi(L"NESiCAUtilLib.dll", "NESiCA_Card_GetDetailKeyConfigLPLK", NESiCA_Card_GetDetailKeyConfigLPLK, (void**)&g_origNESiCA_Card_GetDetailKeyConfigLPLK);
	//	MH_CreateHookApi(L"NESiCAUtilLib.dll", "NESiCA_Card_GetDetailKeyConfigLPMK", NESiCA_Card_GetDetailKeyConfigLPMK, (void**)&g_origNESiCA_Card_GetDetailKeyConfigLPMK);
	//	MH_CreateHookApi(L"NESiCAUtilLib.dll", "NESiCA_Card_GetDetailKeyConfigLPMP", NESiCA_Card_GetDetailKeyConfigLPMP, (void**)&g_origNESiCA_Card_GetDetailKeyConfigLPMP);
	//	MH_CreateHookApi(L"NESiCAUtilLib.dll", "NESiCA_Card_GetDetailKeyConfigMK", NESiCA_Card_GetDetailKeyConfigMK, (void**)&g_origNESiCA_Card_GetDetailKeyConfigMK);
	//	MH_CreateHookApi(L"NESiCAUtilLib.dll", "NESiCA_Card_GetDetailKeyConfigMP", NESiCA_Card_GetDetailKeyConfigMP, (void**)&g_origNESiCA_Card_GetDetailKeyConfigMP);
	//	MH_CreateHookApi(L"NESiCAUtilLib.dll", "NESiCA_Card_GetDetailKeyConfigMPLK", NESiCA_Card_GetDetailKeyConfigMPLK, (void**)&g_origNESiCA_Card_GetDetailKeyConfigMPLK);
	//	MH_CreateHookApi(L"NESiCAUtilLib.dll", "NESiCA_Card_GetDetailKeyConfigMPMK", NESiCA_Card_GetDetailKeyConfigMPMK, (void**)&g_origNESiCA_Card_GetDetailKeyConfigMPMK);
	//	MH_CreateHookApi(L"NESiCAUtilLib.dll", "NESiCA_Card_GetDetailKeyConfigP3", NESiCA_Card_GetDetailKeyConfigP3, (void**)&g_origNESiCA_Card_GetDetailKeyConfigP3);
	//	MH_CreateHookApi(L"NESiCAUtilLib.dll", "NESiCA_Card_GetDetailLastCharacterID", NESiCA_Card_GetDetailLastCharacterID, (void**)&g_origNESiCA_Card_GetDetailLastCharacterID);
	//	MH_CreateHookApi(L"NESiCAUtilLib.dll", "NESiCA_Card_GetDetailLeaguePoint", NESiCA_Card_GetDetailLeaguePoint, (void**)&g_origNESiCA_Card_GetDetailLeaguePoint);
	//	MH_CreateHookApi(L"NESiCAUtilLib.dll", "NESiCA_Card_GetDetailMaxWins", NESiCA_Card_GetDetailMaxWins, (void**)&g_origNESiCA_Card_GetDetailMaxWins);
	//	MH_CreateHookApi(L"NESiCAUtilLib.dll", "NESiCA_Card_GetDetailPlayerPointMonth", NESiCA_Card_GetDetailPlayerPointMonth, (void**)&g_origNESiCA_Card_GetDetailPlayerPointMonth);
	//	MH_CreateHookApi(L"NESiCAUtilLib.dll", "NESiCA_Card_GetDetailPlayerPointTotal", NESiCA_Card_GetDetailPlayerPointTotal, (void**)&g_origNESiCA_Card_GetDetailPlayerPointTotal);
	//	MH_CreateHookApi(L"NESiCAUtilLib.dll", "NESiCA_Card_GetPlayerComment", NESiCA_Card_GetPlayerComment, (void**)&g_origNESiCA_Card_GetPlayerComment);
	//	MH_CreateHookApi(L"NESiCAUtilLib.dll", "NESiCA_Card_GetPlayerCommentUTF16", NESiCA_Card_GetPlayerCommentUTF16, (void**)&g_origNESiCA_Card_GetPlayerCommentUTF16);
	//	MH_CreateHookApi(L"NESiCAUtilLib.dll", "NESiCA_Card_GetPlayerMyStoreID", NESiCA_Card_GetPlayerMyStoreID, (void**)&g_origNESiCA_Card_GetPlayerMyStoreID);
	//	MH_CreateHookApi(L"NESiCAUtilLib.dll", "NESiCA_Card_GetPlayerName", NESiCA_Card_GetPlayerName, (void**)&g_origNESiCA_Card_GetPlayerName);
	//	MH_CreateHookApi(L"NESiCAUtilLib.dll", "NESiCA_Card_GetPlayerNameUTF16", NESiCA_Card_GetPlayerNameUTF16, (void**)&g_origNESiCA_Card_GetPlayerNameUTF16);
	//	MH_CreateHookApi(L"NESiCAUtilLib.dll", "NESiCA_Card_GetPlayerTitleID", NESiCA_Card_GetPlayerTitleID, (void**)&g_origNESiCA_Card_GetPlayerTitleID);
	//	MH_CreateHookApi(L"NESiCAUtilLib.dll", "NESiCA_Card_SetDetailCharacterBattlePointMonth", NESiCA_Card_SetDetailCharacterBattlePointMonth, (void**)&g_origNESiCA_Card_SetDetailCharacterBattlePointMonth);
	//	MH_CreateHookApi(L"NESiCAUtilLib.dll", "NESiCA_Card_SetDetailCharacterBattlePointTotal", NESiCA_Card_SetDetailCharacterBattlePointTotal, (void**)&g_origNESiCA_Card_SetDetailCharacterBattlePointTotal);
	//	MH_CreateHookApi(L"NESiCAUtilLib.dll", "NESiCA_Card_SetDetailCharacterDrawCountMonth", NESiCA_Card_SetDetailCharacterDrawCountMonth, (void**)&g_origNESiCA_Card_SetDetailCharacterDrawCountMonth);
	//	MH_CreateHookApi(L"NESiCAUtilLib.dll", "NESiCA_Card_SetDetailCharacterDrawCountTotal", NESiCA_Card_SetDetailCharacterDrawCountTotal, (void**)&g_origNESiCA_Card_SetDetailCharacterDrawCountTotal);
	//	MH_CreateHookApi(L"NESiCAUtilLib.dll", "NESiCA_Card_SetDetailCharacterLastColorID", NESiCA_Card_SetDetailCharacterLastColorID, (void**)&g_origNESiCA_Card_SetDetailCharacterLastColorID);
	//	MH_CreateHookApi(L"NESiCAUtilLib.dll", "NESiCA_Card_SetDetailCharacterLastComment", NESiCA_Card_SetDetailCharacterLastComment, (void**)&g_origNESiCA_Card_SetDetailCharacterLastComment);
	//	MH_CreateHookApi(L"NESiCAUtilLib.dll", "NESiCA_Card_SetDetailCharacterLastCostumeID", NESiCA_Card_SetDetailCharacterLastCostumeID, (void**)&g_origNESiCA_Card_SetDetailCharacterLastCostumeID);
	//	MH_CreateHookApi(L"NESiCAUtilLib.dll", "NESiCA_Card_SetDetailCharacterLastVTrigger", NESiCA_Card_SetDetailCharacterLastVTrigger, (void**)&g_origNESiCA_Card_SetDetailCharacterLastVTrigger);
	//	MH_CreateHookApi(L"NESiCAUtilLib.dll", "NESiCA_Card_SetDetailCharacterLoseCountMonth", NESiCA_Card_SetDetailCharacterLoseCountMonth, (void**)&g_origNESiCA_Card_SetDetailCharacterLoseCountMonth);
	//	MH_CreateHookApi(L"NESiCAUtilLib.dll", "NESiCA_Card_SetDetailCharacterLoseCountTotal", NESiCA_Card_SetDetailCharacterLoseCountTotal, (void**)&g_origNESiCA_Card_SetDetailCharacterLoseCountTotal);
	//	MH_CreateHookApi(L"NESiCAUtilLib.dll", "NESiCA_Card_SetDetailCharacterUseCountMonth", NESiCA_Card_SetDetailCharacterUseCountMonth, (void**)&g_origNESiCA_Card_SetDetailCharacterUseCountMonth);
	//	MH_CreateHookApi(L"NESiCAUtilLib.dll", "NESiCA_Card_SetDetailCharacterUseCountTotal", NESiCA_Card_SetDetailCharacterUseCountTotal, (void**)&g_origNESiCA_Card_SetDetailCharacterUseCountTotal);
	//	MH_CreateHookApi(L"NESiCAUtilLib.dll", "NESiCA_Card_SetDetailCharacterWinCountMonth", NESiCA_Card_SetDetailCharacterWinCountMonth, (void**)&g_origNESiCA_Card_SetDetailCharacterWinCountMonth);
	//	MH_CreateHookApi(L"NESiCAUtilLib.dll", "NESiCA_Card_SetDetailCharacterWinCountTotal", NESiCA_Card_SetDetailCharacterWinCountTotal, (void**)&g_origNESiCA_Card_SetDetailCharacterWinCountTotal);
	//	MH_CreateHookApi(L"NESiCAUtilLib.dll", "NESiCA_Card_SetDetailControlType", NESiCA_Card_SetDetailControlType, (void**)&g_origNESiCA_Card_SetDetailControlType);
	//	MH_CreateHookApi(L"NESiCAUtilLib.dll", "NESiCA_Card_SetDetailCurrentWins", NESiCA_Card_SetDetailCurrentWins, (void**)&g_origNESiCA_Card_SetDetailCurrentWins);
	//	MH_CreateHookApi(L"NESiCAUtilLib.dll", "NESiCA_Card_SetDetailHighScore", NESiCA_Card_SetDetailHighScore, (void**)&g_origNESiCA_Card_SetDetailHighScore);
	//	MH_CreateHookApi(L"NESiCAUtilLib.dll", "NESiCA_Card_SetDetailHighScoreCharacterID", NESiCA_Card_SetDetailHighScoreCharacterID, (void**)&g_origNESiCA_Card_SetDetailHighScoreCharacterID);
	//	MH_CreateHookApi(L"NESiCAUtilLib.dll", "NESiCA_Card_SetDetailKeyConfigHK", NESiCA_Card_SetDetailKeyConfigHK, (void**)&g_origNESiCA_Card_SetDetailKeyConfigHK);
	//	MH_CreateHookApi(L"NESiCAUtilLib.dll", "NESiCA_Card_SetDetailKeyConfigHP", NESiCA_Card_SetDetailKeyConfigHP, (void**)&g_origNESiCA_Card_SetDetailKeyConfigHP);
	//	MH_CreateHookApi(L"NESiCAUtilLib.dll", "NESiCA_Card_SetDetailKeyConfigHPHK", NESiCA_Card_SetDetailKeyConfigHPHK, (void**)&g_origNESiCA_Card_SetDetailKeyConfigHPHK);
	//	MH_CreateHookApi(L"NESiCAUtilLib.dll", "NESiCA_Card_SetDetailKeyConfigK3", NESiCA_Card_SetDetailKeyConfigK3, (void**)&g_origNESiCA_Card_SetDetailKeyConfigK3);
	//	MH_CreateHookApi(L"NESiCAUtilLib.dll", "NESiCA_Card_SetDetailKeyConfigLK", NESiCA_Card_SetDetailKeyConfigLK, (void**)&g_origNESiCA_Card_SetDetailKeyConfigLK);
	//	MH_CreateHookApi(L"NESiCAUtilLib.dll", "NESiCA_Card_SetDetailKeyConfigLKMK", NESiCA_Card_SetDetailKeyConfigLKMK, (void**)&g_origNESiCA_Card_SetDetailKeyConfigLKMK);
	//	MH_CreateHookApi(L"NESiCAUtilLib.dll", "NESiCA_Card_SetDetailKeyConfigLP", NESiCA_Card_SetDetailKeyConfigLP, (void**)&g_origNESiCA_Card_SetDetailKeyConfigLP);
	//	MH_CreateHookApi(L"NESiCAUtilLib.dll", "NESiCA_Card_SetDetailKeyConfigLPLK", NESiCA_Card_SetDetailKeyConfigLPLK, (void**)&g_origNESiCA_Card_SetDetailKeyConfigLPLK);
	//	MH_CreateHookApi(L"NESiCAUtilLib.dll", "NESiCA_Card_SetDetailKeyConfigLPMK", NESiCA_Card_SetDetailKeyConfigLPMK, (void**)&g_origNESiCA_Card_SetDetailKeyConfigLPMK);
	//	MH_CreateHookApi(L"NESiCAUtilLib.dll", "NESiCA_Card_SetDetailKeyConfigLPMP", NESiCA_Card_SetDetailKeyConfigLPMP, (void**)&g_origNESiCA_Card_SetDetailKeyConfigLPMP);
	//	MH_CreateHookApi(L"NESiCAUtilLib.dll", "NESiCA_Card_SetDetailKeyConfigMK", NESiCA_Card_SetDetailKeyConfigMK, (void**)&g_origNESiCA_Card_SetDetailKeyConfigMK);
	//	MH_CreateHookApi(L"NESiCAUtilLib.dll", "NESiCA_Card_SetDetailKeyConfigMP", NESiCA_Card_SetDetailKeyConfigMP, (void**)&g_origNESiCA_Card_SetDetailKeyConfigMP);
	//	MH_CreateHookApi(L"NESiCAUtilLib.dll", "NESiCA_Card_SetDetailKeyConfigMPLK", NESiCA_Card_SetDetailKeyConfigMPLK, (void**)&g_origNESiCA_Card_SetDetailKeyConfigMPLK);
	//	MH_CreateHookApi(L"NESiCAUtilLib.dll", "NESiCA_Card_SetDetailKeyConfigMPMK", NESiCA_Card_SetDetailKeyConfigMPMK, (void**)&g_origNESiCA_Card_SetDetailKeyConfigMPMK);
	//	MH_CreateHookApi(L"NESiCAUtilLib.dll", "NESiCA_Card_SetDetailKeyConfigP3", NESiCA_Card_SetDetailKeyConfigP3, (void**)&g_origNESiCA_Card_SetDetailKeyConfigP3);
	//	MH_CreateHookApi(L"NESiCAUtilLib.dll", "NESiCA_Card_SetDetailLastCharacterID", NESiCA_Card_SetDetailLastCharacterID, (void**)&g_origNESiCA_Card_SetDetailLastCharacterID);
	//	MH_CreateHookApi(L"NESiCAUtilLib.dll", "NESiCA_Card_SetDetailLeaguePoint", NESiCA_Card_SetDetailLeaguePoint, (void**)&g_origNESiCA_Card_SetDetailLeaguePoint);
	//	MH_CreateHookApi(L"NESiCAUtilLib.dll", "NESiCA_Card_SetDetailMaxWins", NESiCA_Card_SetDetailMaxWins, (void**)&g_origNESiCA_Card_SetDetailMaxWins);
	//	MH_CreateHookApi(L"NESiCAUtilLib.dll", "NESiCA_Card_SetDetailPlayerPointMonth", NESiCA_Card_SetDetailPlayerPointMonth, (void**)&g_origNESiCA_Card_SetDetailPlayerPointMonth);
	//	MH_CreateHookApi(L"NESiCAUtilLib.dll", "NESiCA_Card_SetDetailPlayerPointTotal", NESiCA_Card_SetDetailPlayerPointTotal, (void**)&g_origNESiCA_Card_SetDetailPlayerPointTotal);
	//	MH_CreateHookApi(L"NESiCAUtilLib.dll", "NESiCA_Card_SetPlayerComment", NESiCA_Card_SetPlayerComment, (void**)&g_origNESiCA_Card_SetPlayerComment);
	//	MH_CreateHookApi(L"NESiCAUtilLib.dll", "NESiCA_Card_SetPlayerCommentUTF16", NESiCA_Card_SetPlayerCommentUTF16, (void**)&g_origNESiCA_Card_SetPlayerCommentUTF16);
	//	MH_CreateHookApi(L"NESiCAUtilLib.dll", "NESiCA_Card_SetPlayerMyStoreID", NESiCA_Card_SetPlayerMyStoreID, (void**)&g_origNESiCA_Card_SetPlayerMyStoreID);
	//	MH_CreateHookApi(L"NESiCAUtilLib.dll", "NESiCA_Card_SetPlayerName", NESiCA_Card_SetPlayerName, (void**)&g_origNESiCA_Card_SetPlayerName);
	//	MH_CreateHookApi(L"NESiCAUtilLib.dll", "NESiCA_Card_SetPlayerNameUTF16", NESiCA_Card_SetPlayerNameUTF16, (void**)&g_origNESiCA_Card_SetPlayerNameUTF16);
	//	MH_CreateHookApi(L"NESiCAUtilLib.dll", "NESiCA_Card_SetPlayerTitleID", NESiCA_Card_SetPlayerTitleID, (void**)&g_origNESiCA_Card_SetPlayerTitleID);
	//	MH_CreateHookApi(L"NESiCAUtilLib.dll", "NESiCA_GetNESiCAID", NESiCA_GetNESiCAID, (void**)&g_origNESiCA_GetNESiCAID);
	//	MH_CreateHookApi(L"NESiCAUtilLib.dll", "NESiCA_GetNESiCAType", NESiCA_GetNESiCAType, (void**)&g_origNESiCA_GetNESiCAType);
	//	MH_CreateHookApi(L"NESiCAUtilLib.dll", "NESiCA_IsCardPlayer", NESiCA_IsCardPlayer, (void**)&g_origNESiCA_IsCardPlayer);
	//	MH_CreateHookApi(L"NESiCAUtilLib.dll", "NESiCA_Rank_GetBattlePointRank", NESiCA_Rank_GetBattlePointRank, (void**)&g_origNESiCA_Rank_GetBattlePointRank);
	//	MH_CreateHookApi(L"NESiCAUtilLib.dll", "NESiCA_Rank_GetBattlePointRankPref", NESiCA_Rank_GetBattlePointRankPref, (void**)&g_origNESiCA_Rank_GetBattlePointRankPref);
	//	MH_CreateHookApi(L"NESiCAUtilLib.dll", "NESiCA_Rank_GetBattlePointRankPrefId", NESiCA_Rank_GetBattlePointRankPrefId, (void**)&g_origNESiCA_Rank_GetBattlePointRankPrefId);
	//	MH_CreateHookApi(L"NESiCAUtilLib.dll", "NESiCA_Rank_GetBattlePointRankPrefName", NESiCA_Rank_GetBattlePointRankPrefName, (void**)&g_origNESiCA_Rank_GetBattlePointRankPrefName);
	//	MH_CreateHookApi(L"NESiCAUtilLib.dll", "NESiCA_Rank_GetBattlePointRankPrefNameUTF16", NESiCA_Rank_GetBattlePointRankPrefNameUTF16, (void**)&g_origNESiCA_Rank_GetBattlePointRankPrefNameUTF16);
	//	MH_CreateHookApi(L"NESiCAUtilLib.dll", "Reconnect", Reconnect, (void**)&g_origReconnect);
	//	MH_CreateHookApi(L"NESiCAUtilLib.dll", "SaveOnTheWay", SaveOnTheWay, (void**)&g_origSaveOnTheWay);
	//	MH_CreateHookApi(L"NESiCAUtilLib.dll", "SetCharacterID", SetCharacterID, (void**)&g_origSetCharacterID);
	//	MH_CreateHookApi(L"NESiCAUtilLib.dll", "SetEventMode", SetEventMode, (void**)&g_origSetEventMode);
	//	MH_CreateHookApi(L"NESiCAUtilLib.dll", "SetEventModeResult", SetEventModeResult, (void**)&g_origSetEventModeResult);
	//	MH_CreateHookApi(L"NESiCAUtilLib.dll", "SetOpenCloseTime", SetOpenCloseTime, (void**)&g_origSetOpenCloseTime);
	//	MH_CreateHookApi(L"NESiCAUtilLib.dll", "StartTestMode", StartTestMode, (void**)&g_origStartTestMode);
	//	MH_CreateHookApi(L"NESiCAUtilLib.dll", "TypeXGetTelop", TypeXGetTelop, (void**)&g_origTypeXGetTelop);
	//	MH_CreateHookApi(L"NESiCAUtilLib.dll", "TypeXMatching_ChangeWinLimit", TypeXMatching_ChangeWinLimit, (void**)&g_origTypeXMatching_ChangeWinLimit);
	//	MH_CreateHookApi(L"NESiCAUtilLib.dll", "TypeXMatching_CreateInstance", TypeXMatching_CreateInstance, (void**)&g_origTypeXMatching_CreateInstance);
	//	MH_CreateHookApi(L"NESiCAUtilLib.dll", "TypeXMatching_DeleteInstance", TypeXMatching_DeleteInstance, (void**)&g_origTypeXMatching_DeleteInstance);
	//	MH_CreateHookApi(L"NESiCAUtilLib.dll", "TypeXMatching_GetEnemyAddr", TypeXMatching_GetEnemyAddr, (void**)&g_origTypeXMatching_GetEnemyAddr);
	//	MH_CreateHookApi(L"NESiCAUtilLib.dll", "TypeXMatching_GetEnemyEntryId", TypeXMatching_GetEnemyEntryId, (void**)&g_origTypeXMatching_GetEnemyEntryId);
	//	MH_CreateHookApi(L"NESiCAUtilLib.dll", "TypeXMatching_GetEnemyOption", TypeXMatching_GetEnemyOption, (void**)&g_origTypeXMatching_GetEnemyOption);
	//	MH_CreateHookApi(L"NESiCAUtilLib.dll", "TypeXMatching_GetEnemyPlayerName", TypeXMatching_GetEnemyPlayerName, (void**)&g_origTypeXMatching_GetEnemyPlayerName);
	//	MH_CreateHookApi(L"NESiCAUtilLib.dll", "TypeXMatching_GetEnemyPlayerNameUTF16", TypeXMatching_GetEnemyPlayerNameUTF16, (void**)&g_origTypeXMatching_GetEnemyPlayerNameUTF16);
	//	MH_CreateHookApi(L"NESiCAUtilLib.dll", "TypeXMatching_GetEnemyTenpoName", TypeXMatching_GetEnemyTenpoName, (void**)&g_origTypeXMatching_GetEnemyTenpoName);
	//	MH_CreateHookApi(L"NESiCAUtilLib.dll", "TypeXMatching_GetEnemyTenpoNameUTF16", TypeXMatching_GetEnemyTenpoNameUTF16, (void**)&g_origTypeXMatching_GetEnemyTenpoNameUTF16);
	//	MH_CreateHookApi(L"NESiCAUtilLib.dll", "TypeXMatching_GetFighterInfoInStore", TypeXMatching_GetFighterInfoInStore, (void**)&g_origTypeXMatching_GetFighterInfoInStore);
	//	MH_CreateHookApi(L"NESiCAUtilLib.dll", "TypeXMatching_GetMyAddr", TypeXMatching_GetMyAddr, (void**)&g_origTypeXMatching_GetMyAddr);
	//	MH_CreateHookApi(L"NESiCAUtilLib.dll", "TypeXMatching_GetMyEntryId", TypeXMatching_GetMyEntryId, (void**)&g_origTypeXMatching_GetMyEntryId);
	//	MH_CreateHookApi(L"NESiCAUtilLib.dll", "TypeXMatching_GetRTT", TypeXMatching_GetRTT, (void**)&g_origTypeXMatching_GetRTT);
	//	MH_CreateHookApi(L"NESiCAUtilLib.dll", "TypeXMatching_GetStatus", TypeXMatching_GetStatus, (void**)&g_origTypeXMatching_GetStatus);
	//	MH_CreateHookApi(L"NESiCAUtilLib.dll", "TypeXMatching_GetWinLimit", TypeXMatching_GetWinLimit, (void**)&g_origTypeXMatching_GetWinLimit);
	//	MH_CreateHookApi(L"NESiCAUtilLib.dll", "TypeXMatching_GetWins", TypeXMatching_GetWins, (void**)&g_origTypeXMatching_GetWins);
	//	MH_CreateHookApi(L"NESiCAUtilLib.dll", "TypeXMatching_GetWinsAll", TypeXMatching_GetWinsAll, (void**)&g_origTypeXMatching_GetWinsAll);
	//	MH_CreateHookApi(L"NESiCAUtilLib.dll", "TypeXMatching_IsMatchingDesignatedFailed", TypeXMatching_IsMatchingDesignatedFailed, (void**)&g_origTypeXMatching_IsMatchingDesignatedFailed);
	//	MH_CreateHookApi(L"NESiCAUtilLib.dll", "TypeXMatching_Register_NESiCAUtilLibCancelRequestRFID", TypeXMatching_Register_NESiCAUtilLibCancelRequestRFID, (void**)&g_origTypeXMatching_Register_NESiCAUtilLibCancelRequestRFID);
	//	MH_CreateHookApi(L"NESiCAUtilLib.dll", "TypeXMatching_Register_NESiCAUtilLibGetRFIDID", TypeXMatching_Register_NESiCAUtilLibGetRFIDID, (void**)&g_origTypeXMatching_Register_NESiCAUtilLibGetRFIDID);
	//	MH_CreateHookApi(L"NESiCAUtilLib.dll", "TypeXMatching_Register_NESiCAUtilLibIsRFIDRead", TypeXMatching_Register_NESiCAUtilLibIsRFIDRead, (void**)&g_origTypeXMatching_Register_NESiCAUtilLibIsRFIDRead);
	//	MH_CreateHookApi(L"NESiCAUtilLib.dll", "TypeXMatching_Register_NESiCAUtilLibIsRFIDReadSuccess", TypeXMatching_Register_NESiCAUtilLibIsRFIDReadSuccess, (void**)&g_origTypeXMatching_Register_NESiCAUtilLibIsRFIDReadSuccess);
	//	MH_CreateHookApi(L"NESiCAUtilLib.dll", "TypeXMatching_Register_NESiCAUtilLibRequestRFID", TypeXMatching_Register_NESiCAUtilLibRequestRFID, (void**)&g_origTypeXMatching_Register_NESiCAUtilLibRequestRFID);
	//	MH_CreateHookApi(L"NESiCAUtilLib.dll", "TypeXMatching_ResetWins", TypeXMatching_ResetWins, (void**)&g_origTypeXMatching_ResetWins);
	//	MH_CreateHookApi(L"NESiCAUtilLib.dll", "TypeXMatching_Resume", TypeXMatching_Resume, (void**)&g_origTypeXMatching_Resume);
	//	MH_CreateHookApi(L"NESiCAUtilLib.dll", "TypeXMatching_StartMatchingDesignated", TypeXMatching_StartMatchingDesignated, (void**)&g_origTypeXMatching_StartMatchingDesignated);
	//	MH_CreateHookApi(L"NESiCAUtilLib.dll", "TypeXMatching_StartMathingInternetOnly", TypeXMatching_StartMathingInternetOnly, (void**)&g_origTypeXMatching_StartMathingInternetOnly);
	//	MH_CreateHookApi(L"NESiCAUtilLib.dll", "TypeXMatching_StartMathingStoreOnly", TypeXMatching_StartMathingStoreOnly, (void**)&g_origTypeXMatching_StartMathingStoreOnly);
	//	MH_CreateHookApi(L"NESiCAUtilLib.dll", "TypeXMatching_StartMathingStorePriority", TypeXMatching_StartMathingStorePriority, (void**)&g_origTypeXMatching_StartMathingStorePriority);
	//	MH_CreateHookApi(L"NESiCAUtilLib.dll", "TypeXMatching_StopMatching", TypeXMatching_StopMatching, (void**)&g_origTypeXMatching_StopMatching);
	//	MH_CreateHookApi(L"NESiCAUtilLib.dll", "TypeXMatching_Suspend", TypeXMatching_Suspend, (void**)&g_origTypeXMatching_Suspend);
	//	MH_CreateHookApi(L"NESiCAUtilLib.dll", "TypeXMatching_Update", TypeXMatching_Update, (void**)&g_origTypeXMatching_Update);
	//	MH_CreateHookApi(L"NESiCAUtilLib.dll", "TypeXMatching_UpdateWins", TypeXMatching_UpdateWins, (void**)&g_origTypeXMatching_UpdateWins);
	//	MH_CreateHookApi(L"NESiCAUtilLib.dll", "TypeXPlayRecordAddToSend", TypeXPlayRecordAddToSend, (void**)&g_origTypeXPlayRecordAddToSend);
	//	MH_CreateHookApi(L"NESiCAUtilLib.dll", "TypeXPlayRecordReset", TypeXPlayRecordReset, (void**)&g_origTypeXPlayRecordReset);
	//	MH_CreateHookApi(L"NESiCAUtilLib.dll", "TypeXPlayRecordResetRevenge", TypeXPlayRecordResetRevenge, (void**)&g_origTypeXPlayRecordResetRevenge);
	//	MH_CreateHookApi(L"NESiCAUtilLib.dll", "TypeXPlayRecordSetPlayerInfo", TypeXPlayRecordSetPlayerInfo, (void**)&g_origTypeXPlayRecordSetPlayerInfo);
	//	MH_CreateHookApi(L"NESiCAUtilLib.dll", "TypeXPlayRecordSetRoundResult", TypeXPlayRecordSetRoundResult, (void**)&g_origTypeXPlayRecordSetRoundResult);
	//	MH_CreateHookApi(L"NESiCAUtilLib.dll", "TypeXSafeFileIsFinished", TypeXSafeFileIsFinished, (void**)&g_origTypeXSafeFileIsFinished);
	//	MH_CreateHookApi(L"NESiCAUtilLib.dll", "TypeXSafeFileRead", TypeXSafeFileRead, (void**)&g_origTypeXSafeFileRead);
	//	MH_CreateHookApi(L"NESiCAUtilLib.dll", "TypeXSafeFileWrite", TypeXSafeFileWrite, (void**)&g_origTypeXSafeFileWrite);
	//	MH_CreateHookApi(L"NESiCAUtilLib.dll", "TypeXSafeFileWriteUpdate", TypeXSafeFileWriteUpdate, (void**)&g_origTypeXSafeFileWriteUpdate);
	//	MH_CreateHookApi(L"NESiCAUtilLib.dll", "UnlockProcess", UnlockProcess, (void**)&g_origUnlockProcess);
		MH_EnableHook(MH_ALL_HOOKS);

	// remove all kanji onscreen nesys messages
		if (ToBool(config["General"]["RemoveNesysMessages"]))
		injector::WriteMemoryRaw((nesysbase + 0x1EBE2), "\xE9\xE4\x01\x00\x00", 5, true);
 
	// funny test
		//injector::WriteMemoryRaw((nesysbase + 0x65790), "\x50\x4F\x43\x4B\x59\x21\x21", 7, true);

	// NESYS INIT
		init_RegHooks();
		init_NesysEmu(false);

	// JAP FONT FIX
		FixJapaneseFont();

	// SET COMMAND LINE SWITCHES FOR FULLSCREEN/WINDOWED MODE
		GetModuleFileNameW(NULL, FullPathWithExeName, MAX_PATH);
		SetCommandLineSFV();
		
		MH_Initialize();
		MH_CreateHookApi(L"kernel32.dll", "CreateFileW", &CreateFileWSFV, (void**)&original_CreateFileWSFV);
		MH_CreateHookApi(L"kernel32.dll", "DeleteFileW", &DeleteFileWSFV, (void**)&original_DeleteFileWSFV);
		MH_CreateHookApi(L"kernel32.dll", "FindFirstFileW", &FindFirstFileWSFV, (void**)&original_FindFirstFileWSFV);
		MH_CreateHookApi(L"shell32.dll", "SHGetFolderPathW", &SHGetFolderPathWSFV, (void**)&original_SHGetFolderPathWSFV);
		MH_CreateHookApi(L"kernel32.dll", "GetCommandLineW", &GetCommandLineWSFV, (void**)&original_GetCommandLineWSFV);
		MH_CreateHookApi(L"xinput1_3.dll", "XInputGetState", &XInputGetStateSFV, NULL);
		MH_CreateHookApi(L"user32.dll", "GetRawInputDeviceList", &GetRawInputDeviceListSFV, NULL);
		MH_CreateHook((void*)(imageBase + 0xf52860), Hook_WndProc, (void**)&g_origWndProc);
		MH_CreateHookApi(L"kernel32.dll", "GetDiskFreeSpaceExW", GetDiskFreeSpaceExWWrap, (void**)&g_origGetDiskFreeSpaceExW);
		MH_CreateHookApi(L"kernel32.dll", "CreateDirectoryW", CreateDirectoryWWrap, (void**)&g_origCreateDirectoryW);
		MH_CreateHookApi(L"kernel32.dll", "GetFileAttributesExW", &GetFileAttributesExWWrap, (void**)&g_origGetFileAttributesExW);
		MH_EnableHook(MH_ALL_HOOKS); 
	}, GameID::SFV);
#endif
#endif