#include <StdInc.h>

// ORIGINALL BASED ON ttx_monitor, modified for RFID. https://github.com/zxmarcos/ttx_monitor

#include "Functions/Types.h"

#include "Utility/InitFunction.h"
#include <queue>

#define JVS_TRUE				0x01
#define JVS_FALSE				0x00
#define JVS_SYNC_CODE			0xE0
#define JVS_REPORT_CODE			0x01
#define JVS_COMMAND_REV			0x13
#define JVS_BOARD_REV			0x30
#define JVS_COMM_REV			0x10
#define JVS_ADDR_MASTER			0x00
#define JVS_STATUS_OK			0x01
#define JVS_STATUS_UNKNOWN_CMD	0x02
#define JVS_STATUS_SUM_ERROR	0x03
#define JVS_STATUS_ACK_OVERFLOW	0x04
#define JVS_REPORT_OK			0x01
#define JVS_REPORT_INVAL_PARAM1	0x02
#define JVS_REPORT_INVAL_PARAM2	0x03
#define JVS_REPORT_BUSY			0x04
#define JVS_STREAM_SIZE			1024

#define JVS_OP_RESET			0xF0
#define JVS_OP_ADDRESS			0xF1
#define JVS_OP_CHECKSUM_ERROR	0x2F

#define JVS_IOFUNC_SWINPUT		0x01
#define JVS_IOFUNC_COINTYPE		0x02
#define JVS_IOFUNC_EXITCODE		0x00

#define JVS_SUPPORT_PLAYERS		2
#define JVS_SUPPORT_SLOTS		2

#define LOG_COMMANDS	1
#if LOG_COMMANDS
#define logcmd	logmsg
#else
#define logcmd(str, __VA_LIST__)
#endif

std::queue<BYTE> replyBuffer;
typedef signed char SINT8;
typedef signed short SINT16;
typedef signed short SINT32;
typedef unsigned char UINT8;
typedef unsigned short UINT16;
HANDLE hConnection = (HANDLE)0x1337;

static const char *Rfid_IO_Id = "TAITO CORP.;RFID CTRL P.C.B.;Ver1.00;";

void logmsg(const char *format, ...)
{

}

struct jvs_command_def {
	UINT8 params;
	UINT8 reports;
};

#define __ARG__(n)	((DWORD) pfunc[n])

class jprot_encoder {
	BYTE buffer[1024];
	BYTE *ptr;
	DWORD statusaddr;
	DWORD sizeaddr;
	DWORD sumaddr;
	DWORD startaddr;
	DWORD xpos;
	DWORD size_;
	DWORD nreplys;
public:
	DWORD size() {
		return size_;
	}
	jprot_encoder() {
		memset(buffer, 0, 1024);
		xpos = 0;
	}
	~jprot_encoder() {
		memset(buffer, 0, 1024);
		xpos = 0;
	}

	void addreply() {
		nreplys++;
	}


	void set_status(BYTE v) {
		buffer[statusaddr] = v;
	}
	void clear()
	{
		memset(buffer, 0, 1024);
		size_ = 0;
		xpos = 0;
	}
	void begin_stream() {

		pushz(JVS_SYNC_CODE);
		sumaddr = xpos;
		pushz(JVS_ADDR_MASTER);
		sizeaddr = xpos;
		push(0);
		statusaddr = xpos;
		// STATUS
		push(JVS_STATUS_OK);

	}

	void pushz(BYTE v) {
		buffer[xpos++] = v;
	}

	void push(BYTE v) {
#if 0
		buffer[xpos] = v;
		++xpos;
#else
		if (v == 0xD0) {
			buffer[xpos++] = 0xD0;
			buffer[xpos++] = 0xCF;
		}
		else
			if (v == 0xE0) {
				buffer[xpos++] = 0xD0;
				buffer[xpos++] = 0xDF;
			}
			else
				buffer[xpos++] = v;
#endif
	}
	void report(BYTE v) {
		push(v);
	}
	void end_stream() {
		if (xpos == (statusaddr + 1)) {
			clear();
			return;
		}

		DWORD sizeK = 0;
		for (DWORD i = sizeaddr; i<xpos; i++)
			if (buffer[i] != 0xD0)
				++sizeK;
		// codifica o tamanho da stream
		buffer[sizeaddr] = sizeK;

		// calcula o checksum
		DWORD sum = 0;
		for (DWORD i = sumaddr, inc = 0; i<xpos; i++) {
			if (buffer[i] == 0xD0) {
				inc = 1;
			}
			else {
				sum += (DWORD)((buffer[i] + inc) & 0xFF);
				if (inc)
					inc = 0;
			}
		}
		sum &= 0xFF;
		push(sum);
		size_ = xpos;
	}


