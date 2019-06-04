#include <StdInc.h>
#include "Functions/Types.h"
#include "Utility/InitFunction.h"
#include "Functions/Global.h"

LSTATUS (__stdcall *orig_RegOpenKeyExA)(
	HKEY   hKey,
	LPCSTR lpSubKey,
	DWORD  ulOptions,
	REGSAM samDesired,
	PHKEY  phkResult
);

LSTATUS __stdcall RegOpenKeyExAGlobalWrap(
	HKEY   hKey,
	LPCSTR lpSubKey,
	DWORD  ulOptions,
	REGSAM samDesired,
	PHKEY  phkResult
)
{
	return orig_RegOpenKeyExA(hKey, lpSubKey, ulOptions, samDesired, phkResult);
}

LSTATUS (__stdcall *orig_RegOpenKeyExW)(
	HKEY   hKey,
	LPCWSTR lpSubKey,
	DWORD  ulOptions,
	REGSAM samDesired,
	PHKEY  phkResult
);

LSTATUS __stdcall RegOpenKeyExWGlobalWrap(
	HKEY   hKey,
	LPCWSTR lpSubKey,
	DWORD  ulOptions,
	REGSAM samDesired,
	PHKEY  phkResult
)
{
	return orig_RegOpenKeyExW(hKey, lpSubKey, ulOptions, samDesired, phkResult);
}

LSTATUS (__stdcall *orig_RegCreateKeyExA)(
	HKEY                        hKey,
	LPCSTR                     lpSubKey,
	DWORD                       Reserved,
	LPSTR                      lpClass,
	DWORD                       dwOptions,
	REGSAM                      samDesired,
	CONST LPSECURITY_ATTRIBUTES lpSecurityAttributes,
	PHKEY                       phkResult,
	LPDWORD                     lpdwDisposition
);

LSTATUS __stdcall RegCreateKeyExAGlobalWrap(
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
	return orig_RegCreateKeyExA(hKey, lpSubKey, Reserved, lpClass, dwOptions, samDesired, lpSecurityAttributes, phkResult, lpdwDisposition);
}

LSTATUS (__stdcall *orig_RegCreateKeyExW)(
	HKEY                        hKey,
	LPCWSTR                     lpSubKey,
	DWORD                       Reserved,
	LPWSTR                      lpClass,
	DWORD                       dwOptions,
	REGSAM                      samDesired,
	CONST LPSECURITY_ATTRIBUTES lpSecurityAttributes,
	PHKEY                       phkResult,
	LPDWORD                     lpdwDisposition
);

LSTATUS __stdcall RegCreateKeyExWGlobalWrap(
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
	return orig_RegCreateKeyExW(hKey, lpSubKey, Reserved, lpClass, dwOptions, samDesired, lpSecurityAttributes, phkResult, lpdwDisposition);
}

LSTATUS (__stdcall *orig_RegSetValueExA)(
	HKEY       hKey,
	LPCSTR     lpValueName,
	DWORD      Reserved,
	DWORD      dwType,
	CONST BYTE* lpData,
	DWORD      cbData
);

LSTATUS __stdcall RegSetValueExAGlobalWrap(
	HKEY       hKey,
	LPCSTR     lpValueName,
	DWORD      Reserved,
	DWORD      dwType,
	CONST BYTE *lpData,
	DWORD      cbData
)
{
	return orig_RegSetValueExA(hKey, lpValueName, Reserved, dwType, lpData, cbData);
}

LSTATUS (__stdcall *orig_RegSetValueExW)(
	HKEY       hKey,
	LPCWSTR     lpValueName,
	DWORD      Reserved,
	DWORD      dwType,
	CONST BYTE* lpData,
	DWORD      cbData
);

LSTATUS __stdcall RegSetValueExWGlobalWrap(
	HKEY       hKey,
	LPCWSTR     lpValueName,
	DWORD      Reserved,
	DWORD      dwType,
	CONST BYTE *lpData,
	DWORD      cbData
)
{
	return orig_RegSetValueExW(hKey, lpValueName, Reserved, dwType, lpData, cbData);
}

LSTATUS (__stdcall *orig_RegQueryValueExA)(
	HKEY                              hKey,
	LPCSTR                           lpValueName,
	LPDWORD                           lpReserved,
	LPDWORD                           lpType,
	__out_data_source(REGISTRY)LPBYTE lpData,
	LPDWORD                           lpcbData
);

LSTATUS __stdcall RegQueryValueExAGlobalWrap(
	HKEY                              hKey,
	LPCSTR                           lpValueName,
	LPDWORD                           lpReserved,
	LPDWORD                           lpType,
	__out_data_source(REGISTRY)LPBYTE lpData,
	LPDWORD                           lpcbData
)
{
	return orig_RegQueryValueExA(hKey, lpValueName, lpReserved, lpType, lpData, lpcbData);
}

LSTATUS (__stdcall *orig_RegQueryValueExW)(
	HKEY                              hKey,
	LPCWSTR                           lpValueName,
	LPDWORD                           lpReserved,
	LPDWORD                           lpType,
	__out_data_source(REGISTRY)LPBYTE lpData,
	LPDWORD                           lpcbData
);

LSTATUS __stdcall RegQueryValueExWGlobalWrap(
	HKEY                              hKey,
	LPCWSTR                           lpValueName,
	LPDWORD                           lpReserved,
	LPDWORD                           lpType,
	__out_data_source(REGISTRY)LPBYTE lpData,
	LPDWORD                           lpcbData
)
{
	return orig_RegQueryValueExW(hKey, lpValueName, lpReserved, lpType, lpData, lpcbData);
}

LSTATUS(__stdcall* orig_RegCloseKey)(
	HKEY hKey
	);

LSTATUS __stdcall RegCloseKeyGlobalWrap(
	HKEY hKey
)
{
	return orig_RegCloseKey(hKey);
}

void init_GlobalRegHooks()
{
	MH_Initialize();
	// ASCII
	MH_CreateHookApi(L"advapi32.dll", "RegCreateKeyExA", &RegCreateKeyExAGlobalWrap, (void**)&orig_RegCreateKeyExA);
	MH_CreateHookApi(L"advapi32.dll", "RegOpenKeyExA", &RegOpenKeyExAGlobalWrap, (void**)&orig_RegOpenKeyExA);
	MH_CreateHookApi(L"advapi32.dll", "RegQueryValueExA", &RegQueryValueExAGlobalWrap, (void**)&orig_RegQueryValueExA);
	MH_CreateHookApi(L"advapi32.dll", "RegSetValueExA", &RegSetValueExAGlobalWrap, (void**)&orig_RegSetValueExA);
	// Unicode
	MH_CreateHookApi(L"advapi32.dll", "RegCreateKeyExW", &RegCreateKeyExWGlobalWrap, (void**)&orig_RegCreateKeyExW);
	MH_CreateHookApi(L"advapi32.dll", "RegOpenKeyExW", &RegOpenKeyExWGlobalWrap, (void**)&orig_RegOpenKeyExW);
	MH_CreateHookApi(L"advapi32.dll", "RegQueryValueExW", &RegQueryValueExWGlobalWrap, (void**)&orig_RegQueryValueExW);
	MH_CreateHookApi(L"advapi32.dll", "RegSetValueExW", &RegSetValueExWGlobalWrap, (void**)&orig_RegSetValueExW);
	// Close
	MH_CreateHookApi(L"advapi32.dll", "RegCloseKey", &RegCloseKeyGlobalWrap, (void**)&orig_RegCloseKey);
	MH_EnableHook(MH_ALL_HOOKS);
}