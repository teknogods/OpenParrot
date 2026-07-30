#include <StdInc.h>
#include "Utility/InitFunction.h"
#include "Functions/Global.h"
#include "Utility\Hooking.Patterns.h"
#include <Xinput.h>
#include <math.h>
#include <dinput.h>
#include "Functions/GlobalRegHooks.h"
#include "Utility\Hooking.Patterns.h"
#include <windows.h>
#include <string>
#include <iostream>
#include <shlobj.h>
#include <fstream>
#include "d3d9.h"
#include "Utility/Helper.h"
#include "Mmsystem.h"
#include <intrin.h>

#if _M_IX86
#pragma optimize("", off)
#pragma comment(lib, "Ws2_32.lib")
#pragma comment( lib, "winmm.lib" )

typedef unsigned int U32;
typedef unsigned char U8;

DWORD BaseAddress9 = 0x00400000;
int horizontal9 = 0;
int vertical9 = 0;
HWND hWndRT9 = 0;

static bool button1pressed = false;
static bool button2pressed = false;
static bool button3pressed = false;
static bool button4pressed = false;
static bool STARTpressed = false;
static bool TESTpressed = false;
static bool SERVICEpressed = false;
static bool previousVolMin = false;
static bool previousVolMax = false;
static bool MenuHack = false;
static bool RiptideHack = false;
static bool MenuHackStopWriting = false;
static bool CoinPressed = false;
static bool NameEntryHack;
static BYTE GameState;
static BYTE DirtyLastLoggedGameState = 0xFF;
static HANDLE DirtyVulcanLuxHandle = INVALID_HANDLE_VALUE;

static decltype(&CreateFileA) DirtyCreateFileAOri = nullptr;
static decltype(&CreateFileW) DirtyCreateFileWOri = nullptr;
static decltype(&socket) DirtySocketOri = nullptr;

static bool IsDirtyDiagnosticsEnabled()
{
	static const bool enabled = []()
	{
		char value[8] = {};
		const DWORD length =
			GetEnvironmentVariableA(
				"TP_ANDROID_DEBUG_LOGGING",
				value,
				_countof(value));
		return length > 0 && length < _countof(value) && value[0] == '1';
	}();
	return enabled;
}

static void WriteDirtyDiagnosticProfile(
	const char* section,
	const char* key,
	const char* value,
	const char* file)
{
	if (IsDirtyDiagnosticsEnabled())
		WritePrivateProfileStringA(section, key, value, file);
}

static void DirtyOutputDebugString(const char* value)
{
	if (IsDirtyDiagnosticsEnabled())
		OutputDebugStringA(value);
}

static SOCKET WSAAPI DirtySocketHook(
	const int addressFamily,
	const int socketType,
	const int protocol)
{
	const SOCKET result =
		DirtySocketOri(addressFamily, socketType, protocol);
	const int error = WSAGetLastError();
	char diagnostic[192] = {};
	sprintf_s(
		diagnostic,
		"TP_DIRTY_DIAG socket af=%d type=%d protocol=%d "
		"result=%08X error=%d caller=%08X",
		addressFamily,
		socketType,
		protocol,
		static_cast<unsigned int>(result),
		error,
		static_cast<unsigned int>(
			reinterpret_cast<uintptr_t>(_ReturnAddress())));
	DirtyOutputDebugString(diagnostic);
	WSASetLastError(error);
	return result;
}

static bool IsDirtyVulcanLuxPath(const char* path)
{
	if (path == nullptr)
		return false;

	const char* name = path;
	if (const char* separator = strrchr(path, '\\'))
		name = separator + 1;
	if (const char* separator = strrchr(name, '/'))
		name = separator + 1;
	return _stricmp(name, "Vulcan.lux") == 0;
}

static bool IsDirtyVulcanLuxPath(const wchar_t* path)
{
	if (path == nullptr)
		return false;

	const wchar_t* name = path;
	if (const wchar_t* separator = wcsrchr(path, L'\\'))
		name = separator + 1;
	if (const wchar_t* separator = wcsrchr(name, L'/'))
		name = separator + 1;
	return _wcsicmp(name, L"Vulcan.lux") == 0;
}

static bool BuildDirtyLaaFallbackPath(
	const char* path,
	std::string& fallback)
{
	static constexpr char stagingSegment[] = "\\.teknoparrot-laa\\";
	if (path == nullptr)
		return false;

	for (const char* cursor = path; *cursor != '\0'; ++cursor)
	{
		if (_strnicmp(
			cursor,
			stagingSegment,
			sizeof(stagingSegment) - 1) == 0)
		{
			fallback.assign(path, cursor - path);
			fallback.append(cursor + sizeof(stagingSegment) - 1);
			return true;
		}
	}
	return false;
}

static bool BuildDirtyLaaFallbackPath(
	const wchar_t* path,
	std::wstring& fallback)
{
	static constexpr wchar_t stagingSegment[] = L"\\.teknoparrot-laa\\";
	if (path == nullptr)
		return false;

	for (const wchar_t* cursor = path; *cursor != L'\0'; ++cursor)
	{
		if (_wcsnicmp(
			cursor,
			stagingSegment,
			_countof(stagingSegment) - 1) == 0)
		{
			fallback.assign(path, cursor - path);
			fallback.append(cursor + _countof(stagingSegment) - 1);
			return true;
		}
	}
	return false;
}

static void WriteDirtyVulcanLuxOpenDiagnostic(
	const HANDLE file,
	const DWORD openError,
	const bool usedFallback)
{
	LARGE_INTEGER size = {};
	SetLastError(ERROR_SUCCESS);
	const BOOL sizeResult =
		file != INVALID_HANDLE_VALUE && GetFileSizeEx(file, &size);
	const DWORD sizeError = GetLastError();
	char currentDirectory[MAX_PATH] = {};
	GetCurrentDirectoryA(_countof(currentDirectory), currentDirectory);
	char diagnostic[512] = {};
	sprintf_s(
		diagnostic,
		"handle=%08X openError=%lu sizeResult=%u size=%llu sizeError=%lu "
		"fallback=%u cwd=%s",
		reinterpret_cast<uintptr_t>(file),
		static_cast<unsigned long>(openError),
		static_cast<unsigned int>(sizeResult),
		static_cast<unsigned long long>(size.QuadPart),
		static_cast<unsigned long>(sizeError),
		static_cast<unsigned int>(usedFallback),
		currentDirectory);
	WriteDirtyDiagnosticProfile(
		"Lux",
		"CreateFile",
		diagnostic,
		".\\DirtyDrivinStateDiagnostic.ini");
	SetLastError(openError);
}

static HANDLE WINAPI DirtyCreateFileAHook(
	LPCSTR fileName,
	DWORD desiredAccess,
	DWORD shareMode,
	LPSECURITY_ATTRIBUTES securityAttributes,
	DWORD creationDisposition,
	DWORD flagsAndAttributes,
	HANDLE templateFile)
{
	HANDLE file = DirtyCreateFileAOri(
		fileName,
		desiredAccess,
		shareMode,
		securityAttributes,
		creationDisposition,
		flagsAndAttributes,
		templateFile);
	DWORD openError = GetLastError();
	bool usedFallback = false;
	std::string fallback;
	if (file == INVALID_HANDLE_VALUE &&
		BuildDirtyLaaFallbackPath(fileName, fallback))
	{
		file = DirtyCreateFileAOri(
			fallback.c_str(),
			desiredAccess,
			shareMode,
			securityAttributes,
			creationDisposition,
			flagsAndAttributes,
			templateFile);
		openError = GetLastError();
		usedFallback = file != INVALID_HANDLE_VALUE;
	}
	if (file == INVALID_HANDLE_VALUE && IsDirtyVulcanLuxPath(fileName))
	{
		file = DirtyCreateFileAOri(
			"Vulcan.lux",
			desiredAccess,
			shareMode,
			securityAttributes,
			creationDisposition,
			flagsAndAttributes,
			templateFile);
		openError = GetLastError();
		usedFallback = file != INVALID_HANDLE_VALUE;
	}
	if (IsDirtyVulcanLuxPath(fileName))
	{
		DirtyVulcanLuxHandle = file;
		WriteDirtyVulcanLuxOpenDiagnostic(file, openError, usedFallback);
	}
	SetLastError(openError);
	return file;
}

static HANDLE WINAPI DirtyCreateFileWHook(
	LPCWSTR fileName,
	DWORD desiredAccess,
	DWORD shareMode,
	LPSECURITY_ATTRIBUTES securityAttributes,
	DWORD creationDisposition,
	DWORD flagsAndAttributes,
	HANDLE templateFile)
{
	HANDLE file = DirtyCreateFileWOri(
		fileName,
		desiredAccess,
		shareMode,
		securityAttributes,
		creationDisposition,
		flagsAndAttributes,
		templateFile);
	DWORD openError = GetLastError();
	bool usedFallback = false;
	std::wstring fallback;
	if (file == INVALID_HANDLE_VALUE &&
		BuildDirtyLaaFallbackPath(fileName, fallback))
	{
		file = DirtyCreateFileWOri(
			fallback.c_str(),
			desiredAccess,
			shareMode,
			securityAttributes,
			creationDisposition,
			flagsAndAttributes,
			templateFile);
		openError = GetLastError();
		usedFallback = file != INVALID_HANDLE_VALUE;
	}
	if (file == INVALID_HANDLE_VALUE && IsDirtyVulcanLuxPath(fileName))
	{
		file = DirtyCreateFileWOri(
			L"Vulcan.lux",
			desiredAccess,
			shareMode,
			securityAttributes,
			creationDisposition,
			flagsAndAttributes,
			templateFile);
		openError = GetLastError();
		usedFallback = file != INVALID_HANDLE_VALUE;
	}
	if (IsDirtyVulcanLuxPath(fileName))
	{
		DirtyVulcanLuxHandle = file;
		WriteDirtyVulcanLuxOpenDiagnostic(file, openError, usedFallback);
	}
	SetLastError(openError);
	return file;
}

