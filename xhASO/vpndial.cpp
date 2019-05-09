#include "stdafx.h"
#include "vpndial.h"
#include <stdio.h>

#include "ras.h"
#include "raserror.h"
#include <iphlpapi.h>
#include <icmpapi.h>

#pragma comment(lib, "iphlpapi.lib")
#pragma comment(lib, "ws2_32.lib")

#pragma comment(lib, "shell32.lib")
#pragma comment(lib,"Rasapi32.lib")

LPWSTR connTitlePPPoE = TEXT("Connect to Internet (PPPoE)");
LPWSTR connTitleL2TP = TEXT("Connect to Internet (L2TP)");
LPWSTR strDeviceName = TEXT("WAN Miniport (PPTP)");
LPWSTR strDeviceType = TEXT("VPN");


typedef UINT (CALLBACK* pNetConnectStatusCallback)(LPVOID lpPtr,CString strContext);

pNetConnectStatusCallback ConnectStatusCallback = NULL;

LPVOID g_pObjPtr = NULL;


void SetObjectValue(LPVOID pCallbackPtr,LPVOID pObjPtr)
{
	ConnectStatusCallback = (pNetConnectStatusCallback)pCallbackPtr;
	g_pObjPtr = pObjPtr;
	return;
}

void OutputString( char *lpFmt, ... )
{
 
    char buff[1024] = "\0";
    va_list    arglist;
    va_start( arglist, lpFmt );
    vsnprintf( buff, sizeof(buff), lpFmt, arglist );
    va_end( arglist );
}

void WINAPI RasDialFunc(UINT unMsg, RASCONNSTATE rasconnstate, DWORD dwError)
{
	if (dwError) 
	{
		return;
	}

	switch (rasconnstate)
    {
        // Running States
        case RASCS_OpenPort:
			ConnectStatusCallback(g_pObjPtr,TEXT("正在打开端口……"));
            break;
        case RASCS_PortOpened:
			ConnectStatusCallback(g_pObjPtr,TEXT("端口已打开……"));
            break;
        case RASCS_ConnectDevice: 
            ConnectStatusCallback(g_pObjPtr,TEXT("连接设备……"));
            break;
        case RASCS_DeviceConnected: 
			ConnectStatusCallback(g_pObjPtr,TEXT("设备已经连接"));
            break;
        case RASCS_AllDevicesConnected:
            ConnectStatusCallback(g_pObjPtr,TEXT("所有设备已经连接"));
            break;
        case RASCS_Authenticate: 
            ConnectStatusCallback(g_pObjPtr,TEXT("……"));
            break;
        case RASCS_AuthNotify:
            ConnectStatusCallback(g_pObjPtr,TEXT("链接权限通知……"));
            break;
        case RASCS_AuthRetry: 
            ConnectStatusCallback(g_pObjPtr,TEXT("权限重试……"));
            break;
        case RASCS_AuthCallback:;
			ConnectStatusCallback(g_pObjPtr,TEXT("权限打开……"));
            break;
        case RASCS_AuthChangePassword: 
			ConnectStatusCallback(g_pObjPtr,TEXT("权限改变密码……"));
            break;
        case RASCS_AuthProject: 
            OutputString ("Projection phase started...\n");
            break;
        case RASCS_AuthLinkSpeed: 
            OutputString ("Negotiating speed...\n");
            break;
        case RASCS_AuthAck: 
            OutputString ("Authentication acknowledge...\n");
            break;
        case RASCS_ReAuthenticate: 
            OutputString ("Retrying Authentication...\n");
            break;
        case RASCS_Authenticated: 
			ConnectStatusCallback(g_pObjPtr,TEXT("权限打开完成"));
            break;
        case RASCS_PrepareForCallback: 
            ConnectStatusCallback(g_pObjPtr,TEXT("准备回调"));
            break;
        case RASCS_WaitForModemReset: 
            ConnectStatusCallback(g_pObjPtr,TEXT("等待……"));
            break;
        case RASCS_WaitForCallback:
            ConnectStatusCallback(g_pObjPtr,TEXT("等待……"));
            break;
        case RASCS_Projected:  
            ConnectStatusCallback(g_pObjPtr,TEXT("项目完成"));
            break;
    #if (WINVER >= 0x400) 
        case RASCS_StartAuthentication:
			ConnectStatusCallback(g_pObjPtr,TEXT("……"));
            break;
        case RASCS_CallbackComplete: 
			ConnectStatusCallback(g_pObjPtr,TEXT("……"));
            break;
        case RASCS_LogonNetwork:
			ConnectStatusCallback(g_pObjPtr,TEXT("登录网络……"));
            break;
    #endif 
        case RASCS_SubEntryConnected:
			ConnectStatusCallback(g_pObjPtr,TEXT("子项目已连接"));
            break;
        case RASCS_SubEntryDisconnected:
			ConnectStatusCallback(g_pObjPtr,TEXT("子项目已断开"));
            break;
        case RASCS_Connected: 
            ConnectStatusCallback(g_pObjPtr,TEXT("已连接"));
            //SetEvent(g_TerminalEvent);
            break;
        case RASCS_Disconnected: 
           ConnectStatusCallback(g_pObjPtr,TEXT("已断开"));
            break;
        default:
            OutputString ("Unknown Status = %d\n", rasconnstate);
            break;
    }
}



