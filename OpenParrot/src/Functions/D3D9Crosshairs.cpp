#include <StdInc.h>
#include "Utility/InitFunction.h"
#include "Functions/Global.h"
#include "Utility/Helper.h"
#include <intrin.h>
#include <shlwapi.h>
#include <iostream>
#include <fstream>
#include <d3d9.h>
#include "DirectXSDK/Include/d3dx9.h"

#if _M_IX86
#pragma comment(lib, "../inc/DirectXSDK/Lib/x86/d3d9.lib")
#pragma comment(lib, "../inc/DirectXSDK/Lib/x86/d3dx9.lib")
#else
#pragma comment(lib, "../inc/DirectXSDK/Lib/x64/d3d9.lib")
#pragma comment(lib, "../inc/DirectXSDK/Lib/x64/d3dx9.lib")
#endif

extern int* ffbOffset;
extern int* ffbOffset2;
extern int* ffbOffset3;
extern int* ffbOffset4;
extern int* ffbOffset5;

float EADPRenderWidth;
float EADPRenderHeight;

DWORD resWidthD3D9;
DWORD resHeightD3D9;

int Player1Active;
int Player2Active;

static bool CrosshairInit;
static bool Windowed;

static std::ifstream P1Size{};
static std::ifstream P2Size{};

static wchar_t P1buf[MAX_PATH];
static wchar_t P2buf[MAX_PATH];

static unsigned int P1Width, P1Height;
static unsigned int P2Width, P2Height;

static LPDIRECT3DTEXTURE9 Texture1P = NULL;
static LPD3DXSPRITE Sprite1P = NULL;
static D3DXVECTOR3 vPos1P;
static D3DXVECTOR3 vCen1P;

static LPDIRECT3DTEXTURE9 Texture2P = NULL;
static LPD3DXSPRITE Sprite2P = NULL;
static D3DXVECTOR3 vPos2P;
static D3DXVECTOR3 vCen2P;

static bool P1SceenOut;
static bool P2SceenOut;

static void EADPCrosshairCalculations()
{
	vPos1P.x = (int)round((float)(*ffbOffset2) / 255.0f * (float)resWidthD3D9); // P1 X axis
	vPos1P.y = (int)round((float)(*ffbOffset3) / 255.0f * (float)resHeightD3D9); // P1 Y axis

	vPos2P.x = (int)round((float)(*ffbOffset4) / 255.0f * (float)resWidthD3D9); // P2 X axis
	vPos2P.y = (int)round((float)(*ffbOffset5) / 255.0f * (float)resHeightD3D9); // P2 Y axis

	float LeftMaxWidth = (int)round((float)resWidthD3D9 / 2.0) - ((float)EADPRenderWidth / 2.0);
	float RightMaxWidth = (int)round((float)resWidthD3D9 / 2.0) + ((float)EADPRenderWidth / 2.0);
	float TopMaxHeight = (int)round((float)resHeightD3D9 / 2.0) - ((float)EADPRenderHeight / 2.0);
	float BottomMaxHeight = (int)round((float)resHeightD3D9 / 2.0) + ((float)EADPRenderHeight / 2.0);

	float P1XAxis = (*ffbOffset2 / 255.0) * resWidthD3D9;
	float P1YAxis = (*ffbOffset3 / 255.0) * resHeightD3D9;

	float P2XAxis = (*ffbOffset4 / 255.0) * resWidthD3D9;
	float P2YAxis = (*ffbOffset5 / 255.0) * resHeightD3D9;

	P1SceenOut = (P1XAxis <= LeftMaxWidth || P1XAxis >= RightMaxWidth || P1YAxis <= TopMaxHeight || P1YAxis >= BottomMaxHeight);
	P2SceenOut = (P2XAxis <= LeftMaxWidth || P2XAxis >= RightMaxWidth || P2YAxis <= TopMaxHeight || P2YAxis >= BottomMaxHeight);

	vCen1P.x = P1Width / 2.0; // P1 X Center
	vCen1P.y = P1Height / 2.0; // P1 Y Center

	vCen2P.x = P2Width / 2.0; // P2 X Center
	vCen2P.y = P2Height / 2.0; // P2 Y Center
} 

typedef HRESULT(APIENTRY* Present) (LPDIRECT3DDEVICE9 Device, CONST RECT* pSrcRect, CONST RECT* pDestRect, HWND hDestWindow, CONST RGNDATA* pDirtyRegion);
static HRESULT APIENTRY Present_hook(LPDIRECT3DDEVICE9 Device, CONST RECT* pSrcRect, CONST RECT* pDestRect, HWND hDestWindow, CONST RGNDATA* pDirtyRegion);
static Present Present_orig = 0;

