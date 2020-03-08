#pragma optimize("", off)
#include <StdInc.h>
#include "Utility/GameDetect.h"
#include "Utility/InitFunction.h"
#include "Functions/Global.h"
#if _M_IX86
using namespace std::string_literals;

void AddCommOverride(HANDLE hFile);

static HANDLE __stdcall CreateFileAWrap(LPCSTR lpFileName,
	DWORD dwDesiredAccess,
	DWORD dwShareMode,
	LPSECURITY_ATTRIBUTES lpSecurityAttributes,
	DWORD dwCreationDisposition,
	DWORD dwFlagsAndAttributes,
	HANDLE hTemplateFile)
{
	if (GameDetect::X2Type == X2Type::BG4 && lpFileName[1] == ':' && lpFileName[2] == '\\')
	{
		lpFileName += 3;
	}

	if (strnicmp(lpFileName, "D:\\", 3) == 0)
	{
		if (GetFileAttributesA(lpFileName) == INVALID_FILE_ATTRIBUTES)
		{
			wchar_t pathRoot[MAX_PATH];
			GetModuleFileNameW(GetModuleHandle(nullptr), pathRoot, _countof(pathRoot));

			wcsrchr(pathRoot, L'\\')[0] = L'\0';

			// assume just ASCII
			std::string fn = lpFileName;
			std::wstring wfn(fn.begin(), fn.end());

			CreateDirectoryW((pathRoot + L"\\TeknoParrot\\"s).c_str(), nullptr);

			return CreateFileW((pathRoot + L"\\TeknoParrot\\"s + wfn.substr(3)).c_str(),
				dwDesiredAccess,
				dwShareMode,
				lpSecurityAttributes,
				dwCreationDisposition,
				dwFlagsAndAttributes,
				hTemplateFile);
		}
	}

	if (GameDetect::X2Type == X2Type::BG4 || GameDetect::X2Type == X2Type::VRL)
	{
		if (strncmp(lpFileName, "COM1", 4) == 0)
		{
			HANDLE hFile = (HANDLE)0x8001;

			AddCommOverride(hFile);

			return hFile;
		}
	}
	return CreateFileA(lpFileName,
		dwDesiredAccess,
		dwShareMode,
		lpSecurityAttributes,
		dwCreationDisposition,
		dwFlagsAndAttributes,
		hTemplateFile);
}

#include <deque>

static std::map<HANDLE, std::deque<BYTE>> g_replyBuffers;

static BOOL __stdcall ReadFileWrapTx2(HANDLE hFile,
	LPVOID lpBuffer,
	DWORD nNumberOfBytesToRead,
	LPDWORD lpNumberOfBytesRead,
	LPOVERLAPPED lpOverlapped)
{
	if (hFile == (HANDLE)0x8001)
	{
		auto& outQueue = g_replyBuffers[hFile];

		int toRead = min(outQueue.size(), nNumberOfBytesToRead);

		std::copy(outQueue.begin(), outQueue.begin() + toRead, reinterpret_cast<uint8_t*>(lpBuffer));
		outQueue.erase(outQueue.begin(), outQueue.begin() + toRead);

		*lpNumberOfBytesRead = toRead;

		return TRUE;
	}
	return ReadFile(hFile, lpBuffer, nNumberOfBytesToRead, lpNumberOfBytesRead, lpOverlapped);
}

static DWORD __stdcall GetFileAttributesAWrapTx2(LPCSTR lpFileName)
{
	if (GameDetect::X2Type == X2Type::BG4 && lpFileName[1] == ':' && lpFileName[2] == '\\')
	{
		lpFileName += 3;
	}
	return GetFileAttributesA(lpFileName);
}

extern int* wheelSection;

BOOL __stdcall WriteFileWrapTx2(HANDLE hFile,
	LPVOID lpBuffer,
	DWORD nNumberOfBytesToWrite,
	LPDWORD lpNumberOfBytesWritten,
	LPOVERLAPPED lpOverlapped)
{
	if (hFile == (HANDLE)0x8001)
	{
		DWORD pos = 0;

		char bytes[2] = { 0 };
		uint8_t* taskBuffer = (uint8_t*)lpBuffer;

		while (pos < nNumberOfBytesToWrite)
		{
			switch (taskBuffer[pos])
			{
			case 0x20:
				g_replyBuffers[hFile].push_back(1);

				pos += 2;
				break;

			case 0x1F:
			case 0x00:
			case 0x04:
			{
				int wheelValue = *wheelSection;

				if (GameDetect::X2Type == X2Type::VRL)
				{
					wheelValue = 255 - wheelValue;
				}

				wheelValue = (int)((wheelValue / 256.0f) * 1024.0f);
				wheelValue += 1024;

				g_replyBuffers[hFile].push_back(HIBYTE(wheelValue));
				g_replyBuffers[hFile].push_back(LOBYTE(wheelValue));

				pos += 2;
				break;
			}
			default:
				pos += 2;
				break;
			}
		}

		*lpNumberOfBytesWritten = nNumberOfBytesToWrite;

		return TRUE;
	}
	return WriteFile(hFile, lpBuffer, nNumberOfBytesToWrite, lpNumberOfBytesWritten, lpOverlapped);
}

