#include <Utility/InitFunction.h>
#include <StdInc.h>
#include <thread>
#include "Functions\Global.h"
#include "Sequence.h"
//#define _LOGAPM3SEQUENCE 1
bool CALLPLEB Sequence_beginPlay(SequenceAccountingBegin beginPlay, unsigned int uid, SEQUENCE_PLAY_ERROR_ID* errorID, unsigned long count)
{
#ifdef _LOGAPM3SEQUENCE
	info(true, "Sequence_beginPlay");
#endif
	return Sequence_beginPlayReturnValue;
}

bool CALLPLEB Sequence_clearBackup()
{
#ifdef _LOGAPM3SEQUENCE
	info(true, "Sequence_clearBackup");
#endif
	return Sequence_clearBackupReturnValue;
}

bool CALLPLEB Sequence_continuePlay(SequenceAccountingBegin beginPlay, SequenceAccountingEnd endPlay, SEQUENCE_PLAY_ERROR_ID* errorId, unsigned long count)
{
#ifdef _LOGAPM3SEQUENCE
	info(true, "Sequence_continuePlay");
#endif
	return true;
}

bool CALLPLEB Sequence_endPlay(SequenceAccountingEnd endPlay, SEQUENCE_PLAY_ERROR_ID* errorID)
{
#ifdef _LOGAPM3SEQUENCE
	info(true, "Sequence_endPlay");
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
	info(true, "Sequence_getPlayingAimeId");
#endif
	return Sequence_getPlayingAimeIdReturnValue;
}

bool CALLPLEB Sequence_isAccountingPlaying()
{
#ifdef _LOGAPM3SEQUENCE
	info(true, "Sequence_isAccountingPlaying");
#endif
	return Sequence_isAccountingPlayingReturnValue;
}

bool CALLPLEB Sequence_isPlaying()
{
#ifdef _LOGAPM3SEQUENCE
	info(true, "Sequence_isPlaying");
#endif
	return Sequence_isPlayingReturnValue;
}

bool CALLPLEB Sequence_isTest()
{
#ifdef _LOGAPM3SEQUENCE
	info(true, "Sequence_isTest");
#endif
	return Sequence_isTestReturnValue;
}
