#define CINTERFACE
#define D3D11_NO_HELPERS
#define INITGUID
#include <StdInc.h>
#include "Utility/InitFunction.h"
#pragma optimize("", off)

#include "dxgi1_2.h"
#include "dxgi1_3.h"
#include "dxgi1_4.h"
#include "dxgi1_5.h"
#include "dxgi1_6.h"
#include "d3d11.h"
#pragma comment(lib, "d3d11.lib")
#include "d3d12.h"
#pragma comment(lib, "d3d12.lib")

#include "Utility/GameDetect.h"
#include "Global.h"
#include "FpsLimiter.h"
#include <intrin.h>

// Local variables
static bool Windowed = false;
static bool FpsLimiterEnable = false;
static bool DisableVSync = false;

// Prototypes
static HRESULT(STDMETHODCALLTYPE* g_oldSetFullscreenState)(IDXGISwapChain* This, BOOL Fullscreen, IDXGIOutput* pTarget);
static HRESULT(STDMETHODCALLTYPE* g_oldCreateSwapChain)(IDXGIFactory* This, IUnknown* pDevice, DXGI_SWAP_CHAIN_DESC* pDesc, IDXGISwapChain** ppSwapChain);
static HRESULT(STDMETHODCALLTYPE* g_oldSetFullscreenState1)(IDXGISwapChain1* This, BOOL Fullscreen, IDXGIOutput* pTarget);
static HRESULT(STDMETHODCALLTYPE* g_oldCreateSwapChainForHwnd)(IDXGIFactory2* This, IUnknown* pDevice, HWND hWnd, const DXGI_SWAP_CHAIN_DESC1* pDesc, const DXGI_SWAP_CHAIN_FULLSCREEN_DESC* pFullscreenDesc, IDXGIOutput* pRestrictToOutput, IDXGISwapChain1** ppSwapChain);
static HRESULT(STDMETHODCALLTYPE* g_oldPresentWrap)(IDXGISwapChain* pSwapChain, UINT SyncInterval, UINT Flags);
static HRESULT(STDMETHODCALLTYPE* g_oldPresent1Wrap)(IDXGISwapChain1* pSwapChain, UINT SyncInterval, UINT Flags);
static HRESULT(STDMETHODCALLTYPE* g_oldCreateSwapChain2)(IDXGIFactory2* This, IUnknown* pDevice, DXGI_SWAP_CHAIN_DESC* pDesc, IDXGISwapChain** ppSwapChain);
static HRESULT(WINAPI* g_origCreateDXGIFactory2)(UINT Flags, REFIID riid, void** ppFactory);
static HRESULT(WINAPI* g_origCreateDXGIFactory)(REFIID, void**);
static HRESULT(WINAPI* g_origD3D11CreateDeviceAndSwapChain)(IDXGIAdapter* pAdapter, D3D_DRIVER_TYPE DriverType, HMODULE Software, UINT Flags, const D3D_FEATURE_LEVEL* pFeatureLevels, UINT FeatureLevels, UINT SDKVersion, /*const*/ DXGI_SWAP_CHAIN_DESC* pSwapChainDesc, IDXGISwapChain** ppSwapChain, ID3D11Device** ppDevice, D3D_FEATURE_LEVEL* pFeatureLevel, ID3D11DeviceContext** ppImmediateContext);