// controls
extern int* ffbOffset;
extern int* ffbOffset2;
extern int* ffbOffset3;
extern int* ffbOffset4;

BOOL(__stdcall* original_CreateWindowExW9)(DWORD dwExStyle, LPCWSTR lpClassName, LPCWSTR lpWindowName, DWORD dwStyle, int X, int Y, int nWidth, int nHeight, HWND hWndParent, HMENU hMenu, HINSTANCE hInstance, LPVOID lpParam);
BOOL(__stdcall* original_CreateWindowExA9)(DWORD dwExStyle, LPCSTR lpClassName, LPCSTR lpWindowName, DWORD dwStyle, int X, int Y, int nWidth, int nHeight, HWND hWndParent, HMENU hMenu, HINSTANCE hInstance, LPVOID lpParam);

void __stdcall ServiceControlsPatch()
{
	// TEST
	if ((GetAsyncKeyState(VK_INSERT) & 0x8000) || (*ffbOffset & 0x400))
	{
		if (TESTpressed == false)
		{
			injector::WriteMemory<BYTE>((0x98C380), 0x01, true);
			TESTpressed = true;
		}
	}
	else
	{
		if (TESTpressed == true)
		{
			injector::WriteMemory<BYTE>((0x98C380), 0x00, true);
			TESTpressed = false;
		}
	}
	// SERVICE
	if ((GetAsyncKeyState(VK_DELETE) & 0x8000) || (*ffbOffset & 0x800))
	{
		if (SERVICEpressed == false)
		{
			injector::WriteMemory<BYTE>((0x98C384), 0x01, true);
			SERVICEpressed = true;
		}
	}
	else
	{
		if (SERVICEpressed == true)
		{
			injector::WriteMemory<BYTE>((0x98C384), 0x00, true);
			SERVICEpressed = false;
		}
	}
	// VOL+
	if (*ffbOffset & 0x1000)
	{
		if (previousVolMax == false)
		{
			injector::WriteMemory<BYTE>((0x98C388), 0x01, true);
			previousVolMax = true;
		}
	}
	else
	{
		if (previousVolMax == true)
		{
			injector::WriteMemory<BYTE>((0x98C388), 0x00, true);
			previousVolMax = false;
		}
	}
	// VOL-
	if (*ffbOffset & 0x2000)
	{
		if (previousVolMin == false)
		{
			injector::WriteMemory<BYTE>((0x98C38C), 0x01, true);
			previousVolMin = true;
		}
	}
	else
	{
		if (previousVolMin == true)
		{
			injector::WriteMemory<BYTE>((0x98C38C), 0x00, true);
			previousVolMin = false;
		}
	}

	return;
}

static char CoinDigitChar[256];
static DWORD CoinDigitAddress;
static char Digits[256];
static int Digit1CoinValueinHex;
static int Digit2CoinValueinHex;
static bool SoundFail;
static VOID(WINAPI* DirtyExitProcessOri)(UINT exitCode) = nullptr;
static BOOL(WINAPI* DirtyTerminateProcessOri)(HANDLE process, UINT exitCode) = nullptr;
static VOID(WINAPI* DirtyPostQuitMessageOri)(int exitCode) = nullptr;
using DirtyAppThreadMain = int(__thiscall*)(void*);
static DirtyAppThreadMain DirtyAppThreadMainOri =
	reinterpret_cast<DirtyAppThreadMain>(0x00560EF0);
using DirtyAppUpdate = int(__cdecl*)(uintptr_t);
static DirtyAppUpdate DirtyAppUpdateOri =
	reinterpret_cast<DirtyAppUpdate>(0x00401180);
static DWORD DirtyAppThreadStartedAt = 0;
static volatile LONG DirtyAppUpdateStopLogged = 0;
using DirtyWindowCreate = HRESULT(__stdcall*)(void*);
static DirtyWindowCreate DirtyWindowCreateOri =
	reinterpret_cast<DirtyWindowCreate>(0x00639890);
static HRESULT __stdcall DirtyWindowCreateHook(void* parameters);
using DirtyInitialWindowCreate =
	HRESULT(__fastcall*)(
		uintptr_t,
		uintptr_t,
		uintptr_t,
		uintptr_t,
		uintptr_t);
static DirtyInitialWindowCreate DirtyInitialWindowCreateOri =
	reinterpret_cast<DirtyInitialWindowCreate>(0x00637CA0);
using DirtyInitStep = int(__cdecl*)();
static volatile LONG dirtyExceptionCaptured = 0;

static LONG CALLBACK DirtyExceptionDiagnostic(
	EXCEPTION_POINTERS* exceptionPointers)
{
	if (exceptionPointers == nullptr ||
		exceptionPointers->ExceptionRecord == nullptr ||
		exceptionPointers->ContextRecord == nullptr ||
		exceptionPointers->ExceptionRecord->ExceptionCode !=
			EXCEPTION_ACCESS_VIOLATION)
	{
		return EXCEPTION_CONTINUE_SEARCH;
	}
	if (InterlockedCompareExchange(&dirtyExceptionCaptured, 1, 0) != 0)
		return EXCEPTION_CONTINUE_SEARCH;

	const auto record = exceptionPointers->ExceptionRecord;
	const auto context = exceptionPointers->ContextRecord;
	DWORD stack[12] = {};
	__try
	{
		memcpy(
			stack,
			reinterpret_cast<const void*>(context->Esp),
			sizeof(stack));
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		memset(stack, 0, sizeof(stack));
	}

	char diagnostic[768] = {};
	sprintf_s(
		diagnostic,
		"code=%08X address=%08X eip=%08X esp=%08X ebp=%08X "
		"eax=%08X ebx=%08X ecx=%08X edx=%08X esi=%08X edi=%08X "
		"access=%08X target=%08X stack="
		"%08X,%08X,%08X,%08X,%08X,%08X,%08X,%08X,%08X,%08X,%08X,%08X",
		record->ExceptionCode,
		reinterpret_cast<uintptr_t>(record->ExceptionAddress),
		context->Eip,
		context->Esp,
		context->Ebp,
		context->Eax,
		context->Ebx,
		context->Ecx,
		context->Edx,
		context->Esi,
		context->Edi,
		record->NumberParameters > 0 ?
			static_cast<unsigned int>(record->ExceptionInformation[0]) : 0,
		record->NumberParameters > 1 ?
			static_cast<unsigned int>(record->ExceptionInformation[1]) : 0,
		stack[0],
		stack[1],
		stack[2],
		stack[3],
		stack[4],
		stack[5],
		stack[6],
		stack[7],
		stack[8],
		stack[9],
		stack[10],
		stack[11]);
	WriteDirtyDiagnosticProfile(
		"Crash",
		"Last",
		diagnostic,
		".\\DirtyDrivinCrashDiagnostic.ini");
	DirtyOutputDebugString(diagnostic);
	return EXCEPTION_CONTINUE_SEARCH;
}

static int TraceDirtyInitStep(
	const char* name,
	const DirtyInitStep function)
{
	const int result = function();
	char diagnostic[96] = {};
	sprintf_s(diagnostic, "result=%d", result);
	WriteDirtyDiagnosticProfile(
		"InitSteps",
		name,
		diagnostic,
		".\\DirtyDrivinStateDiagnostic.ini");
	return result;
}

static int __cdecl DirtyInitStep1Hook()
{
	return TraceDirtyInitStep(
		"Step1_004044D0",
		reinterpret_cast<DirtyInitStep>(0x004044D0));
}

static int __cdecl DirtyInitStep2Hook()
{
	return TraceDirtyInitStep(
		"Step2_005A2E10",
		reinterpret_cast<DirtyInitStep>(0x005A2E10));
}

using DirtyInitObjectMethod = int(__thiscall*)(void*);
static DirtyInitObjectMethod DirtyInitStep3Method58Ori = nullptr;
static DirtyInitObjectMethod DirtyInitStep3Method60Ori = nullptr;
static DirtyInitObjectMethod DirtyInitStep3Method68Ori = nullptr;
static bool DirtyBypassCabinetHardware = false;

static int TraceDirtyInitStep3Method(
	const char* name,
	const DirtyInitObjectMethod function,
	void* self)
{
	SetLastError(ERROR_SUCCESS);
	const int result = function(self);
	const DWORD lastError = GetLastError();
	char diagnostic[160] = {};
	sprintf_s(
		diagnostic,
		"result=%d self=%08X target=%08X lastError=%lu",
		result,
		reinterpret_cast<uintptr_t>(self),
		reinterpret_cast<uintptr_t>(function),
		static_cast<unsigned long>(lastError));
	WriteDirtyDiagnosticProfile(
		"InitStep3Methods",
		name,
		diagnostic,
		".\\DirtyDrivinStateDiagnostic.ini");
	return result;
}

static bool IsDirtyDrivinRunningUnderWine()
{
	const HMODULE ntdll = GetModuleHandleW(L"ntdll.dll");
	return ntdll != nullptr &&
		GetProcAddress(ntdll, "wine_get_version") != nullptr;
}

static int __fastcall DirtyInitStep3Method58Hook(void* self, void*)
{
	if (IsDirtyDrivinRunningUnderWine())
	{
		DirtyBypassCabinetHardware = true;
		WriteDirtyDiagnosticProfile(
			"InitStep3Methods",
			"Method58",
			"result=1 bypassedMissingRawThrillsHid=1",
			".\\DirtyDrivinStateDiagnostic.ini");
		return 1;
	}

	return TraceDirtyInitStep3Method(
		"Method58",
		DirtyInitStep3Method58Ori,
		self);
}

