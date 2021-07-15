#include <Utility/InitFunction.h>
#include <StdInc.h>
#include <thread>
#ifdef _M_AMD64
#include "Functions\Global.h"
#include "Sequence.h"

bool __fastcall Sequence_beginPlay(SequenceAccountingBegin beginPlay, unsigned int uid, SEQUENCE_PLAY_ERROR_ID* errorID, unsigned long count)
{
#ifdef _DEBUG
	info(true, "Sequence_beginPlay");
#endif
	return Sequence_beginPlayReturnValue;
}

bool Sequence_clearBackup()
{
#ifdef _DEBUG
	info(true, "Sequence_clearBackup");
#endif
	return Sequence_clearBackupReturnValue;
}

bool __fastcall Sequence_continuePlay(SequenceAccountingBegin beginPlay, SequenceAccountingEnd endPlay, SEQUENCE_PLAY_ERROR_ID* errorId, unsigned long count)
{
#ifdef _DEBUG
	info(true, "Sequence_continuePlay");
#endif
	return Sequence_continuePlayReturnValue;
}

bool __fastcall Sequence_endPlay(SequenceAccountingEnd endPlay, SEQUENCE_PLAY_ERROR_ID* errorID)
{
#ifdef _DEBUG
	info(true, "Sequence_endPlay");
#endif
	return Sequence_endPlayReturnValue;
}

SequenceBookkeeping* Sequence_getBookkeeping()
{
#ifdef _DEBUG
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

unsigned int Sequence_getPlayingAimeId()
{
#ifdef _DEBUG
	info(true, "Sequence_getPlayingAimeId");
#endif
	return Sequence_getPlayingAimeIdReturnValue;
}

bool Sequence_isAccountingPlaying()
{
#ifdef _DEBUG
	info(true, "Sequence_isAccountingPlaying");
#endif
	return Sequence_isAccountingPlayingReturnValue;
}

bool Sequence_isPlaying()
{
#ifdef _DEBUG
	info(true, "Sequence_isPlaying");
#endif
	return Sequence_isPlayingReturnValue;
}

bool Sequence_isTest()
{
#ifdef _DEBUG
	info(true, "Sequence_isTest");
#endif
	return Sequence_isTestReturnValue;
}
#endif