#pragma optimize("", off)
#include <StdInc.h>
#include "Utility/GameDetect.h"
#include "Utility/InitFunction.h"
#include "Functions/Global.h"
#include "Utility/Helper.h"
#if _M_IX86
using namespace std::string_literals;
extern int* wheelSection;
extern int* ffbOffset;
extern int* ffbOffset3;
extern int* ffbOffset4;
extern int* ffbOffset5;
static Helpers* myHelpers;

static bool CoinPressed = false;
static bool TestPressed = false;
static bool ServicePressed = false;
static bool TimeStartPressed = false;
static bool StartPressed = false;
static bool ViewPressed = false;
static bool HazardPressed = false;
static bool OverTakePressed = false;
static bool ShiftLeftPressed = false;
static bool ShiftRightPressed = false;
static bool SideBrakePressed = false;
static bool SeatSwitch1Pressed = false;
static bool SeatSwitch2Pressed = false;
static bool Gear1Pressed = false;
static bool Gear2Pressed = false;
static bool Gear3Pressed = false;
static bool Gear4Pressed = false;
static bool Gear5Pressed = false;
static bool Gear6Pressed = false;
static bool KeyPressed = false;
static bool init = false;
static bool MenuHack = false;
static bool MenuHackDelay = false;
static bool TestMode = false;

void AddCommOverride(HANDLE hFile);

static HANDLE __stdcall CreateFileAWrap(LPCSTR lpFileName,
	DWORD dwDesiredAccess,
	DWORD dwShareMode,
	LPSECURITY_ATTRIBUTES lpSecurityAttributes,
	DWORD dwCreationDisposition,
	DWORD dwFlagsAndAttributes,
	HANDLE hTemplateFile)
{
	if ((GameDetect::X2Type == X2Type::BG4 || GameDetect::X2Type == X2Type::BG4_Eng) && lpFileName[1] == ':' && lpFileName[2] == '\\')
	{
		lpFileName += 3; // apparently this game has absolute paths for game files, so correct them to relative paths from game exe directory.
	}
	// catch absolute paths outside of game directory wherever they are, and redirect them
	if (lpFileName[1] == ':') // it's an absolute path if its second character is a :. :)
	{
		if (GetFileAttributesA(lpFileName) == INVALID_FILE_ATTRIBUTES) // don't need to redirect if the file is present. 
		{
			wchar_t pathRoot[MAX_PATH];
			GetModuleFileNameW(GetModuleHandle(nullptr), pathRoot, _countof(pathRoot)); // get full pathname to game executable

			wcsrchr(pathRoot, L'\\')[0] = L'\0'; // chop off everything from the last backslash.

			// assume just ASCII
			std::string fn = lpFileName;
			std::wstring wfn(fn.begin(), fn.end());

			CreateDirectoryW((pathRoot + L"\\TeknoParrot\\"s).c_str(), nullptr); // create TeknoParrot subdirectory off of launcher's root directory to cleanly store data seperate from rest of files.

			return CreateFileW((pathRoot + L"\\TeknoParrot\\"s + wfn.substr(3)).c_str(), // make or open the file there instead. :)
				dwDesiredAccess,
				dwShareMode,
				lpSecurityAttributes,
				dwCreationDisposition,
				dwFlagsAndAttributes,
				hTemplateFile);
		}
	}

	if (GameDetect::X2Type == X2Type::BG4 || GameDetect::X2Type == X2Type::BG4_Eng || GameDetect::X2Type == X2Type::VRL)
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
	if ((GameDetect::X2Type == X2Type::BG4 || GameDetect::X2Type == X2Type::BG4_Eng) && lpFileName[1] == ':' && lpFileName[2] == '\\')
	{
		lpFileName += 3;
	}
	return GetFileAttributesA(lpFileName);
}

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

static DWORD WINAPI ChangeValues(LPVOID lpParam)
{
	Sleep(10000);

	DWORD imageBase = (DWORD)GetModuleHandleA(0);
	myHelpers->WriteByte(0x42E296, 0x01, true);
	myHelpers->WriteByte(0x42E295, 0x80, true);
	injector::MakeNOP(imageBase + 0x27400, 6);
	return 0;
}

