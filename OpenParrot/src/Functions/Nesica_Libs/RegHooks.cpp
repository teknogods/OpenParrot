#include <StdInc.h>
#include "Functions/Types.h"
#include "Utility/InitFunction.h"
#include "Functions/Global.h"

DWORD GameResult = 0;
DWORD IOErrorCoin = 0;
DWORD IOErrorCredit = 0;
DWORD EventModeEnable = 0;
DWORD SystemType = 0;
DWORD GameKind = 0;
DWORD EventNextTime = 0;
DWORD ConditionTime = 0;
DWORD TrafficCount = 0;
DWORD LogLevel = 0;
DWORD NewsPath = 0;
DWORD EventPath = 0;
DWORD LogPath = 0;

DWORD FillDwordInformation(const char* setting, const char* subkey, DWORD defaultValue)
{
	if (strcmp(config[setting][subkey].c_str(), "1") == 0)
		return 1;
	return defaultValue;
}

LSTATUS __stdcall RegOpenKeyExAWrap(
	HKEY   hKey,
	LPCSTR lpSubKey,
	DWORD  ulOptions,
	REGSAM samDesired,
	PHKEY  phkResult
)
{
	if (hKey == HKEY_LOCAL_MACHINE)
	{
		if (stricmp(lpSubKey, "SOFTWARE\\TAITO\\NESiCAxLive") == 0)
		{
			*phkResult = (HKEY)0x8101;
			return ERROR_SUCCESS;
		}
		if (stricmp(lpSubKey, "SOFTWARE\\TAITO\\TYPEX") == 0)
		{
			*phkResult = (HKEY)0x8102;
			return ERROR_SUCCESS;
		}
	}
	return RegOpenKeyExA(hKey, lpSubKey, ulOptions, samDesired, phkResult);
}

LSTATUS __stdcall RegOpenKeyExWWrap(
	HKEY   hKey,
	LPCWSTR lpSubKey,
	DWORD  ulOptions,
	REGSAM samDesired,
	PHKEY  phkResult
)
{
	if (hKey == HKEY_LOCAL_MACHINE)
	{
		if (wcsicmp(lpSubKey, L"SOFTWARE\\TAITO\\NESiCAxLive") == 0)
		{
			*phkResult = (HKEY)0x8101;
			return ERROR_SUCCESS;
		}
		if (wcsicmp(lpSubKey, L"SOFTWARE\\TAITO\\TYPEX") == 0)
		{
			*phkResult = (HKEY)0x8102;
			return ERROR_SUCCESS;
		}
	}
	return RegOpenKeyExW(hKey, lpSubKey, ulOptions, samDesired, phkResult);
}

LSTATUS __stdcall RegCreateKeyExAWrap(
	HKEY                        hKey,
	LPCSTR                     lpSubKey,
	DWORD                       Reserved,
	LPSTR                      lpClass,
	DWORD                       dwOptions,
	REGSAM                      samDesired,
	CONST LPSECURITY_ATTRIBUTES lpSecurityAttributes,
	PHKEY                       phkResult,
	LPDWORD                     lpdwDisposition
)
{
	if (hKey == HKEY_LOCAL_MACHINE)
	{
		if (stricmp(lpSubKey, "SOFTWARE\\TAITO\\NESiCAxLive") == 0)
		{
			*phkResult = (HKEY)0x8101;
			return ERROR_SUCCESS;
		}
		if (stricmp(lpSubKey, "SOFTWARE\\TAITO\\TYPEX") == 0)
		{
			*phkResult = (HKEY)0x8102;
			return ERROR_SUCCESS;
		}
	}
	return RegCreateKeyExA(hKey, lpSubKey, Reserved, lpClass, dwOptions, samDesired, lpSecurityAttributes, phkResult, lpdwDisposition);
}

