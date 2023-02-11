#include <StdInc.h>
#include "Utility/InitFunction.h"
#include "Functions/Global.h"
#include "Mmsystem.h"
#if _M_IX86
extern int* ffbOffset;
extern int* wheelSection;

static bool CoinButton;
static bool SoundFail;
static bool Init;

static int CoinSoundEntry;
static int Play5;
static int Play4;
static int Play3;
static int Play2;
static int Play1;

static LPCSTR OpenCoin1 = "open .\\Coin\\Coin.mp3 alias CoinSound1";
static LPCSTR OpenCoin2 = "open .\\Coin\\Coin.mp3 alias CoinSound2";
static LPCSTR OpenCoin3 = "open .\\Coin\\Coin.mp3 alias CoinSound3";
static LPCSTR OpenCoin4 = "open .\\Coin\\Coin.mp3 alias CoinSound4";
static LPCSTR OpenCoin5 = "open .\\Coin\\Coin.mp3 alias CoinSound5";
static LPCSTR PlayCoin1 = "play CoinSound1 from 0";
static LPCSTR PlayCoin2 = "play CoinSound2 from 0";
static LPCSTR PlayCoin3 = "play CoinSound3 from 0";
static LPCSTR PlayCoin4 = "play CoinSound4 from 0";
static LPCSTR PlayCoin5 = "play CoinSound5 from 0";

int __stdcall Sr3FfbFunc(DWORD device, DWORD data)
{
	*ffbOffset = data;
	return 0;
}

DWORD(__stdcall* GetPrivateProfileIntAOri)(LPCSTR lpAppName, LPCSTR lpKeyName, INT nDefault, LPCSTR lpFileName);

DWORD WINAPI GetPrivateProfileIntAHook(LPCSTR lpAppName, LPCSTR lpKeyName, INT nDefault, LPCSTR lpFileName)
{
#ifdef _DEBUG
	info("GetPrivateProfileIntAHook %s", lpKeyName);
#endif

	if (_stricmp(lpKeyName, "HorizontalResolution") == 0)
		return FetchDwordInformation("General", "ResolutionWidth", 1280);
	else if (_stricmp(lpKeyName, "VerticalResolution") == 0)
		return FetchDwordInformation("General", "ResolutionHeight", 720);
	else if (_stricmp(lpKeyName, "Freeplay") == 0)
		return (DWORD)ToBool(config["General"]["FreePlay"]);
	else
		return GetPrivateProfileIntAOri(lpAppName, lpKeyName, nDefault, lpFileName);
}

DWORD(__stdcall* GetPrivateProfileStringAOri)(LPCSTR lpAppName, LPCSTR lpKeyName, LPCSTR lpDefault, LPSTR lpReturnedString, DWORD nSize, LPCSTR lpFileName);

DWORD WINAPI GetPrivateProfileStringAHook(LPCSTR lpAppName, LPCSTR lpKeyName, LPCSTR lpDefault, LPSTR lpReturnedString, DWORD nSize, LPCSTR lpFileName)
{
#ifdef _DEBUG
	info("GetPrivateProfileStringAHook %s", lpKeyName);
#endif

	if (_stricmp(lpKeyName, "LANGUAGE") == 0)
	{
		strcpy(lpReturnedString, config["General"]["Language"].c_str());
		return nSize;
	}
	else
	{
		return GetPrivateProfileStringAOri(lpAppName, lpKeyName, lpDefault, lpReturnedString, nSize, lpFileName);
	}
}

HWND(__stdcall* CreateWindowExAOrg)(DWORD dwExStyle, LPCSTR lpClassName, LPCSTR lpWindowName, DWORD dwStyle, int x, int y, int nWidth, int nHeight, HWND hWndParent, HMENU hMenu, HINSTANCE hInstance, LPVOID lpParam);

