#include <StdInc.h>
#include "Utility/InitFunction.h"
#include <thread>

static uint8_t g_fastIOValues[64];

DWORD __cdecl iDmacDrvOpen(int deviceId, LPVOID outBuffer, LPVOID lpSomeFlag)
{
	*(DWORD*)outBuffer = 284;
	*(DWORD*)lpSomeFlag = 0;
#ifdef LogFastIO
	info(true, "iDmacDrvOpen(deviceId=%d, outBuffer='%08X', lpSomeFlag=%08X) -> result=%08X", deviceId, *(DWORD*)outBuffer, *(DWORD*)lpSomeFlag, 0);
	info(true, "--------------------------------------------");
#endif
	return 0;
}

DWORD __cdecl iDmacDrvClose(int deviceId, LPVOID lpWriteAccess)
{
#ifdef LogFastIO
	info(true, "iDmacDrvClose(deviceId=%d, lpWriteAccess='%08X') -> result=%08X", deviceId, lpWriteAccess, 0);
	info(true, "--------------------------------------------");
#endif
	return 0;
}

int __cdecl iDmacDrvDmaRead(int a1, LPVOID lp, UINT_PTR ucb, LPVOID a4)
{
	return 0;
}

int __cdecl iDmacDrvDmaWrite(int a1, void *lp, UINT_PTR ucb, LPVOID a4)
{
	return 0;
}

static bool coinPressed = false;

int __cdecl iDmacDrvRegisterRead(int DeviceId, DWORD CommandCode, LPVOID OutBuffer, LPVOID DeviceResult)
{
	DWORD result = 0;

	switch (CommandCode)
	{
	case 0x400:
	{
		result = 0x00010201;
		break;
	}
	case 0x4000:
		result = 0x00FF00FF;
		break;
	case 0x4004:
		result = 0x00FF0000;
		break;
	case 0x4120:
		result = g_fastIOValues[0] + ((DWORD)g_fastIOValues[1] * 0x100) + ((DWORD)g_fastIOValues[2] * 0x10000) + ((DWORD)g_fastIOValues[3] * 0x1000000);
		break;
	case 0x412C:
		result = 0x00000000;
		break;
	case 0x41A4:
	case 0x4124:
		result = 0x01100000;
		break;
	case 0x4140:
		if(g_fastIOValues[4] == 1)
		{
			if(!coinPressed)
			{
				result = g_fastIOValues[4];
				coinPressed = true;
			}
			else
			{
				result = 0;
			}
		}
		else
		{
			result = 0;
			coinPressed = false;
		}
		break;
	case 0x41C0: // coin 2 ?
		result = 0;
		// For coin not getting stuck. One ping is enough.
		//result = CoinPackage;
		//if (result != 0)
		//	CoinPackage = 0;
		break;
	case 0x4144:
	case 0x41C4:
		result = g_fastIOValues[5];
		// Dual Fast I/O use, not emu'd atm.
		//result = CoinPackageSecond;
		//if (result != 0)
		//	CoinPackageSecond = 0;
		break;
	case 0x41A0:
		result = 0;
		break;
	case 0x41A8:
		result = 0;
		break;
	case 0x4128:
		result = g_fastIOValues[8];
		result += g_fastIOValues[9] * 0x100;
		break;
	case 0x41AC:
		result = 0;
		break;
	case 0x4180:
		result = 0;
		break;
	case 0x4108:
		result = 0;
		break;
	case 0x4150u:
		result = 0x1823C;
		break;
	default:
#ifdef _DEBUG
		info(true, "Unknown Fast I/O Request: %08X", CommandCode);
#endif
		break;
	}
	*(DWORD*)OutBuffer = result;
	*(DWORD*)DeviceResult = 0;
#ifdef LogFastIO
	info(true, "iDmacDrvRegisterRead(DeviceId=%d, CommandCode=%08X, OutBuffer=%08X, DeviceResult=%08X) -> result=%08X", DeviceId, CommandCode, *(DWORD*)OutBuffer, *(DWORD*)DeviceResult, 0);
	info(true, "--------------------------------------------");
#endif
	return 0;
}

