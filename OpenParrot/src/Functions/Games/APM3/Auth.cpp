#include <Utility/InitFunction.h>
#include <StdInc.h>
#include <thread>
#include "Functions\Global.h"
#include "Auth.h"

wchar_t APM3GameId[10];
wchar_t RegionName[64];
wchar_t LocationNickName[64];
wchar_t LocationName[64];
wchar_t LinkServerName[64];
wchar_t ServerName[64];

//#define _LOGAPM3AUTH 1
DWORD_PTR CALLPLEB AllnetAccounting_beginPlay(unsigned int a1)
{
#ifdef _LOGAPM3AUTH
	info(true, "AllnetAccounting_beginPlay");
#endif
	return AllnetAccounting_beginPlayReturnValue;
}

DWORD_PTR CALLPLEB AllnetAccounting_endPlay(int a1, int a2, int a3)
{
#ifdef _LOGAPM3AUTH
	info(true, "AllnetAccounting_endPlay");
#endif
	return AllnetAccounting_endPlayReturnValue;
}

wchar_t* CALLPLEB AllnetAuth_getAbaasGsServerName()
{
#ifdef _LOGAPM3AUTH
	info(true, "AllnetAuth_getAbaasGsServerName");
#endif
	return ServerName;
}

wchar_t* CALLPLEB AllnetAuth_getAbaasLinkServerName()
{
#ifdef _LOGAPM3AUTH
	info(true, "AllnetAuth_getAbaasLinkServerName");
#endif
	return LinkServerName;
}

char countryCode[4] = "JPN";

char * CALLPLEB AllnetAuth_getCountryCode()
{
#ifdef _LOGAPM3AUTH
	info(true, "AllnetAuth_getCountryCode");
#endif
	return countryCode;
}

unsigned int CALLPLEB AllnetAuth_getLocationId()
{
#ifdef _LOGAPM3AUTH
	info(true, "AllnetAuth_getLocationId");
#endif
	return AllnetAuth_getLocationIdReturnValue;
}

wchar_t* CALLPLEB AllnetAuth_getLocationName()
{
#ifdef _LOGAPM3AUTH
	info(true, "AllnetAuth_getLocationName");
#endif
	return LocationName;
}

wchar_t* CALLPLEB AllnetAuth_getLocationNickname(int index)
{
#ifdef _LOGAPM3AUTH
	info(true, "AllnetAuth_getLocationNickname");
#endif
	return LocationNickName;
}

int CALLPLEB AllnetAuth_getLocationNicknamePartCount()
{
#ifdef _LOGAPM3AUTH
	info(true, "AllnetAuth_getLocationNicknamePartCount");
#endif
	return AllnetAuth_getLocationNicknamePartCountReturnValue;
}

int CALLPLEB AllnetAuth_getRegionCode()
{
#ifdef _LOGAPM3AUTH
	info(true, "AllnetAuth_getRegionCode");
#endif
	return AllnetAuth_getRegionCodeReturnValue;
}

wchar_t* CALLPLEB AllnetAuth_getRegionName(int index)
{
#ifdef _LOGAPM3AUTH
	info(true, "AllnetAuth_getRegionName");
#endif
	return RegionName;
}

int CALLPLEB AllnetAuth_getRegionNamePartCount()
{
#ifdef _LOGAPM3AUTH
	info(true, "AllnetAuth_getRegionNamePartCount");
#endif
	return AllnetAuth_getRegionNamePartCountReturnValue;
}

bool CALLPLEB AllnetAuth_isDevelop()
{
#ifdef _LOGAPM3AUTH
	info(true, "AllnetAuth_isDevelop");
#endif
	return AllnetAuth_isDevelopReturnValue;
}

bool CALLPLEB AllnetAuth_isGood()
{
#ifdef _LOGAPM3AUTH
	info(true, "AllnetAuth_isGood");
#endif
	return true;
}
