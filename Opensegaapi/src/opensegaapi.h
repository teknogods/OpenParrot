/*
* This file is part of the OpenParrot project - https://teknoparrot.com / https://github.com/teknogods
*
* See LICENSE and MENTIONS in the root of the source tree for information
* regarding licensing.
*/

#include <guiddef.h>

// TODO: DOCUMENT ALL THESE ACCORDING TO ORIGINAL DOCUMENTS!!!!

#define OPEN_SEGA_SUCCESS 0L
typedef int OPENSEGASTATUS;
#define OPENSEGARESULT_FAILURE(_x)      ((1 << 31) | 0xA000 | (_x))
#define OPENSEGAERR_BAD_HANDLE                    OPENSEGARESULT_FAILURE(18)
#define OPENSEGAERR_BAD_POINTER                   OPENSEGARESULT_FAILURE(3)

// SEGA custom EAX40 properties.
// GUID: {A7FEEC3F-2BFD-4a40-891F-7423E38BAC1F}
DEFINE_GUID(EAXPROPERTYID_EAX40_SEGA_Custom,
	0xa7feec3f, 0x2bfd, 0x4a40, 0x89, 0x1f, 0x74, 0x23, 0xe3, 0x8b, 0xac, 0x1f);

typedef enum
{
	EAXOPENSEGA_STEREO_RETURN_FX2 = 0, 	// front L/R (default)
	EAXOPENSEGA_STEREO_RETURN_FX3 = 1 // rear L/R
} EAXOPENSEGA_PROPERTY;

typedef enum
{
	OPEN_HAWOS_RESOURCE_STOLEN = 0,
	OPEN_HAWOS_NOTIFY = 2
} OPEN_HAWOSMESSAGETYPE;

typedef enum
{
	OPEN_HAWOSTATUS_STOP,
	OPEN_HAWOSTATUS_ACTIVE,
	OPEN_HAWOSTATUS_PAUSE,
	OPEN_HAWOSTATUS_INVALID = -1
} OPEN_HAWOSTATUS;

#define OPEN_HABUF_SYNTH_BUFFER		0x00000001
#define OPEN_HABUF_ALLOC_USER_MEM    0x00000002
#define OPEN_HABUF_USE_MAPPED_MEM    0x00000004
#define OPEN_HASF_UNSIGNED_8PCM      0x0004
#define OPEN_HASF_SIGNED_16PCM       0x0020

typedef struct
{
	unsigned int dwSampleRate;
	unsigned int dwSampleFormat;
	unsigned int byNumChans;
} OPEN_HAWOSEFORMAT;

typedef struct
{
	unsigned int dwSize;
	unsigned int dwOffset;
	void* hBufferHdr;
} OPEN_HAWOSEMAPDATA;

typedef struct
{
	unsigned int dwPriority;
	unsigned int dwSampleRate;
	unsigned int dwSampleFormat;
	unsigned int byNumChans;
	unsigned int dwReserved;
	void* hUserData;
	OPEN_HAWOSEMAPDATA mapData;
} OPEN_HAWOSEBUFFERCONFIG;

#define OPEN_HAWOSEVOL_MAX      0xFFFFFFFF
#define OPEN_HAWOSEP_MINIMUM    0
#define OPEN_HAWOSEP_MAXIMUM    0xFFFFFFFF
#define OPEN_HAWOSE_UNUSED_SEND 0xFFFF0001

typedef enum OPEN_HAROUTING
{
	OPEN_HA_UNUSED_PORT = OPEN_HAWOSE_UNUSED_SEND,
	OPEN_HA_FRONT_LEFT_PORT = 0,
	OPEN_HA_FRONT_RIGHT_PORT = 1,
	OPEN_HA_FRONT_CENTER_PORT = 2,
	OPEN_HA_LFE_PORT = 3,
	OPEN_HA_REAR_LEFT_PORT = 4,
	OPEN_HA_REAR_RIGHT_PORT = 5,
	OPEN_HA_FXSLOT0_PORT = 10,
	OPEN_HA_FXSLOT1_PORT = 11,
	OPEN_HA_FXSLOT2_PORT = 12,
	OPEN_HA_FXSLOT3_PORT = 13
} OPEN_HAROUTING;

