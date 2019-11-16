#include <StdInc.h>
#ifdef _M_AMD64
#include "Utility/ES3XLauncherHook.h"
#include "Utility/InitFunction.h"
#include "Functions/Global.h"
#include "Utility/GameDetect.h"

DWORD WINAPI XInputGetStateStarWars
(
	__in  DWORD  dwUserIndex,	// Index of the gamer associated with the device
	__out DWORD* pState			// Receives the current state
)
{
	if (GameDetect::currentGame == GameID::StarWarsEs3X)
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

extern LPCSTR hookPort;

static InitFunction StarWarsJapEs3XFunc([]()
	{
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
		MH_CreateHookApi(L"xinput1_3.dll", "XInputGetState", &XInputGetStateStarWars, NULL);
		MH_EnableHook(MH_ALL_HOOKS);

	}, GameID::StarWarsEs3X);

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
	MH_CreateHookApi(L"xinput1_3.dll", "XInputGetState", &XInputGetStateStarWars, NULL);
	MH_EnableHook(MH_ALL_HOOKS);
		
	if (ToBool(config["General"]["2D DomeFix"]))
	{
		injector::MakeNOP(imageBase + 0xBCA13, 3);
	}
	
}, GameID::StarWarsEs3X);

static InitFunction StarWarsEs3XLauncherFunc([]()
{
	uintptr_t imageBase = (uintptr_t)GetModuleHandleA(0);

	GenerateDongleData();

	MH_Initialize();
	MH_CreateHookApi(L"kernel32.dll", "CreateProcessW", CreateProcessWHook, (void**)&g_origCreateProcessW);
	MH_CreateHookApi(L"hasp_windows_x64_100610.dll", "hasp_write", Hook_hasp_write, NULL);
	MH_CreateHookApi(L"hasp_windows_x64_100610.dll", "hasp_read", Hook_hasp_read, NULL);
	MH_CreateHookApi(L"hasp_windows_x64_100610.dll", "hasp_get_size", Hook_hasp_get_size, NULL);
	MH_CreateHookApi(L"hasp_windows_x64_100610.dll", "hasp_decrypt", Hook_hasp_decrypt, NULL);
	MH_CreateHookApi(L"hasp_windows_x64_100610.dll", "hasp_encrypt", Hook_hasp_encrypt, NULL);
	MH_CreateHookApi(L"hasp_windows_x64_100610.dll", "hasp_logout", Hook_hasp_logout, NULL);
	MH_CreateHookApi(L"hasp_windows_x64_100610.dll", "hasp_login", Hook_hasp_login, NULL);
	MH_EnableHook(MH_ALL_HOOKS);

	// Ignore Projector Error
	injector::WriteMemory<BYTE>(imageBase + 0x35580, 0xC3, true);

	hookPort = "COM3";
}, GameID::StarWarsEs3XLauncher);

static InitFunction StarWarsJapEs3XLauncherFunc([]()
{
	uintptr_t imageBase = (uintptr_t)GetModuleHandleA(0);

	GenerateDongleData();

	MH_Initialize();
	MH_CreateHookApi(L"kernel32.dll", "CreateProcessW", CreateProcessWHook, (void**)& g_origCreateProcessW);
	MH_CreateHookApi(L"hasp_windows_x64_100610.dll", "hasp_write", Hook_hasp_write, NULL);
	MH_CreateHookApi(L"hasp_windows_x64_100610.dll", "hasp_read", Hook_hasp_read, NULL);
	MH_CreateHookApi(L"hasp_windows_x64_100610.dll", "hasp_get_size", Hook_hasp_get_size, NULL);
	MH_CreateHookApi(L"hasp_windows_x64_100610.dll", "hasp_decrypt", Hook_hasp_decrypt, NULL);
	MH_CreateHookApi(L"hasp_windows_x64_100610.dll", "hasp_encrypt", Hook_hasp_encrypt, NULL);
	MH_CreateHookApi(L"hasp_windows_x64_100610.dll", "hasp_logout", Hook_hasp_logout, NULL);
	MH_CreateHookApi(L"hasp_windows_x64_100610.dll", "hasp_login", Hook_hasp_login, NULL);
	MH_EnableHook(MH_ALL_HOOKS);

	// Ignore Projector Error
	injector::WriteMemory<BYTE>(imageBase + 0x3E4D0, 0xC3, true);

	// Ignore powershell
	injector::MakeRET(imageBase + 0x5E440);
	injector::MakeRET(imageBase + 0x5E540);

	hookPort = "COM3";
}, GameID::StarWarsJapEs3XLauncher);

#endif