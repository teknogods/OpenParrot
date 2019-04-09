#include <StdInc.h>
#include "Functions/Types.h"
#include "Utility/InitFunction.h"
#include "Functions/Global.h"

DWORD GameResult = 0;
DWORD IOErrorCoin = 0;
DWORD IOErrorCredit = 0;
DWORD EventModeEnable = 0;
DWORD SystemType = 0;

DWORD FillDwordInformation(const char *setting, const char *subkey, DWORD defaultValue)
{
	if (strcmp(config[setting][subkey].c_str(), "1") == 0)
		return 1;
	return defaultValue;
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
	if(hKey == HKEY_LOCAL_MACHINE)
	{
		if(wcscmp(lpSubKey, L"SOFTWARE\\TAITO\\NESiCAxLive") == 0)
		{
			*phkResult = (HKEY)0x8101;
			return ERROR_SUCCESS;
		}
		if (wcscmp(lpSubKey, L"SOFTWARE\\TAITO\\TYPEX") == 0)
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

LSTATUS __stdcall RegSetValueExWWrap(
	HKEY       hKey,
	LPCWSTR     lpValueName,
	DWORD      Reserved,
	DWORD      dwType,
	CONST BYTE *lpData,
	DWORD      cbData
)
{
	// These are for the NESiCA Launcher and no need to handle them in any way!
	if(hKey == (HKEY)0x8101)
	{
		if (wcscmp(lpValueName, L"GameResult") == 0)
		{
			GameResult = *(DWORD *)lpData;
			if(GameResult != 0)
			{
				PromptError(GameResult, 0);
			}
		}
		else if(wcscmp(lpValueName, L"IOErrorCoin") == 0)
		{
			IOErrorCoin = *(DWORD *)lpData;;
			if(IOErrorCoin != 0)
			{
				PromptError(IOErrorCoin, 1);
			}
		}
		else if (wcscmp(lpValueName, L"IOErrorCredit") == 0)
		{
			IOErrorCredit = *(DWORD *)lpData;;
			if(IOErrorCredit != 0)
			{
				PromptError(IOErrorCredit, 2);
			}
		}
		else if (wcscmp(lpValueName, L"EventModeEnable") == 0)
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

	if(hKey == (HKEY)0x8102)
	{
		MessageBoxA(0, "RegSetValueExW for TYPEX, Please contact devs!", "Error unhandled registry change", 0);
		return ERROR_FILE_NOT_FOUND;
	}

	return RegSetValueExW(hKey, lpValueName, Reserved, dwType, lpData, cbData);
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
		if (wcscmp(lpValueName, L"ConditionTime") == 0) // REG_DWORD
		{
			*lpData = FillDwordInformation("TYPEX", "ConditionTime", 300); // UNK
			*lpcbData = 4;
		}
		else if (wcscmp(lpValueName, L"EventNextTime") == 0) // REG_DWORD
		{
			*lpData = FillDwordInformation("TYPEX", "EventNextTime", 900); // UNK
			*lpcbData = 4;
		}
		else if (wcscmp(lpValueName, L"EventPath") == 0) // REG_SZ
		{
			// TODO
			return ERROR_FILE_NOT_FOUND;
		}
		else if (wcscmp(lpValueName, L"GameKind") == 0) // REG_DWORD
		{
			// TODO: ADD EACH AND EVERY SINGLE TYPE X GAMEKIND HERE!
			*lpData = FillDwordInformation("TYPEX", "GameKind", 1234); // UNK
			*lpcbData = 4;
		}
		else if (wcscmp(lpValueName, L"LogLevel") == 0) // REG_DWORD
		{
			*lpData = FillDwordInformation("TYPEX", "LogLevel", 0); // UNK
			*lpcbData = 4;
		}
		else if (wcscmp(lpValueName, L"LogPath") == 0) // REG_SZ
		{
			// TODO
			return ERROR_FILE_NOT_FOUND;
		}
		else if (wcscmp(lpValueName, L"NewsPath") == 0) // REG_SZ
		{
			// TODO
			return ERROR_FILE_NOT_FOUND;
		}
		else if (wcscmp(lpValueName, L"TrafficCount") == 0) // REG_DWORD
		{
			*lpData = FillDwordInformation("TYPEX", "TrafficCount", 2); // UNK
			*lpcbData = 4;
		}
		else if (wcscmp(lpValueName, L"UpdateStep") == 0) // REG_DWORD
		{
			*lpData = FillDwordInformation("TYPEX", "UpdateStep", 0); // UNK
			*lpcbData = 4;
		}
		else
		{
			MessageBoxA(0, "UNKNOWN REG QUERY FROM SOFTWWARE\\TAITO\\TYPEX, contact devs!", "Error", 0);
			return ERROR_FILE_NOT_FOUND;
		}

	}

	// SOFTWARE\\TAITO\\NESiCAxLive
	if(hKey == (HKEY)0x8101)
	{
		if(wcscmp(lpValueName, L"CoinCredit") == 0) // REG_DWORD
		{
			*lpData = FillDwordInformation("NESiCA", "CoinCredit", 0); // 0 = FREE PLAY
			*lpcbData = 4;
		}
		else if (wcscmp(lpValueName, L"Resolution") == 0) // REG_DWORD
		{
			*lpData = FillDwordInformation("NESiCA", "Resolution", 1); // 0 = SD / 1 = HD
			*lpcbData = 4;
		}
		else if (wcscmp(lpValueName, L"ScreenVertical") == 0) // REG_DWORD
		{
			*lpData = FillDwordInformation("NESiCA", "ScreenVertical", 0); // 0 = Horizontal / 1 = Vertical
			*lpcbData = 4;
		}
		else if (wcscmp(lpValueName, L"EventModeEnable") == 0) // REG_DWORD
		{
			*lpData = FillDwordInformation("NESiCA", "EventModeEnable", EventModeEnable); // 0 = DISABLE / 1 = ENABLE
			*lpcbData = 4;
		}
		else if (wcscmp(lpValueName, L"UserSelectEnable") == 0) // REG_DWORD
		{
			*lpData = FillDwordInformation("NESiCA", "UserSelectEnable", 0); // 0 = DISABLE / 1 = ENABLE
			*lpcbData = 4;
		}
		else if (wcscmp(lpValueName, L"GameResult") == 0) // REG_DWORD
		{
			*lpData = FillDwordInformation("NESiCA", "GameResult", GameResult); // UNK
			*lpcbData = 4;
		}
		else if (wcscmp(lpValueName, L"IOErrorCoin") == 0) // REG_DWORD
		{
			*lpData = FillDwordInformation("NESiCA", "IOErrorCoin", IOErrorCoin); // UNK
			*lpcbData = 4;
		}
		else if (wcscmp(lpValueName, L"IOErrorCredit") == 0) // REG_DWORD
		{
			*lpData = FillDwordInformation("NESiCA", "IOErrorCredit", IOErrorCredit); // UNK
			*lpcbData = 4;
		}
		else if (wcscmp(lpValueName, L"SystemType") == 0) // REG_DWORD
		{
			*lpData = FillDwordInformation("NESiCA", "SystemType", SystemType);; // UNK
			*lpcbData = 4;
		}
		else
		{
			MessageBoxA(0, "UNKNOWN REG QUERY FROM SOFTWWARE\\TAITO\\NESiCAxLive, contact devs!", "Error", 0);
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
	iatHook("advapi32.dll", RegCreateKeyExWWrap, "RegCreateKeyExW");
	iatHook("advapi32.dll", RegQueryValueExWWrap, "RegQueryValueExW");
	iatHook("advapi32.dll", RegCloseKeyWrap, "RegCloseKey");
	iatHook("advapi32.dll", RegSetValueExWWrap, "RegSetValueExW");	
}