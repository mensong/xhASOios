#include "stdafx.h"
#include <atltime.h>
#include "splookupoffer.h"
#include "GZipHelper.h"
#include <iostream>
#include <string>
#include <strsafe.h>
#include "json.h"

using namespace std;
#pragma   comment   (lib,"ZLIB.LIB")

#define USER_AGENT TEXT("AppStore/2.0 iOS/10.1.1 model/iPhone9,1 hwp/s5l9950x build/14B100 (6; dt:97)")//TEXT("AppStore/2.0 iOS/10.0.2 model/iPhone7,1 hwp/s5l8950x build/13F69 (6; dt:82)")
#define USER_ITUNESSTORED_AGENT  TEXT("itunesstored/1.0 iOS/10.1.1 model/iPhone9,1 hwp/s5l9950x build/14B100 (6; dt:97)")//TEXT("itunesstored/1.0 iOS/10.0.2 model/iPhone7,1 hwp/s5l8950x build/13F69 (6; dt:82)")

#define ALLOCATE_SIZE 4096

BOOL ReadResponseData(HINTERNET hOpenReq,LPVOID lpBuffer,LPDWORD lpdwSize)
{
	LPGZIP lpgzipBuf = NULL;
	DWORD dataLength = 0;
	DWORD dwReadLength = 0;
	DWORD dwMaxDataLength = 200;
	BOOL bReadFile = FALSE;
	char DataBuffer[200] = {0};
	char* pDecodeData = NULL;
	int decodeLen = 0;


	//分配虚拟内存保存页面数据
	lpgzipBuf =  (LPGZIP)VirtualAlloc(NULL,1024*4,MEM_COMMIT | MEM_RESERVE,PAGE_EXECUTE_READWRITE);//分配4K的虚拟内存
	if (lpgzipBuf != NULL)
	{
		while (true)//循环读取页面数据
		{
			bReadFile = InternetReadFile(hOpenReq,DataBuffer,dwMaxDataLength,&dwReadLength);
			if (bReadFile && dwReadLength != 0)
			{
				memcpy(lpgzipBuf + dataLength,DataBuffer,dwReadLength);
				dataLength += dwReadLength;														
				dwReadLength = 0;	

			}
			else
			{
				lpgzipBuf[dataLength] = '\0';
				break;
			}

		}//while end

		CGZIP2A gzip2A(lpgzipBuf,dataLength);
		pDecodeData = gzip2A.psz;//解密出来的网页Json数据指针
		if (!pDecodeData)
		{
			if (lpgzipBuf)
			{
				VirtualFree(lpgzipBuf,0,MEM_RELEASE);
				lpgzipBuf = NULL;
			}

		}
		decodeLen = strlen(pDecodeData);
		memcpy(lpBuffer,pDecodeData,decodeLen);
		*lpdwSize = decodeLen;
		
	}

	if (lpgzipBuf)
	{
		VirtualFree(lpgzipBuf,0,MEM_RELEASE);
		lpgzipBuf = NULL;
	}

	return TRUE;
}


