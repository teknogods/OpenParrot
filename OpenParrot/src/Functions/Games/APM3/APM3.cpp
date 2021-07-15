#include <Utility/InitFunction.h>
#include <StdInc.h>
#include <thread>
#ifdef _M_AMD64
#include "Functions\Global.h"
#include "APM3.h"
#include "Aime.h"
#include "Backup.h"
#include "Auth.h"
#include "Sequence.h"

static uint8_t g_APM3IOValues[256];

bool __fastcall ApmSystemSetting_getAdvertizeSound(bool *enable)
{
#ifdef _LOGAPM3
	info(true, "ApmSystemSetting_getAdvertizeSound");
#endif
	*enable = true;
	return ApmSystemSetting_getAdvertizeSoundReturnValue;
}

bool __fastcall ApmSystemSetting_getClosingTimes(__int64 *closingTimes)
{
#ifdef _LOGAPM3
	info(true, "ApmSystemSetting_getClosingTimes");
#endif
	*closingTimes = 0;
	return ApmSystemSetting_getClosingTimesReturnValue;
}

char __fastcall ApmSystemSetting_getEmoneySetting(__int64 a1)
{
#ifdef _LOGAPM3
	info(true, "ApmSystemSetting_getEmoneySetting");
#endif
	return ApmSystemSetting_getEmoneySettingReturnValue;
}

bool __fastcall ApmSystemSetting_getFixedTitle(bool *enable)
{
#ifdef _LOGAPM3
	info(true, "ApmSystemSetting_getFixedTitle");
#endif
	*enable = false;
	return ApmSystemSetting_getFixedTitleReturnValue;
}

char __fastcall ApmSystemSetting_getGamePadSetting(WORD* a1)
{
#ifdef _LOGAPM3
	info(true, "ApmSystemSetting_getGamePadSetting");
#endif
	return ApmSystemSetting_getGamePadSettingReturnValue;
}

struct MatchingGroup
{
	char alphabet[1];
	char number[1];
};

bool __fastcall ApmSystemSetting_getMatchingGroup(MatchingGroup* matchingGroup)
{
#ifdef _LOGAPM3
	info(true, "ApmSystemSetting_getMatchingGroup");
#endif
	return ApmSystemSetting_getMatchingGroupReturnValue;
}

bool __fastcall ApmSystemSetting_getTimeToClosingTime(unsigned int* seconds)
{
#ifdef _LOGAPM3
	info(true, "ApmSystemSetting_getTimeToClosingTime");
#endif
	*seconds = 0;
	return ApmSystemSetting_getTimeToClosingTimeReturnValue;
}

char __fastcall ApmSystemSetting_getUiSetting(__int64 a1)
{
#ifdef _LOGAPM3
	info(true, "ApmSystemSetting_getUiSetting");
#endif
	return ApmSystemSetting_getUiSettingReturnValue;
}

void Core_execute()
{
#ifdef _LOGAPM3
	info(true, "Core_execute");
#endif
}

bool __fastcall Core_exitGame(bool isTest)
{
#ifdef _LOGAPM3
	info(true, "Core_exitGame");
#endif
	return Core_exitGameReturnValue;
}

__int64 Core_hookException()
{
#ifdef _LOGAPM3
	info(true, "Core_hookException");
#endif
	return Core_hookExceptionReturnValue;
}

bool Core_isExitNeeded()
{
#ifdef _LOGAPM3
	info(true, "Core_isExitNeeded");
#endif
	return Core_isExitNeededReturnValue;
}

void Core_resetCount()
{
#ifdef _LOGAPM3
	info(true, "Core_resetCount");
#endif
}

__int64 Core_resetException()
{
#ifdef _LOGAPM3
	info(true, "Core_resetException");
#endif
	return Core_resetExceptionReturnValue;
}

__int64 Credit_getAddableCoin()
{
#ifdef _LOGAPM3
	info(true, "Credit_getAddableCoin");
#endif
	return Credit_getAddableCoinReturnValue;
}

__int64 Credit_getCoinMultipliers()
{
#ifdef _LOGAPM3
	info(true, "Credit_getCoinMultipliers");
#endif
	return Credit_getCoinMultipliersReturnValue;
}

