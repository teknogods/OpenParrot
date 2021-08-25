#ifdef _M_AMD64
bool Aime_acceptConfirm();
bool Aime_cancel();
struct AccessCode
{
	const int DigitCount = 20;
	const int Size = 10;
	const int StrSize = 41;
	char values[10];
	char valueStr[41];
};

bool __fastcall Aime_getAccessCode(AccessCode* accessCode);
bool __fastcall Aime_getAimeId(unsigned int* uid);
enum class AIME_CONFIRM
{
	NoneDB,
	FeliCaDB,
	AimeDB,
};

AIME_CONFIRM Aime_getConfirm();

enum class AIME_ERROR_CATEGORY
{
	NONE,
	WARNING,
	NETWORK,
	FATAL,
	UNKNOWN,
};

AIME_ERROR_CATEGORY Aime_getErrorCategory();

bool Aime_hasConfirm();

bool Aime_hasError();

bool Aime_hasResult();

bool Aime_isBusy();

bool Aime_isDBAlive();

bool Aime_isMobile();

bool Aime_isReaderDetected();

enum class AIME_LOG_STATUS
{
	BEGIN,
	CONTINUE,
	END,
};

bool __fastcall Aime_sendLog(unsigned int uid, AIME_LOG_STATUS status, unsigned __int64 count);

bool Aime_setLedError();

bool Aime_setLedSuccess();

bool Aime_start();

#endif