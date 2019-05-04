#include <StdInc.h>
#include "Utility/InitFunction.h"
#include "Functions/Global.h"
#if _M_IX86


signed int FordRacingControlInit()
{
	// start input thread
	((void(*)(int, int, int))0x4E0803)(0x4DE030, 0, 0);

	// set input initialized
	*(DWORD*)0x6B02A0 = 1;

	// device count
	*(DWORD*)0x6B0290 = 1;

	// set joystick
	*(char**)0x6A9AF4 = "JOYSTICK";

	// create mutex
	*(DWORD*)0x6A9A74 = (DWORD)CreateMutex(NULL, FALSE, NULL);

	return 1;
}
extern int* ffbOffset;

int __stdcall FordRacingFfbFunc(DWORD device, DWORD data)
{
	if(device == 0x378)
	{
		*ffbOffset = data;
	}
	return 0;
}

HWND hWndFORD;

BOOL(__stdcall *g_origDefWindowProcA)(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
DWORD WINAPI DefWindowProcAFord(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	hWndFORD = hWnd;
	return g_origDefWindowProcA(hWnd, message, wParam, lParam);
}

DWORD WINAPI FordFocus(LPVOID lpParam)
{
	while (true)
	{
		POINT point;
		// LEFT-CLICK MOVES WINDOW FROM TOP-LEFT CORNER
		if (GetAsyncKeyState(VK_LBUTTON) & 0x8000)
		{
			SetWindowPos(hWndFORD, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
			GetCursorPos(&point);
			LPARAM blah = MAKELPARAM(point.x, point.y);
			int xClick = LOWORD(blah);
			int yClick = HIWORD(blah);
			SetWindowPos(hWndFORD, HWND_TOPMOST, xClick, yClick, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
		}
		// RIGHT-CLICK MINIMIZES WINDOW
		if (GetAsyncKeyState(VK_RBUTTON) & 0x8000)
		{
			ShowWindow(hWndFORD, SW_MINIMIZE);
		}
		// ESCAPE QUITS GAME
		if (0 != GetAsyncKeyState(VK_ESCAPE))
		{
			exit(0);
		}

	}
}

static InitFunction fordRacingFunc([]()
{

	DWORD oldprot = 0;
	DWORD oldprot2 = 0;
	VirtualProtect((LPVOID)0x401000, 0xEE000, 0x40, &oldprot);

	// readfile call
	static DWORD source = (DWORD)(LPVOID)&ReadFileHooked;
	*(DWORD *)0x4DE005 = (DWORD)(LPVOID)&source;
	VirtualProtect((LPVOID)0x401000, 0xEE000, oldprot, &oldprot2);

	// replace input init
	injector::MakeJMP(0x4DE1F0, FordRacingControlInit);
	// Hook FFB calls
	injector::MakeJMP(0x00467C9A, FordRacingFfbFunc);

	if (ToBool(config["General"]["Windowed"]))
	{
		// TODO: fix mouse to work without alt-tab, make window moveable
		// NOW FIXED: left-click and drag to move window , right-click to minimize window , ESC to close
		injector::MakeNOP(0x466A70 + 0x1BB, 5);
		injector::WriteMemory<BYTE>(0x4B3E61, 0x01, true);
		injector::WriteMemory<BYTE>(0x4B3F35, 0xEB, true);

		CreateThread(NULL, 0, FordFocus, NULL, 0, NULL);

		MH_Initialize();
		MH_CreateHookApi(L"user32.dll", "DefWindowProcA", &DefWindowProcAFord, (void**)&g_origDefWindowProcA);
		MH_EnableHook(MH_ALL_HOOKS);
	}

}, GameID::FordRacing);
#endif
