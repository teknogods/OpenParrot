#include <StdInc.h>
#include "Utility/InitFunction.h"
#include "Functions/Global.h"
#include <intrin.h>
#include <shlwapi.h>
#include <iostream>
#include <fstream>
#include "Utility/GameDetect.h"
#include "Utility/Helper.h"
#include <d3d9.h>
#include "DirectXSDK/Include/d3dx9.h"
#include "FpsLimiter.h"

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

static char RenderHookChar[256];
static bool PresentHook;

float EADPRenderWidth;
float EADPRenderHeight;

DWORD resWidthD3D9;
DWORD resHeightD3D9;

int Player1Active;
int Player2Active;

static bool D3D9Init;
static bool Windowed;
static bool Landscape;

bool EnableD3D9Bezel;
bool EnableD3D9Border;
bool EnableD3D9Crosshairs;
bool EnableD3D9Doors;

static bool ScaleD3D9Bezel;
static bool ScaleD3D9Border;

float ScaleBezelX = 1.0f;
float ScaleBezelY = 1.0f;

int BorderThickness;
int BezelPixelWidth;
int BezelPixelHeight;

extern float DoorFloatLeft;
extern float DoorFloatRight;

static std::ifstream P1Size{};
static std::ifstream P2Size{};
static std::ifstream BezelSize{};
static std::ifstream BorderSize{};
static std::ifstream DoorLeftSize{};
static std::ifstream DoorRightSize{};
static std::ifstream DoorBezelSize{};

static wchar_t P1buf[MAX_PATH];
static wchar_t P2buf[MAX_PATH];
static wchar_t Bezelbuf[MAX_PATH];
static wchar_t Borderbuf[MAX_PATH];
static wchar_t DoorLeftbuf[MAX_PATH];
static wchar_t DoorRightbuf[MAX_PATH];
static wchar_t DoorBezelbuf[MAX_PATH];

static unsigned int P1Width, P1Height;
static unsigned int P2Width, P2Height;
static unsigned int BezelWidth, BezelHeight;
static unsigned int BorderWidth, BorderHeight;
static unsigned int DoorLeftWidth, DoorLeftHeight;
static unsigned int DoorRightWidth, DoorRightHeight;
static unsigned int DoorBezelWidth, DoorBezelHeight;

static D3DXMATRIX bezel_scaling_matrix;
static D3DXMATRIX border_scaling_matrix;

static LPDIRECT3DTEXTURE9 Texture1P = NULL;
static LPD3DXSPRITE Sprite1P = NULL;
static D3DXVECTOR3 vPos1P;
static D3DXVECTOR3 vCen1P;

static LPDIRECT3DTEXTURE9 Texture2P = NULL;
static LPD3DXSPRITE Sprite2P = NULL;
static D3DXVECTOR3 vPos2P;
static D3DXVECTOR3 vCen2P;

static LPDIRECT3DTEXTURE9 TextureBezel = NULL;
static LPD3DXSPRITE SpriteBezel = NULL;
static D3DXVECTOR3 vPosBezel;
static D3DXVECTOR3 vCenBezel;

static LPDIRECT3DTEXTURE9 TextureDoorBezel = NULL;
static LPD3DXSPRITE SpriteDoorBezel = NULL;
static D3DXVECTOR3 vPosDoorBezel;
static D3DXVECTOR3 vCenDoorBezel;

static LPDIRECT3DTEXTURE9 TextureBorder = NULL;
static LPD3DXSPRITE SpriteBorder = NULL;
static D3DXVECTOR3 vPosBorder;
static D3DXVECTOR3 vCenBorder;

static LPDIRECT3DTEXTURE9 TextureDoorLeft = NULL;
static LPD3DXSPRITE SpriteDoorLeft = NULL;
static D3DXVECTOR3 vPosDoorLeft;
static D3DXVECTOR3 vCenDoorLeft;

