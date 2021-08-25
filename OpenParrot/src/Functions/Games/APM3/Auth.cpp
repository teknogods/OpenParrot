#include <Utility/InitFunction.h>
#include <StdInc.h>
#include <thread>
#ifdef _M_AMD64
#include "Functions\Global.h"
#include "Auth.h"

wchar_t APM3GameId[10];
wchar_t RegionName[64];
wchar_t LocationNickName[64];
wchar_t LocationName[64];
wchar_t LinkServerName[64];
wchar_t ServerName[64];


__int64 __fastcall AllnetAccounting_beginPlay(unsigned int a1)
{
#ifdef _LOGAPM3AUTH
	info(true, "AllnetAccounting_beginPlay");
#endif
	return AllnetAccounting_beginPlayReturnValue;
}

__int64 __fastcall AllnetAccounting_endPlay(int a1, int a2, int a3)
{
#ifdef _LOGAPM3AUTH
	info(true, "AllnetAccounting_endPlay");
#endif
	return AllnetAccounting_endPlayReturnValue;
}

wchar_t* AllnetAuth_getAbaasGsServerName()
{
#ifdef _LOGAPM3AUTH
	info(true, "AllnetAuth_getAbaasGsServerName");
#endif
	return ServerName;
}

wchar_t* AllnetAuth_getAbaasLinkServerName()
{
#ifdef _LOGAPM3AUTH
	info(true, "AllnetAuth_getAbaasLinkServerName");
#endif
	return LinkServerName;
}

char countryCode[4] = "JPN";

char *AllnetAuth_getCountryCode()
{
#ifdef _LOGAPM3AUTH
	info(true, "AllnetAuth_getCountryCode");
#endif
	return countryCode;
}

unsigned int AllnetAuth_getLocationId()
{
#ifdef _LOGAPM3AUTH
	info(true, "AllnetAuth_getLocationId");
#endif
	return AllnetAuth_getLocationIdReturnValue;
}

wchar_t* AllnetAuth_getLocationName()
{
#ifdef _LOGAPM3AUTH
	info(true, "AllnetAuth_getLocationName");
#endif
	return LocationName;
}

wchar_t* __fastcall AllnetAuth_getLocationNickname(int index)
{
#ifdef _LOGAPM3AUTH
	info(true, "AllnetAuth_getLocationNickname");
#endif
	return LocationNickName;
}

int AllnetAuth_getLocationNicknamePartCount()
{
#ifdef _LOGAPM3AUTH
	info(true, "AllnetAuth_getLocationNicknamePartCount");
#endif
	return AllnetAuth_getLocationNicknamePartCountReturnValue;
}

int AllnetAuth_getRegionCode()
{
#ifdef _LOGAPM3AUTH
	info(true, "AllnetAuth_getRegionCode");
#endif
	return AllnetAuth_getRegionCodeReturnValue;
}

wchar_t* __fastcall AllnetAuth_getRegionName(int index)
{
#ifdef _LOGAPM3AUTH
	info(true, "AllnetAuth_getRegionName");
#endif
	return RegionName;
}

int AllnetAuth_getRegionNamePartCount()
{
#ifdef _LOGAPM3AUTH
	info(true, "AllnetAuth_getRegionNamePartCount");
#endif
	return AllnetAuth_getRegionNamePartCountReturnValue;
}

bool AllnetAuth_isDevelop()
{
#ifdef _LOGAPM3AUTH
	info(true, "AllnetAuth_isDevelop");
#endif
	return AllnetAuth_isDevelopReturnValue;
}

bool AllnetAuth_isGood()
{
#ifdef _LOGAPM3AUTH
	info(true, "AllnetAuth_isGood");
#endif
	return true;
}
#endif