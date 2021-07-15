#include <Utility/InitFunction.h>
#include <StdInc.h>
#include <thread>
#ifdef _M_AMD64
#include "Functions\Global.h"
#include "Backup.h"

char* BackupSaveFileName = "save";
char fileBuffer[256];
BackupRecord* internal_Records;
unsigned int internal_recordCount;

BackupRecordStatus __fastcall Backup_getRecordStatus(__int64 recordIndex)
{
#ifdef _LOGAPM3BACKUP
	info(true, "Backup_getRecordStatus %llx", recordIndex);
#endif
	return BackupRecordStatus::BackupRecordStatus_Valid;
}

bool Backup_isSetupSucceeded()
{
#ifdef _LOGAPM3BACKUP
	info(true, "Backup_isSetupSucceeded");
#endif
	return Backup_isSetupSucceededReturnValue;
}

bool __fastcall Backup_saveRecord(unsigned long recordIndex)
{
#ifdef _LOGAPM3BACKUP
	info(true, "Backup_saveRecord recordIndex: %llx", recordIndex);
#endif
	memset(fileBuffer, 0, sizeof(fileBuffer));
	sprintf(fileBuffer, "%s%02d.bin", BackupSaveFileName, recordIndex);
	auto file = fopen(fileBuffer, "wb+");
	if (file)
	{
		fwrite(internal_Records[recordIndex].Address, 1, internal_Records[recordIndex].Size, file);
		fclose(file);
	}
	return Backup_saveRecordReturnValue;
}

__int64 __fastcall Backup_saveRecordByAddress(__int64 recordAddress)
{
#ifdef _LOGAPM3BACKUP
	info(true, "Backup_saveRecordByAddress %llx", recordAddress);
#endif
	return Backup_saveRecordByAddressReturnValue;
}




//__int64 __fastcall Backup_setupRecords(__int128* a1, unsigned int a2)
bool __fastcall Backup_setupRecords(BackupRecord* records, unsigned int recordCount)
{
	for (int i = 0; i < recordCount; i++)
	{
		memset(fileBuffer, 0, sizeof(fileBuffer));
		sprintf(fileBuffer, "%s%02d.bin", BackupSaveFileName, i);
		FILE* fsave = fopen(fileBuffer, "r");
		internal_Records = records;
		internal_recordCount = recordCount;
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
#endif