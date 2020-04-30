#define DIRECTSOUND_VERSION 0x0800
#include "StdInc.h"
#include "mmsystem.h"
#include <dsound.h>
#include "Utility/InitFunction.h"
#include "Utility/GameDetect.h"

struct FAR HookIDirectSound8
{
	/*** IUnknown methods ***/
	virtual HRESULT QueryInterface(LPVOID _this, REFIID riid, LPVOID* ppvObj);
	virtual ULONG AddRef(LPVOID _this);
	virtual ULONG Release(LPVOID _this);

	// IDirectSound methods
	virtual HRESULT CreateSoundBuffer(LPVOID _this, LPCDSBUFFERDESC pcDSBufferDesc, LPDIRECTSOUNDBUFFER* ppDSBuffer, __null LPUNKNOWN pUnkOuter);
	virtual HRESULT GetCaps(LPVOID _this, LPDSCAPS pDSCaps);
	virtual HRESULT DuplicateSoundBuffer(LPVOID _this, LPDIRECTSOUNDBUFFER pDSBufferOriginal, LPDIRECTSOUNDBUFFER* ppDSBufferDuplicate);
	virtual HRESULT SetCooperativeLevel(LPVOID _this, HWND hwnd, DWORD dwLevel);
	virtual HRESULT Compact(LPVOID _this);
	virtual HRESULT GetSpeakerConfig(LPVOID _this, LPDWORD pdwSpeakerConfig);
	virtual HRESULT SetSpeakerConfig(LPVOID _this, DWORD dwSpeakerConfig);
	virtual HRESULT Initialize(LPVOID _this, LPCGUID pcGuidDevice);

	// IDirectSound8 methods
	virtual HRESULT VerifyCertification(LPVOID _this, LPDWORD pdwCertified);
};

struct FAR HookIDirectSoundBuffer8
{
	/*** IUnknown methods ***/
	virtual HRESULT QueryInterface(LPVOID _this, REFIID riid, LPVOID* ppvObj);
	virtual ULONG AddRef(LPVOID _this);
	virtual ULONG Release(LPVOID _this);

	// IDirectSoundBuffer methods
	virtual HRESULT GetCaps(LPVOID _this, LPDSBCAPS pDSBufferCaps);
	virtual HRESULT GetCurrentPosition(LPVOID _this, LPDWORD pdwCurrentPlayCursor, LPDWORD pdwCurrentWriteCursor);
	virtual HRESULT GetFormat(LPVOID _this, LPWAVEFORMATEX pwfxFormat, DWORD dwSizeAllocated, LPDWORD pdwSizeWritten);
	virtual HRESULT GetVolume(LPVOID _this, LPLONG plVolume);
	virtual HRESULT GetPan(LPVOID _this, LPLONG plPan);
	virtual HRESULT GetFrequency(LPVOID _this, LPDWORD pdwFrequency);
	virtual HRESULT GetStatus(LPVOID _this, LPDWORD pdwStatus);
	virtual HRESULT Initialize(LPVOID _this, LPDIRECTSOUND pDirectSound, LPCDSBUFFERDESC pcDSBufferDesc);
	virtual HRESULT Lock(LPVOID _this, DWORD dwOffset, DWORD dwBytes, LPVOID* ppvAudioPtr1, LPDWORD pdwAudioBytes1, LPVOID* ppvAudioPtr2, LPDWORD pdwAudioBytes2, DWORD dwFlags);
	virtual HRESULT Play(LPVOID _this, DWORD dwReserved1, DWORD dwPriority, DWORD dwFlags);
	virtual HRESULT SetCurrentPosition(LPVOID _this, DWORD dwNewPosition);
	virtual HRESULT SetFormat(LPVOID _this, LPCWAVEFORMATEX pcfxFormat);
	virtual HRESULT SetVolume(LPVOID _this, LONG lVolume);
	virtual HRESULT SetPan(LPVOID _this, LONG lPan);
	virtual HRESULT SetFrequency(LPVOID _this, DWORD dwFrequency);
	virtual HRESULT Stop(LPVOID _this);
	virtual HRESULT Unlock(LPVOID _this, LPVOID pvAudioPtr1, DWORD dwAudioBytes1, LPVOID pvAudioPtr2, DWORD dwAudioBytes2);
	virtual HRESULT Restore(LPVOID _this);
	virtual HRESULT SetFX(LPVOID _this, DWORD dwEffectsCount, LPDSEFFECTDESC pDSFXDesc, LPDWORD pdwResultCodes);
	virtual HRESULT AcquireResources(LPVOID _this, DWORD dwFlags, DWORD dwEffectsCount, LPDWORD pdwResultCodes);
	virtual HRESULT GetObjectInPath(LPVOID _this, REFGUID rguidObject, DWORD dwIndex, REFGUID rguidInterface, LPVOID* ppObject);
};

