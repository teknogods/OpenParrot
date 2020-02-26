#define CINTERFACE
#define D3D11_NO_HELPERS
#define INITGUID
#include <StdInc.h>
#ifdef _M_AMD64
#include "Utility/InitFunction.h"
#pragma optimize("", off)
#include "d3d11.h"
#include "dxgi1_6.h"
#include "Utility/GameDetect.h"

HRESULT(WINAPI* g_origCreateDXGIFactory)(REFIID, void**);

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

HRESULT(WINAPI* g_oldCreateSwapChain)(
	IDXGIFactory * This,
	/* [annotation][in] */
	_In_  IUnknown *pDevice,
	/* [annotation][in] */
	_In_  DXGI_SWAP_CHAIN_DESC *pDesc,
	/* [annotation][out] */
	_COM_Outptr_  IDXGISwapChain **ppSwapChain);

HRESULT(STDMETHODCALLTYPE *g_oldSetFullscreenState)(
	IDXGISwapChain * This,
	/* [in] */ BOOL Fullscreen,
	/* [annotation][in] */
	_In_opt_  IDXGIOutput *pTarget);

HRESULT STDMETHODCALLTYPE SetFullscreenStateWrap(
	IDXGISwapChain * This,
	/* [in] */ BOOL Fullscreen,
	/* [annotation][in] */
	_In_opt_  IDXGIOutput *pTarget)
{
	return S_OK;
}

HRESULT WINAPI CreateSwapChainWrap(
	IDXGIFactory * This,
	/* [annotation][in] */
	_In_  IUnknown *pDevice,
	/* [annotation][in] */
	_In_  DXGI_SWAP_CHAIN_DESC *pDesc,
	/* [annotation][out] */
	_COM_Outptr_  IDXGISwapChain **ppSwapChain)
{
	pDesc->Windowed = TRUE;

	HRESULT hr = g_oldCreateSwapChain(This, pDevice, pDesc, ppSwapChain);

	if (*ppSwapChain)
	{
		auto old = HookVtableFunction(&(*ppSwapChain)->lpVtbl->SetFullscreenState, SetFullscreenStateWrap);
		g_oldSetFullscreenState = (old) ? old : g_oldSetFullscreenState;
	}

	return hr;
}

HRESULT(STDMETHODCALLTYPE *g_oldSetFullscreenState1)(
	IDXGISwapChain1 * This,
	/* [in] */ BOOL Fullscreen,
	/* [annotation][in] */
	_In_opt_  IDXGIOutput *pTarget);

HRESULT STDMETHODCALLTYPE SetFullscreenState1Wrap(
	IDXGISwapChain1 * This,
	/* [in] */ BOOL Fullscreen,
	/* [annotation][in] */
	_In_opt_  IDXGIOutput *pTarget)
{
	return S_OK;
}

HRESULT(STDMETHODCALLTYPE *g_oldCreateSwapChainForHwnd)(
	IDXGIFactory2 * This,
	/* [annotation][in] */
	_In_  IUnknown *pDevice,
	/* [annotation][in] */
	_In_  HWND hWnd,
	/* [annotation][in] */
	_In_  const DXGI_SWAP_CHAIN_DESC1 *pDesc,
	/* [annotation][in] */
	_In_opt_  const DXGI_SWAP_CHAIN_FULLSCREEN_DESC *pFullscreenDesc,
	/* [annotation][in] */
	_In_opt_  IDXGIOutput *pRestrictToOutput,
	/* [annotation][out] */
	_COM_Outptr_  IDXGISwapChain1 **ppSwapChain);

HRESULT STDMETHODCALLTYPE CreateSwapChainForHwndWrap(
	IDXGIFactory2 * This,
	/* [annotation][in] */
	_In_  IUnknown *pDevice,
	/* [annotation][in] */
	_In_  HWND hWnd,
	/* [annotation][in] */
	_In_  const DXGI_SWAP_CHAIN_DESC1 *pDesc,
	/* [annotation][in] */
	_In_opt_  const DXGI_SWAP_CHAIN_FULLSCREEN_DESC *pFullscreenDesc,
	/* [annotation][in] */
	_In_opt_  IDXGIOutput *pRestrictToOutput,
	/* [annotation][out] */
	_COM_Outptr_  IDXGISwapChain1 **ppSwapChain)
{
	HRESULT hr = g_oldCreateSwapChainForHwnd(This, pDevice, hWnd, pDesc, NULL, pRestrictToOutput, ppSwapChain);

	if (*ppSwapChain)
	{
		auto old = HookVtableFunction(&(*ppSwapChain)->lpVtbl->SetFullscreenState, SetFullscreenState1Wrap);
		g_oldSetFullscreenState1 = (old) ? old : g_oldSetFullscreenState1;
	}

	return hr;
}