static LPDIRECT3DTEXTURE9 TextureDoorRight = NULL;
static LPD3DXSPRITE SpriteDoorRight = NULL;
static D3DXVECTOR3 vPosDoorRight;
static D3DXVECTOR3 vCenDoorRight;

static IDirect3DSwapChain9* SwapChainMain;
static IDirect3DSurface9* SurfaceMain;

static bool P1SceenOut;
static bool P2SceenOut;

static bool DoorInit;

static float LeftDoorMin;
static float LeftDoorMax;
static float LeftValueTotal;

static float RightDoorMin;
static float RightDoorMax;
static float RightValueTotal;

static float DoorSpeed = 8.0;

static void EADPAttractionDoorsCalculations()
{
	if (!DoorInit)
	{
		DoorInit = true;

		vPosDoorLeft.x = (int)round((float)resWidthD3D9 / 2.0) - (DoorLeftWidth * 2.0) + DoorLeftWidth / 2.0;

		LeftDoorMin = (int)round((float)resWidthD3D9 / 2.0) - (DoorLeftWidth * 2.0) + DoorLeftWidth / 2.0;
		LeftDoorMax = (int)round((float)resWidthD3D9 / 2.0) - DoorLeftWidth / 2.0;

		LeftValueTotal = LeftDoorMax - LeftDoorMin;

		vPosDoorRight.x = (int)round((float)resWidthD3D9 / 2.0) + (DoorRightWidth * 2.0) - DoorRightWidth / 2.0;

		RightDoorMin = (int)round((float)resWidthD3D9 / 2.0) + (DoorRightWidth * 2.0) - DoorRightWidth / 2.0;
		RightDoorMax = (int)round((float)resWidthD3D9 / 2.0) + DoorRightWidth / 2.0;

		RightValueTotal = RightDoorMax - RightDoorMin;
	}

	if (Landscape)
		DoorSpeed = 8.0;
	else
	{
		if (EADPRenderWidth)
			DoorSpeed = ((EADPRenderWidth / 2.0) / 405.0) * 8.0;
	}

	float DoorLeft = DoorFloatLeft * LeftValueTotal + LeftDoorMin;
	vPosDoorLeft.x = fmin(fmax(vPosDoorLeft.x, LeftDoorMin), LeftDoorMax);

	float DoorRight = DoorFloatRight * RightValueTotal + RightDoorMin;
	vPosDoorRight.x = fmin(fmax(vPosDoorRight.x, RightDoorMax), RightDoorMin);

	if (vPosDoorLeft.x > DoorLeft + 4.0)
		vPosDoorLeft.x -= DoorSpeed;
	else if (vPosDoorLeft.x < DoorLeft - 4.0)
		vPosDoorLeft.x += DoorSpeed;

	if (vPosDoorRight.x > DoorRight + 4.0)
		vPosDoorRight.x -= DoorSpeed;
	else if (vPosDoorRight.x < DoorRight - 4.0)
		vPosDoorRight.x += DoorSpeed;

	if (vPosDoorLeft.x > LeftDoorMax)
		vPosDoorLeft.x = LeftDoorMax;

	if (vPosDoorLeft.x < LeftDoorMin)
		vPosDoorLeft.x = LeftDoorMin;

	if (vPosDoorRight.x < RightDoorMax)
		vPosDoorRight.x = RightDoorMax;

	if (vPosDoorRight.x > RightDoorMin)
		vPosDoorRight.x = RightDoorMin;
}

