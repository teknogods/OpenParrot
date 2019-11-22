#include <StdInc.h>
#ifdef _M_AMD64
#include "Utility/ES3XLauncherHook.h"
#include "Utility/InitFunction.h"
#include "Functions/Global.h"
#include "Utility/GameDetect.h"

DWORD WINAPI XInputStateStarWars
(
	__in  DWORD  dwUserIndex,	// Index of the gamer associated with the device
	__out DWORD* pState			// Receives the current state
)
{
	if ((GameDetect::currentGame == GameID::StarWarsEs3X) || (GameDetect::currentGame == GameID::StarWarsJapEs3X))
	{
		return ERROR_SUCCESS;
	}
	else
	{
		return ERROR_DEVICE_NOT_CONNECTED;
	}
}

static unsigned char hasp_buffer[0xD40];

static void GenerateDongleData()
{
	memset(hasp_buffer, 0, 0xD40);
	hasp_buffer[0] = 0x01;
	hasp_buffer[0x13] = 0x01;
	hasp_buffer[0x17] = 0x0A;
	hasp_buffer[0x1B] = 0x04;
	hasp_buffer[0x1C] = 0x3B;
	hasp_buffer[0x1D] = 0x6B;
	hasp_buffer[0x1E] = 0x40;
	hasp_buffer[0x1F] = 0x87;

	hasp_buffer[0x23] = 0x01;
	hasp_buffer[0x27] = 0x0A;
	hasp_buffer[0x2B] = 0x04;
	hasp_buffer[0x2C] = 0x3B;
	hasp_buffer[0x2D] = 0x6B;
	hasp_buffer[0x2E] = 0x40;
	hasp_buffer[0x2F] = 0x87;
	memcpy(hasp_buffer + 0xD00, "274320990002", 12);
	hasp_buffer[0xD3E] = 0x6A;
	hasp_buffer[0xD3F] = 0x95;
}


#define HASP_STATUS_OK 0
static unsigned int Hook_hasp_login(int feature_id, void* vendor_code, int hasp_handle) {
#ifdef _DEBUG
	OutputDebugStringA("hasp_login\n");
#endif
	return HASP_STATUS_OK;
}

static unsigned int Hook_hasp_logout(int hasp_handle) {
#ifdef _DEBUG
	OutputDebugStringA("hasp_logout\n");
#endif
	return HASP_STATUS_OK;
}

static unsigned int Hook_hasp_encrypt(int hasp_handle, unsigned char* buffer, unsigned int buffer_size) {
#ifdef _DEBUG
	OutputDebugStringA("hasp_encrypt\n");
#endif
	return HASP_STATUS_OK;
}

static unsigned int Hook_hasp_decrypt(int hasp_handle, unsigned char* buffer, unsigned int buffer_size) {
#ifdef _DEBUG
	OutputDebugStringA("hasp_decrypt\n");
#endif
	return HASP_STATUS_OK;
}

static unsigned int Hook_hasp_get_size(int hasp_handle, int hasp_fileid, unsigned int* hasp_size) {
#ifdef _DEBUG
	OutputDebugStringA("hasp_get_size\n");
#endif
	*hasp_size = 0xD40; // Max addressable size by the game... absmax is 4k
	return HASP_STATUS_OK;
}

static unsigned int Hook_hasp_read(int hasp_handle, int hasp_fileid, unsigned int offset, unsigned int length, unsigned char* buffer) {
#ifdef _DEBUG
	OutputDebugStringA("hasp_read\n");
#endif
	memcpy(buffer, hasp_buffer + offset, length);
	return HASP_STATUS_OK;
}

static unsigned int Hook_hasp_write(int hasp_handle, int hasp_fileid, unsigned int offset, unsigned int length, unsigned char* buffer) {
	return HASP_STATUS_OK;
}

BOOL (*WINAPI g_origSetWindowPos)(
	HWND hWnd,
	HWND hWndInsertAfter,
	int  X,
	int  Y,
	int  cx,
	int  cy,
	UINT uFlags
);

