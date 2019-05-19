#include <StdInc.h>
#include "Utility/InitFunction.h"
#include "Functions/Global.h"
#include "Utility\Hooking.Patterns.h"
#include <Xinput.h>
#include <math.h>
#include <dinput.h>

#pragma comment(lib, "Ws2_32.lib")

#define clamp( x, xmin, xmax ) min( xmax, max( x, xmin ) )
typedef unsigned int U32;

DWORD BaseAddress2 = 0x00400000;
int horizontal2 = 0;
int vertical2 = 0;
HWND hWndRT2 = 0;
HCURSOR cursorhndle2;
// controls
extern int* ffbOffset;
extern int* ffbOffset2;
extern int* ffbOffset3;
extern int* ffbOffset4;
// hooks ori
BOOL(__stdcall *original_SetWindowPos2)(HWND hWnd, HWND hWndInsertAfter, int X, int Y, int cx, int cy, UINT uFlags);
BOOL(__stdcall *original_CreateWindowExA2)(DWORD dwExStyle, LPCSTR lpClassName, LPCSTR lpWindowName, DWORD dwStyle, int X, int Y, int nWidth, int nHeight, HWND hWndParent, HMENU hMenu, HINSTANCE hInstance, LPVOID lpParam);

DWORD WINAPI InputRT2(LPVOID lpParam)
{
	int deltaTimer = 16;
	INT_PTR keyboardBuffer = (0x437F6F8 + BaseAddress2);
	INT_PTR keyboardBuffer2 = (0x437FC08 + BaseAddress2);
	bool previousLeft = false;
	bool previousRight = false;
	bool previousUp = false;
	bool previousDown = false;

	while (true)
	{
		// ESCAPE QUITS GAME 
		if (GetAsyncKeyState(VK_ESCAPE) & 0x8000)
		{
			exit(0);
		}
		// buttons see bitwise values in TPui//RawThrills.cs
		// START ( = NITRO too)
		if (*ffbOffset & 0x08)
		{
			injector::WriteMemory<BYTE>((keyboardBuffer2 + 4 * 0x00), 2, true);
			injector::WriteMemory<BYTE>((keyboardBuffer + DIK_NUMPADENTER), 2, true);

		}
		// TEST
		if (*ffbOffset & 0x01)
		{
			injector::WriteMemory<BYTE>(keyboardBuffer2 + 0x0B * sizeof(U32), 2, true);
		}
		// NITRO ( = START too)
		if (*ffbOffset & 0x100)
		{
			injector::WriteMemory<BYTE>((keyboardBuffer2 + 4 * 0x00), 2, true);
			injector::WriteMemory<BYTE>((keyboardBuffer + DIK_NUMPADENTER), 2, true);
		}
		// SHIFT DOWN
		if (*ffbOffset & 0x2000)
		{
			if (previousDown == false)
			{
				injector::WriteMemory<BYTE>((keyboardBuffer + DIK_DOWN), 2, true);
				previousDown = true;
			}
		}
		else
		{
			if (previousDown == true)
			{
				previousDown = false;
			}
		}
		// SHIFT UP
		if (*ffbOffset & 0x1000)
		{
			if (previousUp == false)
			{
				injector::WriteMemory<BYTE>((keyboardBuffer + DIK_UP), 2, true);
				previousUp = true;
			}
		}
		else
		{
			if (previousUp == true)
			{
				previousUp = false;
			}
		}
		// BUTTON 1/ VIEW 1
		if (*ffbOffset & 0x200)
		{
			injector::WriteMemory<BYTE>(keyboardBuffer + DIK_A, 2, true);
			injector::WriteMemory<BYTE>(keyboardBuffer2 + 0x01 * sizeof(U32), 2, true);
		}
		// BUTTON 2/ VIEW 2
		if (*ffbOffset & 0x400)
		{
			injector::WriteMemory<BYTE>(keyboardBuffer + DIK_B, 2, true);
			injector::WriteMemory<BYTE>(keyboardBuffer2 + 0x02 * sizeof(U32), 2, true);
		}
		// BUTTON 3/ VIEW 3
		if (*ffbOffset & 0x800)
		{
			injector::WriteMemory<BYTE>(keyboardBuffer + DIK_E, 2, true);
			injector::WriteMemory<BYTE>(keyboardBuffer2 + 0x03 * sizeof(U32), 2, true);
		}
		// MENU LEFT
		if (*ffbOffset & 0x4000)
		{
			if (previousLeft == false)
			{
				injector::WriteMemory<BYTE>((keyboardBuffer + DIK_LEFT), 2, true);
				previousLeft = true;
			}
		}
		else
		{
			if (previousLeft == true)
			{
				previousLeft = false;
			}
		}
		// MENU RIGHT
		if (*ffbOffset & 0x8000)
		{
			if (previousRight == false)
			{
				injector::WriteMemory<BYTE>((keyboardBuffer + DIK_RIGHT), 2, true);
				previousRight = true;
			}
		}
		else
		{
			if (previousRight == true)
			{
				previousRight = false;
			}
		}
		// WHEEL
		int iWheel0 = (((float)*ffbOffset2) - 128);
		float wheel = (iWheel0 * 0.0078125f);
		int iWheel = (int)(2047.5 + 2047.5 * wheel);
		injector::WriteMemory<DWORD>((0x437F808 + BaseAddress2) + 0x20 * sizeof(U32), iWheel, true);
		injector::WriteMemory<float>((0x4380208 + BaseAddress2), wheel, true);
		//// GAS
		float gas = (float)*ffbOffset3 / 255.0f;
		int iGas = (int)(gas * 4095);
		injector::WriteMemory<DWORD>((0x437F808 + BaseAddress2) + 0x21 * sizeof(U32), iGas, true);
		injector::WriteMemory<float>((0x438020C + BaseAddress2), gas, true);
		//// BRAKE
		float brake = (float)*ffbOffset4 / 255.0f;
		int iBrake = (int)(brake * 4095);
		injector::WriteMemory<DWORD>((0x437F808 + BaseAddress2) + 0x22 * sizeof(U32), iBrake, true);
		injector::WriteMemory<float>((0x438020C + BaseAddress2), brake, true);

		//DEBUG//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		//	info(true, "test value %f ", test);
		//DEBUG//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

		Sleep(deltaTimer);
	}
	
	return 0;
}

