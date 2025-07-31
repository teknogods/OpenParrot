#define CINTERFACE
#include <StdInc.h>
#include "Utility/InitFunction.h"
#pragma optimize("", off)
#include "d3d9.h"
#include "Utility/GameDetect.h"
#include "Global.h"
#include "FpsLimiter.h"

// Local variables
static bool Windowed = false;
static bool FpsLimiterEnable = false;
static bool MultiScreen = false;
static bool TextureFixEnable = false;

// Prototypes
static HRESULT(WINAPI* g_oldPresent)(IDirect3DDevice9* self, CONST RECT* pSourceRect, CONST RECT* pDestRect, HWND hDestWindowOverride, CONST RGNDATA* pDirtyRegion);
static HRESULT(WINAPI* g_oldReset)(IDirect3DDevice9* self, D3DPRESENT_PARAMETERS* pPresentationParameters);
static HRESULT(WINAPI* g_oldCreateDevice)(IDirect3D9* self, UINT Adapter, D3DDEVTYPE DeviceType, HWND hFocusWindow, DWORD BehaviorFlags, D3DPRESENT_PARAMETERS* pPresentationParameters, IDirect3DDevice9** ppReturnedDeviceInterface);
static HRESULT(WINAPI* g_oldSetTexture)(IDirect3DDevice9* self, DWORD Stage, IDirect3DBaseTexture9* pTexture);
static IDirect3D9* (WINAPI* g_origDirect3DCreate9)(UINT SDKVersion);

static IDirect3DBaseTexture9* g_EmptyTexture = nullptr;

static HRESULT WINAPI PresentWrap(IDirect3DDevice9* self, CONST RECT* pSourceRect, CONST RECT* pDestRect, HWND hDestWindowOverride, CONST RGNDATA* pDirtyRegion);
static HRESULT WINAPI ResetWrap(IDirect3DDevice9* self, D3DPRESENT_PARAMETERS* pPresentationParameters);
static HRESULT WINAPI CreateDeviceWrap(IDirect3D9* self, UINT Adapter, D3DDEVTYPE DeviceType, HWND hFocusWindow, DWORD BehaviorFlags, D3DPRESENT_PARAMETERS* pPresentationParameters, IDirect3DDevice9** ppReturnedDeviceInterface);
static HRESULT WINAPI SetTextureWrap(IDirect3DDevice9* self, DWORD Stage, IDirect3DBaseTexture9* pTexture);
static IDirect3D9* WINAPI Direct3DCreate9Wrap(UINT SDKVersion);

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

// Workaround where fixed function pipelines in some games (like crazy speed)
// freak out because the game clears stage 0 (sets it to NULL) but then
// still tries to use it to modulate color...
static HRESULT CreateEmptyTexture(IDirect3DDevice9* pDevice)
{
	if (g_EmptyTexture)
		return S_OK;

	IDirect3DTexture9* pTexture = nullptr;
	HRESULT hr = pDevice->lpVtbl->CreateTexture(pDevice, 1, 1, 1, 0, D3DFMT_A8R8G8B8, D3DPOOL_MANAGED, &pTexture, nullptr);

	if (SUCCEEDED(hr))
	{
		D3DLOCKED_RECT lockedRect;
		hr = pTexture->lpVtbl->LockRect(pTexture, 0, &lockedRect, nullptr, 0);
		if (SUCCEEDED(hr))
		{
			DWORD* pixel = (DWORD*)lockedRect.pBits;
			*pixel = 0xFFFFFFFF;
			pTexture->lpVtbl->UnlockRect(pTexture, 0);
			g_EmptyTexture = (IDirect3DBaseTexture9*)pTexture;
		}
	}

	return hr;
}

static HRESULT WINAPI SetTextureWrap(IDirect3DDevice9* self, DWORD Stage, IDirect3DBaseTexture9* pTexture)
{
	if (!pTexture)
	{
		if (!g_EmptyTexture)
		{
			CreateEmptyTexture(self);
		}
		return g_oldSetTexture(self, Stage, g_EmptyTexture);
	}

	return g_oldSetTexture(self, Stage, pTexture);
}

static HRESULT WINAPI PresentWrap(IDirect3DDevice9* self, CONST RECT* pSourceRect, CONST RECT* pDestRect, HWND hDestWindowOverride, CONST RGNDATA* pDirtyRegion)
{
	if (FpsLimiterEnable)
		FpsLimiter();

	return g_oldPresent(self, pSourceRect, pDestRect, hDestWindowOverride, pDirtyRegion);
}

