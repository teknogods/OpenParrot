#include <StdInc.h>
#include <Utility/InitFunction.h>
#include "Global.h"
#include "Utility/GameDetect.h"
#include "Utility/Hooking.Patterns.h"
#include <shlwapi.h>
#include <Windows.h>
#include "Utility/Helper.h"
#include <TlHelp32.h>

#pragma comment(lib,"shlwapi.lib")
#pragma comment(lib,"Winmm.lib")

#pragma optimize("", off)

extern int* ffbOffset;
extern int* ffbOffset2;
extern int* ffbOffset3;
extern int* ffbOffset4;
extern int* ffbOffset5;
extern int* ffbOffset6;
extern int* ffbOffset7;
extern int* ffbOffset8;
extern int* ffbOffset9;

static DWORD xdbg;
static HWND g_HWND = NULL;
static DWORD ProcessID;
static bool DisableGhosting;
static bool EnableSuspend;
static bool PausePressed;
static bool ResumeSuspend;
static bool SuspendPressedOn;
static char SuspendBuf[MAX_PATH];
static char PauseKeyChar[256];
static char ExitKeyChar[256];
static int PauseKeyValue;
static int ExitKeyValue;

extern void PauseGameFixes(Helpers* helpers);
extern void ResetPauseGameFixes(Helpers* helpers);

void *__cdecl memcpy_0(void *a1, const void *a2, size_t a3)
{
	__try
	{
		memcpy(a1, a2, a3);
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{

	}
	return 0;
}

static HMODULE blaster;

// used in SR3 and Ford Racing
BOOL WINAPI ReadFileHooked(_In_ HANDLE hFile, _Out_writes_bytes_to_opt_(nNumberOfBytesToRead, *lpNumberOfBytesRead) __out_data_source(FILE) LPVOID lpBuffer, _In_ DWORD nNumberOfBytesToRead, _Out_opt_ LPDWORD lpNumberOfBytesRead, _Inout_opt_ LPOVERLAPPED lpOverlapped)
{
	static HANDLE file = CreateFileA("\\\\.\\pipe\\TeknoParrotPipe", GENERIC_READ, 0, nullptr, OPEN_EXISTING, 0, NULL);

	ReadFile(file, lpBuffer, nNumberOfBytesToRead, lpNumberOfBytesRead, lpOverlapped);

	return TRUE;
}

std::wstring utf8_decode(const std::string& str)
{
	if (str.empty()) return std::wstring();
	int size_needed = MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), NULL, 0);
	std::wstring wstrTo(size_needed, 0);
	MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), &wstrTo[0], size_needed);
	return wstrTo;
}

static void QuitGame()
{
	if (GameDetect::currentGame == GameID::GHA)
		system("taskkill /f /im GHA.exe"); // is this necessary?

	if ((GameDetect::currentGame == GameID::StarWarsEs3X) || (GameDetect::currentGame == GameID::StarWarsJapEs3X) || (GameDetect::currentGame == GameID::StarWarsEs3XLauncher) || (GameDetect::currentGame == GameID::StarWarsJapEs3XLauncher))
	{
		system("taskkill /f /im RSLauncher.exe");
		system("taskkill /f /im SWArcGame-Win64-Shipping.exe");
	}

	TerminateProcess(GetCurrentProcess(), 0);
}

static void SuspendThreads(DWORD ProcessId, DWORD ThreadId, bool Suspend)
{
	HANDLE hThread = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
	if (hThread != INVALID_HANDLE_VALUE)
	{
		THREADENTRY32 te;
		te.dwSize = sizeof(te);
		if (Thread32First(hThread, &te))
		{
			do
			{
				if (te.dwSize >= FIELD_OFFSET(THREADENTRY32, th32OwnerProcessID) + sizeof(te.th32OwnerProcessID))
				{
					if (te.th32ThreadID != ThreadId && te.th32OwnerProcessID == ProcessId)
					{
						HANDLE thread = ::OpenThread(THREAD_ALL_ACCESS, FALSE, te.th32ThreadID);
						if (thread != NULL)
						{
							if (Suspend)
								SuspendThread(thread);
							else
								ResumeThread(thread);

							CloseHandle(thread);
						}
					}
				}
				te.dwSize = sizeof(te);
			} while (Thread32Next(hThread, &te));
		}
		CloseHandle(hThread);

		if (Suspend)
		{
			EnableSuspend = true;
			ResumeSuspend = false;
		}
		else
		{
			EnableSuspend = false;
			ResumeSuspend = true;
			ResetPauseGameFixes(0);
		}
	}
}