HRESULT WINAPI CreateDXGIFactoryWrap(REFIID riid, _COM_Outptr_ void **ppFactory)
{
	HRESULT hr = g_origCreateDXGIFactory(riid, ppFactory);

	if (SUCCEEDED(hr))
	{
		int factoryType = 0;

		if (IsEqualIID(riid, IID_IDXGIFactory1))
		{
			factoryType = 1;
		}
		else if (IsEqualIID(riid, IID_IDXGIFactory2))
		{
			factoryType = 2;
		}
		else if (IsEqualIID(riid, IID_IDXGIFactory2))
		{
			factoryType = 2;
		}
		else if (IsEqualIID(riid, IID_IDXGIFactory3))
		{
			factoryType = 3;
		}
		else if (IsEqualIID(riid, IID_IDXGIFactory4))
		{
			factoryType = 4;
		}
		else if (IsEqualIID(riid, IID_IDXGIFactory5))
		{
			factoryType = 5;
		}

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

static HRESULT (WINAPI* g_origD3D11CreateDeviceAndSwapChain)(
	_In_opt_        IDXGIAdapter         *pAdapter,
	D3D_DRIVER_TYPE      DriverType,
	HMODULE              Software,
	UINT                 Flags,
	_In_opt_  const D3D_FEATURE_LEVEL    *pFeatureLevels,
	UINT                 FeatureLevels,
	UINT                 SDKVersion,
	_In_opt_  const DXGI_SWAP_CHAIN_DESC *pSwapChainDesc,
	_Out_opt_       IDXGISwapChain       **ppSwapChain,
	_Out_opt_       ID3D11Device         **ppDevice,
	_Out_opt_       D3D_FEATURE_LEVEL    *pFeatureLevel,
	_Out_opt_       ID3D11DeviceContext  **ppImmediateContext
);

static HRESULT WINAPI D3D11CreateDeviceAndSwapChainWrap(
	_In_opt_        IDXGIAdapter         *pAdapter,
	D3D_DRIVER_TYPE      DriverType,
	HMODULE              Software,
	UINT                 Flags,
	_In_opt_  const D3D_FEATURE_LEVEL    *pFeatureLevels,
	UINT                 FeatureLevels,
	UINT                 SDKVersion,
	_In_opt_  /*const*/ DXGI_SWAP_CHAIN_DESC *pSwapChainDesc,
	_Out_opt_       IDXGISwapChain       **ppSwapChain,
	_Out_opt_       ID3D11Device         **ppDevice,
	_Out_opt_       D3D_FEATURE_LEVEL    *pFeatureLevel,
	_Out_opt_       ID3D11DeviceContext  **ppImmediateContext
)
{
	pSwapChainDesc->Windowed = TRUE;

	HRESULT hr = g_origD3D11CreateDeviceAndSwapChain(pAdapter, DriverType, Software, Flags, pFeatureLevels, FeatureLevels, SDKVersion, pSwapChainDesc, ppSwapChain, ppDevice, pFeatureLevel, ppImmediateContext);

	if (ppSwapChain)
	{
		auto old = HookVtableFunction(&(*ppSwapChain)->lpVtbl->SetFullscreenState, SetFullscreenStateWrap);
		g_oldSetFullscreenState = (old) ? old : g_oldSetFullscreenState;
	}

	return hr;
}

void InitDXGIWindowHook()
{
	MH_Initialize();
	MH_CreateHookApi(L"dxgi.dll",
		"CreateDXGIFactory",
		CreateDXGIFactoryWrap,
		(void**)&g_origCreateDXGIFactory);

	MH_CreateHookApi(L"d3d11.dll",
		"D3D11CreateDeviceAndSwapChain",		D3D11CreateDeviceAndSwapChainWrap,
		(void**)&g_origD3D11CreateDeviceAndSwapChain);
	MH_EnableHook(MH_ALL_HOOKS);
}

extern linb::ini config;

static InitFunction initFunc([]()
{
	if (GameDetect::currentGame == GameID::PokkenTournament || GameDetect::currentGame == GameID::SchoolOfRagnarok || GameDetect::currentGame == GameID::TER || GameDetect::currentGame == GameID::Konami)
		return;
	if (ToBool(config["General"]["Windowed"]))
	{
		InitDXGIWindowHook();
	}
});
#endif