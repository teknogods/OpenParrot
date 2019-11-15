#include <windows.h>
#include <WinNT.h>
#define MAX_SECTION_NUM 20

typedef struct _PEStruct
{
	DWORD                   dwRO_first_section;
	IMAGE_DOS_HEADER        image_dos_header;
	char                    *reservedheader;
	IMAGE_NT_HEADERS        image_nt_headers;
	IMAGE_SECTION_HEADER    image_section_header[MAX_SECTION_NUM];
	char                    *image_section[MAX_SECTION_NUM];
	void					  *fileImage;
	DWORD					  fileSize;
	DWORD					  numOfSecs;
} PEStruct, *PPEStruct;

PEStruct getPEFileInformation(char *filename);