static DWORD MyGetProcessId(LPCTSTR ProcessName)
{
	PROCESSENTRY32 pt;
	HANDLE hsnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	pt.dwSize = sizeof(PROCESSENTRY32);
	if (Process32First(hsnap, &pt)) {
		do {
			if (!lstrcmpi(pt.szExeFile, ProcessName)) {
				CloseHandle(hsnap);
				return pt.th32ProcessID;
			}
		} while (Process32Next(hsnap, &pt));
	}
	CloseHandle(hsnap);
	return 0;
}

static std::string getFileName(std::string path)
{
	std::string filename = path;
	const size_t last_slash_idx = filename.find_last_of("\\/");
	if (std::string::npos != last_slash_idx) {
		filename.erase(0, last_slash_idx + 1);
	}
	const size_t period_idx = filename.rfind('.');

	return filename;
}

static BOOL CALLBACK EnumWindowsProcMy(HWND hwnd, LPARAM lParam)
{
	DWORD lpdwProcessId;
	GetWindowThreadProcessId(hwnd, &lpdwProcessId);
	if (lpdwProcessId == lParam)
	{
		g_HWND = hwnd;
		return FALSE;
	}
	return TRUE;
}

DWORD WINAPI GlobalGameThread(__in  LPVOID lpParameter)
{
	while (true)
	{
		if (GetAsyncKeyState(ExitKeyValue))
		{
			/*
#ifdef _DEBUG
#if _M_IX86
			xdbg = MyGetProcessId(L"x32dbg.exe");
#else
			xdbg = MyGetProcessId(L"x64dbg.exe");
#endif
			EnumWindows(EnumWindowsProcMy, xdbg);
#endif
*/
			if (g_HWND == NULL)
				QuitGame();
			else
				g_HWND = NULL;
		}

		if (GetAsyncKeyState(PauseKeyValue))
		{
			if (!SuspendPressedOn)
			{
				SuspendPressedOn = true;

				if (!DisableGhosting)
				{
					DisableGhosting = true;
					DisableProcessWindowsGhosting();
				}

				if (!EnableSuspend)
					PausePressed = true;

				if (EnableSuspend)
					PausePressed = false;
			}
		}
		else
		{
			if (SuspendPressedOn)
				SuspendPressedOn = false;
		}

		if (PausePressed && !EnableSuspend)
			SuspendThreads(ProcessID, GetCurrentThreadId(), true);

		if (!PausePressed && EnableSuspend && !ResumeSuspend)
			SuspendThreads(ProcessID, GetCurrentThreadId(), false);

		if (EnableSuspend)
			PauseGameFixes(0);

		Sleep(16);
	}
}

/* WINDOW HOOKS */

DWORD g_windowStyle;
DWORD hide_windowStyle;

int g_x = 0;
int g_y = 0;

int g_width = 0;
int g_height = 0;

HWND hwndWindowA;
HWND hwndWindowW;