	void read(BYTE *dst, DWORD size) {
		if (size > size_)
			size = size_;
		memcpy(dst, &buffer[0], size);
	}

	void print()
	{
		if (size()) {
#ifdef _DEBUG
			logmsg("WR:  ");
			for (DWORD i = 0; i<size(); i++)
				logmsg("%02X ", buffer[i]);
			logmsg("\n");
#endif
		}
	}
};

static int isAddressed = 0;
int is_addressed() {
	return isAddressed;
}
void reset_addressed()
{
	isAddressed = 0;
}

static WORD p1coin = 0;
static WORD p2coin = 0;
static int coinstate[2] = { 0, 0 };

int handle0xF0()
{
	p1coin = 0;
	p2coin = 0;
	return 2;
}

int handle0xF1(jprot_encoder *r)
{
	r->report(JVS_REPORT_OK);
	isAddressed = 1;

	return 2;
}

int handle0x2F()
{
	return 1;
}

int handle0x10(jprot_encoder *r)
{
	const char *str = NULL;
	r->report(JVS_REPORT_OK);
	//if(gameType == X2Type::RFID)
	str = Rfid_IO_Id;
	//else if(gameType == X2Type::Digital)
	//	str = IO_Id;
	while (*str) { r->push(*str++); }
	r->push(0);
	return 1;
}

int handle0x11(jprot_encoder *r)
{
	r->report(JVS_REPORT_OK);
	r->push(JVS_COMMAND_REV);
	return 1;
}

int handle0x12(jprot_encoder *r)
{
	r->report(JVS_REPORT_OK);
	r->push(JVS_BOARD_REV);
	return 1;
}

int handle0x13(jprot_encoder *r)
{
	r->report(JVS_REPORT_OK);
	r->push(JVS_COMM_REV);
	return 1;
}

int handle0x14(jprot_encoder *r)
{
	r->report(JVS_REPORT_OK);
	r->push(1);
	r->push(7);
	r->push(0);
	r->push(8);
	r->push(0);
	r->push(0x12);
	r->push(8);
	r->push(0);
	r->push(0);
	r->push(0);
	return 1;
}

int handle0x01(jprot_encoder *r)
{
	r->report(JVS_REPORT_OK);
	r->push(1);
	r->push(0);
	return 2;
}

int handle0x20(jprot_encoder *r)
{
	r->report(JVS_REPORT_OK);
	r->push(0);
	r->push(0);
	r->push(0);
	r->push(0);
	r->push(0);
	return 3;
}

int handle0x21(jprot_encoder *r)
{
	int currstate = 0;// inputMgr.GetState(P1_COIN);
	if (!coinstate[0] && (currstate)) {
		++p1coin;
	}
	coinstate[0] = currstate;

	currstate = 0;// inputMgr.GetState(P2_COIN);
	if (!coinstate[1] && (currstate)) {
		++p2coin;

	}
	coinstate[1] = currstate;

	//logcmd("Lendo o ficheiro... \n");
	r->report(JVS_REPORT_OK);
	r->push(p1coin >> 8);
	r->push(p1coin & 0xFF);
	r->push(p2coin >> 8);
	r->push(p2coin & 0xFF);
	return 2;
}

