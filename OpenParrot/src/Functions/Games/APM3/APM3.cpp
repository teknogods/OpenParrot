#include <Utility/InitFunction.h>
#include <StdInc.h>
#include <thread>
#include "Functions\Global.h"
#include "APM3.h"
#include "Aime.h"
#include "Backup.h"
#include "Auth.h"
#include "Sequence.h"
#include <Utility/Hooking.Patterns.h>

// Define default return values here
static DWORD_PTR System_getResolutionReturnValue = 0;
static bool ApmSystemSetting_getAdvertizeSoundReturnValue = false;
static bool ApmSystemSetting_getClosingTimesReturnValue = false;
static char ApmSystemSetting_getEmoneySettingReturnValue = 0;
static bool ApmSystemSetting_getFixedTitleReturnValue = false;
static char ApmSystemSetting_getGamePadSettingReturnValue = 0;
static bool ApmSystemSetting_getMatchingGroupReturnValue = true;
static char ApmSystemSetting_getTimeToClosingTimeReturnValue = 0;
static char ApmSystemSetting_getUiSettingReturnValue = 0;
static bool Core_exitGameReturnValue = false;
static DWORD_PTR Core_hookExceptionReturnValue = 0;
static bool Core_isExitNeededReturnValue = false;
static DWORD_PTR Core_resetExceptionReturnValue = 0;
static DWORD_PTR Credit_getAddableCoinReturnValue = 0;
static DWORD_PTR Credit_getCoinMultipliersReturnValue = 0;
static DWORD_PTR Credit_getCoinToCreditReturnValue = 0;
static DWORD_PTR Credit_getCreditReturnValue = 0;
static DWORD_PTR Credit_getRemainReturnValue = 0;
static DWORD_PTR Credit_isFreePlayReturnValue = 1;
static DWORD_PTR Credit_isGameCostEnoughReturnValue = 0;
static DWORD_PTR Credit_isZeroReturnValue = 0;
static DWORD_PTR Credit_payGameCostReturnValue = 0;
static DWORD_PTR Credit_resetCoinInHookReturnValue = 0;
static DWORD_PTR Credit_setCoinInHookReturnValue = 0;
static DWORD_PTR Emoney_isOpenMainWindowReturnValue = 0;
static char Emoney_launchReturnValue = 0;
static DWORD_PTR Error_isOccurredReturnValue = 0;
static DWORD_PTR Error_setDeviceLostReturnValue = 0;
static char Input_isFlipNowReturnValue = 0;
static DWORD_PTR Input_isGamepadDetectReturnValue = 0;
static char Input_isOffNowReturnValue = 0;
static char Input_isOnNowReturnValue = 0;
static DWORD_PTR Input_isOpenPewviewWindowReturnValue = 0;
static char Input_setGamepadConfigReturnValue = 0;
static char* NetworkProperty_getAddressStringReturnValue = "192.168.0.1";
static DWORD_PTR NetworkProperty_getAddressValueReturnValue = 0;
static DWORD_PTR System_getAppRootPathReturnValue = 0;
static DWORD_PTR System_getRegionCodeReturnValue = 0;
static DWORD_PTR System_isDevelopReturnValue = 0;
static char apmGamepadConfigReturnValue = 0;
static char apmGamepadGetApmInputReturnValue = 0;
static char apmGamepadGetDeviceCapsReturnValue = 0;
static DWORD_PTR apmGamepadGetInfoReturnValue = 0;
static char apmGamepadGetRawInputReturnValue = 0;
static DWORD_PTR apmGamepadInitConfigReturnValue = 0;
static char apmGamepadIsDetectedReturnValue = 0;
static DWORD_PTR apmGamepadIsXInputDeviceReturnValue = 0;
static int apmGamepadUpdateReturnValue = 0;

static uint8_t g_APM3IOValues[256];

bool CALLPLEB ApmSystemSetting_getAdvertizeSound(bool* enable)
{
#ifdef _LOGAPM3
	info(true, "ApmSystemSetting_getAdvertizeSound");
#endif
	*enable = true;
	return true;
}

bool CALLPLEB ApmSystemSetting_getClosingTimes(DWORD_PTR* closingTimes)
{
#ifdef _LOGAPM3
	info(true, "ApmSystemSetting_getClosingTimes");
#endif
	*closingTimes = 0;
	return ApmSystemSetting_getClosingTimesReturnValue;
}

char CALLPLEB ApmSystemSetting_getEmoneySetting(DWORD_PTR a1)
{
#ifdef _LOGAPM3
	info(true, "ApmSystemSetting_getEmoneySetting");
#endif
	return ApmSystemSetting_getEmoneySettingReturnValue;
}

bool CALLPLEB ApmSystemSetting_getFixedTitle(bool* enable)
{
#ifdef _LOGAPM3
	info(true, "ApmSystemSetting_getFixedTitle");
#endif
	*enable = false;
	return ApmSystemSetting_getFixedTitleReturnValue;
}

char CALLPLEB ApmSystemSetting_getGamePadSetting(WORD* a1)
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

bool CALLPLEB ApmSystemSetting_getMatchingGroup(MatchingGroup* matchingGroup)
{
#ifdef _LOGAPM3
	info(true, "ApmSystemSetting_getMatchingGroup");
#endif
	MatchingGroup _matchingGroup = { 'A', 1 }; // TODO: Make this configurable, improve this

	*matchingGroup->alphabet = *_matchingGroup.alphabet;
	*matchingGroup->number = *_matchingGroup.number;

	return ApmSystemSetting_getMatchingGroupReturnValue;
}

bool CALLPLEB ApmSystemSetting_getTimeToClosingTime(unsigned int* seconds)
{
#ifdef _LOGAPM3
	info(true, "ApmSystemSetting_getTimeToClosingTime");
#endif
	*seconds = 10000000;
	return ApmSystemSetting_getTimeToClosingTimeReturnValue;
}

char CALLPLEB ApmSystemSetting_getUiSetting(DWORD_PTR a1)
{
#ifdef _LOGAPM3
	info(true, "ApmSystemSetting_getUiSetting");
#endif
	return ApmSystemSetting_getUiSettingReturnValue;
}

void CALLPLEB Core_execute()
{
#ifdef _LOGAPM3
	info(true, "Core_execute");
#endif
}

bool CALLPLEB Core_exitGame(bool isTest)
{
#ifdef _LOGAPM3
	info(true, "Core_exitGame");
#endif
	return Core_exitGameReturnValue;
}

DWORD_PTR CALLPLEB Core_hookException()
{
#ifdef _LOGAPM3
	info(true, "Core_hookException");
#endif
	return Core_hookExceptionReturnValue;
}

bool CALLPLEB Core_isExitNeeded()
{
#ifdef _LOGAPM3
	info(true, "Core_isExitNeeded");
#endif
	return Core_isExitNeededReturnValue;
}

void CALLPLEB Core_resetCount()
{
#ifdef _LOGAPM3
	info(true, "Core_resetCount");
#endif
}