static HRESULT STDMETHODCALLTYPE SetFullscreenStateWrap(IDXGISwapChain* This, BOOL Fullscreen, IDXGIOutput* pTarget);
static HRESULT STDMETHODCALLTYPE CreateSwapChainWrap(IDXGIFactory* This, IUnknown* pDevice, DXGI_SWAP_CHAIN_DESC* pDesc, IDXGISwapChain** ppSwapChain);
static HRESULT STDMETHODCALLTYPE SetFullscreenState1Wrap(IDXGISwapChain1* This, BOOL Fullscreen, IDXGIOutput* pTarget);
static HRESULT STDMETHODCALLTYPE CreateSwapChainForHwndWrap(IDXGIFactory2* This, IUnknown* pDevice, HWND hWnd, const DXGI_SWAP_CHAIN_DESC1* pDesc, const DXGI_SWAP_CHAIN_FULLSCREEN_DESC* pFullscreenDesc, IDXGIOutput* pRestrictToOutput, IDXGISwapChain1** ppSwapChain);
static HRESULT STDMETHODCALLTYPE PresentWrap(IDXGISwapChain* pSwapChain, UINT SyncInterval, UINT Flags);
static HRESULT STDMETHODCALLTYPE Present1Wrap(IDXGISwapChain1* pSwapChain, UINT SyncInterval, UINT Flags);
static HRESULT STDMETHODCALLTYPE CreateSwapChain2Wrap(IDXGIFactory2* This, IUnknown* pDevice, DXGI_SWAP_CHAIN_DESC* pDesc, IDXGISwapChain** ppSwapChain);
static HRESULT WINAPI CreateDXGIFactory2Wrap(UINT Flags, REFIID riid, void** ppFactory);
static HRESULT WINAPI CreateDXGIFactoryWrap(REFIID riid, _COM_Outptr_ void** ppFactory);
static HRESULT WINAPI D3D11CreateDeviceAndSwapChainWrap(IDXGIAdapter* pAdapter, D3D_DRIVER_TYPE DriverType, HMODULE Software, UINT Flags, const D3D_FEATURE_LEVEL* pFeatureLevels, UINT FeatureLevels, UINT SDKVersion, /*const*/ DXGI_SWAP_CHAIN_DESC* pSwapChainDesc, IDXGISwapChain** ppSwapChain, ID3D11Device** ppDevice, D3D_FEATURE_LEVEL* pFeatureLevel, ID3D11DeviceContext** ppImmediateContext);

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

static HRESULT STDMETHODCALLTYPE SetFullscreenStateWrap(IDXGISwapChain* This, BOOL Fullscreen, IDXGIOutput* pTarget)
{
	return S_OK;
}

static HRESULT STDMETHODCALLTYPE CreateSwapChainWrap(IDXGIFactory* This, IUnknown* pDevice, DXGI_SWAP_CHAIN_DESC* pDesc, IDXGISwapChain** ppSwapChain)
{
	if (Windowed)
		pDesc->Windowed = TRUE;

	HRESULT hr = g_oldCreateSwapChain(This, pDevice, pDesc, ppSwapChain);

	if (*ppSwapChain)
	{
		if (Windowed)
		{
			auto old1 = HookVtableFunction(&(*ppSwapChain)->lpVtbl->SetFullscreenState, SetFullscreenStateWrap);
			g_oldSetFullscreenState = (old1) ? old1 : g_oldSetFullscreenState;
		}

		if (FpsLimiterEnable || DisableVSync)
		{
			auto old2 = HookVtableFunction(&(*ppSwapChain)->lpVtbl->Present, PresentWrap);
			g_oldPresentWrap = (old2) ? old2 : g_oldPresentWrap;
		}
	}

	return hr;
}

static HRESULT STDMETHODCALLTYPE SetFullscreenState1Wrap(IDXGISwapChain1* This, BOOL Fullscreen, IDXGIOutput* pTarget)
{
	return S_OK;
}

static HRESULT STDMETHODCALLTYPE CreateSwapChainForHwndWrap(IDXGIFactory2* This, IUnknown* pDevice, HWND hWnd, const DXGI_SWAP_CHAIN_DESC1* pDesc, const DXGI_SWAP_CHAIN_FULLSCREEN_DESC* pFullscreenDesc, IDXGIOutput* pRestrictToOutput, IDXGISwapChain1** ppSwapChain)
{
	HRESULT hr = g_oldCreateSwapChainForHwnd(This, pDevice, hWnd, pDesc, NULL, pRestrictToOutput, ppSwapChain);

	if (*ppSwapChain)
	{
		if (Windowed)
		{
			auto old = HookVtableFunction(&(*ppSwapChain)->lpVtbl->SetFullscreenState, SetFullscreenState1Wrap);
			g_oldSetFullscreenState1 = (old) ? old : g_oldSetFullscreenState1;
		}

		if (FpsLimiterEnable || DisableVSync)
		{
			auto old2 = HookVtableFunction(&(*ppSwapChain)->lpVtbl->Present, Present1Wrap);
			g_oldPresent1Wrap = (old2) ? old2 : g_oldPresent1Wrap;
		}
	}

	return hr;
}

