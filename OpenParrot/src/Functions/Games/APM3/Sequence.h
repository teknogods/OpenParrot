#pragma once
#include "APM3.h"

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

// Make return values global
extern bool Sequence_beginPlayReturnValue;
extern bool Sequence_clearBackupReturnValue;
extern bool Sequence_continuePlayReturnValue;
extern bool Sequence_endPlayReturnValue;
extern unsigned int Sequence_getPlayingAimeIdReturnValue;
extern bool Sequence_isAccountingPlayingReturnValue;
extern bool Sequence_isPlayingReturnValue;
extern bool Sequence_isTestReturnValue;

bool CALLPLEB Sequence_isTest();
bool CALLPLEB Sequence_isPlaying();
bool CALLPLEB Sequence_isAccountingPlaying();
unsigned int CALLPLEB Sequence_getPlayingAimeId();
SequenceBookkeeping* CALLPLEB Sequence_getBookkeeping();
bool CALLPLEB Sequence_endPlay(SequenceAccountingEnd endPlay, SEQUENCE_PLAY_ERROR_ID* errorID);
bool CALLPLEB Sequence_continuePlay(SequenceAccountingBegin beginPlay, SequenceAccountingEnd endPlay, SEQUENCE_PLAY_ERROR_ID* errorId, unsigned long count);
bool CALLPLEB Sequence_clearBackup();
bool CALLPLEB Sequence_beginPlay(SequenceAccountingBegin beginPlay, unsigned int uid, SEQUENCE_PLAY_ERROR_ID* errorID, unsigned long count);