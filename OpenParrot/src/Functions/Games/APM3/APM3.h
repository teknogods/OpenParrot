#ifdef _M_AMD64
static char APM3GameId[5];
static char RegionName[64];
static char LocationNickName[64];
static char LocationName[64];
static char LinkServerName[64];
static char ServerName[64];

static bool Aime_acceptConfirmReturnValue = false;
static bool Aime_cancelReturnValue = false;
static __int64 Aime_getAccessCodeReturnValue = 0;
static bool Aime_getAimeIdReturnValue = false;
static __int64 Aime_getConfirmReturnValue = 0;
static bool Aime_hasConfirmReturnValue = false;
static bool Aime_hasErrorReturnValue = false;
static bool Aime_hasResultReturnValue = false;
static bool Aime_isBusyReturnValue = false;

static bool Aime_isDBAliveReturnValue = false;

static __int64 System_getResolutionReturnValue = 0;

static bool Aime_isMobileReturnValue = false;

static bool Aime_isReaderDetectedReturnValue = false;

static char Aime_sendLogReturnValue = 0;

static bool Aime_setLedErrorReturnValue = false;

static bool Aime_setLedSuccessReturnValue = false;

static bool Aime_startReturnValue = false;

static __int64 AllnetAccounting_beginPlayReturnValue = 0;

static __int64 AllnetAccounting_endPlayReturnValue = 0;

static __int64 AllnetAuth_getCountryCodeReturnValue = 0;

static __int64 AllnetAuth_getLocationIdReturnValue = 0;

static __int64 AllnetAuth_getLocationNicknamePartCountReturnValue = 0;

static __int64 AllnetAuth_getRegionCodeReturnValue = 0;

static __int64 AllnetAuth_getRegionNamePartCountReturnValue = 0;

static __int64 AllnetAuth_isDevelopReturnValue = 0;

static __int64 AllnetAuth_isGoodReturnValue = 0;

static bool ApmSystemSetting_getAdvertizeSoundReturnValue = false;

static bool ApmSystemSetting_getClosingTimesReturnValue = false;

static char ApmSystemSetting_getEmoneySettingReturnValue = 0;

static bool ApmSystemSetting_getFixedTitleReturnValue = false;

static char ApmSystemSetting_getGamePadSettingReturnValue = 0;

static bool ApmSystemSetting_getMatchingGroupReturnValue = false;

static char ApmSystemSetting_getTimeToClosingTimeReturnValue = 0;

static char ApmSystemSetting_getUiSettingReturnValue = 0;

static bool Backup_isSetupSucceededReturnValue = true;

static __int64 Backup_saveRecordReturnValue = 0;

static __int64 Backup_saveRecordByAddressReturnValue = 0;

static __int64 Backup_setupRecordsReturnValue = 0;

static bool Core_exitGameReturnValue = 0;

static __int64 Core_hookExceptionReturnValue = 0;

static bool Core_isExitNeededReturnValue = false;

static __int64 Core_resetExceptionReturnValue = 0;

static __int64 Credit_getAddableCoinReturnValue = 0;

static __int64 Credit_getCoinMultipliersReturnValue = 0;

static __int64 Credit_getCoinToCreditReturnValue = 0;

static __int64 Credit_getCreditReturnValue = 0;

static __int64 Credit_getRemainReturnValue = 0;

static __int64 Credit_isFreePlayReturnValue = 1;

static __int64 Credit_isGameCostEnoughReturnValue = 0;

static __int64 Credit_isZeroReturnValue = 0;

static __int64 Credit_payGameCostReturnValue = 0;

static __int64 Credit_resetCoinInHookReturnValue = 0;

static __int64 Credit_setCoinInHookReturnValue = 0;

static __int64 Emoney_isOpenMainWindowReturnValue = 0;

static char Emoney_launchReturnValue = 0;

static __int64 Error_isOccurredReturnValue = 0;

static __int64 Error_setDeviceLostReturnValue = 0;

static char Input_isFlipNowReturnValue = 0;

static __int64 Input_isGamepadDetectReturnValue = 0;

static char Input_isOffNowReturnValue = 0;

static char Input_isOnNowReturnValue = 0;

static __int64 Input_isOpenPewviewWindowReturnValue = 0;

static char Input_setGamepadConfigReturnValue = 0;

static void** NetworkProperty_getAddressStringReturnValue = nullptr;

static __int64 NetworkProperty_getAddressValueReturnValue = 0;

static __int64 Sequence_beginPlayReturnValue = 1;

static __int64 Sequence_clearBackupReturnValue = 0;

static __int64 Sequence_continuePlayReturnValue = 0;

static __int64 Sequence_endPlayReturnValue = 0;

static __int64 Sequence_getBookkeepingReturnValue = 0;

static __int64 Sequence_getPlayingAimeIdReturnValue = 0;

static __int64 Sequence_isAccountingPlayingReturnValue = 0;

static __int64 Sequence_isPlayingReturnValue = 0;

static __int64 Sequence_isTestReturnValue = 0;

static __int64 System_getAppRootPathReturnValue = 0;

static __int64 System_getRegionCodeReturnValue = 0;

static __int64 System_isDevelopReturnValue = 0;

static char apmGamepadConfigReturnValue = 0;

static char apmGamepadGetApmInputReturnValue = 0;

static char apmGamepadGetDeviceCapsReturnValue = 0;

static __int64 apmGamepadGetInfoReturnValue = 0;

static char apmGamepadGetRawInputReturnValue = 0;

static __int64 apmGamepadInitConfigReturnValue = 0;

static char apmGamepadIsDetectedReturnValue = 0;

static __int64 apmGamepadIsXInputDeviceReturnValue = 0;

static int apmGamepadUpdateReturnValue = 0;
#endif