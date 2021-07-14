#include <Utility/InitFunction.h>
#include <StdInc.h>
#include <thread>
#ifdef _M_AMD64
#include "Functions\Global.h"
#include "APM3.h"

static uint8_t g_APM3IOValues[256];

bool Aime_acceptConfirm()
{
#ifdef _DEBUG
	info(true, "Aime_acceptConfirm");
#endif
	return Aime_acceptConfirmReturnValue;
}

bool Aime_cancel()
{
#ifdef _DEBUG
	info(true, "Aime_cancel");
#endif

	return Aime_cancelReturnValue;
}

struct AccessCode
{
	const int DigitCount = 20;
	const int Size = 10;
	const int StrSize = 41;
	char values[10];
	char valueStr[41];
};

bool __fastcall Aime_getAccessCode(AccessCode *accessCode)
{
#ifdef _DEBUG
	info(true, "Aime_getAccessCode");
#endif
	return Aime_getAccessCodeReturnValue;
}

bool __fastcall Aime_getAimeId(unsigned int* uid)
{
#ifdef _DEBUG
	info(true, "Aime_getAimeId");
#endif
	return Aime_getAimeIdReturnValue;
}

enum class AIME_CONFIRM
{
	NoneDB,
	FeliCaDB,
	AimeDB,
};

AIME_CONFIRM Aime_getConfirm()
{
#ifdef _DEBUG
	info(true, "Aime_getConfirm");
#endif
	return AIME_CONFIRM::AimeDB;
}

enum class AIME_ERROR_CATEGORY
{
	NONE,
	WARNING,
	NETWORK,
	FATAL,
	UNKNOWN,
};

AIME_ERROR_CATEGORY Aime_getErrorCategory()
{
#ifdef _DEBUG
	info(true, "Aime_getErrorCategory");
#endif
	return AIME_ERROR_CATEGORY::NONE;
}

bool Aime_hasConfirm()
{
#ifdef _DEBUG
	info(true, "Aime_hasConfirm");
#endif
	return Aime_hasConfirmReturnValue;
}

bool Aime_hasError()
{
#ifdef _DEBUG
	info(true, "Aime_hasError");
#endif
	return Aime_hasErrorReturnValue;
}

bool Aime_hasResult()
{
#ifdef _DEBUG
	info(true, "Aime_hasResult");
#endif
	return Aime_hasResultReturnValue;
}

bool Aime_isBusy()
{
#ifdef _DEBUG
	info(true, "Aime_isBusy");
#endif
	return Aime_isBusyReturnValue;
}

bool Aime_isDBAlive()
{
#ifdef _DEBUG
	info(true, "Aime_isDBAlive");
#endif
	return Aime_isDBAliveReturnValue;
}

bool Aime_isMobile()
{
#ifdef _DEBUG
	info(true, "Aime_isMobile");
#endif
	return Aime_isMobileReturnValue;
}

bool Aime_isReaderDetected()
{
#ifdef _DEBUG
	info(true, "Aime_isReaderDetected");
#endif
	return Aime_isReaderDetectedReturnValue;
}


enum class AIME_LOG_STATUS
{
	BEGIN,
	CONTINUE,
	END,
};

bool __fastcall Aime_sendLog(unsigned int uid, AIME_LOG_STATUS status, unsigned __int64 count)
{
#ifdef _DEBUG
	info(true, "Aime_sendLog");
#endif
	return Aime_sendLogReturnValue;
}

bool Aime_setLedError()
{
#ifdef _DEBUG
	info(true, "Aime_setLedError");
#endif
	return Aime_setLedErrorReturnValue;
}

bool Aime_setLedSuccess()
{
#ifdef _DEBUG
	info(true, "Aime_setLedSuccess");
#endif
	return Aime_setLedSuccessReturnValue;
}

bool Aime_start()
{
#ifdef _DEBUG
	info(true, "Aime_start");
#endif
	return Aime_startReturnValue;
}

