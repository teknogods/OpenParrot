#include <StdInc.h>
#include "Utility/InitFunction.h"
#include "Functions/Global.h"
#include "Utility/GameDetect.h"
#include <math.h>

// controls
extern int* ffbOffset;
static uint8_t STATUS_BUFFER[64]{};

constexpr auto SERVICE  = (1 << 0);
constexpr auto TEST     = (1 << 1);
constexpr auto BUTTON1  = (1 << 2);
constexpr auto BUTTON2  = (1 << 3);
constexpr auto BUTTON3  = (1 << 4);
constexpr auto BUTTON4  = (1 << 5);
constexpr auto BUTTON5  = (1 << 6);
constexpr auto BUTTON6  = (1 << 7);
constexpr auto BUTTON7  = (1 << 8);
constexpr auto BUTTON8  = (1 << 9);
constexpr auto BUTTON9  = (1 << 10);
constexpr auto BUTTON10 = (1 << 11);

static void* __cdecl ac_io_hbhi_get_control_status_buffer(uint8_t* buffer)
{
	memcpy(buffer, STATUS_BUFFER, 64);
	return buffer;
}

static char __cdecl ac_io_hbhi_update_control_status_buffer() 
{
    switch (GameDetect::KonamiGame)
    {
    case KonamiGame::HelloPopnMusic:
        memset(STATUS_BUFFER, 0, 64);

        int buttons = *ffbOffset;
        // TODO: add the rest of the buttons
        
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

        // TODO: player 2
        STATUS_BUFFER[6] |= 1;

        break;
    }

    return 1;
}

DWORD WINAPI KonamiInput(LPVOID lpParam)
{
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

    OutputDebugStringA("openparrot: attached");

    return 0;
}

static InitFunction KonamiFunc([]()
{
	CreateThread(NULL, 0, KonamiInput, NULL, 0, NULL); 
}, GameID::Konami);