BOOL FixIPSec(VOID)
{
	HKEY  hKey             = NULL;
	LONG  lhRegSetValueRet = NULL;
	LONG  lhRegOpenRet     = NULL;
	DWORD keyValue         = 1;
	DWORD dwDisposition    = 0;

	lhRegOpenRet = RegCreateKeyEx(HKEY_LOCAL_MACHINE,
		TEXT("System\\CurrentControlSet\\Services\\Rasman\\Parameters"),
		0, NULL, REG_OPTION_NON_VOLATILE,
		KEY_WRITE, NULL,
		&hKey, &dwDisposition
		);
	if (lhRegOpenRet != ERROR_SUCCESS)
	{
		return FALSE;
	}
	else 
	{
		lhRegSetValueRet = RegSetValueEx(hKey, TEXT("ProhibitIpSec"), 0, REG_DWORD, (const BYTE *)&keyValue, sizeof(DWORD));
		if (lhRegSetValueRet != ERROR_SUCCESS)
		{
			return FALSE;
		}
	}
	RegCloseKey(hKey);

	return TRUE;
}

VOID CreateLnkOnDesktop(const LPWSTR connTitle)
{
	IShellLink   *SLink;
	IPersistFile *PF;
	HRESULT HRes = 0;
	TCHAR desktop_path[MAX_PATH] = TEXT("");
	TCHAR pszFullLnkPath[MAX_PATH]; 

	CoInitialize(NULL);

	ITEMIDLIST* pidl1 = NULL;
    SHGetFolderLocation(NULL, CSIDL_CONNECTIONS, NULL, 0, &pidl1);
    IShellFolder *desktop, *ncfolder;
    SHGetDesktopFolder(&desktop);
    desktop->BindToObject(pidl1, NULL, IID_IShellFolder, (void**)&ncfolder);

    IEnumIDList *items;
    ncfolder->EnumObjects(NULL, SHCONTF_NONFOLDERS, &items);
    ITEMIDLIST* pidl2 = NULL;
    while (S_OK == items->Next(1, &pidl2, NULL))
    {
        STRRET sr = {STRRET_WSTR};
        ncfolder->GetDisplayNameOf(pidl2, SHGDN_NORMAL, &sr);

        TCHAR buf[MAX_PATH] = TEXT("");
        StrRetToBuf(&sr, pidl2, buf, MAX_PATH);

        if (0 == StrCmpI(buf, connTitle))
        {
            ITEMIDLIST* pidl3 = ILCombine(pidl1, pidl2);
			HRESULT HRes = CoCreateInstance(CLSID_ShellLink, 0, CLSCTX_INPROC_SERVER, IID_IShellLink, ( LPVOID*)&SLink);
            SLink->SetIDList(pidl3);
			SHGetFolderPath(NULL, CSIDL_DESKTOP, NULL, 0, desktop_path);
			StringCbPrintf(pszFullLnkPath, MAX_PATH * sizeof(TCHAR), TEXT("%s\\%s.lnk"), desktop_path, connTitle);
			HRes = SLink->QueryInterface(IID_IPersistFile, (LPVOID*)&PF);
			HRes = PF->Save((LPCOLESTR)pszFullLnkPath, TRUE);
			PF->Release();
			SLink->Release();
            ILFree(pidl3);
            ILFree(pidl2);
            break;
        }

        ILFree(pidl2);
        pidl2 = NULL;
    }
	ncfolder->Release();
	desktop->Release();

    ILFree(pidl1);

	CoUninitialize();
}

