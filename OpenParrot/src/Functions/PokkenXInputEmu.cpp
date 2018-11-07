#pragma optimize("", off)
#include "StdInc.h"
#include "Utility/GameDetect.h"
#include "Utility/InitFunction.h"
#include "PokkenXInputEmu.h"

struct XboxOneControllerHandler
{
	struct usb_dev_handle *handle;
	bool isConnected;
	XBOXONE_STATE controllerState;

	uint8_t lastState[64];
	unsigned int tickCount;

	XINPUT_GAMEPAD lastGamepadState;
};

XboxOneControllerHandler *controllerHandler[4] = { NULL, NULL, NULL, NULL };
HANDLE XboxOneControllerThread[4] = { 0 };
HANDLE XboxOneControllerMutex[4] = { 0 };

static unsigned short idVendor = 0x045E;
static unsigned short idProduct = 0x02D1;

int configuration = 1;
int interface = 0;
int endpointIn = 0x81;
int endpointOut = 0x01;
int timeout = 2000; // milliseconds 
bool controllerInit = false;
bool runThread = true;

// Structure we receive from the controller
struct XboxOneControllerState
{
	char eventCount;
	char unknown;
	char buttons1;
	char buttons2;
	short leftTrigger;  // Triggers are 0 - 1023
	short rightTrigger;
	short thumbLX;      // Axes are -32767 - 32767
	short thumbLY;
	short thumbRX;
	short thumbRY;
};

bool connectController(bool enable)
{
	controllerInit = enable;
	return true;
}

int iround(double num) {
	return (num > 0.0) ? (int)floor(num + 0.5) : (int)ceil(num - 0.5);
}

extern int* ffbOffset;

DWORD WINAPI XInputGetState
(
	__in  DWORD         dwUserIndex,						// Index of the gamer associated with the device
	__out XINPUT_STATE* pState								// Receives the current state
)
{
	if (!controllerInit)
	{
		connectController(true);
	}
	if (controllerInit && dwUserIndex == 0)
	{
		XINPUT_GAMEPAD gamepadState = { 0 };

		if (GameDetect::currentGame == GameID::Daytona3)
			gamepadState.wButtons |= *ffbOffset;
		else
			gamepadState.wButtons |= 0;
#ifdef _M_IX86
		if (GameDetect::currentGame == GameID::Daytona3)
		{
			gamepadState.bRightTrigger = daytonaPressStart ? 0xFF : 0x00;
		}
#endif
			if (pState->dwPacketNumber == UINT_MAX)
				pState->dwPacketNumber = 0;
			else
				pState->dwPacketNumber++;

		pState->Gamepad = gamepadState;
		return ERROR_SUCCESS;
	}
	else
	{
		return ERROR_DEVICE_NOT_CONNECTED;
	}
}

DWORD WINAPI XInputSetState
(
	__in DWORD             dwUserIndex,						// Index of the gamer associated with the device
	__in XINPUT_VIBRATION* pVibration						// The vibration information to send to the controller
)
{
	if (!controllerInit)
	{
		connectController(true);
	}

	if (controllerInit && dwUserIndex == 0)
	{
		// We're receiving as XInput [0 ~ 65535], need to be [0 ~ 255] !!
		int leftVal = iround(((float)pVibration->wLeftMotorSpeed / 65535) * 255);
		int rightVal = iround(((float)pVibration->wRightMotorSpeed / 65535) * 255);
		return ERROR_SUCCESS;
	}
	else
	{
		return ERROR_DEVICE_NOT_CONNECTED;
	}
}

