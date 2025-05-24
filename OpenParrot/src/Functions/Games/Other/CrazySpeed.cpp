#if _M_IX86
#include <StdInc.h>
#include "Utility/InitFunction.h"
#include "Functions/Global.h"
#include <Functions/FpsLimiter.h>
#include <fstream>
#include <queue>
#include <intrin.h>
#include <winscard.h>

static const HANDLE hIoHandle = (HANDLE)0x8001;
static std::queue<BYTE> ioBuffer;
static SCARDHANDLE g_fakeCardHandle = 0x1337;

extern int* ffbOffset;
extern int* ffbOffset2;
extern int* ffbOffset3;
extern int* ffbOffset4;

namespace CrazySpeedIOBoard {
	static bool coinPressed = false;
	static uint8_t coinCount = 0;
	static uint8_t frameIndex = 0;
	struct IOBOARD_DATA {
		uint16_t value1;     // Bytes 0-1: First analog value (steering)
		uint16_t value2;     // Bytes 2-3: Second analog value (throttle) 
		uint16_t value3;     // Bytes 4-5: Third analog value (brake)
		uint16_t inputBits;  // Bytes 6-7: Buttons, up to 16 of em

		uint8_t statusFlags; // Byte 8: Status flags ? unused
		//   Bit 7 (0x80): Coin
		//   Bit 6 (0x40): Error flag (inverted) ?
		//   Bit 5 (0x20): Flag at offset 372 unsure
		//   Bit 4 (0x10): Flag at offset 364 unsure
		//   Bits 0-3: Unknown buttons, technically, not used in this game

		uint8_t additionalValue; // unused
	};

	struct IOBOARD_PACKET {
		uint8_t sync[2];  // Always 0xAA, 0xAA
		struct IOBOARD_DATA data;
		uint16_t crc16;  // CRC16 of the 10 bytes of data
	};

	enum IOBoardButton {
		BTN_UP = 0,
		BTN_DOWN = 1,
		BTN_LEFT = 2,
		BTN_RIGHT = 3,
		BTN_VIEW = 4,
		BTN_START = 5,
		BTN_SHIFT_DOWN = 6,
		BTN_SHIFT_UP = 7,
		BTN_SERVICE = 8,
		BTN_TEST = 9,  // tries rebooting the game into test mode (bossmenu)
		BTN_BIT10 = 10,
		BTN_BIT11 = 11,
		BTN_BIT12 = 12,
		BTN_BIT13 = 13,
		BTN_BIT14 = 14,
		BTN_BIT15 = 15
	};

	static uint16_t CRC16(uint16_t initialValue, const uint8_t* data, size_t length = 10) {
		unsigned int crc = 0xFFFF;

		for (unsigned int i = 0; i < length; i++) {
			crc ^= data[i];

			int bitCount = 8;
			do {
				if ((crc & 1) != 0)
					crc ^= 0x14002;
				crc >>= 1;
				--bitCount;
			} while (bitCount);
		}

		return (uint16_t)crc;
	}

	static struct {
		uint16_t x_axis = 512;  // Steering wheel
		uint16_t y_axis = 0;    // Throttle
		uint16_t z_axis = 0;    // Brake

		const uint16_t min_value = 0;
		const uint16_t max_value = 1024;
	} analog_controls;

	static void updateAnalogControls() {
		// Important: analog values are inverted! 
		// analog values from TPUI are 8bit so we need to scale them up
		// i still wish we would do 16 bit at least
		analog_controls.x_axis = (255 - *ffbOffset2) * 4;
		analog_controls.y_axis = (255 - *ffbOffset3) * 4;
		analog_controls.z_axis = (255 - *ffbOffset4) * 4;

	}

	static void setBit(uint16_t& value, int bit, bool isSet) {
		if (isSet) {
			value |= (1 << bit);
		}
		else {
			value &= ~(1 << bit);
		}
	}

	static void setBit(uint8_t& value, int bit, bool isSet) {
		if (isSet) {
			value |= (1 << bit);
		}
		else {
			value &= ~(1 << bit);
		}
	}

	static uint16_t previousInputBits = 0;
	static uint8_t previousStatusFlags = 0;

