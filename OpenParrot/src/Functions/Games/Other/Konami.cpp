#include <StdInc.h>
#include "Utility/InitFunction.h"
#include "Functions/Global.h"
#include "Utility/GameDetect.h"
#include <math.h>

// controls
extern int* ffbOffset;
static uint8_t STATUS_BUFFER[64]{};

enum Buttons
{
    SERVICE = (1 << 0),
    TEST = (1 << 1),
    BUTTON1 = (1 << 2),
    BUTTON2 = (1 << 3),
    BUTTON3 = (1 << 4),
    BUTTON4 = (1 << 5),
    BUTTON5 = (1 << 6),
    BUTTON6 = (1 << 7),
    BUTTON7 = (1 << 8),
    BUTTON8 = (1 << 9),
    BUTTON9 = (1 << 10),
    BUTTON10 = (1 << 11),
};

static void* __cdecl ac_io_hbhi_get_control_status_buffer(uint8_t* buffer)
{
    memcpy(buffer, STATUS_BUFFER, 64);
    return buffer;
}

static char __cdecl ac_io_hbhi_update_control_status_buffer() 
{
    memset(STATUS_BUFFER, 0, 64);

    int buttons = *ffbOffset;

    switch (GameDetect::KonamiGame)
    {
    case KonamiGame::HelloPopnMusic:

        if (buttons & SERVICE)
        {
            STATUS_BUFFER[5] |= 1 << 4;
        }

        if (buttons & TEST)
        {
            STATUS_BUFFER[5] |= 1 << 5;
        }

        // player 1 start
        if (!(buttons & BUTTON1))
        {
            STATUS_BUFFER[4] |= 1;
        }

        // player 1 red
        if (buttons & BUTTON2)
        {
            STATUS_BUFFER[12] |= 1 << 0;
        }

        // player 1 blue
        if (buttons & BUTTON3)
        {
            STATUS_BUFFER[12] |= 1 << 1;
        }

        // player 1 yellow
        if (buttons & BUTTON4)
        {
            STATUS_BUFFER[12] |= 1 << 2;
        }

        // player 1 green
        if (buttons & BUTTON5)
        {
            STATUS_BUFFER[12] |= 1 << 3;
        }

        // player 2 start
        if (!(buttons & BUTTON6))
        {
            STATUS_BUFFER[6] |= 1;
        }

        // player 1 red
        if (buttons & BUTTON7)
        {
            STATUS_BUFFER[12] |= 1 << 4;
        }

        // player 1 blue
        if (buttons & BUTTON8)
        {
            STATUS_BUFFER[12] |= 1 << 5;
        }

        // player 1 yellow
        if (buttons & BUTTON9)
        {
            STATUS_BUFFER[12] |= 1 << 6;
        }

        // player 1 green
        if (buttons & BUTTON10)
        {
            STATUS_BUFFER[12] |= 1 << 7;
        }

        break;
    case KonamiGame::RoadFighters3D:

        if (buttons & SERVICE)
        {
            STATUS_BUFFER[5] |= 1 << 4;
        }

        if (buttons & TEST)
        {
            STATUS_BUFFER[5] |= 1 << 5;
        }

        // view
        if (buttons & BUTTON1)
        {
            STATUS_BUFFER[12] |= 1 << 2;
        }

        // dimension toggle
        if (buttons & BUTTON2)
        {
            STATUS_BUFFER[12] |= 1 << 3;
        }

        // lever up
        if (buttons & BUTTON5)
        {
            STATUS_BUFFER[12] |= 1 << 4;
        }

        // lever down
        if (buttons & BUTTON4)
        {
            STATUS_BUFFER[12] |= 1 << 5;
        }

        // lever left
        if (buttons & BUTTON3)
        {
            STATUS_BUFFER[12] |= 1 << 6;
        }

        // lever right
        if (buttons & BUTTON6)
        {
            STATUS_BUFFER[12] |= 1 << 7;
        }
       
        break;
    }

    return 1;
}

DWORD WINAPI KonamiInput(LPVOID lpParam)
{
    // stop early and spam log a bit if it's a unsupported game 
    if (GameDetect::KonamiGame == KonamiGame::None)
    {
        for (int i = 0; i < 10; i++) 
        {
            OutputDebugStringA("openparrot: unsupported game!");
        }
        Sleep(100);
        MessageBoxA(0, "Unsupported Konami Game!", "Error", MB_ICONERROR);
        Sleep(100);
        ExitProcess(0);
        return 0;
    }

    // wait for SpiceTools to hook IO stuff.
    Sleep(500);
    OutputDebugStringA("openparrot: attaching");

    // and replace it's IO hooks with ours >:)
    if (!LoadLibraryA("libacio.dll"))
    {
        OutputDebugStringA("openparrot: failed to load libacio.dll?????");
        return 0;
    }

    MH_Initialize();
    MH_CreateHookApi(L"libacio.dll", "ac_io_hbhi_get_control_status_buffer", ac_io_hbhi_get_control_status_buffer, NULL);
    MH_CreateHookApi(L"libacio.dll", "ac_io_hbhi_update_control_status_buffer", ac_io_hbhi_update_control_status_buffer, NULL);
    MH_EnableHook(MH_ALL_HOOKS);

    // TODO: Boomslangnz's patches
    if (ToBool(config["General"]["Always2D"]))
    {
        OutputDebugStringA("openparrot: applying Always2D patch");
    }

    OutputDebugStringA("openparrot: attached");

    return 0;
}

static InitFunction KonamiFunc([]()
{
    CreateThread(NULL, 0, KonamiInput, NULL, 0, NULL); 
}, GameID::Konami);