__int64 Credit_getCoinToCredit()
{
#ifdef _LOGAPM3
	info(true, "Credit_getCoinToCredit");
#endif
	return Credit_getCoinToCreditReturnValue;
}

__int64 Credit_getCredit()
{
#ifdef _LOGAPM3
	info(true, "Credit_getCredit");
#endif
	return Credit_getCreditReturnValue;
}

__int64 Credit_getRemain()
{
#ifdef _LOGAPM3
	info(true, "Credit_getRemain");
#endif
	return Credit_getRemainReturnValue;
}

__int64 Credit_isFreePlay()
{
#ifdef _LOGAPM3
	info(true, "Credit_isFreePlay");
#endif
	return Credit_isFreePlayReturnValue;
}

__int64 __fastcall Credit_isGameCostEnough(__int64 a1)
{
#ifdef _LOGAPM3
	info(true, "Credit_isGameCostEnough");
#endif
	return Credit_isGameCostEnoughReturnValue; // 1
}

__int64 Credit_isZero()
{
#ifdef _LOGAPM3
	info(true, "Credit_isZero");
#endif
	return Credit_isZeroReturnValue;
}

__int64 __fastcall Credit_payGameCost(__int64 a1)
{
#ifdef _LOGAPM3
	info(true, "Credit_payGameCost");
#endif
	return Credit_payGameCostReturnValue;
}

__int64 Credit_resetCoinInHook()
{
#ifdef _LOGAPM3
	info(true, "Credit_resetCoinInHook");
#endif

	return Credit_resetCoinInHookReturnValue;
}

__int64 Credit_setCoinInHook()
{
#ifdef _LOGAPM3
	info(true, "Credit_setCoinInHook");
#endif

	return Credit_setCoinInHookReturnValue;
}

wchar_t* Credit_toString()
{
#ifdef _LOGAPM3
	info(true, "Credit_toString");
#endif

	static wchar_t lol[64];
	lol[0] = '1';
	return lol;
}

void Emoney_close()
{
#ifdef _LOGAPM3
	info(true, "Emoney_close");
#endif
}

__int64 Emoney_isOpenMainWindow()
{
#ifdef _LOGAPM3
	info(true, "Emoney_isOpenMainWindow");
#endif
	return Emoney_isOpenMainWindowReturnValue;
}

char Emoney_launch()
{
#ifdef _LOGAPM3
	info(true, "Emoney_launch");
#endif
	return Emoney_launchReturnValue;
}

void __fastcall Emoney_setSoundHook(__int64 a1)
{
#ifdef _LOGAPM3
	info(true, "Emoney_setSoundHook");
#endif
}

void __fastcall Emoney_update(BYTE* a1)
{
#ifdef _LOGAPM3
	info(true, "Emoney_update");
#endif
}

__int64 Error_isOccurred()
{
#ifdef _LOGAPM3
	info(true, "Error_isOccurred");
#endif
	return Error_isOccurredReturnValue;
}

__int64 Error_setDeviceLost()
{
#ifdef _LOGAPM3
	info(true, "Error_setDeviceLost");
#endif
	return Error_setDeviceLostReturnValue;
}

struct GamepadInfo
{
	unsigned short vid;
	unsigned short pid;
	char name[260];
};

GamepadInfo* Input_getGamepadInfo()
{
#ifdef _LOGAPM3
	info(true, "Input_getGamepadInfo");
#endif
	static GamepadInfo *_gamePad;
	if (_gamePad == nullptr)
	{
		_gamePad = (GamepadInfo*)malloc(sizeof(GamepadInfo));
	}

	_gamePad->vid = 1337;
	_gamePad->pid = 1337;
	memset(_gamePad->name, 0, 260);
	strcpy(_gamePad->name, "ParrotJoyCon");
	return _gamePad;
}

char __fastcall Input_isFlipNow(unsigned int a1)
{
#ifdef _LOGAPM3
	info(true, "Input_isFlipNow");
#endif
	return Input_isFlipNowReturnValue;
}