__int64 __fastcall AllnetAccounting_beginPlay(unsigned int a1)
{
#ifdef _DEBUG
	info(true, "AllnetAccounting_beginPlay");
#endif
	return AllnetAccounting_beginPlayReturnValue;
}

__int64 __fastcall AllnetAccounting_endPlay(int a1, int a2, int a3)
{
#ifdef _DEBUG
	info(true, "AllnetAccounting_endPlay");
#endif
	return AllnetAccounting_endPlayReturnValue;
}

char* AllnetAuth_getAbaasGsServerName()
{
#ifdef _DEBUG
	info(true, "AllnetAuth_getAbaasGsServerName");
#endif
	return ServerName;
}

char* AllnetAuth_getAbaasLinkServerName()
{
#ifdef _DEBUG
	info(true, "AllnetAuth_getAbaasLinkServerName");
#endif
	return LinkServerName;
}

__int64 AllnetAuth_getCountryCode()
{
#ifdef _DEBUG
	info(true, "AllnetAuth_getCountryCode");
#endif
	return AllnetAuth_getCountryCodeReturnValue;
}

__int64 AllnetAuth_getLocationId()
{
#ifdef _DEBUG
	info(true, "AllnetAuth_getLocationId");
#endif
	return AllnetAuth_getLocationIdReturnValue;
}

char* AllnetAuth_getLocationName()
{
#ifdef _DEBUG
	info(true, "AllnetAuth_getLocationName");
#endif
	return LocationName;
}

char* __fastcall AllnetAuth_getLocationNickname(int a1)
{
#ifdef _DEBUG
	info(true, "AllnetAuth_getLocationNickname");
#endif
	return LocationNickName;
}

__int64 AllnetAuth_getLocationNicknamePartCount()
{
#ifdef _DEBUG
	info(true, "AllnetAuth_getLocationNicknamePartCount");
#endif
	return AllnetAuth_getLocationNicknamePartCountReturnValue;
}

__int64 AllnetAuth_getRegionCode()
{
#ifdef _DEBUG
	info(true, "AllnetAuth_getRegionCode");
#endif
	return AllnetAuth_getRegionCodeReturnValue;
}

char* __fastcall AllnetAuth_getRegionName(int a1)
{
#ifdef _DEBUG
	info(true, "AllnetAuth_getRegionName");
#endif
	return RegionName;
}

__int64 AllnetAuth_getRegionNamePartCount()
{
#ifdef _DEBUG
	info(true, "AllnetAuth_getRegionNamePartCount");
#endif
	return AllnetAuth_getRegionNamePartCountReturnValue;
}

__int64 AllnetAuth_isDevelop()
{
#ifdef _DEBUG
	info(true, "AllnetAuth_isDevelop");
#endif
	return AllnetAuth_isDevelopReturnValue;
}

__int64 AllnetAuth_isGood()
{
#ifdef _DEBUG
	info(true, "AllnetAuth_isGood");
#endif
	return AllnetAuth_isGoodReturnValue;
}

bool __fastcall ApmSystemSetting_getAdvertizeSound(bool *enable)
{
#ifdef _DEBUG
	info(true, "ApmSystemSetting_getAdvertizeSound");
#endif
	return ApmSystemSetting_getAdvertizeSoundReturnValue;
}

bool __fastcall ApmSystemSetting_getClosingTimes(__int64 *closingTimes)
{
#ifdef _DEBUG
	info(true, "ApmSystemSetting_getClosingTimes");
#endif
	return ApmSystemSetting_getClosingTimesReturnValue;
}

char __fastcall ApmSystemSetting_getEmoneySetting(__int64 a1)
{
#ifdef _DEBUG
	info(true, "ApmSystemSetting_getEmoneySetting");
#endif
	return ApmSystemSetting_getEmoneySettingReturnValue;
}

bool __fastcall ApmSystemSetting_getFixedTitle(bool *enable)
{
#ifdef _DEBUG
	info(true, "ApmSystemSetting_getFixedTitle");
#endif
	return ApmSystemSetting_getFixedTitleReturnValue;
}

