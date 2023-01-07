#pragma once

#define BANA_API __declspec(dllexport)
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

extern "C"
{

	BANA_API ULONGLONG BngRwAttach(UINT, char*, int, int, void*, long);
	BANA_API int BngRwDevReset(UINT, ULONGLONG, ULONGLONG);
	BANA_API ULONGLONG BngRwExReadMifareAllBlock();
	BANA_API void BngRwFin();
	BANA_API UINT BngRwGetFwVersion(UINT);
	BANA_API UINT BngRwGetStationID(UINT);
	BANA_API UINT BngRwGetTotalRetryCount(UINT);
	BANA_API const char* BngRwGetVersion();
	BANA_API long BngRwInit();
	BANA_API ULONGLONG BngRwIsCmdExec(UINT);
	BANA_API int BngRwReqAction(UINT, UINT, ULONGLONG, ULONGLONG);
	BANA_API int BngRwReqAiccAuth(UINT, int, UINT, int*, ULONGLONG, ULONGLONG, ULONGLONG*);
	BANA_API int BngRwReqBeep(UINT, UINT, ULONGLONG, ULONGLONG);
	BANA_API int BngRwReqCancel(UINT);
	BANA_API int BngRwReqFwCleanup(UINT, ULONGLONG, ULONGLONG);
	BANA_API int BngRwReqFwVersionup(UINT, ULONGLONG, ULONGLONG, ULONGLONG);
	BANA_API int BngRwReqLatchID(UINT, ULONGLONG, ULONGLONG);
	BANA_API int BngRwReqLed(UINT, UINT, ULONGLONG, ULONGLONG);
	BANA_API int BngRwReqSendMailTo(UINT, int, UINT, int*, char*, char*, char*, char*, ULONGLONG, ULONGLONG);
	BANA_API int BngRwReqSendUrlTo(UINT, int, UINT, int*, char*, char*, ULONGLONG, ULONGLONG);
	BANA_API int BngRwReqWaitTouch(UINT a, int maxIntSomehow, UINT c, void (*callback)(int, int, void*, void*), void* e);
	BANA_API ULONGLONG BngRwReqSetLedPower();

}