LSTATUS __stdcall RegCreateKeyExWWrap(
	HKEY                        hKey,
	LPCWSTR                     lpSubKey,
	DWORD                       Reserved,
	LPWSTR                      lpClass,
	DWORD                       dwOptions,
	REGSAM                      samDesired,
	CONST LPSECURITY_ATTRIBUTES lpSecurityAttributes,
	PHKEY                       phkResult,
	LPDWORD                     lpdwDisposition
)
{
	if (hKey == HKEY_LOCAL_MACHINE)
	{
		if (wcsicmp(lpSubKey, L"SOFTWARE\\TAITO\\NESiCAxLive") == 0)
		{
			*phkResult = (HKEY)0x8101;
			return ERROR_SUCCESS;
		}
		if (wcsicmp(lpSubKey, L"SOFTWARE\\TAITO\\TYPEX") == 0)
		{
			*phkResult = (HKEY)0x8102;
			return ERROR_SUCCESS;
		}
	}
	return RegCreateKeyExW(hKey, lpSubKey, Reserved, lpClass, dwOptions, samDesired, lpSecurityAttributes, phkResult, lpdwDisposition);
}

char tempBuf[256];

// TODO: DOCUMENT ALL ERROR CODES!
void PromptError(DWORD errorCode, int errorType)
{
	memset(tempBuf, 0, 256);
	switch (errorType)
	{
	case 0:
		sprintf(tempBuf, "Game sent NESiCA GameResult error code to %08X to launcher!", errorCode);
		break;
	case 1:
		sprintf(tempBuf, "Game sent NESiCA IOErrorCoin error code to %08X to launcher!", errorCode);
		break;
	case 2:
		sprintf(tempBuf, "Game sent NESiCA IOErrorCredit error code to %08X to launcher!", errorCode);
		break;
	}
	MessageBoxA(0, tempBuf, "NESiCA ERROR!", 0);
}

LSTATUS __stdcall RegSetValueExAWrap(
	HKEY       hKey,
	LPCSTR     lpValueName,
	DWORD      Reserved,
	DWORD      dwType,
	CONST BYTE* lpData,
	DWORD      cbData
)
{
	// These are for the NESiCA Launcher and no need to handle them in any way!
	if (hKey == (HKEY)0x8101)
	{
		if (stricmp(lpValueName, "GameResult") == 0)
		{
			GameResult = *(DWORD*)lpData;
			if (GameResult != 0)
			{
				PromptError(GameResult, 0);
			}
		}
		else if (stricmp(lpValueName, "IOErrorCoin") == 0)
		{
			IOErrorCoin = *(DWORD*)lpData;;
			if (IOErrorCoin != 0)
			{
				PromptError(IOErrorCoin, 1);
			}
		}
		else if (stricmp(lpValueName, "IOErrorCredit") == 0)
		{
			IOErrorCredit = *(DWORD*)lpData;;
			if (IOErrorCredit != 0)
			{
				PromptError(IOErrorCredit, 2);
			}
		}
		else if (stricmp(lpValueName, "EventModeEnable") == 0)
		{
			// USED IN SKULLGIRLS
			EventModeEnable = *(DWORD*)lpData;
		}
		else
		{
			MessageBoxA(0, "Unknown RegSetValueExA for NESiCA, Please contact devs!", "Error unhandled registry change", 0);
		}
		return ERROR_SUCCESS;
	}

	if (hKey == (HKEY)0x8102)
	{
		MessageBoxA(0, "RegSetValueExA for TYPEX, Please contact devs!", "Error unhandled registry change", 0);
		return ERROR_FILE_NOT_FOUND;
	}

	return RegSetValueExA(hKey, lpValueName, Reserved, dwType, lpData, cbData);
}

