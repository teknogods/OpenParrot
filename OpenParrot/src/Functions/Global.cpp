#include <StdInc.h>
#include <Utility/InitFunction.h>
#include "Global.h"
#include "Utility/GameDetect.h"
#include "Utility/Hooking.Patterns.h"
#include <shlwapi.h>
#include <Windows.h>
#include "Utility/Helper.h"
#include <TlHelp32.h>
#include "mmeapi.h"

#pragma comment(lib,"shlwapi.lib")
#pragma comment(lib,"Winmm.lib")

#pragma optimize("", off)

static DWORD dwVolume;
static DWORD ProcessID;
static HANDLE snHandle;
static BOOL rvBool;
static THREADENTRY32 te32 = { 0 };
static bool DisableGhosting;
static bool SuspendInit;
static bool SuspendPressedOn;
static bool SuspendPressedOff;
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

std::wstring utf8_decode(const std::string &str)
{
	if (str.empty()) return std::wstring();
	int size_needed = MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), NULL, 0);
	std::wstring wstrTo(size_needed, 0);
	MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), &wstrTo[0], size_needed);
	return wstrTo;
}

static void QuitGame()
{
	if (GameDetect::currentGame == GameID::Daytona3)
		system("taskkill /f /im InpWrapper.exe");

	if (GameDetect::currentGame == GameID::GHA)
		system("taskkill /f /im GHA.exe"); // is this necessary?

	if ((GameDetect::currentGame == GameID::StarWarsEs3X) || (GameDetect::currentGame == GameID::StarWarsJapEs3X) || (GameDetect::currentGame == GameID::StarWarsEs3XLauncher) || (GameDetect::currentGame == GameID::StarWarsJapEs3XLauncher))
	{
		system("taskkill /f /im RSLauncher.exe");
		system("taskkill /f /im SWArcGame-Win64-Shipping.exe");
	}

	TerminateProcess(GetCurrentProcess(), 0);
}