static HWND WINAPI CreateWindowExAHook(DWORD dwExStyle, LPCSTR lpClassName, LPCSTR lpWindowName, DWORD dwStyle, int x, int y, int nWidth, int nHeight, HWND hWndParent, HMENU hMenu, HINSTANCE hInstance, LPVOID lpParam)
{
	if (nWidth == 0 || nHeight == 0)
	{
		nWidth = FetchDwordInformation("General", "ResolutionWidth", 1280);
		nHeight = FetchDwordInformation("General", "ResolutionHeight", 720);
	}

	return CreateWindowExAOrg(dwExStyle, lpClassName, lpWindowName, dwStyle, x, y, nWidth, nHeight, hWndParent, hMenu, hInstance, lpParam);
}

static BOOL(__stdcall* ClipCursorOrg)(const RECT* lpRect);

static BOOL WINAPI ClipCursorHook(const RECT* lpRect)
{
	return false;
}

static BOOL(__stdcall* GetClipCursorOrg)(LPRECT lpRect);

static BOOL WINAPI GetClipCursorHook(LPRECT lpRect)
{
	return false;
}

static DWORD WINAPI CoinInput(LPVOID lpParam)
{
	while (true)
	{
		if (!Init)
		{
			Init = true;
			mciSendStringA(OpenCoin1, 0, 0, 0);
			mciSendStringA(OpenCoin2, 0, 0, 0);
			mciSendStringA(OpenCoin3, 0, 0, 0);
			mciSendStringA(OpenCoin4, 0, 0, 0);
			mciSendStringA(OpenCoin5, 0, 0, 0);
		}

		if (*wheelSection & 0x01)
		{
			if (!CoinButton)
			{
				CoinButton = true;

				if (!SoundFail)
				{
					++CoinSoundEntry;

					switch (CoinSoundEntry)
					{
					case 0x05:
						Play5 = mciSendStringA(PlayCoin5, 0, 0, 0);
						CoinSoundEntry = 0;
						break;
					case 0x04:
						Play4 = mciSendStringA(PlayCoin4, 0, 0, 0);
						break;
					case 0x03:
						Play3 = mciSendStringA(PlayCoin3, 0, 0, 0);
						break;
					case 0x02:
						Play2 = mciSendStringA(PlayCoin2, 0, 0, 0);
						break;
					case 0x01:
						Play1 = mciSendStringA(PlayCoin1, 0, 0, 0);
						break;

						if (Play1 != 0 && Play2 != 0 && Play3 != 0 && Play4 != 0 && Play5 != 0)
							SoundFail = true;
					}
				}
				++*(BYTE*)(0xB41748);
			}
		}
		else
		{
			if (CoinButton)
				CoinButton = false;
		}
		Sleep(16);
	}
}