static HRESULT APIENTRY Present_hook(LPDIRECT3DDEVICE9 Device, CONST RECT* pSrcRect, CONST RECT* pDestRect, HWND hDestWindow, CONST RGNDATA* pDirtyRegion)
{
	if (!CrosshairInit)
	{
		CrosshairInit = true;

		Windowed = ToBool(config["General"]["Windowed"]);

		GetModuleFileName(NULL, P1buf, MAX_PATH);
		PathRemoveFileSpec(P1buf);
		wcscat(P1buf, L"\\P1.png");

		P1Size.open(P1buf);

		P1Size.seekg(16);
		P1Size.read((char*)&P1Width, 4);
		P1Size.read((char*)&P1Height, 4);

		P1Width = ntohl(P1Width);
		P1Height = ntohl(P1Height);

		GetModuleFileName(NULL, P2buf, MAX_PATH);
		PathRemoveFileSpec(P2buf);
		wcscat(P2buf, L"\\P2.png");

		P2Size.open(P2buf);

		P2Size.seekg(16);
		P2Size.read((char*)&P2Width, 4);
		P2Size.read((char*)&P2Height, 4);

		P2Width = ntohl(P2Width);
		P2Height = ntohl(P2Height);

		D3DXCreateTextureFromFileEx(Device, P1buf, D3DX_DEFAULT_NONPOW2, D3DX_DEFAULT_NONPOW2, D3DX_DEFAULT, 0, D3DFMT_UNKNOWN, D3DPOOL_MANAGED, D3DX_DEFAULT, D3DX_DEFAULT, 0, NULL, NULL, &Texture1P);
		D3DXCreateSprite(Device, &Sprite1P);

		D3DXCreateTextureFromFileEx(Device, P2buf, D3DX_DEFAULT_NONPOW2, D3DX_DEFAULT_NONPOW2, D3DX_DEFAULT, 0, D3DFMT_UNKNOWN, D3DPOOL_MANAGED, D3DX_DEFAULT, D3DX_DEFAULT, 0, NULL, NULL, &Texture2P);
		D3DXCreateSprite(Device, &Sprite2P);
	}

	EADPCrosshairCalculations();

	Sprite1P->Begin(0);
	if (Player1Active && !P1SceenOut)
		Sprite1P->Draw(Texture1P, NULL, &vCen1P, &vPos1P, 0xFFFFFFFF);
	Sprite1P->End();

	Sprite2P->Begin(0);
	if (Player2Active && !P2SceenOut)
		Sprite2P->Draw(Texture2P, NULL, &vCen2P, &vPos2P, 0xFFFFFFFF);
	Sprite2P->End();

	return Present_orig(Device, pSrcRect, pDestRect, hDestWindow, pDirtyRegion);
}

DWORD WINAPI D3D9Crosshairs(__in  LPVOID lpParameter)
{
	while (GetModuleHandleA("d3d9.dll") == 0 || !FindWindowA("Eva", "OpenParrot - Elevator Action: Death Parade"))
	{
		Sleep(100);
	}

	HWND Game = FindWindowA("Eva", "OpenParrot - Elevator Action: Death Parade");

	IDirect3D9* d3d = NULL;
	IDirect3DDevice9* d3ddev = NULL;

	HWND tmpWnd = CreateWindowA("BUTTON", "Temp Window", WS_SYSMENU | WS_MINIMIZEBOX, CW_USEDEFAULT, CW_USEDEFAULT, 300, 300, NULL, NULL, NULL, NULL);
	if (tmpWnd == NULL)
	{
		return 0;
	}

	d3d = Direct3DCreate9(D3D_SDK_VERSION);
	if (d3d == NULL)
	{
		DestroyWindow(tmpWnd);
		return 0;
	}

	D3DPRESENT_PARAMETERS d3dpp;
	ZeroMemory(&d3dpp, sizeof(d3dpp));
	d3dpp.Windowed = TRUE;
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	d3dpp.hDeviceWindow = tmpWnd;
	d3dpp.BackBufferFormat = D3DFMT_UNKNOWN;

	HRESULT result = d3d->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, tmpWnd, D3DCREATE_SOFTWARE_VERTEXPROCESSING, &d3dpp, &d3ddev);
	if (result != D3D_OK)
	{
		d3d->Release();
		DestroyWindow(tmpWnd);
		return 0;
	}

#if defined _M_X64
	DWORD64* dVtable = (DWORD64*)d3ddev;
	dVtable = (DWORD64*)dVtable[0];
#elif defined _M_IX86
	DWORD* dVtable = (DWORD*)d3ddev;
	dVtable = (DWORD*)dVtable[0];
#endif

	Present_orig = (Present)dVtable[17];

	MH_Initialize();
	MH_CreateHook((DWORD_PTR*)dVtable[17], &Present_hook, reinterpret_cast<void**>(&Present_orig));
	MH_EnableHook((DWORD_PTR*)dVtable[17]);

	d3ddev->Release();
	d3d->Release();
	DestroyWindow(tmpWnd);
	return 1;
}