BOOL getSignSapSetup(CString strUserAgent,
					 char* lpBuffer,
					 char* lpOutBuf,
					 int& dataLen,
					 char* lpDsid,
					 CString strCookie,
					 TCHAR* lpX_Apple_I_md_m,
					 TCHAR* lpX_Apple_I_md,
					 TCHAR* lpProxy,
					 int netType)
{
	BOOL bResult = FALSE;
	URL_COMPONENTS uc;
	TCHAR scheme[MAX_PATH] = {0};
	TCHAR hostName[MAX_PATH] = {0};
	TCHAR userName[MAX_PATH] = {0};
	TCHAR password[MAX_PATH] = {0};
	TCHAR urlPath[MAX_PATH] = {0};
	TCHAR ExtraInfo[MAX_PATH] = {0};
	INTERNET_PORT nPort = 0;

	char* lpRecvBuf = NULL;
	DWORD dataLength = 0;
	char firstSignKey[] = "<data>";
	char lastSignKey[] = "</data>";

	CString strAgent;
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
	
	strAgent = strUserAgent;

	CString strUrl = TEXT("https://play.itunes.apple.com");
	CString strHostName = TEXT("/WebObjects/MZPlay.woa/wa/signSapSetup");

	BOOL bCrackUrl = InternetCrackUrl(strUrl,0,0,&uc);
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

		hOpenReq = HttpOpenRequest(hConnect,TEXT("POST"),strHostName,0,0,0,0x84801000|INTERNET_FLAG_NO_COOKIES,0); //0x84A01000是一个常数
		if (!hOpenReq)
		{
			InternetCloseHandle(hConnect);
			InternetCloseHandle(hSession);
			return FALSE;
		}

		CString strHeaders = TEXT("Accept-Language: zh-Hans");
		strHeaders += TEXT("\r\nX-Dsid: ");
		TCHAR strdsid[64] = {0};
		MultiByteToWideChar(CP_ACP,NULL,(LPCSTR)lpDsid,strlen((char*)lpDsid),strdsid,64);
		strHeaders += strdsid;
		
		strHeaders += TEXT("\r\nUser-Agent: ");
		strHeaders += strAgent;
		strHeaders += TEXT("\r\nAccept: */*");
		strHeaders += TEXT("\r\nX-Apple-Connection-Type: WiFi");
		strHeaders += TEXT("\r\nX-Apple-Store-Front: 143465-19,29");
		strHeaders += TEXT("\r\nContent-Type: application/x-www-form-urlencoded");
		
		if (lpX_Apple_I_md_m[0] != TEXT('\0'))
		{
			strHeaders += TEXT("\r\nX-Apple-I-MD-RINFO: 17106176");
			strHeaders += _T("\r\nX-Apple-I-MD-M: ");
			strHeaders += lpX_Apple_I_md_m;
			strHeaders += _T("\r\nX-Apple-I-MD: ");
			strHeaders += lpX_Apple_I_md;
		}

		if (!strCookie.IsEmpty())
		{
			strHeaders += TEXT("\r\nCookie: ");
			strHeaders += strCookie;

		}
		strHeaders += _T("\r\nX-Apple-I-Client-Time: ");
		strHeaders += GetSPClientTime();
		strHeaders += TEXT("\r\nX-Apple-Tz: 28800");
		strHeaders += TEXT("\r\nAccept-Encoding: gzip, deflate");
		strHeaders += TEXT("\r\nConnection: keep-alive");
		strHeaders += TEXT("\r\nProxy-Connection: keep-alive\r\n\r\n");
		
		DWORD dwHeaderLength = wcslen(strHeaders);
					
		bAddRequestHeader = HttpAddRequestHeaders(hOpenReq,strHeaders,dwHeaderLength,0xA0000000);//0xA0000000是一个常量
		if (!bAddRequestHeader)
		{
			InternetCloseHandle(hOpenReq);
			InternetCloseHandle(hConnect);
			InternetCloseHandle(hSession);
			return FALSE;
		}

		///send data to apple server....
		char constStr[] = "<?xml version=\"1.0\" encoding=\"UTF-8\"?><plist version=\"1.0\"><dict><key>sign-sap-setup-buffer</key><data>%s</data></dict></plist>";
		char SendData[1024] = {0};
		sprintf(SendData,constStr,lpBuffer);

		DWORD sendLength = strlen(SendData);

		bSendRequest = HttpSendRequest(hOpenReq,NULL,-1,SendData,sendLength);
		if (!bSendRequest && GetLastError() == ERROR_INTERNET_INVALID_CA)
		{
			DWORD dwFlags;
			DWORD dwBuffLen = sizeof(dwFlags);
			InternetQueryOption (hOpenReq, INTERNET_OPTION_SECURITY_FLAGS,(LPVOID)&dwFlags, &dwBuffLen);
			dwFlags |= SECURITY_FLAG_IGNORE_UNKNOWN_CA;
			InternetSetOption (hOpenReq,INTERNET_OPTION_SECURITY_FLAGS,&dwFlags,sizeof (dwFlags) );

			//再次发送
			bSendRequest = HttpSendRequest(hOpenReq,NULL,-1,SendData,sendLength);
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
		bQueryInfo = HttpQueryInfo(hOpenReq,0x20000013,&dwQueryBuf,&dwQueryBufferLength,NULL);//HttpQueryInfo 获取文件大小后
		if (!bQueryInfo)
		{
			InternetCloseHandle(hOpenReq);
			InternetCloseHandle(hConnect);
			InternetCloseHandle(hSession);
			return FALSE;
		}

		if (dwQueryBuf == 200)
		{
			lpRecvBuf = new char[ALLOCATE_SIZE];
			if (!lpRecvBuf)
			{
				InternetCloseHandle(hOpenReq);
				InternetCloseHandle(hConnect);
				InternetCloseHandle(hSession);
				return FALSE;
			}
			ZeroMemory(lpRecvBuf,ALLOCATE_SIZE);

			//获取返回数据
			ReadResponseData(hOpenReq,lpRecvBuf,&dataLength);

			//获取关键值
			char* ptr_1 = strstr(lpRecvBuf,firstSignKey);
			int signKeyLength = strlen(firstSignKey);
			ptr_1 = ptr_1 + signKeyLength;
			char* ptr_2 = strstr(lpRecvBuf,lastSignKey);
			int signSapLength = ptr_2 - ptr_1;

			memcpy(lpOutBuf,ptr_1,signSapLength);
			dataLen = signSapLength;
			
			if (lpRecvBuf)
			{
				delete []lpRecvBuf;
				lpRecvBuf = NULL;
			}

		}
		else
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

CString GetSPClientTime()
{
	CString strTime;

	char szTimeStamp[MAX_PATH] = {0};
	int day = 0;
	int hour = 0;
	CTime tDateTime= CTime::GetCurrentTime();

	if (tDateTime.GetHour() - 8 >= 0)
	{
		day = tDateTime.GetDay();
		hour = tDateTime.GetHour() - 8;
	}
	else if (tDateTime.GetHour() - 8 < 0)
	{
		day = tDateTime.GetDay() - 1;
		hour = 24 + tDateTime.GetHour() - 8;
	}

	sprintf(szTimeStamp,"%d-%d-%dT%d:%d:%dZ",
							tDateTime.GetYear(),
							tDateTime.GetMonth(),
							day,
							hour,
							tDateTime.GetMinute(),
							tDateTime.GetSecond());

	USES_CONVERSION;
	strTime = A2T(szTimeStamp);
	return strTime;
}

BOOL sendSpLookupOffer(CString strUserAgent,char* lpSignature,char* lpDsid,char* lpSaleId,char* lpTimeStamp,CString strCookie,TCHAR* lpX_Apple_I_md_m,TCHAR* lpX_Apple_I_md,TCHAR* lpProxy,int netType)
{
	BOOL bResult = FALSE;
	URL_COMPONENTS uc;
	TCHAR scheme[MAX_PATH] = {0};
	TCHAR hostName[MAX_PATH] = {0};
	TCHAR userName[MAX_PATH] = {0};
	TCHAR password[MAX_PATH] = {0};
	TCHAR urlPath[MAX_PATH] = {0};
	TCHAR ExtraInfo[MAX_PATH] = {0};
	INTERNET_PORT nPort = 0;

	DWORD dwUrlLen;
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

	CString strHeaders;
	TCHAR dwStrHostName[512] = {0};
	char szHostName[MAX_PATH] = {0};
	TCHAR strdsid[64] = {0};
	TCHAR strTimeStamp[MAX_PATH] = {0};
	int error = -1;

	CString strUrl = TEXT("https://sp.itunes.apple.com");
	CString strAgent = strUserAgent;

	int aSigLength = strlen(lpSignature);
	TCHAR* lpwzSignature = new TCHAR[aSigLength*2+4];
	if (!lpwzSignature)
	{
		return FALSE;
	}
	ZeroMemory(lpwzSignature,aSigLength*2+4);

	MultiByteToWideChar(CP_ACP,0,lpSignature,aSigLength,lpwzSignature,aSigLength*2+4);
	

	sprintf(szHostName,"/WebObjects/MZStorePlatform.woa/wa/lookup?caller=itunesstored&p=offer&id=%s&version=1",lpSaleId);
	MultiByteToWideChar(CP_ACP,0,szHostName,strlen(szHostName),dwStrHostName,sizeof(dwStrHostName));

	MultiByteToWideChar(CP_ACP,NULL,(LPCSTR)lpDsid,strlen((char*)lpDsid),strdsid,64);

	MultiByteToWideChar(CP_ACP,NULL,lpTimeStamp,strlen(lpTimeStamp),strTimeStamp,MAX_PATH);

	BOOL bCrackUrl = InternetCrackUrl(strUrl,0,0,&uc);
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

		hOpenReq = HttpOpenRequest(hConnect,TEXT("GET"),dwStrHostName,0,0,0,0x84A01000|INTERNET_FLAG_NO_COOKIES,0); //0x84A01000是一个常数
		if (!hOpenReq)
		{
			InternetCloseHandle(hConnect);
			InternetCloseHandle(hSession);
			return FALSE;
		}

		strHeaders += TEXT("Accept-Language: zh-Hans");
		strHeaders += TEXT("\r\nX-Dsid: ");
		strHeaders += strdsid;
		strHeaders += TEXT("\r\nUser-Agent: ");
		strHeaders += strAgent;
		strHeaders += TEXT("\r\nX-Request-TimeStamp: ");
		strHeaders += strTimeStamp;
		strHeaders += TEXT("+0800");
		strHeaders += TEXT("\r\nX-Apple-ActionSignature: ");
		strHeaders += lpwzSignature;
		strHeaders += TEXT("\r\nAccept: */*");
		strHeaders += TEXT("\r\nX-Apple-Connection-Type: WiFi");
		strHeaders += TEXT("\r\nX-Apple-Store-Front: 143465-19,29");
		strHeaders += TEXT("\r\nAccept-Encoding: gzip, deflate");

		strHeaders += _T("\r\nX-Apple-I-MD-RINFO: 17106176");
		strHeaders += _T("\r\nX-Apple-I-MD-M: ");
		strHeaders += lpX_Apple_I_md_m;
		strHeaders += _T("\r\nX-Apple-I-MD: ");
		strHeaders += lpX_Apple_I_md;

		if (!strCookie.IsEmpty())
		{
			strHeaders += TEXT("\r\nCookie: ");
			strHeaders += strCookie;

		}
		strHeaders += _T("\r\nX-Apple-I-Client-Time: ");
		strHeaders += GetSPClientTime();

		strHeaders += _T("\r\nX-Apple-Tz: 28800");
		strHeaders += TEXT("\r\nConnection: keep-alive\r\nProxy-Connection: keep-alive\r\n\r\n");				
		
		DWORD dwHeaderLength = strHeaders.GetLength();
					
		bAddRequestHeader = HttpAddRequestHeaders(hOpenReq,strHeaders.GetBuffer(),dwHeaderLength,0xA0000000);//0xA0000000是一个常量
		if (!bAddRequestHeader)
		{
			InternetCloseHandle(hOpenReq);
			InternetCloseHandle(hConnect);
			InternetCloseHandle(hSession);
			return FALSE;
		}


		bSendRequest = HttpSendRequest(hOpenReq,NULL,-1,NULL,0);
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
				error = GetLastError();//ERROR_INTERNET_SEC_CERT_REV_FAILED 
				InternetCloseHandle(hOpenReq);
				InternetCloseHandle(hConnect);
				InternetCloseHandle(hSession);
				return FALSE;
			}
		}
		else if (!bSendRequest)
		{
			error = GetLastError();//ERROR_INTERNET_SEC_CERT_REV_FAILED 
			InternetCloseHandle(hOpenReq);
			InternetCloseHandle(hConnect);
			InternetCloseHandle(hSession);
			return FALSE;
		}
		DWORD dwQueryBuf = 200;
		DWORD dwQueryBufferLength = 4;
		bQueryInfo = HttpQueryInfo(hOpenReq,0x20000013,&dwQueryBuf,&dwQueryBufferLength,NULL);//HttpQueryInfo 获取文件大小后
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



