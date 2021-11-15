#pragma optimize("", off)
// ============================================================================
// ttx_monitor: Opensource TaitoTypeX loader
// by Romhack
// ============================================================================
// Fake API do DirectInput, assim impedimos que os jogos tenham acesso aos controles.
#include <string.h>
#include "StdInc.h"
#include "Utility/InitFunction.h"

#define DIRECTINPUT_VERSION 0x800
#include <dinput.h>

#pragma comment(lib, "dxguid.lib")

#define COM_NO_WINDOWS_H
#include <objbase.h>
#include "Utility/GameDetect.h"

typedef HRESULT(__stdcall *LPDirectInput8Create)(HINSTANCE hinst, DWORD dwVersion, REFIID riidltf, LPVOID * ppvOut, LPUNKNOWN punkOuter);

LPDirectInput8Create __DirectInput8Create;
static DIDEVICEINSTANCEA devi;

#pragma pack(4)
struct FAR HookIUnknown
{
	virtual HRESULT QueryInterface(LPVOID _this, REFIID riid, LPVOID *ppvObj);
	virtual ULONG AddRef(LPVOID _this);
	virtual ULONG Release(LPVOID _this);
};

static HookIUnknown UnknownInfo, *pUnknownInfo = &UnknownInfo;

HRESULT HookIUnknown::QueryInterface(LPVOID _this, REFIID riid, LPVOID *ppvObj)
{
	*ppvObj = NULL;
	return DIERR_UNSUPPORTED;
}

ULONG HookIUnknown::AddRef(LPVOID _this)
{
	return 0;
}

ULONG HookIUnknown::Release(LPVOID _this)
{
	return 0;
}

#pragma pack(4)
struct FAR HookIDirectInputDevice
{
	virtual HRESULT QueryInterface(LPVOID _this, REFIID riid, LPVOID *ppvObj);
	virtual ULONG AddRef(LPVOID _this);
	virtual ULONG Release(LPVOID _this);

	virtual HRESULT GetCapabilities(LPVOID _this, LPDIDEVCAPS lpDIDevCaps);
	virtual HRESULT EnumObjects(LPVOID _this, LPDIENUMDEVICEOBJECTSCALLBACK lpCallback, LPVOID pvRef, DWORD dwFlags);
	virtual HRESULT GetProperty(LPVOID _this, REFGUID rguidProp, LPDIPROPHEADER pdiph);
	virtual HRESULT SetProperty(LPVOID _this, REFGUID rguidProp, LPCDIPROPHEADER pdiph);
	virtual HRESULT Acquire(LPVOID _this);
	virtual HRESULT Unacquire(LPVOID _this);
	virtual HRESULT GetDeviceState(LPVOID _this, DWORD cbData, LPVOID lpvData);
	virtual HRESULT GetDeviceData(LPVOID _this, DWORD cbObjectData, LPDIDEVICEOBJECTDATA rgdod, LPDWORD pdwInOut, DWORD dwFlags);
	virtual HRESULT SetDataFormat(LPVOID _this, LPCDIDATAFORMAT lpdf);
	virtual HRESULT SetEventNotification(LPVOID _this, HANDLE hEvent);
	virtual HRESULT SetCooperativeLevel(LPVOID _this, HWND hwnd, DWORD dwFlags);
	virtual HRESULT GetObjectInfo(LPVOID _this, LPDIDEVICEOBJECTINSTANCE pdidoi, DWORD dwObj, DWORD dwHow);
	virtual HRESULT GetDeviceInfo(LPVOID _this, LPDIDEVICEINSTANCE pdidi);
	virtual HRESULT RunControlPanel(LPVOID _this, HWND hwndOwner, DWORD dwFlags);
	virtual HRESULT Initialize(LPVOID _this, HINSTANCE hinst, DWORD dwVersion, REFGUID rguid);
	virtual HRESULT CreateEffect(LPVOID _this, REFGUID rguid, LPCDIEFFECT lpeff, LPDIRECTINPUTEFFECT * ppdeff, LPUNKNOWN punkOuter);
	virtual HRESULT EnumEffects(LPVOID _this, LPDIENUMEFFECTSCALLBACK lpCallback, LPVOID pvRef, DWORD dwEffType);
	virtual HRESULT GetEffectInfo(LPVOID _this, LPDIEFFECTINFOA pdei, REFGUID rguid);
	virtual HRESULT GetForceFeedbackState(LPVOID _this, LPDWORD pdwOut);
	virtual HRESULT SendForceFeedbackCommand(LPVOID _this, DWORD dwFlags);
	virtual HRESULT EnumCreatedEffectObjects(LPVOID _this, LPDIENUMCREATEDEFFECTOBJECTSCALLBACK lpCallback, LPVOID pvRef, DWORD fl);
	virtual HRESULT Escape(LPVOID _this, LPDIEFFESCAPE pesc);
	virtual HRESULT Poll(LPVOID _this);
	virtual HRESULT SendDeviceData(LPVOID _this, DWORD cbObjectData, LPCDIDEVICEOBJECTDATA rgdod, LPDWORD pdwInOut, DWORD fl);
	virtual HRESULT EnumEffectsInFile(LPVOID _this, LPCSTR lpszFileName, LPDIENUMEFFECTSINFILECALLBACK pec, LPVOID pvRef, DWORD dwFlags);
	virtual HRESULT WriteEffectToFile(LPVOID _this, LPCSTR lpszFileName, DWORD dwEntries, LPCDIFILEEFFECT rgDiFileEft, DWORD dwFlags);
	virtual HRESULT BuildActionMap(LPVOID _this, LPDIACTIONFORMAT lpdiaf, LPCTSTR lpszUserName, DWORD dwFlags);
	virtual HRESULT SetActionMap(LPVOID _this, LPCDIACTIONFORMAT lpdiActionFormat, LPCTSTR lptszUserName, DWORD dwFlags);
	virtual HRESULT GetImageInfo(LPVOID _this, LPDIDEVICEIMAGEINFOHEADER lpdiDevImageInfoHeader);
};