HRESULT __stdcall Hook_DirectSoundCreate8(LPCGUID lpcGuidDevice,LPDIRECTSOUND8* ppDS8,LPUNKNOWN pUnkOuter);
typedef HRESULT(__stdcall* LPDirectSoundCreate8)(LPCGUID lpcGuidDevice,LPDIRECTSOUND8* ppDS8,LPUNKNOWN pUnkOuter);
extern LPDirectSoundCreate8 __DirectSoundCreate8;

LPDirectSoundCreate8 __DirectSoundCreate8;

static LPDIRECTSOUND8 pDS;
static LPDIRECTSOUNDBUFFER8 ppDSB; // Primary Sound Buffer

static HookIDirectSoundBuffer8 DSBWrapper, * pDSBWrapper = &DSBWrapper;
static HookIDirectSound8 DSWrapper, * pDSWrapper = &DSWrapper;

HRESULT HookIDirectSound8::QueryInterface(LPVOID _this, REFIID riid, LPVOID* ppvObj)
{
	return pDS->QueryInterface(riid, ppvObj);
}

ULONG HookIDirectSound8::AddRef(LPVOID _this)
{
	return pDS->AddRef();
}

ULONG HookIDirectSound8::Release(LPVOID _this)
{
	return pDS->Release();
}

HRESULT HookIDirectSound8::CreateSoundBuffer(LPVOID _this, LPCDSBUFFERDESC pcDSBufferDesc, LPDIRECTSOUNDBUFFER* ppDSBuffer, __null LPUNKNOWN pUnkOuter)
{
	if (!(pcDSBufferDesc->dwFlags & DSBCAPS_PRIMARYBUFFER)) {
		WAVEFORMATEX* wfx = pcDSBufferDesc->lpwfxFormat;
	}
	else {
		// primary buffer
		HRESULT result = pDS->CreateSoundBuffer(pcDSBufferDesc, (LPDIRECTSOUNDBUFFER*)&ppDSB, pUnkOuter);
		*ppDSBuffer = (LPDIRECTSOUNDBUFFER)pDSBWrapper;
		return result;
	}
	return pDS->CreateSoundBuffer(pcDSBufferDesc, ppDSBuffer, pUnkOuter);
}

HRESULT HookIDirectSound8::GetCaps(LPVOID _this, LPDSCAPS pDSCaps)
{
	return pDS->GetCaps(pDSCaps);
}

HRESULT HookIDirectSound8::DuplicateSoundBuffer(LPVOID _this, LPDIRECTSOUNDBUFFER pDSBufferOriginal, LPDIRECTSOUNDBUFFER* ppDSBufferDuplicate)
{
	return pDS->DuplicateSoundBuffer(pDSBufferOriginal, ppDSBufferDuplicate);
}

HRESULT HookIDirectSound8::SetCooperativeLevel(LPVOID _this, HWND hwnd, DWORD dwLevel)
{
	return pDS->SetCooperativeLevel(hwnd, dwLevel);
}