static InitFunction sr3Func([]()
{
	DWORD imageBase = (DWORD)GetModuleHandleA(0);
	DWORD oldprot = 0;
	DWORD oldprot2 = 0;
	VirtualProtect((LPVOID)0x401000, 0x273000, 0x40, &oldprot);
	// force controller init
	//injector::MakeJMP(0x57B2F0, ReturnTrue);
	memcpy((LPVOID)0x57B2F0, "\x33\xC0\x40\xC3", 4);

	// disable checks for controller pointer
	memset((LPVOID)0x57B670, 0x90, 15);

	// dereference
	memset((LPVOID)0x57B684, 0x90, 3);

	// Hook FFB
	// Remove loading of inpout32.dll
	injector::MakeNOP(0x006582A8, 0x17);
	// Give our own pointer to the FFB func
	injector::WriteMemory<uint8_t>(0x006582A8, 0xB8, true);
	injector::WriteMemory<DWORD>(0x006582A9, (DWORD)Sr3FfbFunc, true);

	// ReadFile call
	static DWORD source = (DWORD)(LPVOID)&ReadFileHooked;
	*(DWORD *)0x57B696 = (DWORD)(LPVOID)&source;
	VirtualProtect((LPVOID)0x401000, 0x273000, oldprot, &oldprot2);

	// skip minimum resolution check
	injector::WriteMemory<BYTE>(0x588755, 0xEB, true); // width
	injector::WriteMemory<BYTE>(0x588762, 0xEB, true); // height

	//Stop game pausing when click off window
	injector::MakeNOP(0x5588BB, 6);

	MH_Initialize();

	if (ToBool(config["General"]["Windowed"]))
	{
		// don't hide cursor
		injector::MakeNOP(0x591106, 8, true);

		injector::MakeNOP(0x591189, 8, true);
		injector::MakeNOP(0x5910FE, 8, true);

		MH_CreateHookApi(L"User32.dll", "CreateWindowExA", &CreateWindowExAHook, (void**)&CreateWindowExAOrg);
	}

	if (ToBool(config["General"]["Windowed"]) || (ToBool(config["Score"]["Enable Submission (Patreon Only)"]) && ToBool(config["Score"]["Enable GUI"]))) // don't clip cursor
	{
		MH_CreateHookApi(L"User32.dll", "ClipCursor", &ClipCursorHook, (void**)&ClipCursorOrg);
		MH_CreateHookApi(L"User32.dll", "GetClipCursor", &GetClipCursorHook, (void**)&GetClipCursorOrg);
	}

	if ((ToBool(config["Score"]["Enable Submission (Patreon Only)"]) && ToBool(config["Score"]["Enable GUI"]) && ToBool(config["Score"]["Hide Cursor"])))
	{
		ShowCursor(false);
	}

	if (ToBool(config["General"]["InRace 2D Adjust"]))
	{
		DWORD XResolution = FetchDwordInformation("General", "ResolutionWidth", 1280);

		injector::WriteMemoryRaw(imageBase + 0x1A6F28, "\x66\xBA\x00\x05\x90\x90\x90", 7, true); //In Race Timer
		injector::WriteMemoryRaw(imageBase + 0x19EA49, "\x66\xB9\x00\x05\x90\x90\x90", 7, true); //Time Extended
		injector::WriteMemoryRaw(imageBase + 0x19E806, "\x66\xB9\x00\x05\x90\x90\x90", 7, true); //Final Lap
		injector::WriteMemoryRaw(imageBase + 0x1A48E5, "\xBA\x00\x05\x00\x00\x90\x90", 7, true); //CountDown

		switch (XResolution)
		{
		case 640:
			injector::WriteMemory<WORD>(imageBase + 0x1A6F2A, 1160, true);
			injector::WriteMemory<WORD>(imageBase + 0x19EA4B, 1045, true);
			injector::WriteMemory<WORD>(imageBase + 0x19E808, 1100, true);
			injector::WriteMemory<WORD>(imageBase + 0x1A48E6, 1160, true);
			break;
		case 800:
			injector::WriteMemory<WORD>(imageBase + 0x1A6F2A, 1200, true);
			injector::WriteMemory<WORD>(imageBase + 0x19EA4B, 1140, true);
			injector::WriteMemory<WORD>(imageBase + 0x19E808, 1170, true);
			injector::WriteMemory<WORD>(imageBase + 0x1A48E6, 1200, true);
			break;
		case 1024:
			injector::WriteMemory<WORD>(imageBase + 0x1A6F2A, 1255, true);
			injector::WriteMemory<WORD>(imageBase + 0x19EA4B, 1235, true);
			injector::WriteMemory<WORD>(imageBase + 0x19E808, 1240, true);
			injector::WriteMemory<WORD>(imageBase + 0x1A48E6, 1255, true);
			break;
		case 1152:
			injector::WriteMemory<WORD>(imageBase + 0x1A6F2A, 1260, true);
			injector::WriteMemory<WORD>(imageBase + 0x19EA4B, 1270, true);
			injector::WriteMemory<WORD>(imageBase + 0x19E808, 1270, true);
			injector::WriteMemory<WORD>(imageBase + 0x1A48E6, 1260, true);
			break;
		case 1176:
			injector::WriteMemory<WORD>(imageBase + 0x1A6F2A, 1270, true);
			injector::WriteMemory<WORD>(imageBase + 0x19EA4B, 1280, true);
			injector::WriteMemory<WORD>(imageBase + 0x19E808, 1270, true);
			injector::WriteMemory<WORD>(imageBase + 0x1A48E6, 1270, true);
			break;
		case 1360:
			injector::WriteMemory<WORD>(imageBase + 0x1A6F2A, 1280, true);
			injector::WriteMemory<WORD>(imageBase + 0x19EA4B, 1320, true);
			injector::WriteMemory<WORD>(imageBase + 0x19E808, 1305, true);
			injector::WriteMemory<WORD>(imageBase + 0x1A48E6, 1280, true);
			break;
		case 1366:
			injector::WriteMemory<WORD>(imageBase + 0x1A6F2A, 1285, true);
			injector::WriteMemory<WORD>(imageBase + 0x19EA4B, 1320, true);
			injector::WriteMemory<WORD>(imageBase + 0x19E808, 1305, true);
			injector::WriteMemory<WORD>(imageBase + 0x1A48E6, 1285, true);
			break;
		case 1440:
			injector::WriteMemory<WORD>(imageBase + 0x1A6F2A, 1300, true);
			injector::WriteMemory<WORD>(imageBase + 0x19EA4B, 1340, true);
			injector::WriteMemory<WORD>(imageBase + 0x19E808, 1310, true);
			injector::WriteMemory<WORD>(imageBase + 0x1A48E6, 1300, true);
			break;
		case 1600:
			injector::WriteMemory<WORD>(imageBase + 0x1A6F2A, 1305, true);
			injector::WriteMemory<WORD>(imageBase + 0x19EA4B, 1360, true);
			injector::WriteMemory<WORD>(imageBase + 0x19E808, 1325, true);
			injector::WriteMemory<WORD>(imageBase + 0x1A48E6, 1305, true);
			break;
		case 1680:
			injector::WriteMemory<WORD>(imageBase + 0x1A6F2A, 1310, true);
			injector::WriteMemory<WORD>(imageBase + 0x19EA4B, 1370, true);
			injector::WriteMemory<WORD>(imageBase + 0x19E808, 1335, true);
			injector::WriteMemory<WORD>(imageBase + 0x1A48E6, 1310, true);
			break;
		case 1920:
			injector::WriteMemory<WORD>(imageBase + 0x1A6F2A, 1320, true);
			injector::WriteMemory<WORD>(imageBase + 0x19EA4B, 1400, true);
			injector::WriteMemory<WORD>(imageBase + 0x19E808, 1355, true);
			injector::WriteMemory<WORD>(imageBase + 0x1A48E6, 1320, true);
			break;
		case 2560:
			injector::WriteMemory<WORD>(imageBase + 0x1A6F2A, 1340, true);
			injector::WriteMemory<WORD>(imageBase + 0x19EA4B, 1450, true);
			injector::WriteMemory<WORD>(imageBase + 0x19E808, 1390, true);
			injector::WriteMemory<WORD>(imageBase + 0x1A48E6, 1340, true);
			break;
		case 3840:
			injector::WriteMemory<WORD>(imageBase + 0x1A6F2A, 1365, true);
			injector::WriteMemory<WORD>(imageBase + 0x19EA4B, 1500, true);
			injector::WriteMemory<WORD>(imageBase + 0x19E808, 1425, true);
			injector::WriteMemory<WORD>(imageBase + 0x1A48E6, 1365, true);
			break;
		case 5120:
			injector::WriteMemory<WORD>(imageBase + 0x1A6F2A, 1375, true);
			injector::WriteMemory<WORD>(imageBase + 0x19EA4B, 1525, true);
			injector::WriteMemory<WORD>(imageBase + 0x19E808, 1440, true);
			injector::WriteMemory<WORD>(imageBase + 0x1A48E6, 1375, true);
			break;
		}
	}
	
	if (!ToBool(config["General"]["FreePlay"]))
		CreateThread(NULL, 0, CoinInput, NULL, 0, NULL); //Create thread for coin

	MH_CreateHookApi(L"kernel32.dll", "GetPrivateProfileIntA", &GetPrivateProfileIntAHook, (void**)&GetPrivateProfileIntAOri);
	MH_CreateHookApi(L"kernel32.dll", "GetPrivateProfileStringA", &GetPrivateProfileStringAHook, (void**)&GetPrivateProfileStringAOri);
	MH_EnableHook(MH_ALL_HOOKS);

}, GameID::SR3);
#endif