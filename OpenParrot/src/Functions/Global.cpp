#include <StdInc.h>
#include <Utility/InitFunction.h>
#include "Global.h"
#include "Utility/GameDetect.h"
#include "Utility/Hooking.Patterns.h"
#include <shlwapi.h>

#pragma comment(lib,"shlwapi.lib")

#pragma optimize("", off)
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

DWORD WINAPI QuitGameThread(__in  LPVOID lpParameter)
{
	while (true)
	{
		if (GetAsyncKeyState(VK_ESCAPE))
		{
#ifndef _DEBUG
			if (GameDetect::currentGame == GameID::Daytona3)
			{
				system("taskkill /f /im InpWrapper.exe");
			}
			if ((GameDetect::currentGame == GameID::StarWarsEs3X) || (GameDetect::currentGame == GameID::StarWarsJapEs3X) || (GameDetect::currentGame == GameID::StarWarsEs3XLauncher) || (GameDetect::currentGame == GameID::StarWarsJapEs3XLauncher))
			{
				system("taskkill /f /im RSLauncher.exe");
				system("taskkill /f /im SWArcGame-Win64-Shipping.exe");
			}
			if (blaster)
			{
				FreeLibrary(blaster);
			}
			TerminateProcess(GetCurrentProcess(), 0);						
#endif
			//ExitProcess(0);
		}

		Sleep(300);
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
		int value = atol(config[setting][subkey].c_str());

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
	CreateThread(NULL, 0, QuitGameThread, NULL, 0, NULL);
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