static void EADPCrosshairCalculations()
{
	vPos1P.x = (int)round((float)(*ffbOffset2) / 255.0f * (float)resWidthD3D9); // P1 X axis
	vPos1P.y = (int)round((float)(*ffbOffset3) / 255.0f * (float)resHeightD3D9); // P1 Y axis

	vPos2P.x = (int)round((float)(*ffbOffset4) / 255.0f * (float)resWidthD3D9); // P2 X axis
	vPos2P.y = (int)round((float)(*ffbOffset5) / 255.0f * (float)resHeightD3D9); // P2 Y axis

	float LeftMaxWidth = (int)round((float)resWidthD3D9 / 2.0) - (EADPRenderWidth / 2.0);
	float RightMaxWidth = (int)round((float)resWidthD3D9 / 2.0) + (EADPRenderWidth / 2.0);
	float TopMaxHeight = (int)round((float)resHeightD3D9 / 2.0) - (EADPRenderHeight / 2.0);
	float BottomMaxHeight = (int)round((float)resHeightD3D9 / 2.0) + (EADPRenderHeight / 2.0);

	float P1XAxis = (*ffbOffset2 / 255.0) * resWidthD3D9;
	float P1YAxis = (*ffbOffset3 / 255.0) * resHeightD3D9;

	float P2XAxis = (*ffbOffset4 / 255.0) * resWidthD3D9;
	float P2YAxis = (*ffbOffset5 / 255.0) * resHeightD3D9;

	P1SceenOut = (P1XAxis <= LeftMaxWidth || P1XAxis >= RightMaxWidth || P1YAxis <= TopMaxHeight || P1YAxis >= BottomMaxHeight);
	P2SceenOut = (P2XAxis <= LeftMaxWidth || P2XAxis >= RightMaxWidth || P2YAxis <= TopMaxHeight || P2YAxis >= BottomMaxHeight);
}

