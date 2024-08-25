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
extern int* ffbOffset5;

static char Key0Char[256];
static char Key1Char[256];
static char Key2Char[256];
static char Key3Char[256];
static char Key4Char[256];
static char Key5Char[256];
static char Key6Char[256];
static char Key7Char[256];
static char Key8Char[256];
static char Key9Char[256];
static char KeyDelChar[256];
static char KeyEndChar[256];

static int Keypad0 = 0;
static int Keypad1 = 0;
static int Keypad2 = 0;
static int Keypad3 = 0;
static int Keypad4 = 0;
static int Keypad5 = 0;
static int Keypad6 = 0;
static int Keypad7 = 0;
static int Keypad8 = 0;
static int Keypad9 = 0;
static int KeypadDel = 0;
static int KeypadEnd = 0;

static bool startPressed = false;
static bool button1Pressed = false;
static bool button2Pressed = false;
static bool button3Pressed = false;
static bool buttonMusicPressed = false;
static bool buttonCoin1Pressed = false;
static bool buttonCoin2Pressed = false;
static bool buttonBillPressed = false;
static bool buttonTestPressed = false;
static bool buttonVolumeUpPressed = false;
static bool buttonVolumeDownPressed = false;
static bool buttonCreditPressed = false;

static bool buttonGear1Pressed = false;
static bool buttonGear2Pressed = false;
static bool buttonGear3Pressed = false;
static bool buttonGear4Pressed = false;
static bool buttonGear5Pressed = false;
static bool buttonGear6Pressed = false;

static bool buttonBrakePressed = false;

static bool numpad0Pressed = false;
static bool numpad1Pressed = false;
static bool numpad2Pressed = false;
static bool numpad3Pressed = false;
static bool numpad4Pressed = false;
static bool numpad5Pressed = false;
static bool numpad6Pressed = false;
static bool numpad7Pressed = false;
static bool numpad8Pressed = false;
static bool numpad9Pressed = false;
static bool numpadStarPressed = false;
static bool numpadSlashPressed = false;

// hooks ori
BOOL(__stdcall* original_SetWindowPos2)(HWND hWnd, HWND hWndInsertAfter, int X, int Y, int cx, int cy, UINT uFlags);
BOOL(__stdcall* original_CreateWindowExA2)(DWORD dwExStyle, LPCSTR lpClassName, LPCSTR lpWindowName, DWORD dwStyle, int X, int Y, int nWidth, int nHeight, HWND hWndParent, HMENU hMenu, HINSTANCE hInstance, LPVOID lpParam);

static int(__cdecl* SetControlOrig)(u_int param_1, u_int param_2);
static int __cdecl SetControl(u_int param_1, u_int param_2)
{
	return SetControlOrig(param_1, param_2);
}