BOOL SetWindowPosHook(
	HWND hWnd,
	HWND hWndInsertAfter,
	int  X,
	int  Y,
	int  cx,
	int  cy,
	UINT uFlags
)
{
	if((DWORD_PTR)hWndInsertAfter && (DWORD_PTR)HWND_TOPMOST)
	{
		return g_origSetWindowPos(hWnd, HWND_TOP, X, Y, cx, cy, uFlags);
	}
	return g_origSetWindowPos(hWnd, hWndInsertAfter, X, Y, cx, cy, uFlags);
}

char newFileName[MAX_PATH];

static bool IsSaveDrive(LPCSTR filename)
{
	if ((filename[0] == 'f' || filename[0] == 'F' || filename[0] == 'g' || filename[0] == 'G') && filename[1] == ':')
	{
		return true;
	}
	return false;
}

UINT (WINAPI *g_origGetDriveTypeA)(
	LPCSTR lpRootPathName
);

UINT GetDriveTypeAHook(
	LPCSTR lpRootPathName
)
{
	if (IsSaveDrive(lpRootPathName))
	{
		return 3;
	}
	return g_origGetDriveTypeA(lpRootPathName);
}

errno_t (WINAPI *g_orig_sopen_s)(
	int* pfh,
	const char* filename,
	int oflag,
	int shflag,
	int pmode
);

errno_t _sopen_sHook(
	int* pfh,
	const char* filename,
	int oflag,
	int shflag,
	int pmode
)
{
	if (IsSaveDrive(filename))
	{
		memset(newFileName, 0, MAX_PATH);
		if(GetCurrentDirectoryA(MAX_PATH, newFileName) == 0)
		{
			// for some reason current dir fails, so we will just save as usual
			g_orig_sopen_s(pfh, filename, oflag, shflag, pmode);
		}
		
		memcpy(newFileName + strlen(newFileName), "\\OpenParrot\\", 12);
		newFileName[strlen(newFileName)] = tolower(filename[0]);
		newFileName[strlen(newFileName)] = '\\';
		memcpy(newFileName + strlen(newFileName), filename + 3, strlen(filename) - 3);
		
		return g_orig_sopen_s(pfh, newFileName, oflag, shflag, pmode);
	}
	return g_orig_sopen_s(pfh, filename, oflag, shflag, pmode);
}

int (WINAPI *g_orig_mkdir)(
	const char* dirname
);

int _mkdirHook(
	const char* dirname
)
{
	if (IsSaveDrive(dirname))
	{
		memset(newFileName, 0, MAX_PATH);
		if (GetCurrentDirectoryA(MAX_PATH, newFileName) == 0)
		{
			return g_orig_mkdir(dirname);
		}
		memcpy(newFileName + strlen(newFileName), "\\OpenParrot\\", 12);
		newFileName[strlen(newFileName)] = tolower(dirname[0]);
		newFileName[strlen(newFileName)] = '\\';
		memcpy(newFileName + strlen(newFileName), dirname + 3, strlen(dirname) - 3);
		return g_orig_mkdir(newFileName);
	}
	return g_orig_mkdir(dirname);
}

extern LPCSTR hookPort;

static InitFunction StarWarsJapEs3XFunc([]()
	{
		uintptr_t imageBase = (uintptr_t)GetModuleHandleA(0);

		hookPort = "COM3";

		GenerateDongleData();

		MH_Initialize();
		MH_CreateHookApi(L"hasp_windows_x64_100610.dll", "hasp_write", Hook_hasp_write, NULL);
		MH_CreateHookApi(L"hasp_windows_x64_100610.dll", "hasp_read", Hook_hasp_read, NULL);
		MH_CreateHookApi(L"hasp_windows_x64_100610.dll", "hasp_get_size", Hook_hasp_get_size, NULL);
		MH_CreateHookApi(L"hasp_windows_x64_100610.dll", "hasp_decrypt", Hook_hasp_decrypt, NULL);
		MH_CreateHookApi(L"hasp_windows_x64_100610.dll", "hasp_encrypt", Hook_hasp_encrypt, NULL);
		MH_CreateHookApi(L"hasp_windows_x64_100610.dll", "hasp_logout", Hook_hasp_logout, NULL);
		MH_CreateHookApi(L"hasp_windows_x64_100610.dll", "hasp_login", Hook_hasp_login, NULL);
		MH_CreateHookApi(L"xinput1_3.dll", "XInputGetState", &XInputStateStarWars, NULL);
		if (ToBool(config["General"]["Disable Rumble"]))
		{
			MH_CreateHookApi(L"xinput1_3.dll", "XInputSetState", &XInputStateStarWars, NULL);
		}
		MH_EnableHook(MH_ALL_HOOKS);

		if (ToBool(config["General"]["Remove Camera Error"]))
		{
			injector::MakeNOP(imageBase + 0xD2BAE, 6, true);
		}

	}, GameID::StarWarsJapEs3X);