static void RenderOurItems(LPDIRECT3DDEVICE9 Device)
{
	if (!D3D9Init)
	{
		D3D9Init = true;

		Windowed = ToBool(config["General"]["Windowed"]);

		if (EnableD3D9Crosshairs)
		{
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

			vCen1P.x = P1Width / 2.0; // P1 X Center
			vCen1P.y = P1Height / 2.0; // P1 Y Center

			vCen2P.x = P2Width / 2.0; // P2 X Center
			vCen2P.y = P2Height / 2.0; // P2 Y Center

			D3DXCreateTextureFromFileEx(Device, P1buf, D3DX_DEFAULT_NONPOW2, D3DX_DEFAULT_NONPOW2, D3DX_DEFAULT, 0, D3DFMT_UNKNOWN, D3DPOOL_MANAGED, D3DX_DEFAULT, D3DX_DEFAULT, 0, NULL, NULL, &Texture1P);
			D3DXCreateSprite(Device, &Sprite1P);

			D3DXCreateTextureFromFileEx(Device, P2buf, D3DX_DEFAULT_NONPOW2, D3DX_DEFAULT_NONPOW2, D3DX_DEFAULT, 0, D3DFMT_UNKNOWN, D3DPOOL_MANAGED, D3DX_DEFAULT, D3DX_DEFAULT, 0, NULL, NULL, &Texture2P);
			D3DXCreateSprite(Device, &Sprite2P);
		}

		if (EnableD3D9Bezel)
		{
			GetModuleFileName(NULL, Bezelbuf, MAX_PATH);
			PathRemoveFileSpec(Bezelbuf);
			wcscat(Bezelbuf, L"\\Bezel.png");

			BezelSize.open(Bezelbuf);

			BezelSize.seekg(16);
			BezelSize.read((char*)&BezelWidth, 4);
			BezelSize.read((char*)&BezelHeight, 4);

			BezelWidth = ntohl(BezelWidth);
			BezelHeight = ntohl(BezelHeight);

			vCenBezel.x = BezelWidth / 2.0; // Bezel X Center
			vCenBezel.y = BezelHeight / 2.0; // Bezel Y Center

			D3DXCreateTextureFromFileEx(Device, Bezelbuf, D3DX_DEFAULT_NONPOW2, D3DX_DEFAULT_NONPOW2, D3DX_DEFAULT, 0, D3DFMT_UNKNOWN, D3DPOOL_MANAGED, D3DX_DEFAULT, D3DX_DEFAULT, 0, NULL, NULL, &TextureBezel);
			D3DXCreateSprite(Device, &SpriteBezel);
		}

		if (EnableD3D9Border)
		{
			GetModuleFileName(NULL, Borderbuf, MAX_PATH);
			PathRemoveFileSpec(Borderbuf);
			wcscat(Borderbuf, L"\\Border.png");

			BorderSize.open(Borderbuf);

			BorderSize.seekg(16);
			BorderSize.read((char*)&BorderWidth, 4);
			BorderSize.read((char*)&BorderHeight, 4);

			BorderWidth = ntohl(BorderWidth);
			BorderHeight = ntohl(BorderHeight);

			vCenBorder.x = BorderWidth / 2.0; // Border X Center
			vCenBorder.y = BorderHeight / 2.0; // Border Y Center

			D3DXCreateTextureFromFileEx(Device, Borderbuf, D3DX_DEFAULT_NONPOW2, D3DX_DEFAULT_NONPOW2, D3DX_DEFAULT, 0, D3DFMT_UNKNOWN, D3DPOOL_MANAGED, D3DX_DEFAULT, D3DX_DEFAULT, 0, NULL, NULL, &TextureBorder);
			D3DXCreateSprite(Device, &SpriteBorder);
		}

		if (EnableD3D9Doors)
		{
			GetModuleFileName(NULL, DoorBezelbuf, MAX_PATH);
			PathRemoveFileSpec(DoorBezelbuf);
			wcscat(DoorBezelbuf, L"\\DoorsBezel.png");

			DoorBezelSize.open(DoorBezelbuf);

			DoorBezelSize.seekg(16);
			DoorBezelSize.read((char*)&DoorBezelWidth, 4);
			DoorBezelSize.read((char*)&DoorBezelHeight, 4);

			DoorBezelWidth = ntohl(DoorBezelWidth);
			DoorBezelHeight = ntohl(DoorBezelHeight);

			vCenDoorBezel.x = DoorBezelWidth / 2.0; // Bezel X Center
			vCenDoorBezel.y = DoorBezelHeight / 2.0; // Bezel Y Center
			vPosDoorBezel.x = (float)resWidthD3D9 / 2.0;
			vPosDoorBezel.y = (float)resHeightD3D9 / 2.0;

			D3DXCreateTextureFromFileEx(Device, DoorBezelbuf, D3DX_DEFAULT_NONPOW2, D3DX_DEFAULT_NONPOW2, D3DX_DEFAULT, 0, D3DFMT_UNKNOWN, D3DPOOL_MANAGED, D3DX_DEFAULT, D3DX_DEFAULT, 0, NULL, NULL, &TextureDoorBezel);
			D3DXCreateSprite(Device, &SpriteDoorBezel);

			GetModuleFileName(NULL, DoorLeftbuf, MAX_PATH);
			PathRemoveFileSpec(DoorLeftbuf);
			wcscat(DoorLeftbuf, L"\\DoorLeft.png");

			DoorLeftSize.open(DoorLeftbuf);

			DoorLeftSize.seekg(16);
			DoorLeftSize.read((char*)&DoorLeftWidth, 4);
			DoorLeftSize.read((char*)&DoorLeftHeight, 4);

			DoorLeftWidth = ntohl(DoorLeftWidth);
			DoorLeftHeight = ntohl(DoorLeftHeight);

			vCenDoorLeft.x = DoorLeftWidth / 2.0; // Bezel X Center
			vCenDoorLeft.y = DoorLeftHeight / 2.0; // Bezel Y Center
			vPosDoorLeft.y = (float)resHeightD3D9 / 2.0; // Door Left Y

			D3DXCreateTextureFromFileEx(Device, DoorLeftbuf, D3DX_DEFAULT_NONPOW2, D3DX_DEFAULT_NONPOW2, D3DX_DEFAULT, 0, D3DFMT_UNKNOWN, D3DPOOL_MANAGED, D3DX_DEFAULT, D3DX_DEFAULT, 0, NULL, NULL, &TextureDoorLeft);
			D3DXCreateSprite(Device, &SpriteDoorLeft);

			GetModuleFileName(NULL, DoorRightbuf, MAX_PATH);
			PathRemoveFileSpec(DoorRightbuf);
			wcscat(DoorRightbuf, L"\\DoorRight.png");

			DoorRightSize.open(DoorRightbuf);

			DoorRightSize.seekg(16);
			DoorRightSize.read((char*)&DoorRightWidth, 4);
			DoorRightSize.read((char*)&DoorRightHeight, 4);

			DoorRightWidth = ntohl(DoorRightWidth);
			DoorRightHeight = ntohl(DoorRightHeight);

			vCenDoorRight.x = DoorRightWidth / 2.0; // Door Right X Center
			vCenDoorRight.y = DoorRightHeight / 2.0; // Door Right Y Center
			vPosDoorRight.y = (float)resHeightD3D9 / 2.0; // Door Right Y

			D3DXCreateTextureFromFileEx(Device, DoorRightbuf, D3DX_DEFAULT_NONPOW2, D3DX_DEFAULT_NONPOW2, D3DX_DEFAULT, 0, D3DFMT_UNKNOWN, D3DPOOL_MANAGED, D3DX_DEFAULT, D3DX_DEFAULT, 0, NULL, NULL, &TextureDoorRight);
			D3DXCreateSprite(Device, &SpriteDoorRight);
		}
	}

	if (EnableD3D9Doors)
	{
		EADPAttractionDoorsCalculations();

		SpriteDoorBezel->Begin(0);
		SpriteDoorBezel->Draw(TextureDoorBezel, NULL, &vCenDoorBezel, &vPosDoorBezel, 0xFFFFFFFF);
		SpriteDoorBezel->End();

		SpriteDoorLeft->Begin(0);
		SpriteDoorLeft->Draw(TextureDoorLeft, NULL, &vCenDoorLeft, &vPosDoorLeft, 0xFFFFFFFF);
		SpriteDoorLeft->End();

		SpriteDoorRight->Begin(0);
		SpriteDoorRight->Draw(TextureDoorRight, NULL, &vCenDoorRight, &vPosDoorRight, 0xFFFFFFFF);
		SpriteDoorRight->End();
	}

	if (EnableD3D9Crosshairs)
	{
		EADPCrosshairCalculations();

		Sprite1P->Begin(0);
		if (Player1Active && !P1SceenOut)
			Sprite1P->Draw(Texture1P, NULL, &vCen1P, &vPos1P, 0xFFFFFFFF);
		Sprite1P->End();

		Sprite2P->Begin(0);
		if (Player2Active && !P2SceenOut)
			Sprite2P->Draw(Texture2P, NULL, &vCen2P, &vPos2P, 0xFFFFFFFF);
		Sprite2P->End();
	}

	if (EnableD3D9Bezel)
	{
		if (ScaleD3D9Bezel)
		{
			if (EnableD3D9Border)
			{
				ScaleBezelX = ((float)resWidthD3D9 - (float)BorderThickness) / (float)BezelWidth;
				ScaleBezelY = ((float)resHeightD3D9 - (float)BorderThickness) / (float)BezelHeight;

				vPosBezel.x = ((float)BorderThickness / 2.0) / ScaleBezelX;
				vPosBezel.y = ((float)BorderThickness / 2.0) / ScaleBezelY;
			}
			else
			{
				ScaleBezelX = (float)resWidthD3D9 / (float)BezelWidth;
				ScaleBezelY = (float)resHeightD3D9 / (float)BezelHeight;

				vPosBezel.x = 0;
				vPosBezel.y = 0;
			}
		}
		else
		{
			if (EnableD3D9Border)
			{
				ScaleBezelX = ((float)resWidthD3D9 - (float)BorderThickness) / (float)resWidthD3D9;
				ScaleBezelY = ((float)resHeightD3D9 - (float)BorderThickness) / (float)resHeightD3D9;

				vPosBezel.x = (float)resWidthD3D9 / 2.0 + ((float)BorderThickness / 2.0) / ScaleBezelX;
				vPosBezel.y = (float)resHeightD3D9 / 2.0 + ((float)BorderThickness / 2.0) / ScaleBezelY;
			}
			else
			{
				ScaleBezelX = 1.0f;
				ScaleBezelY = 1.0f;

				vPosBezel.x = (float)resWidthD3D9 / 2.0;
				vPosBezel.y = (float)resHeightD3D9 / 2.0;
			}
		}

		SpriteBezel->Begin(0);

		D3DXMatrixScaling(&bezel_scaling_matrix, ScaleBezelX, ScaleBezelY, 1.0f);

		if (ScaleD3D9Bezel)
		{
			SpriteBezel->SetTransform(&bezel_scaling_matrix);
			SpriteBezel->Draw(TextureBezel, NULL, NULL, &vPosBezel, 0xFFFFFFFF);
		}
		else
		{
			SpriteBezel->SetTransform(&bezel_scaling_matrix);
			SpriteBezel->Draw(TextureBezel, NULL, &vCenBezel, &vPosBezel, 0xFFFFFFFF);
		}

		SpriteBezel->End();
	}

	if (EnableD3D9Border)
	{
		vPosBorder.x = resWidthD3D9 / 2.0;
		vPosBorder.y = resHeightD3D9 / 2.0;

		SpriteBorder->Begin(0);

		if (ScaleD3D9Border)
		{
			D3DXMatrixScaling(&border_scaling_matrix, (float)resWidthD3D9 / (float)BorderWidth, (float)resHeightD3D9 / (float)BorderHeight, 1.0f);
			SpriteBorder->SetTransform(&border_scaling_matrix);
			SpriteBorder->Draw(TextureBorder, NULL, NULL, NULL, 0xFFFFFFFF);
		}
		else
			SpriteBorder->Draw(TextureBorder, NULL, &vCenBorder, &vPosBorder, 0xFFFFFFFF);

		SpriteBorder->End();
	}
}

