#include "Utils.h"
#include <windows.h>
#include <string>
#pragma comment(lib, "version.lib")

size_t GetByteSizeOfWchar(const wchar_t* str)
{
	return (wcslen(str) + 1) * sizeof(wchar_t);
}

wchar_t* GetLastErrorAsString()
{
	//Get the error message ID, if any.
	DWORD errorMessageID = ::GetLastError();

	if (errorMessageID == 0)
		return L""; //No error message has been recorded

	static LPWSTR messageBuffer = nullptr;

	//Ask Win32 to give us the string version of that message ID.
	//The parameters we pass in, tell Win32 to create the buffer that holds the message for us (because we don't yet know how long the message string will be).
	size_t size = FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS | FORMAT_MESSAGE_MAX_WIDTH_MASK,
		NULL, errorMessageID, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPWSTR)&messageBuffer, 0, NULL);

	return messageBuffer;
}

// Totally not stolen of stackoverflow
wchar_t* GetFileVersion(const wchar_t* pszFilePath)
{
	DWORD dwSize = 0;
	BYTE* pbVersionInfo = NULL;
	VS_FIXEDFILEINFO* pFileInfo = NULL;
	UINT puLenFileInfo = 0;
	static wchar_t versionString[64];

	// Get the version information for the file requested
	dwSize = GetFileVersionInfoSize(pszFilePath, NULL);
	if (dwSize == 0)
	{
		//wprintf(L"Error in GetFileVersionInfoSize: %d\n", GetLastError());
		return L"0.0.0.0";
	}

	pbVersionInfo = new BYTE[dwSize];

	if (!GetFileVersionInfo(pszFilePath, 0, dwSize, pbVersionInfo))
	{
		//wprintf(L"Error in GetFileVersionInfo: %d\n", GetLastError());
		delete[] pbVersionInfo;
		return L"0.0.0.0";
	}

	if (!VerQueryValue(pbVersionInfo, TEXT("\\"), (LPVOID*)&pFileInfo, &puLenFileInfo))
	{
		//wprintf(L"Error in VerQueryValue: %d\n", GetLastError());
		delete[] pbVersionInfo;
		return L"0.0.0.0";
	}

	swprintf(versionString, 64, L"%d.%d.%d.%d",
		(pFileInfo->dwFileVersionMS >> 16) & 0xFFFF,
		(pFileInfo->dwFileVersionMS >> 0) & 0xFFFF,
		(pFileInfo->dwFileVersionLS >> 16) & 0xFFFF,
		(pFileInfo->dwFileVersionLS >> 0) & 0xFFFF
	);

	return versionString;
}