__int64 Input_isGamepadDetect()
{
#ifdef _LOGAPM3
	info(true, "Input_isGamepadDetect");
#endif
	return Input_isGamepadDetectReturnValue;
}

char __fastcall Input_isOffNow(unsigned int a1)
{
#ifdef _LOGAPM3
	info(true, "Input_isOffNow");
#endif
	return Input_isOffNowReturnValue;
}

int __fastcall Input_isOn(unsigned int buttonId)
{
#ifdef _LOGAPM3
	info(true, "Input_isOn id: %u", buttonId);
#endif
	return g_APM3IOValues[buttonId];
}

char __fastcall Input_isOnNow(unsigned int buttonId)
{
#ifdef _LOGAPM3
	info(true, "Input_isOnNow id: %u", buttonId);
#endif
	return g_APM3IOValues[buttonId];
}

__int64 Input_isOpenPewviewWindow()
{
#ifdef _LOGAPM3
	info(true, "Input_isOpenPewviewWindow");
#endif
	return Input_isOpenPewviewWindowReturnValue;
}

char __fastcall Input_setGamepadConfig(__int64 a1)
{
#ifdef _LOGAPM3
	info(true, "Input_setGamepadConfig");
#endif
	return Input_setGamepadConfigReturnValue;
}

void** NetworkProperty_getAddressString()
{
#ifdef _DEBUG
	info(true, "NetworkProperty_getAddressString");
#endif
	return NetworkProperty_getAddressStringReturnValue;
}

__int64 NetworkProperty_getAddressValue()
{
#ifdef _DEBUG
	info(true, "NetworkProperty_getAddressValue");
#endif
	return NetworkProperty_getAddressValueReturnValue;
}

__int64 System_getAppRootPath()
{
#ifdef _LOGAPM3
	info(true, "System_getAppRootPath");
#endif
	return System_getAppRootPathReturnValue;
}

struct StandardSerialID
{
	const BYTE LENGTH = 16;
	wchar_t Value[15];
};

struct ShortSerialID
{
	const BYTE LENGTH = 12;
	wchar_t Value[12];
};

struct SerialID
{
	StandardSerialID ID;
	ShortSerialID ShortId;
};

SerialID* System_getBoardId()
{
#ifdef _LOGAPM3
	info(true, "System_getBoardId");
#endif
	static SerialID *_serialId;
	if (_serialId == nullptr)
	{
		_serialId = (SerialID*)malloc(sizeof(SerialID));
		wcscpy(_serialId->ID.Value, L"IAMGODBEAVER");
		wcscpy(_serialId->ShortId.Value, L"IAMGODBEAVER");
	}
	return _serialId;
}

wchar_t* System_getGameId()
{
#ifdef _LOGAPM3
	info(true, "System_getGameId");
#endif
	return APM3GameId;
}


struct GameVersion
{
	unsigned int Major;
	unsigned int Minor;
};

GameVersion* System_getGameVersion()
{
#ifdef _LOGAPM3
	info(true, "System_getGameVersion");
#endif
	static GameVersion *_gameVersion;
	if (_gameVersion == nullptr)
	{
		_gameVersion = (GameVersion*)malloc(sizeof(GameVersion));
		_gameVersion->Minor = 0;
		_gameVersion->Major = 1;
	}
	return _gameVersion;
}

SerialID* System_getKeychipId()
{
#ifdef _LOGAPM3
	info(true, "System_getKeychipId");
#endif
	static SerialID* _serialId;
	if (_serialId == nullptr)
	{
		_serialId = (SerialID*)malloc(sizeof(SerialID));
		wcscpy(_serialId->ID.Value, L"IAMGODBEAVER");
		wcscpy(_serialId->ShortId.Value, L"IAMGODBEAVER");
	}
	return _serialId;
}

enum class SystemRegionCode
{
	Unknown,
	Japan,
	China,
	Export,
};

SystemRegionCode System_getRegionCode()
{
#ifdef _LOGAPM3
	info(true, "System_getRegionCode");
#endif
	return SystemRegionCode::Japan;
}

__int64 System_getResolution()
{
#ifdef _LOGAPM3
	info(true, "System_getResolution");
#endif
	return System_getResolutionReturnValue;
}

