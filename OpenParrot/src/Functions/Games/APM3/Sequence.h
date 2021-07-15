#ifdef _M_AMD64
struct SequenceAccountingBegin
{
	bool AccountingBeginValid;
	unsigned int AccountingBeginKindCode;
};

struct SequenceAccountingEnd
{
	bool accountingEndValid;
	unsigned int accountingEndKindCode;
	unsigned int accountingEndItemCount;
};

enum class SEQUENCE_PLAY_ERROR_ID
{
	NONE,
	INVALID_PARAM,
	INVALID_STATE,
	CREDIT_PAY,
	ACCOUNTING_BEGIN,
	ACCOUNTING_END,
};


struct SequenceTimeHistogramItem
{
	unsigned int TimeRangeMin;
	unsigned int TimeRangeMax;
	unsigned int Count;
};

struct SequenceBookkeeping
{
	const unsigned long MaxTimeHistogramCount = 32;
	unsigned int NumberOfGames;
	unsigned int TotalTime;
	unsigned int PlayTime;
	unsigned int AveragePlayTime;
	unsigned int LongestPlayTime;
	unsigned int ShortestPlayTime;
	unsigned int TimeHistogramCount;
	SequenceTimeHistogramItem TimeHistogram[32];
};

static bool Sequence_beginPlayReturnValue = true;

static bool Sequence_clearBackupReturnValue = false;

static bool Sequence_continuePlayReturnValue = 0;

static bool Sequence_endPlayReturnValue = false;

static unsigned int Sequence_getPlayingAimeIdReturnValue = 0;

static bool Sequence_isAccountingPlayingReturnValue = false;

static bool Sequence_isPlayingReturnValue = false;

static bool Sequence_isTestReturnValue = 0;

bool Sequence_isTest();
bool Sequence_isPlaying();
bool Sequence_isAccountingPlaying();
unsigned int Sequence_getPlayingAimeId();
SequenceBookkeeping* Sequence_getBookkeeping();
bool __fastcall Sequence_endPlay(SequenceAccountingEnd endPlay, SEQUENCE_PLAY_ERROR_ID* errorID);
bool __fastcall Sequence_continuePlay(SequenceAccountingBegin beginPlay, SequenceAccountingEnd endPlay, SEQUENCE_PLAY_ERROR_ID* errorId, unsigned long count);
bool Sequence_clearBackup();
bool __fastcall Sequence_beginPlay(SequenceAccountingBegin beginPlay, unsigned int uid, SEQUENCE_PLAY_ERROR_ID* errorID, unsigned long count);

#endif