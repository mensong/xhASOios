#ifndef __VPN_DIAL_H
#define __VPN_DIAL_H

#include <Ras.h>
#include <raserror.h>
#include <ShlObj.h>
#include <Shlwapi.h>
#include <strsafe.h>

void SetObjectValue(LPVOID pCallbackPtr,LPVOID pObjPtr);

BOOL FixIPSec(VOID);
VOID CreateLnkOnDesktop(const LPWSTR connTitle);
BOOL IsUserAdmin(VOID);
BOOL CreateConnection(LPTSTR lpszEnterNameText,
					  LPTSTR lpszLoginText,
					  LPTSTR lpszPasswordText,
					  int typeConnection,
					  LPTSTR lpszServerIP,
					  BOOL bSaveCredentials,
					  BOOL bCreateLnk);

BOOL connectDialVPN(HRASCONN& hRasConn,
					LPTSTR lpszEnterNameText,
					LPTSTR lpszLoginText,
					LPTSTR lpszPasswordText,
					LPTSTR lpszServerIP);

BOOL DisconnecDialtVPN(HRASCONN hRasConn);

BOOL checkConnectStatus(HRASCONN hRasConn);

BOOL DeleteVpnEntry(LPTSTR lpszEnterNameText);

BOOL getDynamicIPAddress(TCHAR* lpIPAddress);

BOOL detectInternetConnect(char* pIPAddress);

#endif