int handle0x30(jprot_encoder *r, DWORD arg1, DWORD arg2, DWORD arg3)
{
	WORD val = ((arg2 & 0xFF) << 8) | (arg3 & 0xFF);
	r->report(JVS_REPORT_OK);
#ifdef _DEBUG
	logcmd("-coin %d, %d\n", arg1, val);
#endif
	switch (arg1)
	{
	case 1:
		if (val > p1coin)
			p1coin = 0;
		else
			p1coin -= val;
		break;
	case 2:
		if (val > p2coin)
			p2coin = 0;
		else
			p2coin -= val;
	}
	return 4;
}

int handle0x31(jprot_encoder *r, DWORD arg1, DWORD arg2, DWORD arg3)
{
	WORD val = ((arg2 & 0xFF) << 8) | (arg3 & 0xFF);
	r->report(JVS_REPORT_OK);
#ifdef _DEBUG
	logcmd("+coin %d, %d\n", arg1, val);
#endif
	switch (arg1)
	{
	case 1: p1coin += val; break;
	case 2: p2coin += val; break;
	}
	return 4;
}

unsigned long process_stream(unsigned char *stream, unsigned long srcsize, unsigned char *dst, unsigned long dstsize)
{
	jprot_encoder r;
	BYTE *pstr = stream;
	BYTE *pfunc = NULL;
	BYTE node = 0;
	BYTE pktsize = 0;
	int pktcount = 0;
	WORD pktaddr = 0;

	r.clear();


	if (pstr[0] != JVS_SYNC_CODE) {
#ifdef _DEBUG
		logmsg("Invalid Sync code!\n");
#endif
	}
	node = pstr[1];
	pktsize = pstr[2];
	pfunc = &pstr[3];

	pktcount = (int)pktsize - 1;

	r.begin_stream();

	while (pktcount > 0)
	{
		int increment = 0;
		switch (pfunc[0] & 0xFF)
		{
		case 0xF0:
			increment = handle0xF0();
			break;
		case 0xF1:
			increment = handle0xF1(&r);
			break;
		case 0x2F:
			increment = handle0x2F();
			break;
		case 0x10:
			increment = handle0x10(&r);
			break;
		case 0x11:
			increment = handle0x11(&r);
			break;
		case 0x12:
			increment = handle0x12(&r);
			break;
		case 0x13:
			increment = handle0x13(&r);
			break;
		case 0x14:
			increment = handle0x14(&r);
			break;
		case 0x01:
			increment = handle0x01(&r);
			break;
		case 0x20:
			increment = handle0x20(&r);
			break;
		case 0x21:
			increment = handle0x21(&r);
			break;
		case 0x30:
			increment = handle0x30(&r, __ARG__(1), __ARG__(2), __ARG__(3));
			break;

		case 0x31:
			increment = handle0x31(&r, __ARG__(1), __ARG__(2), __ARG__(3));
			break;
		default:
#ifdef _DEBUG
			logcmd("Unknown command %X\n", __ARG__(0));
#endif
			r.report(JVS_REPORT_OK);
			increment = 1;
			break;
		}

		pktcount -= increment;
		pfunc += increment;
	}

	r.set_status(JVS_STATUS_OK);
	r.end_stream();
	r.read(dst, dstsize);
	return r.size();
}
BOOL(__stdcall *g_origGetCommModemStatus)(HANDLE hFile, LPDWORD lpModemStat);
BOOL __stdcall GetCommModemStatusWrap(HANDLE hFile, LPDWORD lpModemStat)
{
	if (hFile != hConnection) {
		return g_origGetCommModemStatus(hFile, lpModemStat);
	}
	if (is_addressed())
		*lpModemStat = 0x10;
	else
		*lpModemStat = 0;
#ifdef LogRFID
	info(true, "GetCommModemStatus(hFile=%d, lpModemStat=%p) -> result=%08X", hFile, lpModemStat, TRUE);
	info(true, "--------------------------------------------");
#endif
	return TRUE;
}BOOL(__stdcall *g_origEscapeCommFunction)(HANDLE hFile, DWORD dwFunc);
BOOL __stdcall EscapeCommFunctionWrap(HANDLE hFile, DWORD dwFunc)
{
	if (hFile != hConnection) {
		return g_origEscapeCommFunction(hFile, dwFunc);
	}
	bool ret = true;// __EscapeCommFunction(hFile, dwFunc);
#ifdef LogRFID
	info(true, "EscapeCommFunction(hFile=%d, dwFunc='%08X') -> result=%08X", hFile, dwFunc, ret);
	info(true, "--------------------------------------------");
#endif
	return ret;
}

