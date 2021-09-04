#pragma once

#ifdef _M_IX86
#define CALLPLEB __stdcall
#else
#define CALLPLEB __fastcall
#endif

//#define _LOGAPM3 1
//#define _LOGAPM3AIME 1
//#define _LOGAPM3AUTH 1
//#define _LOGAPM3BACKUP 1
//#define _LOGAPM3SEQUENCE 1