__int64 System_isDevelop()
{
#ifdef _LOGAPM3
	info(true, "System_isDevelop");
#endif
	return System_isDevelopReturnValue;
}

char __fastcall apmGamepadConfig(__int64 a1)
{
#ifdef _LOGAPM3
	info(true, "apmGamepadConfig");
#endif
	return apmGamepadConfigReturnValue;
}

char __fastcall apmGamepadGetApmInput(DWORD* a1)
{
#ifdef _LOGAPM3
	info(true, "apmGamepadGetApmInput");
#endif
	return apmGamepadGetApmInputReturnValue;
}

//__int128* apmGamepadGetConfig()
__int64* apmGamepadGetConfig()
{
#ifdef _LOGAPM3
	info(true, "apmGamepadGetConfig");
#endif
	static __int64 dumb = 0;
	return &dumb;
}

char __fastcall apmGamepadGetDeviceCaps(__int64 a1)
{
#ifdef _LOGAPM3
	info(true, "apmGamepadGetDeviceCaps");
#endif
	return apmGamepadGetDeviceCapsReturnValue;
}

//errno_t __fastcall apmGamepadGetInfo(__int64 a1)
__int64 __fastcall apmGamepadGetInfo(__int64 a1)
{
#ifdef _LOGAPM3
	info(true, "apmGamepadGetInfo");
#endif
	return apmGamepadGetInfoReturnValue;
}

//char __fastcall apmGamepadGetRawInput(_QWORD* a1)
char __fastcall apmGamepadGetRawInput(__int64* a1)
{
#ifdef _LOGAPM3
	info(true, "apmGamepadGetRawInput");
#endif
	return apmGamepadGetRawInputReturnValue;
}

__int64 apmGamepadInitConfig()
{
#ifdef _LOGAPM3
	info(true, "apmGamepadInitConfig");
#endif
	return apmGamepadInitConfigReturnValue;
}

char apmGamepadIsDetected()
{
#ifdef _LOGAPM3
	info(true, "apmGamepadIsDetected");
#endif
	return apmGamepadIsDetectedReturnValue;
}

__int64 apmGamepadIsXInputDevice()
{
#ifdef _LOGAPM3
	info(true, "apmGamepadIsXInputDevice");
#endif
	return apmGamepadIsXInputDeviceReturnValue;
}

int apmGamepadUpdate()
{
#ifdef _LOGAPM3
	info(true, "apmGamepadUpdate");
#endif
	return apmGamepadUpdateReturnValue;
}