static int __fastcall DirtyInitStep3Method60Hook(void* self, void*)
{
	if (*reinterpret_cast<const uintptr_t*>(0x009A1F8C) == 0)
	{
		WriteDirtyDiagnosticProfile(
			"InitStep3Methods",
			"Method60",
			"result=1 bypassedMissingFtdiDevice=1",
			".\\DirtyDrivinStateDiagnostic.ini");
		return 1;
	}

	return TraceDirtyInitStep3Method(
		"Method60",
		DirtyInitStep3Method60Ori,
		self);
}

static int __fastcall DirtyInitStep3Method68Hook(void* self, void*)
{
	if (DirtyBypassCabinetHardware)
	{
		WriteDirtyDiagnosticProfile(
			"InitStep3Methods",
			"Method68",
			"result=1 bypassedMissingPortTalkDriver=1",
			".\\DirtyDrivinStateDiagnostic.ini");
		return 1;
	}

	return TraceDirtyInitStep3Method(
		"Method68",
		DirtyInitStep3Method68Ori,
		self);
}

static int __cdecl DirtyInitStep3Hook()
{
	auto self = reinterpret_cast<void*>(0x009A1F70);
	auto vtable = *reinterpret_cast<uintptr_t**>(self);
	DirtyInitStep3Method58Ori =
		reinterpret_cast<DirtyInitObjectMethod>(vtable[0x58 / sizeof(uintptr_t)]);
	DirtyInitStep3Method60Ori =
		reinterpret_cast<DirtyInitObjectMethod>(vtable[0x60 / sizeof(uintptr_t)]);
	DirtyInitStep3Method68Ori =
		reinterpret_cast<DirtyInitObjectMethod>(vtable[0x68 / sizeof(uintptr_t)]);
	injector::WriteMemory<uintptr_t>(
		reinterpret_cast<uintptr_t>(&vtable[0x58 / sizeof(uintptr_t)]),
		reinterpret_cast<uintptr_t>(DirtyInitStep3Method58Hook),
		true);
	injector::WriteMemory<uintptr_t>(
		reinterpret_cast<uintptr_t>(&vtable[0x60 / sizeof(uintptr_t)]),
		reinterpret_cast<uintptr_t>(DirtyInitStep3Method60Hook),
		true);
	injector::WriteMemory<uintptr_t>(
		reinterpret_cast<uintptr_t>(&vtable[0x68 / sizeof(uintptr_t)]),
		reinterpret_cast<uintptr_t>(DirtyInitStep3Method68Hook),
		true);

	SetLastError(ERROR_SUCCESS);
	const int result =
		reinterpret_cast<DirtyInitStep>(0x005A24D0)();
	const DWORD lastError = GetLastError();
	char diagnostic[512] = {};
	sprintf_s(
		diagnostic,
		"result=%d lastError=%lu object=%08X vtable=%08X "
		"workerFail=%u method58Fail=%u initialized=%u value1FA8=%08X "
		"field18=%08X field5C=%08X fieldA0=%08X",
		result,
		static_cast<unsigned long>(lastError),
		reinterpret_cast<uintptr_t>(self),
		reinterpret_cast<uintptr_t>(vtable),
		*reinterpret_cast<const DWORD*>(0x009A1F98),
		*reinterpret_cast<const DWORD*>(0x009A1F94),
		*reinterpret_cast<const DWORD*>(0x009A1F88),
		*reinterpret_cast<const DWORD*>(0x009A1FA8),
		*reinterpret_cast<const DWORD*>(0x009A1F70 + 0x18),
		*reinterpret_cast<const DWORD*>(0x009A1F70 + 0x5C),
		*reinterpret_cast<const DWORD*>(0x009A1F70 + 0xA0));
	WriteDirtyDiagnosticProfile(
		"InitSteps",
		"Step3_005A24D0",
		diagnostic,
		".\\DirtyDrivinStateDiagnostic.ini");
	DirtyOutputDebugString(diagnostic);
	return result;
}

static int __cdecl DirtyInitStep4Hook()
{
	return TraceDirtyInitStep(
		"Step4_005656F0",
		reinterpret_cast<DirtyInitStep>(0x005656F0));
}

static int __cdecl DirtyInitStep5Hook()
{
	return TraceDirtyInitStep(
		"Step5_00410310",
		reinterpret_cast<DirtyInitStep>(0x00410310));
}

static int __cdecl DirtyLuxInitHook()
{
	using DirtyLuxTextGetter = const char*(__thiscall*)(void*);
	auto source = reinterpret_cast<void*>(0x007CE028);
	auto vtable = *reinterpret_cast<uintptr_t**>(source);
	const auto getLuxPath =
		reinterpret_cast<DirtyLuxTextGetter>(vtable[0x08 / sizeof(uintptr_t)]);
	const auto getLuxLabel =
		reinterpret_cast<DirtyLuxTextGetter>(vtable[0x0C / sizeof(uintptr_t)]);
	char luxPath[260] = {};
	char luxLabel[128] = {};
	strncpy_s(luxPath, getLuxPath(source), _TRUNCATE);
	strncpy_s(luxLabel, getLuxLabel(source), _TRUNCATE);

	SetLastError(ERROR_SUCCESS);
	const int result = reinterpret_cast<DirtyInitStep>(0x0055F240)();
	const DWORD lastError = GetLastError();
	const uintptr_t errorSource =
		*reinterpret_cast<const uintptr_t*>(0x0099D930);
	const DWORD errorCode =
		*reinterpret_cast<const DWORD*>(0x0099D934);
	char diagnostic[768] = {};
	sprintf_s(
		diagnostic,
		"result=%d object=%08X fileHandle=%08X lastError=%lu "
		"errorSource=%08X errorCode=%u path=%s label=%s",
		result,
		*reinterpret_cast<const uintptr_t*>(0x007CE130),
		reinterpret_cast<uintptr_t>(DirtyVulcanLuxHandle),
		static_cast<unsigned long>(lastError),
		errorSource,
		errorCode,
		luxPath,
		luxLabel);
	WriteDirtyDiagnosticProfile(
		"Lux",
		"Initialize",
		diagnostic,
		".\\DirtyDrivinStateDiagnostic.ini");
	DirtyOutputDebugString(diagnostic);
	return result;
}

static void WriteDirtyStateDiagnostic(const char* key, const char* value)
{
	WriteDirtyDiagnosticProfile(
		"State",
		key,
		value,
		".\\DirtyDrivinStateDiagnostic.ini");
}

static void WriteDirtyExitDiagnostic(const char* api, UINT exitCode)
{
	void* frames[12] = {};
	const USHORT frameCount = CaptureStackBackTrace(
		0,
		static_cast<DWORD>(_countof(frames)),
		frames,
		nullptr);
	char value[512] = {};
	int used = sprintf_s(
		value,
		"%s code=%u thread=%u frames=",
		api,
		exitCode,
		GetCurrentThreadId());
	for (USHORT index = 0; index < frameCount && used > 0 &&
		used < static_cast<int>(sizeof(value)); index++)
	{
		const int written = sprintf_s(
			value + used,
			sizeof(value) - used,
			"%s%p",
			index == 0 ? "" : ",",
			frames[index]);
		if (written <= 0)
			break;
		used += written;
	}
	WriteDirtyDiagnosticProfile(
		"Exit",
		api,
		value,
		".\\DirtyDrivinExitDiagnostic.ini");
	DirtyOutputDebugString(value);
}

static VOID WINAPI DirtyExitProcessHook(UINT exitCode)
{
	WriteDirtyExitDiagnostic("ExitProcess", exitCode);
	DirtyExitProcessOri(exitCode);
}

static BOOL WINAPI DirtyTerminateProcessHook(HANDLE process, UINT exitCode)
{
	WriteDirtyExitDiagnostic("TerminateProcess", exitCode);
	return DirtyTerminateProcessOri(process, exitCode);
}

static VOID WINAPI DirtyPostQuitMessageHook(int exitCode)
{
	WriteDirtyExitDiagnostic("PostQuitMessage", static_cast<UINT>(exitCode));
	DirtyPostQuitMessageOri(exitCode);
}

static uintptr_t __cdecl DirtyPublishCreatedApp(const uintptr_t app)
{
	*reinterpret_cast<uintptr_t*>(0x0099D88C) = app;
	MemoryBarrier();
	*reinterpret_cast<DWORD*>(0x0099D880) = 1;

	char diagnostic[160] = {};
	sprintf_s(
		diagnostic,
		"app=%08X elapsedMs=%lu ready=%u",
		app,
		static_cast<unsigned long>(
			GetTickCount() - DirtyAppThreadStartedAt),
		*reinterpret_cast<const DWORD*>(0x0099D880));
	WriteDirtyStateDiagnostic("AppCreated", diagnostic);
	return app;
}

static void __declspec(naked) DirtyStoreCreatedAppAndSignalReadyHook()
{
	__asm
	{
		push eax
		call DirtyPublishCreatedApp
		add esp, 4
		ret
	}
}

static int __cdecl DirtyAppUpdateHook(const uintptr_t shutdownRequested)
{
	const int result = DirtyAppUpdateOri(shutdownRequested);
	if (result == 0 &&
		InterlockedCompareExchange(&DirtyAppUpdateStopLogged, 1, 0) == 0)
	{
		char diagnostic[256] = {};
		sprintf_s(
			diagnostic,
			"result=0 requested=%08X elapsedMs=%lu ready=%u app=%08X "
			"stop=%u stage=%u gameState=%u",
			shutdownRequested,
			static_cast<unsigned long>(
				GetTickCount() - DirtyAppThreadStartedAt),
			*reinterpret_cast<const DWORD*>(0x0099D880),
			*reinterpret_cast<const DWORD*>(0x0099D88C),
			*reinterpret_cast<const DWORD*>(0x0099D888),
			*reinterpret_cast<const DWORD*>(0x008816EC),
			static_cast<unsigned int>(GameState));
		WriteDirtyStateDiagnostic("AppUpdateStop", diagnostic);
		DirtyOutputDebugString(diagnostic);
	}
	return result;
}

