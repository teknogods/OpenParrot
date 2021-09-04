#include <Utility/InitFunction.h>
#include <StdInc.h>
#include <thread>
#include "Functions\Global.h"
#include "Sequence.h"

// Define default return values here
bool Sequence_beginPlayReturnValue = true;
bool Sequence_clearBackupReturnValue = false;
bool Sequence_continuePlayReturnValue = true;
bool Sequence_endPlayReturnValue = true;
unsigned int Sequence_getPlayingAimeIdReturnValue = 0;
bool Sequence_isAccountingPlayingReturnValue = false;
bool Sequence_isPlayingReturnValue = false;
bool Sequence_isTestReturnValue = false;

bool CALLPLEB Sequence_beginPlay(SequenceAccountingBegin beginPlay, unsigned int uid, SEQUENCE_PLAY_ERROR_ID* errorID, unsigned long count)
{
#ifdef _LOGAPM3SEQUENCE
	info(true, "Sequence_beginPlay rv: %u", Sequence_beginPlayReturnValue);
#endif
	return Sequence_beginPlayReturnValue;
}

bool CALLPLEB Sequence_clearBackup()
{
#ifdef _LOGAPM3SEQUENCE
	info(true, "Sequence_clearBackup rv: %u", Sequence_clearBackupReturnValue);
#endif
	return Sequence_clearBackupReturnValue;
}

bool CALLPLEB Sequence_continuePlay(SequenceAccountingBegin beginPlay, SequenceAccountingEnd endPlay, SEQUENCE_PLAY_ERROR_ID* errorId, unsigned long count)
{
#ifdef _LOGAPM3SEQUENCE
	info(true, "Sequence_continuePlay rv: %u", Sequence_continuePlayReturnValue);
#endif
	return Sequence_continuePlayReturnValue;
}

bool CALLPLEB Sequence_endPlay(SequenceAccountingEnd endPlay, SEQUENCE_PLAY_ERROR_ID* errorID)
{
#ifdef _LOGAPM3SEQUENCE
	info(true, "Sequence_endPlay rv: %u", Sequence_endPlayReturnValue);
#endif
	return Sequence_endPlayReturnValue;
}

SequenceBookkeeping* CALLPLEB Sequence_getBookkeeping()
{
#ifdef _LOGAPM3SEQUENCE
	info(true, "Sequence_getBookkeeping");
#endif
	static SequenceBookkeeping* _sequenceBookkeeping;
	if (_sequenceBookkeeping == nullptr)
	{
		_sequenceBookkeeping = (SequenceBookkeeping*)malloc(sizeof(SequenceBookkeeping));
		memset(_sequenceBookkeeping, 0, sizeof(SequenceBookkeeping));
	}
	return _sequenceBookkeeping;
}

unsigned int CALLPLEB Sequence_getPlayingAimeId()
{
#ifdef _LOGAPM3SEQUENCE
	info(true, "Sequence_getPlayingAimeId rv: %u", Sequence_getPlayingAimeIdReturnValue);
#endif
	return Sequence_getPlayingAimeIdReturnValue;
}

bool CALLPLEB Sequence_isAccountingPlaying()
{
#ifdef _LOGAPM3SEQUENCE
	info(true, "Sequence_isAccountingPlaying rv: %u", Sequence_isAccountingPlayingReturnValue);
#endif
	return Sequence_isAccountingPlayingReturnValue;
}

bool CALLPLEB Sequence_isPlaying()
{
#ifdef _LOGAPM3SEQUENCE
	info(true, "Sequence_isPlaying rv: %u", Sequence_isPlayingReturnValue);
#endif
	return Sequence_isPlayingReturnValue;
}

bool CALLPLEB Sequence_isTest()
{
#ifdef _LOGAPM3SEQUENCE
	info(true, "Sequence_isTest rv: %u", Sequence_isTestReturnValue);
#endif
	return Sequence_isTestReturnValue;
}
