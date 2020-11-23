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

static HRESULT(WINAPI* g_oldPresent)(IDirect3DDevice8* self, CONST RECT* pSourceRect, CONST RECT* pDestRect, HWND hDestWindowOverride, CONST RGNDATA* pDirtyRegion);

HRESULT WINAPI PresentWrap(IDirect3DDevice8* self, CONST RECT* pSourceRect, CONST RECT* pDestRect, HWND hDestWindowOverride, CONST RGNDATA* pDirtyRegion)
{
	if (ToBool(config["General"]["Framelimiter"]))
	{
		float fFPSLimit = 60.0;
		static LARGE_INTEGER PerformanceCount1;
		static LARGE_INTEGER PerformanceCount2;
		static bool bOnce1 = false;
		static double targetFrameTime = 1000.0 / fFPSLimit;
		static double t = 0.0;
		static DWORD i = 0;

		if (!bOnce1)
		{
			bOnce1 = true;
			QueryPerformanceCounter(&PerformanceCount1);
			PerformanceCount1.QuadPart = PerformanceCount1.QuadPart >> i;
		}

		while (true)
		{
			QueryPerformanceCounter(&PerformanceCount2);
			if (t == 0.0)
			{
				LARGE_INTEGER PerformanceCount3;
				static bool bOnce2 = false;

				if (!bOnce2)
				{
					bOnce2 = true;
					QueryPerformanceFrequency(&PerformanceCount3);
					i = 0;
					t = 1000.0 / (double)PerformanceCount3.QuadPart;
					auto v = t * 2147483648.0;
					if (60000.0 > v)
					{
						while (true)
						{
							++i;
							v *= 2.0;
							t *= 2.0;
							if (60000.0 <= v)
								break;
						}
					}
				}
				SleepEx(0, 1);
				break;
			}

			if (((double)((PerformanceCount2.QuadPart >> i) - PerformanceCount1.QuadPart) * t) >= targetFrameTime)
				break;

			SleepEx(0, 1);
		}
		QueryPerformanceCounter(&PerformanceCount2);
		PerformanceCount1.QuadPart = PerformanceCount2.QuadPart >> i;
		return g_oldPresent(self, pSourceRect, pDestRect, hDestWindowOverride, pDirtyRegion);
	}
	else
		return g_oldPresent(self, pSourceRect, pDestRect, hDestWindowOverride, pDirtyRegion);
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

		auto old2 = HookVtableFunction(&(*ppReturnedDeviceInterface)->lpVtbl->Present, PresentWrap);
		g_oldPresent = (old2) ? old2 : g_oldPresent;
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
	if (ToBool(config["General"]["Windowed"]) || ToBool(config["General"]["Framelimiter"]))
	{
		InitD3D8WindowHook();
	}
});
#pragma optimize("", on)