// The fifth term in Dirty Drivin's attract wave table multiplies time by
// 472783486976.0 before using x87 FSIN. Box64 eventually reports
// STATUS_FLOAT_MULTIPLE_TRAPS while reducing that very large argument. Reduce
// the phase in double precision first, preserving the intended sine while
// keeping the x87 input in its reliable range.
static float __cdecl DirtyReducedFifthWave(const float value)
{
	if (!_finite(static_cast<double>(value)))
		return 0.0f;

	constexpr double frequency = 472783486976.0;
	constexpr double twoPi = 6.283185307179586476925286766559;
	const double phase = fmod(static_cast<double>(value) * frequency, twoPi);
	if (!_finite(phase))
		return 0.0f;
	return static_cast<float>(sin(phase));
}

static void __declspec(naked) DirtyReducedFifthWaveHook()
{
	__asm
	{
		push eax
		push dword ptr [esp + 20h]
		call DirtyReducedFifthWave
		add esp, 4
		pop eax
		ret
	}
}

static int __fastcall DirtyAppThreadMainHook(void* self, void*)
{
	const DWORD startedAt = GetTickCount();
	DirtyAppThreadStartedAt = startedAt;
	InterlockedExchange(&DirtyAppUpdateStopLogged, 0);
	WriteDirtyStateDiagnostic("AppThreadStart", "1");
	const auto callSite = reinterpret_cast<const BYTE*>(0x0058F3E8);
	const auto relativeTarget =
		*reinterpret_cast<const int32_t*>(callSite + 1);
	char callSiteDiagnostic[192] = {};
	sprintf_s(
		callSiteDiagnostic,
		"bytes=%02X%02X%02X%02X%02X target=%08X expected=%08X",
		callSite[0],
		callSite[1],
		callSite[2],
		callSite[3],
		callSite[4],
		reinterpret_cast<uintptr_t>(callSite + 5 + relativeTarget),
		reinterpret_cast<uintptr_t>(DirtyWindowCreateHook));
	WriteDirtyStateDiagnostic("WindowCreateCallSite", callSiteDiagnostic);
	const int result = DirtyAppThreadMainOri(self);

	char diagnostic[384] = {};
	sprintf_s(
		diagnostic,
		"result=%d elapsedMs=%lu create=%08X app=%08X stop=%08X "
		"shutdown=%08X status=%08X stage=%u errorActive=%u errorCode=%u "
		"gameState=%u",
		result,
		static_cast<unsigned long>(GetTickCount() - startedAt),
		*reinterpret_cast<const DWORD*>(0x0099D80C),
		*reinterpret_cast<const DWORD*>(0x0099D88C),
		*reinterpret_cast<const DWORD*>(0x0099D888),
		*reinterpret_cast<const DWORD*>(0x0099D818),
		*reinterpret_cast<const DWORD*>(0x0099D8EC),
		*reinterpret_cast<const DWORD*>(0x008816EC),
		*reinterpret_cast<const DWORD*>(0x0099D930),
		*reinterpret_cast<const DWORD*>(0x0099D934),
		static_cast<unsigned int>(GameState));
	WriteDirtyStateDiagnostic("AppThreadEnd", diagnostic);
	DirtyOutputDebugString(diagnostic);
	return result;
}

static HRESULT __stdcall DirtyWindowCreateHook(void* parameters)
{
	const auto videoBefore = *reinterpret_cast<const DWORD*>(0x0087F9C8);
	bool clearedStaleCreate = false;
	if (videoBefore != 0 &&
		*reinterpret_cast<const DWORD*>(videoBefore + 8) == 0 &&
		*reinterpret_cast<const BYTE*>(videoBefore + 0x30D) == 0 &&
		*reinterpret_cast<const BYTE*>(videoBefore + 0x310) != 0)
	{
		*reinterpret_cast<BYTE*>(videoBefore + 0x310) = 0;
		clearedStaleCreate = true;
	}
	char preDiagnostic[128] = {};
	sprintf_s(
		preDiagnostic,
		"video=%08X active=%u closing=%u retry=%u",
		videoBefore,
		videoBefore == 0 ? 0 :
			*reinterpret_cast<const BYTE*>(videoBefore + 0x30D),
		videoBefore == 0 ? 0 :
			*reinterpret_cast<const BYTE*>(videoBefore + 0x310),
		clearedStaleCreate ? 1 : 0);
	WriteDirtyStateDiagnostic("WindowCreatePre", preDiagnostic);
	const HRESULT result = DirtyWindowCreateOri(parameters);
	const auto video = *reinterpret_cast<const DWORD*>(0x0087F9C8);
	char diagnostic[384] = {};
	sprintf_s(
		diagnostic,
		"result=%08X video=%08X device=%08X active=%u closing=%u retry=%u "
		"param0=%08X param1=%08X param2=%08X param3=%08X",
		static_cast<unsigned int>(result),
		video,
		video == 0 ? 0 :
			*reinterpret_cast<const DWORD*>(video + 8),
		video == 0 ? 0 :
			*reinterpret_cast<const BYTE*>(video + 0x30D),
		video == 0 ? 0 :
			*reinterpret_cast<const BYTE*>(video + 0x310),
		clearedStaleCreate ? 1 : 0,
		parameters == nullptr ? 0 :
			reinterpret_cast<const DWORD*>(parameters)[0],
		parameters == nullptr ? 0 :
			reinterpret_cast<const DWORD*>(parameters)[1],
		parameters == nullptr ? 0 :
			reinterpret_cast<const DWORD*>(parameters)[2],
		parameters == nullptr ? 0 :
			reinterpret_cast<const DWORD*>(parameters)[3]);
	WriteDirtyStateDiagnostic("WindowCreate", diagnostic);
	DirtyOutputDebugString(diagnostic);
	return result;
}

static HRESULT __fastcall DirtyInitialWindowCreateHook(
	const uintptr_t instance,
	const uintptr_t command,
	const uintptr_t optionA,
	const uintptr_t optionB,
	const uintptr_t window)
{
	// Wine can expose the X11 window just after the game's first create call.
	// A short title-scoped delay avoids racing the guest window handle setup.
	if (IsDirtyDrivinRunningUnderWine())
		Sleep(250);
	const auto video = *reinterpret_cast<const DWORD*>(0x0087F9C8);
	char preDiagnostic[192] = {};
	sprintf_s(
		preDiagnostic,
		"instance=%08X command=%08X optionA=%08X optionB=%08X "
		"window=%08X video=%08X active=%u closing=%u",
		instance,
		command,
		optionA,
		optionB,
		window,
		video,
		video == 0 ? 0 :
			*reinterpret_cast<const BYTE*>(video + 0x30D),
		video == 0 ? 0 :
			*reinterpret_cast<const BYTE*>(video + 0x310));
	WriteDirtyStateDiagnostic("InitialWindowCreatePre", preDiagnostic);

	HRESULT result =
		DirtyInitialWindowCreateOri(
			instance,
			command,
			optionA,
			optionB,
			window);
	unsigned int attempts = 1;
	while (IsDirtyDrivinRunningUnderWine() &&
		result == HRESULT_FROM_WIN32(ERROR_INVALID_WINDOW_HANDLE) &&
		attempts < 8)
	{
		const auto retryVideo =
			*reinterpret_cast<const DWORD*>(0x0087F9C8);
		if (retryVideo != 0 &&
			*reinterpret_cast<const DWORD*>(retryVideo + 8) == 0)
		{
			*reinterpret_cast<BYTE*>(retryVideo + 0x310) = 0;
		}
		Sleep(250);
		result =
			DirtyInitialWindowCreateOri(
				instance,
				command,
				optionA,
				optionB,
				window);
		attempts++;
	}
	const auto videoAfter =
		*reinterpret_cast<const DWORD*>(0x0087F9C8);
	char postDiagnostic[224] = {};
	sprintf_s(
		postDiagnostic,
		"result=%08X attempts=%u video=%08X device=%08X active=%u "
		"closing=%u",
		static_cast<unsigned int>(result),
		attempts,
		videoAfter,
		videoAfter == 0 ? 0 :
			*reinterpret_cast<const DWORD*>(videoAfter + 8),
		videoAfter == 0 ? 0 :
			*reinterpret_cast<const BYTE*>(videoAfter + 0x30D),
		videoAfter == 0 ? 0 :
			*reinterpret_cast<const BYTE*>(videoAfter + 0x310));
	WriteDirtyStateDiagnostic("InitialWindowCreate", postDiagnostic);
	return result;
}