DWORD WINAPI FullscreenRT2(LPVOID lpParam)
{
	while (true)
	{
		HWND hWndTMP = GetForegroundWindow();
		if (hWndRT2 == 0)
		{
			hWndRT2 = FindWindowA(NULL, "FNF SuperCars");
		}
		if (hWndTMP == hWndRT2)
		{
			// UGLY FULLSCREEN FIX (borderless window resized to desktop size)
			RECT rect;
			GetWindowRect(hWndRT2, &rect);
			int currentwidth = rect.right - rect.left;
			int currentheight = rect.bottom - rect.top;
			if (currentwidth != horizontal2)
				original_SetWindowPos2(hWndRT2, NULL, 0, 0, horizontal2, vertical2, SWP_NOMOVE | SWP_NOOWNERZORDER | SWP_NOZORDER);
			if (currentheight != vertical2)
				original_SetWindowPos2(hWndRT2, NULL, 0, 0, horizontal2, vertical2, SWP_NOMOVE | SWP_NOOWNERZORDER | SWP_NOZORDER);
			if (rect.left != 0)
				original_SetWindowPos2(hWndRT2, NULL, 0, 0, horizontal2, vertical2, SWP_NOMOVE | SWP_NOOWNERZORDER | SWP_NOZORDER);
			if (rect.top != 0)
				original_SetWindowPos2(hWndRT2, NULL, 0, 0, horizontal2, vertical2, SWP_NOMOVE | SWP_NOOWNERZORDER | SWP_NOZORDER);
		}
		Sleep(2000);
	}
}