	static void updateInputBits(IOBOARD_DATA& data) {
		data.inputBits = 0; // Clear all bits first

		setBit(data.inputBits, BTN_TEST, (*ffbOffset & 0x1) != 0);
		setBit(data.inputBits, BTN_SERVICE, (*ffbOffset & 0x2) != 0);

		setBit(data.inputBits, BTN_UP, (*ffbOffset & 0x1000));
		setBit(data.inputBits, BTN_DOWN, (*ffbOffset & 0x2000));
		setBit(data.inputBits, BTN_LEFT, (*ffbOffset & 0x4000));
		setBit(data.inputBits, BTN_RIGHT, (*ffbOffset & 0x8000));

		setBit(data.inputBits, BTN_VIEW, (*ffbOffset & 0x0100));
		setBit(data.inputBits, BTN_START, (*ffbOffset & 0x08));
		setBit(data.inputBits, BTN_SHIFT_DOWN, (*ffbOffset & 0x0400));
		setBit(data.inputBits, BTN_SHIFT_UP, (*ffbOffset & 0x0200));

		// these arent used in crazy speed but the io board has room for em
		//setBit(data.inputBits, BTN_BIT10, GetAsyncKeyState(VK_F1) & 0x8000);
		//setBit(data.inputBits, BTN_BIT11, GetAsyncKeyState(VK_F2) & 0x8000);
		//setBit(data.inputBits, BTN_BIT12, GetAsyncKeyState(VK_F3) & 0x8000);
		//setBit(data.inputBits, BTN_BIT13, GetAsyncKeyState(VK_F4) & 0x8000);
		//setBit(data.inputBits, BTN_BIT14, GetAsyncKeyState(VK_F7) & 0x8000);
		//setBit(data.inputBits, BTN_BIT15, GetAsyncKeyState(VK_F6) & 0x8000);

		// these are counted as buttons still, 20 buttons max
		//setBit(data.statusFlags, 0, GetAsyncKeyState('O') & 0x8000);
		//setBit(data.statusFlags, 1, GetAsyncKeyState('R') & 0x8000);
		//setBit(data.statusFlags, 2, GetAsyncKeyState('T') & 0x8000);
		//setBit(data.statusFlags, 3, GetAsyncKeyState('Y') & 0x8000);

		// these set some sort of flags, but i don't know what they do, nothing visible on screen x)
		//setBit(data.statusFlags, 4, GetAsyncKeyState('U') & 0x8000);
		//setBit(data.statusFlags, 5, GetAsyncKeyState('I') & 0x8000);
		//setBit(data.statusFlags, 6, GetAsyncKeyState('L') & 0x8000);
	}

	static void formPacket() {
		// add sync bytes first! it will read them one by one,
		// before reading the rest of the packet (12 bytes, 10 data, 2 checksum)
		ioBuffer.push(0xAA);
		ioBuffer.push(0xAA);
		IOBOARD_DATA data{};

		updateAnalogControls();
		data.value1 = analog_controls.x_axis;
		data.value2 = analog_controls.y_axis;
		data.value3 = analog_controls.z_axis;
		data.inputBits = 0x0000;
		data.statusFlags = 0x00 | 0x40;

		if (*ffbOffset & 0x04)
		{
			if (!coinPressed)
			{
				coinPressed = true;
				data.statusFlags |= 0x80;
			}
		}
		else
		{
			coinPressed = false;
		}

		data.additionalValue = 0;

		updateInputBits(data);

		uint16_t crc = CRC16(0xA, reinterpret_cast<const uint8_t*>(&data), sizeof(data));
		const uint8_t* dataBytes = reinterpret_cast<const uint8_t*>(&data);
		for (size_t i = 0; i < sizeof(data); i++) {
			ioBuffer.push(dataBytes[i]);
		}

		ioBuffer.push(crc & 0xFF);        // Low byte
		ioBuffer.push((crc >> 8) & 0xFF); // High byte
	}
}

void AddCommOverride(HANDLE hFile);

