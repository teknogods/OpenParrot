#pragma once
#ifdef _M_IX86
#define CALLPLEB __stdcall
#else
#define CALLPLEB __fastcall
#endif

extern wchar_t APM3GameId[10];
extern wchar_t RegionName[64];
extern wchar_t LocationNickName[64];
extern wchar_t LocationName[64];
extern wchar_t LinkServerName[64];
extern wchar_t ServerName[64];

static DWORD_PTR AllnetAccounting_beginPlayReturnValue = 0;

static DWORD_PTR AllnetAccounting_endPlayReturnValue = 0;

static DWORD_PTR AllnetAuth_getCountryCodeReturnValue = 0;

static DWORD_PTR AllnetAuth_getLocationIdReturnValue = 0;

static int AllnetAuth_getLocationNicknamePartCountReturnValue = 0;

static DWORD_PTR AllnetAuth_getRegionCodeReturnValue = 0;

static int AllnetAuth_getRegionNamePartCountReturnValue = 0;

static bool AllnetAuth_isDevelopReturnValue = false;

static bool AllnetAuth_isGoodReturnValue = false;

bool CALLPLEB AllnetAuth_isGood();
bool CALLPLEB AllnetAuth_isDevelop();
int CALLPLEB AllnetAuth_getRegionNamePartCount();
wchar_t* CALLPLEB AllnetAuth_getRegionName(int index);
int CALLPLEB AllnetAuth_getRegionCode();
int CALLPLEB AllnetAuth_getLocationNicknamePartCount();
wchar_t* CALLPLEB AllnetAuth_getLocationNickname(int index);
wchar_t* CALLPLEB AllnetAuth_getLocationName();
unsigned int CALLPLEB AllnetAuth_getLocationId();
char * CALLPLEB AllnetAuth_getCountryCode();
wchar_t* CALLPLEB AllnetAuth_getAbaasLinkServerName();
wchar_t* CALLPLEB AllnetAuth_getAbaasGsServerName();
DWORD_PTR CALLPLEB AllnetAccounting_endPlay(int a1, int a2, int a3);
DWORD_PTR CALLPLEB AllnetAccounting_beginPlay(unsigned int a1);