BOOL(__stdcall *g_origClearCommError)(HANDLE hFile, LPDWORD lpErrors, LPCOMSTAT lpStat);
BOOL __stdcall ClearCommErrorWrap(HANDLE hFile, LPDWORD lpErrors, LPCOMSTAT lpStat)
{
	if (hFile != hConnection) {
		return g_origClearCommError(hFile, lpErrors, lpStat);
	}
#ifdef LogRFID
	info(true, "ClearCommError(hFile=%d, lpErrors=%p, lpStat=%p) -> result=%08X", hFile, lpErrors, lpStat, 1);
	info(true, "--------------------------------------------");
#endif
	if (lpStat) {
		if (!replyBuffer.empty())
			lpStat->cbInQue = replyBuffer.size();
		else
			lpStat->cbInQue = 0;
	}
	return true;
}

BOOL(__stdcall *g_origSetupComm)(HANDLE hFile, DWORD dwInQueue, DWORD dwOutQueue);
BOOL __stdcall SetupCommWrap(HANDLE hFile, DWORD dwInQueue, DWORD dwOutQueue)
{

	if (hFile != hConnection) {
		return g_origSetupComm(hFile, dwInQueue, dwOutQueue);
	}
#ifdef LogRFID
	info(true, "SetupComm(hFile=%d, dwInQueue='%08X', dwOutQueue='%08X') -> result=%08X", hFile, dwInQueue, dwOutQueue, 1);
	info(true, "--------------------------------------------");
#endif
	return TRUE;
}

BOOL(__stdcall *g_origGetCommState)(HANDLE hFile, LPDCB lpDCB);
BOOL __stdcall GetCommStateWrap(HANDLE hFile, LPDCB lpDCB)
{
	if (hFile != hConnection) {
		return g_origGetCommState(hFile, lpDCB);
	}
#ifdef LogRFID
	info(true, "GetCommState(hFile=%d, lpDCB=%p) -> result=%08X", hFile, lpDCB, 1);
	info(true, "--------------------------------------------");
#endif
	return TRUE;
}

BOOL(__stdcall *g_origSetCommState)(HANDLE hFile, LPDCB lpDCB);
BOOL __stdcall SetCommStateWrap(HANDLE hFile, LPDCB lpDCB)
{
	if (hFile != hConnection) {
		return g_origSetCommState(hFile, lpDCB);
	}
#ifdef LogRFID
	info(true, "SetCommState(hFile=%d, lpDCB=%p) -> result=%08X", hFile, lpDCB, 1);
	info(true, "--------------------------------------------");
#endif
	return TRUE;
}

BOOL(__stdcall *g_origSetCommMask)(HANDLE hFile, DWORD dwEvtMask);
BOOL __stdcall SetCommMaskWrap(HANDLE hFile, DWORD dwEvtMask)
{

	if (hFile != hConnection) {
		return g_origSetCommMask(hFile, dwEvtMask);
	}
#ifdef LogRFID
	info(true, "SetCommMask(hFile=%d, dwEvtMask='%08X') -> result=%08X", hFile, dwEvtMask, true);
	info(true, "--------------------------------------------");
#endif
	return TRUE;
}

BOOL(__stdcall *g_origGetCommTimeouts)(HANDLE hFile, LPCOMMTIMEOUTS lpCommTimeouts);
BOOL __stdcall GetCommTimeoutsWrap(HANDLE hFile, LPCOMMTIMEOUTS lpCommTimeouts)
{

	if (hFile != hConnection) {
		return g_origGetCommTimeouts(hFile, lpCommTimeouts);
	}
#ifdef LogRFID
	info(true, "GetCommTimeouts(hFile=%d, lpCommTimeouts=%p) -> result=%08X", hFile, lpCommTimeouts, TRUE);
	info(true, "--------------------------------------------");
#endif
	return TRUE;
}