typedef HRESULT(APIENTRY* EndScene) (IDirect3DDevice9*);
static HRESULT APIENTRY EndScene_hook(IDirect3DDevice9*);
static EndScene EndScene_orig = 0;

static HRESULT APIENTRY EndScene_hook(IDirect3DDevice9* Device)
{
	FpsLimiter();

	Device->GetSwapChain(0, &SwapChainMain);

	SwapChainMain->GetBackBuffer(0, D3DBACKBUFFER_TYPE_LEFT, &SurfaceMain);

	Device->SetRenderTarget(0, SurfaceMain);

	RenderOurItems(Device);

	return EndScene_orig(Device);
}

typedef HRESULT(APIENTRY* Present) (LPDIRECT3DDEVICE9 Device, CONST RECT* pSrcRect, CONST RECT* pDestRect, HWND hDestWindow, CONST RGNDATA* pDirtyRegion);
static HRESULT APIENTRY Present_hook(LPDIRECT3DDEVICE9 Device, CONST RECT* pSrcRect, CONST RECT* pDestRect, HWND hDestWindow, CONST RGNDATA* pDirtyRegion);
static Present Present_orig = 0;

static HRESULT APIENTRY Present_hook(LPDIRECT3DDEVICE9 Device, CONST RECT* pSrcRect, CONST RECT* pDestRect, HWND hDestWindow, CONST RGNDATA* pDirtyRegion)
{
	FpsLimiter();

	Device->GetSwapChain(0, &SwapChainMain);

	SwapChainMain->GetBackBuffer(0, D3DBACKBUFFER_TYPE_LEFT, &SurfaceMain);

	Device->SetRenderTarget(0, SurfaceMain);

	RenderOurItems(Device);

	return Present_orig(Device, pSrcRect, pDestRect, hDestWindow, pDirtyRegion);
}