BOOL IsUserAdmin(VOID)
/*++ 
Routine Description: This routine returns TRUE if the caller's
process is a member of the Administrators local group. Caller is NOT
expected to be impersonating anyone and is expected to be able to
open its own process and process token. 
Arguments: None. 
Return Value: 
   TRUE - Caller has Administrators local group. 
   FALSE - Caller does not have Administrators local group. --
*/
{
	BOOL b;
	SID_IDENTIFIER_AUTHORITY NtAuthority = SECURITY_NT_AUTHORITY;
	PSID AdministratorsGroup; 
	b = AllocateAndInitializeSid
		(
		&NtAuthority,
		2,
		SECURITY_BUILTIN_DOMAIN_RID,
		DOMAIN_ALIAS_RID_ADMINS,
		0, 0, 0, 0, 0, 0,
		&AdministratorsGroup); 
	if(b) 
	{
		if (!CheckTokenMembership( NULL, AdministratorsGroup, &b)) 
		{
			 b = FALSE;
		} 

		FreeSid(AdministratorsGroup); 
	}
	return(b);
}

BOOL CreateConnection(LPTSTR lpszEnterNameText,LPTSTR lpszLoginText,LPTSTR lpszPasswordText,int typeConnection,LPTSTR lpszServerIP,BOOL bSaveCredentials,BOOL bCreateLnk)
{
	const size_t NeedRebootMsgChars = 64;
	LPWSTR		connTitle = NULL;
	bool		doDisableIPSec = false;
	int			rasentry_struct_size = sizeof(RASENTRY);
	DWORD		dwDeviceInfoSize = 0;
	DWORD		dwIPLen = 0;

	if (typeConnection == 0) 
		{ // L2TP?
			if (IsUserAdmin()) 
			{ 
				doDisableIPSec = true;
			}
			else 
			{
				MessageBox(NULL, TEXT("Need admin to run this application."), TEXT("Notice"), MB_OK);
				return FALSE;
			}
		}

	DWORD dwRasEntryRet = RasGetEntryProperties(NULL, NULL, NULL, (LPDWORD)&rasentry_struct_size, NULL, &dwDeviceInfoSize);
	if (dwRasEntryRet == ERROR_RASMAN_CANNOT_INITIALIZE) 
	{
		return FALSE;
	}



	//填写连接设置结构
	RASENTRY rasEntry;
	ZeroMemory(&rasEntry, sizeof(RASENTRY));
	rasEntry.dwSize = rasentry_struct_size;
	rasEntry.dwfOptions = RASEO_RemoteDefaultGateway | RASEO_ModemLights |
			RASEO_SecureLocalFiles | RASEO_RequireMsEncryptedPw | RASEO_RequireDataEncryption |
			RASEO_RequireMsCHAP2 | RASEO_ShowDialingProgress;
	
	rasEntry.dwfOptions = rasEntry.dwfOptions | RASEO_PreviewUserPw;
	
	rasEntry.dwfOptions2 = RASEO2_DisableNbtOverIP | RASEO2_ReconnectIfDropped | RASEO2_Internet |
			RASEO2_DontNegotiateMultilink | RASEO2_SecureClientForMSNet | RASEO2_SecureFileAndPrint;
	rasEntry.dwRedialCount = 3;
	rasEntry.dwRedialPause = 60;
	rasEntry.dwFramingProtocol = RASFP_Ppp;
	rasEntry.dwfNetProtocols = RASNP_Ip;
	rasEntry.dwEncryptionType = ET_Optional; // ET_Require

	//rasEntry.dwDialMode = 1;

	dwIPLen = wcslen(lpszServerIP);

	switch (typeConnection)
	{
	case 1: //PPPoE
		connTitle = lpszEnterNameText;
		_tcscpy_s(rasEntry.szLocalPhoneNumber,RAS_MaxPhoneNumber + 1,lpszServerIP);
		wcscpy_s(rasEntry.szDeviceType, RAS_MaxDeviceType + 1, RASDT_PPPoE);
		rasEntry.dwVpnStrategy = VS_Default;
		rasEntry.dwType = RASET_Broadband;
		break;
	case 2: // PPTP
		connTitle = lpszEnterNameText;
		wcscpy_s(rasEntry.szLocalPhoneNumber,RAS_MaxPhoneNumber + 1,lpszServerIP);
		wcscpy_s(rasEntry.szDeviceType, RAS_MaxDeviceType + 1, RASDT_Vpn);
		rasEntry.dwVpnStrategy = VS_PptpOnly;
		rasEntry.dwType = RASET_Vpn;

		//wcscpy_s(rasEntry.szDeviceName,RAS_MaxDeviceName + 1,strDeviceName);
		//wcscpy_s(rasEntry.szDeviceType,RAS_MaxDeviceType + 1,strDeviceType);
		break;
	case 0: // L2TP
	default:
		connTitle = lpszEnterNameText;
		_tcscpy_s(rasEntry.szLocalPhoneNumber,RAS_MaxPhoneNumber + 1,lpszServerIP);
		wcscpy_s(rasEntry.szDeviceType, RAS_MaxDeviceType + 1, RASDT_Vpn);
		rasEntry.dwVpnStrategy = VS_L2tpOnly;
		rasEntry.dwType = RASET_Vpn;
		break;
	}


	//RASDIALPARAMS ras_param = {0};
	RASCREDENTIALS ras_cred = {0};

	//创建连接
	dwRasEntryRet = RasSetEntryProperties(NULL, connTitle, &rasEntry, rasentry_struct_size, NULL, 0);
	switch (dwRasEntryRet)
	{
	case ERROR_ACCESS_DENIED:
		MessageBox(NULL, TEXT("Can not create a connection\nRasSetEntryProperties() - ERROR_ACCESS_DENIED"), TEXT("error"), MB_OK);
		break;
	case ERROR_BUFFER_INVALID:
		MessageBox(NULL, TEXT("Can not create a connection\nRasSetEntryProperties() - ERROR_BUFFER_INVALID"), TEXT("error"), MB_OK);
		break;
	case ERROR_CANNOT_OPEN_PHONEBOOK:
		MessageBox(NULL, TEXT("Can not create a connection\nRasSetEntryProperties() - ERROR_CANNOT_OPEN_PHONEBOOK"), TEXT("error"), MB_OK);
		break;
	case ERROR_INVALID_PARAMETER:
		MessageBox(NULL, TEXT("Can not create a connection\nRasSetEntryProperties() - ERROR_INVALID_PARAMETER"), TEXT("error"), MB_OK);
		break;
	case ERROR_SUCCESS:
		{
			// 管理员登录名和密码连接
			ZeroMemory(&ras_cred, sizeof(RASCREDENTIALS));
			ras_cred.dwSize = sizeof(RASCREDENTIALS);
			ras_cred.dwMask = RASCM_UserName | RASCM_Password; // | RASCM_DefaultCreds;
			if (bSaveCredentials) 
			{
				// 保存登录名和密码
				wcscpy_s(ras_cred.szUserName, 256, lpszLoginText);
				wcscpy_s(ras_cred.szPassword, 256, lpszPasswordText);
				DWORD dwRasCredRet = RasSetCredentials(NULL, connTitle, &ras_cred, FALSE);
				switch (dwRasCredRet) 
				{
				case ERROR_CANNOT_OPEN_PHONEBOOK:
					MessageBox(NULL, TEXT("Can not save your login name and password\nRasSetCredentials() - ERROR_CANNOT_OPEN_PHONEBOOK"), TEXT("error"), MB_OK);
					break;
				case ERROR_CANNOT_FIND_PHONEBOOK_ENTRY:
					MessageBox(NULL, TEXT("Can not save your login name and password\nRasSetCredentials() - ERROR_CANNOT_FIND_PHONEBOOK_ENTRY"), TEXT("error"), MB_OK);
					break;
				case ERROR_INVALID_PARAMETER:
					MessageBox(NULL, TEXT("Can not save your login name and password\nRasSetCredentials() - ERROR_INVALID_PARAMETER"), TEXT("error"), MB_OK);
					break;
				case ERROR_INVALID_SIZE:
					MessageBox(NULL, TEXT("Can not save your login name and password\nRasSetCredentials() - ERROR_INVALID_SIZE"), TEXT("error"), MB_OK);
					break;
				case ERROR_ACCESS_DENIED:
					MessageBox(NULL, TEXT("Can not save your login name and password\nRasSetCredentials() - ERROR_ACCESS_DENIED"), TEXT("error"), MB_OK);
					break;
				case ERROR_SUCCESS:
				default:
					break;
				}
			}
			else 
			{
				// 清除登录名和密码
				RasSetCredentials(NULL, connTitle, &ras_cred, TRUE);
			}
		
			/*//xp之后的系统不使用
			ZeroMemory(&ras_param, sizeof(RASDIALPARAMS));
			ras_param.dwSize = sizeof(RASDIALPARAMS);
			wcscpy_s(ras_param.szEntryName, RAS_MaxEntryName + 1, connTitle);
			wcscpy_s(ras_param.szUserName, UNLEN + 1, lpszLoginText);
			wcscpy_s(ras_param.szPassword, PWLEN + 1, lpszPasswordText);
			DWORD dwRasEntryParamsRet = RasSetEntryDialParams(0, &ras_param, FALSE);
			switch (dwRasEntryParamsRet)
			{
			case ERROR_BUFFER_INVALID:
				MessageBox(NULL, TEXT("Can not request login name and password\nRasSetEntryDialParams() - ERROR_BUFFER_INVALID"), TEXT("error"), MB_OK);
				break;
			case ERROR_CANNOT_OPEN_PHONEBOOK:
				MessageBox(NULL, TEXT("Can not request login name and password\nRasSetEntryDialParams() - ERROR_CANNOT_OPEN_PHONEBOOK"), TEXT("error"), MB_OK);
				break;
			case ERROR_CANNOT_FIND_PHONEBOOK_ENTRY:
				MessageBox(NULL, TEXT("Can not request login name and password\nRasSetEntryDialParams() - ERROR_CANNOT_FIND_PHONEBOOK_ENTRY"), TEXT("error"), MB_OK);
				break;
			case ERROR_SUCCESS:
			default:
				break;
			}*/

			TCHAR strNeedRebootStr[NeedRebootMsgChars];
			ZeroMemory(strNeedRebootStr, NeedRebootMsgChars * sizeof(TCHAR));

			if (doDisableIPSec) 
			{
				FixIPSec();
				StringCbPrintf(strNeedRebootStr, NeedRebootMsgChars * sizeof(TCHAR), TEXT("\nNeed to Restart your computer."));
			}

			//桌面快捷方式
			/*if (bCreateLnk)
			{
				CreateLnkOnDesktop(connTitle);
			}*/
		}
		break;
	default:
		int const arraysize = 254;
		TCHAR lptstrStatupOSPaert[arraysize];
		StringCbPrintf(lptstrStatupOSPaert, arraysize * sizeof(TCHAR), TEXT("Can not create a connection\nerror code: %d"), dwRasEntryRet);
		MessageBox(NULL, lptstrStatupOSPaert, TEXT("error"), MB_OK);
		break;
	}

	return TRUE;
}

