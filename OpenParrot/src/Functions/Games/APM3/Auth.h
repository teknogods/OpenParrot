#pragma once
#include "APM3.h"

extern wchar_t APM3GameId[10];
extern wchar_t RegionName[64];
extern wchar_t LocationNickName[64];
extern wchar_t LocationName[64];
extern wchar_t LinkServerName[64];
extern wchar_t ServerName[64];

// Make return values global
extern DWORD_PTR AllnetAccounting_beginPlayReturnValue;
extern DWORD_PTR AllnetAccounting_endPlayReturnValue;
extern DWORD_PTR AllnetAuth_getCountryCodeReturnValue;
extern DWORD_PTR AllnetAuth_getLocationIdReturnValue;
extern int AllnetAuth_getLocationNicknamePartCountReturnValue;
extern DWORD_PTR AllnetAuth_getRegionCodeReturnValue;
extern int AllnetAuth_getRegionNamePartCountReturnValue;
extern bool AllnetAuth_isDevelopReturnValue;
extern bool AllnetAuth_isGoodReturnValue;

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