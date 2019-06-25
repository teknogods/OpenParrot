#include <StdInc.h>
#include "Utility/InitFunction.h"
#include "Functions/Global.h"
#include "Utility\Hooking.Patterns.h"
#include <Xinput.h>
#include <math.h>
#include <dinput.h>

#pragma comment(lib, "Ws2_32.lib")
#if _M_IX86
#define clamp( x, xmin, xmax ) min( xmax, max( x, xmin ) )
typedef unsigned int U32;
typedef unsigned char U8;

DWORD BaseAddress5 = 0x00400000;
int horizontal5 = 0;
int vertical5 = 0;
HWND hWndRT5 = 0;

static bool previousLeft = false;
static bool previousRight = false;
static bool previousUp = false;
static bool previousDown = false;
static bool button1pressed = false;
static bool button2pressed = false;
static bool button3pressed = false;
static bool button4pressed = false;
static bool viewpressed = false;
static bool PINpressed = false;
static bool NUMpressed = false;
static bool NUMpressed0 = false;
static bool NUMpressed1 = false;
static bool NUMpressed2 = false;
static bool NUMpressed3 = false;
static bool NUMpressed4 = false;
static bool NUMpressed5 = false;
static bool NUMpressed6 = false;
static bool NUMpressed7 = false;
static bool NUMpressed8 = false;
static bool NUMpressed9 = false;

// controls
extern int* ffbOffset;
extern int* ffbOffset2;
extern int* ffbOffset3;
extern int* ffbOffset4;
// hooks ori
BOOL(__stdcall* original_SetWindowPos5)(HWND hWnd, HWND hWndInsertAfter, int X, int Y, int cx, int cy, UINT uFlags);
BOOL(__stdcall* original_CreateWindowExA5)(DWORD dwExStyle, LPCSTR lpClassName, LPCSTR lpWindowName, DWORD dwStyle, int X, int Y, int nWidth, int nHeight, HWND hWndParent, HMENU hMenu, HINSTANCE hInstance, LPVOID lpParam);

