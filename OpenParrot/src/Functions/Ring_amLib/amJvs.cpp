#include <StdInc.h>
#include <Utility/InitFunction.h>
#include <Utility/GameDetect.h>
#include "amJvs.h"
#include <Functions/Games/ES3X/TouchEmu.h>
#pragma optimize("", off)
#include <MinHook.h>

typedef BOOL(__stdcall *LPGetCommModemStatus)(HANDLE hFile, LPDWORD lpModemStat);
static LPGetCommModemStatus __GetCommModemStatus = NULL;

typedef BOOL(WINAPI *LPSetCommState)(HANDLE hFile, LPDCB lpDCB);
static LPSetCommState __SetCommState = NULL;

typedef BOOL(WINAPI *LPGetCommState)(HANDLE hFile, LPDCB lpDCB);
static LPGetCommState __GetCommState = NULL;

typedef BOOL(WINAPI *LPGetCommMask)(HANDLE hFile, DWORD* dwEvtMask);
static LPGetCommMask __GetCommMask = NULL;

typedef BOOL(WINAPI *LPSetCommMask)(HANDLE hFile, DWORD dwEvtMask);
static LPSetCommMask __SetCommMask = NULL;

typedef BOOL(WINAPI *LPEscapeCommFunction)(HANDLE hFile, DWORD);
static LPEscapeCommFunction __EscapeCommFunction = NULL;

typedef BOOL(WINAPI *LPSetCommTimeouts)(HANDLE hFile, LPCOMMTIMEOUTS lpCommTimeouts);
static LPSetCommTimeouts __SetCommTimeouts = NULL;

typedef BOOL(WINAPI *LPGetCommTimeouts)(HANDLE hFile, LPCOMMTIMEOUTS lpCommTimeouts);
static LPGetCommTimeouts __GetCommTimeouts = NULL;

typedef BOOL(WINAPI *LPSetupComm)(HANDLE hFile, DWORD dwInQueue, DWORD dwOutQueue);
static LPSetupComm __SetupComm = NULL;

typedef BOOL(WINAPI *LPPurgeComm)(HANDLE hFile, DWORD dwFlags);
static LPPurgeComm __PurgeComm = NULL;

typedef BOOL(__stdcall *LPClearCommError)(HANDLE hFile, LPDWORD lpErrors, LPCOMSTAT lpStat);
static LPClearCommError __ClearCommError = NULL;

typedef HANDLE(__stdcall *LPCreateFileA)(LPCSTR lpFileName,
	DWORD dwDesiredAccess,
	DWORD dwShareMode,
	LPSECURITY_ATTRIBUTES lpSecurityAttributes,
	DWORD dwCreationDisposition,
	DWORD dwFlagsAndAttributes,
	HANDLE hTemplateFile);
static LPCreateFileA  __CreateFileA = NULL;

typedef HANDLE(__stdcall *LPCreateFileW)(LPCWSTR lpFileName,
	DWORD dwDesiredAccess,
	DWORD dwShareMode,
	LPSECURITY_ATTRIBUTES lpSecurityAttributes,
	DWORD dwCreationDisposition,
	DWORD dwFlagsAndAttributes,
	HANDLE hTemplateFile);
static LPCreateFileW  __CreateFileW = NULL;


LPCSTR emuPort = "\\\\.\\pipe\\teknoparrot_jvs";
LPCWSTR emuPortW = L"\\\\.\\pipe\\teknoparrot_jvs";
LPCSTR hookPort = "COM4";
DWORD amJvsDataOffset = 0;
HANDLE jvsHandle = (HANDLE)-1;
bool JVSAlreadyTaken = false;
HANDLE touchHandle = (HANDLE)-1;
bool touchTaken = false;