HWND WINAPI CreateWindowExAHk(DWORD dwExStyle, LPCSTR lpClassName, LPCSTR lpWindowName, DWORD dwStyle, int x, int y, int nWidth, int nHeight, HWND hWndParent, HMENU hMenu, HINSTANCE hInstance, LPVOID lpParam)
{
#ifdef _DEBUG
	info("CreateWindowExAHk called");
#endif
	if (lpWindowName)
	{
		dwStyle = g_windowStyle;
	}
	if (lpWindowName == NULL)
	{
		lpWindowName = lpClassName;
		dwStyle = g_windowStyle;
	}

	if (GameDetect::currentGame == GameID::ElevatorActionDeathParade)
	{
		if (strcmp(lpWindowName, "CriSmpSoundOutput") == 0)
			dwStyle = hide_windowStyle;
		else
		{
			nWidth = 720;
			nHeight = 1280;
		}
	}

	if (GameDetect::currentGame == GameID::MusicGunGun2)
	{
		if (strcmp(lpWindowName, "CriSmpSoundOutput") == 0)
			dwStyle = hide_windowStyle;
		else
			lpWindowName = "OpenParrot - Music GunGun 2";
	}

	if (GameDetect::currentGame == GameID::HauntedMuseum)
	{
		if (strcmp(lpWindowName, "CriDSoundOutput") == 0)
			dwStyle = hide_windowStyle;
		else
			lpWindowName = "OpenParrot - Haunted Museum";
	}

	if (GameDetect::currentGame == GameID::HauntedMuseum2100 || GameDetect::currentGame == GameID::HauntedMuseum2101J)
	{
		if (strcmp(lpWindowName, "CriDSoundOutput") == 0)
			dwStyle = hide_windowStyle;
		else
			lpWindowName = "OpenParrot - Haunted Museum II";
	}

	if (GameDetect::currentGame == GameID::GaiaAttack4)
	{
		if (strcmp(lpWindowName, "CriDSoundOutput") == 0)
			dwStyle = hide_windowStyle;
		else
			lpWindowName = "OpenParrot - Gaia Attack 4";
	}

	// Make window pos centered
	g_x = (GetSystemMetrics(SM_CXSCREEN) - nWidth) / 2;
	g_y = (GetSystemMetrics(SM_CYSCREEN) - nHeight) / 2;

	HWND thisWindow = CreateWindowExA(dwExStyle, lpClassName, lpWindowName, dwStyle, g_x, g_y, nWidth, nHeight, hWndParent, hMenu, hInstance, lpParam);

	if (lpWindowName)
	{
		hwndWindowA = thisWindow;
	}

	return thisWindow;
}

HWND WINAPI CreateWindowExWHk(DWORD dwExStyle, LPCWSTR lpClassName, LPCWSTR lpWindowName, DWORD dwStyle, int x, int y, int nWidth, int nHeight, HWND hWndParent, HMENU hMenu, HINSTANCE hInstance, LPVOID lpParam)
{
#ifdef _DEBUG
	info("CreateWindowExWHk called");
#endif
	// Calculate window pos centered
	g_x = (GetSystemMetrics(SM_CXSCREEN) - nWidth) / 2;
	g_y = (GetSystemMetrics(SM_CYSCREEN) - nHeight) / 2;


	if (GameDetect::currentGame == GameID::SF4 && x != 0 && y != 0)
	{
		dwStyle = g_windowStyle;
		return CreateWindowExW(dwExStyle, lpClassName, L"OpenParrot - Street Fighter IV", dwStyle, x, y, 1280, 720, hWndParent, hMenu, hInstance, lpParam);
	}
	else if (GameDetect::currentGame == GameID::SF4 && x == 0 && y == 0)
	{
		return CreateWindowExW(dwExStyle, lpClassName, lpWindowName, dwStyle, x, y, nWidth, nHeight, hWndParent, hMenu, hInstance, lpParam);
	}

	if (lpWindowName)
	{
		dwStyle = g_windowStyle;
	}
	if (lpWindowName == NULL)
	{
		lpWindowName = lpClassName;
		dwStyle = g_windowStyle;
	}

	HWND thisWindow = CreateWindowExW(dwExStyle, lpClassName, lpWindowName, dwStyle, g_x, g_y, nWidth, nHeight, hWndParent, hMenu, hInstance, lpParam);

	if (lpWindowName)
	{
		hwndWindowW = thisWindow;
	}

	return thisWindow;
}

BOOL WINAPI AdjustWindowRectHk(LPRECT lpRect, DWORD dwStyle, BOOL bMenu)
{
#ifdef _DEBUG
	info("AdjustWindowRectHk called");
#endif
	dwStyle = g_windowStyle;

	return AdjustWindowRect(lpRect, dwStyle, bMenu);
}

BOOL WINAPI AdjustWindowRectExHk(LPRECT lpRect, DWORD dwStyle, BOOL bMenu, DWORD dwExStyle)
{
#ifdef _DEBUG
	info("AdjustWindowRectExHk called");
#endif
	dwStyle = g_windowStyle;
	dwExStyle = 0;

	return AdjustWindowRectEx(lpRect, dwStyle, bMenu, dwExStyle);
}

BOOL WINAPI SetWindowPosHk(HWND hWnd, HWND hWndInsertAfter, int X, int Y, int cx, int cy, UINT uFlags)
{
#ifdef _DEBUG
	info("SetWindowPosHk called");
#endif
	// Make window pos centered
	int xPos = (GetSystemMetrics(SM_CXSCREEN) - cx) / 2;
	int yPos = (GetSystemMetrics(SM_CYSCREEN) - cy) / 2;

	if (hwndWindowA == hWnd || hwndWindowW == hWnd)
	{
		X = xPos;
		Y = yPos;
	}
	if (hWndInsertAfter == HWND_TOPMOST)
	{
		hWndInsertAfter = HWND_TOP;
	}

	return SetWindowPos(hWnd, hWndInsertAfter, X, Y, cx, cy, uFlags);
}