static HRESULT STDMETHODCALLTYPE PresentWrap(IDXGISwapChain* pSwapChain, UINT SyncInterval, UINT Flags)
{
	if (DisableVSync)
		SyncInterval = 0;

	if (FpsLimiterEnable)
		FpsLimiter();

	return g_oldPresentWrap(pSwapChain, SyncInterval, Flags);
}

static HRESULT STDMETHODCALLTYPE Present1Wrap(IDXGISwapChain1* pSwapChain, UINT SyncInterval, UINT Flags)
{
	if (DisableVSync)
		SyncInterval = 0;

	if (FpsLimiterEnable)
		FpsLimiter();

	return g_oldPresent1Wrap(pSwapChain, SyncInterval, Flags);
}

static HRESULT STDMETHODCALLTYPE CreateSwapChain2Wrap(IDXGIFactory2* This, IUnknown* pDevice, DXGI_SWAP_CHAIN_DESC* pDesc, IDXGISwapChain** ppSwapChain)
{
	if (Windowed)
		pDesc->Windowed = TRUE;

	HRESULT hr = g_oldCreateSwapChain2(This, pDevice, pDesc, ppSwapChain);

	if (*ppSwapChain)
	{
		if (Windowed)
		{
			auto old1 = HookVtableFunction(&(*ppSwapChain)->lpVtbl->SetFullscreenState, SetFullscreenStateWrap);
			g_oldSetFullscreenState = (old1) ? old1 : g_oldSetFullscreenState;
		}

		if (FpsLimiterEnable || DisableVSync)
		{
			auto old2 = HookVtableFunction(&(*ppSwapChain)->lpVtbl->Present, PresentWrap);
			g_oldPresentWrap = (old2) ? old2 : g_oldPresentWrap;
		}
	}

	return hr;
}

static HRESULT WINAPI CreateDXGIFactory2Wrap(UINT Flags, REFIID riid, void** ppFactory)
{
	HRESULT hr = g_origCreateDXGIFactory2(Flags, riid, ppFactory);

	if (SUCCEEDED(hr))
	{
		int factoryType = 0;

		if (IsEqualIID(riid, IID_IDXGIFactory1))
			factoryType = 1;
		else if (IsEqualIID(riid, IID_IDXGIFactory2))
			factoryType = 2;
		else if (IsEqualIID(riid, IID_IDXGIFactory3))
			factoryType = 3;
		else if (IsEqualIID(riid, IID_IDXGIFactory4))
			factoryType = 4;
		else if (IsEqualIID(riid, IID_IDXGIFactory5))
			factoryType = 5;
		else if (IsEqualIID(riid, IID_IDXGIFactory6))
			factoryType = 6;
		else if (IsEqualIID(riid, IID_IDXGIFactory7))
			factoryType = 7;

		IDXGIFactory2* factory = (IDXGIFactory2*)*ppFactory;

		auto old = HookVtableFunction(&factory->lpVtbl->CreateSwapChain, CreateSwapChain2Wrap);
		g_oldCreateSwapChain2 = (old) ? old : g_oldCreateSwapChain2;
	}

	return hr;
}

static HRESULT WINAPI CreateDXGIFactoryWrap(REFIID riid, _COM_Outptr_ void** ppFactory)
{
	HRESULT hr = g_origCreateDXGIFactory(riid, ppFactory);

	if (SUCCEEDED(hr))
	{
		int factoryType = 0;

		if (IsEqualIID(riid, IID_IDXGIFactory1))
			factoryType = 1;
		else if (IsEqualIID(riid, IID_IDXGIFactory2))
			factoryType = 2;
		else if (IsEqualIID(riid, IID_IDXGIFactory3))
			factoryType = 3;
		else if (IsEqualIID(riid, IID_IDXGIFactory4))
			factoryType = 4;
		else if (IsEqualIID(riid, IID_IDXGIFactory5))
			factoryType = 5;
		else if (IsEqualIID(riid, IID_IDXGIFactory6))
			factoryType = 6;
		else if (IsEqualIID(riid, IID_IDXGIFactory7))
			factoryType = 7;

		if (factoryType >= 0)
		{
			IDXGIFactory* factory = (IDXGIFactory*)*ppFactory;

			auto old = HookVtableFunction(&factory->lpVtbl->CreateSwapChain, CreateSwapChainWrap);
			g_oldCreateSwapChain = (old) ? old : g_oldCreateSwapChain;
		}

		if (factoryType >= 2)
		{
			IDXGIFactory2* factory = (IDXGIFactory2*)*ppFactory;

			auto old = HookVtableFunction(&factory->lpVtbl->CreateSwapChainForHwnd, CreateSwapChainForHwndWrap);
			g_oldCreateSwapChainForHwnd = (old) ? old : g_oldCreateSwapChainForHwnd;
		}
	}

	return hr;
}

