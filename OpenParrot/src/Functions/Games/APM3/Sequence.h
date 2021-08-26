#pragma once
#ifdef _M_IX86
#define CALLPLEB __stdcall
#else
#define CALLPLEB __fastcall
#endif
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

bool CALLPLEB Sequence_isTest();
bool CALLPLEB Sequence_isPlaying();
bool CALLPLEB Sequence_isAccountingPlaying();
unsigned int CALLPLEB Sequence_getPlayingAimeId();
SequenceBookkeeping* CALLPLEB Sequence_getBookkeeping();
bool CALLPLEB Sequence_endPlay(SequenceAccountingEnd endPlay, SEQUENCE_PLAY_ERROR_ID* errorID);
bool CALLPLEB Sequence_continuePlay(SequenceAccountingBegin beginPlay, SequenceAccountingEnd endPlay, SEQUENCE_PLAY_ERROR_ID* errorId, unsigned long count);
bool CALLPLEB Sequence_clearBackup();
bool CALLPLEB Sequence_beginPlay(SequenceAccountingBegin beginPlay, unsigned int uid, SEQUENCE_PLAY_ERROR_ID* errorID, unsigned long count);