LSTATUS __stdcall RegSetValueExWWrap(
	HKEY       hKey,
	LPCWSTR     lpValueName,
	DWORD      Reserved,
	DWORD      dwType,
	CONST BYTE* lpData,
	DWORD      cbData
)
{
	// These are for the NESiCA Launcher and no need to handle them in any way!
	if (hKey == (HKEY)0x8101)
	{
		if (wcsicmp(lpValueName, L"GameResult") == 0)
		{
			GameResult = *(DWORD*)lpData;
			if (GameResult != 0)
			{
				PromptError(GameResult, 0);
			}
		}
		else if (wcsicmp(lpValueName, L"IOErrorCoin") == 0)
		{
			IOErrorCoin = *(DWORD*)lpData;;
			if (IOErrorCoin != 0)
			{
				PromptError(IOErrorCoin, 1);
			}
		}
		else if (wcsicmp(lpValueName, L"IOErrorCredit") == 0)
		{
			IOErrorCredit = *(DWORD*)lpData;;
			if (IOErrorCredit != 0)
			{
				PromptError(IOErrorCredit, 2);
			}
		}
		else if (wcsicmp(lpValueName, L"EventModeEnable") == 0)
		{
			// USED IN SKULLGIRLS
			EventModeEnable = *(DWORD*)lpData;
		}
		else
		{
			MessageBoxA(0, "Unknown RegSetValueExW for NESiCA, Please contact devs!", "Error unhandled registry change", 0);
		}
		return ERROR_SUCCESS;
	}

	if (hKey == (HKEY)0x8102)
	{
		MessageBoxA(0, "RegSetValueExW for TYPEX, Please contact devs!", "Error unhandled registry change", 0);
		return ERROR_FILE_NOT_FOUND;
	}

	return RegSetValueExW(hKey, lpValueName, Reserved, dwType, lpData, cbData);
}

