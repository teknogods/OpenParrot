#define CINTERFACE
#include <StdInc.h>
#include "Utility/InitFunction.h"
#pragma optimize("", off)
#include "dx8/d3d8.h"
#include "Utility/GameDetect.h"
#include "Global.h"
#include "FpsLimiter.h"

// Local variables
static bool Windowed = false;
static bool FpsLimiterEnable = false;
static bool SoftwareShaderHack = false;

// Prototypes
static HRESULT(WINAPI* g_oldPresent)(IDirect3DDevice8* self, CONST RECT* pSourceRect, CONST RECT* pDestRect, HWND hDestWindowOverride, CONST RGNDATA* pDirtyRegion);
static HRESULT(WINAPI* g_oldReset)(IDirect3DDevice8* self, D3DPRESENT_PARAMETERS* pPresentationParameters);
static HRESULT(WINAPI* g_oldCreateDevice)(IDirect3D8* self, UINT Adapter, D3DDEVTYPE DeviceType, HWND hFocusWindow, DWORD BehaviorFlags, D3DPRESENT_PARAMETERS* pPresentationParameters, IDirect3DDevice8** ppReturnedDeviceInterface);
static IDirect3D8* (WINAPI* g_origDirect3DCreate8)(UINT SDKVersion);

static HRESULT WINAPI PresentWrap(IDirect3DDevice8* self, CONST RECT* pSourceRect, CONST RECT* pDestRect, HWND hDestWindowOverride, CONST RGNDATA* pDirtyRegion);
static HRESULT WINAPI ResetWrap(IDirect3DDevice8* self, D3DPRESENT_PARAMETERS* pPresentationParameters);
static HRESULT WINAPI CreateDeviceWrap(IDirect3D8* self, UINT Adapter, D3DDEVTYPE DeviceType, HWND hFocusWindow, DWORD BehaviorFlags, D3DPRESENT_PARAMETERS* pPresentationParameters, IDirect3DDevice8** ppReturnedDeviceInterface);
static IDirect3D8* WINAPI Direct3DCreate8Wrap(UINT SDKVersion);

// Functions
template<typename T>
inline T HookVtableFunction(T* functionPtr, T target)
{
	if (*functionPtr == target)
	{
		return nullptr;
	}

	auto old = *functionPtr;
	injector::WriteMemory(functionPtr, target, true);

	return old;
}

static HRESULT WINAPI PresentWrap(IDirect3DDevice8* self, CONST RECT* pSourceRect, CONST RECT* pDestRect, HWND hDestWindowOverride, CONST RGNDATA* pDirtyRegion)
{
	if (FpsLimiterEnable)
		FpsLimiter();

	return g_oldPresent(self, pSourceRect, pDestRect, hDestWindowOverride, pDirtyRegion);
}

static HRESULT WINAPI ResetWrap(IDirect3DDevice8* self, D3DPRESENT_PARAMETERS* pPresentationParameters)
{
	if (Windowed)
	{
		pPresentationParameters->Windowed = TRUE;
		pPresentationParameters->FullScreen_RefreshRateInHz = 0;
		pPresentationParameters->BackBufferWidth = 0;
		pPresentationParameters->BackBufferHeight = 0;
		pPresentationParameters->BackBufferFormat = D3DFMT_A8R8G8B8; // TODO: query current desktop format and use it
		pPresentationParameters->Flags = 0;
		pPresentationParameters->FullScreen_PresentationInterval = D3DPRESENT_INTERVAL_DEFAULT;
	}

	return g_oldReset(self, pPresentationParameters);
}

static bool useXRGB = false;
static HRESULT WINAPI CreateDeviceWrap(IDirect3D8* self, UINT Adapter, D3DDEVTYPE DeviceType, HWND hFocusWindow, DWORD BehaviorFlags, D3DPRESENT_PARAMETERS* pPresentationParameters, IDirect3DDevice8** ppReturnedDeviceInterface)
{
	UINT originalBBW = pPresentationParameters->BackBufferWidth;
	UINT originalBBH = pPresentationParameters->BackBufferHeight;

	if (Windowed)
	{
		pPresentationParameters->Windowed = TRUE;
		pPresentationParameters->FullScreen_RefreshRateInHz = 0;
		pPresentationParameters->BackBufferWidth = 0;
		pPresentationParameters->BackBufferHeight = 0;
		pPresentationParameters->Flags = 0;
		if (useXRGB)
		{
			pPresentationParameters->BackBufferFormat = D3DFMT_X8R8G8B8; // TODO: query current desktop format and use it
		}
		else {
			pPresentationParameters->BackBufferFormat = D3DFMT_A8R8G8B8; // TODO: query current desktop format and use it
		}

		pPresentationParameters->FullScreen_PresentationInterval = D3DPRESENT_INTERVAL_DEFAULT;
	}

	if (SoftwareShaderHack)
		BehaviorFlags = D3DCREATE_SOFTWARE_VERTEXPROCESSING;

	HRESULT hr = g_oldCreateDevice(self, Adapter, DeviceType, hFocusWindow, BehaviorFlags, pPresentationParameters, ppReturnedDeviceInterface);

	if (*ppReturnedDeviceInterface)
	{
	if (Windowed)
		{
			auto old = HookVtableFunction(&(*ppReturnedDeviceInterface)->lpVtbl->Reset, ResetWrap);
			g_oldReset = (old) ? old : g_oldReset;
		}

		if (FpsLimiterEnable)
		{
			auto old2 = HookVtableFunction(&(*ppReturnedDeviceInterface)->lpVtbl->Present, PresentWrap);
			g_oldPresent = (old2) ? old2 : g_oldPresent;
		}
	}

	return hr;
}

static IDirect3D8* WINAPI Direct3DCreate8Wrap(UINT SDKVersion)
{
	auto d3d9 = g_origDirect3DCreate8(SDKVersion);

	auto old = HookVtableFunction(&d3d9->lpVtbl->CreateDevice, CreateDeviceWrap);
	g_oldCreateDevice = (old) ? old : g_oldCreateDevice;

	return d3d9;
}

static void InitD3D8WindowHook()
{
	MH_Initialize();
	MH_CreateHookApi(L"d3d8.dll", "Direct3DCreate8", Direct3DCreate8Wrap, (void**)&g_origDirect3DCreate8);
	MH_EnableHook(MH_ALL_HOOKS);
}

extern linb::ini config;

static InitFunction initFunc([]()
	{
		if (GameDetect::currentGame == GameID::BG4)
			return;
		if (GameDetect::currentGame == GameID::BG4_Eng)
			return;
		if (GameDetect::currentGame == GameID::TER)
			return;
		if (GameDetect::currentGame == GameID::FNF || GameDetect::currentGame == GameID::FNFSB)
		{
			useXRGB = true;
		}

		// Make local variables for speed
		Windowed = ToBool(config["General"]["Windowed"]);

		// Old boolean based limit of 60 fps (keep this for backward compatibility)
		if (ToBool(config["General"]["Framelimiter"]))
		{
			FpsLimiterEnable = true;
			FpsLimiterSet(60.0f);
		}
		// New configurable fps limit
		else if (ToBool(config["General"]["FpsLimitEnable"]))
		{
			FpsLimiterEnable = true;
			FpsLimiterSet((float)FetchDwordInformation("General", "FpsLimit", 60));
		}

		SoftwareShaderHack = ToBool(config["General"]["SoftwareVertexShaders"]);

		if (Windowed || FpsLimiterEnable || SoftwareShaderHack)
			InitD3D8WindowHook();
	});
#pragma optimize("", on)