static InitFunction initFunction([]()
{
	DWORD imageBase = (DWORD)GetModuleHandleA(0);
	if (GameDetect::X2Type == X2Type::None)
	{
		return;
	}

	iatHook("kernel32.dll", CreateFileAWrap, "CreateFileA");
	
	switch (GameDetect::X2Type)
	{
		case X2Type::Wontertainment: // By 00C0FFEE
		{
			// error display routine noping
			injector::MakeNOP(0x004DE4B4, 2);
			// do not hide the cursor
			injector::WriteMemory<BYTE>(0x004E827D, 0x01, true);
			// jump from logos to main menu scene (NO ATTRACT enjoy!)
			injector::WriteMemory<BYTE>(0x004E86CB, 0x05, true);
			injector::WriteMemory<BYTE>(0x004E8C52, 0x05, true);
			injector::WriteMemory<BYTE>(0x004E8702, 0x05, true);
			// card dispenser scene bypass (cause it hangs without one)
			injector::WriteMemory<BYTE>(0x004E9391, 0x0A, true);
			injector::WriteMemory<BYTE>(0x004E93B3, 0x0A, true);
			break;
		}
		case X2Type::MB4:
		{
			// Redirect messagelog file
			injector::WriteMemoryRaw(0x00AD8B6C, ".\\messagelog.dat\0", 18, true);
			break;
		}
		case X2Type::VRL:
		{
			// TODO: DOCUMENT PATCHES
			//injector::MakeNOP(0x0040E22E, 2, true);
			injector::WriteMemory<DWORD>(0x0041C800, 0x08C2C033, true);
			injector::WriteMemory<BYTE>(0x0041C804, 0x00, true);

			iatHook("kernel32.dll", ReadFileWrapTx2, "ReadFile");
			iatHook("kernel32.dll", WriteFileWrapTx2, "WriteFile");
			break;
		}
		case X2Type::Raiden4:
		{
			// TODO: DOCUMENT PATCHES
			//injector::WriteMemory<uint32_t>(0x49DDB0, 0xC3C301B0, true);
			injector::WriteMemory<BYTE>(0x00496EA0, 0xEB, true);
			break;
		}
		case X2Type::BG4:
		{
			// TODO: DOCUMENT PATCHES
			injector::MakeNOP(0x4CBCB8, 10);
			injector::WriteMemory<uint8_t>(0x4CBCB8, 0xB8, true);
			injector::WriteMemory<uint32_t>(0x4CBCB9, 1, true);

			if (!ToBool(config["General"]["Windowed"]))
			{
				injector::MakeRET(0x5F21B0, 4);
			}

			iatHook("kernel32.dll", ReadFileWrapTx2, "ReadFile");
			iatHook("kernel32.dll", WriteFileWrapTx2, "WriteFile");
			iatHook("kernel32.dll", GetFileAttributesAWrapTx2, "GetFileAttributesA");

			break;
		}
		case X2Type::Lupin3:
		{
			// TODO: DOCUMENT PATCHES
			injector::MakeNOP(0x48C66C, 12);

			break;
		}
		case X2Type::BattleFantasia:
		{
			// TODO: DISABLE DUAL INPUT PLS
			OutputDebugStringA("Please fix the dual input issue sir");
			break;
		}
	}

	if(GameDetect::currentGame == GameID::KOFMIRA)
	{
		// TODO: DOCUMENT PATCHES
		injector::WriteMemory<DWORD>(0x0040447C, 0x000800B8, true);
		injector::WriteMemory<WORD>(0x0040447C+4, 0x9000, true);
	}

	if(GameDetect::currentGame == GameID::ChaseHq2)
	{
		// Skip calibration
		injector::WriteMemory<BYTE>(imageBase + 0x107E3, 0xEB, true);
	}
	
	if(GameDetect::currentGame == GameID::TetrisGM3)
	{
		injector::WriteMemory<DWORD>(0x0046A0AC, 0x00005C2E, true); // not sure what this is supposed to do, but won't start under TP without it.
        // working windowed mode patch.
		if (ToBool(config["General"]["Windowed"]))
		{
			injector::WriteMemory<BYTE>(0x44DCC9, 0x00, true);
		}

		// working resolution patch. Thanks, Altimoor. :)
		// the root background doesn't play too nicely with 16:9 resolutions, but they are playable
		// all 4:3 resolutions greater thean normal work as expected.
		// get resolution from config file
		// yeah i wrote ToInt. For ease of readability.
		auto resx = ToInt(config["General"]["ResolutionWidth"]);	// original arcade ran at 640
		auto resy = ToInt(config["General"]["ResolutionHeight"]); 	// original ran at 480. 	

		// and calculate aspect ratio 
		auto aspect_ratio = (float)resx / (float)resy; // hope the resolution used isn't too insane.
		// lets patch every routine that needs this. :)
		injector::WriteMemoryRaw(0x40D160, &resy, sizeof(resy), true);
		injector::WriteMemoryRaw(0x40D165, &resx, sizeof(resx), true);

		injector::WriteMemoryRaw(0x40D19A, &resy, sizeof(resy), true);
		injector::WriteMemoryRaw(0x40D19F, &resx, sizeof(resx), true);

		injector::WriteMemoryRaw(0x41F154, &resx, sizeof(resx), true);
		injector::WriteMemoryRaw(0x41F163, &resx, sizeof(resx), true);
		injector::WriteMemoryRaw(0x41F176, &resy, sizeof(resy), true);
		injector::WriteMemoryRaw(0x41F181, &resy, sizeof(resy), true);

		injector::WriteMemoryRaw(0x44DCA6, &resy, sizeof(resy), true);
		injector::WriteMemoryRaw(0x44DCAB, &resx, sizeof(resx), true);
		injector::WriteMemoryRaw(0x44DCB0, &resy, sizeof(resy), true);
		injector::WriteMemoryRaw(0x44DCB5, &resx, sizeof(resx), true);

		injector::WriteMemoryRaw(0x44DD2D, &resy, sizeof(resy), true);
		injector::WriteMemoryRaw(0x44DD32, &resx, sizeof(resx), true);
		injector::WriteMemoryRaw(0x44DD4D, &resy, sizeof(resy), true);
		injector::WriteMemoryRaw(0x44DD52, &resx, sizeof(resx), true);

		injector::WriteMemoryRaw(0x44DD6B, &aspect_ratio, sizeof(aspect_ratio), true); // aspect ratio goes here. yes, it's a raw float.

		injector::WriteMemoryRaw(0x44E126, &resy, sizeof(resy), true);
		injector::WriteMemoryRaw(0x44E12B, &resx, sizeof(resx), true);

		injector::WriteMemoryRaw(0x44E198, &resy, sizeof(resy), true);
		injector::WriteMemoryRaw(0x44E19D, &resx, sizeof(resx), true);

		injector::WriteMemoryRaw(0x44E349, &resy, sizeof(resy), true);
		injector::WriteMemoryRaw(0x44E34E, &resx, sizeof(resx), true);

		injector::WriteMemoryRaw(0x44E429, &resy, sizeof(resy), true);
		injector::WriteMemoryRaw(0x44E42E, &resx, sizeof(resx), true);

		injector::WriteMemoryRaw(0x450E5B, &resy, sizeof(resy), true);
		injector::WriteMemoryRaw(0x450E60, &resx, sizeof(resx), true);

		injector::WriteMemoryRaw(0x450E90, &resy, sizeof(resy), true);
		injector::WriteMemoryRaw(0x450E95, &resx, sizeof(resx), true);

		injector::WriteMemoryRaw(0x450ED7, &resy, sizeof(resy), true);
		injector::WriteMemoryRaw(0x450EDC, &resx, sizeof(resx), true);
		// and finally patch bookeeping to go off of game root  FIXME: work out a cleaner solution.
		injector::WriteMemoryRaw(0x46A0B0, ".\\TP_BK\\\0\0", 10, true);

	}

	if(GameDetect::currentGame == GameID::SamuraiSpiritsSen)
	{
		// TODO: DOCUMENT PATCHES
		injector::WriteMemory<DWORD>(0x0040117A, 0x90909090, true);
		injector::WriteMemory<BYTE>(0x0040117A+4, 0x90, true);
		injector::WriteMemory<WORD>(0x00401188, 0x9090, true);
		injector::WriteMemory<WORD>(0x004B73DE, 0x9090, true);
		injector::WriteMemory<BYTE>(0x004B73ED, 0xEB, true);
		injector::WriteMemory<BYTE>(0x004C640C, 0xEB, true);
		injector::WriteMemory<DWORD>(0x004CE1C0, 0xC340C033, true);
	}
});
#endif
#pragma optimize("", on)