#include <StdInc.h>
#include "Utility/InitFunction.h"
#include "Functions/Global.h"
#include "Utility\Hooking.Patterns.h"

// Based of the awesome work of BatteryShark! (https://mercaldim.gitbook.io/writeups/20151128)

#if _M_IX86

uint8_t* g_bLAccelEnd = nullptr;  // Left accelerator pedal speed
uint8_t* g_bRAccelEnd = nullptr;  // Right accelerator pedal speed
uint8_t* g_bKeyExc = nullptr;     // Digital button triggers
uint8_t* g_bAccelerator = nullptr;// ?
uint8_t* g_bRAccel = nullptr;     // Right accelerator pedal pressed
uint8_t* g_bLAccel = nullptr;     // Left accelerator pedal pressed
uint8_t* g_bLAccelDif = nullptr;  // Left accelerator pedal speed difference
uint8_t* g_bRAccelDif = nullptr;  // Right accelerator pedal speed difference
uint8_t* g_bBrake = nullptr;      // Brake
uint8_t* g_bHandle = nullptr;     // Steering
uint8_t* g_bKeyImm = nullptr;     // Digital button states

static bool btnStart = false;
static bool btnService = false;
static bool btnTest = false;
static bool btnCoin = false;
static bool btnSelectLeft = false;
static bool btnSelectRight = false;

extern int* ffbOffset;
extern int* ffbOffset2;

static bool ReturnTrue()
{
	return true;
}

static HWND(WINAPI* CreateWindowExAOri)(DWORD dwExStyle, LPCSTR lpClassName, LPCSTR lpWindowName, DWORD dwStyle, int X, int Y, int nWidth, int nHeight, HWND hWndParent, HMENU hMenu, HINSTANCE hInstance, LPVOID lpParam);
static HWND WINAPI CreateWindowExAHook(DWORD dwExStyle, LPCSTR lpClassName, LPCSTR lpWindowName, DWORD dwStyle, int X, int Y, int nWidth, int nHeight, HWND hWndParent, HMENU hMenu, HINSTANCE hInstance, LPVOID lpParam)
{
	LPCSTR overrideName = "OpenParrot - Frenzy Express";
	int overrideX = X;
	int overrideY = Y;
	int overrideWidth = nWidth;
	int overrideHeight = nHeight;
	DWORD overrideStyle = dwStyle;

	if (ToBool(config["General"]["Windowed"]))
	{
		overrideStyle = WS_VISIBLE | WS_POPUP | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX;

		int ResX = GetSystemMetrics(SM_CXSCREEN);
		int ResY = GetSystemMetrics(SM_CYSCREEN);

		RECT rect;
		SetRect(&rect, 0, 0, 640, 480);
		AdjustWindowRect(&rect, overrideStyle, 0);
		overrideWidth = rect.right - rect.left;
		overrideHeight = rect.bottom - rect.top;
		overrideX = ((int)ResX - overrideWidth) / 2;
		overrideY = ((int)ResY - overrideHeight) / 2;
	}

	return CreateWindowExAOri(dwExStyle, lpClassName, overrideName, overrideStyle, overrideX, overrideY, overrideWidth, overrideHeight, hWndParent, hMenu, hInstance, lpParam);
}

static void __fastcall XIOupdateLampStateHook(void* thiss, void* EDX)
{
	//info(true, "XIOupdateLampStateHook");
}

static void __fastcall XIOsetSteeringBarHook(void* thiss, void* EDX, uint8_t a2)
{
	//info(true, "XIOsetSteeringBarHook a2: %u", a2);
}

static void* __fastcall XIOXIOHook(void* thiss, void* EDX)
{
	//info(true, "XIOXIOHook");

	return thiss;
}

static int __fastcall XIOopenInterfaceHook(void* thiss, void* EDX)
{
	//info(true, "XIOopenInterfaceHook");

	return 1;
}

static void __fastcall XIOcloseInterfaceHook(void* thiss, void* EDX)
{
	//info(true, "XIOcloseInterfaceHook");
}