static BOOL __stdcall ReadFileWrap(HANDLE hFile,
	LPVOID lpBuffer,
	DWORD nNumberOfBytesToRead,
	LPDWORD lpNumberOfBytesRead,
	LPOVERLAPPED lpOverlapped)
{
	if (hFile == hIoHandle)
	{
		if (ioBuffer.empty())
		{
			CrazySpeedIOBoard::formPacket();
		}

		int toRead = min(ioBuffer.size(), nNumberOfBytesToRead);

		uint8_t* dest = reinterpret_cast<uint8_t*>(lpBuffer);
		for (int i = 0; i < toRead; i++) {
			dest[i] = ioBuffer.front();
			ioBuffer.pop();
		}
		*lpNumberOfBytesRead = toRead;

		return TRUE;
	}
	return ReadFile(hFile, lpBuffer, nNumberOfBytesToRead, lpNumberOfBytesRead, lpOverlapped);
}

static HANDLE WINAPI CreateFileWWrap(LPCWSTR lpFileName,
	DWORD dwDesiredAccess,
	DWORD dwShareMode,
	LPSECURITY_ATTRIBUTES lpSecurityAttributes,
	DWORD dwCreationDisposition,
	DWORD dwFlagsAndAttributes,
	HANDLE hTemplateFile)
{
	if (wcsstr(lpFileName, L"COM1") != 0)
	{
		TpInfo("COM1 requested, forwarding to io emu");
		HANDLE hFile = hIoHandle;
		AddCommOverride(hFile);
		return hFile;
	}

	return CreateFileW(lpFileName,
		dwDesiredAccess,
		dwShareMode,
		lpSecurityAttributes,
		dwCreationDisposition,
		dwFlagsAndAttributes,
		hTemplateFile);
}

static BOOL __stdcall WriteFileWrap(HANDLE hFile,
	LPVOID lpBuffer,
	DWORD nNumberOfBytesToWrite,
	LPDWORD lpNumberOfBytesWritten,
	LPOVERLAPPED lpOverlapped)
{
	if (hFile == hIoHandle)
	{
		// is this for FFB?
		//TpInfo("IoBoard WriteFile called");
		*lpNumberOfBytesWritten = nNumberOfBytesToWrite;
		return TRUE;
	}
	return WriteFile(hFile, lpBuffer, nNumberOfBytesToWrite, lpNumberOfBytesWritten, lpOverlapped);
}

extern int* ffbOffset;

static BOOL __stdcall ClearCommErrorWrap(HANDLE hFile, LPDWORD lpErrors, LPCOMSTAT lpStat)
{
	if (hFile != hIoHandle)
	{
		return ClearCommError(hFile, lpErrors, lpStat);
	}

	if (lpStat)
	{

		if (!ioBuffer.empty())
		{
			lpStat->cbInQue = ioBuffer.size();
		}
		else
		{
			lpStat->cbInQue = 0;
		}
	}

	return true;
}

static BOOL __stdcall GetCommModemStatusWrap(HANDLE hFile, LPDWORD lpModemStat)
{
	if (hFile != hIoHandle) {
		return GetCommModemStatus(hFile, lpModemStat);
	}

	*lpModemStat = 0;

	return TRUE;
}

static BOOL WINAPI CloseHandleWrap(
	_In_ HANDLE hObject
)
{
	if (hObject == hIoHandle)
	{
		return TRUE;
	}
	return CloseHandle(hObject);
}

// 2 * 3 = 6 threads, should be enough. I assume the og pcb has a dualcore cpu.
static void (WINAPI* GetSystemInfo_orig)(LPSYSTEM_INFO lpSystemInfo);
static void WINAPI GetSystemInfo_hook(LPSYSTEM_INFO lpSystemInfo)
{
	GetSystemInfo_orig(lpSystemInfo);
	lpSystemInfo->dwNumberOfProcessors = 2;
	return;
}

// cpu usage bandaid fix
DWORD g_jumpTargetAddress = 0x746BB0;
static __declspec(naked) void WorkerThreadSleep()
{
	__asm {
		pushad
		pushfd

		// 8ms will be fine, i think
		push 8
		call dword ptr[Sleep]

			popfd
				popad
				mov dword ptr[esp + 10h], edi
				jmp dword ptr[g_jumpTargetAddress]
	}
}

