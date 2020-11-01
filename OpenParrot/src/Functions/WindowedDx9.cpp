#define CINTERFACE
#include <StdInc.h>
#include "Utility/InitFunction.h"
#pragma optimize("", off)
#include "d3d9.h"
#include "Utility/GameDetect.h"

IDirect3D9*(WINAPI* g_origDirect3DCreate9)(UINT SDKVersion);

template<typename T>
T HookVtableFunction(T* functionPtr, T target)
{
	if (*functionPtr == target)
	{
		return nullptr;
	}

	auto old = *functionPtr;
	injector::WriteMemory(functionPtr, target, true);

	return old;
}

static HRESULT(WINAPI* g_oldReset)(IDirect3DDevice9* self, D3DPRESENT_PARAMETERS* pPresentationParameters);

HRESULT WINAPI ResetWrap(IDirect3DDevice9* self, D3DPRESENT_PARAMETERS* pPresentationParameters)
{
	pPresentationParameters->Windowed = TRUE;
	pPresentationParameters->FullScreen_RefreshRateInHz = 0;

	return g_oldReset(self, pPresentationParameters);
}

static HRESULT(WINAPI* g_oldCreateDevice)(IDirect3D9* self, UINT Adapter, D3DDEVTYPE DeviceType, HWND hFocusWindow, DWORD BehaviorFlags, D3DPRESENT_PARAMETERS* pPresentationParameters, IDirect3DDevice9** ppReturnedDeviceInterface);

HRESULT WINAPI CreateDeviceWrap(IDirect3D9* self, UINT Adapter, D3DDEVTYPE DeviceType, HWND hFocusWindow, DWORD BehaviorFlags, D3DPRESENT_PARAMETERS* pPresentationParameters, IDirect3DDevice9** ppReturnedDeviceInterface)
{
	pPresentationParameters->Windowed = TRUE;
	pPresentationParameters->FullScreen_RefreshRateInHz = 0;

	HRESULT hr = g_oldCreateDevice(self, Adapter, DeviceType, hFocusWindow, BehaviorFlags, pPresentationParameters, ppReturnedDeviceInterface);

	if (*ppReturnedDeviceInterface)
	{
		auto old = HookVtableFunction(&(*ppReturnedDeviceInterface)->lpVtbl->Reset, ResetWrap);
		g_oldReset = (old) ? old : g_oldReset;
	}

	return hr;
}

IDirect3D9* WINAPI Direct3DCreate9Wrap(UINT SDKVersion)
{
	auto d3d9 = g_origDirect3DCreate9(SDKVersion);
	
	auto old = HookVtableFunction(&d3d9->lpVtbl->CreateDevice, CreateDeviceWrap);
	g_oldCreateDevice = (old) ? old : g_oldCreateDevice;

	return d3d9;
}

void InitD3D9WindowHook()
{
	MH_Initialize();
	MH_CreateHookApi(L"d3d9.dll", "Direct3DCreate9", Direct3DCreate9Wrap, (void**)&g_origDirect3DCreate9);
	MH_EnableHook(MH_ALL_HOOKS);
}

extern linb::ini config;

static InitFunction initFunc([]()
{
	if (GameDetect::currentGame == GameID::BG4 || GameDetect::currentGame == GameID::JLeague || GameDetect::currentGame == GameID::TER || GameDetect::currentGame == GameID::DirtyDrivin)
		return;
	if (ToBool(config["General"]["Windowed"]))
	{
		InitD3D9WindowHook();
	}
});
#pragma optimize("", on)