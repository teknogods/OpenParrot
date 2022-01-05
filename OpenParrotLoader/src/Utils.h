#pragma once

#include <windows.h>
#include <string>

size_t GetByteSizeOfWchar(const wchar_t* str);
wchar_t* GetLastErrorAsString();
wchar_t* GetFileVersion(const wchar_t* pszFilePath);