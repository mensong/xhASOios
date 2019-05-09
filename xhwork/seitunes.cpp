#include "stdafx.h"
#include "seitunes.h"
#include <iostream>
#include <vector>
#include <string>
#include <strsafe.h>
#include <time.h>
#include "viewpage.h"

using namespace std;

#define      USER_AGENT   TEXT("AppStore/2.0 iOS/10.1.1 model/iPhone9,1 hwp/s5l9950x build/14B100 (6; dt:97)")//TEXT("AppStore/2.0 iOS/10.0.2 model/iPhone7,1 hwp/s5l8950x build/13F69 (6; dt:82)")

typedef BOOL (*PGENERATEXJSSPTOKENVALUE)(LPVOID lpGsaServices,LPVOID lpSrcData,int dataLen,LPVOID lpOutData);

PGENERATEXJSSPTOKENVALUE pGenXJSSPTOKENValue = NULL;

BOOL clientapiLookup(CString strUserAgent,char* lpSendContext,char* lpDsid,char* lpToken,char* lpCookieData,TCHAR* lpProxy,int netType)
{
	URL_COMPONENTS uc;
	TCHAR scheme[MAX_PATH] = {0};
	TCHAR hostName[MAX_PATH] = {0};
	TCHAR userName[MAX_PATH] = {0};
	TCHAR password[MAX_PATH] = {0};
	TCHAR urlPath[MAX_PATH] = {0};
	TCHAR ExtraInfo[MAX_PATH] = {0};
	INTERNET_PORT nPort = 0;

	HINTERNET hSession = NULL;
	HINTERNET hConnect = NULL;
	HINTERNET hOpenReq = NULL;
	BOOL bAddRequestHeader = FALSE;
	BOOL bSendRequest = FALSE;
	BOOL bQueryInfo = FALSE;
	
	::ZeroMemory(&uc,sizeof(URL_COMPONENTS));
	uc.dwStructSize = sizeof(URL_COMPONENTS);
	uc.lpszScheme = scheme;
	uc.lpszHostName = hostName;
	uc.lpszUserName = userName;
	uc.lpszPassword = password;
	uc.lpszUrlPath = urlPath;
	uc.lpszExtraInfo = ExtraInfo;
	uc.nPort = nPort;

	uc.dwSchemeLength = MAX_PATH;
	uc.dwHostNameLength = MAX_PATH;
	uc.dwUserNameLength = MAX_PATH;
	uc.dwPasswordLength = MAX_PATH;
	uc.dwUrlPathLength = MAX_PATH;

	TCHAR strHostName[4096] = {0};
	TCHAR strPasswordToken[128] = {0};
	TCHAR strDsid[128] = {0};
	TCHAR strSendContext[512] = {0};
	TCHAR strCookieData[1024] = _T("X-JS-TIMESTAMP=");
	TCHAR strPartCookieData[512] = {0};
	TCHAR strTimestamp[128] = {0};
	TCHAR strToken[MAX_PATH] = {0};


	char szTimestamp[64] = {0};
	char szParamContext[1024] = {0};
	char szBase64[MAX_PATH] = {0};

	HMODULE hModule = ::GetModuleHandle(_T("xhiTunes.dll"));
	if (!hModule)
	{
		return FALSE;
	}

	PGENERATEXJSSPTOKENVALUE pGenXJSSPTOKENValue = (PGENERATEXJSSPTOKENVALUE)GetProcAddress(hModule,"generateX_JS_SP_TOKEN_Value"); 
	if (!pGenXJSSPTOKENValue)
	{
		return FALSE;
	}

	time_t unixTime;
	SYSTEMTIME sysTime;
	//获取时间戳
	GetLocalTime(&sysTime);
	time(&unixTime);
	sprintf(szTimestamp,"%ld",unixTime);


	sprintf(szParamContext,"%s143465-19,29P6%snative-search-lockup","1497371628","670878422,935576870,1139424430,1151263022,1161402508,1189734118,1217334307");//szTimestamp,lpSendContext);
	//生成x-js-sp-token
	BOOL bToken = pGenXJSSPTOKENValue(NULL,szParamContext,strlen(szParamContext),szBase64);

	CString strUrl = _T("https://client-api.itunes.apple.com");
	CString strAgent = strUserAgent;
	TCHAR szBuffer[] = _T("/WebObjects/MZStorePlatform.woa/wa/lookup?caller=P6&id=%s&p=native-search-lockup&artwork=search_iphone&version=1");

	MultiByteToWideChar(CP_UTF8,0,(LPCSTR)lpSendContext,strlen(lpSendContext),strSendContext,512);
	MultiByteToWideChar(CP_ACP,0,(LPCSTR)lpToken,strlen(lpToken),strPasswordToken,MAX_PATH);
	MultiByteToWideChar(CP_ACP,0,(LPCSTR)lpDsid,strlen(lpDsid),strDsid,128);
	MultiByteToWideChar(CP_ACP,0,(LPCSTR)lpCookieData,strlen(lpCookieData),strPartCookieData,512);
	MultiByteToWideChar(CP_ACP,0,szTimestamp,strlen(szTimestamp),strTimestamp,128);
	MultiByteToWideChar(CP_ACP,0,szBase64,strlen(szBase64),strToken,MAX_PATH);

	_stprintf(strHostName,szBuffer,strSendContext);

	wcscat(strCookieData,strTimestamp);
	wcscat(strCookieData,_T("; X-JS-SP-TOKEN="));
	wcscat(strCookieData,strToken);

	wcscat(strCookieData,_T("; "));
	wcscat(strCookieData,strPartCookieData);

	BOOL bCrackUrl = ::InternetCrackUrl(strUrl,0,0,&uc);
	if (bCrackUrl)
	{
		if (netType == PROXY_NETWORK)
		{
			hSession = InternetOpen(strAgent,INTERNET_OPEN_TYPE_PROXY,lpProxy,0,0);
		}
		else
		{
			hSession = InternetOpen(strAgent,0,0,0,0);
		}
		if (!hSession)
		{
			return FALSE;
		}

		hConnect = InternetConnect(hSession,uc.lpszHostName,uc.nPort,uc.lpszUserName,uc.lpszPassword,3,0,0);
		if (!hConnect)
		{
			InternetCloseHandle(hSession);
			return FALSE;
		}

		hOpenReq = HttpOpenRequest(hConnect,_T("GET"),strHostName,0,0,0,0x84801000|INTERNET_FLAG_NO_COOKIES,0);//0x84A01000是一个常数,注意
		if (!hOpenReq)
		{
			InternetCloseHandle(hConnect);
			InternetCloseHandle(hSession);
			return FALSE;
		}

		//获取时间戳

		CString strHeaders = _T("User-Agent: ");
		strHeaders += strAgent;
		strHeaders += _T("\r\nAccept-Encoding: gzip, deflate");
		strHeaders += _T("\r\nX-Apple-Store-Front: 143465-19,29");
		strHeaders += _T("\r\nAccept: */*");
		strHeaders += _T("\r\nX-Apple-Client-Versions: GameCenter/2.0");
		strHeaders += _T("\r\nAccept-Language: zh-cn");
		strHeaders += _T("\r\nX-Dsid: ");
		strHeaders += strDsid;
		strHeaders += _T("\r\nCookie: ");
		strHeaders += strCookieData;
		strHeaders += _T("\r\nConnection: keep-alive");
		strHeaders += _T("\r\nProxy-Connection: keep-alive\r\n\r\n");

		DWORD dwHeaderLength = wcslen(strHeaders);

		bAddRequestHeader = HttpAddRequestHeaders(hOpenReq,strHeaders,dwHeaderLength,0xA0000000);//0xA0000000是一个常量
		if (!bAddRequestHeader)
		{
			InternetCloseHandle(hOpenReq);
			InternetCloseHandle(hConnect);
			InternetCloseHandle(hSession);
			return FALSE;
		}

		bSendRequest = HttpSendRequestW(hOpenReq,NULL,-1,NULL,0);//第三个参数设置为负一
		if (!bSendRequest && GetLastError() == ERROR_INTERNET_INVALID_CA)
		{
			DWORD dwFlags;
			DWORD dwBuffLen = sizeof(dwFlags);
			InternetQueryOption (hOpenReq, INTERNET_OPTION_SECURITY_FLAGS,(LPVOID)&dwFlags, &dwBuffLen);
			dwFlags |= SECURITY_FLAG_IGNORE_UNKNOWN_CA;
			InternetSetOption (hOpenReq,INTERNET_OPTION_SECURITY_FLAGS,&dwFlags,sizeof (dwFlags) );

			//再次发送
			bSendRequest = HttpSendRequest(hOpenReq,NULL,-1,NULL,0);
			if (!bSendRequest)
			{
				int error = GetLastError();//ERROR_INTERNET_SEC_CERT_REV_FAILED 
				InternetCloseHandle(hOpenReq);
				InternetCloseHandle(hConnect);
				InternetCloseHandle(hSession);
				return FALSE;
			}
		}

		DWORD dwQueryBuf = 200;
		DWORD dwQueryBufferLength = 4;

		BOOL bQueryInfo = HttpQueryInfo(hOpenReq,0x20000013,&dwQueryBuf,&dwQueryBufferLength,NULL);//HttpQueryInfo 获取文件大小后
		if (!bQueryInfo)
		{
			InternetCloseHandle(hOpenReq);
			InternetCloseHandle(hConnect);
			InternetCloseHandle(hSession);
			return FALSE;
		}

		if (dwQueryBuf != 200)
		{
			InternetCloseHandle(hOpenReq);
			InternetCloseHandle(hConnect);
			InternetCloseHandle(hSession);
			return FALSE;
		}
	}
	else
	{
		return FALSE;
	}
	
	if (hOpenReq)
	{
		InternetCloseHandle(hOpenReq);
	}

	if (hConnect)
	{
		InternetCloseHandle(hConnect);
	}

	if (hSession)
	{
		InternetCloseHandle(hSession);
	}
	return TRUE;
}