static void __fastcall XIOupdateHook(void* thiss, void* EDX)
{
	//info(true, "XIOupdateHook");

	uint8_t bKeyExc = 0x00;
	uint8_t bKeyImm = 0x00;

	// Start
	if (*ffbOffset & 0x08)
	{
		if (!btnStart)
		{
			btnStart = true;
			bKeyExc |= 0x01;
		}

		bKeyImm |= 0x01;
	}
	else
	{
		btnStart = false;
	}

	// Service
	if (*ffbOffset & 0x02)
	{
		if (!btnService)
		{
			btnService = true;
			bKeyExc |= 0x02;
		}

		bKeyImm |= 0x02;
	}
	else
	{
		btnService = false;
	}

	// Test
	if (*ffbOffset & 0x01)
	{
		if (!btnTest)
		{
			btnTest = true;
			bKeyExc |= 0x04;
		}

		bKeyImm |= 0x04;
	}
	else
	{
		btnTest = false;
	}

	// Coin
	if (*ffbOffset & 0x04)
	{
		if (!btnCoin)
		{
			btnCoin = true;
			bKeyExc |= 0x08;
		}

		bKeyImm |= 0x08;
	}
	else
	{
		btnCoin = false;
	}

	// Select Left
	if (*ffbOffset & 0x10)
	{
		if (!btnSelectLeft)
		{
			btnSelectLeft = true;
			bKeyExc |= 0x10;
		}

		bKeyImm |= 0x10;
	}
	else
	{
		btnSelectLeft = false;
	}

	// Select Right
	if (*ffbOffset & 0x20)
	{
		if (!btnSelectRight)
		{
			btnSelectRight = true;
			bKeyExc |= 0x20;
		}

		bKeyImm |= 0x20;
	}
	else
	{
		btnSelectRight = false;
	}

	// Write buttons to game
	*g_bKeyExc = bKeyExc;
	*g_bKeyImm = bKeyImm;

	// Steering
	*g_bHandle = (uint8_t)(*ffbOffset2);

	// Gas Left
	if (*ffbOffset & 0x40)
	{
		*g_bLAccelEnd = 1;
		*g_bLAccelDif = 1;
		*g_bLAccel = 1;
	}
	else
	{
		*g_bLAccelEnd = 0x00;
		*g_bLAccelDif = 0x00;
		*g_bLAccel = 0;
	}

	// Gas Right
	if (*ffbOffset & 0x80)
	{
		*g_bRAccelEnd = 1;
		*g_bRAccelDif = 1;
		*g_bRAccel = 1;
	}
	else
	{
		*g_bRAccelEnd = 0x00;
		*g_bRAccelDif = 0x00;
		*g_bRAccel = 0;
	}

	// Brake
	if (*ffbOffset & 0x100)
		*g_bBrake = 0xFF;
	else
		*g_bBrake = 0x00;
}

static void __fastcall XIOinitializeHook(void* thiss, void* EDX)
{
	//info(true, "XIOinitializeHook");
}

static void __fastcall XIOsetVibrationHook(void* thiss, void* EDX, char a2, int a3)
{
	//info(true, "XIOsetVibrationHook a2: %d a3: %d", a2, a3);
}

static void __fastcall XIOlampHook(void* thiss, void* EDX, uint8_t a2)
{
	//info(true, "XIOlampHook a2: %u", a2);
}

static void __fastcall XIOselectLampHook(void* thiss, void* EDX, int a2)
{
	//info(true, "XIOselectLampHook a2: %d", a2);
}

static void __fastcall XIOstartLampHook(void* thiss, void* EDX, int a2)
{
	//info(true, "XIOstartLampHook a2: %d", a2);
}