static void CoinInput(Helpers* helpers)
{
	UINT8 CoinValue = helpers->ReadByte(0x4947AC, true);

	INT_PTR CoinDigitBase = helpers->ReadIntPtr(0x494064, true);
	INT_PTR CoinDigitA = helpers->ReadIntPtr(CoinDigitBase + 0x10, false);
	INT_PTR CoinDigitB = helpers->ReadIntPtr(CoinDigitA + 0x600, false);
	INT_PTR CoinDigitC = helpers->ReadIntPtr(CoinDigitB + 0x1A0, false);

	if (*ffbOffset & 0x04)
	{
		if (!CoinPressed)
		{
			CoinPressed = true;

			if (!SoundFail)
			{
				LPCWSTR Stop = L"stop .\\Coin\\Coin.mp3";
				int Stopping = mciSendString(Stop, NULL, 0, 0);

				LPCWSTR Play = L"play .\\Coin\\Coin.mp3 from 0";
				int Playing = mciSendString(Play, NULL, 0, 0);

				if (Playing != 0 && Stopping != 0)
				{
					SoundFail = true;
				}
			}

			helpers->WriteByte(0x8947AC, ++CoinValue, false);
		}
	}
	else
	{
		if (CoinPressed)
		{
			CoinPressed = false;
		}
	}

	if (CoinDigitC != NULL)
	{
		memset(CoinDigitChar, 0, 256);
		sprintf(CoinDigitChar, "%p", CoinDigitC + 0xE70);
		CoinDigitAddress = strtoul(CoinDigitChar, NULL, 16);

		helpers->WriteIntPtr(0x4947B0, 0x0000, true);

		if (CoinValue > 99)
			helpers->WriteByte(0x8947AC, 0x63, false);

		if (CoinValue >= 0x0A)
		{
			injector::WriteMemoryRaw(CoinDigitAddress + 0x03, "\x00\x20\x00\x20\x00\x43\x00\x52\x00\x45\x00\x44\x00\x49\x00\x54\x00\x53\x00\x00\x00\x00\x00\x00\x00\x00", 27, false);

			if (CoinValue >= 99)
			{
				itoa(0x39, Digits, 16);
				Digit1CoinValueinHex = 0x39;
			}
			else if (CoinValue >= 90)
			{
				itoa(CoinValue - 42, Digits, 16);
				Digit1CoinValueinHex = 0x39;
			}
			else if (CoinValue >= 80)
			{
				itoa(CoinValue - 32, Digits, 16);
				Digit1CoinValueinHex = 0x38;
			}
			else if (CoinValue >= 70)
			{
				itoa(CoinValue - 22, Digits, 16);
				Digit1CoinValueinHex = 0x37;
			}
			else if (CoinValue >= 60)
			{
				itoa(CoinValue - 12, Digits, 16);
				Digit1CoinValueinHex = 0x36;
			}
			else if (CoinValue >= 50)
			{
				itoa(CoinValue - 2, Digits, 16);
				Digit1CoinValueinHex = 0x35;
			}
			else if (CoinValue >= 40)
			{
				itoa(CoinValue + 8, Digits, 16);
				Digit1CoinValueinHex = 0x34;
			}
			else if (CoinValue >= 30)
			{
				itoa(CoinValue + 18, Digits, 16);
				Digit1CoinValueinHex = 0x33;
			}
			else if (CoinValue >= 20)
			{
				itoa(CoinValue + 28, Digits, 16);
				Digit1CoinValueinHex = 0x32;
			}
			else
			{
				itoa(CoinValue + 38, Digits, 16);
				Digit1CoinValueinHex = 0x31;
			}

			Digit2CoinValueinHex = strtoul(Digits, NULL, 16);

			helpers->WriteByte(CoinDigitC + 0xE70, Digit1CoinValueinHex, false);
			helpers->WriteByte(CoinDigitC + 0xE71, 0x00, false);
			helpers->WriteByte(CoinDigitC + 0xE72, Digit2CoinValueinHex, false);
		}
		else
		{
			injector::WriteMemoryRaw(CoinDigitAddress + 0x01, "\x00\x20\x00\x20\x00\x43\x00\x52\x00\x45\x00\x44\x00\x49\x00\x54\x00\x53\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00", 29, false);

			itoa(48 + CoinValue, Digits, 16);

			Digit1CoinValueinHex = strtoul(Digits, NULL, 16);

			helpers->WriteByte(CoinDigitC + 0xE70, Digit1CoinValueinHex, false);
		}
	}
}

