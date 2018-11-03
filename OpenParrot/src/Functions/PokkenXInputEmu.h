#include "StdInc.h"

//
// Stricture used for our Xbox One Controller
//
struct XBOXONE_STATE
{
	int guideButton;
	int view;
	int menu;
	int rightShoulder;
	int rightTrigger;
	int rightThumb;
	int leftShoulder;
	int leftTrigger;
	int leftThumb;
	int thumbRX;
	int thumbRY;
	int thumbLX;
	int thumbLY;
	int up;
	int down;
	int left;
	int right;
	int yButton;
	int bButton;
	int aButton;
	int xButton;
};

//
// Device types available in XINPUT_CAPABILITIES
//
#define XINPUT_DEVTYPE_GAMEPAD          0x01

//
// Device subtypes available in XINPUT_CAPABILITIES
//
#define XINPUT_DEVSUBTYPE_GAMEPAD       0x01

//
// Flags for XINPUT_CAPABILITIES
//
#define XINPUT_CAPS_VOICE_SUPPORTED     0x0004

//
// Constants for gamepad buttons
//
#define XINPUT_GAMEPAD_DPAD_UP          0x0001
#define XINPUT_GAMEPAD_DPAD_DOWN        0x0002
#define XINPUT_GAMEPAD_DPAD_LEFT        0x0004
#define XINPUT_GAMEPAD_DPAD_RIGHT       0x0008
#define XINPUT_GAMEPAD_START            0x0010
#define XINPUT_GAMEPAD_BACK             0x0020
#define XINPUT_GAMEPAD_LEFT_THUMB       0x0040
#define XINPUT_GAMEPAD_RIGHT_THUMB      0x0080
#define XINPUT_GAMEPAD_LEFT_SHOULDER    0x0100
#define XINPUT_GAMEPAD_RIGHT_SHOULDER   0x0200
#define XINPUT_GAMEPAD_A                0x1000
#define XINPUT_GAMEPAD_B                0x2000
#define XINPUT_GAMEPAD_X                0x4000
#define XINPUT_GAMEPAD_Y                0x8000

#define XINPUT_GAMEPAD_GUIDE			0x400

//
// Flags to pass to XInputGetCapabilities
//
#define XINPUT_FLAG_GAMEPAD             0x00000001

//
// Devices that support batteries
//
#define BATTERY_DEVTYPE_GAMEPAD         0x00
#define BATTERY_DEVTYPE_HEADSET         0x01

//
// Flags for battery status level
//
#define BATTERY_TYPE_DISCONNECTED       0x00    // This device is not connected
#define BATTERY_TYPE_WIRED              0x01    // Wired device, no battery
#define BATTERY_TYPE_ALKALINE           0x02    // Alkaline battery source
#define BATTERY_TYPE_NIMH               0x03    // Nickel Metal Hydride battery source
#define BATTERY_TYPE_UNKNOWN            0xFF    // Cannot determine the battery type

// These are only valid for wireless, connected devices, with known battery types
// The amount of use time remaining depends on the type of device.
#define BATTERY_LEVEL_EMPTY             0x00
#define BATTERY_LEVEL_LOW               0x01
#define BATTERY_LEVEL_MEDIUM            0x02
#define BATTERY_LEVEL_FULL              0x03

// User index definitions
#define XUSER_MAX_COUNT                 4

//
// Structures used by XInput APIs
//
typedef struct _XINPUT_GAMEPAD
{
	WORD                                wButtons;
	BYTE                                bLeftTrigger;
	BYTE                                bRightTrigger;
	SHORT                               sThumbLX;
	SHORT                               sThumbLY;
	SHORT                               sThumbRX;
	SHORT                               sThumbRY;
} XINPUT_GAMEPAD, *PXINPUT_GAMEPAD;

typedef struct _XINPUT_STATE
{
	DWORD                               dwPacketNumber;
	XINPUT_GAMEPAD                      Gamepad;
} XINPUT_STATE, *PXINPUT_STATE;

typedef struct _XINPUT_VIBRATION
{
	WORD                                wLeftMotorSpeed;
	WORD                                wRightMotorSpeed;
} XINPUT_VIBRATION, *PXINPUT_VIBRATION;

typedef struct _XINPUT_VIBRATION_EX
{
	WORD                                wLeftMotorSpeed;
	WORD                                wRightMotorSpeed;
	WORD                                wLeftTriggerMotorSpeed;
	WORD                                wRightTriggerMotorSpeed;
} XINPUT_VIBRATION_EX, *PXINPUT_VIBRATION_EX;

typedef struct _XINPUT_CAPABILITIES
{
	BYTE                                Type;
	BYTE                                SubType;
	WORD                                Flags;
	XINPUT_GAMEPAD                      Gamepad;
	XINPUT_VIBRATION                    Vibration;
} XINPUT_CAPABILITIES, *PXINPUT_CAPABILITIES;

typedef struct _XINPUT_BATTERY_INFORMATION
{
	BYTE BatteryType;
	BYTE BatteryLevel;
} XINPUT_BATTERY_INFORMATION, *PXINPUT_BATTERY_INFORMATION;

typedef struct _XINPUT_KEYSTROKE
{
	WORD    VirtualKey;
	WCHAR   Unicode;
	WORD    Flags;
	BYTE    UserIndex;
	BYTE    HidCode;
} XINPUT_KEYSTROKE, *PXINPUT_KEYSTROKE;

extern static bool daytonaPressStart;