DWORD WINAPI XInputGetCapabilities
(
	__in  DWORD                dwUserIndex,					// Index of the gamer associated with the device
	__in  DWORD                dwFlags,						// Input flags that identify the device type
	__out XINPUT_CAPABILITIES* pCapabilities				// Receives the capabilities
)
{
	if (!controllerInit)
	{
		connectController(true);
	}

	if (dwFlags > XINPUT_FLAG_GAMEPAD)
	{
		return ERROR_BAD_ARGUMENTS;
	}

	if (controllerInit && dwUserIndex == 0)
	{
		pCapabilities->Flags = XINPUT_CAPS_VOICE_SUPPORTED;
		pCapabilities->Type = XINPUT_DEVTYPE_GAMEPAD;
		pCapabilities->SubType = XINPUT_DEVSUBTYPE_GAMEPAD;

		pCapabilities->Gamepad.wButtons = 0xF3FF;

		pCapabilities->Gamepad.bLeftTrigger = 0xFF;
		pCapabilities->Gamepad.bRightTrigger = 0xFF;

		pCapabilities->Gamepad.sThumbLX = (SHORT)0xFFC0;
		pCapabilities->Gamepad.sThumbLY = (SHORT)0xFFC0;
		pCapabilities->Gamepad.sThumbRX = (SHORT)0xFFC0;
		pCapabilities->Gamepad.sThumbRY = (SHORT)0xFFC0;

		pCapabilities->Vibration.wLeftMotorSpeed = 0xFF;
		pCapabilities->Vibration.wRightMotorSpeed = 0xFF;

		return ERROR_SUCCESS;
	}
	else
	{
		return ERROR_DEVICE_NOT_CONNECTED;
	}
}

void WINAPI XInputEnable
(
	__in bool enable										// [in] Indicates whether xinput is enabled or disabled. 
)
{
	if (!controllerInit)
	{
		connectController(true);
	}

	if (controllerInit && !enable)
	{
		XINPUT_VIBRATION Vibration = { 0, 0 };
		int xboxControllerCounter = 0;

		while (xboxControllerCounter < 4)
		{
			if (controllerHandler[xboxControllerCounter])
			{
				XInputSetState(xboxControllerCounter, &Vibration);
			}
			xboxControllerCounter++;
		}
	}
}

DWORD WINAPI XInputGetDSoundAudioDeviceGuids
(
	__in  DWORD dwUserIndex,								// Index of the gamer associated with the device
	__out GUID* pDSoundRenderGuid,							// DSound device ID for render
	__out GUID* pDSoundCaptureGuid							// DSound device ID for capture
)
{
	if (!controllerInit)
	{
		connectController(true);
	}

	if (controllerInit && dwUserIndex == 0)
	{
		pDSoundRenderGuid = NULL;
		pDSoundCaptureGuid = NULL;

		return ERROR_SUCCESS;
	}
	else
	{
		return ERROR_DEVICE_NOT_CONNECTED;
	}
}

DWORD XInputGetBatteryInformation
(
	__in  DWORD                       dwUserIndex,			// Index of the gamer associated with the device
	__in  BYTE                        devType,				// Which device on this user index
	__out XINPUT_BATTERY_INFORMATION* pBatteryInformation	// Contains the level and types of batteries
)
{
	if (!controllerInit)
	{
		connectController(true);
	}

	if (controllerInit && dwUserIndex == 0)
	{
		pBatteryInformation->BatteryType = BATTERY_TYPE_WIRED;
		pBatteryInformation->BatteryLevel = BATTERY_LEVEL_FULL;
		return ERROR_SUCCESS;
	}
	else
	{
		return ERROR_DEVICE_NOT_CONNECTED;
	}
}

DWORD WINAPI XInputGetKeystroke
(
	__in       DWORD dwUserIndex,							// Index of the gamer associated with the device
	__reserved DWORD dwReserved,							// Reserved for future use
	__out      PXINPUT_KEYSTROKE pKeystroke					// Pointer to an XINPUT_KEYSTROKE structure that receives an input event.
)
{
	if (!controllerInit)
	{
		connectController(true);
	}

	if (controllerInit && dwUserIndex == 0)
	{
		return ERROR_EMPTY; // or ERROR_SUCCESS
	}
	else
	{
		return ERROR_DEVICE_NOT_CONNECTED;
	}
}