static void NameScreenInput(Helpers* helpers) //"Fix" stupid name not allowing all characters to be selected
{
	if (!NameEntryHack)
	{
		NameEntryHack = true;
		injector::MakeNOP(0x7CC26 + BaseAddress9, 6);
		injector::MakeNOP(0x7CC2C + BaseAddress9, 6);
		injector::MakeNOP(0x76864 + BaseAddress9, 3);
		injector::MakeNOP(0x76867 + BaseAddress9, 5);
	}

	INT_PTR NameBase = helpers->ReadIntPtr(0x5AE500, true);
	INT_PTR NameBaseOff1 = helpers->ReadIntPtr(NameBase + 0x10, false);
	UINT8 NameNum = helpers->ReadByte(NameBaseOff1 + 0x388, false);
	UINT8 ViewNum = helpers->ReadByte(0x57D618, true);

	DWORD LetterAddr{};

	helpers->WriteFloat32(0x4AD0FC, 0.0, true);

	switch (NameNum)
	{
	case 0:
		injector::WriteMemoryRaw((0x57057D + BaseAddress9), "\x00\x00\x00\x00\x00\x00", 6, true);
		LetterAddr = 0x57057C;
		break;
	case 1:
		LetterAddr = 0x57057D;
		break;
	case 2:
		LetterAddr = 0x57057E;
		break;
	case 3:
		LetterAddr = 0x57057F;
		break;
	case 4:
		LetterAddr = 0x570580;
		break;
	case 5:
		LetterAddr = 0x570581;
		break;
	case 6:
		helpers->WriteFloat32(0x4AD0FC, 1.0, true);
		LetterAddr = 0x570582;
		break;
	}

	if (*ffbOffset2 >= 0xF3)
	{
		helpers->WriteFloat32(0x4AD0FC, 1.0, true);
		switch (ViewNum)
		{
		case 0:
			*(BYTE*)(0x5705A0 + BaseAddress9) = 0x10;
			*(BYTE*)(0x5705A4 + BaseAddress9) = 0x1B;
			*(BYTE*)(LetterAddr + BaseAddress9) = 0x10;
			break;
		case 1:
			*(BYTE*)(0x5705A0 + BaseAddress9) = 0x10;
			*(BYTE*)(0x5705A4 + BaseAddress9) = 0x1A;
			*(BYTE*)(LetterAddr + BaseAddress9) = 0x10;
			break;
		}
	}
	else if (*ffbOffset2 >= 0xEA)
	{
		switch (ViewNum)
		{
		case 0:
			*(BYTE*)(0x5705A0 + BaseAddress9) = 0x5A;
			*(BYTE*)(0x5705A4 + BaseAddress9) = 0x1A;
			*(BYTE*)(LetterAddr + BaseAddress9) = 0x5A;
			break;
		case 1:
			helpers->WriteFloat32(0x4AD0FC, 1.0, true);
			*(BYTE*)(0x5705A0 + BaseAddress9) = 0x10;
			*(BYTE*)(0x5705A4 + BaseAddress9) = 0x1A;
			*(BYTE*)(LetterAddr + BaseAddress9) = 0x10;
			break;
		}
	}
	else if (*ffbOffset2 >= 0xE1)
	{
		*(BYTE*)(0x5705A4 + BaseAddress9) = 0x19;
		switch (ViewNum)
		{
		case 0:
			*(BYTE*)(0x5705A0 + BaseAddress9) = 0x59;
			*(BYTE*)(LetterAddr + BaseAddress9) = 0x59;
			break;
		case 1:
			*(BYTE*)(0x5705A0 + BaseAddress9) = 0x7D;
			*(BYTE*)(LetterAddr + BaseAddress9) = 0x7D;
			break;
		}
	}
	else if (*ffbOffset2 >= 0xD8)
	{
		*(BYTE*)(0x5705A4 + BaseAddress9) = 0x18;
		switch (ViewNum)
		{
		case 0:
			*(BYTE*)(0x5705A0 + BaseAddress9) = 0x58;
			*(BYTE*)(LetterAddr + BaseAddress9) = 0x58;
			break;
		case 1:
			*(BYTE*)(0x5705A0 + BaseAddress9) = 0x7B;
			*(BYTE*)(LetterAddr + BaseAddress9) = 0x7B;
			break;
		}
	}
	else if (*ffbOffset2 >= 0xCF)
	{
		*(BYTE*)(0x5705A4 + BaseAddress9) = 0x17;
		switch (ViewNum)
		{
		case 0:
			*(BYTE*)(0x5705A0 + BaseAddress9) = 0x57;
			*(BYTE*)(LetterAddr + BaseAddress9) = 0x57;
			break;
		case 1:
			*(BYTE*)(0x5705A0 + BaseAddress9) = 0x5E;
			*(BYTE*)(LetterAddr + BaseAddress9) = 0x5E;
			break;
		}
	}
	else if (*ffbOffset2 >= 0xC6)
	{
		*(BYTE*)(0x5705A4 + BaseAddress9) = 0x16;
		switch (ViewNum)
		{
		case 0:
			*(BYTE*)(0x5705A0 + BaseAddress9) = 0x56;
			*(BYTE*)(LetterAddr + BaseAddress9) = 0x56;
			break;
		case 1:
			*(BYTE*)(0x5705A0 + BaseAddress9) = 0x5D;
			*(BYTE*)(LetterAddr + BaseAddress9) = 0x5D;
			break;
		}
	}
	else if (*ffbOffset2 >= 0xBD)
	{
		*(BYTE*)(0x5705A4 + BaseAddress9) = 0x15;
		switch (ViewNum)
		{
		case 0:
			*(BYTE*)(0x5705A0 + BaseAddress9) = 0x55;
			*(BYTE*)(LetterAddr + BaseAddress9) = 0x55;
			break;
		case 1:
			*(BYTE*)(0x5705A0 + BaseAddress9) = 0x5B;
			*(BYTE*)(LetterAddr + BaseAddress9) = 0x5B;
			break;
		}
	}
	else if (*ffbOffset2 >= 0xB4)
	{
		*(BYTE*)(0x5705A4 + BaseAddress9) = 0x14;
		switch (ViewNum)
		{
		case 0:
			*(BYTE*)(0x5705A0 + BaseAddress9) = 0x54;
			*(BYTE*)(LetterAddr + BaseAddress9) = 0x54;
			break;
		case 1:
			*(BYTE*)(0x5705A0 + BaseAddress9) = 0x2F;
			*(BYTE*)(LetterAddr + BaseAddress9) = 0x2F;
			break;
		}
	}
	else if (*ffbOffset2 >= 0xAB)
	{
		*(BYTE*)(0x5705A4 + BaseAddress9) = 0x13;
		switch (ViewNum)
		{
		case 0:
			*(BYTE*)(0x5705A0 + BaseAddress9) = 0x53;
			*(BYTE*)(LetterAddr + BaseAddress9) = 0x53;
			break;
		case 1:
			*(BYTE*)(0x5705A0 + BaseAddress9) = 0x3E;
			*(BYTE*)(LetterAddr + BaseAddress9) = 0x3E;
			break;
		}
	}
	else if (*ffbOffset2 >= 0xA2)
	{
		*(BYTE*)(0x5705A4 + BaseAddress9) = 0x12;
		switch (ViewNum)
		{
		case 0:
			*(BYTE*)(0x5705A0 + BaseAddress9) = 0x52;
			*(BYTE*)(LetterAddr + BaseAddress9) = 0x52;
			break;
		case 1:
			*(BYTE*)(0x5705A0 + BaseAddress9) = 0x3C;
			*(BYTE*)(LetterAddr + BaseAddress9) = 0x3C;
			break;
		}
	}
	else if (*ffbOffset2 >= 0x99)
	{
		*(BYTE*)(0x5705A4 + BaseAddress9) = 0x11;
		switch (ViewNum)
		{
		case 0:
			*(BYTE*)(0x5705A0 + BaseAddress9) = 0x51;
			*(BYTE*)(LetterAddr + BaseAddress9) = 0x51;
			break;
		case 1:
			*(BYTE*)(0x5705A0 + BaseAddress9) = 0x40;
			*(BYTE*)(LetterAddr + BaseAddress9) = 0x40;
			break;
		}
	}
	else if (*ffbOffset2 >= 0x90)
	{
		*(BYTE*)(0x5705A4 + BaseAddress9) = 0x10;
		switch (ViewNum)
		{
		case 0:
			*(BYTE*)(0x5705A0 + BaseAddress9) = 0x50;
			*(BYTE*)(LetterAddr + BaseAddress9) = 0x50;
			break;
		case 1:
			*(BYTE*)(0x5705A0 + BaseAddress9) = 0x5F;
			*(BYTE*)(LetterAddr + BaseAddress9) = 0x5F;
			break;
		}
	}
	else if (*ffbOffset2 >= 0x87)
	{
		*(BYTE*)(0x5705A4 + BaseAddress9) = 0x0F;
		switch (ViewNum)
		{
		case 0:
			*(BYTE*)(0x5705A0 + BaseAddress9) = 0x4F;
			*(BYTE*)(LetterAddr + BaseAddress9) = 0x4F;
			break;
		case 1:
			*(BYTE*)(0x5705A0 + BaseAddress9) = 0x2D;
			*(BYTE*)(LetterAddr + BaseAddress9) = 0x2D;
			break;
		}
	}
	else if (*ffbOffset2 >= 0x7E)
	{
		*(BYTE*)(0x5705A4 + BaseAddress9) = 0x0E;
		switch (ViewNum)
		{
		case 0:
			*(BYTE*)(0x5705A0 + BaseAddress9) = 0x4E;
			*(BYTE*)(LetterAddr + BaseAddress9) = 0x4E;
			break;
		case 1:
			*(BYTE*)(0x5705A0 + BaseAddress9) = 0x21;
			*(BYTE*)(LetterAddr + BaseAddress9) = 0x21;
			break;
		}
	}
	else if (*ffbOffset2 >= 0x75)
	{
		*(BYTE*)(0x5705A4 + BaseAddress9) = 0x0D;
		switch (ViewNum)
		{
		case 0:
			*(BYTE*)(0x5705A0 + BaseAddress9) = 0x4D;
			*(BYTE*)(LetterAddr + BaseAddress9) = 0x4D;
			break;
		case 1:
			*(BYTE*)(0x5705A0 + BaseAddress9) = 0x3F;
			*(BYTE*)(LetterAddr + BaseAddress9) = 0x3F;
			break;
		}
	}
	else if (*ffbOffset2 >= 0x6C)
	{
		*(BYTE*)(0x5705A4 + BaseAddress9) = 0x0C;
		switch (ViewNum)
		{
		case 0:
			*(BYTE*)(0x5705A0 + BaseAddress9) = 0x4C;
			*(BYTE*)(LetterAddr + BaseAddress9) = 0x4C;
			break;
		case 1:
			*(BYTE*)(0x5705A0 + BaseAddress9) = 0x2E;
			*(BYTE*)(LetterAddr + BaseAddress9) = 0x2E;
			break;
		}
	}
	else if (*ffbOffset2 >= 0x63)
	{
		*(BYTE*)(0x5705A4 + BaseAddress9) = 0x0B;
		switch (ViewNum)
		{
		case 0:
			*(BYTE*)(0x5705A0 + BaseAddress9) = 0x4B;
			*(BYTE*)(LetterAddr + BaseAddress9) = 0x4B;
			break;
		case 1:
			*(BYTE*)(0x5705A0 + BaseAddress9) = 0x20;
			*(BYTE*)(LetterAddr + BaseAddress9) = 0x20;
			break;
		}
	}
	else if (*ffbOffset2 >= 0x5A)
	{
		*(BYTE*)(0x5705A4 + BaseAddress9) = 0x0A;
		switch (ViewNum)
		{
		case 0:
			*(BYTE*)(0x5705A0 + BaseAddress9) = 0x4A;
			*(BYTE*)(LetterAddr + BaseAddress9) = 0x4A;
			break;
		case 1:
			*(BYTE*)(0x5705A0 + BaseAddress9) = 0x39;
			*(BYTE*)(LetterAddr + BaseAddress9) = 0x39;
			break;
		}
	}
	else if (*ffbOffset2 >= 0x51)
	{
		*(BYTE*)(0x5705A4 + BaseAddress9) = 0x09;
		switch (ViewNum)
		{
		case 0:
			*(BYTE*)(0x5705A0 + BaseAddress9) = 0x49;
			*(BYTE*)(LetterAddr + BaseAddress9) = 0x49;
			break;
		case 1:
			*(BYTE*)(0x5705A0 + BaseAddress9) = 0x38;
			*(BYTE*)(LetterAddr + BaseAddress9) = 0x38;
			break;
		}
	}
	else if (*ffbOffset2 >= 0x48)
	{
		*(BYTE*)(0x5705A4 + BaseAddress9) = 0x08;
		switch (ViewNum)
		{
		case 0:
			*(BYTE*)(0x5705A0 + BaseAddress9) = 0x48;
			*(BYTE*)(LetterAddr + BaseAddress9) = 0x48;
			break;
		case 1:
			*(BYTE*)(0x5705A0 + BaseAddress9) = 0x37;
			*(BYTE*)(LetterAddr + BaseAddress9) = 0x37;
			break;
		}
	}
	else if (*ffbOffset2 >= 0x3F)
	{
		*(BYTE*)(0x5705A4 + BaseAddress9) = 0x07;
		switch (ViewNum)
		{
		case 0:
			*(BYTE*)(0x5705A0 + BaseAddress9) = 0x47;
			*(BYTE*)(LetterAddr + BaseAddress9) = 0x47;
			break;
		case 1:
			*(BYTE*)(0x5705A0 + BaseAddress9) = 0x36;
			*(BYTE*)(LetterAddr + BaseAddress9) = 0x36;
			break;
		}

	}
	else if (*ffbOffset2 >= 0x36)
	{
		*(BYTE*)(0x5705A4 + BaseAddress9) = 0x06;
		switch (ViewNum)
		{
		case 0:
			*(BYTE*)(0x5705A0 + BaseAddress9) = 0x46;
			*(BYTE*)(LetterAddr + BaseAddress9) = 0x46;
			break;
		case 1:
			*(BYTE*)(0x5705A0 + BaseAddress9) = 0x35;
			*(BYTE*)(LetterAddr + BaseAddress9) = 0x35;
			break;
		}
	}
	else if (*ffbOffset2 >= 0x2D)
	{
		*(BYTE*)(0x5705A4 + BaseAddress9) = 0x05;
		switch (ViewNum)
		{
		case 0:
			*(BYTE*)(0x5705A0 + BaseAddress9) = 0x45;
			*(BYTE*)(LetterAddr + BaseAddress9) = 0x45;
			break;
		case 1:
			*(BYTE*)(0x5705A0 + BaseAddress9) = 0x34;
			*(BYTE*)(LetterAddr + BaseAddress9) = 0x34;
			break;
		}
	}
	else if (*ffbOffset2 >= 0x24)
	{
		*(BYTE*)(0x5705A4 + BaseAddress9) = 0x04;
		switch (ViewNum)
		{
		case 0:
			*(BYTE*)(0x5705A0 + BaseAddress9) = 0x44;
			*(BYTE*)(LetterAddr + BaseAddress9) = 0x44;
			break;
		case 1:
			*(BYTE*)(0x5705A0 + BaseAddress9) = 0x33;
			*(BYTE*)(LetterAddr + BaseAddress9) = 0x33;
			break;
		}
	}
	else if (*ffbOffset2 >= 0x1B)
	{
		*(BYTE*)(0x5705A4 + BaseAddress9) = 0x03;
		switch (ViewNum)
		{
		case 0:
			*(BYTE*)(0x5705A0 + BaseAddress9) = 0x43;
			*(BYTE*)(LetterAddr + BaseAddress9) = 0x43;
			break;
		case 1:
			*(BYTE*)(0x5705A0 + BaseAddress9) = 0x32;
			*(BYTE*)(LetterAddr + BaseAddress9) = 0x32;
			break;
		}
	}
	else if (*ffbOffset2 >= 0x12)
	{
		*(BYTE*)(0x5705A4 + BaseAddress9) = 0x02;
		switch (ViewNum)
		{
		case 0:
			*(BYTE*)(0x5705A0 + BaseAddress9) = 0x42;
			*(BYTE*)(LetterAddr + BaseAddress9) = 0x42;
			break;
		case 1:
			*(BYTE*)(0x5705A0 + BaseAddress9) = 0x31;
			*(BYTE*)(LetterAddr + BaseAddress9) = 0x31;
			break;
		}
	}
	else if (*ffbOffset2 >= 0x09)
	{
		*(BYTE*)(0x5705A4 + BaseAddress9) = 0x01;
		switch (ViewNum)
		{
		case 0:
			*(BYTE*)(0x5705A0 + BaseAddress9) = 0x41;
			*(BYTE*)(LetterAddr + BaseAddress9) = 0x41;
			break;
		case 1:
			*(BYTE*)(0x5705A0 + BaseAddress9) = 0x30;
			*(BYTE*)(LetterAddr + BaseAddress9) = 0x30;
			break;
		}
	}
	else if (*ffbOffset2 >= 0x00)
	{
		helpers->WriteFloat32(0x4AD0FC, -1.0, true);
		*(BYTE*)(0x5705A0 + BaseAddress9) = 0x08;
		*(BYTE*)(0x5705A4 + BaseAddress9) = 0x00;
		*(BYTE*)(LetterAddr + BaseAddress9) = 0x00;
	}
}

