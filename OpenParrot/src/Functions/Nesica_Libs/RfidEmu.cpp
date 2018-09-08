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
X2Type gameType;
static uint8_t g_jvsIOValues[64];
typedef BOOL(__stdcall *LPCloseHandle)(HANDLE hObject);
static LPCloseHandle __CloseHandle = NULL;

typedef BOOL(__stdcall *LPGetCommModemStatus)(HANDLE hFile, LPDWORD lpModemStat);
static LPGetCommModemStatus __GetCommModemStatus = NULL;

typedef BOOL(__stdcall *LPEscapeCommFunction)(HANDLE hFile, DWORD dwFunc);
static LPEscapeCommFunction __EscapeCommFunction = NULL;

typedef BOOL(__stdcall *LPClearCommError)(HANDLE hFile, LPDWORD lpErrors, LPCOMSTAT lpStat);
static LPClearCommError __ClearCommError = NULL;

typedef BOOL(__stdcall *LPSetupComm)(HANDLE hFile, DWORD dwInQueue, DWORD dwOutQueue);
static LPSetupComm  __SetupComm = NULL;

typedef BOOL(__stdcall *LPGetCommState)(HANDLE hFile, LPDCB lpDCB);
static LPGetCommState  __GetCommState = NULL;

typedef BOOL(__stdcall *LPSetCommState)(HANDLE hFile, LPDCB lpDCB);
static LPSetCommState  __SetCommState = NULL;

typedef BOOL(__stdcall *LPSetCommMask)(HANDLE hFile, DWORD dwEvtMask);
static LPSetCommMask  __SetCommMask = NULL;

typedef BOOL(__stdcall *LPGetCommTimeouts)(HANDLE hFile, LPCOMMTIMEOUTS lpCommTimeouts);
static LPGetCommTimeouts  __GetCommTimeouts = NULL;

typedef BOOL(__stdcall *LPSetCommTimeouts)(HANDLE hFile, LPCOMMTIMEOUTS lpCommTimeouts);
static LPSetCommTimeouts  __SetCommTimeouts = NULL;

typedef BOOL(__stdcall *LPWriteFile)(HANDLE hFile,
	LPVOID lpBuffer,
	DWORD nNumberOfBytesToWrite,
	LPDWORD lpNumberOfBytesWritten,
	LPOVERLAPPED lpOverlapped);
static LPWriteFile __WriteFile = NULL;
typedef BOOL(__stdcall *LPReadFile)(HANDLE hFile,
	LPVOID lpBuffer,
	DWORD nNumberOfBytesToRead,
	LPDWORD lpNumberOfBytesRead,
	LPOVERLAPPED lpOverlapped);
typedef BOOL(__stdcall *LPCloseHandle)(HANDLE hObject);
static LPReadFile __ReadFile = NULL;


typedef HANDLE(__stdcall *LPCreateFileA)(LPCSTR lpFileName,
	DWORD dwDesiredAccess,
	DWORD dwShareMode,
	LPSECURITY_ATTRIBUTES lpSecurityAttributes,
	DWORD dwCreationDisposition,
	DWORD dwFlagsAndAttributes,
	HANDLE hTemplateFile);
static LPCreateFileA  __CreateFileA = NULL;

typedef HANDLE(__stdcall *LPCreateFileW)(LPCWSTR lpFileName,
	DWORD dwDesiredAccess,
	DWORD dwShareMode,
	LPSECURITY_ATTRIBUTES lpSecurityAttributes,
	DWORD dwCreationDisposition,
	DWORD dwFlagsAndAttributes,
	HANDLE hTemplateFile);
static LPCreateFileW  __CreateFileW = NULL;

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
#if 0
		if (xpos == (statusaddr + 1)) {
			clear();
			return;
		}
		// codifica o tamanho da stream
		buffer[sizeaddr] = (BYTE)xpos - sizeaddr;

		// calcula o checksum
		DWORD sum = 0;
		for (DWORD i = sumaddr; i<xpos; i++) {
			sum += (DWORD)(buffer[i] & 0xFF);
		}
		sum &= 0xFF;
		if ((sum == 0xD0) || (sum == 0xE0)) {
			push(0xD0);
			push(sum - 1);
		}
		else
			push(sum);
		size_ = xpos;