DWORD WINAPI InputRT2(LPVOID lpParam)
{
	int deltaTimer = 16;
	INT_PTR keyboardBuffer = (0x437F6F8 + BaseAddress2);
	INT_PTR keyboardBuffer2 = (0x437FC08 + BaseAddress2);

	GetPrivateProfileStringA("Keypad", "0 Key", "", Key0Char, 256, ".\\teknoparrot.ini");
	GetPrivateProfileStringA("Keypad", "1 Key", "", Key1Char, 256, ".\\teknoparrot.ini");
	GetPrivateProfileStringA("Keypad", "2 Key", "", Key2Char, 256, ".\\teknoparrot.ini");
	GetPrivateProfileStringA("Keypad", "3 Key", "", Key3Char, 256, ".\\teknoparrot.ini");
	GetPrivateProfileStringA("Keypad", "4 Key", "", Key4Char, 256, ".\\teknoparrot.ini");
	GetPrivateProfileStringA("Keypad", "5 Key", "", Key5Char, 256, ".\\teknoparrot.ini");
	GetPrivateProfileStringA("Keypad", "6 Key", "", Key6Char, 256, ".\\teknoparrot.ini");
	GetPrivateProfileStringA("Keypad", "7 Key", "", Key7Char, 256, ".\\teknoparrot.ini");
	GetPrivateProfileStringA("Keypad", "8 Key", "", Key8Char, 256, ".\\teknoparrot.ini");
	GetPrivateProfileStringA("Keypad", "9 Key", "", Key9Char, 256, ".\\teknoparrot.ini");
	GetPrivateProfileStringA("Keypad", "Del Key", "", KeyDelChar, 256, ".\\teknoparrot.ini");
	GetPrivateProfileStringA("Keypad", "End Key", "", KeyEndChar, 256, ".\\teknoparrot.ini");

	std::string Key0 = Key0Char;
	if (Key0.find('0x') != std::string::npos)
	{
		std::string Key0_string = Key0Char;
		Keypad0 = std::stoi(Key0_string, nullptr, 16);
	}
	std::string Key1 = Key1Char;
	if (Key1.find('0x') != std::string::npos)
	{
		std::string Key1_string = Key1Char;
		Keypad1 = std::stoi(Key1_string, nullptr, 16);
	}
	std::string Key2 = Key2Char;
	if (Key2.find('0x') != std::string::npos)
	{
		std::string Key2_string = Key2Char;
		Keypad2 = std::stoi(Key2_string, nullptr, 16);
	}
	std::string Key3 = Key3Char;
	if (Key3.find('0x') != std::string::npos)
	{
		std::string Key3_string = Key3Char;
		Keypad3 = std::stoi(Key3_string, nullptr, 16);
	}
	std::string Key4 = Key4Char;
	if (Key4.find('0x') != std::string::npos)
	{
		std::string Key4_string = Key4Char;
		Keypad4 = std::stoi(Key4_string, nullptr, 16);
	}
	std::string Key5 = Key5Char;
	if (Key5.find('0x') != std::string::npos)
	{
		std::string Key5_string = Key5Char;
		Keypad5 = std::stoi(Key5_string, nullptr, 16);
	}
	std::string Key6 = Key6Char;
	if (Key6.find('0x') != std::string::npos)
	{
		std::string Key6_string = Key6Char;
		Keypad6 = std::stoi(Key6_string, nullptr, 16);
	}
	std::string Key7 = Key7Char;
	if (Key7.find('0x') != std::string::npos)
	{
		std::string Key7_string = Key7Char;
		Keypad7 = std::stoi(Key7_string, nullptr, 16);
	}
	std::string Key8 = Key8Char;
	if (Key8.find('0x') != std::string::npos)
	{
		std::string Key8_string = Key8Char;
		Keypad8 = std::stoi(Key8_string, nullptr, 16);
	}
	std::string Key9 = Key9Char;
	if (Key9.find('0x') != std::string::npos)
	{
		std::string Key9_string = Key9Char;
		Keypad9 = std::stoi(Key9_string, nullptr, 16);
	}
	std::string KeyDel = KeyDelChar;
	if (KeyDel.find('0x') != std::string::npos)
	{
		std::string KeyDel_string = KeyDelChar;
		KeypadDel = std::stoi(KeyDel_string, nullptr, 16);
	}
	std::string KeyEnd = KeyEndChar;
	if (KeyEnd.find('0x') != std::string::npos)
	{
		std::string KeyEnd_string = KeyEndChar;
		KeypadEnd = std::stoi(KeyEnd_string, nullptr, 16);
	}



	while (true)
	{
		if (GetKeyState(Keypad0) & 0x8000)
		{
			SetControlOrig(0x10080, 0x1);
			numpad0Pressed = true;
		}
		else if (numpad0Pressed) {
			SetControlOrig(0x10080, 0x0);
			numpad0Pressed = false;
		}

		if (GetKeyState(Keypad1) & 0x8000)
		{
			SetControlOrig(0x10081, 0x1);
			numpad1Pressed = true;
		}
		else if (numpad1Pressed) {
			SetControlOrig(0x10081, 0x0);
			numpad1Pressed = false;
		}

		if (GetKeyState(Keypad2) & 0x8000)
		{
			SetControlOrig(0x10082, 0x1);
			numpad2Pressed = true;
		}
		else if (numpad2Pressed) {
			SetControlOrig(0x10082, 0x0);
			numpad2Pressed = false;
		}

		if (GetKeyState(Keypad3) & 0x8000)
		{
			SetControlOrig(0x10083, 0x1);
			numpad3Pressed = true;
		}
		else if (numpad3Pressed) {
			SetControlOrig(0x10083, 0x0);
			numpad3Pressed = false;
		}

		if (GetKeyState(Keypad4) & 0x8000)
		{
			SetControlOrig(0x10084, 0x1);
			numpad4Pressed = true;
		}
		else if (numpad4Pressed) {
			SetControlOrig(0x10084, 0x0);
			numpad4Pressed = false;
		}

		if (GetKeyState(Keypad5) & 0x8000)
		{
			SetControlOrig(0x10085, 0x1);
			numpad5Pressed = true;
		}
		else if (numpad5Pressed) {
			SetControlOrig(0x10085, 0x0);
			numpad5Pressed = false;
		}

		if (GetKeyState(Keypad6) & 0x8000)
		{
			SetControlOrig(0x10086, 0x1);
			numpad6Pressed = true;
		}
		else if (numpad6Pressed) {
			SetControlOrig(0x10086, 0x0);
			numpad6Pressed = false;
		}

		if (GetKeyState(Keypad7) & 0x8000)
		{
			SetControlOrig(0x10087, 0x1);
			numpad7Pressed = true;
		}
		else if (numpad7Pressed) {
			SetControlOrig(0x10087, 0x0);
			numpad7Pressed = false;
		}

		if (GetKeyState(Keypad8) & 0x8000)
		{
			SetControlOrig(0x10088, 0x1);
			numpad8Pressed = true;
		}
		else if (numpad8Pressed) {
			SetControlOrig(0x10088, 0x0);
			numpad8Pressed = false;
		}

		if (GetKeyState(Keypad9) & 0x8000)
		{
			SetControlOrig(0x10089, 0x1);
			numpad9Pressed = true;
		}
		else if (numpad9Pressed) {
			SetControlOrig(0x10089, 0x0);
			numpad9Pressed = false;
		}

		if (GetKeyState(KeypadDel) & 0x8000)
		{
			SetControlOrig(0x1008A, 0x1);
			numpadStarPressed = true;
		}
		else if (numpadStarPressed) {
			SetControlOrig(0x1008A, 0x0);
			numpadStarPressed = false;
		}

		if (GetKeyState(KeypadEnd) & 0x8000)
		{
			SetControlOrig(0x1008B, 0x1);
			numpadSlashPressed = true;
		}
		else if (numpadSlashPressed) {
			SetControlOrig(0x1008B, 0x0);
			numpadSlashPressed = false;
		}

		//START
		if (*ffbOffset & 0x08)
		{
			if (!startPressed)
			{
				SetControlOrig(0x10000, 0x1);
				startPressed = true;
			}
		}
		else if (startPressed) {
			SetControlOrig(0x10000, 0x0);
			startPressed = false;
		}

		//BUTTON 1
		if (*ffbOffset & 0x200)
		{
			if (!button1Pressed)
			{
				SetControlOrig(0x10001, 0x1);
				button1Pressed = true;
			}
		}
		else if (button1Pressed) {
			SetControlOrig(0x10001, 0x0);
			button1Pressed = false;
		}

		//BUTTON 2
		if (*ffbOffset & 0x400)
		{
			if (!button2Pressed)
			{
				SetControlOrig(0x10002, 0x1);
				button2Pressed = true;
			}
		}
		else if (button2Pressed) {
			SetControlOrig(0x10002, 0x0);
			button2Pressed = false;
		}

		//BUTTON 3
		if (*ffbOffset & 0x800)
		{
			if (!button3Pressed)
			{
				SetControlOrig(0x10003, 0x1);
				button3Pressed = true;
			}
		}
		else if (button3Pressed) {
			SetControlOrig(0x10003, 0x0);
			button3Pressed = false;
		}

		//BUTTON MUSIC
		if (*ffbOffset & 0x10)
		{
			if (!buttonMusicPressed)
			{
				SetControlOrig(0x10004, 0x1);
				buttonMusicPressed = true;
			}
		}
		else if (buttonMusicPressed) {
			SetControlOrig(0x10004, 0x0);
			buttonMusicPressed = false;
		}

		//BUTTON TEST
		if (*ffbOffset & 0x01)
		{
			if (!buttonTestPressed)
			{
				SetControlOrig(0x1000b, 0x1);
				buttonTestPressed = true;
			}
		}
		else if (buttonTestPressed) {
			SetControlOrig(0x1000b, 0x0);
			buttonTestPressed = false;
		}

		//BUTTON COIN 1
		if (*ffbOffset & 0x0004)
		{
			if (!buttonCoin1Pressed)
			{
				SetControlOrig(0x10009, 0x1);
				buttonCoin1Pressed = true;
			}
		}
		else if (buttonCoin1Pressed) {
			SetControlOrig(0x10009, 0x0);
			buttonCoin1Pressed = false;
		}


		//BUTTON VOLUME UP
		if (*ffbOffset & 0x4000)
		{
			if (!buttonVolumeUpPressed)
			{
				SetControlOrig(0x1000e, 0x1);
				buttonVolumeUpPressed = true;
			}
		}
		else if (buttonVolumeUpPressed) {
			SetControlOrig(0x1000e, 0x0);
			buttonVolumeUpPressed = false;
		}

		//BUTTON VOLUME DOWN
		if (*ffbOffset & 0x8000)
		{
			if (!buttonVolumeDownPressed)
			{
				SetControlOrig(0x1000f, 0x1);
				buttonVolumeDownPressed = true;
			}
		}
		else if (buttonVolumeDownPressed) {
			SetControlOrig(0x1000f, 0x0);
			buttonVolumeDownPressed = false;
		}



		//GEAR 1 
		if (*ffbOffset5 & 0x1)
		{
			if (!buttonGear1Pressed)
			{
				SetControlOrig(0x10005, 0x1);
				SetControlOrig(0x10006, 0x0);
				SetControlOrig(0x10007, 0x0);
				SetControlOrig(0x10008, 0x0);
				buttonGear1Pressed = true;
			}
		}
		else if (buttonGear1Pressed) {
			SetControlOrig(0x10005, 0x0);
			buttonGear1Pressed = false;
		}

		//GEAR 2
		if (*ffbOffset5 & 0x2)
		{
			if (!buttonGear2Pressed)
			{
				SetControlOrig(0x10006, 0x1);
				SetControlOrig(0x10005, 0x0);
				SetControlOrig(0x10007, 0x0);
				SetControlOrig(0x10008, 0x0);
				buttonGear2Pressed = true;
			}
		}
		else if (buttonGear2Pressed) {
			SetControlOrig(0x10006, 0x0);
			buttonGear2Pressed = false;
		}

		//GEAR 3
		if (*ffbOffset5 & 0x4)
		{
			if (!buttonGear3Pressed)
			{
				SetControlOrig(0x10007, 0x1);
				SetControlOrig(0x10005, 0x0);
				SetControlOrig(0x10006, 0x0);
				SetControlOrig(0x10008, 0x0);
				buttonGear3Pressed = true;
			}
		}
		else if (buttonGear3Pressed) {
			SetControlOrig(0x10007, 0x0);
			buttonGear3Pressed = false;
		}

		//GEAR 4 TODO
		if (*ffbOffset5 & 0x8)
		{
			if (!buttonGear4Pressed)
			{
				SetControlOrig(0x10008, 0x1);
				SetControlOrig(0x10005, 0x0);
				SetControlOrig(0x10006, 0x0);
				SetControlOrig(0x10007, 0x0);
				buttonGear4Pressed = true;
			}
		}
		else if (buttonGear4Pressed) {
			SetControlOrig(0x10008, 0x0);
			buttonGear4Pressed = false;
		}


		// Need to do this in a mo

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

BOOL(__stdcall* original_DefWindowProcA2)(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
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
	return original_CreateWindowExA2(dwExStyle, lpClassName, "FNF SuperCars", 0x96000000, 0, 0, nWidth, nHeight, hWndParent, hMenu, hInstance, lpParam);
}

DWORD WINAPI SetCursorPosRT2(int X, int Y)
{
	return 1;
}

DWORD WINAPI SetWindowPosRT2(HWND hWnd, HWND hWndInsertAfter, int X, int Y, int cx, int cy, UINT uFlags)
{
	return 1;
}


static int(__cdecl* SetUSBIOOnOrig)();
static int __cdecl SetUSBIOOn()
{
	return 1;
}

//004c27f0 void h2oBoardSendMessages(void)
static void(__cdecl* h2oBoardSendMessagesOrig)();
static void __cdecl h2oBoardSendMessages()
{
	return;
}

static InitFunction FNFSCFunc([]()
	{
		GetDesktopResolution(horizontal2, vertical2);
		cursorhndle2 = GetCursor();

		// REMOVE ERROR MESSAGEBOX ON CLOSE
		injector::WriteMemory<BYTE>((0x73794 + BaseAddress2), 0xEB, true);

		// REMOVE CMD WATSON nonsense
		injector::MakeNOP((0x5F3882), 2, true);

		// REMOVE ESC BOX
		injector::MakeNOP((0x464A58), 5, true);

		//TURN OFF hack for keyboard + dinput, we'll do it ourselves above.
		injector::MakeNOP((0x04741db), 5, true);

		CreateThread(NULL, 0, InputRT2, NULL, 0, NULL);

		MH_Initialize();
		MH_CreateHookApi(L"user32.dll", "CreateWindowExA", &CreateWindowExART2, (void**)&original_CreateWindowExA2);
		MH_CreateHookApi(L"user32.dll", "SetWindowPos", &SetWindowPosRT2, (void**)&original_SetWindowPos2);
		MH_CreateHook((void*)0x04a9510, SetControl, (void**)&SetControlOrig);
		MH_CreateHook((void*)0x0401ab0, SetUSBIOOn, (void**)&SetUSBIOOnOrig);
		MH_CreateHook((void*)0x04c27f0, h2oBoardSendMessages, (void**)&h2oBoardSendMessagesOrig);


		MH_EnableHook(MH_ALL_HOOKS);



		if (ToBool(config["General"]["Windowed"]))
		{
			// NO HIDE CURSOR
			//injector::WriteMemory<BYTE>((0x72FCF + BaseAddress2), 0x01, true);

			MH_Initialize();
			MH_CreateHookApi(L"user32.dll", "DefWindowProcA", &DefWindowProcART2, (void**)&original_DefWindowProcA2);
			MH_CreateHookApi(L"user32.dll", "SetCursorPos", &SetCursorPosRT2, NULL);
			MH_EnableHook(MH_ALL_HOOKS);
		}
		else
		{
			CreateThread(NULL, 0, FullscreenRT2, NULL, 0, NULL);
		}

		// use relative paths instead of absolute paths 
		safeJMP(0x4a7ee0, genericRetZero, true);

		// Changing the res breaks any sort of 2D UI T_T
		//injector::WriteMemory<DWORD>(0x730f24, 1920, true);
		//injector::WriteMemory<DWORD>(0x730f28, 1080, true);

		// MACHINE ID setting
		if ((strcmp(config["Network"]["MachineID"].c_str(), "2") == 0))
		{
			injector::WriteMemory<DWORD>((0x3036A8 + BaseAddress2), 0x01, true);
		}
		else if ((strcmp(config["Network"]["MachineID"].c_str(), "3") == 0))
		{
			injector::WriteMemory<DWORD>((0x3036A8 + BaseAddress2), 0x02, true);
		}
		else if ((strcmp(config["Network"]["MachineID"].c_str(), "4") == 0))
		{
			injector::WriteMemory<DWORD>((0x3036A8 + BaseAddress2), 0x03, true);
		}
		else // MACHINE ID = 1
		{
			injector::WriteMemory<DWORD>((0x3036A8 + BaseAddress2), 0x00, true);
		}

	}, GameID::FNFSC);