BOOL connectDialVPN(HRASCONN& hRasConn,LPTSTR lpszEnterNameText,LPTSTR lpszLoginText,LPTSTR lpszPasswordText,LPTSTR lpszServerIP)
{
	//检验名字是否有效
	if (RasValidateEntryName(NULL,lpszEnterNameText) != ERROR_ALREADY_EXISTS)
	{
		return FALSE;
	}


	//开始拨号连接VPN
	OSVERSIONINFOEX osvi;
    ZeroMemory(&osvi, sizeof(osvi));
	osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);

    GetVersionEx((OSVERSIONINFO*)&osvi);

	RASDIALPARAMSW theRasDialParams; 
	ZeroMemory(&theRasDialParams, sizeof(theRasDialParams));

	BOOL bPassword;
    RasGetEntryDialParams(NULL, &theRasDialParams,&bPassword);

    wcscpy_s(theRasDialParams.szUserName,UNLEN + 1,lpszLoginText);  
    wcscpy_s(theRasDialParams.szPassword,PWLEN + 1,lpszPasswordText);  
	wcscpy_s(theRasDialParams.szPhoneNumber,RAS_MaxPhoneNumber + 1,lpszServerIP);
    wcscpy_s(theRasDialParams.szEntryName,RAS_MaxEntryName + 1,lpszEnterNameText);  
	wcscpy_s(theRasDialParams.szDomain,DNLEN + 1,TEXT(""));
	wcscpy_s(theRasDialParams.szCallbackNumber,TEXT(""));

