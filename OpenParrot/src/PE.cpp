#include "PE.H"
DWORD PEAlign(DWORD dwTarNum, DWORD dwAlignTo);
PEStruct getPEFileInformation(char *filename)
{
	HANDLE hFile;
	PEStruct pRetnStruct;
	pRetnStruct.fileSize = 0;
	DWORD dwBytesRead;
	hFile = CreateFileA(filename, GENERIC_READ, FILE_SHARE_READ, NULL,
		OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (!hFile)
		return pRetnStruct;

	pRetnStruct.fileSize = GetFileSize(hFile, 0);
	if (!pRetnStruct.fileSize)
		return pRetnStruct;
	pRetnStruct.fileImage = malloc(pRetnStruct.fileSize);
	if (!pRetnStruct.fileImage)
		return pRetnStruct;
	ReadFile(hFile, pRetnStruct.fileImage, pRetnStruct.fileSize, &dwBytesRead, NULL);
	CloseHandle(hFile);
	if (!dwBytesRead)
		return pRetnStruct;
	//copy portions to relevant sections
	CopyMemory(&pRetnStruct.image_dos_header, pRetnStruct.fileImage, sizeof(IMAGE_DOS_HEADER));
	CopyMemory(&pRetnStruct.image_nt_headers,
		((BYTE *)pRetnStruct.fileImage + pRetnStruct.image_dos_header.e_lfanew), sizeof(IMAGE_NT_HEADERS));
	//address of first section
	pRetnStruct.dwRO_first_section = pRetnStruct.image_dos_header.e_lfanew + sizeof(IMAGE_NT_HEADERS);
	pRetnStruct.numOfSecs = pRetnStruct.image_nt_headers.FileHeader.NumberOfSections;

	CopyMemory(&pRetnStruct.image_section_header, ((BYTE *)pRetnStruct.fileImage + pRetnStruct.dwRO_first_section),
		pRetnStruct.numOfSecs*sizeof(IMAGE_SECTION_HEADER));
	//now to fill in individual sections (.text .data)
	for (int i = 0; i < pRetnStruct.numOfSecs; i++)
	{
		pRetnStruct.image_section[i] = (char *)malloc(PEAlign(pRetnStruct.image_section_header[i].SizeOfRawData,
			pRetnStruct.image_nt_headers.OptionalHeader.FileAlignment));
		CopyMemory(pRetnStruct.image_section[i], ((BYTE *)pRetnStruct.fileImage + pRetnStruct.image_section_header[i].PointerToRawData),
			pRetnStruct.image_section_header[i].SizeOfRawData);
	}
	return pRetnStruct;
}

//googled function to correct file alignement in PE header.
//Just makes sure the outputted number will align correctly with the rest of the data.
DWORD PEAlign(DWORD dwTarNum, DWORD dwAlignTo)
{
	DWORD dwtemp;
	dwtemp = dwTarNum / dwAlignTo;
	if ((dwTarNum%dwAlignTo) != 0)
	{
		dwtemp++;
	}
	dwtemp = dwtemp*dwAlignTo;
	return(dwtemp);
}