static HookIDirectInputDevice FakeDevice, *pFakeDevice = &FakeDevice;

HRESULT HookIDirectInputDevice::QueryInterface(LPVOID _this, REFIID riid, LPVOID *ppvObj)
{
	*ppvObj = NULL;

	return 0;
}

ULONG HookIDirectInputDevice::AddRef(LPVOID _this)
{
	return 2;
}

ULONG HookIDirectInputDevice::Release(LPVOID _this)
{
	return 0;
}


HRESULT HookIDirectInputDevice::GetCapabilities(LPVOID _this, LPDIDEVCAPS lpDIDevCaps)
{
	return DI_OK;
}

HRESULT HookIDirectInputDevice::EnumObjects(LPVOID _this, LPDIENUMDEVICEOBJECTSCALLBACK lpCallback, LPVOID pvRef, DWORD dwFlags)
{
	return DI_OK;
}

HRESULT HookIDirectInputDevice::GetProperty(LPVOID _this, REFGUID rguidProp, LPDIPROPHEADER pdiph)
{
	return DI_OK;
}

HRESULT HookIDirectInputDevice::SetProperty(LPVOID _this, REFGUID rguidProp, LPCDIPROPHEADER pdiph)
{
	return DI_OK;
}

HRESULT HookIDirectInputDevice::Acquire(LPVOID _this)
{
	return DI_OK;
}

HRESULT HookIDirectInputDevice::Unacquire(LPVOID _this)
{
	return DI_OK;
}

HRESULT HookIDirectInputDevice::GetDeviceState(LPVOID _this, DWORD cbData, LPVOID lpvData)
{
	if (lpvData)
	{
		ZeroMemory(lpvData, cbData);
	}

	return DI_OK;
}

HRESULT HookIDirectInputDevice::GetDeviceData(LPVOID _this, DWORD cbObjectData, LPDIDEVICEOBJECTDATA rgdod, LPDWORD pdwInOut, DWORD dwFlags)
{
	if (rgdod) 
	{
		ZeroMemory(rgdod, cbObjectData * (*pdwInOut));
	}

	return DI_OK;
}

HRESULT HookIDirectInputDevice::SetDataFormat(LPVOID _this, LPCDIDATAFORMAT lpdf)
{
	return DI_OK;
}

HRESULT HookIDirectInputDevice::SetEventNotification(LPVOID _this, HANDLE hEvent)
{
	return DI_OK;
}

HRESULT HookIDirectInputDevice::SetCooperativeLevel(LPVOID _this, HWND hwnd, DWORD dwFlags)
{
	return DI_OK;
}

HRESULT HookIDirectInputDevice::GetObjectInfo(LPVOID _this, LPDIDEVICEOBJECTINSTANCE pdidoi, DWORD dwObj, DWORD dwHow)
{
	return DI_OK;
}

HRESULT HookIDirectInputDevice::GetDeviceInfo(LPVOID _this, LPDIDEVICEINSTANCE pdidi)
{
	return DI_OK;
}

HRESULT HookIDirectInputDevice::RunControlPanel(LPVOID _this, HWND hwndOwner, DWORD dwFlags)
{
	return DI_OK;
}

HRESULT HookIDirectInputDevice::Initialize(LPVOID _this, HINSTANCE hinst, DWORD dwVersion, REFGUID rguid)
{
	return DI_OK;
}