HRESULT HookIDirectSound8::Compact(LPVOID _this)
{
	return pDS->Compact();
}

HRESULT HookIDirectSound8::GetSpeakerConfig(LPVOID _this, LPDWORD pdwSpeakerConfig)
{
	if (pdwSpeakerConfig) {
		*pdwSpeakerConfig = DSSPEAKER_COMBINED(DSSPEAKER_7POINT1_SURROUND, DSSPEAKER_GEOMETRY_MAX);
	}
	return DS_OK;
}

HRESULT HookIDirectSound8::SetSpeakerConfig(LPVOID _this, DWORD dwSpeakerConfig)
{
	return pDS->SetSpeakerConfig(dwSpeakerConfig);
}

HRESULT HookIDirectSound8::Initialize(LPVOID _this, LPCGUID pcGuidDevice)
{
	return pDS->Initialize(pcGuidDevice);
}

HRESULT HookIDirectSound8::VerifyCertification(LPVOID _this, LPDWORD pdwCertified)
{
	return pDS->VerifyCertification(pdwCertified);
}

HRESULT __stdcall Hook_DirectSoundCreate8(LPCGUID lpcGuidDevice,LPDIRECTSOUND8* ppDS8,LPUNKNOWN pUnkOuter)
{
	HRESULT result = __DirectSoundCreate8(lpcGuidDevice, &pDS, pUnkOuter);

	*ppDS8 = (LPDIRECTSOUND8)pDSWrapper;
	pUnkOuter = NULL;
	return result;
}

HRESULT HookIDirectSoundBuffer8::QueryInterface(LPVOID _this, REFIID riid, LPVOID* ppvObj)
{
	return ppDSB->QueryInterface(riid, ppvObj);
}

ULONG HookIDirectSoundBuffer8::AddRef(LPVOID _this)
{
	return ppDSB->AddRef();
}

ULONG HookIDirectSoundBuffer8::Release(LPVOID _this)
{
	return ppDSB->Release();
}

HRESULT HookIDirectSoundBuffer8::GetCaps(LPVOID _this, LPDSBCAPS pDSBufferCaps)
{
	return ppDSB->GetCaps(pDSBufferCaps);
}

HRESULT HookIDirectSoundBuffer8::GetCurrentPosition(LPVOID _this, LPDWORD pdwCurrentPlayCursor, LPDWORD pdwCurrentWriteCursor)
{
	return ppDSB->GetCurrentPosition(pdwCurrentPlayCursor, pdwCurrentWriteCursor);
}

HRESULT HookIDirectSoundBuffer8::GetFormat(LPVOID _this, LPWAVEFORMATEX pwfxFormat, DWORD dwSizeAllocated, LPDWORD pdwSizeWritten)
{
	return ppDSB->GetFormat(pwfxFormat, dwSizeAllocated, pdwSizeWritten);
}

HRESULT HookIDirectSoundBuffer8::GetVolume(LPVOID _this, LPLONG plVolume)
{
	return ppDSB->GetVolume(plVolume);
}

HRESULT HookIDirectSoundBuffer8::GetPan(LPVOID _this, LPLONG plPan)
{
	return ppDSB->GetPan(plPan);
}

HRESULT HookIDirectSoundBuffer8::GetFrequency(LPVOID _this, LPDWORD pdwFrequency)
{
	return ppDSB->GetFrequency(pdwFrequency);
}

HRESULT HookIDirectSoundBuffer8::GetStatus(LPVOID _this, LPDWORD pdwStatus)
{
	return ppDSB->GetStatus(pdwStatus);
}

HRESULT HookIDirectSoundBuffer8::Initialize(LPVOID _this, LPDIRECTSOUND pDirectSound, LPCDSBUFFERDESC pcDSBufferDesc)
{
	return ppDSB->Initialize(pDirectSound, pcDSBufferDesc);
}