DWORD WINAPI InputRT9(LPVOID lpParam)
{
	int deltaTimer = 16;

	while (true)
	{
		GameState = *(BYTE*)(0x570190 + BaseAddress9);
		if (GameState != DirtyLastLoggedGameState)
		{
			char diagnostic[64] = {};
			sprintf_s(
				diagnostic,
				"TP_DIRTY_DIAG state=%u",
				static_cast<unsigned int>(GameState));
			DirtyOutputDebugString(diagnostic);
			WriteDirtyStateDiagnostic("LastGameState", diagnostic);
			DirtyLastLoggedGameState = GameState;
		}
		BYTE Chosen = *(BYTE*)(0x5705E8 + BaseAddress9);

		if (GameState == 0x05)
		{
			if (!MenuHack)
			{
				MenuHack = true;
				injector::MakeNOP((0x78A27 + BaseAddress9), 6, true);
			}
		}
		else if (GameState == 0x06 || GameState == 0x08 || GameState == 0x12)
		{
			if (MenuHack)
			{
				MenuHack = false;
				MenuHackStopWriting = false;

				injector::WriteMemory((0x78A27 + BaseAddress9), 0x03448689, true);
				injector::WriteMemory((0x78A2B + BaseAddress9), 0x8E890000, true);
			}
		}

		if (MenuHack)
		{
			if (Chosen == 0x01)
			{
				MenuHackStopWriting = true;
			}

			if (!MenuHackStopWriting)
			{
				if (*ffbOffset2 >= 0xEE)
				{
					*(BYTE*)(0x570234 + BaseAddress9) = 0x0A;
				}
				else if (*ffbOffset2 >= 0xDD)
				{
					*(BYTE*)(0x570234 + BaseAddress9) = 0x0C;
				}
				else if (*ffbOffset2 >= 0xCC)
				{
					*(BYTE*)(0x570234 + BaseAddress9) = 0x08;
				}
				else if (*ffbOffset2 >= 0xBB)
				{
					*(BYTE*)(0x570234 + BaseAddress9) = 0x0D;
				}
				else if (*ffbOffset2 >= 0xAA)
				{
					*(BYTE*)(0x570234 + BaseAddress9) = 0x0E;
				}
				else if (*ffbOffset2 >= 0x99)
				{
					*(BYTE*)(0x570234 + BaseAddress9) = 0x09;
				}
				else if (*ffbOffset2 >= 0x88)
				{
					*(BYTE*)(0x570234 + BaseAddress9) = 0x0B;
				}
				else if (*ffbOffset2 >= 0x77)
				{
					*(BYTE*)(0x570234 + BaseAddress9) = 0x02;
				}
				else if (*ffbOffset2 >= 0x66)
				{
					*(BYTE*)(0x570234 + BaseAddress9) = 0x00;
				}
				else if (*ffbOffset2 >= 0x55)
				{
					*(BYTE*)(0x570234 + BaseAddress9) = 0x04;
				}
				else if (*ffbOffset2 >= 0x44)
				{
					*(BYTE*)(0x570234 + BaseAddress9) = 0x06;
				}
				else if (*ffbOffset2 >= 0x33)
				{
					*(BYTE*)(0x570234 + BaseAddress9) = 0x05;
				}
				else if (*ffbOffset2 >= 0x22)
				{
					*(BYTE*)(0x570234 + BaseAddress9) = 0x03;
				}
				else if (*ffbOffset2 >= 0x11)
				{
					*(BYTE*)(0x570234 + BaseAddress9) = 0x01;
				}
				else if (*ffbOffset2 >= 0x00)
				{
					*(BYTE*)(0x570234 + BaseAddress9) = 0x10;
				}
			}
		}

		if (GameState == 0x09)
			NameScreenInput(0);
		else
		{
			if (NameEntryHack)
			{
				NameEntryHack = false;
				injector::WriteMemoryRaw((0x7CC26 + BaseAddress9), "\x89\x83\xB4\x06\x00\x00", 6, true);
				injector::WriteMemoryRaw((0x7CC2C + BaseAddress9), "\x89\xB3\xB0\x06\x00\x00", 6, true);
				injector::WriteMemoryRaw((0x76864 + BaseAddress9), "\x88\x0C\x06", 3, true);
				injector::WriteMemoryRaw((0x76867 + BaseAddress9), "\xC6\x44\x06\x01\x00", 5, true);
			}
		}

		if (ToBool(config["General"]["Windowed"]))
		{
			if (hWndRT9 == 0)
			{
				hWndRT9 = FindWindowW(NULL, TEXT("Dirty Drivin'"));
			}
			else if ((GetWindowLongPtrA(hWndRT9, GWL_STYLE) != 0x14CA0000))
			{
				SetWindowLongPtr(hWndRT9, GWL_STYLE, 0x14CA0000);
				SetWindowLongPtr(hWndRT9, GWL_EXSTYLE, 0x00000000);
				SetWindowPos(hWndRT9, HWND_TOPMOST, 0, 0, 1366, 797, SWP_SHOWWINDOW | SWP_NOZORDER);
			}
		}

		if (!ToBool(config["General"]["Free Play"]))
		{
			CoinInput(0);
		}

		// buttons see bitwise values in TPui//RawThrills.cs
		// START
		if (*ffbOffset & 0x08)
		{
			if (STARTpressed == false)
			{
				keybd_event(VK_SPACE, 0, 0, 0);
				STARTpressed = true;
			}
		}
		else
		{
			if (STARTpressed == true)
			{
				keybd_event(VK_SPACE, 0, KEYEVENTF_KEYUP, 0);
				STARTpressed = false;
			}
		}

		// BUTTON 1/ CRANK BACKWARD
		// !!! NOTE: CRANK FORWARD ONLY USED FOR GAS WHEN CONVERTING FROM H2Overdrive CABINET !!! //
		if (*ffbOffset & 0x100)
		{
			if (button1pressed == false)
			{
				keybd_event(VK_SPACE, 0, 0, 0);
				button1pressed = true;
			}
		}
		else
		{
			if (button1pressed == true)
			{
				keybd_event(VK_SPACE, 0, KEYEVENTF_KEYUP, 0);
				button1pressed = false;
			}
		}

		// BUTTON 2/ VIEW
		if (*ffbOffset & 0x200)
		{
			if (button2pressed == false)
			{
				keybd_event(0x56, 0, 0, 0);
				button2pressed = true;
			}
		}
		else
		{
			if (button2pressed == true)
			{
				keybd_event(0x56, 0, KEYEVENTF_KEYUP, 0);
				button2pressed = false;
			}
		}

		// WHEEL
		if ((GameState == 0x06) && (*ffbOffset2 > 0x60 && *ffbOffset2 < 0x70))
		{
			if (!RiptideHack)
			{
				RiptideHack = true;
				*(BYTE*)(0x44BAD0 + BaseAddress9) = 0x08;
				injector::WriteMemory<float>((0x4AD0FC + BaseAddress9), -1.0f, true);
			}
		}
		else
		{
			if (RiptideHack)
			{
				RiptideHack = false;
				*(BYTE*)(0x44BAD0 + BaseAddress9) = 0x0B;
			}

			if (GameState != 9)
			{
				int iWheel = (((float)*ffbOffset2) - 128);
				float wheel = (iWheel * 0.0078125f);
				injector::WriteMemory<float>((0x4AD0FC + BaseAddress9), wheel, true);
			}
		}
		//// GAS
		float gas = (float)*ffbOffset3 / 255.0f;
		float brake = (float)*ffbOffset4 / 255.0f;
		// injector::WriteMemory<float>((0x4AD0F8 + BaseAddress9), gas, true);
		// BRAKE BUTTON HACK = if brake pressed gas is reduced
		injector::WriteMemory<float>((0x4AD0F8 + BaseAddress9), gas - brake, true);

		//DEBUG//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		// info(true, "test value %f %f ", *ffbOffset2, x);
		//DEBUG//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

		Sleep(deltaTimer);
	}

	return 0;
}