#else
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
#endif
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
	if (gameType == X2Type::RFID)
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
	if(gameType == X2Type::Digital)
	{
		r->report(JVS_REPORT_OK);

		r->push(JVS_IOFUNC_SWINPUT);
		{
			r->push(JVS_SUPPORT_PLAYERS); // 2 players
			r->push(14); // 14 bot�es
			r->push(0); // null
		}
		r->push(JVS_IOFUNC_COINTYPE);
		{
			r->push(JVS_SUPPORT_SLOTS); // 2 slots
			r->push(0); // null
			r->push(0); // null
		}

		r->push(JVS_IOFUNC_EXITCODE);
		{
			r->push(0);
			r->push(0);
			r->push(0);
		}
	}
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

	if(gameType == X2Type::Digital)
	{
		if (g_jvsIOValues[24])
			r->push(0x80);
		else
			r->push(0);
	}
	else
	{
		r->push(0);
	}

	r->push(0);
	r->push(0);
	r->push(0);
	r->push(0);
	return 3;
}

int handle0x21(jprot_encoder *r)
{
	if(gameType == X2Type::RFID)
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
	int currstate = g_jvsIOValues[25];
	if (!coinstate[0] && (currstate)) {
		++p1coin;
	}
	coinstate[0] = currstate;

	currstate = g_jvsIOValues[26];
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

#define __XHOOKn(mod, name)	\
	MH_CreateHookApi(L ## mod, #name, &Hookz_##name, (void**)&__##name)

BOOL __stdcall Hookz_GetCommModemStatus(HANDLE hFile, LPDWORD lpModemStat)
{
	if (hFile != hConnection) {
		return __GetCommModemStatus(hFile, lpModemStat);
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
}
BOOL __stdcall Hookz_EscapeCommFunction(HANDLE hFile, DWORD dwFunc)
{
	if (hFile != hConnection) {
		return __EscapeCommFunction(hFile, dwFunc);
	}
	bool ret = true;// __EscapeCommFunction(hFile, dwFunc);
#ifdef LogRFID
	info(true, "EscapeCommFunction(hFile=%d, dwFunc='%08X') -> result=%08X", hFile, dwFunc, ret);
	info(true, "--------------------------------------------");
#endif
	return ret;
}

BOOL __stdcall Hookz_ClearCommError(HANDLE hFile, LPDWORD lpErrors, LPCOMSTAT lpStat)
{
	if (hFile != hConnection) {
		return __ClearCommError(hFile, lpErrors, lpStat);
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

BOOL __stdcall Hookz_SetupComm(HANDLE hFile, DWORD dwInQueue, DWORD dwOutQueue)
{

	if (hFile != hConnection) {
		return __SetupComm(hFile, dwInQueue, dwOutQueue);
	}
#ifdef LogRFID
	info(true, "SetupComm(hFile=%d, dwInQueue='%08X', dwOutQueue='%08X') -> result=%08X", hFile, dwInQueue, dwOutQueue, 1);
	info(true, "--------------------------------------------");
#endif
	return TRUE;
}
BOOL __stdcall Hookz_GetCommState(HANDLE hFile, LPDCB lpDCB)
{
	if (hFile != hConnection) {
		return __GetCommState(hFile, lpDCB);
	}
#ifdef LogRFID
	info(true, "GetCommState(hFile=%d, lpDCB=%p) -> result=%08X", hFile, lpDCB, 1);
	info(true, "--------------------------------------------");
#endif
	return TRUE;
}

BOOL __stdcall Hookz_SetCommState(HANDLE hFile, LPDCB lpDCB)
{
	if (hFile != hConnection) {
		return __SetCommState(hFile, lpDCB);
	}
#ifdef LogRFID
	info(true, "SetCommState(hFile=%d, lpDCB=%p) -> result=%08X", hFile, lpDCB, 1);
	info(true, "--------------------------------------------");
#endif
	return TRUE;
}

BOOL __stdcall Hookz_SetCommMask(HANDLE hFile, DWORD dwEvtMask)
{

	if (hFile != hConnection) {
		return __SetCommMask(hFile, dwEvtMask);
	}
#ifdef LogRFID
	info(true, "SetCommMask(hFile=%d, dwEvtMask='%08X') -> result=%08X", hFile, dwEvtMask, true);
	info(true, "--------------------------------------------");
#endif
	return TRUE;
}

BOOL __stdcall Hookz_GetCommTimeouts(HANDLE hFile, LPCOMMTIMEOUTS lpCommTimeouts)
{

	if (hFile != hConnection) {
		return __GetCommTimeouts(hFile, lpCommTimeouts);
	}
#ifdef LogRFID
	info(true, "GetCommTimeouts(hFile=%d, lpCommTimeouts=%p) -> result=%08X", hFile, lpCommTimeouts, TRUE);
	info(true, "--------------------------------------------");
#endif
	return TRUE;
}

BOOL __stdcall Hookz_SetCommTimeouts(HANDLE hFile, LPCOMMTIMEOUTS lpCommTimeouts)
{

	if (hFile != hConnection) {
		return __SetCommTimeouts(hFile, lpCommTimeouts);
	}
#ifdef LogRFID
	info(true, "SetCommTimeouts(hFile=%d, lpCommTimeouts=%p) -> result=%08X", hFile, lpCommTimeouts, TRUE);
	info(true, "--------------------------------------------");
#endif
	return TRUE;
}

BOOL __stdcall Hookz_WriteFile(HANDLE hFile,
	LPVOID lpBuffer,
	DWORD nNumberOfBytesToWrite,
	LPDWORD lpNumberOfBytesWritten,
	LPOVERLAPPED lpOverlapped)
{
	if (hFile != hConnection) {
		return __WriteFile(hFile, lpBuffer, nNumberOfBytesToWrite, lpNumberOfBytesWritten, lpOverlapped);
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

BOOL __stdcall  Hookz_ReadFile(HANDLE hFile,
	LPVOID lpBuffer,
	DWORD nNumberOfBytesToRead,
	LPDWORD lpNumberOfBytesRead,
	LPOVERLAPPED lpOverlapped)
{
	if (hFile != hConnection) {
		return __ReadFile(hFile, lpBuffer, nNumberOfBytesToRead, lpNumberOfBytesRead, lpOverlapped);
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

BOOL __stdcall Hookz_CloseHandle(HANDLE hObject)
{
	if (hObject != hConnection) {
		return __CloseHandle(hObject);
	}
#ifdef LogRFID
	info(true, "CloseHandle(hObject=%d) -> result=%08X", hObject, TRUE);
	info(true, "--------------------------------------------");
#endif
	return TRUE;
}
DWORD priority = 0;
HANDLE __stdcall Hookz_CreateFileA(LPCSTR lpFileName,
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

	return __CreateFileA(lpFileName,
		dwDesiredAccess,
		dwShareMode,
		lpSecurityAttributes,
		dwCreationDisposition,
		dwFlagsAndAttributes,
		hTemplateFile);
}

#define __XHOOKn(mod, name)	\
	MH_CreateHookApi(L ## mod, #name, &Hookz_##name, (void**)&__##name)

void init_RfidEmu(X2Type type)
{
	MH_Initialize();
	gameType = X2Type::RFID;
	__XHOOKn("kernel32.dll", CreateFileA);
	//__XHOOKn("kernel32.dll", CreateFileW);
	__XHOOKn("kernel32.dll", WriteFile);
	__XHOOKn("kernel32.dll", ReadFile);
	__XHOOKn("kernel32.dll", CloseHandle);
	__XHOOKn("kernel32.dll", GetCommModemStatus);
	__XHOOKn("kernel32.dll", EscapeCommFunction);
	__XHOOKn("kernel32.dll", ClearCommError);
	__XHOOKn("kernel32.dll", SetCommMask);
	__XHOOKn("kernel32.dll", SetupComm);
	__XHOOKn("kernel32.dll", GetCommState);
	__XHOOKn("kernel32.dll", SetCommState);
	__XHOOKn("kernel32.dll", SetCommTimeouts);
	__XHOOKn("kernel32.dll", GetCommTimeouts);
	MH_EnableHook(MH_ALL_HOOKS);
}