
// xhASO.h : main header file for the PROJECT_NAME application
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols

#define WM_LOADAPPLEID_MSG (WM_USER + 104)
#define WM_TERMERATE_THREAD_MSG (WM_USER + 105)
#define WM_LOAD_COMMENT_CONTENT_MSG (WM_USER + 106)
#define WM_LOAD_VPNIP_LIST_MSG (WM_USER + 107)
#define WM_LOGINVPN_MSG (WM_USER + 108)
#define WM_DIALUPLOGIN_MSG (WM_USER + 109)
#define	WM_EXCEPTION_TASK_PROCESS_MSG (WM_USER+110)

#define  MAINTITLE TEXT("苹果商店APP优化工具")
// CxhASOApp:
// See xhASO.cpp for the implementation of this class
//

class CxhASOApp : public CWinAppEx
{
public:
	CxhASOApp();

// Overrides
	public:
	virtual BOOL InitInstance();

// Implementation

	DECLARE_MESSAGE_MAP()
};

extern CxhASOApp theApp;