LONG __stdcall ChangeDisplaySettingsHk(DEVMODEA* lpDevMode, DWORD dwFlags)
{
#ifdef _DEBUG
	info("ChangeDisplaySettingsHk called");
#endif
	lpDevMode = NULL; // retain original changes instead of applying modified values

	return ChangeDisplaySettingsA(lpDevMode, dwFlags);
}

LONG __stdcall ChangeDisplaySettingsExWHk(LPCWSTR lpszDeviceName, DEVMODEW* lpDevMode, HWND hwnd, DWORD dwflags, LPVOID lParam)
{
#ifdef _DEBUG
	info("ChangeDisplaySettingsExWHk called");
#endif
	lpDevMode = NULL; // retain original changes instead of applying modified values

	return ChangeDisplaySettingsExW(lpszDeviceName, lpDevMode, hwnd, dwflags, lParam);
}

BOOL WINAPI UpdateWindowHk(HWND hWnd)
{
	return true;
}

BOOL WINAPI ClipCursorHk(const RECT* lpRect)
{
	return false;
}

BOOL WINAPI SetCursorPosHk(int X, int Y)
{
	return true;
}

void init_windowHooks(windowHooks* data)
{
	g_windowStyle = WS_VISIBLE | WS_POPUP | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_MAXIMIZEBOX;
	hide_windowStyle = WS_POPUP | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_MAXIMIZEBOX;

	int rx, ry;
	GetDesktopResolution(rx, ry);

	g_x = (rx / 2) - (g_width / 2);
	g_y = (ry / 2) - (g_height / 2);

	if (data->createWindowExA != NULL)
	{
		*(HWND*)data->createWindowExA = (HWND)CreateWindowExAHk;
	}

	if (data->createWindowExW != NULL)
	{
		*(HWND*)data->createWindowExW = (HWND)CreateWindowExWHk;
	}

	if (data->adjustWindowRect != NULL)
	{
		*(BOOL*)data->adjustWindowRect = (BOOL)AdjustWindowRectHk;
	}

	if (data->adjustWindowRectEx != NULL)
	{
		*(BOOL*)data->adjustWindowRectEx = (BOOL)AdjustWindowRectExHk;
	}

	if (data->setWindowPos != NULL)
	{
		*(BOOL*)data->setWindowPos = (BOOL)SetWindowPosHk;
	}

	if (data->changeDisplaySettings != NULL)
	{
		*(LONG*)data->changeDisplaySettings = (LONG)ChangeDisplaySettingsHk;
	}

	if (data->changeDisplaySettingsExW != NULL)
	{
		*(LONG*)data->changeDisplaySettingsExW = (LONG)ChangeDisplaySettingsExWHk;
	}

	if (data->updateWindow != NULL)
	{
		*(BOOL*)data->updateWindow = (BOOL)UpdateWindowHk;
	}

	if (data->clipCursor != NULL)
	{
		*(BOOL*)data->clipCursor = (BOOL)ClipCursorHk;
	}

	if (data->setCursorPos != NULL)
	{
		*(BOOL*)data->setCursorPos = (BOOL)SetCursorPosHk;
	}
}
/* END WINDOW HOOKS */

DWORD FetchDwordInformation(const char* setting, const char* subkey, DWORD defaultValue)
{
	try
	{
		DWORD value = (DWORD)atol(config[setting][subkey].c_str());

		if (value == NULL)
			return defaultValue;

		return value;
	}
	catch (int e)
	{
		return defaultValue;
	}

	return defaultValue;
}

static void ResetffbOffset()
{
	*ffbOffset = 0;
	*ffbOffset2 = 0;
	*ffbOffset3 = 0;
	*ffbOffset4 = 0;
	*ffbOffset5 = 0;
	*ffbOffset6 = 0;
	*ffbOffset7 = 0;
	*ffbOffset8 = 0;
	*ffbOffset9 = 0;
}

DWORD genericRetZero()
{
	return 0;
}