BOOL(__stdcall *g_origSetCommTimeouts)(HANDLE hFile, LPCOMMTIMEOUTS lpCommTimeouts);
BOOL __stdcall SetCommTimeoutsWrap(HANDLE hFile, LPCOMMTIMEOUTS lpCommTimeouts)
{

	if (hFile != hConnection) {
		return g_origSetCommTimeouts(hFile, lpCommTimeouts);
	}
#ifdef LogRFID
	info(true, "SetCommTimeouts(hFile=%d, lpCommTimeouts=%p) -> result=%08X", hFile, lpCommTimeouts, TRUE);
	info(true, "--------------------------------------------");
#endif
	return TRUE;
}

BOOL(__stdcall *g_origWriteFile)(HANDLE hFile,
	LPVOID lpBuffer,
	DWORD nNumberOfBytesToWrite,
	LPDWORD lpNumberOfBytesWritten,
	LPOVERLAPPED lpOverlapped);
BOOL __stdcall WriteFileWrap(HANDLE hFile,
	LPVOID lpBuffer,
	DWORD nNumberOfBytesToWrite,
	LPDWORD lpNumberOfBytesWritten,
	LPOVERLAPPED lpOverlapped)
{
	if (hFile != hConnection) {
		return g_origWriteFile(hFile, lpBuffer, nNumberOfBytesToWrite, lpNumberOfBytesWritten, lpOverlapped);
	}
	static BYTE rbuffer[1024];

	DWORD sz = process_stream((LPBYTE)lpBuffer, nNumberOfBytesToWrite, rbuffer, 1024);
	if (sz != 1) {
		for (DWORD i = 0; i < sz; i++)
		{
			replyBuffer.push(rbuffer[i]);
		}
	}

#ifdef LogRFID
	info(true, "");
	info(true, "--------------------------------------------");
#endif
	return TRUE;
}

BOOL(__stdcall *g_origReadFile)(HANDLE hFile,
	LPVOID lpBuffer,
	DWORD nNumberOfBytesToRead,
	LPDWORD lpNumberOfBytesRead,
	LPOVERLAPPED lpOverlapped);
BOOL __stdcall  ReadFileWrap(HANDLE hFile,
	LPVOID lpBuffer,
	DWORD nNumberOfBytesToRead,
	LPDWORD lpNumberOfBytesRead,
	LPOVERLAPPED lpOverlapped)
{
	if (hFile != hConnection) {
		return g_origReadFile(hFile, lpBuffer, nNumberOfBytesToRead, lpNumberOfBytesRead, lpOverlapped);
	}

#ifdef LogRFID
	info(true, "ReadFile(hFile=%d, lpBuffer='%08X', nNumberOfBytesToRead=%08X) -> result=%08X", hFile, lpBuffer, nNumberOfBytesToRead, TRUE);
#endif
	if (replyBuffer.size())
	{
		if (nNumberOfBytesToRead >= replyBuffer.size())
			nNumberOfBytesToRead = replyBuffer.size();
		*lpNumberOfBytesRead = nNumberOfBytesToRead;
		BYTE *ptr = (BYTE*)lpBuffer;
		for (DWORD i = 0; i < nNumberOfBytesToRead; i++) {
			if (!replyBuffer.empty()) {
				*ptr++ = replyBuffer.front();
				replyBuffer.pop();
			}
			else
				*ptr++ = 0;
		}
	}
	else {
		*lpNumberOfBytesRead = 0;
		return TRUE;
	}
#ifdef LogRFID
	info(true, "");
	info(true, "--------------------------------------------");
#endif
	return TRUE;
}

BOOL(__stdcall *g_origCloseHandle)(HANDLE hObject);
BOOL __stdcall CloseHandleWrap(HANDLE hObject)
{
	if (hObject != hConnection) {
		return g_origCloseHandle(hObject);
	}
#ifdef LogRFID
	info(true, "CloseHandle(hObject=%d) -> result=%08X", hObject, TRUE);
	info(true, "--------------------------------------------");
#endif
	return TRUE;
}