typedef enum
{
	OPEN_HASPDIFOUT_44_1KHZ = 0,
	OPEN_HASPDIFOUT_48KHZ,
	OPEN_HASPDIFOUT_96KHZ
} OPEN_HASPDIFOUTRATE;

typedef enum OPEN_HAPHYSICALIO
{
	OPEN_HA_OUT_FRONT_LEFT = 0,
	OPEN_HA_OUT_FRONT_RIGHT = 1,
	OPEN_HA_OUT_FRONT_CENTER = 2,
	OPEN_HA_OUT_LFE_PORT = 3,
	OPEN_HA_OUT_REAR_LEFT = 4,
	OPEN_HA_OUT_REAR_RIGHT = 5,
	OPEN_HA_OUT_OPTICAL_LEFT = 10,
	OPEN_HA_OUT_OPTICAL_RIGHT = 11,
	OPEN_HA_IN_LINEIN_LEFT = 20,
	OPEN_HA_IN_LINEIN_RIGHT = 21
} OPEN_HAPHYSICALIO;

typedef enum OPEN_HASYNTHPARAMSEXT
{
	OPEN_HAVP_ATTENUATION,
	OPEN_HAVP_PITCH,
	OPEN_HAVP_FILTER_CUTOFF,
	OPEN_HAVP_FILTER_Q,
	OPEN_HAVP_DELAY_VOL_ENV,
	OPEN_HAVP_ATTACK_VOL_ENV,
	OPEN_HAVP_HOLD_VOL_ENV,
	OPEN_HAVP_DECAY_VOL_ENV,
	OPEN_HAVP_SUSTAIN_VOL_ENV,
	OPEN_HAVP_RELEASE_VOL_ENV,
	OPEN_HAVP_DELAY_MOD_ENV,
	OPEN_HAVP_ATTACK_MOD_ENV,
	OPEN_HAVP_HOLD_MOD_ENV,
	OPEN_HAVP_DECAY_MOD_ENV,
	OPEN_HAVP_SUSTAIN_MOD_ENV,
	OPEN_HAVP_RELEASE_MOD_ENV,
	OPEN_HAVP_DELAY_MOD_LFO,
	OPEN_HAVP_FREQ_MOD_LFO,
	OPEN_HAVP_DELAY_VIB_LFO,
	OPEN_HAVP_FREQ_VIB_LFO,
	OPEN_HAVP_MOD_LFO_TO_PITCH,
	OPEN_HAVP_VIB_LFO_TO_PITCH,
	OPEN_HAVP_MOD_LFO_TO_FILTER_CUTOFF,
	OPEN_HAVP_MOD_LFO_TO_ATTENUATION,
	OPEN_HAVP_MOD_ENV_TO_PITCH,
	OPEN_HAVP_MOD_ENV_TO_FILTER_CUTOFF
} OPEN_HASYNTHPARAMSEXT;

typedef enum
{
	OPEN_VOICEIOCTL_SET_START_LOOP_OFFSET = 0x100,
	OPEN_VOICEIOCTL_SET_END_LOOP_OFFSET,
	OPEN_VOICEIOCTL_SET_END_OFFSET,
	OPEN_VOICEIOCTL_SET_PLAY_POSITION,
	OPEN_VOICEIOCTL_SET_LOOP_STATE,
	OPEN_VOICEIOCTL_SET_NOTIFICATION_POINT,
	OPEN_VOICEIOCTL_CLEAR_NOTIFICATION_POINT,
	OPEN_VOICEIOCTL_SET_NOTIFICATION_FREQUENCY
} OPEN_VOICEIOCTL;

typedef struct OPEN_SendRouteParamSetExt
{
	unsigned int dwChannel;
	unsigned int dwSend;
	OPEN_HAROUTING dwDest;
} OPEN_SendRouteParamSet;

typedef struct OPEN_SendLevelParamSetExt
{
	unsigned int dwChannel;
	unsigned int dwSend;
	unsigned int dwLevel;
} OPEN_SendLevelParamSet;

typedef struct OPEN_VoiceParamSetExt
{
	OPEN_VOICEIOCTL VoiceIoctl;
	unsigned int dwParam1;
	unsigned int dwParam2;
} OPEN_VoiceParamSet;

typedef struct OPEN_SynthParamSetExt
{
	OPEN_HASYNTHPARAMSEXT param;
	int lPARWValue;
} OPEN_SynthParamSet;