DWORD WINAPI InputRT5(LPVOID lpParam)
{
	int deltaTimer = 16;
	INT_PTR keyboardBuffer = (0x8501438 + BaseAddress5);
	INT_PTR keyboardBuffer2 = (0x8501948 + BaseAddress5);

	while (true)
	{
		// ESCAPE QUITS GAME 
		if (GetAsyncKeyState(VK_ESCAPE) & 0x8000)
		{
			HWND hWndTMP = GetForegroundWindow();
			if (hWndRT5 == 0)
			{
				hWndRT5 = FindWindowA(NULL, "FNF SuperBikes 2");
			}
			if (hWndTMP == hWndRT5)
			{
				exit(0);
			}
		}

		if (GetAsyncKeyState(VK_NUMPAD1) & 0x0001)
		{
			if (NUMpressed1 == false)
			{
				injector::WriteMemoryRaw((0x179A7 + BaseAddress5), "\xB8\x01\x00\x00\x00\x90", 6, true);
				NUMpressed1 = true;
			}
		}
		else
		{
			if (NUMpressed1 == true)
			{
				injector::WriteMemoryRaw((0x179A7 + BaseAddress5), "\x8D\x85\x80\xFF\xFE\xFF", 6, true);
				NUMpressed1 = false;
			}
		}
		if (GetAsyncKeyState(VK_NUMPAD2) & 0x0001)
		{
			if (NUMpressed2 == false)
			{
				injector::WriteMemoryRaw((0x179A7 + BaseAddress5), "\xB8\x02\x00\x00\x00\x90", 6, true);
				NUMpressed2 = true;
			}
		}
		else
		{
			if (NUMpressed2 == true)
			{
				injector::WriteMemoryRaw((0x179A7 + BaseAddress5), "\x8D\x85\x80\xFF\xFE\xFF", 6, true);
				NUMpressed2 = false;
			}
		}
		if (GetAsyncKeyState(VK_NUMPAD3) & 0x0001)
		{
			if (NUMpressed3 == false)
			{
				injector::WriteMemoryRaw((0x179A7 + BaseAddress5), "\xB8\x03\x00\x00\x00\x90", 6, true);
				NUMpressed3 = true;
			}
		}
		else
		{
			if (NUMpressed3 == true)
			{
				injector::WriteMemoryRaw((0x179A7 + BaseAddress5), "\x8D\x85\x80\xFF\xFE\xFF", 6, true);
				NUMpressed3 = false;
			}
		}
		if (GetAsyncKeyState(VK_NUMPAD4) & 0x0001)
		{
			if (NUMpressed4 == false)
			{
				injector::WriteMemoryRaw((0x179A7 + BaseAddress5), "\xB8\x04\x00\x00\x00\x90", 6, true);
				NUMpressed4 = true;
			}
		}
		else
		{
			if (NUMpressed4 == true)
			{
				injector::WriteMemoryRaw((0x179A7 + BaseAddress5), "\x8D\x85\x80\xFF\xFE\xFF", 6, true);
				NUMpressed4 = false;
			}
		}
		if (GetAsyncKeyState(VK_NUMPAD5) & 0x0001)
		{
			if (NUMpressed5 == false)
			{
				injector::WriteMemoryRaw((0x179A7 + BaseAddress5), "\xB8\x05\x00\x00\x00\x90", 6, true);
				NUMpressed5 = true;
			}
		}
		else
		{
			if (NUMpressed5 == true)
			{
				injector::WriteMemoryRaw((0x179A7 + BaseAddress5), "\x8D\x85\x80\xFF\xFE\xFF", 6, true);
				NUMpressed5 = false;
			}
		}
		if (GetAsyncKeyState(VK_NUMPAD6) & 0x0001)
		{
			if (NUMpressed6 == false)
			{
				injector::WriteMemoryRaw((0x179A7 + BaseAddress5), "\xB8\x06\x00\x00\x00\x90", 6, true);
				NUMpressed6 = true;
			}
		}
		else
		{
			if (NUMpressed6 == true)
			{
				injector::WriteMemoryRaw((0x179A7 + BaseAddress5), "\x8D\x85\x80\xFF\xFE\xFF", 6, true);
				NUMpressed6 = false;
			}
		}
		if (GetAsyncKeyState(VK_NUMPAD7) & 0x0001)
		{
			if (NUMpressed7 == false)
			{
				injector::WriteMemoryRaw((0x179A7 + BaseAddress5), "\xB8\x07\x00\x00\x00\x90", 6, true);
				NUMpressed7 = true;
			}
		}
		else
		{
			if (NUMpressed7 == true)
			{
				injector::WriteMemoryRaw((0x179A7 + BaseAddress5), "\x8D\x85\x80\xFF\xFE\xFF", 6, true);
				NUMpressed7 = false;
			}
		}
		if (GetAsyncKeyState(VK_NUMPAD8) & 0x0001)
		{
			if (NUMpressed8 == false)
			{
				injector::WriteMemoryRaw((0x179A7 + BaseAddress5), "\xB8\x08\x00\x00\x00\x90", 6, true);
				NUMpressed8 = true;
			}
		}
		else
		{
			if (NUMpressed8 == true)
			{
				injector::WriteMemoryRaw((0x179A7 + BaseAddress5), "\x8D\x85\x80\xFF\xFE\xFF", 6, true);
				NUMpressed8 = false;
			}
		}
		if (GetAsyncKeyState(VK_NUMPAD9) & 0x0001)
		{
			if (NUMpressed9 == false)
			{
				injector::WriteMemoryRaw((0x179A7 + BaseAddress5), "\xB8\x09\x00\x00\x00\x90", 6, true);
				NUMpressed9 = true;
			}
		}
		else
		{
			if (NUMpressed9 == true)
			{
				injector::WriteMemoryRaw((0x179A7 + BaseAddress5), "\x8D\x85\x80\xFF\xFE\xFF", 6, true);
				NUMpressed9 = false;
			}
		}
		if (GetAsyncKeyState(VK_NUMPAD0) & 0x0001)
		{
			if (NUMpressed0 == false)
			{
				injector::WriteMemoryRaw((0x179A7 + BaseAddress5), "\xB8\x00\x00\x00\x00\x90", 6, true);
				NUMpressed0 = true;
			}
		}
		else
		{
			if (NUMpressed0 == true)
			{
				injector::WriteMemoryRaw((0x179A7 + BaseAddress5), "\x8D\x85\x80\xFF\xFE\xFF", 6, true);
				NUMpressed0 = false;
			}
		}
		if (GetAsyncKeyState(VK_RETURN) & 0x0001)
		{
			if (NUMpressed == false)
			{
				injector::WriteMemoryRaw((0x179A7 + BaseAddress5), "\xB8\x0A\x00\x00\x00\x90", 6, true);
				injector::WriteMemoryRaw((0x17B61 + BaseAddress5), "\x90\x90", 2, true);
				NUMpressed = true;
			}
		}
		else
		{
			if (NUMpressed == true)
			{
				injector::WriteMemoryRaw((0x179A7 + BaseAddress5), "\x8D\x85\x80\xFF\xFE\xFF", 6, true);
				injector::WriteMemoryRaw((0x17B61 + BaseAddress5), "\x7C\x1E", 2, true);
				NUMpressed = false;
			}
		}

		// buttons see bitwise values in TPui//RawThrills.cs
		// START
		if (*ffbOffset & 0x08)
		{
			injector::WriteMemory<BYTE>((keyboardBuffer + DIK_SPACE), 2, true);
			if (NUMpressed == false)
				{
					injector::WriteMemoryRaw((0x179A7 + BaseAddress5), "\xB8\x0A\x00\x00\x00\x90", 6, true);
					injector::WriteMemoryRaw((0x17B61 + BaseAddress5), "\x90\x90", 2, true);
					NUMpressed = true;
				}
			else
			{
			if (NUMpressed == true)
				{
				injector::WriteMemoryRaw((0x179A7 + BaseAddress5), "\x8D\x85\x80\xFF\xFE\xFF", 6, true);
				injector::WriteMemoryRaw((0x17B61 + BaseAddress5), "\x7C\x1E", 2, true);
				NUMpressed = false;
				}
			}
		}
		// TEST
		if (*ffbOffset & 0x01)
		{
			injector::WriteMemory<BYTE>((keyboardBuffer + DIK_RBRACKET), 2, true);
		}
		// NITRO ( = START too)
		if (*ffbOffset & 0x100)
		{
			if (button1pressed == false)
			{
				injector::WriteMemory<BYTE>((keyboardBuffer + DIK_SPACE), 2, true);
				injector::WriteMemory<BYTE>((keyboardBuffer + DIK_N), 2, true);
				button1pressed = true;
			}
		}
		else
		{
			if (button1pressed == true)
			{
				button1pressed = false;
			}
		}
		// BUTTON 1/ VIEW 1
		if (*ffbOffset & 0x200)
		{
			if (button2pressed == false)
			{
				injector::WriteMemory<BYTE>((keyboardBuffer + DIK_A), 2, true);
				button2pressed = true;
			}
		}
		else
		{
			if (button2pressed == true)
			{
				button2pressed = false;
			}
		}
		// BUTTON 2/ VIEW ? (+ RIGHT BRAKE)
		if (*ffbOffset & 0x400)
		{
			if (button3pressed == false)
			{
				injector::WriteMemory<BYTE>((keyboardBuffer + DIK_B), 2, true);
				injector::WriteMemory<BYTE>((keyboardBuffer + DIK_4), 2, true);
				button3pressed = true;
			}
		}
		else
		{
			if (button3pressed == true)
			{
				button3pressed = false;
			}
		}
		// BUTTON 3/ MUSIC (+ LEFT BRAKE)
		if (*ffbOffset & 0x800)
		{
			if (button4pressed == false)
			{
				injector::WriteMemory<BYTE>((keyboardBuffer + DIK_E), 2, true);
				injector::WriteMemory<BYTE>((keyboardBuffer + DIK_3), 2, true);
				button4pressed = true;
			}
		}
		else
		{
			if (button4pressed == true)
			{
				button4pressed = false;
			}
		}
		// BUTTON VIEW
		if (*ffbOffset & 0x10)
		{
			if (viewpressed == false)
			{
			// TO-DO
			//	injector::WriteMemory<BYTE>((keyboardBuffer + DIK_ESCAPE), 2, true);
			//	injector::WriteMemory<BYTE>((keyboardBuffer + DIK_RETURN), 2, true);
				viewpressed = true;
			}
		}
		else
		{
			if (viewpressed == true)
			{
				viewpressed = false;
			}
		}
		// BUTTON ENTER-PIN
		if (*ffbOffset & 0x20)
		{
			if (PINpressed == false)
			{
				injector::WriteMemoryRaw((0x203A2 + BaseAddress5), "\x90\x90", 2, true);
				injector::WriteMemoryRaw((0x203AC + BaseAddress5), "\x90\x90", 2, true);

				injector::WriteMemoryRaw((0x21F74 + BaseAddress5), "\x90\x90", 2, true);
				injector::WriteMemoryRaw((0x21F83 + BaseAddress5), "\x90\x90", 2, true);

				injector::WriteMemoryRaw((0x2309B + BaseAddress5), "\x90\x90", 2, true);
				injector::WriteMemoryRaw((0x230A5 + BaseAddress5), "\x90\x90", 2, true);
				PINpressed = true;
			}
		}
		else
		{
			if (PINpressed == true)
			{
				injector::WriteMemoryRaw((0x203A2 + BaseAddress5), "\x74\x1B", 2, true);
				injector::WriteMemoryRaw((0x203AC + BaseAddress5), "\x75\x07", 2, true);

				injector::WriteMemoryRaw((0x21F74 + BaseAddress5), "\x74\x20", 2, true);
				injector::WriteMemoryRaw((0x21F83 + BaseAddress5), "\x75\x07", 2, true);

				injector::WriteMemoryRaw((0x2309B + BaseAddress5), "\x74\x1B", 2, true);
				injector::WriteMemoryRaw((0x230A5 + BaseAddress5), "\x75\x07", 2, true);
				PINpressed = false;
			}
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
		// WHEEL
		int iWheel0 = (((float)* ffbOffset2) - 128);
		float wheel = (iWheel0 * 0.0078125f);
		injector::WriteMemory<float>((0x4E2A54 + BaseAddress5), wheel, true);
		//// GAS
		float gas = (float)* ffbOffset3 / 255.0f;
		injector::WriteMemory<float>((0x4E2A48 + BaseAddress5), gas, true);
		//// BRAKE
		float brake = (float)* ffbOffset4 / 255.0f;
		injector::WriteMemory<float>((0x4E2A9C + BaseAddress5), brake, true);

		//DEBUG//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		//	info(true, "test value %f ", test);
		//DEBUG//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

		Sleep(deltaTimer);
	}

	return 0;
}

DWORD WINAPI FullscreenRT5(LPVOID lpParam)
{
	while (true)
	{
		HWND hWndTMP = GetForegroundWindow();
		if (hWndRT5 == 0)
		{
			hWndRT5 = FindWindowA(NULL, "FNF SuperBikes 2");
		}
		if (hWndTMP == hWndRT5)
		{
			// UGLY FULLSCREEN FIX (borderless window resized to desktop size)
			RECT rect;
			GetWindowRect(hWndRT5, &rect);
			int currentwidth = rect.right - rect.left;
			int currentheight = rect.bottom - rect.top;
			if (currentwidth != horizontal5)
				original_SetWindowPos5(hWndRT5, HWND_TOP, 0, 0, horizontal5, vertical5, SWP_NOMOVE | SWP_NOOWNERZORDER | SWP_NOZORDER);
			if (currentheight != vertical5)
				original_SetWindowPos5(hWndRT5, HWND_TOP, 0, 0, horizontal5, vertical5, SWP_NOMOVE | SWP_NOOWNERZORDER | SWP_NOZORDER);
			if (rect.left != 0)
				original_SetWindowPos5(hWndRT5, HWND_TOP, 0, 0, horizontal5, vertical5, SWP_NOMOVE | SWP_NOOWNERZORDER | SWP_NOZORDER);
			if (rect.top != 0)
				original_SetWindowPos5(hWndRT5, HWND_TOP, 0, 0, horizontal5, vertical5, SWP_NOMOVE | SWP_NOOWNERZORDER | SWP_NOZORDER);
		}
		Sleep(2000);
	}
}

DWORD WINAPI WindowRT5(LPVOID lpParam)
{
	while (true)
	{
		// RIGHT-CLICK MINIMIZES WINDOW
		if (GetAsyncKeyState(VK_RBUTTON) & 0x8000)
		{
			HWND hWndTMP = GetForegroundWindow();
			if (hWndRT5 == 0)
			{
				hWndRT5 = FindWindowA(NULL, "FNF SuperBikes 2");
			}
			if (hWndTMP == hWndRT5)
			{
				RECT rect;
				GetWindowRect(hWndRT5, &rect);
				int currentwidth = rect.right - rect.left;
				int currentheight = rect.bottom - rect.top;
				original_SetWindowPos5(hWndRT5, HWND_BOTTOM, 0, 0, 1366, 768, SWP_NOSIZE);
				ShowWindow(hWndRT5, SW_MINIMIZE);
			}
		}
	}
}

BOOL(__stdcall* original_DefWindowProcA5)(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
DWORD WINAPI DefWindowProcART5(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
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
			if (xWindow >= (horizontal5 - 100))
				xWindow = 0;
			if (yWindow >= (vertical5 - 100))
				yWindow = 0;
			original_SetWindowPos5(hWnd, NULL, xWindow, yWindow, 1366, 768, SWP_NOSIZE | SWP_NOZORDER);
		}
		break;
	}

	}
	return original_DefWindowProcA5(hWnd, message, wParam, lParam);
}