DWORD WINAPI D3D9MiscHook(__in  LPVOID lpParameter)
{
	while (!FindWindowA("Eva", "OpenParrot - Elevator Action: Death Parade"))
	{
		Sleep(8);
	}

	FpsLimiterSet(60.0f);

	HRESULT hr = E_FAIL;
	IDirect3D9Ex* pD3D = NULL;
	IDirect3DDevice9Ex* pDevice = NULL;

	// Create the D3D object, which is needed to create the D3DDevice.
	if (FAILED(hr = Direct3DCreate9Ex(D3D_SDK_VERSION, &pD3D)))
	{
		pD3D = NULL;
		return hr;
	}

	// Set up the structure used to create the D3DDevice. 
	D3DPRESENT_PARAMETERS d3dpp;
	ZeroMemory(&d3dpp, sizeof(d3dpp));
	d3dpp.Windowed = TRUE;
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;

	// Create the Direct3D device. 
	if (FAILED(hr = pD3D->CreateDeviceEx(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, GetForegroundWindow(),
		D3DCREATE_SOFTWARE_VERTEXPROCESSING,
		&d3dpp, NULL, &pDevice)))
	{
		pDevice = NULL;
		return hr;
	}

#if defined _M_X64
	DWORD64* dVtable = (DWORD64*)pDevice;
	dVtable = (DWORD64*)dVtable[0];
#elif defined _M_IX86
	DWORD* dVtable = (DWORD*)pDevice;
	dVtable = (DWORD*)dVtable[0];
#endif

	Present_orig = (Present)dVtable[17];
	EndScene_orig = (EndScene)dVtable[42];

	MH_Initialize();
	if (PresentHook)
		MH_CreateHook((DWORD_PTR*)dVtable[17], &Present_hook, reinterpret_cast<void**>(&Present_orig));
	else
		MH_CreateHook((DWORD_PTR*)dVtable[42], &EndScene_hook, reinterpret_cast<void**>(&EndScene_orig));
	MH_EnableHook(MH_ALL_HOOKS);

	pDevice->Release();
	pD3D->Release();
	return hr;
}