static HRESULT WINAPI ResetWrap(IDirect3DDevice9* self, D3DPRESENT_PARAMETERS* pPresentationParameters)
{
	if (Windowed)
	{
		pPresentationParameters->Windowed = TRUE;
		pPresentationParameters->FullScreen_RefreshRateInHz = 0;
	}

	if (GameDetect::currentGame == GameID::TroubleWitches)
		pPresentationParameters->BackBufferFormat = D3DFMT_A8R8G8B8; // TODO: query current desktop format and use it


	if (g_EmptyTexture)
	{
		g_EmptyTexture->lpVtbl->Release(g_EmptyTexture);
		g_EmptyTexture = nullptr;
	}

	return g_oldReset(self, pPresentationParameters);
}

static HRESULT WINAPI CreateDeviceWrap(IDirect3D9* self, UINT Adapter, D3DDEVTYPE DeviceType, HWND hFocusWindow, DWORD BehaviorFlags, D3DPRESENT_PARAMETERS* pPresentationParameters, IDirect3DDevice9** ppReturnedDeviceInterface)
{
	if (Windowed || GameDetect::currentGame == GameID::ElevatorActionDeathParade && !MultiScreen)
	{
		pPresentationParameters->Windowed = TRUE;
		pPresentationParameters->FullScreen_RefreshRateInHz = 0;
	}

	if (GameDetect::currentGame == GameID::TroubleWitches)
		pPresentationParameters->BackBufferFormat = D3DFMT_A8R8G8B8; // TODO: query current desktop format and use it

	if (GameDetect::currentGame == GameID::CrazySpeed)
	{
		pPresentationParameters->BackBufferFormat = D3DFMT_X8R8G8B8;
	}

	HRESULT hr = g_oldCreateDevice(self, Adapter, DeviceType, hFocusWindow, BehaviorFlags, pPresentationParameters, ppReturnedDeviceInterface);

	if (*ppReturnedDeviceInterface)
	{
		if (Windowed || GameDetect::currentGame == GameID::TroubleWitches)
		{
			auto old1 = HookVtableFunction(&(*ppReturnedDeviceInterface)->lpVtbl->Reset, ResetWrap);
			g_oldReset = (old1) ? old1 : g_oldReset;
		}

		if (FpsLimiterEnable)
		{
			auto old2 = HookVtableFunction(&(*ppReturnedDeviceInterface)->lpVtbl->Present, PresentWrap);
			g_oldPresent = (old2) ? old2 : g_oldPresent;
		}

		if (TextureFixEnable)
		{
			auto old3 = HookVtableFunction(&(*ppReturnedDeviceInterface)->lpVtbl->SetTexture, SetTextureWrap);
			g_oldSetTexture = (old3) ? old3 : g_oldSetTexture;
		}
	}

	return hr;
}

static IDirect3D9* WINAPI Direct3DCreate9Wrap(UINT SDKVersion)
{
	auto d3d9 = g_origDirect3DCreate9(SDKVersion);

	auto old = HookVtableFunction(&d3d9->lpVtbl->CreateDevice, CreateDeviceWrap);
	g_oldCreateDevice = (old) ? old : g_oldCreateDevice;

	return d3d9;
}

static void InitD3D9WindowHook()
{
	MH_Initialize();
	MH_CreateHookApi(L"d3d9.dll", "Direct3DCreate9", Direct3DCreate9Wrap, (void**)&g_origDirect3DCreate9);
	MH_EnableHook(MH_ALL_HOOKS);
}

extern linb::ini config;

static InitFunction initFunc([]()
{
	if (GameDetect::currentGame == GameID::BG4 || GameDetect::currentGame == GameID::BG4_Eng || GameDetect::currentGame == GameID::JLeague || GameDetect::currentGame == GameID::TER || GameDetect::currentGame == GameID::RumbleFish2 || GameDetect::currentGame == GameID::DirtyDrivin || GameDetect::currentGame == GameID::Friction)
		return;

	if (GameDetect::currentGame == GameID::CrazySpeed)
	{
		// This will fix the rearview mirror being fully white
		TextureFixEnable = true;
	}

	// Make local variables for speed
	Windowed = ToBool(config["General"]["Windowed"]);
	MultiScreen = ToBool(config["General"]["Multi Screen"]);

	if (GameDetect::currentGame == GameID::ElevatorActionDeathParade && MultiScreen)
		Windowed = false;

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

	if (Windowed || FpsLimiterEnable || TextureFixEnable || GameDetect::currentGame == GameID::TroubleWitches || GameDetect::currentGame == GameID::ElevatorActionDeathParade && !MultiScreen)
		InitD3D9WindowHook();
});
#pragma optimize("", on)