static HRESULT WINAPI D3D11CreateDeviceAndSwapChainWrap(IDXGIAdapter* pAdapter, D3D_DRIVER_TYPE DriverType, HMODULE Software, UINT Flags, const D3D_FEATURE_LEVEL* pFeatureLevels, UINT FeatureLevels, UINT SDKVersion, /*const*/ DXGI_SWAP_CHAIN_DESC* pSwapChainDesc, IDXGISwapChain** ppSwapChain, ID3D11Device** ppDevice, D3D_FEATURE_LEVEL* pFeatureLevel, ID3D11DeviceContext** ppImmediateContext)
{
	if (Windowed)
		pSwapChainDesc->Windowed = TRUE;

	HRESULT hr = g_origD3D11CreateDeviceAndSwapChain(pAdapter, DriverType, Software, Flags, pFeatureLevels, FeatureLevels, SDKVersion, pSwapChainDesc, ppSwapChain, ppDevice, pFeatureLevel, ppImmediateContext);

	if (ppSwapChain)
	{
		if (Windowed)
		{
			auto old1 = HookVtableFunction(&(*ppSwapChain)->lpVtbl->SetFullscreenState, SetFullscreenStateWrap);
			g_oldSetFullscreenState = (old1) ? old1 : g_oldSetFullscreenState;
		}

		if (FpsLimiterEnable)
		{
			auto old2 = HookVtableFunction(&(*ppSwapChain)->lpVtbl->Present, PresentWrap);
			g_oldPresentWrap = (old2) ? old2 : g_oldPresentWrap;
		}
	}

	return hr;
}

static void InitDXGIWindowHook()
{
	MH_Initialize();
	MH_CreateHookApi(L"dxgi.dll", "CreateDXGIFactory", CreateDXGIFactoryWrap, (void**)&g_origCreateDXGIFactory);
	MH_CreateHookApi(L"dxgi.dll", "CreateDXGIFactory2", CreateDXGIFactory2Wrap, (void**)&g_origCreateDXGIFactory2);
	MH_CreateHookApi(L"d3d11.dll", "D3D11CreateDeviceAndSwapChain", D3D11CreateDeviceAndSwapChainWrap, (void**)&g_origD3D11CreateDeviceAndSwapChain);
	MH_EnableHook(MH_ALL_HOOKS);
}

extern linb::ini config;

static InitFunction initFunc([]()
{
	if (GameDetect::currentGame == GameID::PokkenTournament || GameDetect::currentGame == GameID::PokkenTournament26 || GameDetect::currentGame == GameID::SchoolOfRagnarok || GameDetect::currentGame == GameID::TER || GameDetect::currentGame == GameID::Tekken7Update00 || GameDetect::currentGame == GameID::SFV)
		return;

#ifndef _M_AMD64
	// Hook only Daytona on x86 for now
	if (GameDetect::currentGame != GameID::Daytona3)
		return;
#endif
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

	DisableVSync = ToBool(config["General"]["DisableVSync"]);

	if (GameDetect::currentGame == GameID::Daytona3)
	{
		if (ToBool(config["General"]["Borderless Fullscreen"]))
		{
			Windowed = true;
			FpsLimiterEnable = true;
			FpsLimiterSet(60.0f);
		}
	}

	if (Windowed || FpsLimiterEnable || DisableVSync)
		InitDXGIWindowHook();
});