static int BG4ThreadLoop(Helpers* helpers)
{
	//Hack to allow us to select Manual		
	INT_PTR MenuTimerBase = helpers->ReadIntPtr(0x4C2924, true);
	INT_PTR MenuTimerBaseA = helpers->ReadIntPtr(MenuTimerBase + 0x08, false);
	INT_PTR MenuTime = helpers->ReadIntPtr(MenuTimerBaseA + 0x45C, false);

	if (MenuTime == 0x1194)
	{
		if (!MenuHack)
		{
			MenuHack = true;
		}
	}

	if (MenuTime == 0x00)
	{
		if (MenuHack)
		{
			MenuHack = false;
			MenuHackDelay = false;
		}
	}

	if (MenuHack)
	{
		if (!MenuHackDelay)
		{
			MenuHackDelay = true;
			Sleep(2500);
		}	
		helpers->WriteByte(MenuTimerBaseA + 0x454, 0x03, false);
	}
	return 0;
}

static int BG4ProThreadLoop(Helpers* helpers)
{
	if (!init)
	{
		init = true;
		myHelpers = helpers;
		CreateThread(NULL, 0, ChangeValues, NULL, 0, NULL);
	}

	//Hack to allow us to select Manual and Manual with Clutch		
	INT_PTR MenuTimerBase = helpers->ReadIntPtr(0x4C2924, true);
	INT_PTR MenuTimerBaseA = helpers->ReadIntPtr(MenuTimerBase + 0x08, false);
	INT_PTR MenuTime = helpers->ReadIntPtr(MenuTimerBaseA + 0x45C, false);

	if (MenuTime == 0x1194)
	{
		if (!MenuHack)
		{
			MenuHack = true;
		}
	}

	if (MenuTime == 0x00)
	{
		if (MenuHack)
		{
			MenuHack = false;
			MenuHackDelay = false;
		}
	}

	if (MenuHack)
	{
		if (!MenuHackDelay)
		{
			MenuHackDelay = true;
			Sleep(2500);
		}
		helpers->WriteByte(MenuTimerBaseA + 0x454, 0x04, false);
		BYTE This = helpers->ReadByte(MenuTimerBaseA + 0x44C,false);

		if (This == 0x02)
		{
			helpers->WriteByte(0x42E341, 0xD0, true);  //Set Shift SEN 2 to ON or error
		}
		else if (This == 0x03)
		{
			helpers->WriteByte(0x42E341, 0xE0, true);  //Set Shift SEN 1 to ON or error
		}
	}

	DWORD imageBase = (DWORD)GetModuleHandleA(0);

	if (*ffbOffset & 0x100) //Test
	{
		if (!TestPressed)
		{
			TestPressed = true;
			TestMode = true;
			*(BYTE*)(imageBase + 0x42E297) += 0x80;
		}
	}
	else
	{
		if (TestPressed)
		{
			TestPressed = false;
			*(BYTE*)(imageBase + 0x42E297) -= 0x80;
		}
	}

	if (*ffbOffset & 0x200) //Service
	{
		if (!ServicePressed)
		{
			ServicePressed = true;
			*(BYTE*)(imageBase + 0x42E296) += 0x40;
		}
	}
	else
	{
		if (ServicePressed)
		{
			ServicePressed = false;
			*(BYTE*)(imageBase + 0x42E296) -= 0x40;
		}
	}

	if (*ffbOffset & 0x800) //Shift Left
	{
		if (!ShiftLeftPressed)
		{
			ShiftLeftPressed = true;
			*(BYTE*)(imageBase + 0x42E341) -= 0x80;
		}
	}
	else
	{
		if (ShiftLeftPressed)
		{
			ShiftLeftPressed = false;
			*(BYTE*)(imageBase + 0x42E341) += 0x80;
		}
	}

	if (*ffbOffset & 0x4000) //Shift Right
	{
		if (!ShiftRightPressed)
		{
			ShiftRightPressed = true;
			*(BYTE*)(imageBase + 0x42E341) -= 0x40;
		}
	}
	else
	{
		if (ShiftRightPressed)
		{
			ShiftRightPressed = false;
			*(BYTE*)(imageBase + 0x42E341) += 0x40;
		}
	}

	if (*ffbOffset & 0x40) //Start
	{
		if (!StartPressed)
		{
			StartPressed = true;
			*(BYTE*)(imageBase + 0x42E296) += 0x80;
		}
	}
	else
	{
		if (StartPressed)
		{
			StartPressed = false;
			*(BYTE*)(imageBase + 0x42E296) -= 0x80;
		}
	}

	if (*ffbOffset & 0x8000) //View
	{
		if (!ViewPressed)
		{
			ViewPressed = true;
			*(BYTE*)(imageBase + 0x42E296) += 0x20;
		}
	}
	else
	{
		if (ViewPressed)
		{
			ViewPressed = false;
			*(BYTE*)(imageBase + 0x42E296) -= 0x20;
		}
	}

	if (*ffbOffset & 0x01) //Hazard
	{
		if (!HazardPressed)
		{
			HazardPressed = true;
			*(BYTE*)(imageBase + 0x42E296) += 0x10;
		}
	}
	else
	{
		if (HazardPressed)
		{
			HazardPressed = false;
			*(BYTE*)(imageBase + 0x42E296) -= 0x10;
		}
	}

	if (*ffbOffset & 0x20) //Overtake
	{
		if (!OverTakePressed)
		{
			OverTakePressed = true;
			*(BYTE*)(imageBase + 0x42E296) += 0x08;
		}
	}
	else
	{
		if (OverTakePressed)
		{
			OverTakePressed = false;
			*(BYTE*)(imageBase + 0x42E296) -= 0x08;
		}
	}

	if (*ffbOffset & 0x02) //SideBrake
	{
		if (!SideBrakePressed)
		{
			SideBrakePressed = true;
			*(BYTE*)(imageBase + 0x42E296) += 0x02;
		}
	}
	else
	{
		if (SideBrakePressed)
		{
			SideBrakePressed = false;
			*(BYTE*)(imageBase + 0x42E296) -= 0x02;
		}
	}

	if (*ffbOffset & 0x10000) //Timer Start
	{
		if (!TimeStartPressed)
		{
			TimeStartPressed = true;
			*(BYTE*)(imageBase + 0x42E295) += 0x01;
		}
	}
	else
	{
		if (TimeStartPressed)
		{
			TimeStartPressed = false;
			*(BYTE*)(imageBase + 0x42E295) -= 0x01;
		}
	}

	if (*ffbOffset & 0x20000) //Seat Switch 1
	{
		if (!SeatSwitch1Pressed)
		{
			SeatSwitch1Pressed = true;
			*(BYTE*)(imageBase + 0x42E295) += 0x04;
		}
	}
	else
	{
		if (SeatSwitch1Pressed)
		{
			SeatSwitch1Pressed = false;
			*(BYTE*)(imageBase + 0x42E295) -= 0x04;
		}
	}

	if (*ffbOffset & 0x40000) //Seat Switch 2
	{
		if (!SeatSwitch2Pressed)
		{
			SeatSwitch2Pressed = true;
			*(BYTE*)(imageBase + 0x42E295) += 0x02;
		}
	}
	else
	{
		if (SeatSwitch2Pressed)
		{
			SeatSwitch2Pressed = false;
			*(BYTE*)(imageBase + 0x42E295) -= 0x02;
		}
	}

	if (*ffbOffset & 0x400) //Coin Chute 1
	{
		if (!CoinPressed)
		{
			CoinPressed = true; 
			*(BYTE*)(imageBase + 0x42E294) += 0x40;
			if (!TestMode) //Let's get that sweet sweet sexy coin sound
			{
				*(BYTE*)(imageBase + 0x42E296) += 0x40;
			}
		}
	}
	else
	{
		if (CoinPressed)
		{
			CoinPressed = false;
			*(BYTE*)(imageBase + 0x42E294) -= 0x40;
			if (!TestMode)
			{
				*(BYTE*)(imageBase + 0x42E296) -= 0x40;
			}
		}
	}

	if (*ffbOffset & 0x2000) //Key
	{
		if (!KeyPressed)
		{
			KeyPressed = true;
			*(BYTE*)(imageBase + 0x42E296) += 0x04;
		}
	}
	else
	{
		if (KeyPressed)
		{
			KeyPressed = false;
			*(BYTE*)(imageBase + 0x42E296) -= 0x04;
		}
	}

	if (*ffbOffset & 0x100000) //Gear 1
	{
		if ((!Gear1Pressed) && (!Gear2Pressed) && (!Gear3Pressed) && (!Gear4Pressed) && (!Gear5Pressed) && (!Gear6Pressed))
		{
			Gear1Pressed = true;
			*(BYTE*)(imageBase + 0x42E295) -= 0x80;
			*(BYTE*)(imageBase + 0x42E341) -= 0x80;
		}
	}
	else
	{
		if (Gear1Pressed)
		{
			Gear1Pressed = false;
			*(BYTE*)(imageBase + 0x42E295) += 0x80;
			*(BYTE*)(imageBase + 0x42E341) += 0x80;
		}
	}

	if (*ffbOffset & 0x200000) //Gear 2
	{
		if ((!Gear2Pressed) && (!Gear1Pressed) && (!Gear3Pressed) && (!Gear4Pressed) && (!Gear5Pressed) && (!Gear6Pressed))
		{
			Gear2Pressed = true;
			*(BYTE*)(imageBase + 0x42E296) -= 0x01;
			*(BYTE*)(imageBase + 0x42E341) -= 0x80;
		}
	}
	else
	{
		if (Gear2Pressed)
		{
			Gear2Pressed = false;
			*(BYTE*)(imageBase + 0x42E296) += 0x01;
			*(BYTE*)(imageBase + 0x42E341) += 0x80;
		}
	}

	if (*ffbOffset & 0x08) //Gear 3 / Shift Up
	{
		if ((!Gear3Pressed) && (!Gear2Pressed) && (!Gear1Pressed) && (!Gear4Pressed) && (!Gear5Pressed) && (!Gear6Pressed))
		{
			Gear3Pressed = true;
			*(BYTE*)(imageBase + 0x42E296) -= 0x01;
		}
	}
	else
	{
		if (Gear3Pressed)
		{
			Gear3Pressed = false;
			*(BYTE*)(imageBase + 0x42E296) += 0x01;
		}
	}

	if (*ffbOffset & 0x04) //Gear 4 / Shift Down
	{
		if ((!Gear4Pressed) && (!Gear2Pressed) && (!Gear3Pressed) && (!Gear1Pressed) && (!Gear5Pressed) && (!Gear6Pressed))
		{
			Gear4Pressed = true;
			*(BYTE*)(imageBase + 0x42E295) -= 0x80;
		}
	}
	else
	{
		if (Gear4Pressed)
		{
			Gear4Pressed = false;
			*(BYTE*)(imageBase + 0x42E295) += 0x80;
		}
	}

	if (*ffbOffset & 0x1000000) //Gear 5
	{
		if ((!Gear5Pressed) && (!Gear2Pressed) && (!Gear3Pressed) && (!Gear4Pressed) && (!Gear1Pressed) && (!Gear6Pressed))
		{
			Gear5Pressed = true;
			*(BYTE*)(imageBase + 0x42E295) -= 0x80;
			*(BYTE*)(imageBase + 0x42E341) -= 0x40;
		}
	}
	else
	{
		if (Gear5Pressed)
		{
			Gear5Pressed = false;
			*(BYTE*)(imageBase + 0x42E295) += 0x80;
			*(BYTE*)(imageBase + 0x42E341) += 0x40;
		}
	}

	if (*ffbOffset & 0x2000000) //Gear 6/Reverse
	{
		if ((!Gear6Pressed) && (!Gear2Pressed) && (!Gear3Pressed) && (!Gear4Pressed) && (!Gear5Pressed) && (!Gear1Pressed))
		{
			Gear6Pressed = true;
			*(BYTE*)(imageBase + 0x42E296) -= 0x01;
			*(BYTE*)(imageBase + 0x42E341) -= 0x40;
		}
	}
	else
	{
		if (Gear6Pressed)
		{
			Gear6Pressed = false;
			*(BYTE*)(imageBase + 0x42E296) += 0x01;
			*(BYTE*)(imageBase + 0x42E341) += 0x40;
		}
	}

	*(float*)(imageBase + 0x42D994) = (*wheelSection - 128) / 128.0; // Steering
	*(float*)(imageBase + 0x42D998) = (*ffbOffset3) / 255.0; // Gas
	*(float*)(imageBase + 0x42D99C) = (*ffbOffset4) / 255.0; // Brake
	*(float*)(imageBase + 0x42D9A0) = (*ffbOffset5) / 255.0; // Clutch

	return 0;
}