static InitFunction StarWarsEs3XFunc([]()
{
	uintptr_t imageBase = (uintptr_t)GetModuleHandleA(0);

	hookPort = "COM3";
	
	GenerateDongleData();
	
	MH_Initialize();
	MH_CreateHookApi(L"hasp_windows_x64_100610.dll", "hasp_write", Hook_hasp_write, NULL);
	MH_CreateHookApi(L"hasp_windows_x64_100610.dll", "hasp_read", Hook_hasp_read, NULL);
	MH_CreateHookApi(L"hasp_windows_x64_100610.dll", "hasp_get_size", Hook_hasp_get_size, NULL);
	MH_CreateHookApi(L"hasp_windows_x64_100610.dll", "hasp_decrypt", Hook_hasp_decrypt, NULL);
	MH_CreateHookApi(L"hasp_windows_x64_100610.dll", "hasp_encrypt", Hook_hasp_encrypt, NULL);
	MH_CreateHookApi(L"hasp_windows_x64_100610.dll", "hasp_logout", Hook_hasp_logout, NULL);
	MH_CreateHookApi(L"hasp_windows_x64_100610.dll", "hasp_login", Hook_hasp_login, NULL);
	MH_CreateHookApi(L"xinput1_3.dll", "XInputGetState", &XInputStateStarWars, NULL);
	if (ToBool(config["General"]["Disable Rumble"]))
	{
		MH_CreateHookApi(L"xinput1_3.dll", "XInputSetState", &XInputStateStarWars, NULL);
	}	
	MH_EnableHook(MH_ALL_HOOKS);

	if (ToBool(config["General"]["Remove Camera Error"]))
	{
		injector::MakeNOP(imageBase + 0xD82EE, 6, true);
	}

}, GameID::StarWarsEs3X);

static InitFunction StarWarsEs3XLauncherFunc([]()
{
	uintptr_t imageBase = (uintptr_t)GetModuleHandleA(0);

	GenerateDongleData();

	MH_Initialize();
	MH_CreateHookApi(L"kernel32.dll", "CreateProcessW", CreateProcessWHook, (void**)&g_origCreateProcessW);
	MH_CreateHookApi(L"kernel32.dll", "GetDriveTypeA", GetDriveTypeAHook, (void**)&g_origGetDriveTypeA);
	MH_CreateHookApi(L"hasp_windows_x64_100610.dll", "hasp_write", Hook_hasp_write, NULL);
	MH_CreateHookApi(L"hasp_windows_x64_100610.dll", "hasp_read", Hook_hasp_read, NULL);
	MH_CreateHookApi(L"hasp_windows_x64_100610.dll", "hasp_get_size", Hook_hasp_get_size, NULL);
	MH_CreateHookApi(L"hasp_windows_x64_100610.dll", "hasp_decrypt", Hook_hasp_decrypt, NULL);
	MH_CreateHookApi(L"hasp_windows_x64_100610.dll", "hasp_encrypt", Hook_hasp_encrypt, NULL);
	MH_CreateHookApi(L"hasp_windows_x64_100610.dll", "hasp_logout", Hook_hasp_logout, NULL);
	MH_CreateHookApi(L"hasp_windows_x64_100610.dll", "hasp_login", Hook_hasp_login, NULL);
	MH_CreateHookApi(L"user32.dll", "SetWindowPos", SetWindowPosHook, (void**)&g_origSetWindowPos);
	MH_CreateHookApi(L"msvcr100.dll", "_sopen_s", _sopen_sHook, (void**)&g_orig_sopen_s);
	MH_CreateHookApi(L"msvcr100.dll", "_mkdir", _mkdirHook, (void**)&g_orig_mkdir);
	MH_EnableHook(MH_ALL_HOOKS);

	// Ignore Projector Error
	injector::WriteMemory<BYTE>(imageBase + 0x35580, 0xC3, true);

	if (ToBool(config["General"]["FreePlay"]))
	{
		injector::MakeNOP(imageBase + 0x4DD2F, 6, true);
		injector::MakeNOP(imageBase + 0x4DBEA, 7, true);
		injector::MakeNOP(imageBase + 0x4DD53, 7, true);
	}

	// Don't minimize all windows
	injector::MakeNOP(imageBase + 0x33AE2, 6);
	injector::WriteMemory<WORD>(imageBase + 0x33A45, 0xE990, true);

	// Don't hide cursor pls
	if (!ToBool(config["General"]["HideCursor"]))
	{
		injector::MakeNOP(imageBase + 0x342B3, 6);
	}

	memset(newFileName, 0, MAX_PATH);
	GetCurrentDirectoryA(MAX_PATH, newFileName);
	memcpy(newFileName + strlen(newFileName), "\\OpenParrot\\", 12);
	CreateDirectoryA(newFileName, nullptr);
	newFileName[strlen(newFileName)] = 'f';
	CreateDirectoryA(newFileName, nullptr);
	newFileName[strlen(newFileName) - 1] = 'g';
	CreateDirectoryA(newFileName, nullptr);
	
	hookPort = "COM3";
}, GameID::StarWarsEs3XLauncher);