static InitFunction globalFunc([]()
{
	hook::pattern::InitializeHints();

	ResetffbOffset();
	
	GetPrivateProfileStringA("GlobalHotkeys", "PauseKey", "", PauseKeyChar, 256, ".\\teknoparrot.ini");
	GetPrivateProfileStringA("GlobalHotkeys", "ExitKey", "", ExitKeyChar, 256, ".\\teknoparrot.ini");

	std::string PauseKeyStr = PauseKeyChar;
	if (PauseKeyStr.find('0x') != std::string::npos)
		PauseKeyValue = stoi(PauseKeyStr, 0, 16);

	std::string ExitKeyStr = ExitKeyChar;
	if (ExitKeyStr.find('0x') != std::string::npos)
		ExitKeyValue = stoi(ExitKeyStr, 0, 16);

	GetModuleFileNameA(NULL, SuspendBuf, MAX_PATH);

	std::string ExeName = getFileName(SuspendBuf);
	std::basic_string<TCHAR> converted(ExeName.begin(), ExeName.end());
	const TCHAR* tchar = converted.c_str();

	ProcessID = MyGetProcessId(tchar);

	CreateThread(NULL, 0, GlobalGameThread, NULL, 0, NULL);
	if (ToBool(config["General"]["Enable Outputs"]))
	{
		blaster = LoadLibraryA("OutputBlaster.dll");
		if (blaster)
		{
			printf("OutputBlaster loaded!");
		}
		else
		{
			printf("Failed to Load OutputBlaster!");
		}
	}
	
	if (ToBool(config["Score"]["Enable Submission"]))
	{
		hook::pattern::InitializeHints();

		ResetffbOffset();

		GetPrivateProfileStringA("GlobalHotkeys", "PauseKey", "", PauseKeyChar, 256, ".\\teknoparrot.ini");
		GetPrivateProfileStringA("GlobalHotkeys", "ExitKey", "", ExitKeyChar, 256, ".\\teknoparrot.ini");

		std::string PauseKeyStr = PauseKeyChar;
		if (PauseKeyStr.find('0x') != std::string::npos)
			PauseKeyValue = stoi(PauseKeyStr, 0, 16);

		std::string ExitKeyStr = ExitKeyChar;
		if (ExitKeyStr.find('0x') != std::string::npos)
			ExitKeyValue = stoi(ExitKeyStr, 0, 16);

		GetModuleFileNameA(NULL, SuspendBuf, MAX_PATH);

		std::string ExeName = getFileName(SuspendBuf);
		std::basic_string<TCHAR> converted(ExeName.begin(), ExeName.end());
		const TCHAR* tchar = converted.c_str();

		ProcessID = MyGetProcessId(tchar);

		CreateThread(NULL, 0, GlobalGameThread, NULL, 0, NULL);

		if (ToBool(config["General"]["Enable Outputs"]))
		{
			blaster = LoadLibraryA("OutputBlaster.dll");
			if (blaster)
			{
				printf("OutputBlaster loaded!");
			}
			else
			{
				printf("Failed to Load OutputBlaster!");
			}
		}

		if (ToBool(config["Score"]["Enable Submission"]))
		{
			if (ToBool(config["Score"]["Enable Capture"]))
				WritePrivateProfileStringA("Score", "Enable Capture", "1", ".\\ScoreSubmission.ini");
			else
				WritePrivateProfileStringA("Score", "Enable Capture", "0", ".\\ScoreSubmission.ini");

			static char buf[MAX_PATH];
			HMODULE hMod;
#if defined(_M_IX86)
			hMod = LoadLibrary(L"OpenParrot.dll");
#else
			hMod = LoadLibrary(L"OpenParrot64.dll");
#endif
			GetModuleFileNameA(hMod, buf, MAX_PATH);
			PathRemoveFileSpecA(buf);
			PathAppendA(buf, (".."));
#if defined(_M_IX86)
			strcat(buf, "\\TeknoParrot\\ScoreSubmission.dll");
#else
			strcat(buf, "\\TeknoParrot\\ScoreSubmission64.dll");
#endif
			HMODULE hModA = LoadLibraryA(buf);

			if (hModA)
			{
				printf("Score Submission loaded!");
				void(*fn)() = (void(*)())GetProcAddress(hModA, "Score_Submit_Init");
				fn();
			}
			else
			{
				printf("Failed to load Score Submission!");
			}
		}

	}, GameID::Global);
#pragma optimize("", on)