char __fastcall ApmSystemSetting_getGamePadSetting(WORD* a1)
{
#ifdef _DEBUG
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
#ifdef _DEBUG
	info(true, "ApmSystemSetting_getMatchingGroup");
#endif
	return ApmSystemSetting_getMatchingGroupReturnValue;
}

bool __fastcall ApmSystemSetting_getTimeToClosingTime(unsigned int* seconds)
{
#ifdef _DEBUG
	info(true, "ApmSystemSetting_getTimeToClosingTime");
#endif
	*seconds = 0;
	return ApmSystemSetting_getTimeToClosingTimeReturnValue;
}

char __fastcall ApmSystemSetting_getUiSetting(__int64 a1)
{
#ifdef _DEBUG
	info(true, "ApmSystemSetting_getUiSetting");
#endif
	return ApmSystemSetting_getUiSettingReturnValue;
}

unsigned char saveArray[0x2000]; // some stupid size
int sectorSize = 0x20; // Guess
__int64* InGameSavePointer;
__int64 SaveFileSize = 0;
char* SaveFileName = "save";
char fileBuffer[256];

struct BackupRecord
{
	void* Address;
	unsigned long Size;
};

BackupRecord *internal_Records;
unsigned int internal_recordCount;

enum class BackupRecordStatus
{
	BackupRecordStatus_InvalidCall = -1, // 0xFFFFFFFF
	BackupRecordStatus_Valid = 0,
	BackupRecordStatus_DiffApp = 1,
	BackupRecordStatus_BrokenData = 2,
};

BackupRecordStatus __fastcall Backup_getRecordStatus(__int64 recordIndex)
{
#ifdef _DEBUG
	info(true, "Backup_getRecordStatus %llx", recordIndex);
#endif
	return BackupRecordStatus::BackupRecordStatus_Valid;
}

bool Backup_isSetupSucceeded()
{
#ifdef _DEBUG
	info(true, "Backup_isSetupSucceeded");
#endif
	return Backup_isSetupSucceededReturnValue;
}

bool __fastcall Backup_saveRecord(unsigned long recordIndex)
{
#ifdef _DEBUG
	info(true, "Backup_saveRecord recordIndex: %llx", recordIndex);
#endif
	memset(fileBuffer, 0, sizeof(fileBuffer));
	sprintf(fileBuffer, "%s%02d.bin", SaveFileName, recordIndex);
	auto file = fopen(fileBuffer, "wb+");
	if (file)
	{
		fwrite(internal_Records[recordIndex].Address, 1, internal_Records[recordIndex].Size, file);
		fclose(file);
	}
	return Backup_saveRecordReturnValue;
}

__int64 __fastcall Backup_saveRecordByAddress(__int64 recordAddress)
{
#ifdef _DEBUG
	info(true, "Backup_saveRecordByAddress %llx", recordAddress);
#endif
	return Backup_saveRecordByAddressReturnValue;
}




//__int64 __fastcall Backup_setupRecords(__int128* a1, unsigned int a2)
bool __fastcall Backup_setupRecords(BackupRecord *records, unsigned int recordCount)
{
	for (int i = 0; i < recordCount; i++)
	{
		memset(fileBuffer, 0, sizeof(fileBuffer));
		sprintf(fileBuffer, "%s%02d.bin", SaveFileName, i);
		FILE* fsave = fopen(fileBuffer, "r");
		internal_Records = records;
		internal_recordCount = recordCount;
		if (fsave != NULL)
		{
#ifdef _DEBUG
		info(true, "Backup setuprecords %02d of %02d, loading file %s", i, recordCount, fileBuffer);
#endif		
			fread(records[i].Address, 1, records[i].Size, fsave); // add file size check noob
			fclose(fsave);
		}
		else
		{
#ifdef _DEBUG
			info(true, "Backup setuprecords %02d of %02d, saving file %s", i, recordCount, fileBuffer);
#endif
			auto file = fopen(fileBuffer, "wb+");
			if (file)
			{
				fwrite(records[i].Address, 1, records[i].Size, file);
				fclose(file);
			}
		}
	}
	return true;
}