int __cdecl iDmacDrvRegisterWrite(int deviceId, DWORD CommandCode, int unused, DWORD *DeviceResult)
{
#ifdef LogFastIO
	info(true, "iDmacDrvRegisterWrite(deviceId=%d, CommandCode='%08X', unused=%08X, DeviceResult=%08X) -> result=%08X", deviceId, CommandCode, unused, *(DWORD*)DeviceResult, 0);
	info(true, "--------------------------------------------");
#endif

	if (CommandCode <= 0x4100)
	{
		if (CommandCode != 0x4100 && CommandCode != 0x4000 && CommandCode != 0x4004)
			return 0;
		*DeviceResult = -1;
		return 0;
	}

	if (CommandCode > 0x410C)
	{
		switch (CommandCode)
		{
			case 0x4180:
			case 0x4184:
			case 0x4188:
			case 0x418C:
			case 0x410C:
			case 0x4104:
			{
				*DeviceResult = -1;
				return 0;
			}
			case 0x4108:
			default:
				return 0;
		}
	}
	*DeviceResult = -1;
	return 0;
}

int __cdecl iDmacDrvRegisterBufferRead(int a1, DWORD BytesReturned, LPVOID lp, UINT_PTR ucb, LPVOID a5)
{
	return 0;
}

int __cdecl iDmacDrvRegisterBufferWrite(int a1, DWORD BytesReturned, void *lp, UINT_PTR ucb, LPVOID a5)
{
	return 0;
}

int __cdecl iDmacDrvMemoryRead(int a1, DWORD BytesReturned, LPVOID lp, LPVOID a4)
{
	return 0;
}

int __cdecl iDmacDrvMemoryWrite(int a1, DWORD BytesReturned, int a3, LPVOID lp)
{
	return 0;
}

int __cdecl iDmacDrvMemoryReadExt(int a1, DWORD BytesReturned, int a3, LPVOID lp, DWORD nOutBufferSize, LPVOID a6)
{
	return 0;
}

int __cdecl iDmacDrvMemoryWriteExt(int a1, int a2, int a3, void *Src, rsize_t DstSize, LPVOID lp)
{
	return 0;
}

int __cdecl iDmacDrvMemoryBufferRead(int a1, DWORD BytesReturned, LPVOID lp, UINT_PTR ucb, LPVOID a5)
{
	return 0;
}

int __cdecl iDmacDrvMemoryBufferWrite(int a1, int a2, void *lp, UINT_PTR ucb, LPVOID a5)
{
	return 0;
}