__declspec(dllexport) OPENSEGASTATUS SEGAAPI_Play(void* hHandle);
__declspec(dllexport) OPENSEGASTATUS SEGAAPI_Pause(void* hHandle);
__declspec(dllexport) OPENSEGASTATUS SEGAAPI_Stop(void* hHandle);
__declspec(dllexport) OPEN_HAWOSTATUS SEGAAPI_GetPlaybackStatus(void* hHandle);
__declspec(dllexport) OPENSEGASTATUS SEGAAPI_SetFormat(void* hHandle, OPEN_HAWOSEFORMAT* pFormat);
__declspec(dllexport) OPENSEGASTATUS SEGAAPI_GetFormat(void* hHandle, OPEN_HAWOSEFORMAT* pFormat);
__declspec(dllexport) OPENSEGASTATUS SEGAAPI_SetSampleRate(void* hHandle, unsigned int dwSampleRate);
__declspec(dllexport) unsigned int SEGAAPI_GetSampleRate(void* hHandle);
__declspec(dllexport) OPENSEGASTATUS SEGAAPI_SetPriority(void* hHandle, unsigned int dwPriority);
__declspec(dllexport) unsigned int SEGAAPI_GetPriority(void* hHandle);
__declspec(dllexport) OPENSEGASTATUS SEGAAPI_SetUserData(void* hHandle, void* hUserData);
__declspec(dllexport) void* SEGAAPI_GetUserData(void* hHandle);
__declspec(dllexport) OPENSEGASTATUS SEGAAPI_SetSendRouting(void* hHandle, unsigned int dwChannel, unsigned int dwSend,
	OPEN_HAROUTING dwDest);
__declspec(dllexport) OPEN_HAROUTING SEGAAPI_GetSendRouting(void* hHandle, unsigned int dwChannel, unsigned int dwSend);
__declspec(dllexport) OPENSEGASTATUS SEGAAPI_SetSendLevel(void* hHandle, unsigned int dwChannel, unsigned int dwSend,
	unsigned int dwLevel);
__declspec(dllexport) unsigned int SEGAAPI_GetSendLevel(void* hHandle, unsigned int dwChannel, unsigned int dwSend);
__declspec(dllexport) OPENSEGASTATUS SEGAAPI_SetChannelVolume(void* hHandle, unsigned int dwChannel,
	unsigned int dwVolume);
__declspec(dllexport) unsigned int SEGAAPI_GetChannelVolume(void* hHandle, unsigned int dwChannel);
__declspec(dllexport) OPENSEGASTATUS SEGAAPI_SetPlaybackPosition(void* hHandle, unsigned int dwPlaybackPos);
__declspec(dllexport) unsigned int SEGAAPI_GetPlaybackPosition(void* hHandle);
__declspec(dllexport) OPENSEGASTATUS SEGAAPI_SetNotificationFrequency(void* hHandle, unsigned int dwFrameCount);
__declspec(dllexport) OPENSEGASTATUS SEGAAPI_SetNotificationPoint(void* hHandle, unsigned int dwBufferOffset);
__declspec(dllexport) OPENSEGASTATUS SEGAAPI_ClearNotificationPoint(void* hHandle, unsigned int dwBufferOffset);
__declspec(dllexport) OPENSEGASTATUS SEGAAPI_SetStartLoopOffset(void* hHandle, unsigned int dwOffset);
__declspec(dllexport) unsigned int SEGAAPI_GetStartLoopOffset(void* hHandle);
__declspec(dllexport) OPENSEGASTATUS SEGAAPI_SetEndLoopOffset(void* hHandle, unsigned int dwOffset);
__declspec(dllexport) unsigned int SEGAAPI_GetEndLoopOffset(void* hHandle);
__declspec(dllexport) OPENSEGASTATUS SEGAAPI_SetEndOffset(void* hHandle, unsigned int dwOffset);
__declspec(dllexport) unsigned int SEGAAPI_GetEndOffset(void* hHandle);
__declspec(dllexport) OPENSEGASTATUS SEGAAPI_SetLoopState(void* hHandle, int bDoContinuousLooping);
__declspec(dllexport)int SEGAAPI_GetLoopState(void* hHandle);
__declspec(dllexport)OPENSEGASTATUS SEGAAPI_UpdateBuffer(void* hHandle, unsigned int dwStartOffset,
	unsigned int dwLength);