static DWORD WINAPI BG4ProRunningLoop(LPVOID lpParam)
{
	while (true)
	{
		BG4ProThreadLoop(0);
		Sleep(16);
	}
}

static DWORD WINAPI BG4RunningLoop(LPVOID lpParam)
{
	while (true)
	{
		BG4ThreadLoop(0);
		Sleep(16);
	}
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

			// redirect E:\data to .\data
			injector::WriteMemoryRaw(0x0076D96C, "./data/", 8, true);
			injector::WriteMemoryRaw(0x007ACA60, ".\\data", 7, true);

			if (ToBool(config["General"]["IntroFix"]))
			{
				// thanks for Ducon2016 for the patch!
				injector::WriteMemoryRaw(imageBase + 0x57ACE, "\x89\x68\x14\xC7\x40\x08\x00\x00\x80\x3F\x83\xC1\x08\xEB\x0A", 15, true);
				injector::WriteMemoryRaw(imageBase + 0x57AE7, "\xE9\x1B\x04\x00\x00", 5, true);
				injector::WriteMemoryRaw(imageBase + 0x57F01, "\xE9\xC8\xFB\xFF\xFF\x90", 6, true);
			}

			if (!ToBool(config["General"]["Windowed"]))
			{
				injector::MakeRET(0x5F21B0, 4);
			}

			if (ToBool(config["General"]["Professional Edition Enable"]))
			{
				injector::MakeNOP(imageBase + 0x1E7AB, 6);
				injector::MakeNOP(imageBase + 0x1E7DC, 6);
				injector::MakeNOP(imageBase + 0x1E7A5, 6);
				injector::MakeNOP(imageBase + 0x1E82B, 6);
				injector::MakeNOP(imageBase + 0x1E79F, 6);
				injector::MakeNOP(imageBase + 0x1E858, 6);
				injector::MakeNOP(imageBase + 0x1E799, 6);
				injector::MakeNOP(imageBase + 0x1E880, 6);
				injector::MakeNOP(imageBase + 0x27447, 3);

				if (ToBool(config["General"]["Professional Edition Hold Gear"]))
				{
					injector::MakeNOP(imageBase + 0x8ADFF, 10);
				}

				CreateThread(NULL, 0, BG4ProRunningLoop, NULL, 0, NULL);
			}
			else
			{
				CreateThread(NULL, 0, BG4RunningLoop, NULL, 0, NULL);
			}

			iatHook("kernel32.dll", ReadFileWrapTx2, "ReadFile");
			iatHook("kernel32.dll", WriteFileWrapTx2, "WriteFile");
			iatHook("kernel32.dll", GetFileAttributesAWrapTx2, "GetFileAttributesA");

			break;
		}
		case X2Type::BG4_Eng:
		{
			// TODO: DOCUMENT PATCHES
			//injector::MakeNOP(0x4CBCB8, 10);
			//injector::WriteMemory<uint8_t>(0x4CBCB8, 0xB8, true);
			//injector::WriteMemory<uint32_t>(0x4CBCB9, 1, true);

			// redirect E:\data to .\data
			injector::WriteMemoryRaw(0x0073139C, "./data/", 8, true);
			injector::WriteMemoryRaw(0x00758978, ".\\data", 7, true);

			if (ToBool(config["General"]["IntroFix"]))
			{
				// thanks for Ducon2016 for the patch!
				injector::WriteMemoryRaw(imageBase + 0x46E3E, "\x89\x68\x14\xC7\x40\x08\x00\x00\x80\x3F\x83\xC1\x08\xEB\x0A", 15, true);
				injector::WriteMemoryRaw(imageBase + 0x46E57, "\xE9\x5F\x03\x00\x00", 5, true);
				injector::WriteMemoryRaw(imageBase + 0x471B5, "\xE9\x89\xFC\xFF\xFF\x90", 6, true);
				injector::WriteMemoryRaw(imageBase + 0xEB964, "\x89\x4C", 2, true);
				injector::WriteMemoryRaw(imageBase + 0xEB967, "\x40\x8B\x70\x18\xD9\x5C\x24\x4C\x89\x4C", 10, true);
				injector::WriteMemoryRaw(imageBase + 0xEB972, "\x5C", 1, true);
				injector::WriteMemoryRaw(imageBase + 0xEB982, "\x8B\x70\x18\x89\x4C\x24\x78\x89\x74\x24\x54\x8B\xB4\x24\x9C\x00", 16, true);
				injector::WriteMemoryRaw(imageBase + 0xEB994, "\x89\x74\x24\x68\x8B\xB4\x24\xA0\x00\x00\x00\x89\x74\x24\x6C\x31\xF6\x89\x74\x24\x60\x89\x74\x24\x64\x89\xB4\x24\x80\x00\x00\x00\x8B\x70\x18\x89\x74\x24\x70\xBE\x00\x00\x80\x3F\x89\x74\x24\x2C\x89\x74\x24\x44\x89\x74\x24\x48\x89\x74\x24\x7C\x89\x74\x24\x20\x89\x74\x24\x3C\x89\x74\x24\x58\x89\x74\x24\x74\x90\x90\x90\x90\x90", 81, true);
			}

			if (!ToBool(config["General"]["Windowed"]))
			{
				injector::MakeRET(0x5B8030, 4);
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

		if (ToBool(config["General"]["Disable Cel Shaded"]))
		{
			injector::MakeNOP(imageBase + 0x31FFA, 6);
			injector::MakeRET(0x42DD30);
		}
	}
	
	if(GameDetect::currentGame == GameID::TetrisGM3)
	{
		// TODO: DOCUMENT PATCHES
		injector::WriteMemory<DWORD>(0x0046A0AC, 0x00005C2E, true); //required to boot
		// windowed mode patch (if set to windowed in config) Makes it a proper windowed app. Not the first person to have done this patch
		if (ToBool(config["General"]["Windowed"]))
		{
			injector::WriteMemory<BYTE>(0x44DCC9, 0x00, true);
		}
		// user configurable resolution patch.
		// credit to Altimoor
		// get resolution from config file 
		auto resx = ToInt(config["General"]["ResolutionWidth"]);        // original arcade ran at 640
		auto resy = ToInt(config["General"]["ResolutionHeight"]);       // original ran at 480.
		// can't have game window TOO small.
		if (resx < 640)
			resx = 640;
		if (resy < 480)
			resy = 480;
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

		// and finally, to protect integrity of dump, change bookeeping directory to TeknoParrot, because the global redirect misses this one.
		injector::WriteMemoryRaw(0x46A0B0, "TeknoParrot\\\0\0", 12, true);
		// create it if it doesn't exist

		wchar_t pathRoot[MAX_PATH];
		GetModuleFileNameW(GetModuleHandle(nullptr), pathRoot, _countof(pathRoot)); // get full pathname to game executable

		wcsrchr(pathRoot, L'\\')[0] = L'\0'; // chop off everything from the last backslash.

		CreateDirectoryW((pathRoot + L"\\TeknoParrot\\"s).c_str(), nullptr); // create TeknoParrot subdirectory off of launcher's root directory to cleanly store data seperate from rest of files.

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