HRESULT HookIDirectInputDevice::CreateEffect(LPVOID _this, REFGUID rguid, LPCDIEFFECT lpeff, LPDIRECTINPUTEFFECT * ppdeff,LPUNKNOWN punkOuter)
{
	return DIERR_UNSUPPORTED;
}

HRESULT HookIDirectInputDevice::EnumEffects(LPVOID _this, LPDIENUMEFFECTSCALLBACK lpCallback, LPVOID pvRef, DWORD dwEffType) 
{
	return DIERR_UNSUPPORTED;
}

HRESULT HookIDirectInputDevice::GetEffectInfo(LPVOID _this, LPDIEFFECTINFOA pdei, REFGUID rguid) 
{
	return DIERR_UNSUPPORTED;
}


HRESULT HookIDirectInputDevice::GetForceFeedbackState(LPVOID _this, LPDWORD pdwOut) 
{
	return DIERR_UNSUPPORTED;
}

HRESULT HookIDirectInputDevice::SendForceFeedbackCommand(LPVOID _this, DWORD dwFlags) 
{
	 return DIERR_UNSUPPORTED;
}

HRESULT HookIDirectInputDevice::EnumCreatedEffectObjects(LPVOID _this, LPDIENUMCREATEDEFFECTOBJECTSCALLBACK lpCallback, LPVOID pvRef, DWORD fl) 
{
	 return DIERR_UNSUPPORTED;
}

HRESULT HookIDirectInputDevice::Escape(LPVOID _this, LPDIEFFESCAPE pesc) 
{
	 return DIERR_UNSUPPORTED;
}

HRESULT HookIDirectInputDevice::Poll(LPVOID _this) 
{ 
	return DI_NOEFFECT; 
}

HRESULT HookIDirectInputDevice::SendDeviceData(LPVOID _this, DWORD cbObjectData, LPCDIDEVICEOBJECTDATA rgdod, LPDWORD pdwInOut, DWORD fl)
{
	return DIERR_UNSUPPORTED;
}

HRESULT HookIDirectInputDevice::EnumEffectsInFile(LPVOID _this, LPCSTR lpszFileName, LPDIENUMEFFECTSINFILECALLBACK pec, LPVOID pvRef, DWORD dwFlags)
{
	return DIERR_UNSUPPORTED;
}

HRESULT HookIDirectInputDevice::WriteEffectToFile(LPVOID _this, LPCSTR lpszFileName, DWORD dwEntries, LPCDIFILEEFFECT rgDiFileEft, DWORD dwFlags) 
{
	return DIERR_UNSUPPORTED;
}

HRESULT HookIDirectInputDevice::BuildActionMap(LPVOID _this, LPDIACTIONFORMAT lpdiaf, LPCTSTR lpszUserName, DWORD dwFlags)
{
	return DIERR_UNSUPPORTED;
}

HRESULT HookIDirectInputDevice::SetActionMap(LPVOID _this, LPCDIACTIONFORMAT lpdiActionFormat, LPCTSTR lptszUserName, DWORD dwFlags) 
{
	return DIERR_UNSUPPORTED;
}

HRESULT HookIDirectInputDevice::GetImageInfo(LPVOID _this, LPDIDEVICEIMAGEINFOHEADER lpdiDevImageInfoHeader) 
{
	return DIERR_UNSUPPORTED;
}

#pragma pack(4)
struct FAR HookIDirectInput8
{
	virtual HRESULT QueryInterface(LPVOID _this, REFIID riid, LPVOID *ppvObj);
	virtual ULONG AddRef(LPVOID _this);
	virtual ULONG Release(LPVOID _this);

	virtual HRESULT CreateDevice(LPVOID _this, REFGUID rguid, LPDIRECTINPUTDEVICE8A * lplpDirectInputDevice, LPUNKNOWN pUnkOuter);

	virtual HRESULT EnumDevices(LPVOID _this, DWORD dwDevType, LPDIENUMDEVICESCALLBACK lpCallback, LPVOID pvRef, DWORD dwFlags);

	virtual HRESULT GetDeviceStatus(LPVOID _this, REFGUID rguidInstance);

	virtual HRESULT RunControlPanel(LPVOID _this, HWND hwndOwner, DWORD dwFlags);

	virtual HRESULT Initialize(LPVOID _this, HINSTANCE hinst, DWORD dwVersion);

	virtual HRESULT FindDevice(LPVOID _this, REFGUID rguidClass, LPCTSTR ptszName, LPGUID pguidInstance);

	virtual HRESULT EnumDevicesBySemantics(LPVOID _this, LPCTSTR ptszUserName, LPDIACTIONFORMAT lpdiActionFormat, LPDIENUMDEVICESBYSEMANTICSCB lpCallback, LPVOID pvRef, DWORD dwFlags);