DWORD WINAPI CreateWindowExART5(DWORD dwExStyle, LPCSTR lpClassName, LPCSTR lpWindowName, DWORD dwStyle, int X, int Y, int nWidth, int nHeight, HWND hWndParent, HMENU hMenu, HINSTANCE hInstance, LPVOID lpParam)
{
	return original_CreateWindowExA5(dwExStyle, lpClassName, "FNF SuperBikes 2", 0x94000000, 0, 0, 1366, 768, hWndParent, hMenu, hInstance, lpParam);
}

DWORD WINAPI SetCursorPosRT5(int X, int Y)
{
	return 1;
}

DWORD WINAPI SetWindowPosRT5(HWND hWnd, HWND hWndInsertAfter, int X, int Y, int cx, int cy, UINT uFlags)
{
	return 1;
}

static InitFunction FNFSB2Func([]()
	{

		GetDesktopResolution(horizontal5, vertical5);

		// CRASHFIX!
		injector::MakeJMP(BaseAddress5 + 0xB7BD2, BaseAddress5 + 0xB7BE3, true);

		// PATCH WHEEL WRITE
		injector::WriteMemoryRaw((0x6AA87 + BaseAddress5), "\xDD\xD8\x90\x90\x90\x90", 6, true);
		// PATCH GAS WRITE
		injector::WriteMemoryRaw((0x6AADD + BaseAddress5), "\xDD\xD8\x90\x90\x90\x90", 6, true);
		injector::WriteMemoryRaw((0x6AAFE + BaseAddress5), "\xDD\xD8\x90\x90\x90\x90", 6, true);
		injector::WriteMemoryRaw((0x6AB13 + BaseAddress5), "\xDD\xD8\x90\x90\x90\x90", 6, true);
		// PATCH BRAKE WRITE
		injector::WriteMemoryRaw((0x6AB91 + BaseAddress5), "\xDD\xD8\x90\x90\x90\x90", 6, true);
		injector::WriteMemoryRaw((0x6ABAC + BaseAddress5), "\xDD\xD8\x90\x90\x90\x90", 6, true);

		// REPLACE SPACE KEY WITH ESC TO PREVENT EXITING LEVEL PREMATURELY
		injector::WriteMemory<BYTE>((0x71A82 + BaseAddress5), DIK_ESCAPE, true);

		// REMOVE ERROR MESSAGEBOX ON CLOSE
		injector::WriteMemory<BYTE>((0x11C02 + BaseAddress5), 0xEB, true);

		// REMOVE CMD WATSON nonsense
		injector::MakeNOP((0x529E62), 2, true);

		// Force correct backbuffer format
		injector::MakeJMP(BaseAddress5 + 0xB7BD2, BaseAddress5 + 0xB7BE3, true);

		CreateThread(NULL, 0, InputRT5, NULL, 0, NULL);

		MH_Initialize();
		MH_CreateHookApi(L"user32.dll", "CreateWindowExA", &CreateWindowExART5, (void**)& original_CreateWindowExA5);
		MH_CreateHookApi(L"user32.dll", "SetWindowPos", &SetWindowPosRT5, (void**)& original_SetWindowPos5);
		MH_EnableHook(MH_ALL_HOOKS);

		if (ToBool(config["General"]["Windowed"]))
		{
			// NO HIDE CURSOR
			injector::WriteMemory<BYTE>((0x11515 + BaseAddress5), 0x01, true);

			CreateThread(NULL, 0, WindowRT5, NULL, 0, NULL);

			MH_Initialize();
			MH_CreateHookApi(L"user32.dll", "DefWindowProcA", &DefWindowProcART5, (void**)& original_DefWindowProcA5);
			MH_CreateHookApi(L"user32.dll", "SetCursorPos", &SetCursorPosRT5, NULL);
			MH_EnableHook(MH_ALL_HOOKS);
		}
		else
		{
			CreateThread(NULL, 0, FullscreenRT5, NULL, 0, NULL);
		}

		// MACHINE ID setting
		if ((strcmp(config["Network"]["MachineID"].c_str(), "2") == 0))
		{
			injector::WriteMemory<DWORD>((0x46D7D8 + BaseAddress5), 0x01, true);
		}
		else if ((strcmp(config["Network"]["MachineID"].c_str(), "3") == 0))
		{
			injector::WriteMemory<DWORD>((0x46D7D8 + BaseAddress5), 0x02, true);
		}
		else if ((strcmp(config["Network"]["MachineID"].c_str(), "4") == 0))
		{
			injector::WriteMemory<DWORD>((0x46D7D8 + BaseAddress5), 0x03, true);
		}
		else // MACHINE ID = 1
		{
			injector::WriteMemory<DWORD>((0x46D7D8 + BaseAddress5), 0x00, true);
		}

		// FPS Fix
		if ((strcmp(config["General"]["FPSFix"].c_str(), "0") != 0))
		{
			std::string FPSstring = (LPCSTR)(config["General"]["FPSFix"].c_str());
			int FPSvalue = std::stoi(FPSstring);
			if (FPSvalue > 255)
			{
				FPSvalue = 0;
			}
			if (FPSvalue < 0)
			{
				FPSvalue = 0;
			}
			injector::WriteMemoryRaw((0x9F3D0 + BaseAddress5), "\x6A", 1, true);
			injector::WriteMemory<BYTE>((0x9F3D1 + BaseAddress5), FPSvalue, true);
			injector::WriteMemoryRaw((0x9F3D2 + BaseAddress5), "\xFF\x15\x7C\x71\x71\x00", 6, true);
			injector::WriteMemoryRaw((0x9F3D8 + BaseAddress5), "\xC3", 1, true);
		}

	}, GameID::FNFSB2);
#endif