#if (WINVER >= 0x401)// windows 95,NT and later
    if ((osvi.dwMajorVersion < 4) ||
		((osvi.dwMajorVersion == 4) && (osvi.dwMinorVersion < 1)) )
    {
        theRasDialParams.dwSize = offsetof(RASDIALPARAMSW, dwSubEntry);
    }
    else
#endif
/*#if (WINVER >= 0x601)
    if ((osvi.dwMajorVersion < 6) ||
       ((osvi.dwMajorVersion == 6) && (osvi.dwMinVersion < 1)) )
    {
        ras_param.dwSize = offsetof(RASDIALPARAMSW, dwIfIndex);
    }
    else*/
#if (WINVER >= 0x601)// Windows 7 and later
    if (((osvi.dwMajorVersion >= 6) && (osvi.dwMinorVersion >= 1)) )
    {
        theRasDialParams.dwSize = offsetof(RASDIALPARAMSW, dwIfIndex);
    }
    else
#endif
    {
        theRasDialParams.dwSize = sizeof(theRasDialParams);
    }

	hRasConn = NULL;

    DWORD dwRasDialyRet = RasDial(NULL,NULL,&theRasDialParams,0,NULL/*&RasDialFunc*/,&hRasConn);  
    if (ERROR_SUCCESS != dwRasDialyRet)  
    {  
        return FALSE;  
    }  

	return TRUE;
}


