#include "stdafx.h"
#include "comment.h"
#include "urlcode.h"
#include "viewpage.h"
#include "GZipHelper.h"

using namespace std;
#pragma   comment   (lib,"ZLIB.LIB")
#define    MAX_NICKNAME            128
#define    MAX_TITLE               128
#define    MAX_BODY                512
#define    MAX_SEND_DATA           1024

#define      USER_AGENT   TEXT("AppStore/2.0 iOS/10.1.1 model/iPhone9,1 hwp/s5l9950x build/14B100 (6; dt:97)")//TEXT("AppStore/2.0 iOS/10.0.2 model/iPhone7,1 hwp/s5l8950x build/13F69 (6; dt:82)")
#define		 USER_STOREUI_AGENT		TEXT("StoreKitUIService/1.0 iOS/9.3.3 model/iPhone6,1 hwp/s5l8960x build/13G34 (6; dt:89)")

CString parseCommentCookie(HINTERNET hOpenReq)
{
	string strResHeaderData;
	string strSubData;
	int beginPos = 0;
	int endPos = 0;
	DWORD headSize = 0;
	char* pAnsiiBuf = NULL;
	CString strCommentCookie;
	string strNSC_navtfsqvcmjtijoh = "NSC_navtfsqvcmjtijoh";

	HttpQueryInfo(hOpenReq,HTTP_QUERY_RAW_HEADERS_CRLF,NULL,&headSize,NULL);

	LPVOID pHeaderBuf = VirtualAlloc(NULL,headSize,MEM_COMMIT | MEM_RESERVE,PAGE_EXECUTE_READWRITE);
	if (!pHeaderBuf)
	{
		return strCommentCookie;
	}
	memset(pHeaderBuf,0,headSize);
	HttpQueryInfo(hOpenReq,HTTP_QUERY_RAW_HEADERS_CRLF,pHeaderBuf,&headSize,NULL);
	if (((char*)pHeaderBuf)[1] == 0)//UNICODE
	{
		int actualSize = WideCharToMultiByte(CP_ACP,0,(LPCWSTR)pHeaderBuf,-1,NULL,0,NULL,NULL);
		pAnsiiBuf = (char*)VirtualAlloc(NULL,actualSize,MEM_COMMIT | MEM_RESERVE,PAGE_EXECUTE_READWRITE);
		if (!pAnsiiBuf)
		{
			if (pHeaderBuf)
			{
				VirtualFree(pHeaderBuf,0,MEM_RELEASE);
				pHeaderBuf = NULL;
			}
			return strCommentCookie;
		}
		memset(pAnsiiBuf,0,actualSize);
		WideCharToMultiByte(CP_ACP,0,(LPCWSTR)pHeaderBuf,headSize,pAnsiiBuf,actualSize,NULL,NULL);
		strResHeaderData = pAnsiiBuf;
	}
	else//ANSII
	{
		strResHeaderData = (char*)pHeaderBuf;
	}

	USES_CONVERSION;

	//提取Cookie的值
	beginPos = strResHeaderData.find("NSC_navtfsqvcmjtijoh");
	if (beginPos >= 0)
	{
		strSubData = strResHeaderData.substr(beginPos);
		endPos = strSubData.find(";");
		strNSC_navtfsqvcmjtijoh += strSubData.substr(0,endPos);
		strCommentCookie = A2T(strNSC_navtfsqvcmjtijoh.c_str());
		
	}

	if (pAnsiiBuf)
	{
		VirtualFree(pAnsiiBuf,0,MEM_RELEASE);
		pAnsiiBuf = NULL;
	}

	if (pHeaderBuf)
	{
		VirtualFree(pHeaderBuf,0,MEM_RELEASE);
		pHeaderBuf = NULL;
	}

	return strCommentCookie;

}

