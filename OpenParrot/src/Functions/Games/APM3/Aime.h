#pragma once
#include "APM3.h"

bool CALLPLEB Aime_acceptConfirm();
bool CALLPLEB Aime_cancel();

struct AccessCode
{
	int DigitCount = 20;
	int Size = 10;
	int StrSize = 41;
	char values[10];
	char valueStr[41];
};

bool CALLPLEB Aime_getAccessCode(AccessCode* accessCode);
bool CALLPLEB Aime_getAimeId(unsigned int* uid);

enum class AIME_CONFIRM
{
	NoneDB,
	FeliCaDB,
	AimeDB,
};

AIME_CONFIRM CALLPLEB Aime_getConfirm();

enum class AIME_ERROR_CATEGORY
{
	NONE,
	WARNING,
	NETWORK,
	FATAL,
	UNKNOWN,
};

AIME_ERROR_CATEGORY CALLPLEB Aime_getErrorCategory();
bool CALLPLEB Aime_hasConfirm();
bool CALLPLEB Aime_hasError();
bool CALLPLEB Aime_hasResult();
bool CALLPLEB Aime_isBusy();
bool CALLPLEB Aime_isDBAlive();
bool CALLPLEB Aime_isMobile();
bool CALLPLEB Aime_isReaderDetected();

enum class AIME_LOG_STATUS
{
	BEGIN,
	CONTINUE,
	END,
};

bool CALLPLEB Aime_sendLog(unsigned int uid, AIME_LOG_STATUS status, DWORD_PTR count);
bool CALLPLEB Aime_setLedError();
bool CALLPLEB Aime_setLedSuccess();
bool CALLPLEB Aime_start();

extern bool Aime_isDBAliveReturnValue;
extern bool Aime_hasConfirmValue;
extern bool Aime_hasResultValue;