void Core_execute()
{
#ifdef _DEBUG
	info(true, "Core_execute");
#endif
}

bool __fastcall Core_exitGame(bool isTest)
{
#ifdef _DEBUG
	info(true, "Core_exitGame");
#endif
	return Core_exitGameReturnValue;
}

__int64 Core_hookException()
{
#ifdef _DEBUG
	info(true, "Core_hookException");
#endif
	return Core_hookExceptionReturnValue;
}

bool Core_isExitNeeded()
{
#ifdef _DEBUG
	info(true, "Core_isExitNeeded");
#endif
	return Core_isExitNeededReturnValue;
}

void Core_resetCount()
{
#ifdef _DEBUG
	info(true, "Core_resetCount");
#endif
}

__int64 Core_resetException()
{
#ifdef _DEBUG
	info(true, "Core_resetException");
#endif
	return Core_resetExceptionReturnValue;
}

__int64 Credit_getAddableCoin()
{
#ifdef _DEBUG
	info(true, "Credit_getAddableCoin");
#endif
	return Credit_getAddableCoinReturnValue;
}

__int64 Credit_getCoinMultipliers()
{
#ifdef _DEBUG
	info(true, "Credit_getCoinMultipliers");
#endif
	return Credit_getCoinMultipliersReturnValue;
}

__int64 Credit_getCoinToCredit()
{
#ifdef _DEBUG
	info(true, "Credit_getCoinToCredit");
#endif
	return Credit_getCoinToCreditReturnValue;
}

__int64 Credit_getCredit()
{
#ifdef _DEBUG
	info(true, "Credit_getCredit");
#endif
	return Credit_getCreditReturnValue;
}

__int64 Credit_getRemain()
{
#ifdef _DEBUG
	info(true, "Credit_getRemain");
#endif
	return Credit_getRemainReturnValue;
}

__int64 Credit_isFreePlay()
{
#ifdef _DEBUG
	info(true, "Credit_isFreePlay");
#endif
	return Credit_isFreePlayReturnValue;
}

__int64 __fastcall Credit_isGameCostEnough(__int64 a1)
{
#ifdef _DEBUG
	info(true, "Credit_isGameCostEnough");
#endif
	return Credit_isGameCostEnoughReturnValue; // 1
}

__int64 Credit_isZero()
{
#ifdef _DEBUG
	info(true, "Credit_isZero");
#endif
	return Credit_isZeroReturnValue;
}

__int64 __fastcall Credit_payGameCost(__int64 a1)
{
#ifdef _DEBUG
	info(true, "Credit_payGameCost");
#endif
	return Credit_payGameCostReturnValue;
}

__int64 Credit_resetCoinInHook()
{
#ifdef _DEBUG
	info(true, "Credit_resetCoinInHook");
#endif

	return Credit_resetCoinInHookReturnValue;
}

__int64 Credit_setCoinInHook()
{
#ifdef _DEBUG
	info(true, "Credit_setCoinInHook");
#endif

	return Credit_setCoinInHookReturnValue;
}

wchar_t* Credit_toString()
{
#ifdef _DEBUG
	info(true, "Credit_toString");
#endif

	static wchar_t lol[64];
	lol[0] = '1';
	return lol;
}

void Emoney_close()
{
#ifdef _DEBUG
	info(true, "Emoney_close");
#endif
}

__int64 Emoney_isOpenMainWindow()
{
#ifdef _DEBUG
	info(true, "Emoney_isOpenMainWindow");
#endif
	return Emoney_isOpenMainWindowReturnValue;
}

char Emoney_launch()
{
#ifdef _DEBUG
	info(true, "Emoney_launch");
#endif
	return Emoney_launchReturnValue;
}

void __fastcall Emoney_setSoundHook(__int64 a1)
{
#ifdef _DEBUG
	info(true, "Emoney_setSoundHook");
#endif
}

void __fastcall Emoney_update(BYTE* a1)
{
#ifdef _DEBUG
	info(true, "Emoney_update");
#endif
}