LSTATUS __stdcall RegQueryValueExAWrap(
	HKEY                              hKey,
	LPCSTR                           lpValueName,
	LPDWORD                           lpReserved,
	LPDWORD                           lpType,
	__out_data_source(REGISTRY)LPBYTE lpData,
	LPDWORD                           lpcbData
)
{
	// SOFTWARE\\TAITO\\TYPEX
	if (hKey == (HKEY)0x8102)
	{
		if (lpData == nullptr)
		{
			*lpcbData = 4;
			return ERROR_SUCCESS;
		}
		if (stricmp(lpValueName, "ConditionTime") == 0) // REG_DWORD
		{
			*lpData = FillDwordInformation("TYPEX", "ConditionTime", 300); // UNK
			*lpcbData = 4;
		}
		else if (stricmp(lpValueName, "EventNextTime") == 0) // REG_DWORD
		{
			*lpData = FillDwordInformation("TYPEX", "EventNextTime", 900); // UNK
			*lpcbData = 4;
		}
		else if (stricmp(lpValueName, "EventPath") == 0) // REG_SZ
		{
			// TODO
			return ERROR_FILE_NOT_FOUND;
		}
		else if (stricmp(lpValueName, "GameKind") == 0) // REG_DWORD
		{
			// TODO: ADD EACH AND EVERY SINGLE TYPE X GAMEKIND HERE!
			*lpData = FillDwordInformation("TYPEX", "GameKind", 1234); // UNK
			*lpcbData = 4;
		}
		else if (stricmp(lpValueName, "LogLevel") == 0) // REG_DWORD
		{
			*lpData = FillDwordInformation("TYPEX", "LogLevel", 0); // UNK
			*lpcbData = 4;
		}
		else if (stricmp(lpValueName, "LogPath") == 0) // REG_SZ
		{
			// TODO
			return ERROR_FILE_NOT_FOUND;
		}
		else if (stricmp(lpValueName, "NewsPath") == 0) // REG_SZ
		{
			// TODO
			return ERROR_FILE_NOT_FOUND;
		}
		else if (stricmp(lpValueName, "TrafficCount") == 0) // REG_DWORD
		{
			*lpData = FillDwordInformation("TYPEX", "TrafficCount", 2); // UNK
			*lpcbData = 4;
		}
		else if (stricmp(lpValueName, "UpdateStep") == 0) // REG_DWORD
		{
			*lpData = FillDwordInformation("TYPEX", "UpdateStep", 0); // UNK
			*lpcbData = 4;
		}
		else if (stricmp(lpValueName, "Country") == 0) // REG_DWORD
		{
			*(DWORD*)lpData = FillDwordInformation("TYPEX", "Country", 1); // UNK
			*lpcbData = 1;
		}
		else
		{
			MessageBoxA(0, "UNKNOWN REG QUERY FROM SOFTWARE\\TAITO\\TYPEX, contact devs!", "Error", 0);
			return ERROR_FILE_NOT_FOUND;
		}
		return ERROR_SUCCESS;
	}

	// SOFTWARE\\TAITO\\NESiCAxLive
	if (hKey == (HKEY)0x8101)
	{
		if (lpData == nullptr)
		{
			*lpcbData = 4;
			return ERROR_SUCCESS;
		}
		if (stricmp(lpValueName, "CoinCredit") == 0) // REG_DWORD
		{
			*lpData = FillDwordInformation("NESiCA", "CoinCredit", 0); // 0 = FREE PLAY
			*lpcbData = 4;
		}
		else if (stricmp(lpValueName, "Resolution") == 0) // REG_DWORD
		{
			*lpData = FillDwordInformation("NESiCA", "Resolution", 1); // 0 = SD / 1 = HD
			*lpcbData = 4;
		}
		else if (stricmp(lpValueName, "ScreenVertical") == 0) // REG_DWORD
		{
			*lpData = FillDwordInformation("NESiCA", "ScreenVertical", 0); // 0 = Horizontal / 1 = Vertical
			*lpcbData = 4;
		}
		else if (stricmp(lpValueName, "EventModeEnable") == 0) // REG_DWORD
		{
			*lpData = FillDwordInformation("NESiCA", "EventModeEnable", EventModeEnable); // 0 = DISABLE / 1 = ENABLE
			*lpcbData = 4;
		}
		else if (stricmp(lpValueName, "UserSelectEnable") == 0) // REG_DWORD
		{
			*lpData = FillDwordInformation("NESiCA", "UserSelectEnable", 0); // 0 = DISABLE / 1 = ENABLE
			*lpcbData = 4;
		}
		else if (stricmp(lpValueName, "GameResult") == 0) // REG_DWORD
		{
			*lpData = FillDwordInformation("NESiCA", "GameResult", GameResult); // UNK
			*lpcbData = 4;
		}
		else if (stricmp(lpValueName, "IOErrorCoin") == 0) // REG_DWORD
		{
			*lpData = FillDwordInformation("NESiCA", "IOErrorCoin", IOErrorCoin); // UNK
			*lpcbData = 4;
		}
		else if (stricmp(lpValueName, "IOErrorCredit") == 0) // REG_DWORD
		{
			*lpData = FillDwordInformation("NESiCA", "IOErrorCredit", IOErrorCredit); // UNK
			*lpcbData = 4;
		}
		else if (stricmp(lpValueName, "SystemType") == 0) // REG_DWORD
		{
			*lpData = FillDwordInformation("NESiCA", "SystemType", SystemType); // UNK
			*lpcbData = 4;
		}
		else if (stricmp(lpValueName, "ConditionTime") == 0) // REG_DWORD
		{
			*lpData = FillDwordInformation("NESiCA", "ConditionTime", 300); // UNK
			*lpcbData = 4;
		}
		else if (stricmp(lpValueName, "EventNextTime") == 0) // REG_DWORD
		{
			*lpData = FillDwordInformation("NESiCA", "EventNextTime", 900); // UNK
			*lpcbData = 4;
		}
		else if (stricmp(lpValueName, "EventPath") == 0) // REG_SZ
		{
			// TODO
			return ERROR_FILE_NOT_FOUND;
		}
		else if (stricmp(lpValueName, "GameKind") == 0) // REG_DWORD
		{
			// TODO: ADD EACH AND EVERY SINGLE TYPE X GAMEKIND HERE!
			*lpData = FillDwordInformation("NESiCA", "GameKind", 1234); // UNK
			*lpcbData = 4;
		}
		else if (stricmp(lpValueName, "LogLevel") == 0) // REG_DWORD
		{
			*lpData = FillDwordInformation("NESiCA", "LogLevel", 0); // UNK
			*lpcbData = 4;
		}
		else if (stricmp(lpValueName, "LogPath") == 0) // REG_SZ
		{
			// TODO
			return ERROR_FILE_NOT_FOUND;
		}
		else if (stricmp(lpValueName, "NewsPath") == 0) // REG_SZ
		{
			// TODO
			return ERROR_FILE_NOT_FOUND;
		}
		else if (stricmp(lpValueName, "TrafficCount") == 0) // REG_DWORD
		{
			*lpData = FillDwordInformation("NESiCA", "TrafficCount", 2); // UNK
			*lpcbData = 4;
		}
		else if (stricmp(lpValueName, "UpdateStep") == 0) // REG_DWORD
		{
			*lpData = FillDwordInformation("NESiCA", "UpdateStep", 0); // UNK
			*lpcbData = 4;
		}
		else if (stricmp(lpValueName, "Country") == 0) // REG_DWORD
		{
			*(DWORD*)lpData = FillDwordInformation("NESiCA", "Country", 1); // UNK
			*lpcbData = 1;
		}
		else if (stricmp(lpValueName, "AppVer") == 0) // REG_DWORD
		{
			*(DWORD*)lpData = FillDwordInformation("NESiCA", "AppVer", 1); // UNK
			*lpcbData = 1;
		}
		else
		{
			MessageBoxA(0, "UNKNOWN REG QUERY FROM SOFTWARE\\TAITO\\NESiCAxLive, contact devs!", "Error", 0);
			return ERROR_FILE_NOT_FOUND;
		}
		return ERROR_SUCCESS;
	}

	return RegQueryValueExA(hKey, lpValueName, lpReserved, lpType, lpData, lpcbData);
}