DWORD_PTR CALLPLEB Core_resetException()
{
#ifdef _LOGAPM3
	info(true, "Core_resetException");
#endif
	return Core_resetExceptionReturnValue;
}

DWORD_PTR CALLPLEB Credit_getAddableCoin()
{
#ifdef _LOGAPM3
	info(true, "Credit_getAddableCoin");
#endif
	return Credit_getAddableCoinReturnValue;
}

DWORD_PTR CALLPLEB Credit_getCoinMultipliers()
{
#ifdef _LOGAPM3
	info(true, "Credit_getCoinMultipliers");
#endif
	return Credit_getCoinMultipliersReturnValue;
}

DWORD_PTR CALLPLEB Credit_getCoinToCredit()
{
#ifdef _LOGAPM3
	info(true, "Credit_getCoinToCredit");
#endif
	return Credit_getCoinToCreditReturnValue;
}

DWORD_PTR CALLPLEB Credit_getCredit()
{
#ifdef _LOGAPM3
	info(true, "Credit_getCredit");
#endif
	return Credit_getCreditReturnValue;
}

DWORD_PTR CALLPLEB Credit_getRemain()
{
#ifdef _LOGAPM3
	info(true, "Credit_getRemain");
#endif
	return Credit_getRemainReturnValue;
}

DWORD_PTR CALLPLEB Credit_isFreePlay()
{
#ifdef _LOGAPM3
	info(true, "Credit_isFreePlay");
#endif
	return Credit_isFreePlayReturnValue;
}

DWORD_PTR CALLPLEB Credit_isGameCostEnough(DWORD_PTR a1)
{
#ifdef _LOGAPM3
	info(true, "Credit_isGameCostEnough");
#endif
	return Credit_isGameCostEnoughReturnValue; // 1
}

DWORD_PTR Credit_isZero()
{
#ifdef _LOGAPM3
	info(true, "Credit_isZero");
#endif
	return Credit_isZeroReturnValue;
}

DWORD_PTR CALLPLEB Credit_payGameCost(DWORD_PTR a1)
{
#ifdef _LOGAPM3
	info(true, "Credit_payGameCost");
#endif
	return Credit_payGameCostReturnValue;
}

DWORD_PTR CALLPLEB Credit_resetCoinInHook()
{
#ifdef _LOGAPM3
	info(true, "Credit_resetCoinInHook");
#endif

	return Credit_resetCoinInHookReturnValue;
}

DWORD_PTR CALLPLEB Credit_setCoinInHook(int a1)
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

void CALLPLEB Emoney_close()
{
#ifdef _LOGAPM3
	info(true, "Emoney_close");
#endif
}

DWORD_PTR CALLPLEB Emoney_isOpenMainWindow()
{
#ifdef _LOGAPM3
	info(true, "Emoney_isOpenMainWindow");
#endif
	return Emoney_isOpenMainWindowReturnValue;
}

char CALLPLEB Emoney_launch()
{
#ifdef _LOGAPM3
	info(true, "Emoney_launch");
#endif
	return Emoney_launchReturnValue;
}

void CALLPLEB Emoney_setSoundHook(DWORD_PTR a1)
{
#ifdef _LOGAPM3
	info(true, "Emoney_setSoundHook");
#endif
}

void CALLPLEB Emoney_update(BYTE* a1)
{
#ifdef _LOGAPM3
	info(true, "Emoney_update");
#endif
}

DWORD_PTR CALLPLEB Error_isOccurred()
{
#ifdef _LOGAPM3
	info(true, "Error_isOccurred");
#endif
	return Error_isOccurredReturnValue;
}

DWORD_PTR CALLPLEB Error_setDeviceLost()
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

static GamepadInfo* _gamePad;