// unused but keeping it again for if the issue appears again so i can try and debug it
static BOOL(WINAPI* GetQueuedCompletionStatus_orig)(HANDLE CompletionPort, LPDWORD lpNumberOfBytesTransferred, PULONG_PTR lpCompletionKey, LPOVERLAPPED* lpOverlapped, DWORD dwMilliseconds);
static BOOL WINAPI GetQueuedCompletionStatus_hook(HANDLE CompletionPort, LPDWORD lpNumberOfBytesTransferred, PULONG_PTR lpCompletionKey, LPOVERLAPPED* lpOverlapped, DWORD dwMilliseconds)
{
	TpInfo("GetQueuedCompletionStatus_hook");
	bool result = GetQueuedCompletionStatus_orig(CompletionPort, lpNumberOfBytesTransferred, lpCompletionKey, lpOverlapped, dwMilliseconds);
	if (!result)
	{
		TpInfo("GetQueuedCompletionStatus failed");
		TpInfo("%ls", GetLastErrorAsString().c_str());
	}
	else {
		TpInfo("GetCompletionStatus success");
	}
	return result;
}

// this calls recvfrom in a while loop but with zero sleep means it takes up 100% of cpu core 1
static int(__stdcall* sub_54AFB0_orig)(SOCKET s, int a2, DWORD* p_n10054);
static int __stdcall sub_54AFB0_hook(SOCKET s, int a2, DWORD* p_n10054)
{
	Sleep(16);
	return sub_54AFB0_orig(s, a2, p_n10054);
}

static BOOL(__stdcall* original_PeekMessageA)(LPMSG lpMsg, HWND hWnd, UINT wMsgFilterMin, UINT wMsgFilterMax, UINT wRemoveMsg);
static BOOL WINAPI hook_PeekMessageA(LPMSG lpMsg, HWND hWnd, UINT wMsgFilterMin, UINT wMsgFilterMax, UINT wRemoveMsg)
{
	BOOL ret = original_PeekMessageA(lpMsg, hWnd, 0, 0, PM_REMOVE);

	if ((lpMsg->message == WM_KEYDOWN) || (lpMsg->message == WM_KEYUP) || ((lpMsg->message >= 512) && (lpMsg->message <= 521))) // remove keydown & keyup & mouse msgs
	{
		return FALSE;
	}
	return ret;
}

// Dongle hooks
namespace DIC_Dongle {
	enum GetCommands {
		RUN = 0x03000000,
		READ_FILE = 0x0000b000,
		PRODUCTION_TIME = 0x80030000,
		HARDWARE_SERIAL = 0x80040000,
		FACTORY_TIME = 0x80050000,
		COS_VERSION = 0x80060000,
		FILE_ID = 0x80110000,
		WRITE_DATA = 0x80170000,
		READ_DATA = 0x80180000,
		RUN_DATA = 0x80190000,
	};
}

static constexpr int dongleHandle = 1;
static constexpr int machineIdLength = 34;
static int __stdcall DIC_Open(int n32, char* src)
{
	// src is the name, but crazy speed doesn't set it
	TpInfo("DIC_Open");
	return 0;
}

static int __stdcall DIC_Close(unsigned int n32)
{
	TpInfo("DIC_Close");
	return 0;
}

static int __stdcall DIC_Create(int a1, int a2, int a3, int a4)
{
	TpInfo("DIC_Create");
	return 0xA010000A;
}

static int DIC_Find()
{
	return dongleHandle;
}

static std::string bufferToHexString(const unsigned char* buffer, size_t size, bool spaced = true) {
	static const char hexChars[] = "0123456789ABCDEF";
	std::string result;

	result.reserve(size * 3);

	for (size_t i = 0; i < size; i++) {
		result += hexChars[(buffer[i] >> 4) & 0x0F];
		result += hexChars[buffer[i] & 0x0F];

		if (spaced) {
			result += ' ';
		}

		if (spaced && (i + 1) % 16 == 0 && i + 1 < size) {
			result += '\n';
		}
	}

	return result;
}

static int __stdcall DIC_Set(int destBuffer, int commandId, int size, int n15, char* srcBuffer)
{
	TpInfo("DIC_Set commandId: %08x (size: %d)", commandId, size);
	if (srcBuffer != NULL)
	{
		TpInfo("Buffer valid");
		std::string out = bufferToHexString((unsigned char*)srcBuffer, size);
		TpInfo("%s", out.c_str());
	}

	return 0;
}