static char moveBuf[256];
LPCSTR ParseFileNamesA(LPCSTR lpFileName)
{
	if (!strncmp(lpFileName, "D:", 2) || !strncmp(lpFileName, "d:", 2))
	{
		memset(moveBuf, 0, 256);
		if(lpFileName[2] == '\\' || lpFileName[2] == '/')
		{
			sprintf(moveBuf, ".\\OpenParrot\\%s", lpFileName + 3);
		}
		else
		{
			// Magical Beat has d: WTF?
			sprintf(moveBuf, ".\\OpenParrot\\%s", lpFileName + 2);
		}
		return moveBuf;
	}

	return lpFileName;
}

static wchar_t moveBufW[256];
LPCWSTR ParseFileNamesW(LPCWSTR lpFileName)
{
	if (!wcsncmp(lpFileName, L"D:", 2) || !wcsncmp(lpFileName, L"d:", 2))
	{
		memset(moveBuf, 0, 256);
		if (lpFileName[2] == '\\' || lpFileName[2] == '/')
		{
			swprintf(moveBufW, L".\\OpenParrot\\%ls", lpFileName + 3);
		}
		else
		{
			// Magical Beat has d: WTF?
			swprintf(moveBufW, L".\\OpenParrot\\%ls", lpFileName + 2);
		}
		return moveBufW;
	}

	return lpFileName;
}

DWORD priority = 0;
HANDLE(__stdcall *g_origCreateFileA)(LPCSTR lpFileName,
	DWORD dwDesiredAccess,
	DWORD dwShareMode,
	LPSECURITY_ATTRIBUTES lpSecurityAttributes,
	DWORD dwCreationDisposition,
	DWORD dwFlagsAndAttributes,
	HANDLE hTemplateFile);
HANDLE __stdcall CreateFileAWrap(LPCSTR lpFileName,
	DWORD dwDesiredAccess,
	DWORD dwShareMode,
	LPSECURITY_ATTRIBUTES lpSecurityAttributes,
	DWORD dwCreationDisposition,
	DWORD dwFlagsAndAttributes,
	HANDLE hTemplateFile)
{
	if (!strcmp(lpFileName, "COM2"))
	{
#ifdef LogRFID
		info(true, "CreateFile(lpFileName=%s) -> result=%08X", lpFileName, hConnection);
		info(true, "--------------------------------------------");
#endif
		return hConnection;
	}

	return g_origCreateFileA(ParseFileNamesA(lpFileName),
		dwDesiredAccess,
		dwShareMode,
		lpSecurityAttributes,
		dwCreationDisposition,
		dwFlagsAndAttributes,
		hTemplateFile);
}

HANDLE(__stdcall *g_origCreateFileW)(LPCWSTR lpFileName,
	DWORD dwDesiredAccess,
	DWORD dwShareMode,
	LPSECURITY_ATTRIBUTES lpSecurityAttributes,
	DWORD dwCreationDisposition,
	DWORD dwFlagsAndAttributes,
	HANDLE hTemplateFile);
HANDLE __stdcall CreateFileWWrap(LPCWSTR lpFileName,
	DWORD dwDesiredAccess,
	DWORD dwShareMode,
	LPSECURITY_ATTRIBUTES lpSecurityAttributes,
	DWORD dwCreationDisposition,
	DWORD dwFlagsAndAttributes,
	HANDLE hTemplateFile)
{
	if (!wcscmp(lpFileName, L"COM2"))
	{
#ifdef LogRFID
		info(true, "CreateFile(lpFileName=%s) -> result=%08X", lpFileName, hConnection);
		info(true, "--------------------------------------------");
#endif
		return hConnection;
	}

	return g_origCreateFileW(ParseFileNamesW(lpFileName),
		dwDesiredAccess,
		dwShareMode,
		lpSecurityAttributes,
		dwCreationDisposition,
		dwFlagsAndAttributes,
		hTemplateFile);
}

static DWORD(__stdcall *g_origGetFileAttributesA)(LPCSTR lpFileName);
static DWORD __stdcall GetFileAttributesAWrap(LPCSTR lpFileName)
{
	return g_origGetFileAttributesA(ParseFileNamesA(lpFileName));
}