static InitFunction StarWarsJapEs3XLauncherFunc([]()
{
	uintptr_t imageBase = (uintptr_t)GetModuleHandleA(0);

	GenerateDongleData();

	MH_Initialize();
	MH_CreateHookApi(L"kernel32.dll", "CreateProcessW", CreateProcessWHook, (void**)& g_origCreateProcessW);
	MH_CreateHookApi(L"kernel32.dll", "GetDriveTypeA", GetDriveTypeAHook, (void**)&g_origGetDriveTypeA);
	MH_CreateHookApi(L"hasp_windows_x64_100610.dll", "hasp_write", Hook_hasp_write, NULL);
	MH_CreateHookApi(L"hasp_windows_x64_100610.dll", "hasp_read", Hook_hasp_read, NULL);
	MH_CreateHookApi(L"hasp_windows_x64_100610.dll", "hasp_get_size", Hook_hasp_get_size, NULL);
	MH_CreateHookApi(L"hasp_windows_x64_100610.dll", "hasp_decrypt", Hook_hasp_decrypt, NULL);
	MH_CreateHookApi(L"hasp_windows_x64_100610.dll", "hasp_encrypt", Hook_hasp_encrypt, NULL);
	MH_CreateHookApi(L"hasp_windows_x64_100610.dll", "hasp_logout", Hook_hasp_logout, NULL);
	MH_CreateHookApi(L"hasp_windows_x64_100610.dll", "hasp_login", Hook_hasp_login, NULL);
	MH_CreateHookApi(L"user32.dll", "SetWindowPos", SetWindowPosHook, (void**)&g_origSetWindowPos);
	MH_CreateHookApi(L"msvcr100.dll", "_sopen_s", _sopen_sHook, (void**)&g_orig_sopen_s);
	MH_CreateHookApi(L"msvcr100.dll", "_mkdir", _mkdirHook, (void**)&g_orig_mkdir);
	MH_EnableHook(MH_ALL_HOOKS);

	// Ignore Projector Error
	injector::WriteMemory<BYTE>(imageBase + 0x3E4D0, 0xC3, true);

	// Ignore powershell
	injector::MakeRET(imageBase + 0x5E440);
	injector::MakeRET(imageBase + 0x5E540);

	memset(newFileName, 0, MAX_PATH);
	GetCurrentDirectoryA(MAX_PATH, newFileName);
	memcpy(newFileName + strlen(newFileName), "\\OpenParrot\\", 12);
	CreateDirectoryA(newFileName, nullptr);
	newFileName[strlen(newFileName)] = 'f';
	CreateDirectoryA(newFileName, nullptr);
	newFileName[strlen(newFileName) - 1] = 'g';
	CreateDirectoryA(newFileName, nullptr);

	hookPort = "COM3";
}, GameID::StarWarsJapEs3XLauncher);

#endif