// note: the machine ID is a serial that's 34 characters long
// with 30 being the serial, and the other 4 being a crc32 hash of the serial
// although, cut in half because it's in hex
static int __stdcall DIC_Command(int handle, int commandId, int commandData)
{
	TpInfo("DIC_Command | Handle: %d, command id: %08x", handle, commandId);

	if (commandId == DIC_Dongle::READ_FILE)
	{
		*(DWORD*)commandData = machineIdLength + 1;
		memcpy((void*)(commandData + 2), "12345678901234567890123456789E190E", machineIdLength + 1);
		return 0;
	}

	if (commandData)
	{
		*(WORD*)commandData = 0;
	}
	return 0;
}

static __int16 __stdcall DIC_Get(unsigned __int16* a1, int commandId, int p_Value_1, char* Buffer)
{
	TpInfo("DIC_Get Command: %08x", commandId);
	switch (commandId)
	{
		// if this is wrong then it will throw a Unidentified Serial ID on boot and won't let you start
	case DIC_Dongle::READ_DATA:
	{
		//memcpy(Buffer, "12345678901234567890123456789E190E", machineIdLength + 1);
		int dataSize = *a1;
		memcpy(Buffer, a1 + 1, dataSize);
		return dataSize;
	}
	break;
	case DIC_Dongle::PRODUCTION_TIME:
		return 0x1234;
		break;
	case DIC_Dongle::HARDWARE_SERIAL:
		return 0x3456;
		break;
	case DIC_Dongle::FACTORY_TIME:
		return 0x9000;
		break;
	case DIC_Dongle::COS_VERSION:
		return 0x1234;
		break;
	case DIC_Dongle::RUN_DATA:
		// this needs us to copy 64 bytes of data... but what data?
		return 0x00;
		break;
	default:
		TpInfo("Unimplemented DIC_Get command: %08x", commandId);
		return 0;
	}

	return 0;
}

static std::string serverUrl = "tpme.local";
static void* (WSAAPI* gethostbyname_orig)(const char* name);
static void* WSAAPI gethostbyname_hook(const char* name)
{
	TpInfo("gethostbyname: %s", name);
	if (strstr(name, "croversea.i9yu.com") != NULL) {
		TpInfo("gethostbyname: server url found, replacing with %s", serverUrl.c_str());
		return gethostbyname_orig(serverUrl.c_str());
	}
	return gethostbyname_orig(name);
}

// server troubleshooting
static BOOL(WINAPI* SetEvent_orig)(HANDLE hEvent);
static BOOL WINAPI SetEvent_wrp(HANDLE hEvent)
{
	uint32_t ra = (uint32_t)_ReturnAddress();
	TpInfo("SetEvent called for %08x, return: %08x", hEvent, ra);
	return SetEvent_orig(hEvent);
}

static u_short(WINAPI* htonsOrig)(u_short hostshort);
static u_short WINAPI htonsHook(u_short hostshort)
{
	uint32_t ra = (uint32_t)_ReturnAddress();
	//TpInfo("htons: %u, %08x", hostshort, ra);
	if (ra == 0x007480e1) {
		return htonsOrig(21114);
	}
	return htonsOrig(hostshort);
}

// this will log into a text file but for some reason it doesn't do it sometimes?
// also seeing it in the cmd is easier.
static int(__thiscall* errorPrintOrig)(void* thisPtr, void* Src, size_t Size);
static int __fastcall errorPrintHook(void* thisPtr, void* edx, void* Src, size_t Size) {
	TpInfo("EVENTLOG: %s", Src);
	return errorPrintOrig(thisPtr, Src, Size);
}

// CARD READER STUFF
static LONG(WINAPI* SCardEstablishContext_orig)(
	DWORD dwScope,
	LPCVOID pvReserved1,
	LPCVOID pvReserved2,
	LPSCARDCONTEXT phContext
	);

static LONG WINAPI SCardEstablishContext_hook(
	DWORD dwScope,
	LPCVOID pvReserved1,
	LPCVOID pvReserved2,
	LPSCARDCONTEXT phContext
) {
	TpInfo("[Crazy Speed SCARD] SCardEstablishContext");
	if (phContext)
		*phContext = 0x12345678;

	return SCARD_S_SUCCESS;
}