BOOL DisconnecDialtVPN(HRASCONN hRasConn)
{
	LONG lRet = ERROR_SUCCESS;

	DWORD dwRet = RasHangUpW(hRasConn);
	if (dwRet != ERROR_SUCCESS)
	{
		return FALSE;
	}

	return TRUE;
}

BOOL checkConnectStatus(HRASCONN hRasConn)
{
	RASCONNSTATUS rasConStatus = {0};
	rasConStatus.dwSize = sizeof(RASCONNSTATUS);

	DWORD dwRet = RasGetConnectStatus(hRasConn,&rasConStatus);
	if (dwRet != 0)
	{
		return FALSE;
	}

	if (rasConStatus.rasconnstate != RASCS_Connected)
	{
		return FALSE;
	}
	return TRUE;
}

BOOL DeleteVpnEntry(LPTSTR lpszEnterNameText)
{
	LONG lRet = ERROR_SUCCESS;

	DWORD dwRet = RasDeleteEntryW(0, lpszEnterNameText);
	if (dwRet != ERROR_SUCCESS)
	{
		return FALSE;
	}

	return TRUE;
}

BOOL getDynamicIPAddress(TCHAR* lpIPAddress)
{
	BOOL			bResult = FALSE;
	DWORD			dwCb = 0;
    DWORD			dwRet = ERROR_SUCCESS;
    DWORD			dwConnections = 0;
    LPRASCONN		lpRasConn = NULL;
	HRASCONN		hrasconn = NULL;
	RASCONNSTATUS	rasStatus = {0};
	RASPPPIP        rip = {0};


	// Call RasEnumConnections with lpRasConn = NULL. dwCb is returned with the required buffer size and 
    // a return code of ERROR_BUFFER_TOO_SMALL
	dwRet = RasEnumConnections(lpRasConn, &dwCb, &dwConnections);
	if (dwRet == ERROR_BUFFER_TOO_SMALL)
	{
		// Allocate the memory needed for the array of RAS structure(s).
		lpRasConn = (LPRASCONN) HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, dwCb);
        if (lpRasConn == NULL)
		{
            return bResult;
        }
		// The first RASCONN structure in the array must contain the RASCONN structure size
		lpRasConn[0].dwSize = sizeof(RASCONN);
		// Call RasEnumConnections to enumerate active connections
		dwRet = RasEnumConnections(lpRasConn, &dwCb, &dwConnections);

		 // If successful,get active connection handle
		if (ERROR_SUCCESS == dwRet)
		{
			for (DWORD i=0; i<dwConnections; i++)
			{
				// get to HRASCONN
				hrasconn = lpRasConn[i].hrasconn;
				// get connection status
				rasStatus.dwSize = sizeof(RASCONNSTATUS);
				dwRet = RasGetConnectStatus( hrasconn, &rasStatus );
				if ( 0==dwRet )
				{
					if (rasStatus.rasconnstate == RASCS_Connected)
					{
						// 取动态分配的IP 地址
						rip.dwSize=sizeof(RASPPPIP);
						dwRet = RasGetProjectionInfo(hrasconn,RASP_PppIp,(LPVOID)&rip,(LPDWORD)&dwCb);
						if (ERROR_SUCCESS==dwRet)
						{
							OutputDebugString(rip.szIpAddress);
							_tcscpy(lpIPAddress,rip.szIpAddress);
							bResult = TRUE;
						}
						else
						{
							bResult = FALSE;
						}

						break;
					}

				}

			}
		}

		//Deallocate memory for the connection buffer
        HeapFree(GetProcessHeap(), 0, lpRasConn);
        lpRasConn = NULL;

	}

	return bResult;
}

BOOL detectInternetConnect(char* pIPAddress)
{
	BOOL		bResult = FALSE; 
	char        requestData[32] = "Data Buffer";
    LPVOID		pReplyBuffer = NULL;
    DWORD		dwRetVal = 0;
    HANDLE		hIcmpFile = NULL;
    if ((hIcmpFile = IcmpCreateFile()) == INVALID_HANDLE_VALUE)
	{
		return bResult;
	}

    pReplyBuffer = (LPVOID) malloc(sizeof(ICMP_ECHO_REPLY) + sizeof(requestData));
	if (!pReplyBuffer)
	{
		return bResult;
	}

    dwRetVal = IcmpSendEcho(hIcmpFile,
							inet_addr(pIPAddress),
							requestData,
							sizeof(requestData), 
							NULL,
							pReplyBuffer,
							sizeof(pReplyBuffer) + sizeof(ICMP_ECHO_REPLY),
							1000);
	
	if (dwRetVal != 0)
	{
        bResult = TRUE;
    }

	if (pReplyBuffer)
	{
		free(pReplyBuffer);
	}

    return bResult;

}