DWORD WINAPI WindowRT2(LPVOID lpParam)
{
	while (true)
	{
			// RIGHT-CLICK MINIMIZES WINDOW
			if (GetAsyncKeyState(VK_RBUTTON) & 0x8000)
			{
				HWND hWndTMP = GetForegroundWindow();
				if (hWndRT2 == 0)
				{
					hWndRT2 = FindWindowA(NULL, "FNF SuperCars");
				}
				if (hWndTMP == hWndRT2)
				{
					original_SetWindowPos2(hWndRT2, HWND_BOTTOM, 0, 0, 1360, 768, SWP_NOSIZE);
					ShowWindow(hWndRT2, SW_MINIMIZE);
				}
			}
	}
}

BOOL(__stdcall *original_DefWindowProcA2)(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
DWORD WINAPI DefWindowProcART2(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	static int xClick;
	static int yClick;

	switch (message)
	{
	case WM_LBUTTONDOWN:
		SetCapture(hWnd);
		xClick = LOWORD(lParam);
		yClick = HIWORD(lParam);
		break;

	case WM_LBUTTONUP:
		ReleaseCapture();
		break;

	case WM_MOUSEMOVE:
	{
		if (GetCapture() == hWnd)
		{
			RECT rcWindow;
			GetWindowRect(hWnd, &rcWindow);
			int xMouse = LOWORD(lParam);
			int yMouse = HIWORD(lParam);
			int xWindow = rcWindow.left + xMouse - xClick;
			int yWindow = rcWindow.top + yMouse - yClick;
			if (xWindow >= (horizontal2 - 100))
				xWindow = 0;
			if (yWindow >= (vertical2 - 100))
				yWindow = 0;
			original_SetWindowPos2(hWnd, NULL, xWindow, yWindow, 1360, 768, SWP_NOSIZE | SWP_NOZORDER);
		}
		break;
	}
	
	}
	return original_DefWindowProcA2(hWnd, message, wParam, lParam);
}

DWORD WINAPI CreateWindowExART2(DWORD dwExStyle, LPCSTR lpClassName, LPCSTR lpWindowName, DWORD dwStyle, int X, int Y, int nWidth, int nHeight, HWND hWndParent, HMENU hMenu, HINSTANCE hInstance, LPVOID lpParam)
{
		return original_CreateWindowExA2(dwExStyle, lpClassName, "FNF SuperCars", 0x96000000, 0, 0, 1360, 768, hWndParent, hMenu, hInstance, lpParam);
}

DWORD WINAPI SetCursorPosRT2(int X, int Y)
{
	return 1;
}

DWORD WINAPI SetWindowPosRT2(HWND hWnd, HWND hWndInsertAfter, int X, int Y, int cx, int cy, UINT uFlags)
{
	return 1;
}

static InitFunction FNFSCFunc([]()
{
	GetDesktopResolution(horizontal2, vertical2);
	cursorhndle2 = GetCursor();

	// REMOVE ERROR MESSAGEBOX ON CLOSE
	injector::WriteMemory<BYTE>((0x73794 + BaseAddress2), 0xEB, true);
	
	// REMOVE CMD WATSON nonsense
	injector::MakeNOP((0x5F3882), 2, true);

	CreateThread(NULL, 0, InputRT2, NULL, 0, NULL);

	MH_Initialize();
	MH_CreateHookApi(L"user32.dll", "CreateWindowExA", &CreateWindowExART2, (void**)&original_CreateWindowExA2);
	MH_CreateHookApi(L"user32.dll", "SetWindowPos", &SetWindowPosRT2, (void**)&original_SetWindowPos2);
	MH_EnableHook(MH_ALL_HOOKS);

	if (ToBool(config["General"]["Windowed"]))
	{
	// NO HIDE CURSOR
	injector::WriteMemory<BYTE>((0x72FCF + BaseAddress2), 0x01, true);

	CreateThread(NULL, 0, WindowRT2, NULL, 0, NULL);

	MH_Initialize();
	MH_CreateHookApi(L"user32.dll", "DefWindowProcA", &DefWindowProcART2, (void**)&original_DefWindowProcA2);
	MH_CreateHookApi(L"user32.dll", "SetCursorPos", &SetCursorPosRT2, NULL);
	MH_EnableHook(MH_ALL_HOOKS);
	}
	else
	{
		CreateThread(NULL, 0, FullscreenRT2, NULL, 0, NULL);
	}

}, GameID::FNFSC);