GamepadInfo* CALLPLEB Input_getGamepadInfo()
{
#ifdef _LOGAPM3
	info(true, "Input_getGamepadInfo");
#endif

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

char CALLPLEB Input_isFlipNow(unsigned int a1)
{
#ifdef _LOGAPM3
	info(true, "Input_isFlipNow");
#endif
	return Input_isFlipNowReturnValue;
}

DWORD_PTR CALLPLEB Input_isGamepadDetect()
{
#ifdef _LOGAPM3
	info(true, "Input_isGamepadDetect");
#endif
	return Input_isGamepadDetectReturnValue;
}

char CALLPLEB Input_isOffNow(unsigned int a1)
{
#ifdef _LOGAPM3
	info(true, "Input_isOffNow");
#endif
	return Input_isOffNowReturnValue;
}

int CALLPLEB Input_isOn(unsigned int buttonId)
{
#ifdef _LOGAPM3
	info(true, "Input_isOn id: %u", buttonId);
#endif
	return g_APM3IOValues[buttonId];
}

char CALLPLEB Input_isOnNow(unsigned int buttonId)
{
#ifdef _LOGAPM3
	info(true, "Input_isOnNow id: %u", buttonId);
#endif
	return g_APM3IOValues[buttonId];
}

DWORD_PTR CALLPLEB Input_isOpenPewviewWindow()
{
#ifdef _LOGAPM3
	info(true, "Input_isOpenPewviewWindow");
#endif
	return Input_isOpenPewviewWindowReturnValue;
}

GamepadInfo* CALLPLEB Input_setGamepadConfig(GamepadInfo *gamepadInfo)
{
#ifdef _LOGAPM3
	info("Input_setGamepadConfig");
#endif

	return 0;
}

char* CALLPLEB NetworkProperty_getAddressString()
{
#ifdef _DEBUG
	info("NetworkProperty_getAddressString");
#endif
	return NetworkProperty_getAddressStringReturnValue;
}

DWORD_PTR CALLPLEB NetworkProperty_getAddressValue()
{
#ifdef _DEBUG
	info("NetworkProperty_getAddressValue");
#endif
	return NetworkProperty_getAddressValueReturnValue;
}

static wchar_t* gameDir = L".\\";

wchar_t* CALLPLEB System_getAppRootPath()
{
#ifdef _LOGAPM3
	info("System_getAppRootPath");
#endif
	return gameDir;
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
	static SerialID* _serialId;
	if (_serialId == nullptr)
	{
		_serialId = (SerialID*)malloc(sizeof(SerialID));
		memcpy(_serialId->ID.Value, L"A69E-01A88888888", 32);
		memcpy(_serialId->ShortId.Value, L"A69E01A8888", 24);
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
	static GameVersion* _gameVersion;
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
		memcpy(_serialId->ID.Value, L"A69E-01A88888888", 32);
		memcpy(_serialId->ShortId.Value, L"A69E01A8888", 24);
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

DWORD_PTR resolution = 0;

DWORD_PTR *System_getResolution()
{
#ifdef _LOGAPM3
	info(true, "System_getResolution");
#endif
	return &resolution;
}

DWORD_PTR System_isDevelop()
{
#ifdef _LOGAPM3
	info(true, "System_isDevelop");
#endif
	return System_isDevelopReturnValue;
}

char CALLPLEB apmGamepadConfig(DWORD_PTR a1)
{
#ifdef _LOGAPM3
	info(true, "apmGamepadConfig");
#endif
	return apmGamepadConfigReturnValue;
}

char CALLPLEB apmGamepadGetApmInput(DWORD* a1)
{
#ifdef _LOGAPM3
	info(true, "apmGamepadGetApmInput");
#endif
	return apmGamepadGetApmInputReturnValue;
}

//__int128* apmGamepadGetConfig()
DWORD_PTR* apmGamepadGetConfig()
{
#ifdef _LOGAPM3
	info(true, "apmGamepadGetConfig");
#endif
	static DWORD_PTR dumb = 0;
	return &dumb;
}

char CALLPLEB apmGamepadGetDeviceCaps(DWORD_PTR a1)
{
#ifdef _LOGAPM3
	info(true, "apmGamepadGetDeviceCaps");
#endif
	return apmGamepadGetDeviceCapsReturnValue;
}

//errno_t CALLPLEB apmGamepadGetInfo(__int64 a1)
DWORD_PTR CALLPLEB apmGamepadGetInfo(DWORD_PTR a1)
{
#ifdef _LOGAPM3
	info(true, "apmGamepadGetInfo");
#endif
	return apmGamepadGetInfoReturnValue;
}

//char CALLPLEB apmGamepadGetRawInput(_QWORD* a1)
char CALLPLEB apmGamepadGetRawInput(DWORD_PTR* a1)
{
#ifdef _LOGAPM3
	info(true, "apmGamepadGetRawInput");
#endif
	return apmGamepadGetRawInputReturnValue;
}

DWORD_PTR CALLPLEB apmGamepadInitConfig()
{
#ifdef _LOGAPM3
	info(true, "apmGamepadInitConfig");
#endif
	return apmGamepadInitConfigReturnValue;
}

char CALLPLEB apmGamepadIsDetected()
{
#ifdef _LOGAPM3
	info(true, "apmGamepadIsDetected");
#endif
	return apmGamepadIsDetectedReturnValue;
}

DWORD_PTR CALLPLEB apmGamepadIsXInputDevice()
{
#ifdef _LOGAPM3
	info(true, "apmGamepadIsXInputDevice");
#endif
	return apmGamepadIsXInputDeviceReturnValue;
}

int CALLPLEB apmGamepadUpdate()
{
#ifdef _LOGAPM3
	info(true, "apmGamepadUpdate");
#endif
	return apmGamepadUpdateReturnValue;
}

static void HookAPM3(wchar_t* id)
{
	// TODO: read from config?
    wcscpy(ServerName, L"localhost");
    wcscpy(LinkServerName, L"localhost");
	wcscpy(LocationNickName, L"Flatty");
	wcscpy(LocationName, L"Flatearth");
	wcscpy(RegionName, L"Santaland");

	wcscpy(APM3GameId, id);

	Credit_isGameCostEnoughReturnValue = 1;

	MH_Initialize();
#ifdef _M_AMD64
	LPCWSTR dllName = L"apm.dll";
	auto apm = LoadLibrary(dllName);
	if (apm == nullptr)
	{
		MessageBoxA(0, "Cannot load apm.dll!", "Error", 0);
		ExitProcess(0);
	}

	MH_CreateHookApi(dllName, "Aime_acceptConfirm", Aime_acceptConfirm, NULL);
	MH_CreateHookApi(dllName, "Aime_cancel", Aime_cancel, NULL);
	MH_CreateHookApi(dllName, "Aime_getAccessCode", Aime_getAccessCode, NULL);
	MH_CreateHookApi(dllName, "Aime_getAimeId", Aime_getAimeId, NULL);
	MH_CreateHookApi(dllName, "Aime_getConfirm", Aime_getConfirm, NULL);
	MH_CreateHookApi(dllName, "Aime_getErrorCategory", Aime_getErrorCategory, NULL);
	MH_CreateHookApi(dllName, "Aime_hasConfirm", Aime_hasConfirm, NULL);
	MH_CreateHookApi(dllName, "Aime_hasError", Aime_hasError, NULL);
	MH_CreateHookApi(dllName, "Aime_hasResult", Aime_hasResult, NULL);
	MH_CreateHookApi(dllName, "Aime_isBusy", Aime_isBusy, NULL);
	MH_CreateHookApi(dllName, "Aime_isDBAlive", Aime_isDBAlive, NULL);
	MH_CreateHookApi(dllName, "Aime_isMobile", Aime_isMobile, NULL);
	MH_CreateHookApi(dllName, "Aime_isReaderDetected", Aime_isReaderDetected, NULL);
	MH_CreateHookApi(dllName, "Aime_sendLog", Aime_sendLog, NULL);
	MH_CreateHookApi(dllName, "Aime_setLedError", Aime_setLedError, NULL);
	MH_CreateHookApi(dllName, "Aime_setLedSuccess", Aime_setLedSuccess, NULL);
	MH_CreateHookApi(dllName, "Aime_start", Aime_start, NULL);
	MH_CreateHookApi(dllName, "AllnetAccounting_beginPlay", AllnetAccounting_beginPlay, NULL);
	MH_CreateHookApi(dllName, "AllnetAccounting_endPlay", AllnetAccounting_endPlay, NULL);
	MH_CreateHookApi(dllName, "AllnetAuth_getAbaasGsServerName", AllnetAuth_getAbaasGsServerName, NULL);
	MH_CreateHookApi(dllName, "AllnetAuth_getAbaasLinkServerName", AllnetAuth_getAbaasLinkServerName, NULL);
	MH_CreateHookApi(dllName, "AllnetAuth_getCountryCode", AllnetAuth_getCountryCode, NULL);
	MH_CreateHookApi(dllName, "AllnetAuth_getLocationId", AllnetAuth_getLocationId, NULL);
	MH_CreateHookApi(dllName, "AllnetAuth_getLocationName", AllnetAuth_getLocationName, NULL);
	MH_CreateHookApi(dllName, "AllnetAuth_getLocationNickname", AllnetAuth_getLocationNickname, NULL);
	MH_CreateHookApi(dllName, "AllnetAuth_getLocationNicknamePartCount", AllnetAuth_getLocationNicknamePartCount, NULL);
	MH_CreateHookApi(dllName, "AllnetAuth_getRegionCode", AllnetAuth_getRegionCode, NULL);
	MH_CreateHookApi(dllName, "AllnetAuth_getRegionName", AllnetAuth_getRegionName, NULL);
	MH_CreateHookApi(dllName, "AllnetAuth_getRegionNamePartCount", AllnetAuth_getRegionNamePartCount, NULL);
	MH_CreateHookApi(dllName, "AllnetAuth_isDevelop", AllnetAuth_isDevelop, NULL);
	MH_CreateHookApi(dllName, "AllnetAuth_isGood", AllnetAuth_isGood, NULL);
	MH_CreateHookApi(dllName, "ApmSystemSetting_getAdvertizeSound", ApmSystemSetting_getAdvertizeSound, NULL);
	MH_CreateHookApi(dllName, "ApmSystemSetting_getClosingTimes", ApmSystemSetting_getClosingTimes, NULL);
	MH_CreateHookApi(dllName, "ApmSystemSetting_getEmoneySetting", ApmSystemSetting_getEmoneySetting, NULL);
	MH_CreateHookApi(dllName, "ApmSystemSetting_getFixedTitle", ApmSystemSetting_getFixedTitle, NULL);
	MH_CreateHookApi(dllName, "ApmSystemSetting_getGamePadSetting", ApmSystemSetting_getGamePadSetting, NULL);
	MH_CreateHookApi(dllName, "ApmSystemSetting_getMatchingGroup", ApmSystemSetting_getMatchingGroup, NULL);
	MH_CreateHookApi(dllName, "ApmSystemSetting_getTimeToClosingTime", ApmSystemSetting_getTimeToClosingTime, NULL);
	MH_CreateHookApi(dllName, "ApmSystemSetting_getUiSetting", ApmSystemSetting_getUiSetting, NULL);
	MH_CreateHookApi(dllName, "Backup_getRecordStatus", Backup_getRecordStatus, NULL);
	MH_CreateHookApi(dllName, "Backup_isSetupSucceeded", Backup_isSetupSucceeded, NULL);
	MH_CreateHookApi(dllName, "Backup_saveRecord", Backup_saveRecord, NULL);
	MH_CreateHookApi(dllName, "Backup_saveRecordByAddress", Backup_saveRecordByAddress, NULL);
	MH_CreateHookApi(dllName, "Backup_setupRecords", Backup_setupRecords, NULL);
	MH_CreateHookApi(dllName, "Core_execute", Core_execute, NULL);
	MH_CreateHookApi(dllName, "Core_exitGame", Core_exitGame, NULL);
	MH_CreateHookApi(dllName, "Core_hookException", Core_hookException, NULL);
	MH_CreateHookApi(dllName, "Core_isExitNeeded", Core_isExitNeeded, NULL);
	MH_CreateHookApi(dllName, "Core_resetCount", Core_resetCount, NULL);
	MH_CreateHookApi(dllName, "Core_resetException", Core_resetException, NULL);
	MH_CreateHookApi(dllName, "Credit_getAddableCoin", Credit_getAddableCoin, NULL);
	MH_CreateHookApi(dllName, "Credit_getCoinMultipliers", Credit_getCoinMultipliers, NULL);
	MH_CreateHookApi(dllName, "Credit_getCoinToCredit", Credit_getCoinToCredit, NULL);
	MH_CreateHookApi(dllName, "Credit_getCredit", Credit_getCredit, NULL);
	MH_CreateHookApi(dllName, "Credit_getRemain", Credit_getRemain, NULL);
	MH_CreateHookApi(dllName, "Credit_isFreePlay", Credit_isFreePlay, NULL);
	MH_CreateHookApi(dllName, "Credit_isGameCostEnough", Credit_isGameCostEnough, NULL);
	MH_CreateHookApi(dllName, "Credit_isZero", Credit_isZero, NULL);
	MH_CreateHookApi(dllName, "Credit_payGameCost", Credit_payGameCost, NULL);
	MH_CreateHookApi(dllName, "Credit_resetCoinInHook", Credit_resetCoinInHook, NULL);
	MH_CreateHookApi(dllName, "Credit_setCoinInHook", Credit_setCoinInHook, NULL);
	MH_CreateHookApi(dllName, "Credit_toString", Credit_toString, NULL);
	MH_CreateHookApi(dllName, "Emoney_close", Emoney_close, NULL);
	MH_CreateHookApi(dllName, "Emoney_isOpenMainWindow", Emoney_isOpenMainWindow, NULL);
	MH_CreateHookApi(dllName, "Emoney_launch", Emoney_launch, NULL);
	MH_CreateHookApi(dllName, "Emoney_setSoundHook", Emoney_setSoundHook, NULL);
	MH_CreateHookApi(dllName, "Emoney_update", Emoney_update, NULL);
	MH_CreateHookApi(dllName, "Error_isOccurred", Error_isOccurred, NULL);
	MH_CreateHookApi(dllName, "Error_setDeviceLost", Error_setDeviceLost, NULL);
	MH_CreateHookApi(dllName, "Input_getGamepadInfo", Input_getGamepadInfo, NULL);
	MH_CreateHookApi(dllName, "Input_isFlipNow", Input_isFlipNow, NULL);
	MH_CreateHookApi(dllName, "Input_isGamepadDetect", Input_isGamepadDetect, NULL);
	MH_CreateHookApi(dllName, "Input_isOffNow", Input_isOffNow, NULL);
	MH_CreateHookApi(dllName, "Input_isOn", Input_isOn, NULL);
	MH_CreateHookApi(dllName, "Input_isOnNow", Input_isOnNow, NULL);
	MH_CreateHookApi(dllName, "Input_isOpenPewviewWindow", Input_isOpenPewviewWindow, NULL);
	MH_CreateHookApi(dllName, "Input_setGamepadConfig", Input_setGamepadConfig, NULL);
	MH_CreateHookApi(dllName, "NetworkProperty_getAddressString", NetworkProperty_getAddressString, NULL);
	MH_CreateHookApi(dllName, "NetworkProperty_getAddressValue", NetworkProperty_getAddressValue, NULL);
	MH_CreateHookApi(dllName, "Sequence_beginPlay", Sequence_beginPlay, NULL);
	MH_CreateHookApi(dllName, "Sequence_clearBackup", Sequence_clearBackup, NULL);
	MH_CreateHookApi(dllName, "Sequence_continuePlay", Sequence_continuePlay, NULL);
	MH_CreateHookApi(dllName, "Sequence_endPlay", Sequence_endPlay, NULL);
	MH_CreateHookApi(dllName, "Sequence_getBookkeeping", Sequence_getBookkeeping, NULL);
	MH_CreateHookApi(dllName, "Sequence_getPlayingAimeId", Sequence_getPlayingAimeId, NULL);
	MH_CreateHookApi(dllName, "Sequence_isAccountingPlaying", Sequence_isAccountingPlaying, NULL);
	MH_CreateHookApi(dllName, "Sequence_isPlaying", Sequence_isPlaying, NULL);
	MH_CreateHookApi(dllName, "Sequence_isTest", Sequence_isTest, NULL);
	MH_CreateHookApi(dllName, "System_getAppRootPath", System_getAppRootPath, NULL);
	MH_CreateHookApi(dllName, "System_getBoardId", System_getBoardId, NULL);
	MH_CreateHookApi(dllName, "System_getGameId", System_getGameId, NULL);
	MH_CreateHookApi(dllName, "System_getGameVersion", System_getGameVersion, NULL);
	MH_CreateHookApi(dllName, "System_getKeychipId", System_getKeychipId, NULL);
	MH_CreateHookApi(dllName, "System_getRegionCode", System_getRegionCode, NULL);
	MH_CreateHookApi(dllName, "System_getResolution", System_getResolution, NULL);
	MH_CreateHookApi(dllName, "System_isDevelop", System_isDevelop, NULL);
	MH_CreateHookApi(dllName, "apmGamepadConfig", apmGamepadConfig, NULL);
	MH_CreateHookApi(dllName, "apmGamepadGetApmInput", apmGamepadGetApmInput, NULL);
	MH_CreateHookApi(dllName, "apmGamepadGetConfig", apmGamepadGetConfig, NULL);
	MH_CreateHookApi(dllName, "apmGamepadGetDeviceCaps", apmGamepadGetDeviceCaps, NULL);
	MH_CreateHookApi(dllName, "apmGamepadGetInfo", apmGamepadGetInfo, NULL);
	MH_CreateHookApi(dllName, "apmGamepadGetRawInput", apmGamepadGetRawInput, NULL);
	MH_CreateHookApi(dllName, "apmGamepadInitConfig", apmGamepadInitConfig, NULL);
	MH_CreateHookApi(dllName, "apmGamepadIsDetected", apmGamepadIsDetected, NULL);
	MH_CreateHookApi(dllName, "apmGamepadIsXInputDevice", apmGamepadIsXInputDevice, NULL);
	MH_CreateHookApi(dllName, "apmGamepadUpdate", apmGamepadUpdate, NULL);
#else
	LPCWSTR dllName = L"apm_x86.dll";
	auto apm = LoadLibrary(dllName);
	if (apm == nullptr)
	{
		MessageBoxA(0, "Cannot load apm_x86.dll!", "Error", 0);
		ExitProcess(0);
	}

	MH_CreateHookApi(dllName, "_Aime_acceptConfirm@0", Aime_acceptConfirm, NULL);
	MH_CreateHookApi(dllName, "_Aime_cancel@0", Aime_cancel, NULL);
	MH_CreateHookApi(dllName, "_Aime_getAccessCode@4", Aime_getAccessCode, NULL);
	MH_CreateHookApi(dllName, "_Aime_getAimeId@4", Aime_getAimeId, NULL);
	MH_CreateHookApi(dllName, "_Aime_getConfirm@0", Aime_getConfirm, NULL);
	MH_CreateHookApi(dllName, "_Aime_getErrorCategory@0", Aime_getErrorCategory, NULL);
	MH_CreateHookApi(dllName, "_Aime_hasConfirm@0", Aime_hasConfirm, NULL);
	MH_CreateHookApi(dllName, "_Aime_hasError@0", Aime_hasError, NULL);
	MH_CreateHookApi(dllName, "_Aime_hasResult@0", Aime_hasResult, NULL);
	MH_CreateHookApi(dllName, "_Aime_isBusy@0", Aime_isBusy, NULL);
	MH_CreateHookApi(dllName, "_Aime_isDBAlive@0", Aime_isDBAlive, NULL);
	MH_CreateHookApi(dllName, "_Aime_isMobile@0", Aime_isMobile, NULL);
	MH_CreateHookApi(dllName, "_Aime_isReaderDetected@0", Aime_isReaderDetected, NULL);
	MH_CreateHookApi(dllName, "_Aime_sendLog@12", Aime_sendLog, NULL);
	MH_CreateHookApi(dllName, "_Aime_setLedError@0", Aime_setLedError, NULL);
	MH_CreateHookApi(dllName, "_Aime_setLedSuccess@0", Aime_setLedSuccess, NULL);
	MH_CreateHookApi(dllName, "_Aime_start@0", Aime_start, NULL);
	MH_CreateHookApi(dllName, "_AllnetAccounting_beginPlay@4", AllnetAccounting_beginPlay, NULL);
	MH_CreateHookApi(dllName, "_AllnetAccounting_endPlay@12", AllnetAccounting_endPlay, NULL);
	MH_CreateHookApi(dllName, "_AllnetAuth_getAbaasGsServerName@0", AllnetAuth_getAbaasGsServerName, NULL);
	MH_CreateHookApi(dllName, "_AllnetAuth_getAbaasLinkServerName@0", AllnetAuth_getAbaasLinkServerName, NULL);
	MH_CreateHookApi(dllName, "_AllnetAuth_getCountryCode@0", AllnetAuth_getCountryCode, NULL);
	MH_CreateHookApi(dllName, "_AllnetAuth_getLocationId@0", AllnetAuth_getLocationId, NULL);
	MH_CreateHookApi(dllName, "_AllnetAuth_getLocationName@0", AllnetAuth_getLocationName, NULL);
	MH_CreateHookApi(dllName, "_AllnetAuth_getLocationNickname@4", AllnetAuth_getLocationNickname, NULL);
	MH_CreateHookApi(dllName, "_AllnetAuth_getLocationNicknamePartCount@0", AllnetAuth_getLocationNicknamePartCount, NULL);
	MH_CreateHookApi(dllName, "_AllnetAuth_getRegionCode@0", AllnetAuth_getRegionCode, NULL);
	MH_CreateHookApi(dllName, "_AllnetAuth_getRegionName@4", AllnetAuth_getRegionName, NULL);
	MH_CreateHookApi(dllName, "_AllnetAuth_getRegionNamePartCount@0", AllnetAuth_getRegionNamePartCount, NULL);
	MH_CreateHookApi(dllName, "_AllnetAuth_isDevelop@0", AllnetAuth_isDevelop, NULL);
	MH_CreateHookApi(dllName, "_AllnetAuth_isGood@0", AllnetAuth_isGood, NULL);
	MH_CreateHookApi(dllName, "_ApmSystemSetting_getAdvertizeSound@4", ApmSystemSetting_getAdvertizeSound, NULL);
	MH_CreateHookApi(dllName, "_ApmSystemSetting_getClosingTimes@4", ApmSystemSetting_getClosingTimes, NULL);
	MH_CreateHookApi(dllName, "_ApmSystemSetting_getEmoneySetting@4", ApmSystemSetting_getEmoneySetting, NULL);
	MH_CreateHookApi(dllName, "_ApmSystemSetting_getFixedTitle@4", ApmSystemSetting_getFixedTitle, NULL);
	MH_CreateHookApi(dllName, "_ApmSystemSetting_getGamePadSetting@4", ApmSystemSetting_getGamePadSetting, NULL);
	MH_CreateHookApi(dllName, "_ApmSystemSetting_getMatchingGroup@4", ApmSystemSetting_getMatchingGroup, NULL);
	MH_CreateHookApi(dllName, "_ApmSystemSetting_getTimeToClosingTime@4", ApmSystemSetting_getTimeToClosingTime, NULL);
	MH_CreateHookApi(dllName, "_ApmSystemSetting_getUiSetting@4", ApmSystemSetting_getUiSetting, NULL);
	MH_CreateHookApi(dllName, "_Backup_getRecordStatus@4", Backup_getRecordStatus, NULL);
	MH_CreateHookApi(dllName, "_Backup_isSetupSucceeded@0", Backup_isSetupSucceeded, NULL);
	MH_CreateHookApi(dllName, "_Backup_saveRecord@4", Backup_saveRecord, NULL);
	MH_CreateHookApi(dllName, "_Backup_saveRecordByAddress@4", Backup_saveRecordByAddress, NULL);
	MH_CreateHookApi(dllName, "_Backup_setupRecords@8", Backup_setupRecords, NULL);
	MH_CreateHookApi(dllName, "_Core_execute@0", Core_execute, NULL);
	MH_CreateHookApi(dllName, "_Core_exitGame@4", Core_exitGame, NULL);
	MH_CreateHookApi(dllName, "_Core_hookException@0", Core_hookException, NULL);
	MH_CreateHookApi(dllName, "_Core_isExitNeeded@0", Core_isExitNeeded, NULL);
	MH_CreateHookApi(dllName, "_Core_resetCount@0", Core_resetCount, NULL);
	MH_CreateHookApi(dllName, "_Core_resetException@0", Core_resetException, NULL);
	MH_CreateHookApi(dllName, "_Credit_getAddableCoin@0", Credit_getAddableCoin, NULL);
	MH_CreateHookApi(dllName, "_Credit_getCoinMultipliers@0", Credit_getCoinMultipliers, NULL);
	MH_CreateHookApi(dllName, "_Credit_getCoinToCredit@0", Credit_getCoinToCredit, NULL);
	MH_CreateHookApi(dllName, "_Credit_getCredit@0", Credit_getCredit, NULL);
	MH_CreateHookApi(dllName, "_Credit_getRemain@0", Credit_getRemain, NULL);
	MH_CreateHookApi(dllName, "_Credit_isFreePlay@0", Credit_isFreePlay, NULL);
	MH_CreateHookApi(dllName, "_Credit_isGameCostEnough@4", Credit_isGameCostEnough, NULL);
	MH_CreateHookApi(dllName, "_Credit_isZero@0", Credit_isZero, NULL);
	MH_CreateHookApi(dllName, "_Credit_payGameCost@4", Credit_payGameCost, NULL);
	MH_CreateHookApi(dllName, "_Credit_resetCoinInHook@0", Credit_resetCoinInHook, NULL);
	MH_CreateHookApi(dllName, "_Credit_setCoinInHook@4", Credit_setCoinInHook, NULL);
	MH_CreateHookApi(dllName, "_Credit_toString@0", Credit_toString, NULL);
	MH_CreateHookApi(dllName, "_Emoney_close@0", Emoney_close, NULL);
	MH_CreateHookApi(dllName, "_Emoney_isOpenMainWindow@0", Emoney_isOpenMainWindow, NULL);
	MH_CreateHookApi(dllName, "_Emoney_launch@0", Emoney_launch, NULL);
	MH_CreateHookApi(dllName, "_Emoney_setSoundHook@4", Emoney_setSoundHook, NULL);
	MH_CreateHookApi(dllName, "_Emoney_update@4", Emoney_update, NULL);
	MH_CreateHookApi(dllName, "_Error_isOccurred@0", Error_isOccurred, NULL);
	MH_CreateHookApi(dllName, "_Error_setDeviceLost@0", Error_setDeviceLost, NULL);
	MH_CreateHookApi(dllName, "_Input_getGamepadInfo@0", Input_getGamepadInfo, NULL);
	MH_CreateHookApi(dllName, "_Input_isFlipNow@4", Input_isFlipNow, NULL);
	MH_CreateHookApi(dllName, "_Input_isGamepadDetect@0", Input_isGamepadDetect, NULL);
	MH_CreateHookApi(dllName, "_Input_isOffNow@4", Input_isOffNow, NULL);
	MH_CreateHookApi(dllName, "_Input_isOn@4", Input_isOn, NULL);
	MH_CreateHookApi(dllName, "_Input_isOnNow@4", Input_isOnNow, NULL);
	MH_CreateHookApi(dllName, "_Input_isOpenPewviewWindow@0", Input_isOpenPewviewWindow, NULL);
	MH_CreateHookApi(dllName, "_Input_setGamepadConfig@4", Input_setGamepadConfig, NULL);
	MH_CreateHookApi(dllName, "_NetworkProperty_getAddressString@0", NetworkProperty_getAddressString, NULL);
	MH_CreateHookApi(dllName, "_NetworkProperty_getAddressValue@0", NetworkProperty_getAddressValue, NULL);
	MH_CreateHookApi(dllName, "_Sequence_beginPlay@20", Sequence_beginPlay, NULL);
	MH_CreateHookApi(dllName, "_Sequence_clearBackup@0", Sequence_clearBackup, NULL);
	MH_CreateHookApi(dllName, "_Sequence_continuePlay@28", Sequence_continuePlay, NULL);
	MH_CreateHookApi(dllName, "_Sequence_endPlay@16", Sequence_endPlay, NULL);
	MH_CreateHookApi(dllName, "_Sequence_getBookkeeping@0", Sequence_getBookkeeping, NULL);
	MH_CreateHookApi(dllName, "_Sequence_getPlayingAimeId@0", Sequence_getPlayingAimeId, NULL);
	MH_CreateHookApi(dllName, "_Sequence_isAccountingPlaying@0", Sequence_isAccountingPlaying, NULL);
	MH_CreateHookApi(dllName, "_Sequence_isPlaying@0", Sequence_isPlaying, NULL);
	MH_CreateHookApi(dllName, "_Sequence_isTest@0", Sequence_isTest, NULL);
	MH_CreateHookApi(dllName, "_System_getAppRootPath@0", System_getAppRootPath, NULL);
	MH_CreateHookApi(dllName, "_System_getBoardId@0", System_getBoardId, NULL);
	MH_CreateHookApi(dllName, "_System_getGameId@0", System_getGameId, NULL);
	MH_CreateHookApi(dllName, "_System_getGameVersion@0", System_getGameVersion, NULL);
	MH_CreateHookApi(dllName, "_System_getKeychipId@0", System_getKeychipId, NULL);
	MH_CreateHookApi(dllName, "_System_getRegionCode@0", System_getRegionCode, NULL);
	MH_CreateHookApi(dllName, "_System_getResolution@0", System_getResolution, NULL);
	MH_CreateHookApi(dllName, "_System_isDevelop@0", System_isDevelop, NULL);
	MH_CreateHookApi(dllName, "_apmGamepadConfig@20", apmGamepadConfig, NULL);
	MH_CreateHookApi(dllName, "_apmGamepadGetApmInput@4", apmGamepadGetApmInput, NULL);
	MH_CreateHookApi(dllName, "_apmGamepadGetConfig@0", apmGamepadGetConfig, NULL);
	MH_CreateHookApi(dllName, "_apmGamepadGetDeviceCaps@4", apmGamepadGetDeviceCaps, NULL);
	MH_CreateHookApi(dllName, "_apmGamepadGetInfo@4", apmGamepadGetInfo, NULL);
	MH_CreateHookApi(dllName, "_apmGamepadGetRawInput@4", apmGamepadGetRawInput, NULL);
	MH_CreateHookApi(dllName, "_apmGamepadInitConfig@0", apmGamepadInitConfig, NULL);
	MH_CreateHookApi(dllName, "_apmGamepadIsDetected@0", apmGamepadIsDetected, NULL);
	MH_CreateHookApi(dllName, "_apmGamepadIsXInputDevice@0", apmGamepadIsXInputDevice, NULL);
	MH_CreateHookApi(dllName, "_apmGamepadUpdate@0", apmGamepadUpdate, NULL);
#endif

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

BYTE compressionOn = 0;
BYTE encryptionOn = 0;

static unsigned int WINAPI joyGetPosExHook()
{
	return 167; // JOYERR_UNPLUGGED
}

#ifdef _M_AMD64

// am::abaas::link::HttpData::setOption

void __fastcall setOption_x64(void* me, char a2, char a3)
{
	*((BYTE*)me + 0x21) = compressionOn;
	*((BYTE*)me + 0x22) = encryptionOn;
}

static InitFunction initFuncTapping([]()
{
	HookAPM3(L"SDFJ");

	__int64 mainModuleBase = (__int64)GetModuleHandle(0);

}, GameID::TappingSkillTest);

static InitFunction initFuncPengoeAPM3([]()
{
	MH_Initialize();
	MH_CreateHookApi(L"winmm.dll", "joyGetPosEx", joyGetPosExHook, NULL);
	MH_EnableHook(MH_ALL_HOOKS);

	HookAPM3(L"SDFH");

	__int64 mainModuleBase = (__int64)GetModuleHandle(0);

	// 55 8B EC 8A 45 08 88 41 15 -> 0x140060
#ifdef _DEBUG
	safeJMP(hook::get_pattern("88 51 21 44 88 41 22 C3"), setOption_x64);
#endif

	// Skip keyboard
	injector::MakeRET(hook::get_pattern("48 89 5C 24 08 48 89 6C 24 10 48 89 74 24 18 57 41 54 41 55 41 56 41 57 48 81 EC A0 00 00 00"));

}, GameID::PengoAPM3);

static InitFunction initPengoAPM3TestFunc([]()
{
	MH_Initialize();
	MH_CreateHookApi(L"winmm.dll", "joyGetPosEx", joyGetPosExHook, NULL);
	MH_EnableHook(MH_ALL_HOOKS);

	HookAPM3(L"SDFH");
	__int64 mainModuleBase = (__int64)GetModuleHandle(0);

	injector::MakeRET(hook::get_pattern("48 89 5C 24 08 48 89 6C 24 10 48 89 74 24 18 57 41 54 41 55 41 56 41 57 48 81 EC A0 00 00 00"));

	Sequence_isTestReturnValue = true;

}, GameID::PengoAPM3_Test);

static InitFunction initVF5Func([]()
{
	HookAPM3(L"SDHF");
	__int64 mainModuleBase = (__int64)GetModuleHandle(0);

	BackupSaveFileName = ".\\vf5fs\\save";

}, GameID::VF5Esports);

static InitFunction initVF5TestFunc([]()
{
	HookAPM3(L"SDHF");
	__int64 mainModuleBase = (__int64)GetModuleHandle(0);

	if (strstr(GetCommandLineA(), "-tptest") != NULL)
		Sequence_isTestReturnValue = true;

}, GameID::VF5EsportsTest);

static InitFunction initGoonyaFunc([]()
{
	HookAPM3(L"SDGX");
	Aime_hasConfirmValue = true;
	__int64 mainModuleBase = (__int64)GetModuleHandle(0);
	auto mod = LoadLibraryA(".\\Goonya Fighter_Data\\Plugins\\abaasgs.dll");
	if (mod == nullptr)
	{
		MessageBoxA(0, "Cannot load abaasgs.dll!", "Error", 0);
		ExitProcess(0);
	}

	auto hookLoc = (__int64)mod + 0x9A20;
	safeJMP(hookLoc, setOption_x64);

	auto mod2 = LoadLibraryA(".\\Goonya Fighter_Data\\Plugins\\abaaslink.dll");
	if (mod2 == nullptr)
	{
		MessageBoxA(0, "Cannot load abaaslink.dll!", "Error", 0);
		ExitProcess(0);
	}

	auto hookLoc2 = (__int64)mod2 + 0x46FD0;
	safeJMP(hookLoc2, setOption_x64);

	// 88 51 21 44 88 41 22 C3

}, GameID::GoonyaFighter);

static InitFunction initFuncGoonyaFighterTest([]()
{
	HookAPM3(L"SDGX");

	DWORD_PTR mainModuleBase = (DWORD_PTR)GetModuleHandle(0);

}, GameID::GoonyaFighterTest);

static InitFunction initPuyoFunc([]()
{
	HookAPM3(L"SDFF");
	__int64 mainModuleBase = (__int64)GetModuleHandle(0);

}, GameID::PuyoPuyoEsports);

static InitFunction initDoa6Func([]()
{
	HookAPM3(L"SDFM");
	__int64 mainModuleBase = (__int64)GetModuleHandle(0);

}, GameID::Doa6);

static InitFunction initGGSFunc([]()
{
	HookAPM3(L"SDGM");
	__int64 mainModuleBase = (__int64)GetModuleHandle(0);

	// 55 8B EC 8A 45 08 88 41 15 -> 0x140060
#ifdef _DEBUG
	safeJMP(hook::get_pattern("88 51 21 44 88 41 22 C3"), setOption_x64);
#endif
}, GameID::GGS);

static InitFunction initFuncCottonRocknRollATest([]()
{
	HookAPM3(L"SDHW");

	DWORD_PTR mainModuleBase = (DWORD_PTR)GetModuleHandle(0);

	LoadLibraryA(".\\\dll\\x64\\libcurl.dll");

}, GameID::CottonRocknRollATest);

static InitFunction initDoa6TestFunc([]()
{
	HookAPM3(L"SDFM");
	__int64 mainModuleBase = (__int64)GetModuleHandle(0);

}, GameID::Doa6Test);

static InitFunction initNosferatuLilinorTestFunc([]()
{
	HookAPM3(L"SDHP");
	__int64 mainModuleBase = (__int64)GetModuleHandle(0);

}, GameID::NosferatuLilinor);

HMODULE(WINAPI* g_origLoadLibraryW)(LPCWSTR lpRootPathName);

HMODULE LoadLibraryWHook(LPCWSTR lpLibFileName)
{
	if (wcscmp(L"X:/lib/apm.dll", lpLibFileName) == 0)
	{
		return g_origLoadLibraryW(L"apm.dll");
	}
	return g_origLoadLibraryW(lpLibFileName);
}

static InitFunction initKasioriTestFunc([]()
{
	// voce pode copiar meu trabalho jajajajajajajaja
	MH_Initialize();
	MH_CreateHookApi(L"kernel32.dll", "LoadLibraryW", LoadLibraryWHook, (void**)&g_origLoadLibraryW);
	MH_EnableHook(MH_ALL_HOOKS);
	HookAPM3(L"SDHV");
	__int64 mainModuleBase = (__int64)GetModuleHandle(0);

}, GameID::Kasiori);

static InitFunction initOtoshuDXTestFunc([]()
{
	HookAPM3(L"SDGF");
	__int64 mainModuleBase = (__int64)GetModuleHandle(0);

}, GameID::OtoshuDX);

static InitFunction initFuncBladeStrangesAPM3Test([]()
{
	HookAPM3(L"SDFD");

	DWORD_PTR mainModuleBase = (DWORD_PTR)GetModuleHandle(0);

}, GameID::BladeStrangesAPM3Test);

static InitFunction initFuncDengekiBunkoFCAPM3Test([]()
{
	HookAPM3(L"SDGC");

	DWORD_PTR mainModuleBase = (DWORD_PTR)GetModuleHandle(0);

}, GameID::DengekiBunkoFCTest);

static InitFunction initFuncUmifreshTest([]()
{
	HookAPM3(L"SDGU");

	DWORD_PTR mainModuleBase = (DWORD_PTR)GetModuleHandle(0);

}, GameID::UmifreshTest);

#else

void __declspec(naked) setOption_x86()
{
	__asm
	{
		mov al, compressionOn
		mov byte ptr ds : [ecx + 0x15] , al
		mov al, encryptionOn
		mov byte ptr ds : [ecx + 0x16] , al
		ret 0x8
	}
}

static InitFunction initFuncUmifresh([]()
{
	HookAPM3(L"SDGU");

	DWORD_PTR mainModuleBase = (DWORD_PTR)GetModuleHandle(0);

	// Windowed
	if (ToBool(config["General"]["Windowed"]))
	{
		injector::WriteMemory<BYTE>(hook::get_pattern("74 7A 68 DC 00 00 00"), 0xEB, true);
	}

	injector::WriteMemory<BYTE>(hook::get_pattern("75 47 53 8B 1D ? ? ? ?"), 0xEB, true); // Skip emoneyUI check

}, GameID::Umifresh);

static InitFunction initFuncRollingGunner([]()
{
	HookAPM3(L"SDGW");

	DWORD_PTR mainModuleBase = (DWORD_PTR)GetModuleHandle(0);

}, GameID::RollingGunner);

static InitFunction initFuncAleste([]()
{
	HookAPM3(L"SDHB");

	DWORD_PTR mainModuleBase = (DWORD_PTR)GetModuleHandle(0);
#ifdef _DEBUG
	// 55 8B EC 8A 45 08 88 41 15 -> 0x140060
	safeJMP(hook::get_pattern("55 8B EC 8A 45 08 88 41 15"), setOption_x86);
#endif
}, GameID::Aleste);

static InitFunction initFuncAleste11([]()
{
	HookAPM3(L"SDHB");

	DWORD_PTR mainModuleBase = (DWORD_PTR)GetModuleHandle(0);

}, GameID::Aleste11);

static InitFunction initFuncAleste12([]()
	{
		HookAPM3(L"SDHB");

		DWORD_PTR mainModuleBase = (DWORD_PTR)GetModuleHandle(0);

}, GameID::Aleste12);

BOOL WINAPI ClipCursorHook(const RECT* lpRect)
{
	return false;
}

static InitFunction initFuncGGXrdAPM3([]()
{
	HookAPM3(L"SDFB");

	DWORD_PTR mainModuleBase = (DWORD_PTR)GetModuleHandle(0);

	if (strstr(GetCommandLineA(), "-tptest") != NULL)
		Sequence_isTestReturnValue = true;

	// force fullscreen
	injector::MakeNOP(mainModuleBase + 0x7300F2, 2, true); // B300F2

	if (ToBool(config["General"]["Windowed"]))
	{
		MH_Initialize();
		MH_CreateHookApi(L"user32.dll", "ClipCursor", ClipCursorHook, NULL);
		MH_EnableHook(MH_ALL_HOOKS);

		// force windowed
		injector::WriteMemory<BYTE>(mainModuleBase + 0x7300FA, 0x00, true); // B300FA
	}

}, GameID::GGXrdAPM3);

static InitFunction initFuncCottonRocknRollA([]()
{
	HookAPM3(L"SDHW");

	DWORD_PTR mainModuleBase = (DWORD_PTR)GetModuleHandle(0);

	LoadLibraryA(".\\\dll\\x86\\libcurl.dll");

	if (ToBool(config["General"]["Windowed"]))
	{
		injector::WriteMemory<BYTE>(hook::get_pattern("74 7A 68 DC 00 00 00"), 0xEB, true); // Windowed
	}

	injector::WriteMemory<BYTE>(hook::get_pattern("75 47 53 8B 1D ? ? ? ?"), 0xEB, true); // Skip emoneyUI check

}, GameID::CottonRocknRollA);

static InitFunction initFuncBlazBlueCrossTagBattle([]()
{
	HookAPM3(L"SDFU");

	Credit_getCreditReturnValue = 3;

	DWORD_PTR mainModuleBase = (DWORD_PTR)GetModuleHandle(0);

}, GameID::BlazBlueCrossTagBattle);

static InitFunction initFuncDengekiBunkoFC([]()
{
	HookAPM3(L"SDGC");

	DWORD_PTR mainModuleBase = (DWORD_PTR)GetModuleHandle(0);

}, GameID::DengekiBunkoFC);

static InitFunction initFuncBladeStrangesAPM3([]()
{
	HookAPM3(L"SDFD");

	DWORD_PTR mainModuleBase = (DWORD_PTR)GetModuleHandle(0);

	injector::WriteMemory<BYTE>(hook::get_pattern("75 47 53 8B 1D ? ? ? ?"), 0xEB, true); // Skip emoneyUI check

	AllnetAuth_isGoodReturnValue = false;

}, GameID::BladeStrangesAPM3);

static InitFunction initFuncKoihimeEnbuAPM3([]()
{
	HookAPM3(L"SDFR");

	DWORD_PTR mainModuleBase = (DWORD_PTR)GetModuleHandle(0);

}, GameID::KoihimeEnbuAPM3);

static InitFunction initFuncUnderNightAPM3([]()
{
	HookAPM3(L"SDGR");

	DWORD_PTR mainModuleBase = (DWORD_PTR)GetModuleHandle(0);

}, GameID::UnderNightAPM3);

static InitFunction initFuncUnderNightAPM3Test([]()
{
	HookAPM3(L"SDGR");

	DWORD_PTR mainModuleBase = (DWORD_PTR)GetModuleHandle(0);

}, GameID::UnderNightAPM3Test);

#endif
