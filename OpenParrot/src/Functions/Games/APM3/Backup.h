#ifdef _M_AMD64
#pragma once
extern char* BackupSaveFileName;
extern char fileBuffer[256];

struct BackupRecord
{
	void* Address;
	unsigned long Size;
};

extern BackupRecord* internal_Records;
extern unsigned int internal_recordCount;

enum class BackupRecordStatus
{
	BackupRecordStatus_InvalidCall = -1, // 0xFFFFFFFF
	BackupRecordStatus_Valid = 0,
	BackupRecordStatus_DiffApp = 1,
	BackupRecordStatus_BrokenData = 2,
};

static bool Backup_isSetupSucceededReturnValue = true;

static __int64 Backup_saveRecordReturnValue = 0;

static __int64 Backup_saveRecordByAddressReturnValue = 0;

static __int64 Backup_setupRecordsReturnValue = 0;

BackupRecordStatus __fastcall Backup_getRecordStatus(__int64 recordIndex);
bool Backup_isSetupSucceeded();
bool __fastcall Backup_saveRecord(unsigned long recordIndex);
__int64 __fastcall Backup_saveRecordByAddress(__int64 recordAddress);
bool __fastcall Backup_setupRecords(BackupRecord* records, unsigned int recordCount);

#endif