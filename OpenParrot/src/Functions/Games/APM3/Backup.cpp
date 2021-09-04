#include <Utility/InitFunction.h>
#include <StdInc.h>
#include <thread>
#include "Functions\Global.h"
#include "Backup.h"

char* BackupSaveFileName = "save";
char fileBuffer[256];
unsigned int internal_recordCount;

// Prefill some memory for save pointers, only 2-3 are used typically. Can be made dynamic, but this is for testing and seeing live data rn.
static auto backups = new BackupRecord[64];

// Define default return values here
bool Backup_isSetupSucceededReturnValue = true;
DWORD_PTR Backup_saveRecordReturnValue = 0;
DWORD_PTR Backup_saveRecordByAddressReturnValue = 0;
DWORD_PTR Backup_setupRecordsReturnValue = 0;

BackupRecordStatus CALLPLEB Backup_getRecordStatus(DWORD_PTR recordIndex)
{
#ifdef _LOGAPM3BACKUP
	info(true, "Backup_getRecordStatus %llx", recordIndex);
#endif
	return BackupRecordStatus::BackupRecordStatus_Valid;
}

bool CALLPLEB Backup_isSetupSucceeded()
{
#ifdef _LOGAPM3BACKUP
	info(true, "Backup_isSetupSucceeded");
#endif
	return Backup_isSetupSucceededReturnValue;
}

bool CALLPLEB Backup_saveRecord(unsigned long recordIndex)
{
#ifdef _LOGAPM3BACKUP
	info(true, "Backup_saveRecord recordIndex: %llx", recordIndex);
#endif
	memset(fileBuffer, 0, sizeof(fileBuffer));
	sprintf(fileBuffer, "%s%02d.bin", BackupSaveFileName, recordIndex);
	auto file = fopen(fileBuffer, "wb+");
	if (file)
	{
		fwrite(backups[recordIndex].Address, 1, backups[recordIndex].Size, file);
		fclose(file);
	}
	return Backup_saveRecordReturnValue;
}

DWORD_PTR CALLPLEB Backup_saveRecordByAddress(DWORD_PTR recordAddress)
{
#ifdef _LOGAPM3BACKUP
	info(true, "Backup_saveRecordByAddress %llx", recordAddress);
#endif
	return Backup_saveRecordByAddressReturnValue;
}

//__int64 __fastcall Backup_setupRecords(__int128* a1, unsigned int a2)
bool CALLPLEB Backup_setupRecords(BackupRecord* records, unsigned int recordCount)
{
	for (int i = 0; i < recordCount; i++)
	{
		backups[i].Address = records[i].Address;
		backups[i].Size = records[i].Size;
	}

	internal_recordCount = recordCount;

	for (int i = 0; i < recordCount; i++)
	{
		memset(fileBuffer, 0, sizeof(fileBuffer));
		sprintf(fileBuffer, "%s%02d.bin", BackupSaveFileName, i);
		FILE* fsave = fopen(fileBuffer, "r");
		if (fsave != NULL)
		{
#ifdef _LOGAPM3BACKUP
			info(true, "Backup setuprecords %02d of %02d, loading file %s", i, recordCount, fileBuffer);
#endif		
			fread(records[i].Address, 1, records[i].Size, fsave); // add file size check noob
			fclose(fsave);
		}
		else
		{
#ifdef _LOGAPM3BACKUP
			info(true, "Backup setuprecords %02d of %02d, saving file %s", i, recordCount, fileBuffer);
#endif
			auto file = fopen(fileBuffer, "wb+");
			if (file)
			{
				fwrite(records[i].Address, 1, records[i].Size, file);
				fclose(file);
			}
		}
	}
	return true;
}
