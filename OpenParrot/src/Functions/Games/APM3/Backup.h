#pragma once
#include "APM3.h"

extern char* BackupSaveFileName;
extern char fileBuffer[256];

struct BackupRecord
{
	void* Address;
	unsigned long Size;
};

enum class BackupRecordStatus
{
	BackupRecordStatus_InvalidCall = -1, // 0xFFFFFFFF
	BackupRecordStatus_Valid = 0,
	BackupRecordStatus_DiffApp = 1,
	BackupRecordStatus_BrokenData = 2,
};

// Make return values global
extern bool Backup_isSetupSucceededReturnValue;
extern DWORD_PTR Backup_saveRecordReturnValue;
extern DWORD_PTR Backup_saveRecordByAddressReturnValue;
extern DWORD_PTR Backup_setupRecordsReturnValue;

BackupRecordStatus CALLPLEB Backup_getRecordStatus(DWORD_PTR recordIndex);
bool CALLPLEB Backup_isSetupSucceeded();
bool CALLPLEB Backup_saveRecord(unsigned long recordIndex);
DWORD_PTR CALLPLEB Backup_saveRecordByAddress(DWORD_PTR recordAddress);
bool CALLPLEB Backup_setupRecords(BackupRecord* records, unsigned int recordCount);