D3DPRESENT_PARAMETERS* pPresentationParameters_RT9;
uintptr_t d3dcall;
void __stdcall D3D9CreateParamPatch()
{
#if _M_IX86
	__asm mov d3dcall, edx
#endif
	DirtyOutputDebugString("TP_DIRTY_DIAG d3d9-create");
	WriteDirtyStateDiagnostic("D3D9Create", "1");
#if _M_IX86
	__asm mov edx, [ebp + 0x1c]
		__asm mov pPresentationParameters_RT9, edx
	pPresentationParameters_RT9->Windowed = TRUE;
	pPresentationParameters_RT9->FullScreen_RefreshRateInHz = 0;
	pPresentationParameters_RT9->hDeviceWindow = NULL;
	injector::WriteMemoryRaw(0x63B332, "\xFF\xD2\x3D\x68\x08", 5, true);
	__asm mov edx, pPresentationParameters_RT9
	__asm mov[ebp + 0x1c], edx
	__asm mov edx, d3dcall
#endif
	return;
}

static InitFunction DirtyDrivinFunc([]()
	{
		DirtyOutputDebugString("TP_DIRTY_DIAG init");
		WriteDirtyStateDiagnostic("Init", "1");
		WriteDirtyStateDiagnostic("WindowCreatePre", "pending");
		WriteDirtyStateDiagnostic("WindowCreate", "pending");
		WriteDirtyStateDiagnostic("InitialWindowCreatePre", "pending");
		WriteDirtyStateDiagnostic("InitialWindowCreate", "pending");
		WriteDirtyDiagnosticProfile(
			"Crash",
			"Last",
			"pending",
			".\\DirtyDrivinCrashDiagnostic.ini");
		WriteDirtyDiagnosticProfile(
			"InitSteps",
			"Step1_004044D0",
			"pending",
			".\\DirtyDrivinStateDiagnostic.ini");
		const bool runningUnderWine = IsDirtyDrivinRunningUnderWine();
		if (runningUnderWine)
		{
			// These compatibility hooks address Wine/Box86 startup and x87
			// behavior only. Native Windows retains the established title path.
			if (IsDirtyDiagnosticsEnabled())
			{
				AddVectoredExceptionHandler(1, DirtyExceptionDiagnostic);
				DirtyExitProcessOri =
					*reinterpret_cast<decltype(DirtyExitProcessOri)*>(
						BaseAddress9 + 0x32B320);
				DirtyTerminateProcessOri =
					*reinterpret_cast<decltype(DirtyTerminateProcessOri)*>(
						BaseAddress9 + 0x32B324);
				DirtyPostQuitMessageOri =
					*reinterpret_cast<decltype(DirtyPostQuitMessageOri)*>(
						BaseAddress9 + 0x32B524);
				injector::WriteMemory<uintptr_t>(
					BaseAddress9 + 0x32B320,
					reinterpret_cast<uintptr_t>(DirtyExitProcessHook),
					true);
				injector::WriteMemory<uintptr_t>(
					BaseAddress9 + 0x32B324,
					reinterpret_cast<uintptr_t>(DirtyTerminateProcessHook),
					true);
				injector::WriteMemory<uintptr_t>(
					BaseAddress9 + 0x32B524,
					reinterpret_cast<uintptr_t>(DirtyPostQuitMessageHook),
					true);
				injector::WriteMemory<uintptr_t>(
					0x00769E00,
					reinterpret_cast<uintptr_t>(DirtyAppThreadMainHook),
					true);
				injector::WriteMemory<uintptr_t>(
					0x00561163,
					reinterpret_cast<uintptr_t>(DirtyAppUpdateHook),
					true);
				injector::MakeCALL(0x00403A7B, DirtyInitStep1Hook);
				injector::MakeCALL(0x00403AD4, DirtyInitStep2Hook);
				injector::MakeCALL(0x00403B51, DirtyInitStep4Hook);
				injector::MakeCALL(0x00403B5A, DirtyInitStep5Hook);
				injector::MakeCALL(0x0040111E, DirtyLuxInitHook);
			}

			injector::MakeCALL(0x0058F3E8, DirtyWindowCreateHook);
			injector::MakeCALL(0x0058F3BF, DirtyInitialWindowCreateHook);
			injector::MakeCALL(0x00403AF9, DirtyInitStep3Hook);
			injector::MakeNOP(0x005E2A42, 0x14);
			injector::MakeCALL(
				0x005E2A42,
				DirtyReducedFifthWaveHook);

			DirtyCreateFileAOri =
				iatHook("kernel32.dll", DirtyCreateFileAHook, "CreateFileA");
			DirtyCreateFileWOri =
				iatHook("kernel32.dll", DirtyCreateFileWHook, "CreateFileW");
			if (IsDirtyDiagnosticsEnabled())
			{
				DirtySocketOri =
					iatHook("ws2_32.dll", DirtySocketHook, 23);
			}
		}

		// PATCHING EXE AT RUNTIME (reboots, network, filepath, config, CRC...
		injector::WriteMemoryRaw((0x335DD4 + BaseAddress9), "\x44\x69\x72\x74\x79\x20\x44\x72\x69\x76\x69\x6E\x27\x00", 14, true); // edit window caption text
		injector::WriteMemoryRaw((0x3B00 + BaseAddress9), "\xEB", 1, true);
		if (ToBool(config["General"]["Free Play"]))
		{
			injector::WriteMemoryRaw((0x43B88 + BaseAddress9), "\xC6\x05\x6C\xFE\x96\x00\x01\x90\x90\xC6\x05\x58\x4A\x88\x00\x01\xB8\x58\x2A\x74\x00\xEB", 22, true);
		}
		else
		{
			injector::WriteMemoryRaw((0x43B88 + BaseAddress9), "\x83\x3D\x6C\xFE\x96\x00\x00\x74\x0E\x83\x3D\x58\x4A\x88\x00\x00\xB8\x58\x2A\x74\x00\x75", 22, true);

			LPCWSTR Open = L"open .\\Coin\\Coin.mp3 type mpegvideo";
			int Opening = mciSendString(Open, NULL, 0, 0);
			if (Opening != 0)
			{
				SoundFail = true;
			}
		}
		injector::WriteMemoryRaw((0x65447 + BaseAddress9), "\xEB\x0B\x90\x90", 4, true);
		injector::WriteMemoryRaw((0xBC9E8 + BaseAddress9), "\xEB", 1, true);
		injector::WriteMemoryRaw((0x1B5BF5 + BaseAddress9), "\x90\x90", 2, true);
		injector::WriteMemoryRaw((0x239B5C + BaseAddress9), "\xEB", 1, true);
		injector::WriteMemoryRaw((0x2F236E + BaseAddress9), "\x90\x90\x90", 3, true);
		injector::WriteMemoryRaw((0x2FB88F + BaseAddress9), "\x90\x90", 2, true);
		injector::WriteMemoryRaw((0x337A07 + BaseAddress9), "\x35", 1, true);

		//CONTROLS PATCH
		injector::MakeNOP((0x5159A + BaseAddress9), 6, true);
		injector::MakeNOP((0x515AC + BaseAddress9), 6, true);
		injector::MakeNOP((0x514BB + BaseAddress9), 4, true);
		injector::MakeNOP((0x514C0 + BaseAddress9), 3, true);

		//TVVS CONTROLS PATCH
		injector::MakeNOP((0x506C57), 6, true);
		injector::MakeNOP((0x506C5D), 6, true);
		injector::MakeNOP((0x506C63), 6, true);
		injector::MakeNOP((0x506C69), 6, true);
		injector::MakeNOP((0x506C6F), 6, true);
		injector::MakeNOP((0x506C75), 6, true);
		injector::MakeNOP((0x506C7B), 6, true);

		injector::MakeNOP((0x506883), 2, true);
		injector::MakeNOP((0x54DDE0), 2, true);
		injector::MakeNOP((0x54DD3B), 2, true);

		//tvvs controls patch #1
		injector::MakeJMP(0x506EEF, 0x729F00);
		injector::MakeCALL(0x729F00, ServiceControlsPatch);
		injector::WriteMemoryRaw(0x729F05, "\xBF\x00\x00\x00\x00", 5, true);
		injector::WriteMemoryRaw(0x729F0A, "\x39\x3D\x30\xC3\x98\x00", 6, true);
		injector::MakeJMP(0x729F10, 0x506EF5);
		//tvvs controls patch #2
		injector::MakeJMP(0x500A70, 0x729F20);
		injector::MakeCALL(0x729F20, ServiceControlsPatch);
		injector::WriteMemoryRaw(0x729F25, "\x83\x3D\x80\xC3\x98\x00\x00", 7, true);
		injector::WriteMemoryRaw(0x729F2C, "\x0F\x84\xB9\x6B\xDD\xFF", 6, true);
		injector::MakeJMP(0x729F32, 0x500A79);

		CreateThread(NULL, 0, InputRT9, NULL, 0, NULL);

		// auto accl off
		if (ToBool(config["General"]["AutoAcclOff"]))
		{
			injector::MakeNOP((0xAA6E6 + BaseAddress9), 6, true);
			// alternative patch
			// injector::WriteMemoryRaw((0x96fd70), "\x00", 1, true);
		}

		if (ToBool(config["General"]["Windowed"]))
		{
			injector::MakeJMP(0x63B332, 0x729EF0);
			injector::MakeCALL(0x729EF0, D3D9CreateParamPatch);
			injector::MakeJMP(0x729EF5, 0x63B332);
		}

	}, GameID::DirtyDrivin);
#endif
#pragma optimize("", on)