static InitFunction initFunc([]()
	{
		if (GameDetect::currentGame == GameID::ElevatorActionDeathParade)
		{
			EnableD3D9Crosshairs = (ToBool(config["Crosshairs"]["Enable"]));
			EnableD3D9Bezel = (ToBool(config["Bezel"]["Enable"]));
			EnableD3D9Border = (ToBool(config["Border"]["Enable"]));
			EnableD3D9Doors = (ToBool(config["Attraction Doors"]["Enable"]));

			ScaleD3D9Bezel = (ToBool(config["Bezel"]["Scale"]));
			ScaleD3D9Border = (ToBool(config["Border"]["Scale"]));

			BezelPixelWidth = ToInt(config["Bezel"]["Width Thickness"]);
			BezelPixelHeight = ToInt(config["Bezel"]["Height Thickness"]);
			BorderThickness = ToInt(config["Border"]["Thickness"]);

			int theScreenWidth = GetSystemMetrics(SM_CXFULLSCREEN);
			int theScreenHeight = GetSystemMetrics(SM_CYFULLSCREEN);

			if (theScreenWidth > theScreenHeight)
				Landscape = true;

			if (BorderThickness)
				BorderThickness = BorderThickness * 4.0;

			GetPrivateProfileStringA("General", "Render Hook", "", RenderHookChar, 256, ".\\teknoparrot.ini");

			if (strcmpi(RenderHookChar, "Present") == 0)
				PresentHook = true;

			if (EnableD3D9Crosshairs || EnableD3D9Bezel || EnableD3D9Border)
				CreateThread(NULL, 0, D3D9MiscHook, NULL, 0, NULL);
		}
	});