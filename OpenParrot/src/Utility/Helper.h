#pragma once
#include <Windows.h>

class Helpers {
public:
	// reading/writing memory
	LPVOID GetTranslatedOffset(INT_PTR offset);
	int ReadInt32(INT_PTR offset, bool isRelativeOffset);
	UINT8 ReadByte(INT_PTR offset, bool isRelativeOffset);
	float WriteFloat32(INT_PTR offset, float val, bool isRelativeOffset);
	UINT8 WriteByte(INT_PTR offset, UINT8 val, bool isRelativeOffset);
	INT_PTR WriteIntPtr(INT_PTR offset, INT_PTR val, bool isRelativeOffset);
	INT_PTR ReadIntPtr(INT_PTR offset, bool isRelativeOffset);
	float ReadFloat32(INT_PTR offset, bool isRelativeOffset);
};