static void HookAPM3()
{
	wcscpy(ServerName, L"localhost");
	wcscpy(LinkServerName, L"localhost");
	wcscpy(LocationNickName, L"Flatty");
	wcscpy(LocationName, L"Flatearth");
	wcscpy(RegionName, L"Santaland");
	Credit_isGameCostEnoughReturnValue = 1;

	MH_Initialize();
	MH_CreateHookApi(L"apm.dll", "Aime_acceptConfirm", Aime_acceptConfirm, NULL);
	MH_CreateHookApi(L"apm.dll", "Aime_cancel", Aime_cancel, NULL);
	MH_CreateHookApi(L"apm.dll", "Aime_getAccessCode", Aime_getAccessCode, NULL);
	MH_CreateHookApi(L"apm.dll", "Aime_getAimeId", Aime_getAimeId, NULL);
	MH_CreateHookApi(L"apm.dll", "Aime_getConfirm", Aime_getConfirm, NULL);
	MH_CreateHookApi(L"apm.dll", "Aime_getErrorCategory", Aime_getErrorCategory, NULL);
	MH_CreateHookApi(L"apm.dll", "Aime_hasConfirm", Aime_hasConfirm, NULL);
	MH_CreateHookApi(L"apm.dll", "Aime_hasError", Aime_hasError, NULL);
	MH_CreateHookApi(L"apm.dll", "Aime_hasResult", Aime_hasResult, NULL);
	MH_CreateHookApi(L"apm.dll", "Aime_isBusy", Aime_isBusy, NULL);
	MH_CreateHookApi(L"apm.dll", "Aime_isDBAlive", Aime_isDBAlive, NULL);
	MH_CreateHookApi(L"apm.dll", "Aime_isMobile", Aime_isMobile, NULL);
	MH_CreateHookApi(L"apm.dll", "Aime_isReaderDetected", Aime_isReaderDetected, NULL);
	MH_CreateHookApi(L"apm.dll", "Aime_sendLog", Aime_sendLog, NULL);
	MH_CreateHookApi(L"apm.dll", "Aime_setLedError", Aime_setLedError, NULL);
	MH_CreateHookApi(L"apm.dll", "Aime_setLedSuccess", Aime_setLedSuccess, NULL);
	MH_CreateHookApi(L"apm.dll", "Aime_start", Aime_start, NULL);
	MH_CreateHookApi(L"apm.dll", "AllnetAccounting_beginPlay", AllnetAccounting_beginPlay, NULL);
	MH_CreateHookApi(L"apm.dll", "AllnetAccounting_endPlay", AllnetAccounting_endPlay, NULL);
	MH_CreateHookApi(L"apm.dll", "AllnetAuth_getAbaasGsServerName", AllnetAuth_getAbaasGsServerName, NULL);
	MH_CreateHookApi(L"apm.dll", "AllnetAuth_getAbaasLinkServerName", AllnetAuth_getAbaasLinkServerName, NULL);
	MH_CreateHookApi(L"apm.dll", "AllnetAuth_getCountryCode", AllnetAuth_getCountryCode, NULL);
	MH_CreateHookApi(L"apm.dll", "AllnetAuth_getLocationId", AllnetAuth_getLocationId, NULL);
	MH_CreateHookApi(L"apm.dll", "AllnetAuth_getLocationName", AllnetAuth_getLocationName, NULL);
	MH_CreateHookApi(L"apm.dll", "AllnetAuth_getLocationNickname", AllnetAuth_getLocationNickname, NULL);
	MH_CreateHookApi(L"apm.dll", "AllnetAuth_getLocationNicknamePartCount", AllnetAuth_getLocationNicknamePartCount, NULL);
	MH_CreateHookApi(L"apm.dll", "AllnetAuth_getRegionCode", AllnetAuth_getRegionCode, NULL);
	MH_CreateHookApi(L"apm.dll", "AllnetAuth_getRegionName", AllnetAuth_getRegionName, NULL);
	MH_CreateHookApi(L"apm.dll", "AllnetAuth_getRegionNamePartCount", AllnetAuth_getRegionNamePartCount, NULL);
	MH_CreateHookApi(L"apm.dll", "AllnetAuth_isDevelop", AllnetAuth_isDevelop, NULL);
	MH_CreateHookApi(L"apm.dll", "AllnetAuth_isGood", AllnetAuth_isGood, NULL);
	MH_CreateHookApi(L"apm.dll", "ApmSystemSetting_getAdvertizeSound", ApmSystemSetting_getAdvertizeSound, NULL);
	MH_CreateHookApi(L"apm.dll", "ApmSystemSetting_getClosingTimes", ApmSystemSetting_getClosingTimes, NULL);
	MH_CreateHookApi(L"apm.dll", "ApmSystemSetting_getEmoneySetting", ApmSystemSetting_getEmoneySetting, NULL);
	MH_CreateHookApi(L"apm.dll", "ApmSystemSetting_getFixedTitle", ApmSystemSetting_getFixedTitle, NULL);
	MH_CreateHookApi(L"apm.dll", "ApmSystemSetting_getGamePadSetting", ApmSystemSetting_getGamePadSetting, NULL);
	MH_CreateHookApi(L"apm.dll", "ApmSystemSetting_getMatchingGroup", ApmSystemSetting_getMatchingGroup, NULL);
	MH_CreateHookApi(L"apm.dll", "ApmSystemSetting_getTimeToClosingTime", ApmSystemSetting_getTimeToClosingTime, NULL);
	MH_CreateHookApi(L"apm.dll", "ApmSystemSetting_getUiSetting", ApmSystemSetting_getUiSetting, NULL);
	MH_CreateHookApi(L"apm.dll", "Backup_getRecordStatus", Backup_getRecordStatus, NULL);
	MH_CreateHookApi(L"apm.dll", "Backup_isSetupSucceeded", Backup_isSetupSucceeded, NULL);
	MH_CreateHookApi(L"apm.dll", "Backup_saveRecord", Backup_saveRecord, NULL);
	MH_CreateHookApi(L"apm.dll", "Backup_saveRecordByAddress", Backup_saveRecordByAddress, NULL);
	MH_CreateHookApi(L"apm.dll", "Backup_setupRecords", Backup_setupRecords, NULL);
	MH_CreateHookApi(L"apm.dll", "Core_execute", Core_execute, NULL);
	MH_CreateHookApi(L"apm.dll", "Core_exitGame", Core_exitGame, NULL);
	MH_CreateHookApi(L"apm.dll", "Core_hookException", Core_hookException, NULL);
	MH_CreateHookApi(L"apm.dll", "Core_isExitNeeded", Core_isExitNeeded, NULL);
	MH_CreateHookApi(L"apm.dll", "Core_resetCount", Core_resetCount, NULL);
	MH_CreateHookApi(L"apm.dll", "Core_resetException", Core_resetException, NULL);
	MH_CreateHookApi(L"apm.dll", "Credit_getAddableCoin", Credit_getAddableCoin, NULL);
	MH_CreateHookApi(L"apm.dll", "Credit_getCoinMultipliers", Credit_getCoinMultipliers, NULL);
	MH_CreateHookApi(L"apm.dll", "Credit_getCoinToCredit", Credit_getCoinToCredit, NULL);
	MH_CreateHookApi(L"apm.dll", "Credit_getCredit", Credit_getCredit, NULL);
	MH_CreateHookApi(L"apm.dll", "Credit_getRemain", Credit_getRemain, NULL);
	MH_CreateHookApi(L"apm.dll", "Credit_isFreePlay", Credit_isFreePlay, NULL);
	MH_CreateHookApi(L"apm.dll", "Credit_isGameCostEnough", Credit_isGameCostEnough, NULL);
	MH_CreateHookApi(L"apm.dll", "Credit_isZero", Credit_isZero, NULL);
	MH_CreateHookApi(L"apm.dll", "Credit_payGameCost", Credit_payGameCost, NULL);
	MH_CreateHookApi(L"apm.dll", "Credit_resetCoinInHook", Credit_resetCoinInHook, NULL);
	MH_CreateHookApi(L"apm.dll", "Credit_setCoinInHook", Credit_setCoinInHook, NULL);
	MH_CreateHookApi(L"apm.dll", "Credit_toString", Credit_toString, NULL);
	MH_CreateHookApi(L"apm.dll", "Emoney_close", Emoney_close, NULL);
	MH_CreateHookApi(L"apm.dll", "Emoney_isOpenMainWindow", Emoney_isOpenMainWindow, NULL);
	MH_CreateHookApi(L"apm.dll", "Emoney_launch", Emoney_launch, NULL);
	MH_CreateHookApi(L"apm.dll", "Emoney_setSoundHook", Emoney_setSoundHook, NULL);
	MH_CreateHookApi(L"apm.dll", "Emoney_update", Emoney_update, NULL);
	MH_CreateHookApi(L"apm.dll", "Error_isOccurred", Error_isOccurred, NULL);
	MH_CreateHookApi(L"apm.dll", "Error_setDeviceLost", Error_setDeviceLost, NULL);
	MH_CreateHookApi(L"apm.dll", "Input_getGamepadInfo", Input_getGamepadInfo, NULL);
	MH_CreateHookApi(L"apm.dll", "Input_isFlipNow", Input_isFlipNow, NULL);
	MH_CreateHookApi(L"apm.dll", "Input_isGamepadDetect", Input_isGamepadDetect, NULL);
	MH_CreateHookApi(L"apm.dll", "Input_isOffNow", Input_isOffNow, NULL);
	MH_CreateHookApi(L"apm.dll", "Input_isOn", Input_isOn, NULL);
	MH_CreateHookApi(L"apm.dll", "Input_isOnNow", Input_isOnNow, NULL);
	MH_CreateHookApi(L"apm.dll", "Input_isOpenPewviewWindow", Input_isOpenPewviewWindow, NULL);
	MH_CreateHookApi(L"apm.dll", "Input_setGamepadConfig", Input_setGamepadConfig, NULL);
	MH_CreateHookApi(L"apm.dll", "NetworkProperty_getAddressString", NetworkProperty_getAddressString, NULL);
	MH_CreateHookApi(L"apm.dll", "NetworkProperty_getAddressValue", NetworkProperty_getAddressValue, NULL);
	MH_CreateHookApi(L"apm.dll", "Sequence_beginPlay", Sequence_beginPlay, NULL);
	MH_CreateHookApi(L"apm.dll", "Sequence_clearBackup", Sequence_clearBackup, NULL);
	MH_CreateHookApi(L"apm.dll", "Sequence_continuePlay", Sequence_continuePlay, NULL);
	MH_CreateHookApi(L"apm.dll", "Sequence_endPlay", Sequence_endPlay, NULL);
	MH_CreateHookApi(L"apm.dll", "Sequence_getBookkeeping", Sequence_getBookkeeping, NULL);
	MH_CreateHookApi(L"apm.dll", "Sequence_getPlayingAimeId", Sequence_getPlayingAimeId, NULL);
	MH_CreateHookApi(L"apm.dll", "Sequence_isAccountingPlaying", Sequence_isAccountingPlaying, NULL);
	MH_CreateHookApi(L"apm.dll", "Sequence_isPlaying", Sequence_isPlaying, NULL);
	MH_CreateHookApi(L"apm.dll", "Sequence_isTest", Sequence_isTest, NULL);
	MH_CreateHookApi(L"apm.dll", "System_getAppRootPath", System_getAppRootPath, NULL);
	MH_CreateHookApi(L"apm.dll", "System_getBoardId", System_getBoardId, NULL);
	MH_CreateHookApi(L"apm.dll", "System_getGameId", System_getGameId, NULL);
	MH_CreateHookApi(L"apm.dll", "System_getGameVersion", System_getGameVersion, NULL);
	MH_CreateHookApi(L"apm.dll", "System_getKeychipId", System_getKeychipId, NULL);
	MH_CreateHookApi(L"apm.dll", "System_getRegionCode", System_getRegionCode, NULL);
	MH_CreateHookApi(L"apm.dll", "System_getResolution", System_getResolution, NULL);
	MH_CreateHookApi(L"apm.dll", "System_isDevelop", System_isDevelop, NULL);
	MH_CreateHookApi(L"apm.dll", "apmGamepadConfig", apmGamepadConfig, NULL);
	MH_CreateHookApi(L"apm.dll", "apmGamepadGetApmInput", apmGamepadGetApmInput, NULL);
	MH_CreateHookApi(L"apm.dll", "apmGamepadGetConfig", apmGamepadGetConfig, NULL);
	MH_CreateHookApi(L"apm.dll", "apmGamepadGetDeviceCaps", apmGamepadGetDeviceCaps, NULL);
	MH_CreateHookApi(L"apm.dll", "apmGamepadGetInfo", apmGamepadGetInfo, NULL);
	MH_CreateHookApi(L"apm.dll", "apmGamepadGetRawInput", apmGamepadGetRawInput, NULL);
	MH_CreateHookApi(L"apm.dll", "apmGamepadInitConfig", apmGamepadInitConfig, NULL);
	MH_CreateHookApi(L"apm.dll", "apmGamepadIsDetected", apmGamepadIsDetected, NULL);
	MH_CreateHookApi(L"apm.dll", "apmGamepadIsXInputDevice", apmGamepadIsXInputDevice, NULL);
	MH_CreateHookApi(L"apm.dll", "apmGamepadUpdate", apmGamepadUpdate, NULL);
	MH_EnableHook(MH_ALL_HOOKS);

	memset(g_APM3IOValues, 0, 256);

	CreateThread(nullptr, 0, [](LPVOID) -> DWORD
		{
			HANDLE hPipe = CreateFileA("\\\\.\\pipe\\TeknoParrotPipe", GENERIC_READ, 0, nullptr, OPEN_EXISTING, 0, NULL);

			if (hPipe != INVALID_HANDLE_VALUE)
			{
				DWORD read;

				while (true)
				{
					ReadFile(hPipe, g_APM3IOValues, sizeof(g_APM3IOValues), &read, nullptr);
				}
			}

			return 0;
		}, nullptr, 0, nullptr);
}