__int64 Error_isOccurred()
{
#ifdef _DEBUG
	info(true, "Error_isOccurred");
#endif
	return Error_isOccurredReturnValue;
}

__int64 Error_setDeviceLost()
{
#ifdef _DEBUG
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
#ifdef _DEBUG
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
#ifdef _DEBUG
	info(true, "Input_isFlipNow");
#endif
	return Input_isFlipNowReturnValue;
}

__int64 Input_isGamepadDetect()
{
#ifdef _DEBUG
	info(true, "Input_isGamepadDetect");
#endif
	return Input_isGamepadDetectReturnValue;
}

char __fastcall Input_isOffNow(unsigned int a1)
{
#ifdef _DEBUG
	info(true, "Input_isOffNow");
#endif
	return Input_isOffNowReturnValue;
}

int __fastcall Input_isOn(unsigned int buttonId)
{
#ifdef _DEBUG
	info(true, "Input_isOn id: %u", buttonId);
#endif
	return g_APM3IOValues[buttonId];
}

char __fastcall Input_isOnNow(unsigned int buttonId)
{
#ifdef _DEBUG
	info(true, "Input_isOnNow id: %u", buttonId);
#endif
	return g_APM3IOValues[buttonId];
}

__int64 Input_isOpenPewviewWindow()
{
#ifdef _DEBUG
	info(true, "Input_isOpenPewviewWindow");
#endif
	return Input_isOpenPewviewWindowReturnValue;
}