BOOL AppStoreCustomerReviews(CString strUserAgent,
							 CString strCookie,
							 char* lpPasswordToken,
							 char* lpDsid,
							 char* lpSaleId,
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
	
	CString strAgent = strUserAgent;
	CString strUrl = _T("https://itunes.apple.com");
	
	CString strVerb = _T("GET");

	char* lpBuf = NULL;


	TCHAR dwObjectName[512] = {0};
	char szObjectName[MAX_PATH] = {0};
	sprintf(szObjectName,"/cn/customer-reviews/id%s?dataOnly=true&displayable-kind=11&appVersion=current",lpSaleId);
	::MultiByteToWideChar(CP_ACP,0,szObjectName,strlen(szObjectName),dwObjectName,512);
	CString strAdrArg(dwObjectName);
	

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

		hOpenReq = HttpOpenRequest(hConnect,strVerb,strAdrArg,0,0,0,0x84801000|INTERNET_FLAG_NO_COOKIES,0);//0x84A01000是一个常数
		if (!hOpenReq)
		{
			InternetCloseHandle(hConnect);
			InternetCloseHandle(hSession);
			return FALSE;
		}

		TCHAR buf[128] = {0};
		MultiByteToWideChar(CP_ACP,NULL,(char*)lpPasswordToken,-1,buf,128);

		CString strSaleId;
		strSaleId.Format(_T("%d"),atoi((char*)lpSaleId));

		CString strHeaders = _T("User-Agent: ");
		strHeaders += strAgent;
		strHeaders += _T("\r\nAccept-Language: zh-Hans\r\nX-Token: ");
		strHeaders.Append(buf);
		strHeaders += _T("\r\nX-Dsid: ");

		TCHAR strdsid[64] = {0};
		MultiByteToWideChar(CP_ACP,NULL,(LPCSTR)lpDsid,strlen((char*)lpDsid),strdsid,64);
		strHeaders += strdsid;
		strHeaders += _T("\r\nAccept: */*");
		strHeaders += _T("\r\nX-Apple-Connection-Type: WiFi");
		strHeaders += _T("\r\nX-Apple-Client-Versions: GameCenter/2.0");
		strHeaders += _T("\r\nX-Apple-Store-Front: 143465-19,29 t:native");
		strHeaders += _T("\r\nX-Apple-I-MD-RINFO: 17106176");
		strHeaders += _T("\r\nX-Apple-I-MD-M: ");
		strHeaders += lpX_Apple_I_md_m;
		strHeaders += _T("\r\nX-Apple-I-MD: ");
		strHeaders += lpX_Apple_I_md;
		strHeaders += _T("\r\nX-Apple-Partner: origin.0");
		strHeaders += _T("\r\nAccept-Encoding: gzip, deflate");
		strHeaders += TEXT("\r\nCookie: ");
		strHeaders += strCookie;
		strHeaders += _T("\r\nProxy-Connection: keep-alive\r\nConnection: keep-alive\r\n\r\n");

		DWORD dwHeaderLength = wcslen(strHeaders);
		
		bAddRequestHeader = HttpAddRequestHeaders(hOpenReq,strHeaders,dwHeaderLength,0xA0000000);//0xA0000000是一个常量
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
			InternetCloseHandle(hOpenReq);
			InternetCloseHandle(hConnect);
			InternetCloseHandle(hSession);
			return TRUE;
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

	//关闭连接请求
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


BOOL AppStoreUserReviewRow(CString strUserAgent,
						   CString strCookie,
						   char* lpPasswordToken,
						   char* lpDsid,
						   char* lpSaleId,
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
	
	CString strAgent = strUserAgent;
	CString strUrl = _T("https://itunes.apple.com");
	
	CString strVerb = _T("GET");

	char* lpBuf = NULL;


	TCHAR dwObjectName[1024] = {0};
	char szObjectName[MAX_BODY] = {0};
	sprintf(szObjectName,"/WebObjects/MZStore.woa/wa/userReviewsRow?appVersion=current&id=%s&displayable-kind=11&startIndex=0&endIndex=15&sort=1",lpSaleId);
	::MultiByteToWideChar(CP_ACP,0,szObjectName,strlen(szObjectName),dwObjectName,1024);
	CString strAdrArg(dwObjectName);
	

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

		hOpenReq = HttpOpenRequest(hConnect,strVerb,strAdrArg,0,0,0,0x84801000|INTERNET_FLAG_NO_COOKIES,0);//0x84A01000是一个常数
		if (!hOpenReq)
		{
			InternetCloseHandle(hConnect);
			InternetCloseHandle(hSession);
			return FALSE;
		}

		TCHAR buf[MAX_PATH] = {0};
		MultiByteToWideChar(CP_ACP,NULL,(char*)lpPasswordToken,-1,buf,MAX_PATH);

		CString strSaleId;
		strSaleId.Format(_T("%d"),atoi((char*)lpSaleId));

		CString strHeaders = _T("User-Agent: ");
		strHeaders += strAgent;
		strHeaders += _T("\r\nAccept-Language: zh-Hans\r\nX-Token: ");
		strHeaders.Append(buf);
		strHeaders += _T("\r\nX-Dsid: ");

		TCHAR strdsid[64] = {0};
		MultiByteToWideChar(CP_ACP,NULL,(LPCSTR)lpDsid,strlen((char*)lpDsid),strdsid,64);
		strHeaders += strdsid;
		strHeaders += _T("\r\nAccept: */*");
		strHeaders += _T("\r\nX-Apple-Connection-Type: WiFi");
		strHeaders += _T("\r\nX-Apple-Client-Versions: GameCenter/2.0");
		strHeaders += _T("\r\nX-Apple-Store-Front: 143465-19,29 t:native");
		strHeaders += _T("\r\nX-Apple-I-MD-RINFO: 17106176");
		strHeaders += _T("\r\nX-Apple-I-MD-M: ");
		strHeaders += lpX_Apple_I_md_m;
		strHeaders += _T("\r\nX-Apple-I-MD: ");
		strHeaders += lpX_Apple_I_md;
		strHeaders += _T("\r\nX-Apple-Partner: origin.0");
		strHeaders += _T("\r\nAccept-Encoding: gzip, deflate");
		strHeaders += TEXT("\r\nCookie: ");
		strHeaders += strCookie;
		strHeaders += _T("\r\nProxy-Connection: keep-alive\r\nConnection: keep-alive\r\n\r\n");

		DWORD dwHeaderLength = wcslen(strHeaders);
		
		bAddRequestHeader = HttpAddRequestHeaders(hOpenReq,strHeaders,dwHeaderLength,0xA0000000);//0xA0000000是一个常量
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
			InternetCloseHandle(hOpenReq);
			InternetCloseHandle(hConnect);
			InternetCloseHandle(hSession);
			return TRUE;
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

	//关闭连接请求
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

BOOL AppStoreWriteUserReview(CString strUserAgent,
							 CString strCookie,
							 CString& strCommentCookie,
							 char* lpPasswordToken,
							 char* lpDsid,
							 char* lpSaleId,
							 char* lpAppExtVrsId,
							 CString& strOutNickName,
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
	
	LPGZIP lpgzipBuf = NULL;
	int nicknameLen = 0;
	char* pBeginPos = NULL;
	char* pEndPos = NULL;
	char szNickname[128] = {0};
	TCHAR buf[MAX_PATH] = {0};
	TCHAR swNickname[MAX_PATH] = {0};
	char strBegin[] = "<key>nickname</key><string>";

	CString strAgent = strUserAgent;
	CString strUrl = _T("https://userpub.itunes.apple.com");	
	CString strVerb = _T("GET");

	TCHAR dwObjectName[MAX_BODY] = {0};
	char szObjectName[MAX_BODY] = {0};
	sprintf(szObjectName,"/WebObjects/MZUserPublishing.woa/wa/writeUserReview?cc=cn&displayable-kind=11&id=%s&dataOnly=true&version-to-review=%s",lpSaleId,lpAppExtVrsId);
	::MultiByteToWideChar(CP_ACP,0,szObjectName,strlen(szObjectName),dwObjectName,512);
	CString strAdrArg(dwObjectName);
	

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

		hOpenReq = HttpOpenRequest(hConnect,strVerb,strAdrArg,0,0,0,0x84801000|INTERNET_FLAG_NO_COOKIES,0);//0x84A01000是一个常数
		if (!hOpenReq)
		{
			InternetCloseHandle(hConnect);
			InternetCloseHandle(hSession);
			return FALSE;
		}

		MultiByteToWideChar(CP_ACP,NULL,(char*)lpPasswordToken,strlen((char*)lpPasswordToken),buf,MAX_PATH);

		CString strSaleId;
		strSaleId.Format(_T("%d"),atoi((char*)lpSaleId));

		CString strHeaders = _T("User-Agent: ");
		strHeaders += strAgent;
		strHeaders += _T("\r\nAccept-Language: zh-Hans\r\nX-Token: ");
		strHeaders.Append(buf);
		strHeaders += _T("\r\nX-Dsid: ");

		TCHAR strdsid[64] = {0};
		MultiByteToWideChar(CP_ACP,NULL,(LPCSTR)lpDsid,strlen((char*)lpDsid),strdsid,64);
		strHeaders += strdsid;
		strHeaders += _T("\r\nAccept: */*");
		strHeaders += _T("\r\nX-Apple-Connection-Type: WiFi");
		strHeaders += _T("\r\nX-Apple-Client-Versions: GameCenter/2.0");
		strHeaders += _T("\r\nX-Apple-Store-Front: 143465-19,29 t:native");
		strHeaders += _T("\r\nX-Apple-I-MD-RINFO: 17106176");
		strHeaders += _T("\r\nX-Apple-I-MD-M: ");
		strHeaders += lpX_Apple_I_md_m;
		strHeaders += _T("\r\nX-Apple-I-MD: ");
		strHeaders += lpX_Apple_I_md;
		strHeaders += _T("\r\nX-Apple-Partner: origin.0");
		strHeaders += _T("\r\nAccept-Encoding: gzip, deflate");
		strHeaders += TEXT("\r\nCookie: ");
		strHeaders += strCookie;
		strHeaders += _T("\r\nProxy-Connection: keep-alive\r\nConnection: keep-alive\r\n\r\n");

		DWORD dwHeaderLength = strHeaders.GetLength();
		
		bAddRequestHeader = HttpAddRequestHeaders(hOpenReq,strHeaders,dwHeaderLength,0xA0000000);//0xA0000000是一个常量
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
				int error = GetLastError();//ERROR_INTERNET_SEC_CERT_REV_FAILED 
				InternetCloseHandle(hOpenReq);
				InternetCloseHandle(hConnect);
				InternetCloseHandle(hSession);
				return FALSE;
			}
		}
		else if (!bSendRequest)
		{
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
		//获取评论需要的COOKIE
		strCommentCookie = parseCommentCookie(hOpenReq);
		if (dwQueryBuf == 200)
		{
			//分配虚拟内存保存页面数据
			DWORD dataLength = 0;
			lpgzipBuf =  (LPGZIP)VirtualAlloc(NULL,1024*3,MEM_COMMIT | MEM_RESERVE,PAGE_EXECUTE_READWRITE);//分配3K的虚拟内存
			if (lpgzipBuf != NULL)
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
				
			}
			else
			{
				InternetCloseHandle(hOpenReq);
				InternetCloseHandle(hConnect);
				InternetCloseHandle(hSession);
				return FALSE;
			}

			CGZIP2A gzip2A(lpgzipBuf,dataLength);
			char * pDecodeData = gzip2A.psz;//解密出来的网页Json数据指针
			if (!pDecodeData)
			{
				if (lpgzipBuf)
				{
					VirtualFree(lpgzipBuf,0,MEM_RELEASE);
					lpgzipBuf = NULL;
				}

			}

			//检索获取昵称名字字符串
			pBeginPos = strstr(pDecodeData,strBegin);
			if (pBeginPos)
			{
				pEndPos = strstr(pBeginPos,"</string>");
				if (!pEndPos)
				{
					if (lpgzipBuf)
					{
						VirtualFree(lpgzipBuf,0,MEM_RELEASE);
						lpgzipBuf = NULL;
					}

					InternetCloseHandle(hOpenReq);
					InternetCloseHandle(hConnect);
					InternetCloseHandle(hSession);
					return FALSE;
				}
				

			}
			else
			{
				if (lpgzipBuf)
				{
					VirtualFree(lpgzipBuf,0,MEM_RELEASE);
					lpgzipBuf = NULL;
				}

				InternetCloseHandle(hOpenReq);
				InternetCloseHandle(hConnect);
				InternetCloseHandle(hSession);
				return FALSE;

			}
			
			pBeginPos = pBeginPos + strlen(strBegin);
			nicknameLen = pEndPos - pBeginPos;

			if (nicknameLen != 0)
			{
				memcpy(szNickname,pBeginPos,nicknameLen);
				CViewPage::ConvertUtf8ToGBK(szNickname,nicknameLen);
				MultiByteToWideChar(CP_ACP,0,szNickname,strlen(szNickname),swNickname,MAX_PATH);
				strOutNickName = swNickname;

			}


			if (lpgzipBuf)
			{
				VirtualFree(lpgzipBuf,0,MEM_RELEASE);
				lpgzipBuf = NULL;
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

	//关闭连接请求
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

BOOL AppStoreSaveUserReview(CString strUserAgent,
							CString strCookie,
							CString strCommentCookie,
							char* lpPasswordToken,
							char* lpDsid,
							char* lpSaleId,
							char* lpAppExtVrsId,
							int rating,
							 LPTSTR lpwNickname,
							 LPTSTR lpwTitle,
							 LPTSTR lpwBody,
							 char* pGUID,
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

	unsigned char* pNickname = NULL;
	unsigned char* pTitle = NULL;
	unsigned char* pBody = NULL;
	unsigned char urlencodeNickname[MAX_NICKNAME] = {0};
	unsigned char urlencodeTitle[MAX_TITLE] = {0};
	unsigned char urlencodeBody[MAX_BODY] = {0};

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
	
	CString strAgent = strUserAgent;
	CString strUrl = _T("https://userpub.itunes.apple.com");
	CString strVerb = _T("POST");

	LPGZIP lpgzipBuf = NULL;
	char strBeginKey[] = "<key>status-code</key><integer>3100</integer>";
	TCHAR szNickName[MAX_PATH] = {0};
	char* pSearchStr = NULL;
	TCHAR dwObjectName[MAX_BODY] = {0};
	char szObjectName[MAX_BODY] = {0};
	sprintf(szObjectName,"/WebObjects/MZUserPublishing.woa/wa/saveUserReview?type=Purple+Software&id=%s&version-to-review=%s&displayable-kind=11",lpSaleId,lpAppExtVrsId);
	::MultiByteToWideChar(CP_ACP,0,szObjectName,strlen(szObjectName),dwObjectName,512);
	CString strAdrArg(dwObjectName);

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

		hOpenReq = HttpOpenRequest(hConnect,strVerb,strAdrArg,0,0,0,0x84801000|INTERNET_FLAG_NO_COOKIES,0);//0x84A01000是一个常数
		if (!hOpenReq)
		{
			InternetCloseHandle(hConnect);
			InternetCloseHandle(hSession);
			return FALSE;
		}

		TCHAR buf[128] = {0};
		MultiByteToWideChar(CP_ACP,NULL,(char*)lpPasswordToken,-1,buf,128);

		CString strSaleId;
		strSaleId.Format(_T("%d"),atoi((char*)lpSaleId));

		CString strHeaders = _T("User-Agent: ");
		strHeaders += strAgent;
		strHeaders += _T("\r\nAccept-Language: zh-Hans\r\nX-Token: ");
		strHeaders.Append(buf);
		strHeaders += _T("\r\nX-Dsid: ");

		TCHAR strdsid[64] = {0};
		MultiByteToWideChar(CP_ACP,NULL,(LPCSTR)lpDsid,strlen((char*)lpDsid),strdsid,64);
		strHeaders += strdsid;
		strHeaders += _T("\r\nAccept: */*");
		strHeaders += _T("\r\nX-Apple-Connection-Type: WiFi");
		strHeaders += _T("\r\nX-Apple-Client-Versions: GameCenter/2.0");
		strHeaders += _T("\r\nX-Apple-Store-Front: 143465-19,29 t:native");
		strHeaders += _T("\r\nContent-Type: application/x-www-form-urlencoded");
		strHeaders += _T("\r\nX-Apple-I-MD-RINFO: 17106176");
		strHeaders += _T("\r\nX-Apple-I-MD-M: ");
		strHeaders += lpX_Apple_I_md_m;
		strHeaders += _T("\r\nX-Apple-I-MD: ");
		strHeaders += lpX_Apple_I_md;
		strHeaders += _T("\r\nX-Apple-Partner: origin.0");
		strHeaders += _T("\r\nAccept-Encoding: gzip, deflate");
		strHeaders += TEXT("\r\nCookie: ");
		strHeaders += strCookie;

		strHeaders += TEXT(";");
		strHeaders += strCommentCookie;

		strHeaders += _T("\r\nProxy-Connection: keep-alive\r\nConnection: keep-alive\r\n\r\n");

		DWORD dwHeaderLength = wcslen(strHeaders);
		
		bAddRequestHeader = HttpAddRequestHeaders(hOpenReq,strHeaders,dwHeaderLength,0xA0000000);//0xA0000000是一个常量
		if (!bAddRequestHeader)
		{
			InternetCloseHandle(hOpenReq);
			InternetCloseHandle(hConnect);
			InternetCloseHandle(hSession);
			return FALSE;
		}

		int wNicknameLen = wcslen(lpwNickname);
		int wTitleLen = wcslen(lpwTitle);
		int wBodyLen = wcslen(lpwBody);

		int nicknameLen = WideCharToMultiByte(CP_UTF8,0,lpwNickname,wNicknameLen,NULL,0,NULL,NULL);
		int titleLen = WideCharToMultiByte(CP_UTF8,0,lpwTitle,wTitleLen,NULL,0,NULL,NULL);
		int bodyLen = WideCharToMultiByte(CP_UTF8,0,lpwBody,wBodyLen,NULL,0,NULL,NULL);
		
		pNickname = new unsigned char[nicknameLen+1];
		if (!pNickname)
		{
			return FALSE;
		}
		memset(pNickname,'\0',nicknameLen+1);
		WideCharToMultiByte(CP_UTF8,0,lpwNickname,wNicknameLen,(LPSTR)pNickname,nicknameLen,NULL,NULL);

		//Nickname url编码
		urlencode(pNickname,nicknameLen,urlencodeNickname,MAX_NICKNAME);

		pTitle = new unsigned char[titleLen+1];
		if (!pTitle)
		{
			return FALSE;
		}
		memset(pTitle,'\0',titleLen+1);
		WideCharToMultiByte(CP_UTF8,0,lpwTitle,wTitleLen,(LPSTR)pTitle,titleLen,NULL,NULL);
		//Title url编码
		urlencode(pTitle,titleLen,urlencodeTitle,MAX_TITLE);

		pBody = new unsigned char[bodyLen+1];
		if (!pBody)
		{
			return FALSE;
		}
		memset(pBody,'\0',bodyLen+1);	
		WideCharToMultiByte(CP_UTF8,0,lpwBody,wBodyLen,(LPSTR)pBody,bodyLen,NULL,NULL);		
		//Body url编码
		urlencode(pBody,bodyLen,urlencodeBody,MAX_BODY);
	
		float rate;
		switch (rating)
		{
		case 5:
			rate = 1.00;
			break;
		case 4:
			rate = 0.80;
			break;
		case 3:
			rate = 0.60;
			break;
		case 2:
			rate = 0.40;
			break;
		case 1:
			rate = 0.20;
			break;
		default:
			break;
		}
		
		char strSendData[MAX_SEND_DATA] = {0};
		sprintf(strSendData,"body=%s&nickname=%s&guid=%s&title=%s&rating=%.2f",
			urlencodeBody,urlencodeNickname,pGUID,urlencodeTitle,rate);

		int sendLen = strlen(strSendData);
		bSendRequest = HttpSendRequest(hOpenReq,strHeaders,-1,strSendData,sendLen);
		if (!bSendRequest && GetLastError() == ERROR_INTERNET_INVALID_CA)
		{
			DWORD dwFlags;
			DWORD dwBuffLen = sizeof(dwFlags);
			InternetQueryOption (hOpenReq, INTERNET_OPTION_SECURITY_FLAGS,(LPVOID)&dwFlags, &dwBuffLen);
			dwFlags |= SECURITY_FLAG_IGNORE_UNKNOWN_CA;
			InternetSetOption (hOpenReq,INTERNET_OPTION_SECURITY_FLAGS,&dwFlags,sizeof (dwFlags) );

			//再次发送
			bSendRequest = HttpSendRequest(hOpenReq,strHeaders,-1,strSendData,sendLen);
			if (!bSendRequest)
			{
				int error = GetLastError();//ERROR_INTERNET_SEC_CERT_REV_FAILED 
				InternetCloseHandle(hOpenReq);
				InternetCloseHandle(hConnect);
				InternetCloseHandle(hSession);
				//释放内存空间
				if (pTitle)
				{
					delete []pTitle;
					pTitle = NULL;
				}
				if (pBody)
				{
					delete []pBody;
					pBody = NULL;
				}
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
			//释放内存空间
			if (pNickname)
			{
				delete []pNickname;
				pNickname = NULL;
			}
			if (pTitle)
			{
				delete []pTitle;
				pTitle = NULL;
			}
			if (pBody)
			{
				delete []pBody;
				pBody = NULL;
			}

			return FALSE;
		}

		if (dwQueryBuf == 200)
		{
			///分配虚拟内存保存页面数据
			DWORD dataLength = 0;
			lpgzipBuf =  (LPGZIP)VirtualAlloc(NULL,1024*4,MEM_COMMIT | MEM_RESERVE,PAGE_EXECUTE_READWRITE);
			if (lpgzipBuf != NULL)
			{
				DWORD dwReadLength = 0;
				DWORD dwMaxDataLength = 200;
				BOOL bReadFile = FALSE;
				char DataBuffer[200];
				memset(DataBuffer,0x00,200*sizeof(char));

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
						InternetCloseHandle(hOpenReq);
						InternetCloseHandle(hConnect);
						InternetCloseHandle(hSession);
						break;
					}

				}//while end

				CGZIP2A gzip2A(lpgzipBuf,dataLength);
				char * pDecodeData = gzip2A.psz;//解密出来的网页Json数据指针
				if (pDecodeData)
				{
					//解析errorcode值
					pSearchStr = strstr(pDecodeData,strBeginKey);
					if (pSearchStr)
					{
						//昵称已经有人使用，后缀加随机数处理
						LARGE_INTEGER timeRand; 
						QueryPerformanceCounter(&timeRand);
						swprintf(szNickName,TEXT("%d%d"),lpwNickname,timeRand.LowPart);

						AppStoreSaveUserReview(strUserAgent,
											strCookie,
											strCommentCookie,
											lpPasswordToken,
											lpDsid,
											lpSaleId,
											lpAppExtVrsId,
											rating,
											szNickName,
											lpwTitle,
											lpwBody,
											pGUID,
											lpX_Apple_I_md_m,
											lpX_Apple_I_md,
											lpProxy,
											netType);
					}


				}
				else
				{
					if (lpgzipBuf)
					{
						VirtualFree(lpgzipBuf,0,MEM_RELEASE);
						lpgzipBuf = NULL;
					}
					if (pNickname)
					{
						delete []pNickname;
						pNickname = NULL;
					}
					if (pTitle)
					{
						delete []pTitle;
						pTitle = NULL;
					}
					if (pBody)
					{
						delete []pBody;
						pBody = NULL;
					}

					InternetCloseHandle(hOpenReq);
					InternetCloseHandle(hConnect);
					InternetCloseHandle(hSession);
					return FALSE;

				}

			}

			if (lpgzipBuf)
			{
				VirtualFree(lpgzipBuf,0,MEM_RELEASE);
				lpgzipBuf = NULL;
			}

		}

		//释放内存空间
		if (pNickname)
		{
			delete []pNickname;
			pNickname = NULL;
		}
		if (pTitle)
		{
			delete []pTitle;
			pTitle = NULL;
		}
		if (pBody)
		{
			delete []pBody;
			pBody = NULL;
		}

	}
	else
	{
		return FALSE;
	}

	//关闭连接请求
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

///////下面两个函数是itunes12.1.1.4版本上评论内容的
BOOL CommentwriteUserReview(char* lpPasswordToken,char* lpDsid,char* lpSaleId,char* pGUID,TCHAR* lpProxy,int netType)
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
	
	CString strAgent = USER_AGENT;
	CString strUrl = _T("https://userpub.itunes.apple.com");
	
	CString strVerb = _T("POST");


	TCHAR dwObjectName[512] = {0};
	char szObjectName[MAX_PATH] = {0};
	sprintf(szObjectName,"/WebObjects/MZUserPublishing.woa/wa/writeUserReview?cc=cn&displayable-kind=11&id=%s&dataOnly=true",lpSaleId);
	::MultiByteToWideChar(CP_ACP,0,szObjectName,strlen(szObjectName),dwObjectName,512);
	CString strAdrArg(dwObjectName);
	

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

		hOpenReq = HttpOpenRequest(hConnect,strVerb,strAdrArg,0,0,0,0x84801000,0);//0x84A01000是一个常数
		if (!hOpenReq)
		{
			InternetCloseHandle(hConnect);
			InternetCloseHandle(hSession);
			return FALSE;
		}

		TCHAR buf[128] = {0};
		MultiByteToWideChar(CP_ACP,NULL,(char*)lpPasswordToken,-1,buf,128);

		CString strSaleId;
		strSaleId.Format(_T("%d"),atoi((char*)lpSaleId));

		CString strHeaders;
		
		strHeaders = _T("Cache-Control: no-cache\r\nReferer: http://itunes.apple.com/cn/app//id");
		strHeaders += strSaleId;
		strHeaders += _T("?mt=8\r\n");
		strHeaders += _T("Accept-Language: zh-cn, zh;q=0.75, en-us;q=0.50, en;q=0.25\r\nX-Apple-Tz: 28800\r\nX-Apple-Store-Front: 143465-19,17\r\nX-Token: ");
		strHeaders.Append(buf);
		strHeaders += _T("\r\nOrigin: https://itunes.apple.com");
		strHeaders += _T("\r\nX-Dsid: ");

		TCHAR strdsid[64] = {0};
		MultiByteToWideChar(CP_ACP,NULL,(LPCSTR)lpDsid,strlen((char*)lpDsid),strdsid,64);
		strHeaders += strdsid;
		strHeaders += _T("\r\nContent-Type: application/x-www-form-urlencoded\r\nConnection: keep-alive\r\n\r\n");

		DWORD dwHeaderLength = wcslen(strHeaders);
		
		bAddRequestHeader = HttpAddRequestHeaders(hOpenReq,strHeaders,dwHeaderLength,0xA0000000);//0xA0000000是一个常量
		if (!bAddRequestHeader)
		{
			InternetCloseHandle(hOpenReq);
			InternetCloseHandle(hConnect);
			InternetCloseHandle(hSession);
			return FALSE;
		}
	
		//准备发送数据
		char strSendData[MAX_SEND_DATA] = {0};
		sprintf(strSendData,"id=%d&displayable-kind=11&desktopGuid=%s",
			atoi((char*)lpSaleId),pGUID);


		int sendLen = strlen(strSendData);
		bSendRequest = HttpSendRequest(hOpenReq,strHeaders,-1,strSendData,sendLen);
		if (!bSendRequest && GetLastError() == ERROR_INTERNET_INVALID_CA)
		{
			DWORD dwFlags;
			DWORD dwBuffLen = sizeof(dwFlags);
			InternetQueryOption (hOpenReq, INTERNET_OPTION_SECURITY_FLAGS,(LPVOID)&dwFlags, &dwBuffLen);
			dwFlags |= SECURITY_FLAG_IGNORE_UNKNOWN_CA;
			InternetSetOption (hOpenReq,INTERNET_OPTION_SECURITY_FLAGS,&dwFlags,sizeof (dwFlags) );

			//再次发送
			bSendRequest = HttpSendRequest(hOpenReq,strHeaders,-1,strSendData,sendLen);
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
		
	}
	else
	{
		return FALSE;
	}

	//关闭连接请求
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

BOOL WriteCommentContentToServer(IN LPVOID lpPasswordToken,
									IN LPVOID lpDsid,
									IN LPVOID lpSaleId,
									IN int rating,
									IN LPTSTR lpwNickname,
									IN LPTSTR lpwTitle,
									IN LPTSTR lpwBody,
									char* pGUID,
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

	unsigned char* pNickname = NULL;
	unsigned char* pTitle = NULL;
	unsigned char* pBody = NULL;
	unsigned char urlencodeNickname[MAX_NICKNAME] = {0};
	unsigned char urlencodeTitle[MAX_TITLE] = {0};
	unsigned char urlencodeBody[MAX_BODY] = {0};

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
	
	CString strAgent = USER_AGENT;
	CString strUrl = _T("https://userpub.itunes.apple.com");
	CString strAdrArg = _T("/WebObjects/MZUserPublishing.woa/wa/saveUserReview?displayable-kind=11&dataOnly=true");
	CString strVerb = _T("POST");

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

		hOpenReq = HttpOpenRequest(hConnect,strVerb,strAdrArg,0,0,0,0x84801000,0);//0x84A01000是一个常数
		if (!hOpenReq)
		{
			InternetCloseHandle(hConnect);
			InternetCloseHandle(hSession);
			return FALSE;
		}

		TCHAR buf[128] = {0};
		MultiByteToWideChar(CP_ACP,NULL,(char*)lpPasswordToken,-1,buf,128);

		CString strSaleId;
		strSaleId.Format(_T("%d"),atoi((char*)lpSaleId));

		CString strHeaders;
		
		strHeaders = _T("Cache-Control: no-cache\r\nReferer: http://itunes.apple.com/cn/app//id");
		strHeaders += strSaleId;
		strHeaders += _T("?mt=8\r\n");
		strHeaders += _T("Accept-Language: zh-cn, zh;q=0.75, en-us;q=0.50, en;q=0.25\r\nX-Apple-Tz: 28800\r\nX-Apple-Store-Front: 143465-19,17\r\nX-Token: ");
		strHeaders.Append(buf);
		strHeaders += _T("\r\nOrigin: https://itunes.apple.com");
		strHeaders += _T("\r\nX-Dsid: ");

		TCHAR strdsid[64] = {0};
		MultiByteToWideChar(CP_ACP,NULL,(LPCSTR)lpDsid,strlen((char*)lpDsid),strdsid,64);
		strHeaders += strdsid;
		strHeaders += _T("\r\nContent-Type: application/x-www-form-urlencoded\r\nConnection: keep-alive\r\n\r\n");

		DWORD dwHeaderLength = wcslen(strHeaders);
		
		bAddRequestHeader = HttpAddRequestHeaders(hOpenReq,strHeaders,dwHeaderLength,0xA0000000);//0xA0000000是一个常量
		if (!bAddRequestHeader)
		{
			InternetCloseHandle(hOpenReq);
			InternetCloseHandle(hConnect);
			InternetCloseHandle(hSession);
			return FALSE;
		}

		int wNicknameLen = wcslen(lpwNickname);
		int wTitleLen = wcslen(lpwTitle);
		int wBodyLen = wcslen(lpwBody);

		int nicknameLen = WideCharToMultiByte(CP_UTF8,0,lpwNickname,wNicknameLen,NULL,0,NULL,NULL);
		int titleLen = WideCharToMultiByte(CP_UTF8,0,lpwTitle,wTitleLen,NULL,0,NULL,NULL);
		int bodyLen = WideCharToMultiByte(CP_UTF8,0,lpwBody,wBodyLen,NULL,0,NULL,NULL);
		
		pNickname = new unsigned char[nicknameLen+1];
		if (!pNickname)
		{
			return FALSE;
		}
		memset(pNickname,'\0',nicknameLen+1);
		WideCharToMultiByte(CP_UTF8,0,lpwNickname,wNicknameLen,(LPSTR)pNickname,nicknameLen,NULL,NULL);
		//Nickname url编码
		urlencode(pNickname,nicknameLen,urlencodeNickname,MAX_NICKNAME);


		pTitle = new unsigned char[titleLen+1];
		if (!pTitle)
		{
			return FALSE;
		}
		memset(pTitle,'\0',titleLen+1);
		WideCharToMultiByte(CP_UTF8,0,lpwTitle,wTitleLen,(LPSTR)pTitle,titleLen,NULL,NULL);
		//Title url编码
		urlencode(pTitle,titleLen,urlencodeTitle,MAX_TITLE);

		pBody = new unsigned char[bodyLen+1];
		if (!pBody)
		{
			return FALSE;
		}
		memset(pBody,'\0',bodyLen+1);	
		WideCharToMultiByte(CP_UTF8,0,lpwBody,wBodyLen,(LPSTR)pBody,bodyLen,NULL,NULL);		
		//Body url编码
		urlencode(pBody,bodyLen,urlencodeBody,MAX_BODY);
	
		char strSendData[MAX_SEND_DATA] = {0};
		sprintf(strSendData,"id=%d&displayable-kind=11&rating=%d&title=%s&nickname=%s&body=%s&desktopGuid=%s",
			atoi((char*)lpSaleId),rating,urlencodeTitle,urlencodeNickname,urlencodeBody,pGUID);


		int sendLen = strlen(strSendData);
		bSendRequest = HttpSendRequest(hOpenReq,strHeaders,-1,strSendData,sendLen);
		if (!bSendRequest && GetLastError() == ERROR_INTERNET_INVALID_CA)
		{
			DWORD dwFlags;
			DWORD dwBuffLen = sizeof(dwFlags);
			InternetQueryOption (hOpenReq, INTERNET_OPTION_SECURITY_FLAGS,(LPVOID)&dwFlags, &dwBuffLen);
			dwFlags |= SECURITY_FLAG_IGNORE_UNKNOWN_CA;
			InternetSetOption (hOpenReq,INTERNET_OPTION_SECURITY_FLAGS,&dwFlags,sizeof (dwFlags) );

			//再次发送
			bSendRequest = HttpSendRequest(hOpenReq,strHeaders,-1,strSendData,sendLen);
			if (!bSendRequest)
			{
				int error = GetLastError();//ERROR_INTERNET_SEC_CERT_REV_FAILED 
				InternetCloseHandle(hOpenReq);
				InternetCloseHandle(hConnect);
				InternetCloseHandle(hSession);
				//释放内存空间
				if (pTitle)
				{
					delete []pTitle;
					pTitle = NULL;
				}
				if (pBody)
				{
					delete []pBody;
					pBody = NULL;
				}
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
			//释放内存空间
			if (pNickname)
			{
				delete []pNickname;
				pNickname = NULL;
			}
			if (pTitle)
			{
				delete []pTitle;
				pTitle = NULL;
			}
			if (pBody)
			{
				delete []pBody;
				pBody = NULL;
			}

			return FALSE;
		}

		if (dwQueryBuf == 200)
		{
			DWORD dwLength = 0;
			DWORD dwReadLength = 0;
			DWORD dwMaxDataLength = 200;
			BOOL bReadFile = FALSE;
			char DataBuffer[200] = {0};
			char nickerror[] = "nickname_error";
			char* pSearchPtr = NULL;
			TCHAR szNickName[MAX_PATH] = {0};

			bReadFile = InternetReadFile(hOpenReq,DataBuffer,dwMaxDataLength,&dwReadLength);
			if (bReadFile && dwReadLength != 0)
			{
				pSearchPtr = strstr(DataBuffer,nickerror);
				if (pSearchPtr)
				{
					//昵称已经有人使用，后缀加随机数处理
					LARGE_INTEGER timeRand; 
					QueryPerformanceCounter(&timeRand);
					swprintf(szNickName,sizeof(szNickName),TEXT("%s%d"),lpwNickname,timeRand.LowPart);

					WriteCommentContentToServer(lpPasswordToken,
										lpDsid,
										lpSaleId,
										rating,
										szNickName,
										lpwTitle,
										lpwBody,
										pGUID,
										lpProxy,
										netType);

				}

			}
			else
			{

			}
		}

		//释放内存空间
		if (pNickname)
		{
			delete []pNickname;
			pNickname = NULL;
		}
		if (pTitle)
		{
			delete []pTitle;
			pTitle = NULL;
		}
		if (pBody)
		{
			delete []pBody;
			pBody = NULL;
		}

	}
	else
	{
		return FALSE;
	}

	//关闭连接请求
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