LSTATUS __stdcall RegQueryValueExWWrap(
	HKEY                              hKey,
	LPCWSTR                           lpValueName,
	LPDWORD                           lpReserved,
	LPDWORD                           lpType,
	__out_data_source(REGISTRY)LPBYTE lpData,
	LPDWORD                           lpcbData
)
{
	// SOFTWARE\\TAITO\\TYPEX
	if (hKey == (HKEY)0x8102)
	{
		if (lpData == nullptr)
		{
			*lpcbData = 4;
			return ERROR_MORE_DATA;
		}
		if (wcsicmp(lpValueName, L"ConditionTime") == 0) // REG_DWORD
		{
			*lpData = FillDwordInformation("TYPEX", "ConditionTime", 300); // UNK
			*lpcbData = 4;
		}
		else if (wcsicmp(lpValueName, L"EventNextTime") == 0) // REG_DWORD
		{
			*lpData = FillDwordInformation("TYPEX", "EventNextTime", 900); // UNK
			*lpcbData = 4;
		}
		else if (wcsicmp(lpValueName, L"EventPath") == 0) // REG_SZ
		{
			// TODO
			return ERROR_FILE_NOT_FOUND;
		}
		else if (wcsicmp(lpValueName, L"GameKind") == 0) // REG_DWORD
		{
			// TODO: ADD EACH AND EVERY SINGLE TYPE X GAMEKIND HERE!
			*lpData = FillDwordInformation("TYPEX", "GameKind", 1234); // UNK
			*lpcbData = 4;
		}
		else if (wcsicmp(lpValueName, L"LogLevel") == 0) // REG_DWORD
		{
			*lpData = FillDwordInformation("TYPEX", "LogLevel", 0); // UNK
			*lpcbData = 4;
		}
		else if (wcsicmp(lpValueName, L"LogPath") == 0) // REG_SZ
		{
			// TODO
			return ERROR_FILE_NOT_FOUND;
		}
		else if (wcsicmp(lpValueName, L"NewsPath") == 0) // REG_SZ
		{
			// TODO
			return ERROR_FILE_NOT_FOUND;
		}
		else if (wcsicmp(lpValueName, L"TrafficCount") == 0) // REG_DWORD
		{
			*lpData = FillDwordInformation("TYPEX", "TrafficCount", 2); // UNK
			*lpcbData = 4;
		}
		else if (wcsicmp(lpValueName, L"UpdateStep") == 0) // REG_DWORD
		{
			*lpData = FillDwordInformation("TYPEX", "UpdateStep", 0); // UNK
			*lpcbData = 4;
		}
		else if (wcsicmp(lpValueName, L"Country") == 0) // REG_DWORD
		{
			*(DWORD*)lpData = FillDwordInformation("TYPEX", "Country", 0); // UNK
			*lpcbData = 4;
		}
		else
		{
			MessageBoxA(0, "UNKNOWN REG QUERY FROM SOFTWARE\\TAITO\\TYPEX, contact devs!", "Error", 0);
			return ERROR_FILE_NOT_FOUND;
		}
		return ERROR_SUCCESS;
	}

	// SOFTWARE\\TAITO\\NESiCAxLive
	if (hKey == (HKEY)0x8101)
	{
		if (lpData == nullptr)
		{
			*lpcbData = 4;
			return ERROR_MORE_DATA;
		}
		if (wcsicmp(lpValueName, L"CoinCredit") == 0) // REG_DWORD
		{
			*lpData = FillDwordInformation("NESiCA", "CoinCredit", 0); // 0 = FREE PLAY
			*lpcbData = 4;
		}
		else if (wcsicmp(lpValueName, L"Resolution") == 0) // REG_DWORD
		{
			*lpData = FillDwordInformation("NESiCA", "Resolution", 1); // 0 = SD / 1 = HD
			*lpcbData = 4;
		}
		else if (wcsicmp(lpValueName, L"ScreenVertical") == 0) // REG_DWORD
		{
			*lpData = FillDwordInformation("NESiCA", "ScreenVertical", 0); // 0 = Horizontal / 1 = Vertical
			*lpcbData = 4;
		}
		else if (wcsicmp(lpValueName, L"EventModeEnable") == 0) // REG_DWORD
		{
			*lpData = FillDwordInformation("NESiCA", "EventModeEnable", EventModeEnable); // 0 = DISABLE / 1 = ENABLE
			*lpcbData = 4;
		}
		else if (wcsicmp(lpValueName, L"UserSelectEnable") == 0) // REG_DWORD
		{
			*lpData = FillDwordInformation("NESiCA", "UserSelectEnable", 0); // 0 = DISABLE / 1 = ENABLE
			*lpcbData = 4;
		}
		else if (wcsicmp(lpValueName, L"GameResult") == 0) // REG_DWORD
		{
			*lpData = FillDwordInformation("NESiCA", "GameResult", GameResult); // UNK
			*lpcbData = 4;
		}
		else if (wcsicmp(lpValueName, L"IOErrorCoin") == 0) // REG_DWORD
		{
			*lpData = FillDwordInformation("NESiCA", "IOErrorCoin", IOErrorCoin); // UNK
			*lpcbData = 4;
		}
		else if (wcsicmp(lpValueName, L"IOErrorCredit") == 0) // REG_DWORD
		{
			*lpData = FillDwordInformation("NESiCA", "IOErrorCredit", IOErrorCredit); // UNK
			*lpcbData = 4;
		}
		else if (wcsicmp(lpValueName, L"SystemType") == 0) // REG_DWORD
		{
			*lpData = FillDwordInformation("NESiCA", "SystemType", SystemType);; // UNK
			*lpcbData = 4;
		}
		else if (wcsicmp(lpValueName, L"ConditionTime") == 0) // REG_DWORD
		{
			*lpData = FillDwordInformation("NESiCA", "ConditionTime", 300); // UNK
			*lpcbData = 4;
		}
		else if (wcsicmp(lpValueName, L"EventNextTime") == 0) // REG_DWORD
		{
			*lpData = FillDwordInformation("NESiCA", "EventNextTime", 900); // UNK
			*lpcbData = 4;
		}
		else if (wcsicmp(lpValueName, L"EventPath") == 0) // REG_SZ
		{
			// TODO
			return ERROR_FILE_NOT_FOUND;
		}
		else if (wcsicmp(lpValueName, L"GameKind") == 0) // REG_DWORD
		{
			// TODO: ADD EACH AND EVERY SINGLE TYPE X GAMEKIND HERE!
			*lpData = FillDwordInformation("NESiCA", "GameKind", 1234); // UNK
			*lpcbData = 4;
		}
		else if (wcsicmp(lpValueName, L"LogLevel") == 0) // REG_DWORD
		{
			*lpData = FillDwordInformation("NESiCA", "LogLevel", 0); // UNK
			*lpcbData = 4;
		}
		else if (wcsicmp(lpValueName, L"LogPath") == 0) // REG_SZ
		{
			// TODO
			return ERROR_FILE_NOT_FOUND;
		}
		else if (wcsicmp(lpValueName, L"NewsPath") == 0) // REG_SZ
		{
			// TODO
			return ERROR_FILE_NOT_FOUND;
		}
		else if (wcsicmp(lpValueName, L"TrafficCount") == 0) // REG_DWORD
		{
			*lpData = FillDwordInformation("NESiCA", "TrafficCount", 2); // UNK
			*lpcbData = 4;
		}
		else if (wcsicmp(lpValueName, L"UpdateStep") == 0) // REG_DWORD
		{
			*lpData = FillDwordInformation("NESiCA", "UpdateStep", 0); // UNK
			*lpcbData = 4;
		}
		else if (wcsicmp(lpValueName, L"Country") == 0) // REG_DWORD
		{
			*(DWORD*)lpData = FillDwordInformation("NESiCA", "Country", 1); // UNK
			*lpcbData = 4;
		}
		else if (wcsicmp(lpValueName, L"AppVer") == 0) // REG_DWORD
		{
			*(DWORD*)lpData = FillDwordInformation("NESiCA", "Appver", 1); // UNK
			*lpcbData = 4;
		}
		else
		{
			MessageBoxA(0, "UNKNOWN REG QUERY FROM SOFTWARE\\TAITO\\NESiCAxLive, contact devs!", "Error", 0);
			return ERROR_FILE_NOT_FOUND;
		}
		return ERROR_SUCCESS;
	}

	return RegQueryValueExW(hKey, lpValueName, lpReserved, lpType, lpData, lpcbData);
}

LSTATUS __stdcall RegCloseKeyWrap(
	HKEY hKey
)
{
	if (hKey == (HKEY)0x8101)
		return ERROR_SUCCESS;
	if (hKey == (HKEY)0x8102)
		return ERROR_SUCCESS;
	return RegCloseKey(hKey);
}

void init_RegHooks()
{ 
	// ASCII
	iatHook("advapi32.dll", RegCreateKeyExAWrap, "RegCreateKeyExA");
	iatHook("advapi32.dll", RegOpenKeyExAWrap, "RegOpenKeyExA");
	iatHook("advapi32.dll", RegQueryValueExAWrap, "RegQueryValueExA");
	iatHook("advapi32.dll", RegSetValueExAWrap, "RegSetValueExA");
	// Unicode
	iatHook("advapi32.dll", RegCreateKeyExWWrap, "RegCreateKeyExW");
	iatHook("advapi32.dll", RegOpenKeyExWWrap, "RegOpenKeyExW");
	iatHook("advapi32.dll", RegQueryValueExWWrap, "RegQueryValueExW");
	iatHook("advapi32.dll", RegSetValueExWWrap, "RegSetValueExW");
	iatHook("advapi32.dll", RegCloseKeyWrap, "RegCloseKey");
}