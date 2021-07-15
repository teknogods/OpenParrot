#include <Utility/InitFunction.h>
#include <StdInc.h>
#include <thread>
#ifdef _M_AMD64
#include "Functions\Global.h"
#include "Aime.h"

static uint8_t g_APM3IOValues[256];

bool Aime_acceptConfirm()
{
#ifdef _DEBUG
	info(true, "Aime_acceptConfirm");
#endif
	return 0;
}

bool Aime_cancel()
{
#ifdef _DEBUG
	info(true, "Aime_cancel");
#endif

	return 0;
}

bool __fastcall Aime_getAccessCode(AccessCode *accessCode)
{
#ifdef _DEBUG
	info(true, "Aime_getAccessCode");
#endif
	return 0;
}

bool __fastcall Aime_getAimeId(unsigned int* uid)
{
#ifdef _DEBUG
	info(true, "Aime_getAimeId");
#endif
	return 0;
}


AIME_CONFIRM Aime_getConfirm()
{
#ifdef _DEBUG
	info(true, "Aime_getConfirm");
#endif
	return AIME_CONFIRM::AimeDB;
}


AIME_ERROR_CATEGORY Aime_getErrorCategory()
{
#ifdef _DEBUG
	info(true, "Aime_getErrorCategory");
#endif
	return AIME_ERROR_CATEGORY::NONE;
}

bool Aime_hasConfirm()
{
#ifdef _DEBUG
	info(true, "Aime_hasConfirm");
#endif
	return 0;
}

bool Aime_hasError()
{
#ifdef _DEBUG
	info(true, "Aime_hasError");
#endif
	return 0;
}

bool Aime_hasResult()
{
#ifdef _DEBUG
	info(true, "Aime_hasResult");
#endif
	return 0;
}

bool Aime_isBusy()
{
#ifdef _DEBUG
	info(true, "Aime_isBusy");
#endif
	return 0;
}

bool Aime_isDBAlive()
{
#ifdef _DEBUG
	info(true, "Aime_isDBAlive");
#endif
	return 0;
}

bool Aime_isMobile()
{
#ifdef _DEBUG
	info(true, "Aime_isMobile");
#endif
	return 0;
}

bool Aime_isReaderDetected()
{
#ifdef _DEBUG
	info(true, "Aime_isReaderDetected");
#endif
	return 0;
}

bool __fastcall Aime_sendLog(unsigned int uid, AIME_LOG_STATUS status, unsigned __int64 count)
{
#ifdef _DEBUG
	info(true, "Aime_sendLog");
#endif
	return 0;
}

bool Aime_setLedError()
{
#ifdef _DEBUG
	info(true, "Aime_setLedError");
#endif
	return 0;
}

bool Aime_setLedSuccess()
{
#ifdef _DEBUG
	info(true, "Aime_setLedSuccess");
#endif
	return 0;
}

bool Aime_start()
{
#ifdef _DEBUG
	info(true, "Aime_start");
#endif
	return 0;
}
#endif