	virtual HRESULT ConfigureDevices(LPVOID _this, LPDICONFIGUREDEVICESCALLBACK lpdiCallback, LPDICONFIGUREDEVICESPARAMS lpdiCDParams, DWORD dwFlags, LPVOID pvRefData);
};

static HookIDirectInput8 FakeInterface, *pFakeInterface = &FakeInterface;

HRESULT HookIDirectInput8::QueryInterface(LPVOID _this, REFIID riid, LPVOID *ppvObj)
{
	*ppvObj = NULL;

	return DIERR_UNSUPPORTED;
}

ULONG HookIDirectInput8::AddRef(LPVOID _this)
{
	return 0;
}

ULONG HookIDirectInput8::Release(LPVOID _this)
{
	return 0;
}

HRESULT HookIDirectInput8::CreateDevice(LPVOID _this, REFGUID rguid, LPDIRECTINPUTDEVICE8A * lplpDirectInputDevice, LPUNKNOWN pUnkOuter)
{
	if (rguid == GUID_SysKeyboard)
	{
//		info(true, V("DirectInput8::CreateDevice: Device is keyboard"));
	}
	else if (rguid == GUID_SysMouse)
	{
//		info(true, V("DirectInput8::CreateDevice: Device is mouse"));
	}
	else if (rguid == GUID_Joystick)
	{
//		info(true, V("DirectInput8::CreateDevice: Device is joypad"));
	}

	*lplpDirectInputDevice = (LPDIRECTINPUTDEVICE8A)pFakeDevice;

	if (pUnkOuter) 
	{
		pUnkOuter = (LPUNKNOWN)NULL;
	}

	return DI_OK;
}

HRESULT HookIDirectInput8::EnumDevices(LPVOID _this, DWORD dwDevType, LPDIENUMDEVICESCALLBACK lpCallback, LPVOID pvRef, DWORD dwFlags)
{
	return DI_OK;
}


HRESULT HookIDirectInput8::GetDeviceStatus(LPVOID _this, REFGUID rguidInstance)
{
	return DI_OK;
}


HRESULT HookIDirectInput8::RunControlPanel(LPVOID _this, HWND hwndOwner, DWORD dwFlags)
{
	return DIERR_UNSUPPORTED;
}

HRESULT HookIDirectInput8::Initialize(LPVOID _this, HINSTANCE hinst, DWORD dwVersion)
{
	return DI_OK;
}

HRESULT HookIDirectInput8::FindDevice(LPVOID _this, REFGUID rguidClass, LPCTSTR ptszName, LPGUID pguidInstance)
{
	return DI_OK;
}

HRESULT HookIDirectInput8::EnumDevicesBySemantics(LPVOID _this, LPCTSTR ptszUserName, LPDIACTIONFORMAT lpdiActionFormat, LPDIENUMDEVICESBYSEMANTICSCB lpCallback, LPVOID pvRef, DWORD dwFlags)
{
	return DI_OK;
}

HRESULT HookIDirectInput8::ConfigureDevices(LPVOID _this, LPDICONFIGUREDEVICESCALLBACK lpdiCallback, LPDICONFIGUREDEVICESPARAMS lpdiCDParams, DWORD dwFlags, LPVOID pvRefData)
{
	return DI_OK;
}

HRESULT __stdcall Hook_DirectInput8Create(HINSTANCE hinst, DWORD dwVersion, REFIID riidltf, LPVOID * ppvOut, LPUNKNOWN punkOuter)
{
		*ppvOut = (LPVOID)pFakeInterface;

		return DI_OK;
}

static InitFunction initFunc([]() 
{
	if (GameDetect::currentGame == GameID::PokkenTournament 
		|| GameDetect::currentGame == GameID::PokkenTournament26 
		|| GameDetect::currentGame == GameID::FNFDrift 
		|| GameDetect::currentGame == GameID::FNFSC 
		|| GameDetect::currentGame == GameID::FNF 
		|| GameDetect::currentGame == GameID::FNFSB 
		|| GameDetect::currentGame == GameID::FNFSB2 
		|| GameDetect::currentGame == GameID::JLeague
		|| GameDetect::currentGame == GameID::VF5Esports
		|| GameDetect::currentGame == GameID::VF5EsportsTest
		|| GameDetect::currentGame == GameID::BlazBlueCrossTagBattle)
		return;
	MH_Initialize();

	MH_CreateHookApi(L"DINPUT8.dll", "DirectInput8Create", &Hook_DirectInput8Create, (void**)&__DirectInput8Create);

	MH_EnableHook(MH_ALL_HOOKS);
});
#pragma optimize("", on)