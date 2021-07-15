#ifdef _M_AMD64
extern wchar_t APM3GameId[10];
extern wchar_t RegionName[64];
extern wchar_t LocationNickName[64];
extern wchar_t LocationName[64];
extern wchar_t LinkServerName[64];
extern wchar_t ServerName[64];

static __int64 AllnetAccounting_beginPlayReturnValue = 0;

static __int64 AllnetAccounting_endPlayReturnValue = 0;

static __int64 AllnetAuth_getCountryCodeReturnValue = 0;

static __int64 AllnetAuth_getLocationIdReturnValue = 0;

static int AllnetAuth_getLocationNicknamePartCountReturnValue = 0;

static __int64 AllnetAuth_getRegionCodeReturnValue = 0;

static int AllnetAuth_getRegionNamePartCountReturnValue = 0;

static bool AllnetAuth_isDevelopReturnValue = false;

static bool AllnetAuth_isGoodReturnValue = false;

bool AllnetAuth_isGood();
bool AllnetAuth_isDevelop();
int AllnetAuth_getRegionNamePartCount();
wchar_t* __fastcall AllnetAuth_getRegionName(int index);
int AllnetAuth_getRegionCode();
int AllnetAuth_getLocationNicknamePartCount();
wchar_t* __fastcall AllnetAuth_getLocationNickname(int index);
wchar_t* AllnetAuth_getLocationName();
unsigned int AllnetAuth_getLocationId();
int AllnetAuth_getCountryCode();
wchar_t* AllnetAuth_getAbaasLinkServerName();
wchar_t* AllnetAuth_getAbaasGsServerName();
__int64 __fastcall AllnetAccounting_endPlay(int a1, int a2, int a3);
__int64 __fastcall AllnetAccounting_beginPlay(unsigned int a1);


#endif