HRESULT HookIDirectSoundBuffer8::Lock(LPVOID _this, DWORD dwOffset, DWORD dwBytes, LPVOID* ppvAudioPtr1, LPDWORD pdwAudioBytes1, LPVOID* ppvAudioPtr2, LPDWORD pdwAudioBytes2, DWORD dwFlags)
{
	return ppDSB->Lock(dwOffset, dwBytes, ppvAudioPtr1, pdwAudioBytes1, ppvAudioPtr2, pdwAudioBytes2, dwFlags);
}

HRESULT HookIDirectSoundBuffer8::Play(LPVOID _this, DWORD dwReserved1, DWORD dwPriority, DWORD dwFlags)
{
	return ppDSB->Play(dwReserved1, dwPriority, dwFlags);
}

HRESULT HookIDirectSoundBuffer8::SetCurrentPosition(LPVOID _this, DWORD dwNewPosition)
{
	return ppDSB->SetCurrentPosition(dwNewPosition);
}

HRESULT HookIDirectSoundBuffer8::SetFormat(LPVOID _this, LPCWAVEFORMATEX pcfxFormat)
{
	WAVEFORMATEX wfx;
	CopyMemory(&wfx, pcfxFormat, sizeof(wfx));
	wfx.wFormatTag = 0;
	wfx.nChannels = 2;

	return ppDSB->SetFormat(&wfx);
}

HRESULT HookIDirectSoundBuffer8::SetVolume(LPVOID _this, LONG lVolume)
{
	return ppDSB->SetVolume(lVolume);
}

HRESULT HookIDirectSoundBuffer8::SetPan(LPVOID _this, LONG lPan)
{
	return ppDSB->SetPan(lPan);
}

HRESULT HookIDirectSoundBuffer8::SetFrequency(LPVOID _this, DWORD dwFrequency)
{
	return ppDSB->SetFrequency(dwFrequency);
}

HRESULT HookIDirectSoundBuffer8::Stop(LPVOID _this)
{
	return ppDSB->Stop();
}

HRESULT HookIDirectSoundBuffer8::Unlock(LPVOID _this, LPVOID pvAudioPtr1, DWORD dwAudioBytes1, LPVOID pvAudioPtr2, DWORD dwAudioBytes2)
{
	return ppDSB->Unlock(pvAudioPtr1, dwAudioBytes1, pvAudioPtr2, dwAudioBytes2);
}

HRESULT HookIDirectSoundBuffer8::Restore(LPVOID _this)
{
	return ppDSB->Restore();
}

HRESULT HookIDirectSoundBuffer8::SetFX(LPVOID _this, DWORD dwEffectsCount, LPDSEFFECTDESC pDSFXDesc, LPDWORD pdwResultCodes)
{
	return ppDSB->SetFX(dwEffectsCount, pDSFXDesc, pdwResultCodes);
}

HRESULT HookIDirectSoundBuffer8::AcquireResources(LPVOID _this, DWORD dwFlags, DWORD dwEffectsCount, LPDWORD pdwResultCodes)
{
	return ppDSB->AcquireResources(dwFlags, dwEffectsCount, pdwResultCodes);
}

HRESULT HookIDirectSoundBuffer8::GetObjectInPath(LPVOID _this, REFGUID rguidObject, DWORD dwIndex, REFGUID rguidInterface, LPVOID* ppObject)
{
	return ppDSB->GetObjectInPath(rguidObject, dwIndex, rguidInterface, ppObject);
}

static InitFunction initFunc([]()
	{
		if (GameDetect::currentGame == GameID::BG4)
		{
			MH_Initialize();

			MH_CreateHookApi(L"DSOUND.dll", "DirectSoundCreate8", &Hook_DirectSoundCreate8, (void**)&__DirectSoundCreate8);

			MH_EnableHook(MH_ALL_HOOKS);
		}
	});
#pragma optimize("", on)