static LONG(WINAPI* SCardListReaders_orig)(
	SCARDCONTEXT hContext,
	LPCSTR mszGroups,
	LPSTR mszReaders,
	LPDWORD pcchReaders
	);
static LONG WINAPI SCardListReaders_hook(
	SCARDCONTEXT hContext,
	LPCSTR mszGroups,
	LPSTR mszReaders,
	LPDWORD pcchReaders
) {
	TpInfo("[Crazy Speed SCARD] SCardListReaders");
	if (mszReaders)
	{
		strcpy(mszReaders, "CrazySpeedCardReaderForTp");
		*pcchReaders = 25;
	}
	return SCARD_S_SUCCESS;
}

static LONG WINAPI SCardReleaseContext_hook(
	SCARDCONTEXT hContext
) {
	TpInfo("[Crazy Speed SCARD] SCardReleaseContext");
	return SCARD_S_SUCCESS;
}

static LONG(WINAPI* SCardConnectA_orig)(
	SCARDCONTEXT hContext,
	LPCSTR szReader,
	DWORD dwShareMode,
	DWORD dwPreferredProtocols,
	LPSCARDHANDLE phCard,
	LPDWORD pdwActiveProtocol
	);
static LONG WINAPI SCardConnectA_hook(
	SCARDCONTEXT hContext,
	LPCSTR szReader,
	DWORD dwShareMode,
	DWORD dwPreferredProtocols,
	LPSCARDHANDLE phCard,
	LPDWORD pdwActiveProtocol
) {
	TpInfo("[Crazy Speed SCARD] SCardConnectA for reader: %s", szReader);
	if (phCard)
		*phCard = g_fakeCardHandle;

	if (pdwActiveProtocol)
		*pdwActiveProtocol = SCARD_PROTOCOL_T1;

	return SCARD_S_SUCCESS;
}

static int SCARDLastFileIndex = 0x00;
static LONG(WINAPI* SCardTransmit_orig)(
	SCARDHANDLE hCard,
	LPCSCARD_IO_REQUEST pioSendPci,
	LPCBYTE pbSendBuffer,
	DWORD cbSendLength,
	LPSCARD_IO_REQUEST pioRecvPci,
	LPBYTE pbRecvBuffer,
	LPDWORD pcbRecvLength
	);
static LONG WINAPI SCardTransmit_hook(
	SCARDHANDLE hCard,
	LPCSCARD_IO_REQUEST pioSendPci,
	LPCBYTE pbSendBuffer,
	DWORD cbSendLength,
	LPSCARD_IO_REQUEST pioRecvPci,
	LPBYTE pbRecvBuffer,
	LPDWORD pcbRecvLength
) {
	// Logging the command sent to the card for now
	char buffer[1024];
	int offset = 0;

	offset += sprintf(buffer, "Command: ");

	for (DWORD i = 0; i < cbSendLength && offset < 1000; i++) {
		offset += sprintf(buffer + offset, "%02X ", pbSendBuffer[i]);
	}

	TpInfo("[SCARD] %s", buffer);
#ifdef _DEBUG
	FILE* logFile = fopen("card_hook.log", "a");
	fprintf(logFile, "Command: ");
	for (DWORD i = 0; i < cbSendLength; i++) {
		fprintf(logFile, "%02X ", pbSendBuffer[i]);
	}
	fprintf(logFile, "\n");
	fclose(logFile);
#endif

	BYTE command = pbSendBuffer[1];
	if (command == 0x28)
	{
		pbRecvBuffer[0] = 0x90;
		pbRecvBuffer[1] = 0x00;
		*pcbRecvLength = 2;

		return SCARD_S_SUCCESS;
	}

	if (command == 0x24)
	{
		pbRecvBuffer[0] = 0x90;
		pbRecvBuffer[1] = 0x00;
		*pcbRecvLength = 2;

		return SCARD_S_SUCCESS;
	}

	if (pbSendBuffer[1] == 0x20 && pbSendBuffer[2] == 0x00 && pbSendBuffer[3] == 0x02) {
		char pin[9] = { 0 };
		for (int i = 0; i < 8; i++) {
			pin[i] = (char)pbSendBuffer[i + 5];
		}

		pbRecvBuffer[0] = 0x90;
		pbRecvBuffer[1] = 0x00;
		*pcbRecvLength = 2;

		return SCARD_S_SUCCESS;
	}

	if (command == 0x20) {
		pbRecvBuffer[0] = 0x90;
		pbRecvBuffer[1] = 0x00;
		*pcbRecvLength = 2;

		return SCARD_S_SUCCESS;
	}

	// Select file
	if (command == 0xA4) {
		pbRecvBuffer[0] = 0x9F;
		pbRecvBuffer[1] = 0x0F;
		*pcbRecvLength = 2;

		return SCARD_S_SUCCESS;
	}

	// READ BINARY
	// 0x1A, 0x00 == card has been renewed, this one is invalid, use the new one instead
	if (command == 0xB0) {
		const BYTE cardData[] = {
			// random test data
			0xC4, 0x01,
			//0x1A, 0x00,
			// Success status at the end
			0x90, 0x00
		};

		memcpy(pbRecvBuffer, cardData, sizeof(cardData));
		*pcbRecvLength = sizeof(cardData);

		return SCARD_S_SUCCESS;
	}

	// UPDATE BINARY
	if (command == 0xD6) {
		pbRecvBuffer[0] = 0x90;
		pbRecvBuffer[1] = 0x00;
		*pcbRecvLength = 2;

		return SCARD_S_SUCCESS;
	}

	return SCARD_S_SUCCESS;
}