static InitFunction initFuncTapping([]()
{
	HookAPM3();
	wcscpy(APM3GameId, L"SDFJ");

	__int64 mainModuleBase = (__int64)GetModuleHandle(0);

}, GameID::TappingSkillTest);

void __fastcall printPengo(const char* format, ...)
{
	__try
	{
		va_list args;
		char buffer[1024];

		va_start(args, format);
		int len = _vsnprintf(buffer, sizeof(buffer), format, args);
		va_end(args);

		buffer[len] = '\n';
		buffer[len + 1] = '\0';

		OutputDebugStringA(buffer);
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{

	}
}

static InitFunction initFunc([]()
{
	HookAPM3();
	wcscpy(APM3GameId, L"SDFH");

	__int64 mainModuleBase = (__int64)GetModuleHandle(0);

	MH_Initialize();
	//MH_CreateHook((void*)(mainModuleBase + 0x1944B0), (void *)printPengo, NULL);
	MH_EnableHook(MH_ALL_HOOKS);

	/// PATTERNS BELOW
	// Skip joysticks
	injector::MakeRET(mainModuleBase + 0x15C5B0);
	// Skip keyboard
	injector::MakeRET(mainModuleBase + 0x15CBA0);

}, GameID::Pengoe5);

static InitFunction initFuncPengoe511([]()
{
	HookAPM3();
	wcscpy(APM3GameId, L"SDFH");

	__int64 mainModuleBase = (__int64)GetModuleHandle(0);

	// Skip joysticks
	injector::MakeRET(mainModuleBase + 0x16A7C0); // CC 48 89 5C 24 10 48 89 6C 24 18 48 89 74 24 20 57 48 83 EC 40
	// Skip keyboard
	injector::MakeRET(mainModuleBase + 0x16ADB0); // 48 89 5C 24 08 48 89 6C 24 10 48 89 74 24 18 57 41 54 41 55 41 56 41 57 48 81 EC A0 00 00 00

}, GameID::Pengoe511);

static InitFunction initTestFunc([]()
{
	HookAPM3();
	wcscpy(APM3GameId, L"SDFH");
	__int64 mainModuleBase = (__int64)GetModuleHandle(0);

	// Skip joysticks
	injector::MakeRET(mainModuleBase + 0x158820);
	// Skip keyboard
	injector::MakeRET(mainModuleBase + 0x158E10);

	Sequence_isTestReturnValue = 1;

}, GameID::Pengoe5_Test);

static InitFunction initVF5Func([]()
{
	HookAPM3();
	wcscpy(APM3GameId, L"SDHF");
	__int64 mainModuleBase = (__int64)GetModuleHandle(0);

	BackupSaveFileName = ".\\vf5fs\\save";

}, GameID::VF5Esports);

static InitFunction initVF5TestFunc([]()
{
	HookAPM3();
	wcscpy(APM3GameId, L"SDHF");
	__int64 mainModuleBase = (__int64)GetModuleHandle(0);

	if (strstr(GetCommandLineA(), "-tptest") != NULL)
		Sequence_isTestReturnValue = 1;

}, GameID::VF5EsportsTest);

static InitFunction initGoonyaFunc([]()
{
	HookAPM3();
	wcscpy(APM3GameId, L"SDGX");
	__int64 mainModuleBase = (__int64)GetModuleHandle(0);

}, GameID::GoonyaFighter);

static InitFunction initPuyoFunc([]()
{
	HookAPM3();
	wcscpy(APM3GameId, L"SDFF");
	__int64 mainModuleBase = (__int64)GetModuleHandle(0);

}, GameID::PuyoPuyoEsports);
#endif