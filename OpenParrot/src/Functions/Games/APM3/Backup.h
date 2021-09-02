#pragma once
#ifdef _M_IX86
#define CALLPLEB __stdcall
#else
#define CALLPLEB __fastcall
#endif
extern char* BackupSaveFileName;
extern char fileBuffer[256];

struct BackupRecord
{
	void *Address;
	unsigned long Size;
};

enum class BackupRecordStatus
{
	BackupRecordStatus_InvalidCall = -1, // 0xFFFFFFFF
	BackupRecordStatus_Valid = 0,
	BackupRecordStatus_DiffApp = 1,
	BackupRecordStatus_BrokenData = 2,
};

static bool Backup_isSetupSucceededReturnValue = true;

static DWORD_PTR Backup_saveRecordReturnValue = 0;

static DWORD_PTR Backup_saveRecordByAddressReturnValue = 0;

static DWORD_PTR Backup_setupRecordsReturnValue = 0;

BackupRecordStatus CALLPLEB Backup_getRecordStatus(DWORD_PTR recordIndex);
bool CALLPLEB Backup_isSetupSucceeded();
bool CALLPLEB Backup_saveRecord(unsigned long recordIndex);
DWORD_PTR CALLPLEB Backup_saveRecordByAddress(DWORD_PTR recordAddress);
bool CALLPLEB Backup_setupRecords(BackupRecord* records, unsigned int recordCount);