char __fastcall Input_setGamepadConfig(__int64 a1)
{
#ifdef _DEBUG
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

//__int64 __fastcall Sequence_beginPlay(__int64 a1, struct Concurrency::details::_CancellationTokenState* a2, _DWORD* a3, __int64 a4)
__int64 __fastcall Sequence_beginPlay(__int64 a1, void* a2, DWORD* a3, __int64 a4)
{
#ifdef _DEBUG
	info(true, "Sequence_beginPlay");
#endif
	return Sequence_beginPlayReturnValue;
}

__int64 Sequence_clearBackup()
{
#ifdef _DEBUG
	info(true, "Sequence_clearBackup");
#endif
	return Sequence_clearBackupReturnValue;
}

__int64 __fastcall Sequence_continuePlay(__int64 a1, __int64 a2, DWORD* a3, __int64 a4)
{
#ifdef _DEBUG
	info(true, "Sequence_continuePlay");
#endif
	return Sequence_continuePlayReturnValue;
}

//__int64 __fastcall Sequence_endPlay(__int64 a1, struct Concurrency::details::_CancellationTokenState* a2)
__int64 __fastcall Sequence_endPlay(__int64 a1, void* a2)
{
#ifdef _DEBUG
	info(true, "Sequence_endPlay");
#endif
	return Sequence_endPlayReturnValue;
}

struct SequenceTimeHistogramItem
{
	unsigned int TimeRangeMin;
	unsigned int TimeRangeMax;
	unsigned int Count;
};

struct SequenceBookkeeping
{
	const unsigned long MaxTimeHistogramCount = 32;
	unsigned int NumberOfGames;
	unsigned int TotalTime;
	unsigned int PlayTime;
	unsigned int AveragePlayTime;
	unsigned int LongestPlayTime;
	unsigned int ShortestPlayTime;
	unsigned int TimeHistogramCount;
	SequenceTimeHistogramItem TimeHistogram[32];
};

SequenceBookkeeping* Sequence_getBookkeeping()
{
#ifdef _DEBUG
	info(true, "Sequence_getBookkeeping");
#endif
	static SequenceBookkeeping *_sequenceBookkeeping;
	if (_sequenceBookkeeping == nullptr)
	{
		_sequenceBookkeeping = (SequenceBookkeeping*)malloc(sizeof(SequenceBookkeeping));
		memset(_sequenceBookkeeping, 0, sizeof(SequenceBookkeeping));
	}
	return _sequenceBookkeeping;
}

__int64 Sequence_getPlayingAimeId()
{
#ifdef _DEBUG
	info(true, "Sequence_getPlayingAimeId");
#endif
	return Sequence_getPlayingAimeIdReturnValue;
}

__int64 Sequence_isAccountingPlaying()
{
#ifdef _DEBUG
	info(true, "Sequence_isAccountingPlaying");
#endif
	return Sequence_isAccountingPlayingReturnValue;
}

__int64 Sequence_isPlaying()
{
#ifdef _DEBUG
	info(true, "Sequence_isPlaying");
#endif
	return Sequence_isPlayingReturnValue;
}

__int64 Sequence_isTest()
{
#ifdef _DEBUG
	info(true, "Sequence_isTest");
#endif
	return Sequence_isTestReturnValue;
}

__int64 System_getAppRootPath()
{
#ifdef _DEBUG
	info(true, "System_getAppRootPath");
#endif
	return System_getAppRootPathReturnValue;
}

struct StandardSerialID
{
	const unsigned long LENGTH = 16;
	char Value[17];
};

struct ShortSerialID
{
	const unsigned long LENGTH = 11;
	char Value[17];
};

struct SerialID
{
	StandardSerialID *ID;
	ShortSerialID *ShortId;
};

SerialID* System_getBoardId()
{
#ifdef _DEBUG
	info(true, "System_getBoardId");
#endif
	static SerialID *_serialId;
	if (_serialId == nullptr)
	{
		_serialId = (SerialID*)malloc(sizeof(SerialID));
		_serialId->ID = (StandardSerialID*)malloc(sizeof(StandardSerialID));
		_serialId->ShortId = (ShortSerialID*)malloc(sizeof(ShortSerialID));
	}
	return _serialId;
}

char* System_getGameId()
{
#ifdef _DEBUG
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
#ifdef _DEBUG
	info(true, "System_getGameVersion");
#endif
	static GameVersion *_gameVersion;
	if (_gameVersion == nullptr)
	{
		_gameVersion = (GameVersion*)malloc(sizeof(GameVersion));
	}
	return _gameVersion;
}

SerialID* System_getKeychipId()
{
#ifdef _DEBUG
	info(true, "System_getKeychipId");
#endif
	static SerialID* _serialId;
	if (_serialId == nullptr)
	{
		_serialId = (SerialID*)malloc(sizeof(SerialID));
		_serialId->ID = (StandardSerialID*)malloc(sizeof(StandardSerialID));
		_serialId->ShortId = (ShortSerialID*)malloc(sizeof(ShortSerialID));
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
#ifdef _DEBUG
	info(true, "System_getRegionCode");
#endif
	return SystemRegionCode::Japan;
}

__int64 System_getResolution()
{
#ifdef _DEBUG
	info(true, "System_getResolution");
#endif
	return System_getResolutionReturnValue;
}

__int64 System_isDevelop()
{
#ifdef _DEBUG
	info(true, "System_isDevelop");
#endif
	return System_isDevelopReturnValue;
}

char __fastcall apmGamepadConfig(__int64 a1)
{
#ifdef _DEBUG
	info(true, "apmGamepadConfig");
#endif
	return apmGamepadConfigReturnValue;
}

char __fastcall apmGamepadGetApmInput(DWORD* a1)
{
#ifdef _DEBUG
	info(true, "apmGamepadGetApmInput");
#endif
	return apmGamepadGetApmInputReturnValue;
}

//__int128* apmGamepadGetConfig()
__int64* apmGamepadGetConfig()
{
#ifdef _DEBUG
	info(true, "apmGamepadGetConfig");
#endif
	static __int64 dumb = 0;
	return &dumb;
}

char __fastcall apmGamepadGetDeviceCaps(__int64 a1)
{
#ifdef _DEBUG
	info(true, "apmGamepadGetDeviceCaps");
#endif
	return apmGamepadGetDeviceCapsReturnValue;
}

//errno_t __fastcall apmGamepadGetInfo(__int64 a1)
__int64 __fastcall apmGamepadGetInfo(__int64 a1)
{
#ifdef _DEBUG
	info(true, "apmGamepadGetInfo");
#endif
	return apmGamepadGetInfoReturnValue;
}

//char __fastcall apmGamepadGetRawInput(_QWORD* a1)
char __fastcall apmGamepadGetRawInput(__int64* a1)
{
#ifdef _DEBUG
	info(true, "apmGamepadGetRawInput");
#endif
	return apmGamepadGetRawInputReturnValue;
}

__int64 apmGamepadInitConfig()
{
#ifdef _DEBUG
	info(true, "apmGamepadInitConfig");
#endif
	return apmGamepadInitConfigReturnValue;
}

char apmGamepadIsDetected()
{
#ifdef _DEBUG
	info(true, "apmGamepadIsDetected");
#endif
	return apmGamepadIsDetectedReturnValue;
}

__int64 apmGamepadIsXInputDevice()
{
#ifdef _DEBUG
	info(true, "apmGamepadIsXInputDevice");
#endif
	return apmGamepadIsXInputDeviceReturnValue;
}

int apmGamepadUpdate()
{
#ifdef _DEBUG
	info(true, "apmGamepadUpdate");
#endif
	return apmGamepadUpdateReturnValue;
}

static void HookAPM3()
{
	strcpy(ServerName, "TeknoParrot Server Hi!");
	strcpy(LinkServerName, "TeknoParrot Link Server Hi!");
	strcpy(LocationNickName, "Flatty");
	strcpy(LocationName, "Flatearth");
	strcpy(RegionName, "Santaland");
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
	strcpy(APM3GameId, "SDFJ");

	__int64 mainModuleBase = (__int64)GetModuleHandle(0);

}, GameID::TappingSkillTest);

static InitFunction initFunc([]()
{
	HookAPM3();
	strcpy(APM3GameId, "SDFH");

	__int64 mainModuleBase = (__int64)GetModuleHandle(0);
	/// PATTERNS BELOW
	// Skip joysticks
	injector::MakeRET(mainModuleBase + 0x15C5B0);
	// Skip keyboard
	injector::MakeRET(mainModuleBase + 0x15CBA0);

}, GameID::Pengoe5);

static InitFunction initFuncPengoe511([]()
{
	HookAPM3();
	strcpy(APM3GameId, "SDFH");

	__int64 mainModuleBase = (__int64)GetModuleHandle(0);

	// Skip joysticks
	injector::MakeRET(mainModuleBase + 0x16A7C0); // CC 48 89 5C 24 10 48 89 6C 24 18 48 89 74 24 20 57 48 83 EC 40
	// Skip keyboard
	injector::MakeRET(mainModuleBase + 0x16ADB0); // 48 89 5C 24 08 48 89 6C 24 10 48 89 74 24 18 57 41 54 41 55 41 56 41 57 48 81 EC A0 00 00 00

}, GameID::Pengoe511);

static InitFunction initTestFunc([]()
{
	HookAPM3();
	strcpy(APM3GameId, "SDFH");
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
	strcpy(APM3GameId, "SDHF");
	__int64 mainModuleBase = (__int64)GetModuleHandle(0);

	SaveFileName = ".\\vf5fs\\save";

}, GameID::VF5Esports);

static InitFunction initVF5TestFunc([]()
{
	HookAPM3();
	strcpy(APM3GameId, "SDHF");
	__int64 mainModuleBase = (__int64)GetModuleHandle(0);

	if (strstr(GetCommandLineA(), "-tptest") != NULL)
		Sequence_isTestReturnValue = 1;

}, GameID::VF5EsportsTest);

static InitFunction initGoonyaFunc([]()
{
	HookAPM3();
	strcpy(APM3GameId, "SDGX");
	__int64 mainModuleBase = (__int64)GetModuleHandle(0);

}, GameID::GoonyaFighter);

static InitFunction initPuyoFunc([]()
{
	HookAPM3();
	strcpy(APM3GameId, "SDFF");
	__int64 mainModuleBase = (__int64)GetModuleHandle(0);

}, GameID::PuyoPuyoEsports);
#endif