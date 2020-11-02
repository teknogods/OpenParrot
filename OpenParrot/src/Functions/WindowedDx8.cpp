#define CINTERFACE
#include <StdInc.h>
#include "Utility/InitFunction.h"
#pragma optimize("", off)
#include "dx8/d3d8.h"
#include "Utility/GameDetect.h"

static bool swShaderHack = false;
//static bool windowed = false;

IDirect3D8*(WINAPI* g_origDirect3DCreate8)(UINT SDKVersion);

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

static HRESULT(WINAPI* g_oldReset)(IDirect3DDevice8* self, D3DPRESENT_PARAMETERS* pPresentationParameters);

HRESULT WINAPI ResetWrap(IDirect3DDevice8* self, D3DPRESENT_PARAMETERS* pPresentationParameters)
{
	if (ToBool(config["General"]["Windowed"]))
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

static HRESULT(WINAPI* g_oldCreateDevice)(IDirect3D8* self, UINT Adapter, D3DDEVTYPE DeviceType, HWND hFocusWindow, DWORD BehaviorFlags, D3DPRESENT_PARAMETERS* pPresentationParameters, IDirect3DDevice8** ppReturnedDeviceInterface);

HRESULT WINAPI CreateDeviceWrap(IDirect3D8* self, UINT Adapter, D3DDEVTYPE DeviceType, HWND hFocusWindow, DWORD BehaviorFlags, D3DPRESENT_PARAMETERS* pPresentationParameters, IDirect3DDevice8** ppReturnedDeviceInterface)
{
	if (ToBool(config["General"]["Windowed"]))
	{
		pPresentationParameters->Windowed = TRUE;
		pPresentationParameters->FullScreen_RefreshRateInHz = 0;
		pPresentationParameters->BackBufferWidth = 0;
		pPresentationParameters->BackBufferHeight = 0;
		pPresentationParameters->BackBufferFormat = D3DFMT_A8R8G8B8; // TODO: query current desktop format and use it
		pPresentationParameters->Flags = 0;
		pPresentationParameters->FullScreen_PresentationInterval = D3DPRESENT_INTERVAL_DEFAULT;
	}
	if (swShaderHack)
		BehaviorFlags = D3DCREATE_SOFTWARE_VERTEXPROCESSING;
	HRESULT hr = g_oldCreateDevice(self, Adapter, DeviceType, hFocusWindow, BehaviorFlags, pPresentationParameters, ppReturnedDeviceInterface);

	if (*ppReturnedDeviceInterface)
	{
		auto old = HookVtableFunction(&(*ppReturnedDeviceInterface)->lpVtbl->Reset, ResetWrap);
		g_oldReset = (old) ? old : g_oldReset;
	}

	return hr;
}

IDirect3D8* WINAPI Direct3DCreate8Wrap(UINT SDKVersion)
{
	auto d3d8 = g_origDirect3DCreate8(SDKVersion);
	
	auto old = HookVtableFunction(&d3d8->lpVtbl->CreateDevice, CreateDeviceWrap);
	g_oldCreateDevice = (old) ? old : g_oldCreateDevice;

	return d3d8;
}

void InitD3D8WindowHook()
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
	if (GameDetect::currentGame == GameID::FNFSC)
		InitD3D8WindowHook();
	if (GameDetect::currentGame == GameID::SnoCross)
	{
		swShaderHack = ToBool(config["General"]["SoftwareVertexShaders"]);
		if (swShaderHack)
			InitD3D8WindowHook();
	}
	if (ToBool(config["General"]["Windowed"]))
	{
		InitD3D8WindowHook();
	}
});
#pragma optimize("", on)