__declspec(dllexport)OPENSEGASTATUS SEGAAPI_SetSynthParam(void* hHandle, OPEN_HASYNTHPARAMSEXT param, int lPARWValue);
__declspec(dllexport) int SEGAAPI_GetSynthParam(void* hHandle, OPEN_HASYNTHPARAMSEXT param);
__declspec(dllexport) OPENSEGASTATUS SEGAAPI_SetSynthParamMultiple(void* hHandle, unsigned int dwNumParams,
	OPEN_SynthParamSet* pSynthParams);
__declspec(dllexport) OPENSEGASTATUS SEGAAPI_GetSynthParamMultiple(void* hHandle, unsigned int dwNumParams,
	OPEN_SynthParamSet* pSynthParams);
__declspec(dllexport) OPENSEGASTATUS SEGAAPI_SetReleaseState(void* hHandle, int bSet);
__declspec(dllexport) OPENSEGASTATUS SEGAAPI_PlayWithSetup(
	void* hHandle,
	unsigned int dwNumSendRouteParams, OPEN_SendRouteParamSet* pSendRouteParams,
	unsigned int dwNumSendLevelParams, OPEN_SendLevelParamSet* pSendLevelParams,
	unsigned int dwNumVoiceParams, OPEN_VoiceParamSet* pVoiceParams,
	unsigned int dwNumSynthParams, OPEN_SynthParamSet* pSynthParams
);
typedef void(*OPEN_HAWOSEGABUFFERCALLBACK)(void* hHandle,
	OPEN_HAWOSMESSAGETYPE message);
__declspec(dllexport) OPENSEGASTATUS SEGAAPI_CreateBuffer(OPEN_HAWOSEBUFFERCONFIG* pConfig,
	OPEN_HAWOSEGABUFFERCALLBACK pCallback,
	unsigned int dwFlags,
	void* * phHandle);
__declspec(dllexport) OPENSEGASTATUS SEGAAPI_DestroyBuffer(void* hHandle);
__declspec(dllexport) int SEGAAPI_SetGlobalEAXProperty(GUID* guid, unsigned long ulProperty, void* pData,
	unsigned long ulDataSize);
__declspec(dllexport) int SEGAAPI_GetGlobalEAXProperty(GUID* guid, unsigned long ulProperty, void* pData,
	unsigned long ulDataSize);
__declspec(dllexport) OPENSEGASTATUS SEGAAPI_SetSPDIFOutChannelStatus(
	unsigned int dwChannelStatus,
	unsigned int dwExtChannelStatus);
__declspec(dllexport) OPENSEGASTATUS SEGAAPI_GetSPDIFOutChannelStatus(
	unsigned int* pdwChannelStatus,
	unsigned int* pdwExtChannelStatus);
__declspec(dllexport) OPENSEGASTATUS SEGAAPI_SetSPDIFOutSampleRate(OPEN_HASPDIFOUTRATE dwSamplingRate);
__declspec(dllexport) OPEN_HASPDIFOUTRATE SEGAAPI_GetSPDIFOutSampleRate(void);
__declspec(dllexport) OPENSEGASTATUS SEGAAPI_SetSPDIFOutChannelRouting(
	unsigned int dwChannel,
	OPEN_HAROUTING dwSource);
__declspec(dllexport) OPEN_HAROUTING SEGAAPI_GetSPDIFOutChannelRouting(unsigned int dwChannel);
__declspec(dllexport) OPENSEGASTATUS SEGAAPI_SetIOVolume(OPEN_HAPHYSICALIO dwPhysIO, unsigned int dwVolume);
__declspec(dllexport) unsigned int SEGAAPI_GetIOVolume(OPEN_HAPHYSICALIO dwPhysIO);
__declspec(dllexport) void SEGAAPI_SetLastStatus(OPENSEGASTATUS LastStatus);
__declspec(dllexport) OPENSEGASTATUS SEGAAPI_GetLastStatus(void);
__declspec(dllexport) OPENSEGASTATUS SEGAAPI_Reset(void);
__declspec(dllexport) OPENSEGASTATUS SEGAAPI_Init(void);
__declspec(dllexport) OPENSEGASTATUS SEGAAPI_Exit(void);