static InitFunction FrenzyExpressFunc([]()
{
	uintptr_t mainModuleBase = (uintptr_t)GetModuleHandleA(0);

	// Global variables shared by io dll and game exe
	g_bLAccelEnd = (uint8_t*)GetProcAddress(GetModuleHandleW(L"xio.dll"), "?g_bLAccelEnd@@3EA");
	g_bRAccelEnd = (uint8_t*)GetProcAddress(GetModuleHandleW(L"xio.dll"), "?g_bRAccelEnd@@3EA");
	g_bKeyExc = (uint8_t*)GetProcAddress(GetModuleHandleW(L"xio.dll"), "?g_bKeyExc@@3EA");
	g_bAccelerator = (uint8_t*)GetProcAddress(GetModuleHandleW(L"xio.dll"), "?g_bAccelerator@@3EA");
	g_bRAccel = (uint8_t*)GetProcAddress(GetModuleHandleW(L"xio.dll"), "?g_bRAccel@@3EA");
	g_bLAccel = (uint8_t*)GetProcAddress(GetModuleHandleW(L"xio.dll"), "?g_bLAccel@@3EA");
	g_bLAccelDif = (uint8_t*)GetProcAddress(GetModuleHandleW(L"xio.dll"), "?g_bLAccelDif@@3EA");
	g_bRAccelDif = (uint8_t*)GetProcAddress(GetModuleHandleW(L"xio.dll"), "?g_bRAccelDif@@3EA");
	g_bBrake = (uint8_t*)GetProcAddress(GetModuleHandleW(L"xio.dll"), "?g_bBrake@@3EA");
	g_bHandle = (uint8_t*)GetProcAddress(GetModuleHandleW(L"xio.dll"), "?g_bHandle@@3EA");
	g_bKeyImm = (uint8_t*)GetProcAddress(GetModuleHandleW(L"xio.dll"), "?g_bKeyImm@@3EA");

	// Hooks
	MH_Initialize();

	// This blocks the game from changing your windows resolution
	if (ToBool(config["General"]["Windowed"]))
		MH_CreateHook((void*)(mainModuleBase + 0x28020), ReturnTrue, NULL);

	MH_CreateHookApi(L"user32.dll", "CreateWindowExA", CreateWindowExAHook, (void**)&CreateWindowExAOri);

	MH_CreateHookApi(L"xio.dll", "?updateLampState@XIO@@QAEXXZ", XIOupdateLampStateHook, NULL); // void __thiscall XIO::updateLampState(XIO *this)
	MH_CreateHookApi(L"xio.dll", "?setSteeringBar@XIO@@QAEXE@Z", XIOsetSteeringBarHook, NULL); // void __thiscall XIO::setSteeringBar(XIO *this, unsigned __int8 a2)
	MH_CreateHookApi(L"xio.dll", "??0XIO@@QAE@XZ", XIOXIOHook, NULL); // XIO *__thiscall XIO::XIO(XIO *this)
	MH_CreateHookApi(L"xio.dll", "?openInterface@XIO@@QAEHXZ", XIOopenInterfaceHook, NULL); // int __thiscall XIO::openInterface(XIO *this)
	MH_CreateHookApi(L"xio.dll", "?closeInterface@XIO@@QAEXXZ", XIOcloseInterfaceHook, NULL); // void __thiscall XIO::closeInterface(XIO *this)
	MH_CreateHookApi(L"xio.dll", "?update@XIO@@QAEXXZ", XIOupdateHook, NULL); // void __thiscall XIO::update(XIO *this)
	MH_CreateHookApi(L"xio.dll", "?initialize@XIO@@QAEXXZ", XIOinitializeHook, NULL); // void __thiscall XIO::initialize(XIO *this)
	MH_CreateHookApi(L"xio.dll", "?setVibration@XIO@@QAEXEH@Z", XIOsetVibrationHook, NULL); // void __thiscall XIO::setVibration(XIO *this, char a2, int a3)
	MH_CreateHookApi(L"xio.dll", "?lamp@XIO@@QAEXE@Z", XIOlampHook, NULL); // void __thiscall XIO::lamp(XIO *this, unsigned __int8 a2)
	MH_CreateHookApi(L"xio.dll", "?selectLamp@XIO@@QAEXH@Z", XIOselectLampHook, NULL); // void __thiscall XIO::selectLamp(XIO *this, int a2)
	MH_CreateHookApi(L"xio.dll", "?startLamp@XIO@@QAEXH@Z", XIOstartLampHook, NULL); // void __thiscall XIO::startLamp(XIO *this, int a2)

	MH_EnableHook(MH_ALL_HOOKS);
}, GameID::FrenzyExpress);

#endif