HANDLE __stdcall Hook_CreateFileA(LPCSTR lpFileName,
	DWORD dwDesiredAccess,
	DWORD dwShareMode,
	LPSECURITY_ATTRIBUTES lpSecurityAttributes,
	DWORD dwCreationDisposition,
	DWORD dwFlagsAndAttributes,
	HANDLE hTemplateFile)
{
	// Cheap hack for the touchscreen
	// ReadFile checks handle
	if (stricmp(lpFileName, "\\\\.\\COM1") == 0)
	{
		// Assume this is maxitune6...
		// This *very* sucks, I'll write something better one day...
		if (!touchTaken)
		{
			mt6SerialTouchInit();
		}

		HANDLE hResult = __CreateFileA("\\\\.\\pipe\\mt6-touchemu",
			dwDesiredAccess,
			dwShareMode,
			lpSecurityAttributes,
			dwCreationDisposition,
			dwFlagsAndAttributes,
			hTemplateFile);
		touchHandle = hResult;
		touchTaken = true;
		return hResult;
	}


	if (strcmp(lpFileName, hookPort) == 0)
	{
#ifdef _DEBUG
		info(true, "CreateFileA JVS: %s", lpFileName);
		info(true, "--------------------------------------------");
#endif

		if (jvsHandle != INVALID_HANDLE_VALUE)
		{
#ifdef _DEBUG
			info(true, "Closing JVS handle!");
#endif
			CloseHandle(jvsHandle);
			jvsHandle = INVALID_HANDLE_VALUE;
		}

		HANDLE hResult = __CreateFileA(emuPort,
			dwDesiredAccess,
			dwShareMode,
			lpSecurityAttributes,
			dwCreationDisposition,
			dwFlagsAndAttributes,
			hTemplateFile);
		jvsHandle = hResult;

		//JVSAlreadyTaken = true;
		return hResult;
	}

	if (strncmp(lpFileName, "Y:\\", 3) == 0)
	{
		DWORD len = strlen(lpFileName);
		char temp[32];
		memset(temp, 0, 32);
		memcpy(temp, ".\\", 2);
		memcpy(temp + 2, lpFileName + 3, len - 3);
#ifdef _DEBUG
		info(true, "data drive file redirected: %s -> %s", lpFileName, temp);
#endif
		HANDLE hResult = __CreateFileA(temp,
			dwDesiredAccess,
			dwShareMode,
			lpSecurityAttributes,
			dwCreationDisposition,
			dwFlagsAndAttributes,
			hTemplateFile);
		return hResult;
	}
	HANDLE hResult = __CreateFileA(lpFileName,
		dwDesiredAccess,
		dwShareMode,
		lpSecurityAttributes,
		dwCreationDisposition,
		dwFlagsAndAttributes,
		hTemplateFile);
	return hResult;
}
HANDLE __stdcall Hook_CreateFileW(LPCWSTR lpFileName,
	DWORD dwDesiredAccess,
	DWORD dwShareMode,
	LPSECURITY_ATTRIBUTES lpSecurityAttributes,
	DWORD dwCreationDisposition,
	DWORD dwFlagsAndAttributes,
	HANDLE hTemplateFile)
{
	if (wcsicmp(lpFileName, L"\\\\.\\COM1") == 0)
	{
		if (!touchTaken)
		{
			mt6SerialTouchInit();
		}

		HANDLE hResult = __CreateFileW(L"\\\\.\\pipe\\mt6-touchemu",
			dwDesiredAccess,
			dwShareMode,
			lpSecurityAttributes,
			dwCreationDisposition,
			dwFlagsAndAttributes,
			hTemplateFile);

		touchHandle = hResult;
		touchTaken = true;
		return hResult;
	}
	if (wcscmp(lpFileName, L"COM4") == 0 && !JVSAlreadyTaken)
	{
		HANDLE hResult = __CreateFileW(emuPortW,
			dwDesiredAccess,
			dwShareMode,
			lpSecurityAttributes,
			dwCreationDisposition,
			dwFlagsAndAttributes,
			hTemplateFile);
		jvsHandle = hResult;
		JVSAlreadyTaken = true;
		return hResult;
	}

	HANDLE hResult = __CreateFileW(lpFileName,
		dwDesiredAccess,
		dwShareMode,
		lpSecurityAttributes,
		dwCreationDisposition,
		dwFlagsAndAttributes,
		hTemplateFile);
	return hResult;
}

static HANDLE hSection = CreateFileMapping(INVALID_HANDLE_VALUE, nullptr, PAGE_READWRITE, 0, 64, L"TeknoParrot_JvsState");
static int* secData = (int*)MapViewOfFile(hSection, FILE_MAP_ALL_ACCESS, 0, 0, 64);
// Battle gear
int* wheelSection = &secData[1];
int* ffbOffset = &secData[2];
int* ffbOffset2 = &secData[3];
int* ffbOffset3 = &secData[4];
int* ffbOffset4 = &secData[5];
int* ffbOffset5 = &secData[6]; //Vibration Left Motor
int* ffbOffset6 = &secData[7]; //Vibration Right Motor

static bool IsCommHooked(HANDLE hFile);

bool check = false;
BOOL __stdcall Hook_GetCommModemStatus(HANDLE hFile, LPDWORD lpModemStat)
{
	if (!IsCommHooked(hFile)) {
		return __GetCommModemStatus(hFile, lpModemStat);
	}

	if (hFile != jvsHandle)
	{
		return TRUE;
	}

	if(GameDetect::IsTypeX())
	{
		if (*secData) // addressed?
		{
			*lpModemStat = 0x10;
		}
		else
		{
			*lpModemStat = 0x0;
		}
	}
	else
	{
		if (*secData) // addressed?
		{
			*lpModemStat = 0x30; // 0x10
		}
		else
		{
			*lpModemStat = 0x10; // 0x0
		}
	}
	return TRUE;
}