DWORD WINAPI XInputGetStateEx
(
	__in  DWORD         dwUserIndex,						// Index of the gamer associated with the device
	__out XINPUT_STATE* pState								// Receives the current state
)
{
	if (!controllerInit)
	{
		connectController(true);
	}
	if (controllerInit && dwUserIndex == 0)
	{
		XINPUT_GAMEPAD gamepadState = { 0 };

		if (GameDetect::currentGame == GameID::Daytona3)
			gamepadState.wButtons = *ffbOffset;
		else
			gamepadState.wButtons = 0;

#ifdef _M_IX86
		if (GameDetect::currentGame == GameID::Daytona3)
		{
			gamepadState.bRightTrigger = daytonaPressStart ? 0xFF : 0x00;
		}
#endif
		
		if (pState->dwPacketNumber == UINT_MAX)
			pState->dwPacketNumber = 0;
		else
			pState->dwPacketNumber++;
		pState->Gamepad = gamepadState;
		return ERROR_SUCCESS;
	}
	else
	{
		return ERROR_DEVICE_NOT_CONNECTED;
	}
}

DWORD WINAPI XInputSetStateEx
(
__in DWORD             dwUserIndex,						// Index of the gamer associated with the device
__in XINPUT_VIBRATION_EX* pVibration					// The vibration information to send to the controller
)
{
	if (!controllerInit)
	{
		connectController(true);
	}

	if (controllerInit && dwUserIndex == 0)
	{
		int leftTriggerVal = iround(((float)pVibration->wLeftTriggerMotorSpeed / 65535) * 255);
		int rightTriggerVal = iround(((float)pVibration->wRightTriggerMotorSpeed / 65535) * 255);
		int leftVal = iround(((float)pVibration->wLeftMotorSpeed / 65535) * 255);
		int rightVal = iround(((float)pVibration->wRightMotorSpeed / 65535) * 255);

		return ERROR_SUCCESS;
	}
	else
	{
		return ERROR_DEVICE_NOT_CONNECTED;
	}
}

LPCWSTR libName = L"xinput1_3.dll";
LPCWSTR daytonalibName = L"xinput9_1_0.dll";
LPCWSTR ptrToUse;

static InitFunction XInputHook([]()
{
	if (GameDetect::currentGame == GameID::PokkenTournament || GameDetect::currentGame == GameID::SchoolOfRagnarok || GameDetect::currentGame == GameID::Daytona3)
	{
		controllerInit = true;

		MH_Initialize();

		if (GameDetect::currentGame == GameID::Daytona3)
			ptrToUse = daytonalibName;
		else
			ptrToUse = libName;

		MH_CreateHookApi(ptrToUse, "XInputGetState", &XInputGetState, NULL);
		MH_CreateHookApi(ptrToUse, "XInputSetState", &XInputSetState, NULL);
		MH_CreateHookApi(ptrToUse, "XInputGetCapabilities", &XInputGetCapabilities, NULL);
		MH_CreateHookApi(ptrToUse, "XInputEnable", &XInputEnable, NULL);
		MH_CreateHookApi(ptrToUse, "XInputGetDSoundAudioDeviceGuids", &XInputGetDSoundAudioDeviceGuids, NULL);
		MH_CreateHookApi(ptrToUse, "XInputGetBatteryInformation", &XInputGetBatteryInformation, NULL);
		MH_CreateHookApi(ptrToUse, "XInputGetKeystroke", &XInputGetKeystroke, NULL);
		MH_CreateHookApi(ptrToUse, "XInputGetStateEx", &XInputGetStateEx, NULL);
		MH_CreateHookApi(ptrToUse, "XInputSetStateEx", &XInputSetStateEx, NULL);

		MH_EnableHook(MH_ALL_HOOKS);
	}
});
#pragma optimize("", on)´