static bool checkDLLCRC() {
	const char* dllPath = "dic32u.dll";
	const DWORD forbiddenCRC = 0xA9989470;

	if (GetFileAttributesA(dllPath) == INVALID_FILE_ATTRIBUTES) {
		return true;
	}

	std::ifstream file(dllPath, std::ios::binary);
	if (!file) {
		return true;
	}

	std::vector<char> buffer(0x400);
	file.read(buffer.data(), 0x400);

	if (file.gcount() < 0x400) {
		return true;
	}

	DWORD actualCRC = GetCRC32(buffer.data(), 0x400);
	
	TpInfo("DLL CRC: %08X", actualCRC);
	if (actualCRC == forbiddenCRC) {
		MessageBoxA(nullptr, "Incompatible DLL detected! dic32u.dll contains another loader. Please replace it with a valid dongle dll. The game will now exit.", "Error", MB_OK | MB_ICONERROR);
		exit(0);
		return false;
	}

	return true;
}

static InitFunction CrazySpeedFunc([]()
	{
		TpInfo("Crazy Speed is booting up\n");
		// Some dumps come with a dic32u.dll that's not the original dongle dll,
		// but rather some other loader. It will break the games IO board and thus makes our emu fail (no analog inputs)
		// so let's quit if its detected.

		checkDLLCRC();
		HMODULE dongleDll = LoadLibraryA("dic32u.dll");

		MH_Initialize();
		MH_CreateHookApi(L"kernel32.dll", "GetSystemInfo", &GetSystemInfo_hook, (void**)&GetSystemInfo_orig);
		//MH_CreateHookApi(L"kernel32.dll"), "SetEvent"), &SetEvent_wrp, (void**)&SetEvent_orig);
		//MH_CreateHook((void*)0x54afb0, sub_54AFB0_hook, (void**)&sub_54AFB0_orig);
		//MH_CreateHook((void*)0x5535ac, htonsHook, (void**)&htonsOrig);
#ifdef DEBUG
		MH_CreateHook((void*)0x7468a0, errorPrintHook, (void**)&errorPrintOrig);
#endif
		MH_CreateHookApi(L"user32.dll", "PeekMessageA", &hook_PeekMessageA, (void**)&original_PeekMessageA);
		MH_CreateHookApi(L"dic32u.dll", "DIC_Open", &DIC_Open, nullptr);
		MH_CreateHookApi(L"dic32u.dll", "DIC_Close", &DIC_Close, nullptr);
		MH_CreateHookApi(L"dic32u.dll", "DIC_Create", &DIC_Create, nullptr);
		MH_CreateHookApi(L"dic32u.dll", "DIC_Command", &DIC_Command, nullptr);
		MH_CreateHookApi(L"dic32u.dll", "DIC_Get", &DIC_Get, nullptr);
		MH_CreateHookApi(L"dic32u.dll", "DIC_Set", &DIC_Set, nullptr);
		MH_CreateHookApi(L"dic32u.dll", "DIC_Find", &DIC_Find, nullptr);
		MH_CreateHookApi(L"ws2_32.dll", "gethostbyname", gethostbyname_hook, (void**)&gethostbyname_orig);
		MH_CreateHookApi(L"winscard.dll", "SCardEstablishContext", SCardEstablishContext_hook, (void**)&SCardEstablishContext_orig);
		MH_CreateHookApi(L"winscard.dll", "SCardListReadersA", SCardListReaders_hook, (void**)&SCardListReaders_orig);
		MH_CreateHookApi(L"winscard.dll", "SCardReleaseContext", SCardReleaseContext_hook, nullptr);
		MH_CreateHookApi(L"winscard.dll", "SCardConnectA", SCardConnectA_hook, (void**)&SCardConnectA_orig);
		MH_CreateHookApi(L"winscard.dll", "SCardTransmit", SCardTransmit_hook, (void**)&SCardTransmit_orig);
		//MH_CreateHookApi(L"kernel32.dll"), "GetQueuedCompletionStatus"), &GetQueuedCompletionStatus_hook, (void**)&GetQueuedCompletionStatus_orig);
		MH_EnableHook(MH_ALL_HOOKS);

		iatHook("kernel32.dll", CreateFileWWrap, "CreateFileW");
		iatHook("kernel32.dll", ReadFileWrap, "ReadFile");
		iatHook("kernel32.dll", WriteFileWrap, "WriteFile");
		iatHook("kernel32.dll", ClearCommErrorWrap, "ClearCommError");
		iatHook("kernel32.dll", GetCommModemStatusWrap, "GetCommModemStatus");
		iatHook("kernel32.dll", CloseHandleWrap, "CloseHandle");

		injector::WriteMemory<BYTE>(0x47f356, 0, true); // this and the one below are for having the dongle decrypt the data
		injector::MakeNOP(0x47f49f, 2, true); // but we don't need it, as we stop the game from encrypting it in the first place
		injector::MakeRET(0x47f1d0); // RSA encryption for the data it sends to the dongle

		// these don't seem to be used?
		//injector::WriteMemory<BYTE>(0x4e738c, 0, true); // enable windowed mode
		//injector::WriteMemory<DWORD>(0x4e7399, 1920, true);
		//injector::WriteMemory<DWORD>(0x4e738e, 1080, true);

		// skip overlapped check for io board or else it resets our numberOfReadBytes 
		injector::MakeNOP(0x53d665, 0x17, true);
		injector::WriteMemory<BYTE>(0x53d67c, 0xEB, true);

		// server tests
		//injector::WriteMemory<BYTE>(0x8ab79b, 0x00); // don't send execproc request

		if (ToBool(config["General"]["Windowed"]))
		{
			injector::WriteMemory<DWORD>(0x4e6bef, 0x4e6c53, true); // start in windowed mode
		}

		// res tests, patching only the main res will render 3d fine, but ui won't scale, as is usual
		//injector::WriteMemory<DWORD>(0x4e6b6f, 1920, true);
		//injector::WriteMemory<DWORD>(0x4e6b6a, 1080, true);

		// so, to get the ui to scale we would need to find the SetViewport function on the GFxMovieRoot object
		// however, even with the SDK i was unable to find the function in the exe, cause there's a lot of stuff...
		// maybe something to check in the future, just putting this down here as a note :)
		// but if someone does find it, it should make the ui scale perfectly, simply because they used Scaleform GFx

		// for some reason sometimes GetQueuedCompletionStatus exits immediately instead of waiting or something, every time
		// and then all of the many many threads it spawns will spin, using 100% cpu across all cores on my 9800x3d... jesus christ.
		// let's inject a small sleep into the threads.
		// although, after some more runs this bug has disappeared, which is scary. keeping it in case someone else runs into this issue.
		// it does still hammer core 1 though.
		//injector::MakeJMP(0x746daa, WorkerThreadSleep, true);

	}, GameID::CrazySpeed);
#endif