static DWORD(__stdcall *g_origGetFileAttributesW)(LPCWSTR lpFileName);
static DWORD __stdcall GetFileAttributesWWrap(LPCWSTR lpFileName)
{
	return g_origGetFileAttributesW(ParseFileNamesW(lpFileName));
}

static BOOL(__stdcall *g_origCreateDirectoryA)(LPCSTR lpPathName, LPSECURITY_ATTRIBUTES lpSecurityAttributes);
static BOOL __stdcall CreateDirectoryAWrap(
	LPCSTR                lpPathName,
	LPSECURITY_ATTRIBUTES lpSecurityAttributes
)
{
	return g_origCreateDirectoryA(ParseFileNamesA(lpPathName), lpSecurityAttributes);
}

static BOOL(__stdcall *g_origCreateDirectoryW)(LPCWSTR lpPathName, LPSECURITY_ATTRIBUTES lpSecurityAttributes);
static BOOL __stdcall CreateDirectoryWWrap(
	LPCWSTR                lpPathName,
	LPSECURITY_ATTRIBUTES lpSecurityAttributes
)
{
	return g_origCreateDirectoryW(ParseFileNamesW(lpPathName), lpSecurityAttributes);
}

void init_RfidEmu()
{
	MH_Initialize();
	CreateDirectoryA("OpenParrot", nullptr);
	MH_CreateHookApi(L"kernel32.dll", "CreateDirectoryA", CreateDirectoryAWrap, (void**)&g_origCreateDirectoryA);
	MH_CreateHookApi(L"kernel32.dll", "CreateDirectoryW", CreateDirectoryWWrap, (void**)&g_origCreateDirectoryW);
	MH_CreateHookApi(L"kernel32.dll", "GetFileAttributesA", GetFileAttributesAWrap, (void**)&g_origGetFileAttributesA);
	MH_CreateHookApi(L"kernel32.dll", "GetFileAttributesW", GetFileAttributesWWrap, (void**)&g_origGetFileAttributesW);
	MH_CreateHookApi(L"kernel32.dll", "CreateFileW", CreateFileWWrap, (void**)&g_origCreateFileW);
	MH_CreateHookApi(L"kernel32.dll", "CreateFileA", CreateFileAWrap, (void**)&g_origCreateFileA);
	MH_CreateHookApi(L"kernel32.dll", "WriteFile", WriteFileWrap, (void**)&g_origWriteFile);
	MH_CreateHookApi(L"kernel32.dll", "ReadFile", ReadFileWrap, (void**)&g_origReadFile);
	MH_CreateHookApi(L"kernel32.dll", "CloseHandle", CloseHandleWrap, (void**)&g_origCloseHandle);
	MH_CreateHookApi(L"kernel32.dll", "GetCommModemStatus", GetCommModemStatusWrap, (void**)&g_origGetCommModemStatus);
	MH_CreateHookApi(L"kernel32.dll", "EscapeCommFunction", EscapeCommFunctionWrap, (void**)&g_origEscapeCommFunction);
	MH_CreateHookApi(L"kernel32.dll", "ClearCommError", ClearCommErrorWrap, (void**)&g_origClearCommError);
	MH_CreateHookApi(L"kernel32.dll", "SetCommMask", SetCommMaskWrap, (void**)&g_origSetCommMask);
	MH_CreateHookApi(L"kernel32.dll", "SetupComm", SetupCommWrap, (void**)&g_origSetupComm);
	MH_CreateHookApi(L"kernel32.dll", "GetCommState", GetCommStateWrap, (void**)&g_origGetCommState);
	MH_CreateHookApi(L"kernel32.dll", "SetCommState", SetCommStateWrap, (void**)&g_origSetCommState);
	MH_CreateHookApi(L"kernel32.dll", "SetCommTimeouts", SetCommTimeoutsWrap, (void**)&g_origSetCommTimeouts);
	MH_CreateHookApi(L"kernel32.dll", "GetCommTimeouts", GetCommTimeoutsWrap, (void**)&g_origGetCommTimeouts);
	MH_EnableHook(MH_ALL_HOOKS);
}