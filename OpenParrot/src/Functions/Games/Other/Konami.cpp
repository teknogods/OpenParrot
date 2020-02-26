#include <StdInc.h>
#include "Utility/InitFunction.h"
#include "Functions/Global.h"
#include "Utility/GameDetect.h"
#include <math.h>

// controls
extern int* ffbOffset;
static uint8_t STATUS_BUFFER[64]{};

static void* __cdecl ac_io_hbhi_get_control_status_buffer(uint8_t* buffer)
{
    //Beep(100, 100);

	memcpy(buffer, STATUS_BUFFER, 64);
	return buffer;
}

static char __cdecl ac_io_hbhi_update_control_status_buffer() 
{
    switch (GameDetect::KonamiGame)
    {
    case KonamiGame::HelloPopnMusic:
        memset(STATUS_BUFFER, 0x00, 64);

        // TODO: make the buttons actually work, and add the rest of them
        
        // service
        if (*ffbOffset & 0x01)
        {
            Beep(100, 100);
            STATUS_BUFFER[5] |= 1 << 4;
        }

        // test
        if (*ffbOffset & 0x02)
        {
            Beep(100, 100);
            STATUS_BUFFER[5] |= 1 << 5;
        }

        // player 1 start  
        //if (*ffbOffset & 0x04) 
        {
            //Beep(100, 100);
            STATUS_BUFFER[4] |= 1;
        }

        break;
    }

    return true;
}

DWORD WINAPI KonamiInput(LPVOID lpParam)
{
	// wait for SpiceTools to hook IO stuff.
	Sleep(1000);

    // and replace it's IO hooks with ours >:)
    LoadLibraryA("libacio.dll");

    MH_Initialize();
    MH_CreateHookApi(L"libacio.dll", "ac_io_hbhi_get_control_status_buffer", ac_io_hbhi_get_control_status_buffer, NULL);
    MH_CreateHookApi(L"libacio.dll", "ac_io_hbhi_update_control_status_buffer", ac_io_hbhi_update_control_status_buffer, NULL);
    MH_EnableHook(MH_ALL_HOOKS);
    Beep(500, 500);
    //while (true)
    //{
    //    Sleep(100);
    //}

    return 0;
}

static InitFunction KonamiFunc([]()
{
	CreateThread(NULL, 0, KonamiInput, NULL, 0, NULL);
}, GameID::Konami);
