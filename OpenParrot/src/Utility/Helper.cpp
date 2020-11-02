#include <Windows.h>
#include "Helper.h"

typedef unsigned char U8;

// reading/writing memory
LPVOID Helpers::GetTranslatedOffset(INT_PTR offset)
{
	return reinterpret_cast<LPVOID>((INT_PTR)GetModuleHandle(NULL) + offset);
}

UINT8 Helpers::ReadByte(INT_PTR offset, bool isRelativeOffset)
{
	UINT8 val = 0;
	SIZE_T read;
	LPVOID trueOffset = (isRelativeOffset ? GetTranslatedOffset(offset) : (LPVOID)offset);
	ReadProcessMemory(GetCurrentProcess(), trueOffset, &val, sizeof(UINT8), &read);
	return val;
}

float Helpers::WriteFloat32(INT_PTR offset, float val, bool isRelativeOffset)
{
	SIZE_T written;
	LPVOID trueOffset = (isRelativeOffset ? GetTranslatedOffset(offset) : (LPVOID)offset);
	WriteProcessMemory(GetCurrentProcess(), trueOffset, &val, sizeof(float), &written);
	return val;
};

UINT8 Helpers::WriteByte(INT_PTR offset, UINT8 val, bool isRelativeOffset)
{
	SIZE_T written;
	LPVOID trueOffset = (isRelativeOffset ? GetTranslatedOffset(offset) : (LPVOID)offset);
	WriteProcessMemory(GetCurrentProcess(), trueOffset, &val, sizeof(UINT8), &written);
	return val;
}

INT_PTR Helpers::WriteIntPtr(INT_PTR offset, INT_PTR val, bool isRelativeOffset)
{
	SIZE_T written;
	LPVOID trueOffset = (isRelativeOffset ? GetTranslatedOffset(offset) : (LPVOID)offset);
	WriteProcessMemory(GetCurrentProcess(), trueOffset, &val, sizeof(INT_PTR), &written);
	return val;
};

int Helpers::ReadInt32(INT_PTR offset, bool isRelativeOffset)
{
	int val = 0;
	SIZE_T read;
	LPVOID trueOffset = (isRelativeOffset ? GetTranslatedOffset(offset) : (LPVOID)offset);
	ReadProcessMemory(GetCurrentProcess(), trueOffset, &val, sizeof(int), &read);
	return val;
}

INT_PTR Helpers::ReadIntPtr(INT_PTR offset, bool isRelativeOffset)
{
	SIZE_T read;
	LPVOID trueOffset = (isRelativeOffset ? GetTranslatedOffset(offset) : (LPVOID)offset);
	INT_PTR val;
	ReadProcessMemory(GetCurrentProcess(), trueOffset, &val, sizeof(INT_PTR), &read);
	return val;
};

float Helpers::ReadFloat32(INT_PTR offset, bool isRelativeOffset)
{
	float val = 0.0f;
	SIZE_T read;
	LPVOID trueOffset = (isRelativeOffset ? GetTranslatedOffset(offset) : (LPVOID)offset);
	ReadProcessMemory(GetCurrentProcess(), trueOffset, &val, sizeof(float), &read);
	return val;
};