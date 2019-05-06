#pragma optimize("", off)
#include <Windows.h>
int main();

BOOL WINAPI DllMain(
    HINSTANCE hinstDLL,  // handle to DLL module
    DWORD fdwReason,     // reason for calling function
    LPVOID lpReserved )  // reserved
{
    // Perform actions based on the reason for calling.
    switch( fdwReason ) 
    { 
        case DLL_PROCESS_ATTACH:
         // Initialize once for each new process.
         // Return FALSE to fail DLL load.
			return main();
            break;

        case DLL_THREAD_ATTACH:
         // Do thread-specific initialization.
            break;

        case DLL_THREAD_DETACH:
         // Do thread-specific cleanup.
            break;

        case DLL_PROCESS_DETACH:
         // Perform any necessary cleanup.
            break;
    }
    return TRUE;  // Successful DLL_PROCESS_ATTACH.
}

bool initOK = false;

extern "C" __declspec(dllexport) DWORD __cdecl iDmacDrvOpen(int deviceId, LPVOID outBuffer, LPVOID lpSomeFlag)
{
	return 0;
}

extern "C" __declspec(dllexport) DWORD __cdecl iDmacDrvClose(int deviceId, LPVOID lpWriteAccess)
{
	return 0;
}

extern "C" __declspec(dllexport) int __cdecl iDmacDrvDmaRead(int a1, LPVOID lp, UINT_PTR ucb, LPVOID a4)
{
	return 0;
}

extern "C" __declspec(dllexport) int __cdecl iDmacDrvDmaWrite(int a1, void *lp, UINT_PTR ucb, LPVOID a4)
{
	return 0;
}

extern "C" __declspec(dllexport) int __cdecl iDmacDrvRegisterRead(int DeviceId, DWORD CommandCode, LPVOID OutBuffer, LPVOID DeviceResult)
{
	return 0;
}

extern "C" __declspec(dllexport) int __cdecl iDmacDrvRegisterWrite(int deviceId, DWORD CommandCode, int unused, LPVOID DeviceResult)
{
	return 0;
}

extern "C" __declspec(dllexport) int __cdecl iDmacDrvRegisterBufferRead(int a1, DWORD BytesReturned, LPVOID lp, UINT_PTR ucb, LPVOID a5)
{
	return 0;
}

extern "C" __declspec(dllexport) int __cdecl iDmacDrvRegisterBufferWrite(int a1, DWORD BytesReturned, void *lp, UINT_PTR ucb, LPVOID a5)
{
	return 0;
}

extern "C" __declspec(dllexport) int __cdecl iDmacDrvMemoryRead(int a1, DWORD BytesReturned, LPVOID lp, LPVOID a4)
{
	return 0;
}

extern "C" __declspec(dllexport) int __cdecl iDmacDrvMemoryWrite(int a1, DWORD BytesReturned, int a3, LPVOID lp)
{
	return 0;
}

extern "C" __declspec(dllexport) int __cdecl iDmacDrvMemoryBufferRead(int a1, DWORD BytesReturned, LPVOID lp, UINT_PTR ucb, LPVOID a5)
{
	return 0;
}

extern "C" __declspec(dllexport) int __cdecl iDmacDrvMemoryBufferWrite(int a1, int a2, void *lp, UINT_PTR ucb, LPVOID a5)
{
	return 0;
}

#ifdef _M_IX86

extern "C" __declspec(dllexport) int __cdecl iDmacDrvMemoryReadExt(int a1, DWORD BytesReturned, int a3, LPVOID lp, DWORD nOutBufferSize, LPVOID a6)
{
	return 0;
}

extern "C" __declspec(dllexport) int __cdecl iDmacDrvMemoryWriteExt(int a1, int a2, int a3, void *Src, rsize_t DstSize, LPVOID lp)
{
	return 0;
}

#else

extern "C" __declspec(dllexport) int __cdecl iDmacDrvProgramDownload()
{
	return 0;
}

#endif

int main()
{
	return 1;
}
#pragma optimize("", on)