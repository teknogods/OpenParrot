#pragma once

BOOL WINAPI ReadFileHooked(_In_ HANDLE hFile, _Out_writes_bytes_to_opt_(nNumberOfBytesToRead, *lpNumberOfBytesRead) __out_data_source(FILE) LPVOID lpBuffer, _In_ DWORD nNumberOfBytesToRead, _Out_opt_ LPDWORD lpNumberOfBytesRead, _Inout_opt_ LPOVERLAPPED lpOverlapped);
void *__cdecl memcpy_0(void *a1, const void *a2, size_t a3);
DWORD FetchDwordInformation(const char* setting, const char* subkey, DWORD defaultValue);

DWORD genericRetZero();

struct windowHooks
{
	int createWindowExA;
	int createWindowExW;
	int adjustWindowRect;
	int adjustWindowRectEx;
	int setWindowPos;
	int changeDisplaySettings;
	int changeDisplaySettingsExW;
	int updateWindow;
	int clipCursor;
	int setCursorPos;
};

void init_windowHooks(windowHooks* data);
std::wstring utf8_decode(const std::string &str);

namespace
{
	template<typename T>
	inline T* getRVA(uintptr_t rva)
	{
		return (T*)((char*)GetModuleHandle(nullptr) + rva);
	}

	template<typename TOrdinal>
	inline bool iat_matches_ordinal(uintptr_t* nameTableEntry, TOrdinal ordinal)
	{

	}

	template<>
	inline bool iat_matches_ordinal(uintptr_t* nameTableEntry, int ordinal)
	{
		if (IMAGE_SNAP_BY_ORDINAL(*nameTableEntry))
		{
			return IMAGE_ORDINAL(*nameTableEntry) == ordinal;
		}

		return false;
	}

	template<>
	inline bool iat_matches_ordinal(uintptr_t* nameTableEntry, const char* ordinal)
	{
		if (!IMAGE_SNAP_BY_ORDINAL(*nameTableEntry))
		{
			auto import = getRVA<IMAGE_IMPORT_BY_NAME>(*nameTableEntry);

			return !_stricmp(import->Name, ordinal);
		}

		return false;
	}
}

template<typename T, typename TOrdinal>
T iatHook(const char* moduleName, T function, TOrdinal ordinal)
{
	IMAGE_DOS_HEADER* imageHeader = (IMAGE_DOS_HEADER*)(GetModuleHandle(nullptr));
	IMAGE_NT_HEADERS* ntHeader = getRVA<IMAGE_NT_HEADERS>(imageHeader->e_lfanew);

	IMAGE_IMPORT_DESCRIPTOR* descriptor = getRVA<IMAGE_IMPORT_DESCRIPTOR>(ntHeader->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress);

	while (descriptor->Name)
	{
		const char* name = getRVA<char>(descriptor->Name);

		if (_stricmp(name, moduleName))
		{
			descriptor++;

			continue;
		}

		if (descriptor->OriginalFirstThunk == 0)
		{
			return nullptr;
		}

		auto nameTableEntry = getRVA<uintptr_t>(descriptor->OriginalFirstThunk);
		auto addressTableEntry = getRVA<uintptr_t>(descriptor->FirstThunk);

		while (*nameTableEntry)
		{
			if (iat_matches_ordinal(nameTableEntry, ordinal))
			{
				T origEntry = (T)*addressTableEntry;
				injector::WriteMemory(addressTableEntry, (uintptr_t)function, true);

				return origEntry;
			}

			nameTableEntry++;
			addressTableEntry++;
		}

		return nullptr;
	}

	return nullptr;
}