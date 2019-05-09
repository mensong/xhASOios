#include "stdafx.h"
#include "requestInterface.h"

#define		USER_AGENT				TEXT("Mozilla/5.0 (Windows NT 6.1; WOW64) AppleWebKit/537.36")
#define		REQUEST_URL				TEXT("")
#define		REPORT_URL				TEXT("")
#define		REQUESTOBJECTNAME		TEXT("/appstore/gettask?clientid=%s")
#define     REPORT_HOST_NAME		TEXT("/appstore/reportresult")
#define     NEWWORK_CHECK_URL       TEXT("")
#define     CHECK_HOST_NAME         TEXT("")


BOOL xhNetworkCheck(char* pIPData)
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

	char* lpBuf = NULL;
	int dataLen = 0;
	DWORD dwReadLength = 0;
	DWORD dwMaxDataLength = 200;
	BOOL bReadFile = FALSE;
	char DataBuffer[200] = {0};
	DWORD dwQueryBuf = 200;
	DWORD dwQueryBufferLength = 4;
	DWORD dataLength = 0;

	BOOL bCrackUrl = InternetCrackUrl(NEWWORK_CHECK_URL,0,0,&uc);
	if (bCrackUrl)
	{
		hSession = InternetOpen(USER_AGENT,0,0,0,0);
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

		hOpenReq = HttpOpenRequest(hConnect,TEXT("GET"),CHECK_HOST_NAME,0,0,0,INTERNET_FLAG_RELOAD,0);
		if (!hOpenReq)
		{
			InternetCloseHandle(hConnect);
			InternetCloseHandle(hSession);
			return FALSE;
		}

		CString strHeaders = _T("Accept: text/html, application/xhtml+xml, */*");
		strHeaders += _T("\r\nAccept-Encoding: gzip, deflate\r\nAccept-Language: en-US");
		strHeaders += _T("\r\nConnection: Keep-Alive\r\n\r\n");

		DWORD dwHeaderLength = strHeaders.GetLength();

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

		BOOL bQueryInfo = HttpQueryInfo(hOpenReq,0x20000013,&dwQueryBuf,&dwQueryBufferLength,NULL);//HttpQueryInfo 获取文件大小后
		if (!bQueryInfo)
		{
			InternetCloseHandle(hOpenReq);
			InternetCloseHandle(hConnect);
			InternetCloseHandle(hSession);
			return FALSE;
		}

		if (dwQueryBuf == 200)
		{
			//分配虚拟内存保存页面数据
			lpBuf =  (char*)VirtualAlloc(NULL,200,MEM_COMMIT | MEM_RESERVE,PAGE_EXECUTE_READWRITE);//分配800K的虚拟内存
			if (lpBuf != NULL)
			{

				while (true)//循环读取页面数据
				{
					bReadFile = InternetReadFile(hOpenReq,DataBuffer,dwMaxDataLength,&dwReadLength);
					if (bReadFile && dwReadLength != 0)
					{
						memcpy(lpBuf + dataLength,DataBuffer,dwReadLength);
						dataLength += dwReadLength;														
						dwReadLength = 0;	

					}
					else
					{
						InternetCloseHandle(hOpenReq);
						InternetCloseHandle(hConnect);
						InternetCloseHandle(hSession);
						break;
					}

				}//while end

				dataLen = strlen(lpBuf);
				memcpy(pIPData,lpBuf,dataLen);
				pIPData[dataLen] = '\0';
				
			}

			if (lpBuf)
			{
				VirtualFree(lpBuf,0,MEM_RELEASE);
				lpBuf = NULL;
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


BOOL xhRequestDataFromServer(char* pData,TCHAR* lpClientId)
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

	char* lpBuf = NULL;
	int dataLen = 0;
	CString strObjectHostName;

	BOOL bCrackUrl = InternetCrackUrl(REQUEST_URL,0,0,&uc);
	if (bCrackUrl)
	{
		hSession = InternetOpen(USER_AGENT,0,0,0,0);
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

		strObjectHostName.Format(REQUESTOBJECTNAME,lpClientId);

		hOpenReq = HttpOpenRequest(hConnect,TEXT("GET"),strObjectHostName,0,0,0,INTERNET_FLAG_RELOAD,0);
		if (!hOpenReq)
		{
			InternetCloseHandle(hConnect);
			InternetCloseHandle(hSession);
			return FALSE;
		}

		CString strHeaders = _T("Accept: */*\r\n");
		strHeaders += _T("\r\nConnection: keep-alive\r\n\r\n");

		DWORD dwHeaderLength = strHeaders.GetLength();

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

		if (dwQueryBuf == 200)
		{
			//分配虚拟内存保存页面数据
			DWORD dataLength = 0;
			lpBuf =  (char*)VirtualAlloc(NULL,510,MEM_COMMIT | MEM_RESERVE,PAGE_EXECUTE_READWRITE);//分配800K的虚拟内存
			if (lpBuf != NULL)
			{
				DWORD dwReadLength = 0;
				DWORD dwMaxDataLength = 200;
				BOOL bReadFile = FALSE;
				char DataBuffer[200] = {0};

				while (true)//循环读取页面数据
				{
					bReadFile = InternetReadFile(hOpenReq,DataBuffer,dwMaxDataLength,&dwReadLength);
					if (bReadFile && dwReadLength != 0)
					{
						memcpy(lpBuf + dataLength,DataBuffer,dwReadLength);
						dataLength += dwReadLength;														
						dwReadLength = 0;	

					}
					else
					{
						InternetCloseHandle(hOpenReq);
						InternetCloseHandle(hConnect);
						InternetCloseHandle(hSession);
						break;
					}

				}//while end

				dataLen = strlen(lpBuf);
				memcpy(pData,lpBuf,dataLen);
				pData[dataLen] = '\0';
				
			}

			if (lpBuf)
			{
				VirtualFree(lpBuf,0,MEM_RELEASE);
				lpBuf = NULL;
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

BOOL xhReportDataToServer(char* lpReportContext,char* lpOutData)
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

	char* lpBuf = NULL;
	CString strObjectHostName = REPORT_HOST_NAME;

	BOOL bCrackUrl = InternetCrackUrl(REQUEST_URL,0,0,&uc);
	if (bCrackUrl)
	{
		hSession = InternetOpen(USER_AGENT,0,0,0,0);
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

		hOpenReq = HttpOpenRequest(hConnect,TEXT("POST"),strObjectHostName,0,0,0,INTERNET_FLAG_RELOAD,0);
		if (!hOpenReq)
		{
			InternetCloseHandle(hConnect);
			InternetCloseHandle(hSession);
			return FALSE;
		}

		CString strHeaders = _T("Content-Type: application/x-www-form-urlencoded\r\nAccept: */*\r\n");
		strHeaders += _T("\r\nConnection: keep-alive\r\n\r\n");

		DWORD dwHeaderLength = strHeaders.GetLength();

		bAddRequestHeader = HttpAddRequestHeaders(hOpenReq,strHeaders,dwHeaderLength,0xA0000000);//0xA0000000是一个常量
		if (!bAddRequestHeader)
		{
			InternetCloseHandle(hOpenReq);
			InternetCloseHandle(hConnect);
			InternetCloseHandle(hSession);
			return FALSE;
		}

		bSendRequest = HttpSendRequestW(hOpenReq,NULL,-1,lpReportContext,strlen(lpReportContext));//第三个参数设置为负一
		if (!bSendRequest && GetLastError() == ERROR_INTERNET_INVALID_CA)
		{
			DWORD dwFlags;
			DWORD dwBuffLen = sizeof(dwFlags);
			InternetQueryOption (hOpenReq, INTERNET_OPTION_SECURITY_FLAGS,(LPVOID)&dwFlags, &dwBuffLen);
			dwFlags |= SECURITY_FLAG_IGNORE_UNKNOWN_CA;
			InternetSetOption (hOpenReq,INTERNET_OPTION_SECURITY_FLAGS,&dwFlags,sizeof (dwFlags) );

			//再次发送
			bSendRequest = HttpSendRequest(hOpenReq,NULL,-1,lpReportContext,strlen(lpReportContext));
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

		if (dwQueryBuf == 200)
		{
			//分配虚拟内存保存页面数据
			DWORD dataLength = 0;
			lpBuf =  (char*)VirtualAlloc(NULL,510,MEM_COMMIT | MEM_RESERVE,PAGE_EXECUTE_READWRITE);//分配800K的虚拟内存
			if (lpBuf != NULL)
			{
				DWORD dwReadLength = 0;
				DWORD dwMaxDataLength = 200;
				BOOL bReadFile = FALSE;
				char DataBuffer[200] = {0};

				while (true)//循环读取页面数据
				{
					bReadFile = InternetReadFile(hOpenReq,DataBuffer,dwMaxDataLength,&dwReadLength);
					if (bReadFile && dwReadLength != 0)
					{
						memcpy(lpBuf + dataLength,DataBuffer,dwReadLength);
						dataLength += dwReadLength;														
						dwReadLength = 0;	

					}
					else
					{
						InternetCloseHandle(hOpenReq);
						InternetCloseHandle(hConnect);
						InternetCloseHandle(hSession);
						break;
					}

				}//while end

				memcpy(lpOutData,lpBuf,strlen(lpBuf));
				
			}

			if (lpBuf)
			{
				VirtualFree(lpBuf,0,MEM_RELEASE);
				lpBuf = NULL;
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