void init_FastIoEmu()
{
	MH_Initialize();
#if _M_IX86
	const char* libName = "idmacdrv32.dll";
	LoadLibraryA(libName);
	if((DWORD)GetProcAddress(GetModuleHandleA(libName), "iDmacDrvMemoryBufferWrite") != 0)
		injector::MakeJMP(GetProcAddress(GetModuleHandleA(libName), "iDmacDrvMemoryBufferWrite"), iDmacDrvMemoryBufferWrite);
	if ((DWORD)GetProcAddress(GetModuleHandleA(libName), "iDmacDrvMemoryBufferRead") != 0)
		injector::MakeJMP(GetProcAddress(GetModuleHandleA(libName), "iDmacDrvMemoryBufferRead"), iDmacDrvMemoryBufferRead);
	if ((DWORD)GetProcAddress(GetModuleHandleA(libName), "iDmacDrvMemoryWriteExt") != 0)
		injector::MakeJMP(GetProcAddress(GetModuleHandleA(libName), "iDmacDrvMemoryWriteExt"), iDmacDrvMemoryWriteExt);
	if ((DWORD)GetProcAddress(GetModuleHandleA(libName), "iDmacDrvMemoryReadExt") != 0)
		injector::MakeJMP(GetProcAddress(GetModuleHandleA(libName), "iDmacDrvMemoryReadExt"), iDmacDrvMemoryReadExt);
	if ((DWORD)GetProcAddress(GetModuleHandleA(libName), "iDmacDrvMemoryWrite") != 0)
		injector::MakeJMP(GetProcAddress(GetModuleHandleA(libName), "iDmacDrvMemoryWrite"), iDmacDrvMemoryWrite);
	if ((DWORD)GetProcAddress(GetModuleHandleA(libName), "iDmacDrvMemoryRead") != 0)
		injector::MakeJMP(GetProcAddress(GetModuleHandleA(libName), "iDmacDrvMemoryRead"), iDmacDrvMemoryRead);
	if ((DWORD)GetProcAddress(GetModuleHandleA(libName), "iDmacDrvRegisterBufferWrite") != 0)
		injector::MakeJMP(GetProcAddress(GetModuleHandleA(libName), "iDmacDrvRegisterBufferWrite"), iDmacDrvRegisterBufferWrite);
	if ((DWORD)GetProcAddress(GetModuleHandleA(libName), "iDmacDrvRegisterBufferRead") != 0)
		injector::MakeJMP(GetProcAddress(GetModuleHandleA(libName), "iDmacDrvRegisterBufferRead"), iDmacDrvRegisterBufferRead);
	if ((DWORD)GetProcAddress(GetModuleHandleA(libName), "iDmacDrvDmaWrite") != 0)
		injector::MakeJMP(GetProcAddress(GetModuleHandleA(libName), "iDmacDrvDmaWrite"), iDmacDrvDmaWrite);
	if ((DWORD)GetProcAddress(GetModuleHandleA(libName), "iDmacDrvDmaRead") != 0)
		injector::MakeJMP(GetProcAddress(GetModuleHandleA(libName), "iDmacDrvDmaRead"), iDmacDrvDmaRead);
	if ((DWORD)GetProcAddress(GetModuleHandleA(libName), "iDmacDrvClose") != 0)
		injector::MakeJMP(GetProcAddress(GetModuleHandleA(libName), "iDmacDrvClose"), iDmacDrvClose);
	if ((DWORD)GetProcAddress(GetModuleHandleA(libName), "iDmacDrvOpen") != 0)
		injector::MakeJMP(GetProcAddress(GetModuleHandleA(libName), "iDmacDrvOpen"), iDmacDrvOpen);
	if ((DWORD)GetProcAddress(GetModuleHandleA(libName), "iDmacDrvRegisterWrite") != 0)
		injector::MakeJMP(GetProcAddress(GetModuleHandleA(libName), "iDmacDrvRegisterWrite"), iDmacDrvRegisterWrite);
	if ((DWORD)GetProcAddress(GetModuleHandleA(libName), "iDmacDrvRegisterRead") != 0)
		injector::MakeJMP(GetProcAddress(GetModuleHandleA(libName), "iDmacDrvRegisterRead"), iDmacDrvRegisterRead);
#else
	const wchar_t* libName = L"idmacdrv64.dll";
	MH_CreateHookApi(libName, "iDmacDrvClose", &iDmacDrvClose, NULL);
	MH_CreateHookApi(libName, "iDmacDrvOpen", &iDmacDrvOpen, NULL);
	MH_CreateHookApi(libName, "iDmacDrvRegisterWrite", &iDmacDrvRegisterWrite, NULL);
	MH_CreateHookApi(libName, "iDmacDrvRegisterRead", &iDmacDrvRegisterRead, NULL);
#endif
	MH_EnableHook(MH_ALL_HOOKS);

	memset(g_fastIOValues, 0, 64);

	CreateThread(nullptr, 0, [] (LPVOID) -> DWORD
	{
		HANDLE hPipe = CreateFileA("\\\\.\\pipe\\TeknoParrotPipe", GENERIC_READ, 0, nullptr, OPEN_EXISTING, 0, NULL);

		if (hPipe != INVALID_HANDLE_VALUE)
		{
			DWORD read;

			while (true)
			{
				ReadFile(hPipe, g_fastIOValues, sizeof(g_fastIOValues), &read, nullptr);
			}
		}

		return 0;
	}, nullptr, 0, nullptr);
}