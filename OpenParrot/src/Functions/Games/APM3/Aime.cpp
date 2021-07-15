#include <Utility/InitFunction.h>
#include <StdInc.h>
#include <thread>
#ifdef _M_AMD64
#include "Functions\Global.h"
#include "Aime.h"
	
bool Aime_acceptConfirm()
{
#ifdef _LOGAPM3AIME
	info(true, "Aime_acceptConfirm");
#endif
	return 0;
}

bool Aime_cancel()
{
#ifdef _LOGAPM3AIME
	info(true, "Aime_cancel");
#endif

	return 0;
}

bool __fastcall Aime_getAccessCode(AccessCode *accessCode)
{
#ifdef _LOGAPM3AIME
	info(true, "Aime_getAccessCode");
#endif
	return 0;
}

bool __fastcall Aime_getAimeId(unsigned int* uid)
{
#ifdef _LOGAPM3AIME
	info(true, "Aime_getAimeId");
#endif
	return 0;
}


AIME_CONFIRM Aime_getConfirm()
{
#ifdef _LOGAPM3AIME
	info(true, "Aime_getConfirm");
#endif
	return AIME_CONFIRM::AimeDB;
}


AIME_ERROR_CATEGORY Aime_getErrorCategory()
{
#ifdef _LOGAPM3AIME
	info(true, "Aime_getErrorCategory");
#endif
	return AIME_ERROR_CATEGORY::NONE;
}

bool Aime_hasConfirm()
{
#ifdef _LOGAPM3AIME
	info(true, "Aime_hasConfirm");
#endif
	return false;
}

bool Aime_hasError()
{
#ifdef _LOGAPM3AIME
	info(true, "Aime_hasError");
#endif
	return false;
}

bool Aime_hasResult()
{
#ifdef _LOGAPM3AIME
	info(true, "Aime_hasResult");
#endif
	return false;
}

bool Aime_isBusy()
{
#ifdef _LOGAPM3AIME
	info(true, "Aime_isBusy");
#endif
	return false;
}

bool Aime_isDBAlive()
{
#ifdef _LOGAPM3AIME
	info(true, "Aime_isDBAlive");
#endif
	return false;
}

bool Aime_isMobile()
{
#ifdef _LOGAPM3AIME
	info(true, "Aime_isMobile");
#endif
	return false;
}

bool Aime_isReaderDetected()
{
#ifdef _LOGAPM3AIME
	info(true, "Aime_isReaderDetected");
#endif
	return true;
}

bool __fastcall Aime_sendLog(unsigned int uid, AIME_LOG_STATUS status, unsigned __int64 count)
{
#ifdef _LOGAPM3AIME
	info(true, "Aime_sendLog");
#endif
	return true;
}

bool Aime_setLedError()
{
#ifdef _LOGAPM3AIME
	info(true, "Aime_setLedError");
#endif
	return true;
}

bool Aime_setLedSuccess()
{
#ifdef _LOGAPM3AIME
	info(true, "Aime_setLedSuccess");
#endif
	return true;
}

bool Aime_start()
{
#ifdef _LOGAPM3AIME
	info(true, "Aime_start");
#endif
	return false;
}
#endif