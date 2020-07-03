#include <StdInc.h>

// ORIGINALL BASED ON ttx_monitor, modified for RFID. https://github.com/zxmarcos/ttx_monitor

#include "Utility/InitFunction.h"
#include <queue>
#include "Utility/GameDetect.h"

using namespace std::string_literals;

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
		// encode the size of the stream
		buffer[sizeaddr] = sizeK;

		// calculate the checksum
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

	void printReply()
	{
		static char printer[1024];
		memset(printer, 0, 1024);
		if (size()) {
#ifdef PrintRFIDReplies
			sprintf(printer, "R:");
			for (DWORD i = 0; i<size(); i++)
			{
				sprintf(printer + 2 + (i * 3), "%02X ", buffer[i]);
			}
			info(true, printer);
#endif
		}
	}

	void printSource(BYTE* srcbuffer, int strsize)
	{
		static char printer[1024];
		memset(printer, 0, 1024);
		if (strsize) {
#ifdef PrintRFIDRequests
			sprintf(printer, "S:");
			for (DWORD i = 0; i<strsize; i++)
			{
				sprintf(printer + 2 + (i * 3), "%02X ", srcbuffer[i]);
			}
			info(true, printer);
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

int handleBusReset()
{
	p1coin = 0;
	p2coin = 0;
	return 2;
}

// 0xF1 -- set address
int handleSetAddress(jprot_encoder *r)
{
	r->report(JVS_REPORT_OK);
	isAddressed = 1;

	return 2;
}

static bool cardInserted = false;

// 0x26 -- read general-purpose input
int handleReadGeneralPurposeInput(jprot_encoder *r, DWORD arg1)
{
	r->report(JVS_REPORT_OK);
	for(DWORD i = 0; i < arg1; i++)
	{
		if (cardInserted)
			r->push(0x19); // This should be only injected with first package of the 3, but does not seem to care.
		else
			r->push(0);
	}
	return 2 + arg1;
}

// Dumped from my own Japanese Lord Vermilion Nesica XLive card -Reaver
static char cardData[0x18] = { 0x04, 0xC2, 0x3D, 0xDA, 0x6F, 0x52, 0x80, 0x00, 0x37, 0x30, 0x32, 0x30, 0x33, 0x39, 0x32, 0x30, 0x31, 0x30, 0x32, 0x38, 0x31, 0x35, 0x30, 0x32 };


// 0x32 -- read general-purpose output. This is very confusing 0x32 0x01 0x00 returns 0x01 (0x18 times 0x00) 0x01
// See JVSP manual for more information.
int handleReadGeneralPurposeOutput(jprot_encoder *r, DWORD arg1)
{
#ifdef _DEBUG
	//OutputDebugStringA("Requested card data!");
#endif
	r->report(JVS_REPORT_OK);
	if(cardInserted)
	{
		for(int i = 0; i < 0x18; i++)
		{
			r->push(cardData[i]);
		}
	}
	else
	{
		for (DWORD i = 0; i < arg1 * 0x18; i++)
		{
			r->push(0);
		}
	}
	r->report(JVS_REPORT_OK);
	return 2 + arg1;
}

int handleReTransmitDataInCaseOfChecksumFailure()
{
	return 1;
}

int handleReadIDData(jprot_encoder *r)
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

int handleGetCommandFormatVersion(jprot_encoder *r)
{
	r->report(JVS_REPORT_OK);
	r->push(JVS_COMMAND_REV);
	return 1;
}

int handleGetJVSVersion(jprot_encoder *r)
{
	r->report(JVS_REPORT_OK);
	r->push(JVS_BOARD_REV);
	return 1;
}

int handleGetCommunicationVersion(jprot_encoder *r)
{
	r->report(JVS_REPORT_OK);
	r->push(JVS_COMM_REV);
	return 1;
}

int handleGetSlaveFeatures(jprot_encoder *r)
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

int handleTaito01Call(jprot_encoder *r, DWORD arg1)
{
	r->report(JVS_REPORT_OK); 
	r->push(1);
	return 2;
}

int handleTaito03Call(jprot_encoder *r)
{
	r->report(JVS_REPORT_OK);
	r->push(1);
	return 2;
}

int handleTaito04Call(jprot_encoder *r)
{
	r->report(JVS_REPORT_OK);
	return 1;
}

int handleTaito05Call(jprot_encoder *r)
{
	r->report(JVS_REPORT_OK);
	return 3;
}

int handleReadSwitchInputs(jprot_encoder *r)
{
	r->report(JVS_REPORT_OK);
	r->push(0);
	r->push(0);
	r->push(0);
	r->push(0);
	r->push(0);
	return 3;
}

int handleReadCoinInputs(jprot_encoder *r)
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

int handleDecreaseNumberOfCoins(jprot_encoder *r, DWORD arg1, DWORD arg2, DWORD arg3)
{
	WORD val = ((arg2 & 0xFF) << 8) | (arg3 & 0xFF);
	r->report(JVS_REPORT_OK);
#ifdef _DEBUG
	//logcmd("-coin %d, %d\n", arg1, val);
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

int handlePayouts(jprot_encoder *r, DWORD arg1, DWORD arg2, DWORD arg3)
{
	WORD val = ((arg2 & 0xFF) << 8) | (arg3 & 0xFF);
	r->report(JVS_REPORT_OK);
#ifdef _DEBUG
	//logcmd("+coin %d, %d\n", arg1, val);
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

	// Ignore weird packages
	if (pstr[1] != 0x00 && pstr[1] != 0x01 && pstr[1] != 0xFF)
	{
#ifdef _DEBUG
		OutputDebugStringA("Invalid package received!");
#endif
		return 0;
	}

	if (pstr[0] != JVS_SYNC_CODE) {
#ifdef _DEBUG
		info(true, "Invalid Sync code!\n");
#endif
	}
#ifdef _DEBUG
	r.printSource(stream, srcsize);
#endif
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
			increment = handleBusReset();
			break;
		case 0xF1:
			increment = handleSetAddress(&r);
			break;
		case 0x01:
			increment = handleTaito01Call(&r, __ARG__(1));
			break;
		case 0x03:
			increment = handleTaito03Call(&r);
			break;
		case 0x04:
			increment = handleTaito04Call(&r);
			break;
		case 0x05:
			increment = handleTaito05Call(&r);
			break;
		case 0x10:
			increment = handleReadIDData(&r);
			break;
		case 0x11:
			increment = handleGetCommandFormatVersion(&r);
			break;
		case 0x12:
			increment = handleGetJVSVersion(&r);
			break;
		case 0x13:
			increment = handleGetCommunicationVersion(&r);
			break;
		case 0x14:
			increment = handleGetSlaveFeatures(&r);
			break;
		case 0x20:
			increment = handleReadSwitchInputs(&r);
			break;
		case 0x21:
			increment = handleReadCoinInputs(&r);
			break;
		case 0x26:
			increment = handleReadGeneralPurposeInput(&r, __ARG__(1));
			break;
		case 0x2F:
			increment = handleReTransmitDataInCaseOfChecksumFailure();
			break;
		case 0x30:
			increment = handleDecreaseNumberOfCoins(&r, __ARG__(1), __ARG__(2), __ARG__(3));
			break;
		case 0x31:
			increment = handlePayouts(&r, __ARG__(1), __ARG__(2), __ARG__(3));
			break;
		case 0x32:
			increment = handleReadGeneralPurposeOutput(&r, __ARG__(1));
			break;
		default:
#ifdef _DEBUG
			info(true, "Unknown command %X\n", __ARG__(0));
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
#ifdef _DEBUG
	r.printReply();
#endif
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
	static BYTE logger[1024];

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
	if (GameDetect::currentGame == GameID::HyperStreetFighterII)
	{
		if (!strncmp(lpFileName, "D:\\3", 4))
		{
			memset(moveBuf, 0, 256);
			if (lpFileName[3] == '3')
			{
				sprintf(moveBuf, ".\\OpenParrot\\%s", lpFileName + 3);
			}
			return moveBuf;
		}

		return lpFileName;
	}

	if (GameDetect::currentGame == GameID::StreetFigherZero3)
	{
		if (!strncmp(lpFileName, "D:\\c", 4))
		{
			memset(moveBuf, 0, 256);
			if (lpFileName[3] == 'c')
			{
				sprintf(moveBuf, ".\\OpenParrot\\%s", lpFileName + 3);
			}
			return moveBuf;
		}

		return lpFileName;
	}

	if (GameDetect::currentGame == GameID::StreetFighter3rdStrike)
	{
		if (!strncmp(lpFileName, "D:\\9", 4))
		{
			memset(moveBuf, 0, 256);
			if (lpFileName[3] == '9')
			{
				sprintf(moveBuf, ".\\OpenParrot\\%s", lpFileName + 3);
			}
			return moveBuf;
		}

		return lpFileName;
	}

	if (GameDetect::currentGame == GameID::RumbleFish2)
	{
		if (!strncmp(lpFileName, "D:\\eb342", 8))
		{
			memset(moveBuf, 0, 256);
			if (lpFileName[3] == 'e')
			{
				sprintf(moveBuf, ".\\OpenParrot\\%s", lpFileName + 3);
			}
			return moveBuf;
		}

		return lpFileName;
	}

	if (GameDetect::currentGame == GameID::KOF98Nesica)
	{
		if (!strncmp(lpFileName, "d:/SettingKOF98UM", 17) || !strncmp(lpFileName, "d:/RankingKOF98UM", 17) || !strncmp(lpFileName, "d:/CoinFileKOF98UM", 18))
		{
			memset(moveBuf, 0, 256);
			if (lpFileName[2] == '\\' || lpFileName[2] == '/')
			{
				sprintf(moveBuf, ".\\OpenParrot\\%s", lpFileName + 3);
			}

			return moveBuf;
		}

		if (!strncmp(lpFileName, "SettingKOF98UM*.txt", 19) || !strncmp(lpFileName, "RankingKOF98UM*.txt", 19) || !strncmp(lpFileName, "CoinFileKOF98UM*.txt", 20))
		{
			memset(moveBuf, 0, 256);
			sprintf(moveBuf, ".\\OpenParrot\\%s", lpFileName);
			
			return moveBuf;
		}

		return lpFileName;
	}

	if (GameDetect::currentGame == GameID::CrimzonClover)
	{
		if (!strncmp(lpFileName, "D:\\save\\*.dat", 13) || !strncmp(lpFileName, "D:\\save", 7) || !strncmp(lpFileName, "D:\\save\\config", 14) || !strncmp(lpFileName, "D:\\save\\bkeep", 13) || !strncmp(lpFileName, "D:\\save\\score", 13))
		{
			memset(moveBuf, 0, 256);
			if (lpFileName[2] == '\\' || lpFileName[2] == '/')
			{
				sprintf(moveBuf, ".\\OpenParrot\\%s", lpFileName + 3);
			}

			return moveBuf;
		}

		return lpFileName;
	}

	if (GameDetect::currentGame == GameID::VampireSavior)
	{
		if (!strncmp(lpFileName, "D:\\0343d7a98baf803c268e7ed73239464b", 35))
		{
			memset(moveBuf, 0, 256);
			if (lpFileName[3] == '0')
			{
				sprintf(moveBuf, ".\\OpenParrot\\%s", lpFileName + 3);
			}

			return moveBuf;
		}

		return lpFileName;
	}

	if (GameDetect::currentGame == GameID::ChaosCode)
	{
		if (!strncmp(lpFileName, "D:/ChaosCode/save/TYPEX2_RECORD.rcd", 35) || !strncmp(lpFileName, "D:/ChaosCode/save/TYPEX2_SETTING.rcd", 36) || !strncmp(lpFileName, "D:/ChaosCode/save/", 18) || !strncmp(lpFileName, "D:/ChaosCode/", 14))
		{
			memset(moveBuf, 0, 256);
			if (lpFileName[2] == '\\' || lpFileName[2] == '/')
			{
				sprintf(moveBuf, ".\\OpenParrot\\%s", lpFileName + 3);
			}

			return moveBuf;
		}

		return lpFileName;
	}

	if (GameDetect::currentGame == GameID::RaidenIVNesica)
	{
		if (!strncmp(lpFileName, "d:\\income.log", 13) || !strncmp(lpFileName, "D:\\setting.dat", 14) || !strncmp(lpFileName, "D:\\hiscore.dat", 14))
		{
			memset(moveBuf, 0, 256);
			if (lpFileName[2] == '\\' || lpFileName[2] == '/')
			{
				sprintf(moveBuf, ".\\OpenParrot\\%s", lpFileName + 3);
			}
		
			return moveBuf;
		}

		return lpFileName;
	}

	if (GameDetect::currentGame == GameID::SenkoNoRondeDuoNesica)
	{
		if (!strncmp(lpFileName, "D:\\0bd4d39aff8d7219f92f72451f642c2f", 35))
		{
			memset(moveBuf, 0, 256);
			if (lpFileName[4] == 'b')
			{
				sprintf(moveBuf, ".\\OpenParrot\\%s", lpFileName + 3);
			}

			return moveBuf;
		}

		return lpFileName;
	}

	if (GameDetect::currentGame == GameID::SkullGirls)
	{
		if (!strncmp(lpFileName, "D:\\sg_conf_", 11))
		{
			memset(moveBuf, 0, 256);
			if (lpFileName[2] == '\\' || lpFileName[2] == '/')
			{
				sprintf(moveBuf, ".\\OpenParrot\\%s", lpFileName + 3);
			}
		
			return moveBuf;
		}

		return lpFileName;
	}

	if (GameDetect::currentGame == GameID::TroubleWitchesNesica)
	{
		if (!strncmp(lpFileName, "D:\\Save", 7))
		{
			memset(moveBuf, 0, 256);
			if (lpFileName[2] == '\\' || lpFileName[2] == '/')
			{
				sprintf(moveBuf, ".\\OpenParrot\\%s", lpFileName + 3);
			}

			return moveBuf;
		}

		return lpFileName;
	}

	if (GameDetect::currentGame == GameID::Yatagarasu)
	{
		if (!strncmp(lpFileName, "D:\\b2cfd825e375395ce05629616d881045", 35))
		{
			memset(moveBuf, 0, 256);
			if (lpFileName[2] == '\\' || lpFileName[2] == '/')
			{
				sprintf(moveBuf, ".\\OpenParrot\\%s", lpFileName + 3);
			}
		
			return moveBuf;
		}

		return lpFileName;
	}

	if (GameDetect::currentGame == GameID::Exception)
	{
		if (!strncmp(lpFileName, "d:\\game0000.ini", 15) || !strncmp(lpFileName, "d:/ranking", 10) || !strncmp(lpFileName, "d:/log", 6))
		{
			memset(moveBuf, 0, 256);
			if (lpFileName[2] == '\\' || lpFileName[2] == '/')
			{
				sprintf(moveBuf, ".\\OpenParrot\\%s", lpFileName + 3);
			}

			return moveBuf;
		}
		if (!strncmp(lpFileName, "stdout.txt", 10) || !strncmp(lpFileName, "stderr.txt", 10))
		{
			memset(moveBuf, 0, 256);
			if (!strncmp(lpFileName, "stdout.txt", 10))
			{
				sprintf(moveBuf, ".\\OpenParrot\\%s", "stdout.txt");
			}
			if (!strncmp(lpFileName, "stderr.txt", 10))
			{
				sprintf(moveBuf, ".\\OpenParrot\\%s", "stderr.txt");
			}

			return moveBuf;
		}

		return lpFileName;
	}

	if (GameDetect::currentGame == GameID::KOF2002)
	{
		if (!strncmp(lpFileName, "d:/Setting", 10) || !strncmp(lpFileName, "d:/CoinFile", 11))
		{
			memset(moveBuf, 0, 256);
			if (lpFileName[2] == '\\' || lpFileName[2] == '/')
			{
				sprintf(moveBuf, ".\\OpenParrot\\%s", lpFileName + 3);
			}

			return moveBuf;
		}

		return lpFileName;
	}

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
	if (GameDetect::currentGame == GameID::HyperStreetFighterII)
	{
		if (!wcsncmp(lpFileName, L"D:\\3", 4))
		{
			memset(moveBufW, 0, 256);
			if (lpFileName[3] == '3')
			{
				swprintf(moveBufW, L".\\OpenParrot\\%s", lpFileName + 3);
			}
			return moveBufW;
		}

		return lpFileName;
	}

	if (GameDetect::currentGame == GameID::StreetFigherZero3)
	{
		if (!wcsncmp(lpFileName, L"D:\\c", 4))
		{
			memset(moveBufW, 0, 256);
			if (lpFileName[3] == 'c')
			{
				swprintf(moveBufW, L".\\OpenParrot\\%s", lpFileName + 3);
			}
			return moveBufW;
		}

		return lpFileName;
	}

	if (GameDetect::currentGame == GameID::StreetFighter3rdStrike)
	{
		if (!wcsncmp(lpFileName, L"D:\\9", 4))
		{
			memset(moveBufW, 0, 256);
			if (lpFileName[3] == '9')
			{
				swprintf(moveBufW, L".\\OpenParrot\\%s", lpFileName + 3);
			}
			return moveBufW;
		}

		return lpFileName;
	}

	if (GameDetect::currentGame == GameID::RumbleFish2)
	{
		if (!wcsncmp(lpFileName, L"D:\\eb342", 8))
		{
			memset(moveBufW, 0, 256);
			if (lpFileName[3] == 'e')
			{
				swprintf(moveBufW, L".\\OpenParrot\\%s", lpFileName + 3);
			}
			return moveBufW;
		}

		return lpFileName;
	}

	if (GameDetect::currentGame == GameID::KOF98Nesica)
	{
		if (!wcsncmp(lpFileName, L"d:/SettingKOF98UM", 17) || !wcsncmp(lpFileName, L"d:/RankingKOF98UM", 17) || !wcsncmp(lpFileName, L"d:/CoinFileKOF98UM", 18))
		{
			memset(moveBufW, 0, 256);
			if (lpFileName[2] == '\\' || lpFileName[2] == '/')
			{
				swprintf(moveBufW, L".\\OpenParrot\\%s", lpFileName + 3);
			}

			return moveBufW;
		}

		if (!wcsncmp(lpFileName, L"SettingKOF98UM*.txt", 19) || !wcsncmp(lpFileName, L"RankingKOF98UM*.txt", 19) || !wcsncmp(lpFileName, L"CoinFileKOF98UM*.txt", 20))
		{
			memset(moveBufW, 0, 256);
			swprintf(moveBufW, L".\\OpenParrot\\%s", lpFileName);

			return moveBufW;
		}

		return lpFileName;
	}

	if (GameDetect::currentGame == GameID::CrimzonClover)
	{
		if (!wcsncmp(lpFileName, L"D:\\save\\*.dat", 13) || !wcsncmp(lpFileName, L"D:\\save", 7) || !wcsncmp(lpFileName, L"D:\\save\\config", 14) || !wcsncmp(lpFileName, L"D:\\save\\bkeep", 13) || !wcsncmp(lpFileName, L"D:\\save\\score", 13))
		{
			memset(moveBufW, 0, 256);
			if (lpFileName[2] == '\\' || lpFileName[2] == '/')
			{
				swprintf(moveBufW, L".\\OpenParrot\\%s", lpFileName + 3);
			}

			return moveBufW;
		}

		return lpFileName;
	}

	if (GameDetect::currentGame == GameID::VampireSavior)
	{
		if (!wcsncmp(lpFileName, L"D:\\0343d7a98baf803c268e7ed73239464b", 35))
		{
			memset(moveBufW, 0, 256);
			if (lpFileName[3] == '0')
			{
				swprintf(moveBufW, L".\\OpenParrot\\%s", lpFileName + 3);
			}

			return moveBufW;
		}

		return lpFileName;
	}

	if (GameDetect::currentGame == GameID::ChaosCode)
	{
		if (!wcsncmp(lpFileName, L"D:/ChaosCode/save/TYPEX2_RECORD.rcd", 35) || !wcsncmp(lpFileName, L"D:/ChaosCode/save/TYPEX2_SETTING.rcd", 36) || !wcsncmp(lpFileName, L"D:/ChaosCode/save/TYPEX2_SETTING.rcd", 36) || !wcsncmp(lpFileName, L"D:/ChaosCode/save/", 18) || !wcsncmp(lpFileName, L"D:/ChaosCode/", 14))
		{
			memset(moveBufW, 0, 256);
			if (lpFileName[2] == '\\' || lpFileName[2] == '/')
			{
				swprintf(moveBufW, L".\\OpenParrot\\%s", lpFileName + 3);
			}
			return moveBufW;
		}

		return lpFileName;
	}

	if (GameDetect::currentGame == GameID::RaidenIVNesica)
	{
		if (!wcsncmp(lpFileName, L"d:\\income.log", 13) || !wcsncmp(lpFileName, L"D:\\setting.dat", 14) || !wcsncmp(lpFileName, L"D:\\hiscore.dat", 14))
		{
			memset(moveBufW, 0, 256);
			if (lpFileName[2] == '\\' || lpFileName[2] == '/')
			{
				swprintf(moveBufW, L".\\OpenParrot\\%s", lpFileName + 3);
			}

			return moveBufW;
		}

		return lpFileName;
	}

	if (GameDetect::currentGame == GameID::SenkoNoRondeDuoNesica)
	{
		if (!wcsncmp(lpFileName, L"D:\\0bd4d39aff8d7219f92f72451f642c2f", 35))
		{
			memset(moveBufW, 0, 256);
			if (lpFileName[4] == 'b')
			{
				swprintf(moveBufW, L".\\OpenParrot\\%s", lpFileName + 3);
			}

			return moveBufW;
		}

		return lpFileName;
	}

	if (GameDetect::currentGame == GameID::SkullGirls)
	{
		if (!wcsncmp(lpFileName, L"D:\\sg_conf_", 11))
		{
			memset(moveBufW, 0, 256);
			if (lpFileName[2] == '\\' || lpFileName[2] == '/')
			{
				swprintf(moveBufW, L".\\OpenParrot\\%s", lpFileName + 3);
			}
		
			return moveBufW;
		}
	
		return lpFileName;
	}

	if (GameDetect::currentGame == GameID::TroubleWitchesNesica)
	{
		if (!wcsncmp(lpFileName, L"D:\\Save", 7))
		{
			memset(moveBufW, 0, 256);
			if (lpFileName[2] == '\\' || lpFileName[2] == '/')
			{
				swprintf(moveBufW, L".\\OpenParrot\\%s", lpFileName + 3);
			}

			return moveBufW;
		}

		return lpFileName;
	}

	if (GameDetect::currentGame == GameID::Yatagarasu)
	{
		if (!wcsncmp(lpFileName, L"D:\\b2cfd825e375395ce05629616d881045", 35))
		{
			memset(moveBufW, 0, 256);
			if (lpFileName[2] == '\\' || lpFileName[2] == '/')
			{
				swprintf(moveBufW, L".\\OpenParrot\\%s", lpFileName + 3);
			}
		
			return moveBufW;
		}

		return lpFileName;
	}

	if (GameDetect::currentGame == GameID::Exception)
	{
		if (!wcsncmp(lpFileName, L"d:\\game0000.ini", 15) || !wcsncmp(lpFileName, L"d:/ranking", 10) || !wcsncmp(lpFileName, L"d:/log", 6))
		{
			memset(moveBufW, 0, 256);
			if (lpFileName[2] == '\\' || lpFileName[2] == '/')
			{
				swprintf(moveBufW, L".\\OpenParrot\\%s", lpFileName + 3);
			}

			return moveBufW;
		}
		if (!wcsncmp(lpFileName, L"stdout.txt", 10) || !wcsncmp(lpFileName, L"stderr.txt", 10))
		{
			memset(moveBufW, 0, 256);
			if (!wcsncmp(lpFileName, L"stdout.txt", 10))
			{
				swprintf(moveBufW, L".\\OpenParrot\\%s", L"stdout.txt");
			}
			if (!wcsncmp(lpFileName, L"stderr.txt", 10))
			{
				swprintf(moveBufW, L".\\OpenParrot\\%s", L"stderr.txt");
			}

			return moveBufW;
		}

		return lpFileName;
	}

	if (GameDetect::currentGame == GameID::KOF2002)
	{
		if (!wcsncmp(lpFileName, L"d:/Setting", 10) || !wcsncmp(lpFileName, L"d:/CoinFile", 11))
		{
			memset(moveBufW, 0, 256);
			if (lpFileName[2] == '\\' || lpFileName[2] == '/')
			{
				swprintf(moveBufW, L".\\OpenParrot\\%s", lpFileName + 3);
			}

			return moveBufW;
		}

		return lpFileName;
	}

	if (!wcsncmp(lpFileName, L"D:", 2) || !wcsncmp(lpFileName, L"d:", 2))
	{
		memset(moveBufW, 0, 256);
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

	if(GameDetect::currentGame == GameID::GrooveCoaster2)
	{
		return g_origCreateFileA(lpFileName,
			dwDesiredAccess,
			dwShareMode,
			lpSecurityAttributes,
			dwCreationDisposition,
			dwFlagsAndAttributes,
			hTemplateFile);
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

	if (GameDetect::currentGame == GameID::GrooveCoaster2 || GameDetect::currentGame == GameID::KOFXIIIClimax)
	{
		return g_origCreateFileW(lpFileName,
			dwDesiredAccess,
			dwShareMode,
			lpSecurityAttributes,
			dwCreationDisposition,
			dwFlagsAndAttributes,
			hTemplateFile);
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
	if (GameDetect::currentGame == GameID::GrooveCoaster2)
	{
		return g_origGetFileAttributesA(lpFileName);
	}

	return g_origGetFileAttributesA(ParseFileNamesA(lpFileName));
}

static DWORD(__stdcall *g_origGetFileAttributesW)(LPCWSTR lpFileName);
static DWORD __stdcall GetFileAttributesWWrap(LPCWSTR lpFileName)
{
	if (GameDetect::currentGame == GameID::GrooveCoaster2)
	{
		return g_origGetFileAttributesW(lpFileName);
	}

	return g_origGetFileAttributesW(ParseFileNamesW(lpFileName));
}

static BOOL(__stdcall *g_origCreateDirectoryA)(LPCSTR lpPathName, LPSECURITY_ATTRIBUTES lpSecurityAttributes);
static BOOL __stdcall CreateDirectoryAWrap(
	LPCSTR                lpPathName,
	LPSECURITY_ATTRIBUTES lpSecurityAttributes
)
{
	if (GameDetect::currentGame == GameID::GrooveCoaster2)
	{
		return g_origCreateDirectoryA(lpPathName, lpSecurityAttributes);
	}

	return g_origCreateDirectoryA(ParseFileNamesA(lpPathName), lpSecurityAttributes);
}

static BOOL(__stdcall *g_origCreateDirectoryW)(LPCWSTR lpPathName, LPSECURITY_ATTRIBUTES lpSecurityAttributes);
static BOOL __stdcall CreateDirectoryWWrap(
	LPCWSTR                lpPathName,
	LPSECURITY_ATTRIBUTES lpSecurityAttributes
)
{
	if (GameDetect::currentGame == GameID::GrooveCoaster2)
	{
		return g_origCreateDirectoryW(lpPathName, lpSecurityAttributes);
	}

	return g_origCreateDirectoryW(ParseFileNamesW(lpPathName), lpSecurityAttributes);
}

static HANDLE(__stdcall *g_origFindFirstFileA)(LPCSTR lpFileName, LPWIN32_FIND_DATAA lpFindFileData);
static HANDLE __stdcall FindFirstFileAWrap(
	LPCSTR             lpFileName,
	LPWIN32_FIND_DATAA lpFindFileData
)
{
	if (GameDetect::currentGame == GameID::GrooveCoaster2)
	{
		return g_origFindFirstFileA(lpFileName, lpFindFileData);
	}

	return g_origFindFirstFileA(ParseFileNamesA(lpFileName), lpFindFileData);
}

static HANDLE(__stdcall *g_origFindFirstFileW)(LPCWSTR lpFileName, LPWIN32_FIND_DATAA lpFindFileData);
static HANDLE __stdcall FindFirstFileWWrap(
	LPCWSTR             lpFileName,
	LPWIN32_FIND_DATAA lpFindFileData
)
{
	if (GameDetect::currentGame == GameID::GrooveCoaster2)
	{
		return g_origFindFirstFileW(lpFileName, lpFindFileData);
	}

	return g_origFindFirstFileW(ParseFileNamesW(lpFileName), lpFindFileData);
}

static BOOL(__stdcall* g_origGetDiskFreeSpaceExA)(LPCSTR lpDirectoryName, PULARGE_INTEGER lpFreeBytesAvailableToCaller, PULARGE_INTEGER lpTotalNumberOfBytes, PULARGE_INTEGER lpTotalNumberOfFreeBytes);
static BOOL __stdcall GetDiskFreeSpaceExAWrap(LPCSTR lpDirectoryName, PULARGE_INTEGER lpFreeBytesAvailableToCaller, PULARGE_INTEGER lpTotalNumberOfBytes, PULARGE_INTEGER lpTotalNumberOfFreeBytes)
{
	return g_origGetDiskFreeSpaceExA(NULL, lpFreeBytesAvailableToCaller, lpTotalNumberOfBytes, lpTotalNumberOfFreeBytes);
}

static BOOL(__stdcall* g_origGetDiskFreeSpaceExW)(LPCWSTR lpDirectoryName, PULARGE_INTEGER lpFreeBytesAvailableToCaller, PULARGE_INTEGER lpTotalNumberOfBytes, PULARGE_INTEGER lpTotalNumberOfFreeBytes);
static BOOL __stdcall GetDiskFreeSpaceExWWrap(LPCWSTR lpDirectoryName, PULARGE_INTEGER lpFreeBytesAvailableToCaller, PULARGE_INTEGER lpTotalNumberOfBytes, PULARGE_INTEGER lpTotalNumberOfFreeBytes)
{
	return g_origGetDiskFreeSpaceExW(NULL, lpFreeBytesAvailableToCaller, lpTotalNumberOfBytes, lpTotalNumberOfFreeBytes);
}

static BOOL(__stdcall* g_origDeleteFileA)(LPCSTR lpFileName);
static BOOL __stdcall DeleteFileAWrap(LPCSTR lpFileName) // seems to be used by Akai Katana Shin?
{
	return g_origDeleteFileA(ParseFileNamesA(lpFileName));
}

static BOOL(__stdcall* g_origDeleteFileW)(LPCWSTR lpFileName);
static BOOL __stdcall DeleteFileWWrap(LPCWSTR lpFileName) // seems to be used by Akai Katana Shin?
{
	return g_origDeleteFileW(ParseFileNamesW(lpFileName));
}

static HANDLE(__stdcall* g_origFindFirstFileExA)(LPCSTR lpFileName, FINDEX_INFO_LEVELS fInfoLevelId, LPVOID lpFindFileData, FINDEX_SEARCH_OPS  fSearchOp, LPVOID lpSearchFilter, DWORD dwAdditionalFlags);
static HANDLE __stdcall FindFirstFileExAWrap(LPCSTR lpFileName, FINDEX_INFO_LEVELS fInfoLevelId, LPVOID lpFindFileData, FINDEX_SEARCH_OPS  fSearchOp, LPVOID lpSearchFilter, DWORD dwAdditionalFlags)
{
	return g_origFindFirstFileExA(ParseFileNamesA(lpFileName), fInfoLevelId, lpFindFileData, fSearchOp, lpSearchFilter, dwAdditionalFlags);
}

static HANDLE(__stdcall* g_origFindFirstFileExW)(LPCWSTR lpFileName, FINDEX_INFO_LEVELS fInfoLevelId, LPVOID lpFindFileData, FINDEX_SEARCH_OPS  fSearchOp, LPVOID lpSearchFilter, DWORD dwAdditionalFlags);
static HANDLE __stdcall FindFirstFileExWWrap(LPCWSTR lpFileName, FINDEX_INFO_LEVELS fInfoLevelId, LPVOID lpFindFileData, FINDEX_SEARCH_OPS  fSearchOp, LPVOID lpSearchFilter, DWORD dwAdditionalFlags)
{
	return g_origFindFirstFileExW(ParseFileNamesW(lpFileName), fInfoLevelId, lpFindFileData, fSearchOp, lpSearchFilter, dwAdditionalFlags);
}

static BOOL(__stdcall* g_origSetCurrentDirectoryA)(LPCSTR lpPathName);
static BOOL __stdcall SetCurrentDirectoryAWrap(LPCSTR lpPathName)
{
	char pathRoot[MAX_PATH];
	GetModuleFileNameA(GetModuleHandleA(nullptr), pathRoot, _countof(pathRoot)); // get full pathname to game executable

	strrchr(pathRoot, '\\')[0] = '\0'; // chop off everything from the last backslash.

	return g_origSetCurrentDirectoryA((pathRoot + ""s).c_str());
}

static DWORD WINAPI InsertCardThread(LPVOID)
{
	static bool keyDown;
	while (true)
	{
		if (GetAsyncKeyState(VK_F4))
		{
			if (!keyDown)
			{
				cardInserted = !cardInserted;
				keyDown = true;
			}
		}
		else
		{
			keyDown = false;
		}
		Sleep(100);
	}
	return 1;
}

void init_RfidEmu()
{
	MH_Initialize();
	CreateDirectoryA("OpenParrot", nullptr);
	MH_CreateHookApi(L"kernel32.dll", "FindFirstFileA", FindFirstFileAWrap, (void**)&g_origFindFirstFileA);
	MH_CreateHookApi(L"kernel32.dll", "FindFirstFileW", FindFirstFileWWrap, (void**)&g_origFindFirstFileW);
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
	MH_CreateHookApi(L"kernel32.dll", "GetDiskFreeSpaceExA", GetDiskFreeSpaceExAWrap, (void**)&g_origGetDiskFreeSpaceExA);
	MH_CreateHookApi(L"kernel32.dll", "GetDiskFreeSpaceExW", GetDiskFreeSpaceExWWrap, (void**)&g_origGetDiskFreeSpaceExW);
	MH_CreateHookApi(L"kernel32.dll", "DeleteFileA", DeleteFileAWrap, (void**)&g_origDeleteFileA);
	MH_CreateHookApi(L"kernel32.dll", "DeleteFileW", DeleteFileWWrap, (void**)&g_origDeleteFileW);
	MH_CreateHookApi(L"kernel32.dll", "FindFirstFileExA", FindFirstFileExAWrap, (void**)&g_origFindFirstFileExA);
	MH_CreateHookApi(L"kernel32.dll", "FindFirstFileExW", FindFirstFileExWWrap, (void**)&g_origFindFirstFileExW);
	MH_CreateHookApi(L"kernel32.dll", "SetCurrentDirectoryA", SetCurrentDirectoryAWrap, (void**)&g_origSetCurrentDirectoryA);
	MH_EnableHook(MH_ALL_HOOKS);

	CreateThread(0, 0, InsertCardThread, 0, 0, 0);
}