// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "targetver.h"







#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files:
#include <windows.h>
#include <atlbase.h>
#include <atlstr.h>
#include <Wininet.h>
#pragma comment(lib,"Wininet.lib")

#include <Psapi.h>
#pragma comment(lib,"psapi.lib")
#pragma comment(lib,"ws2_32.lib")
#pragma comment(lib, "gdiplus.lib")
#pragma comment(lib,"ZLIB.LIB")

// TODO: reference additional headers your program requires here