static BOOL SuspendProcess(DWORD ProcessId, bool Suspend)
{
	snHandle = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
	if (snHandle == INVALID_HANDLE_VALUE)
		return (false);

	te32.dwSize = sizeof(THREADENTRY32);
	if (Thread32First(snHandle, &te32))
	{
		do
		{
			if (te32.th32OwnerProcessID == ProcessId)
			{
				HANDLE hThread = OpenThread(THREAD_SUSPEND_RESUME, FALSE, te32.th32ThreadID);

				if (!Suspend)
				{
					ResumeThread(hThread);
				}
				else
				{
					SuspendThread(hThread);

					while (true)
					{
						if (!DisableGhosting)
						{
							DisableGhosting = true;
							DisableProcessWindowsGhosting();
						}

						PauseGameFixes(0);
#ifndef _DEBUG
						if (GetAsyncKeyState(ExitKeyValue))
						{
							waveOutSetVolume(NULL, dwVolume);
							QuitGame();
						}
#endif
						if (GetAsyncKeyState(PauseKeyValue))
						{
							if (SuspendPressedOff)
							{
								SuspendPressedOff = false;
								ResetPauseGameFixes(0);
								waveOutSetVolume(NULL, dwVolume);
								SuspendProcess(ProcessID, false);
								break;
							}
						}
						else
						{
							if (SuspendPressedOn)
								SuspendPressedOff = true;
						}
						Sleep(16);
					}
				}
				CloseHandle(hThread);
			}
		} 
		while (Thread32Next(snHandle, &te32));
		rvBool = TRUE;
	}
	else
		rvBool = FALSE;

	CloseHandle(snHandle);
	return (rvBool);
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

DWORD WINAPI GlobalGameThread(__in  LPVOID lpParameter)
{
	while (true)
	{
#ifndef _DEBUG
		if (GetAsyncKeyState(ExitKeyValue))
			QuitGame();
#endif
		if (!SuspendInit)
		{
			SuspendInit = true;
			GetModuleFileNameA(NULL, SuspendBuf, MAX_PATH);

			std::string ExeName = getFileName(SuspendBuf);
			std::basic_string<TCHAR> converted(ExeName.begin(), ExeName.end());
			const TCHAR* tchar = converted.c_str();

			ProcessID = MyGetProcessId(tchar);
		}

		if (GetAsyncKeyState(PauseKeyValue))
		{
			if (!SuspendPressedOn)
			{
				SuspendPressedOn = true;
				if (waveOutGetVolume(NULL, &dwVolume) == MMSYSERR_NOERROR)
					waveOutSetVolume(NULL, 0);
				SuspendProcess(ProcessID, true);
			}
		}
		else
		{
			if (SuspendPressedOn)
				SuspendPressedOn = false;
		}
		Sleep(64);
	}
}
	
/* WINDOW HOOKS */

DWORD g_windowStyle;

int g_x = 0;
int g_y = 0;

int g_width = 0;
int g_height = 0;

HWND hwndWindowA;
HWND hwndWindowW;

HWND WINAPI CreateWindowExAHk(DWORD dwExStyle, LPCSTR lpClassName, LPCSTR lpWindowName, DWORD dwStyle, int x, int y, int nWidth, int nHeight, HWND hWndParent, HMENU hMenu, HINSTANCE hInstance, LPVOID lpParam)
{
#ifdef _DEBUG
	info(true, "CreateWindowExAHk called");
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
	info(true, "CreateWindowExWHk called");
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
	info(true, "AdjustWindowRectHk called");
#endif
	dwStyle = g_windowStyle;

	return AdjustWindowRect(lpRect, dwStyle, bMenu);
}

BOOL WINAPI AdjustWindowRectExHk(LPRECT lpRect, DWORD dwStyle, BOOL bMenu, DWORD dwExStyle)
{
#ifdef _DEBUG
	info(true, "AdjustWindowRectExHk called");
#endif
	dwStyle = g_windowStyle;
	dwExStyle = 0;

	return AdjustWindowRectEx(lpRect, dwStyle, bMenu, dwExStyle);
}

BOOL WINAPI SetWindowPosHk(HWND hWnd, HWND hWndInsertAfter, int X, int Y, int cx, int cy, UINT uFlags)
{
#ifdef _DEBUG
	info(true, "SetWindowPosHk called");
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

LONG __stdcall ChangeDisplaySettingsHk(DEVMODEA *lpDevMode, DWORD dwFlags)
{
#ifdef _DEBUG
	info(true, "ChangeDisplaySettingsHk called");
#endif
	lpDevMode = NULL; // retain original changes instead of applying modified values

	return ChangeDisplaySettingsA(lpDevMode, dwFlags);
}

LONG __stdcall ChangeDisplaySettingsExWHk(LPCWSTR lpszDeviceName, DEVMODEW* lpDevMode, HWND hwnd, DWORD dwflags, LPVOID lParam)
{
#ifdef _DEBUG
	info(true, "ChangeDisplaySettingsExWHk called");
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

static InitFunction globalFunc([]()
{
	hook::pattern::InitializeHints();
	
	GetPrivateProfileStringA("GlobalHotkeys", "PauseKey", "", PauseKeyChar, 256, ".\\teknoparrot.ini");
	GetPrivateProfileStringA("GlobalHotkeys", "ExitKey", "", ExitKeyChar, 256, ".\\teknoparrot.ini");

	std::string PauseKeyStr = PauseKeyChar;
	if (PauseKeyStr.find('0x') != std::string::npos)
		PauseKeyValue = stoi(PauseKeyStr, 0, 16);

	std::string ExitKeyStr = ExitKeyChar;
	if (ExitKeyStr.find('0x') != std::string::npos)
		ExitKeyValue = stoi(ExitKeyStr, 0, 16);

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

	if (ToBool(config["Score"]["Enable Submission (Patreon Only)"]))
	{
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