#include <set>

static std::set<HANDLE> g_commOverrides;

static bool IsCommHooked(HANDLE hFile)
{
	return (hFile == jvsHandle || hFile==touchHandle || g_commOverrides.find(hFile) != g_commOverrides.end());
}

void AddCommOverride(HANDLE hFile)
{
	g_commOverrides.insert(hFile);
}

BOOL __stdcall Hook_SetCommState(HANDLE hFile, LPDCB lpDCB)
{
	if (!IsCommHooked(hFile)) {
		return __SetCommState(hFile, lpDCB);
	}

	return true;
}

BOOL __stdcall Hook_GetCommState(HANDLE hFile, LPDCB lpDCB)
{
	if (!IsCommHooked(hFile)) {
		return __GetCommState(hFile, lpDCB);
	}

	return true;
}

BOOL __stdcall Hook_SetCommTimeouts(HANDLE hFile, LPCOMMTIMEOUTS lpCommTimeouts)
{
	if (!IsCommHooked(hFile)) {
		return __SetCommTimeouts(hFile, lpCommTimeouts);
	}

	return true;
}

BOOL __stdcall Hook_GetCommTimeouts(HANDLE hFile, LPCOMMTIMEOUTS lpCommTimeouts)
{
	if (!IsCommHooked(hFile)) {
		return __GetCommTimeouts(hFile, lpCommTimeouts);
	}

	return true;
}

BOOL __stdcall Hook_SetCommMask(HANDLE hFile, DWORD dwEvtMask)
{
	if (!IsCommHooked(hFile)) {
		return __SetCommMask(hFile, dwEvtMask);
	}

	return true;
}


BOOL __stdcall Hook_GetCommMask(HANDLE hFile, DWORD* dwEvtMask)
{
	if (!IsCommHooked(hFile)) {
		return __GetCommMask(hFile, dwEvtMask);
	}

	return true;
}

BOOL __stdcall Hook_EscapeCommFunction(HANDLE hFile, DWORD dwFunc)
{
	if (!IsCommHooked(hFile)) {
		return __EscapeCommFunction(hFile, dwFunc);
	}

	return true;
}

BOOL WINAPI Hook_SetupComm(HANDLE hFile, DWORD dwInQueue, DWORD dwOutQueue)
{
	if (!IsCommHooked(hFile))
	{
		return __SetupComm(hFile, dwInQueue, dwOutQueue);
	}

#if defined(_DEBUG)
	info(true, "SetupComm(hFile %x, dwInQueue %x, dwOutQueue %x)", hFile, dwInQueue, dwOutQueue);
#endif

	return true;
}

BOOL WINAPI Hook_PurgeComm(HANDLE hFile, DWORD dwFlags)
{
	if (!IsCommHooked(hFile)) 
	{
		return __PurgeComm(hFile, dwFlags);
	}

#if defined(WantPurgeCommLogging)
	info(true, "PurgeComm(hFile %x, dwFlags %x)", hFile, dwFlags);
#endif

	CancelIoEx(hFile, nullptr);

	return true;
}

BOOL __stdcall Hook_ClearCommError(HANDLE hFile, LPDWORD lpErrors, LPCOMSTAT lpStat)
{
	if (!IsCommHooked(hFile))
	{
		return __ClearCommError(hFile, lpErrors, lpStat);
	}

	lpStat->cbInQue = 0x7FFF;

	return true;
}

#define __XHOOKn(mod, name)	\
	MH_CreateHookApi(L ## mod, #name, &Hook_##name, (void**)&__##name)

static InitFunction jvsInit([]()
{
	if (GameDetect::IsNesicaGame())
	{
		return;
	}
	if (GameDetect::currentGame == GameID::SFV || GameDetect::currentGame == GameID::FrenzyExpress)
		return;
	if (GameDetect::IsTypeX())
		hookPort = "COM2";

	MH_Initialize();
	__XHOOKn("kernel32.dll", GetCommState);
	__XHOOKn("kernel32.dll", SetCommState);
	__XHOOKn("kernel32.dll", GetCommTimeouts);
	__XHOOKn("kernel32.dll", SetCommTimeouts);
	__XHOOKn("kernel32.dll", GetCommMask);
	__XHOOKn("kernel32.dll", SetCommMask);
	__XHOOKn("kernel32.dll", SetupComm);
	__XHOOKn("kernel32.dll", PurgeComm);
	__XHOOKn("kernel32.dll", EscapeCommFunction);
	__XHOOKn("kernel32.dll", ClearCommError);

	__XHOOKn("kernel32.dll", CreateFileA);
	__XHOOKn("kernel32.dll", CreateFileW);
	__XHOOKn("kernel32.dll", GetCommModemStatus);
	MH_EnableHook(MH_ALL_HOOKS);
});
#pragma optimize("", on)