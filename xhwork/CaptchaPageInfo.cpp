#include "CaptchaPageInfo.h"
#include "GZipHelper.h"
#include "Util.h"
#include "json/json.h"
#include <time.h>
#include <atltime.h>

#define	NDPD_S_TAG           TEXT("\"fd\":{\"s\":\"")
#define NDPD_F_TAG           TEXT("\",\"f\":\"")
#define NDPD_FM_TAG          TEXT("\",\"fm\":\"")
#define NDPD_W_TAG           TEXT("\",\"w\":\"")
#define NUCAPTCHA_TOKEN_TAG  TEXT("name=\"nucaptcha-token\" type=\"hidden\" value=\"")
#define NDPD_VK_TAG          TEXT("id=\"ndpd-vk\" name=\"ndpd-vk\" value=\"")
#define NDWK_URL_TAG         TEXT("\"wk\":{\"r\":\"")
#define NDWT_VALUE_TAG       TEXT("\\/w\\/\",\"w\":\"")
#define GET_CAPTCHA_TAG      TEXT("id=\"nucaptcha-media\" class=\"nucaptcha-media\" src=\"")
#define END_TAG              TEXT("\"")
CCaptchaPageInfo::CCaptchaPageInfo(void)
{
	m_strNdpd_s = TEXT("ndpd-s=");
	m_strNdpd_f = TEXT("ndpd-f=");
	m_strNdpd_fm = TEXT("ndpd-fm=");
	m_strNdpd_w = TEXT("ndpd-w=");
	m_strNdpd_ipr = TEXT("ndpd-ipr=");
	m_strNdpd_di = TEXT("ndpd-di=");
	m_strNdpd_bi = TEXT("ndpd-bi=");
	m_strNdpd_probe = TEXT("ndpd-probe=");
	m_strNdpd_af = TEXT("ndpd-af=");
	m_strNdpd_fv = TEXT("ndpd-fv=fv%2Cmp4");
	m_strNdpd_fa = TEXT("ndpd-fa=fa%2Cmpeg%2Cwav");
	m_strNdpd_bp = TEXT("ndpd-bp=p");
	m_strNdpd_wkr = TEXT("ndpd-wkr=");  //rand_range函数计算出来
	m_strNdpd_vk = TEXT("ndpd-vk=");
	m_strNdpd_wk = TEXT("ndpd-wk=");
	m_strNuCaptcha_token = TEXT("nucaptcha-token=");
}

CCaptchaPageInfo::~CCaptchaPageInfo(void)
{
}

int CCaptchaPageInfo::rand_range(int low, int hi)
{
	if (low >= hi)
		return low;
	else
		return rand() % (hi - low) + low;
}


CStringA CCaptchaPageInfo::_getDI(CStringA ua)
{
	CStringA di;

	di.AppendFormat("DI");
	di.AppendFormat(".");
	di.AppendFormat(ua);
	di.AppendFormat(".");
	di.AppendFormat(_getScreenFingerprint());
	di.AppendFormat(".");
	di.AppendFormat(_getDeviceTimezone());
	di.AppendFormat(".");
	di.AppendFormat(_getPlugins());

	CStringA d1;
	d1.Format("d1-%s",_quickHash(di.GetBuffer()) );

	return d1;
}

CStringA CCaptchaPageInfo::_getBI(CStringA ua, CStringA refer_url)
{
	CStringA bi;
	bi.AppendFormat("b2");
	bi.AppendFormat("|");
	bi.AppendFormat(_getScreenInfo());
	bi.AppendFormat("|");
	bi.AppendFormat(_getDeviceTimezone());
	bi.AppendFormat("|");
	bi.AppendFormat(_getDeviceLanguage());
	bi.AppendFormat("|");
	bi.AppendFormat(_get_bp1());
	bi.AppendFormat("|");
	bi.AppendFormat(_isFlashInstalled());
	bi.AppendFormat("|");
	bi.AppendFormat(refer_url);
	bi.AppendFormat("|");
	bi.AppendFormat(ua);
	bi.AppendFormat("|");
	bi.AppendFormat(_get_wg1());
	bi.Replace(" ","+");
	return bi;
}

CStringA CCaptchaPageInfo::_quickHash(char* pStrData)
{
	char q_hash[17];
	int b =0;
	int c = 0;
	memset(q_hash, 0, sizeof(q_hash));

	unsigned long len = strlen(pStrData);
	for (unsigned long i=0;i<len;i++)
	{
		char f = pStrData[i];
		if (i%2==0)
		{
			b = (b << 5) - b + f;
			b |= 0;
		}
		else
		{
			c = (c << 5) - c + f;
			c |= 0;
		}
	}

	if (b<0)
	{
		b = 4294967295 + b + 1;
	}

	if (c<0)
	{
		c = 4294967295 + c + 1;
	}
	sprintf(q_hash, "%x%x", b, c);
	CStringA ha(q_hash);
	return ha;
}

CStringA CCaptchaPageInfo::_get_wg1()
{
	//todo: simulate webglinfo
	srand((unsigned int)time(0));

	//return "wg1-35d0dfc944b2a132";
	CStringA d1;
	d1.Format("wg1-%x%x%x%x%x%x%x%x", (rand()%0xEE)+0x10, (rand()%0xEE)+0x10, (rand()%0xEE)+0x10, (rand()%0xEE)+0x10, (rand()%0xEE)+0x10, (rand()%0xEE)+0x10, (rand()%0xEE)+0x10, (rand()%0xEE)+0x10);
	return d1;
}

CStringA CCaptchaPageInfo::_getScreenFingerprint()
{
	return "320x568 32";
}

CStringA CCaptchaPageInfo::_getScreenInfo()
{
	return "320x568 320x548 32 32";
}

CStringA CCaptchaPageInfo::_getDeviceTimezone()
{
	return "-480";
}

CStringA CCaptchaPageInfo::_getDeviceLanguage()
{
	return "zh-cn";
}

CStringA CCaptchaPageInfo::_getPlugins()
{
	return "p";
}

CStringA CCaptchaPageInfo::_get_bp1()
{
	CStringA bp1;
	bp1.Format("bp1-%s",_quickHash( _getPlugins().GetBuffer() ).GetBuffer());
	return bp1;
}

CStringA CCaptchaPageInfo::_isFlashInstalled()
{
	return "false";
}

CStringA CCaptchaPageInfo::_getWebGLInfo()
{
	return "";
}

void CCaptchaPageInfo::ParseCaptchaPageData(char* lpSrcData)
{
	CString		strSrcData;
	CString		strTmp;
	int			beginPos = 0;
	int			endPos = 0;
	if (!lpSrcData)
	{
		return;
	}

	USES_CONVERSION;
	
	strSrcData = A2T(lpSrcData);

	//提取Ndpd_s
	beginPos = strSrcData.Find(NDPD_S_TAG);
	if (beginPos >= 0)
	{
		strTmp = strSrcData.Right(strSrcData.GetLength()-beginPos-wcslen(NDPD_S_TAG));

		endPos = strTmp.Find(END_TAG);
		if (endPos < 0)
		{
			return;
		}
		m_strNdpd_s += Util::URLEncode2(strTmp.Left(endPos));
	}

	//提取Ndpd_f
	beginPos = strSrcData.Find(NDPD_F_TAG);
	if (beginPos >= 0)
	{
		strTmp = strSrcData.Right(strSrcData.GetLength()-beginPos-wcslen(NDPD_F_TAG));

		endPos = strTmp.Find(END_TAG);
		if (endPos < 0)
		{
			return;
		}
		m_strNdpd_f += Util::URLEncode2(strTmp.Left(endPos));
	}

	//提取Ndpd_fm
	beginPos = strSrcData.Find(NDPD_FM_TAG);
	if (beginPos >= 0)
	{
		strTmp = strSrcData.Right(strSrcData.GetLength()-beginPos-wcslen(NDPD_FM_TAG));

		endPos = strTmp.Find(END_TAG);
		if (endPos < 0)
		{
			return;
		}
		m_strNdpd_fm += Util::URLEncode2(strTmp.Left(endPos));
	}

	//提取Ndpd_w
	beginPos = strSrcData.Find(NDPD_W_TAG);
	if (beginPos >= 0)
	{
		strTmp = strSrcData.Right(strSrcData.GetLength()-beginPos-wcslen(NDPD_W_TAG));

		endPos = strTmp.Find(END_TAG);
		if (endPos < 0)
		{
			return;
		}
		m_strNdpd_w += Util::URLEncode2(strTmp.Left(endPos));
	}

	//提取NuCaptcha_token
	beginPos = strSrcData.Find(NUCAPTCHA_TOKEN_TAG);
	if (beginPos >= 0)
	{
		strTmp = strSrcData.Right(strSrcData.GetLength()-beginPos-wcslen(NUCAPTCHA_TOKEN_TAG));

		endPos = strTmp.Find(END_TAG);
		if (endPos < 0)
		{
			return;
		}
		m_strNuCaptcha_token += Util::URLEncode2(strTmp.Left(endPos) + TEXT("|0|VIDEO|3||0|0"));
	}

	//提取获取 wk 值的连接地址
	beginPos = strSrcData.Find(NDWK_URL_TAG);
	if (beginPos >= 0)
	{
		strTmp = strSrcData.Right(strSrcData.GetLength()-beginPos-wcslen(NDWK_URL_TAG));
		endPos = strTmp.Find(END_TAG);
		if (endPos < 0)
		{
			return;
		}
		strTmp = strTmp.Left(endPos);

		beginPos = strTmp.Find(TEXT(".com"));
		m_strGetnudatasecurityUrl = strTmp.Left( beginPos + wcslen(TEXT(".com")) );
		m_strGetnudatasecurityUrl.Replace(TEXT("\\/"), TEXT("/"));

		m_strGetWkHostName = strTmp.Right(strTmp.GetLength() - beginPos - wcslen(TEXT(".com")) );
		m_strGetWkHostName.Replace(TEXT("\\/"), TEXT("/"));
		m_strGetWkHostName += TEXT("?");

	}

	//提取获取wk值需要发送的参数值wt
	beginPos = strSrcData.Find(NDWT_VALUE_TAG);
	if (beginPos >= 0)
	{
		strTmp = strSrcData.Right(strSrcData.GetLength()-beginPos-wcslen(NDWT_VALUE_TAG));
		endPos = strTmp.Find(END_TAG);
		if (endPos < 0)
		{
			return;
		}
		m_strGetWk_wt_Value = strTmp.Left(endPos);
	}

	//提取获取验证码的连接
	beginPos = strSrcData.Find(GET_CAPTCHA_TAG);
	if (beginPos >= 0)
	{
		strTmp = strSrcData.Right(strSrcData.GetLength()-beginPos-wcslen(GET_CAPTCHA_TAG));
		endPos = strTmp.Find(END_TAG);
		if (endPos < 0)
		{
			return;
		}
		strTmp = strTmp.Left(endPos);
		beginPos = strTmp.Find(TEXT(".com"));
		m_strGetCaptchaSecurityUrl = strTmp.Left( beginPos + wcslen(TEXT(".com")) );
		m_strGetCaptchaHostName = strTmp.Right(strTmp.GetLength() - beginPos - wcslen(TEXT(".com")) );

	}

	return;
}

CString CCaptchaPageInfo::GetClientTime()
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

BOOL CCaptchaPageInfo::SendNuDataValidateCaptchaSrv(CString strUserAgent,CString strRefererUrl,CString strCookie,CString strToken,CString strDsid,CString strGuid,CString strSaleId,CString strX_Apple_I_md_m,CString strX_Apple_I_md,CString strProxy,int netType)
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

	DWORD dwSize = 0;

	CString strUrl = _T("https://play.itunes.apple.com");
	CString strHostName = _T("/WebObjects/MZPlay.woa/wa/nuDataShowCaptchaSrv?X-Token=");
	strHostName += strToken;
	strHostName += _T("&guid=");
	strHostName += strGuid;
	strHostName += _T("&salableAdamId=");
	strHostName += strSaleId;

	BOOL bCrackUrl = ::InternetCrackUrl(strUrl,0,0,&uc);
	if (bCrackUrl)
	{
		if (netType == PROXY_NETWORK)
		{
			hSession = InternetOpen(strUserAgent,INTERNET_OPEN_TYPE_PROXY,strProxy,0,0);
		}
		else
		{
			hSession = InternetOpen(strUserAgent,0,0,0,0);
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

		CString strHeaders = _T("\r\nUser-Agent: ");
		strHeaders += strUserAgent;
		strHeaders += _T("\r\nAccept-Language: zh-Hans-CN");
		strHeaders += _T("\r\nX-Apple-Store-Front: 143465-19,29");
		strHeaders += _T("\r\nX-Apple-Connection-Type: WiFi");
		strHeaders += _T("\r\nX-Apple-Partner: origin.0");
		strHeaders += _T("\r\nAccept: */*");
		strHeaders += _T("\r\nX-Apple-I-Client-Time: ");
		strHeaders += GetClientTime();
		strHeaders += _T("\r\nOrigin: https://finance-app.itunes.apple.com");
		strHeaders += _T("\r\nReferer: ");
		strHeaders += strRefererUrl;
		strHeaders += _T("\r\nX-Apple-Partner: origin.0");
		strHeaders += _T("\r\nX-Dsid: ");
		strHeaders += strDsid;
		strHeaders += _T("\r\nAccept-Encoding: gzip, deflate");
		strHeaders += _T("\r\nX-Apple-I-MD-RINFO: 17106176");
		strHeaders += _T("\r\nX-Apple-I-MD-M: ");
		strHeaders += strX_Apple_I_md_m;
		strHeaders += _T("\r\nX-Apple-I-MD: ");
		strHeaders += strX_Apple_I_md;

		if (!strCookie.IsEmpty())
		{
			strHeaders += TEXT("\r\nCookie: ");
			strHeaders += strCookie;

		}

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

		if (dwQueryBuf == 200)
		{
			if (InternetQueryDataAvailable(hOpenReq,&dwSize,0,0))
			{
				//读取返回的数据
				DecodeResponseCaptchaInfoData(hOpenReq,dwSize,m_strDecodeResponseCaptchaData);
				//提取有用字段信息
				ParseCaptchaPageData((char*)m_strDecodeResponseCaptchaData.c_str());
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


void CCaptchaPageInfo::DecodeResponseCaptchaInfoData(HINTERNET hOpenReq,DWORD dwSrcSize,string& strDecodeData)
{
	LPGZIP			lpgzipBuf = NULL;
	DWORD			dataLength = 0;
	DWORD			dwReadLength = 0;
	DWORD			dwMaxDataLength = 200;
	BOOL			bReadFile = FALSE;
	char			DataBuffer[200] = {0};
	char*			pDecodeData = NULL;
	int				decodeLen = 0;


	//分配虚拟内存保存页面数据
	lpgzipBuf =  (LPGZIP)VirtualAlloc(NULL,dwSrcSize,MEM_COMMIT | MEM_RESERVE,PAGE_EXECUTE_READWRITE);//分配100K的虚拟内存
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
		strDecodeData = pDecodeData;
		
	}

	if (lpgzipBuf)
	{
		VirtualFree(lpgzipBuf,0,MEM_RELEASE);
		lpgzipBuf = NULL;
	}

	return;
}

BOOL CCaptchaPageInfo::SendAppleLoc(CString strUserAgent,CString strRefererUrl,CString strDsid,CString strCookie,CString strX_Apple_I_md_m,CString strX_Apple_I_md,CString strProxy,int netType)
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

	CString strUrl = _T("https://itunes.apple.com");
	CString strHostName = _T("/WebObjects/MZStore.woa/wa/appLoc");
	BOOL bCrackUrl = ::InternetCrackUrl(strUrl,0,0,&uc);
	if (bCrackUrl)
	{
		if (netType == PROXY_NETWORK)
		{
			hSession = InternetOpen(strUserAgent,INTERNET_OPEN_TYPE_PROXY,strProxy,0,0);
		}
		else
		{
			hSession = InternetOpen(strUserAgent,0,0,0,0);
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

		CString strHeaders = _T("\r\nUser-Agent: ");
		strHeaders += strUserAgent;
		strHeaders += _T("\r\nAccept-Language: zh-Hans-CN");
		strHeaders += _T("\r\nX-Apple-Store-Front: 143465-19,29");
		strHeaders += _T("\r\nX-Apple-Connection-Type: WiFi");
		strHeaders += _T("\r\nX-Apple-Partner: origin.0");
		strHeaders += _T("\r\nAccept: application/json, text/javascript, */*; q=0.01");
		strHeaders += _T("\r\nX-Apple-Tz: 28800");
		strHeaders += _T("\r\nX-Apple-I-Client-Time: ");
		strHeaders += GetClientTime();
		strHeaders += _T("\r\nOrigin: https://finance-app.itunes.apple.com");
		strHeaders += _T("\r\nReferer: ");
		strHeaders += strRefererUrl;
		strHeaders += _T("\r\nX-Apple-Partner: origin.0");
		strHeaders += _T("\r\nX-Dsid: ");
		strHeaders += strDsid;
		strHeaders += _T("\r\nAccept-Encoding: gzip, deflate");
		strHeaders += _T("\r\nX-Apple-I-MD-RINFO: 17106176");
		strHeaders += _T("\r\nX-Apple-I-MD-M: ");
		strHeaders += strX_Apple_I_md_m;
		strHeaders += _T("\r\nX-Apple-I-MD: ");
		strHeaders += strX_Apple_I_md;

		if (!strCookie.IsEmpty())
		{
			strHeaders += TEXT("\r\nCookie: ");
			strHeaders += strCookie;

		}

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


BOOL CCaptchaPageInfo::SendFinanceAppChallengeReport(CString strUserAgent,CString strXpReportUrl,CString strXpReportHostName,CString strDsid,CString strCookie,CString strX_Apple_I_md_m,CString strX_Apple_I_md,CString strProxy,int netType)
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


	BOOL bCrackUrl = ::InternetCrackUrl(strXpReportUrl,0,0,&uc);
	if (bCrackUrl)
	{
		if (netType == PROXY_NETWORK)
		{
			hSession = InternetOpen(strUserAgent,INTERNET_OPEN_TYPE_PROXY,strProxy,0,0);
		}
		else
		{
			hSession = InternetOpen(strUserAgent,0,0,0,0);
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

		hOpenReq = HttpOpenRequest(hConnect,_T("GET"),strXpReportHostName,0,0,0,0x84801000|INTERNET_FLAG_NO_COOKIES,0);//0x84A01000是一个常数,注意
		if (!hOpenReq)
		{
			InternetCloseHandle(hConnect);
			InternetCloseHandle(hSession);
			return FALSE;
		}

		CString strHeaders = _T("\r\nUser-Agent: ");
		strHeaders += strUserAgent;
		strHeaders += _T("\r\nAccept-Language: zh-Hans-CN");
		strHeaders += _T("\r\nX-Apple-Store-Front: 143465-19,29");
		strHeaders += _T("\r\nX-Apple-Connection-Type: WiFi");
		strHeaders += _T("\r\nX-Apple-Partner: origin.0");
		strHeaders += _T("\r\nAccept: text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8");
		strHeaders += _T("\r\nX-Apple-Tz: 28800");
		strHeaders += _T("\r\nX-Apple-I-Client-Time: ");
		strHeaders += GetClientTime();
		strHeaders += _T("\r\nX-Dsid: ");
		strHeaders += strDsid;
		strHeaders += _T("\r\nAccept-Encoding: gzip, deflate");
		strHeaders += _T("\r\nX-Apple-I-MD-RINFO: 17106176");
		strHeaders += _T("\r\nX-Apple-I-MD-M: ");
		strHeaders += strX_Apple_I_md_m;
		strHeaders += _T("\r\nX-Apple-I-MD: ");
		strHeaders += strX_Apple_I_md;

		if (!strCookie.IsEmpty())
		{
			strHeaders += TEXT("\r\nCookie: ");
			strHeaders += strCookie;

		}

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

void CCaptchaPageInfo::ReadNdpd_wkFromServer(HINTERNET hOpenReq,DWORD dwSrcSize)
{
	LPGZIP			lpgzipBuf = NULL;
	DWORD			dataLength = 0;
	DWORD			dwReadLength = 0;
	DWORD			dwMaxDataLength = 200;
	BOOL			bReadFile = FALSE;
	char			DataBuffer[200] = {0};
	char*			pDecodeData = NULL;
	int				decodeLen = 0;
	string          strDecodeData = "";
	Json::Reader	reader;  
	Json::Value		root; 

	//分配虚拟内存保存页面数据
	lpgzipBuf =  (LPGZIP)VirtualAlloc(NULL,dwSrcSize,MEM_COMMIT | MEM_RESERVE,PAGE_EXECUTE_READWRITE);//分配100K的虚拟内存
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
		if (pDecodeData)
		{
			CString strTmp;
			strDecodeData = pDecodeData;
			//字符串转为JSON格式存于root
			if ( reader.parse(strDecodeData,root) )
			{
				strTmp.Format(TEXT("%d"),root["wk"].asInt());
				m_strNdpd_wk += strTmp;
			}
		}
		
	}

	if (lpgzipBuf)
	{
		VirtualFree(lpgzipBuf,0,MEM_RELEASE);
		lpgzipBuf = NULL;
	}

	return;
}

BOOL CCaptchaPageInfo::SendNuDataSecurityGetWk(CString strUserAgent,CString strRefererUrl,CString strDsid,CString strCookie,CString strX_Apple_I_md_m,CString strX_Apple_I_md,CString strProxy,int netType)
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

	DWORD dwSize = 0;
	CString strUrl = m_strGetnudatasecurityUrl;
	CString strHostName = m_strGetWkHostName;
	strHostName += TEXT("r=");
	m_str_r_Value.Format(TEXT("%d"),rand_range(1e3, 1e6));
	strHostName += m_str_r_Value;
	strHostName += TEXT("&wt=");
	strHostName += m_strGetWk_wt_Value;

	BOOL bCrackUrl = ::InternetCrackUrl(strUrl,0,0,&uc);
	if (bCrackUrl)
	{
		if (netType == PROXY_NETWORK)
		{
			hSession = InternetOpen(strUserAgent,INTERNET_OPEN_TYPE_PROXY,strProxy,0,0);
		}
		else
		{
			hSession = InternetOpen(strUserAgent,0,0,0,0);
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

		CString strHeaders = _T("\r\nUser-Agent: ");
		strHeaders += strUserAgent;
		strHeaders += _T("\r\nAccept-Language: zh-Hans-CN");
		strHeaders += _T("\r\nX-Apple-Store-Front: 143465-19,29");
		strHeaders += _T("\r\nX-Apple-Connection-Type: WiFi");
		strHeaders += _T("\r\nX-Apple-Partner: origin.0");
		strHeaders += _T("\r\nAccept: application/json, text/javascript, */*; q=0.01");
		strHeaders += _T("\r\nX-Apple-Tz: 28800");
		strHeaders += _T("\r\nX-Apple-I-Client-Time: ");
		strHeaders += GetClientTime();
		strHeaders += _T("\r\nOrigin: https://finance-app.itunes.apple.com");
		strHeaders += _T("\r\nReferer: ");
		strHeaders += strRefererUrl;
		strHeaders += _T("\r\nX-Apple-Partner: origin.0");
		strHeaders += _T("\r\nX-Dsid: ");
		strHeaders += strDsid;
		strHeaders += _T("\r\nAccept-Encoding: gzip, deflate");
		strHeaders += _T("\r\nX-Apple-I-MD-RINFO: 17106176");
		strHeaders += _T("\r\nX-Apple-I-MD-M: ");
		strHeaders += strX_Apple_I_md_m;
		strHeaders += _T("\r\nX-Apple-I-MD: ");
		strHeaders += strX_Apple_I_md;

		if (!strCookie.IsEmpty())
		{
			strHeaders += TEXT("\r\nCookie: ");
			strHeaders += strCookie;

		}

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

		if (dwQueryBuf == 200)
		{
			//从服务器获取wk的值
			if (InternetQueryDataAvailable(hOpenReq,&dwSize,0,0))
			{
				ReadNdpd_wkFromServer(hOpenReq,dwSize);
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

BOOL CCaptchaPageInfo::SendGetCaptchaGifData(CString strUserAgent,CString strRefererUrl,CString strDsid,CString strCookie,CString strX_Apple_I_md_m,CString strX_Apple_I_md,CString strProxy,int netType)
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

	DWORD dwSize = 0;
	CString strUrl = m_strGetCaptchaSecurityUrl;
	CString strHostName = m_strGetCaptchaHostName;

	BOOL bCrackUrl = ::InternetCrackUrl(strUrl,0,0,&uc);
	if (bCrackUrl)
	{
		if (netType == PROXY_NETWORK)
		{
			hSession = InternetOpen(strUserAgent,INTERNET_OPEN_TYPE_PROXY,strProxy,0,0);
		}
		else
		{
			hSession = InternetOpen(strUserAgent,0,0,0,0);
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

		CString strHeaders = _T("\r\nUser-Agent: ");
		strHeaders += strUserAgent;
		strHeaders += _T("\r\nAccept-Language: zh-Hans-CN");
		strHeaders += _T("\r\nX-Apple-Store-Front: 143465-19,29");
		strHeaders += _T("\r\nX-Apple-Connection-Type: WiFi");
		strHeaders += _T("\r\nX-Apple-Partner: origin.0");
		strHeaders += _T("\r\nAccept: application/json, text/javascript, */*; q=0.01");
		strHeaders += _T("\r\nX-Apple-Tz: 28800");
		strHeaders += _T("\r\nX-Apple-I-Client-Time: ");
		strHeaders += GetClientTime();
		strHeaders += _T("\r\nOrigin: https://finance-app.itunes.apple.com");
		strHeaders += _T("\r\nReferer: ");
		strHeaders += strRefererUrl;
		strHeaders += _T("\r\nX-Apple-Partner: origin.0");
		strHeaders += _T("\r\nX-Dsid: ");
		strHeaders += strDsid;
		strHeaders += _T("\r\nAccept-Encoding: gzip, deflate");
		strHeaders += _T("\r\nX-Apple-I-MD-RINFO: 17106176");
		strHeaders += _T("\r\nX-Apple-I-MD-M: ");
		strHeaders += strX_Apple_I_md_m;
		strHeaders += _T("\r\nX-Apple-I-MD: ");
		strHeaders += strX_Apple_I_md;

		if (!strCookie.IsEmpty())
		{
			strHeaders += TEXT("\r\nCookie: ");
			strHeaders += strCookie;

		}

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

		if (dwQueryBuf == 200)
		{
			//从服务器获取wk的值
			if (InternetQueryDataAvailable(hOpenReq,&dwSize,0,0))
			{
				ReadCaptchaGifData(hOpenReq,dwSize);
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

void CCaptchaPageInfo::ReadCaptchaGifData(HINTERNET hOpenReq,DWORD dwSrcSize)
{
	DWORD			dataLength = 0;
	DWORD			dwReadLength = 0;
	DWORD			dwMaxDataLength = 200;
	BOOL			bReadFile = FALSE;
	char			DataBuffer[200] = {0};
	char*           pGifDataBuf = NULL;
	string          strGifCaptchaData = "";

	//分配虚拟内存保存页面数据
	pGifDataBuf =  (char*)VirtualAlloc(NULL,dwSrcSize,MEM_COMMIT | MEM_RESERVE,PAGE_EXECUTE_READWRITE);//分配100K的虚拟内存
	if (!pGifDataBuf)
	{		
		return;
	}
	//循环读取页面数据
	while (true)
	{
		bReadFile = InternetReadFile(hOpenReq,DataBuffer,dwMaxDataLength,&dwReadLength);
		if (bReadFile && dwReadLength != 0)
		{
			memcpy(pGifDataBuf + dataLength,DataBuffer,dwReadLength);
			dataLength += dwReadLength;														
			dwReadLength = 0;	

		}
		else
		{
			pGifDataBuf[dataLength] = '\0';
			break;
		}

	}//while end
	//字节流处理gif图片数据
	strGifCaptchaData = pGifDataBuf;
	::CreateStreamOnHGlobal(::GlobalAlloc(GMEM_ZEROINIT | GMEM_MOVEABLE, 0), TRUE, &m_StreamCaptcha);
	m_StreamCaptcha->Write(strGifCaptchaData.c_str(),strGifCaptchaData.length(),NULL);

	if (pGifDataBuf)
	{
		VirtualFree(pGifDataBuf,0,MEM_RELEASE);
		pGifDataBuf = NULL;
	}

	return;
}

void CCaptchaPageInfo::GetBase64FromStream(CStringA& base64, IStream* stream)
{
	LARGE_INTEGER lMove;
	ULARGE_INTEGER lLast;
	lMove.QuadPart = 0;
	lLast.QuadPart = 0;
	stream->Seek(lMove, STREAM_SEEK_END, &lLast);
	ULONGLONG length = lLast.QuadPart;
	unsigned char* data = (unsigned char*)malloc(lLast.QuadPart);
	stream->Seek(lMove, STREAM_SEEK_SET, &lLast);
	stream->Read(data, length, 0);
	base64 = Base64Encode(data, length);
	free(data);
}

void CCaptchaPageInfo::GetStringFromStream(CStringA& str, IStream* stream)
{
	if (stream == NULL)return;
	LARGE_INTEGER lMove;
	ULARGE_INTEGER lLast;
	lMove.QuadPart = 0;
	lLast.QuadPart = 0;
	stream->Seek(lMove, STREAM_SEEK_END, &lLast);
	ULONGLONG length = lLast.QuadPart;
	char* data = (char*)malloc(lLast.QuadPart);
	stream->Seek(lMove, STREAM_SEEK_SET, &lLast);
	stream->Read(data, length, 0);
	str.Append(data, length);
	free(data);
}

CStringA CCaptchaPageInfo::Base64Encode(const unsigned char* Data, int DataByte)
{
	//编码表
	const char EncodeTable[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
	//返回值
	CStringA strEncode;
	unsigned char Tmp[4] = { 0 };
	int LineLength = 0;
	for (int i = 0; i < (int)(DataByte / 3); i++)
	{
		Tmp[1] = *Data++;
		Tmp[2] = *Data++;
		Tmp[3] = *Data++;
		strEncode += EncodeTable[Tmp[1] >> 2];
		strEncode += EncodeTable[((Tmp[1] << 4) | (Tmp[2] >> 4)) & 0x3F];
		strEncode += EncodeTable[((Tmp[2] << 2) | (Tmp[3] >> 6)) & 0x3F];
		strEncode += EncodeTable[Tmp[3] & 0x3F];
		//	if(LineLength+=4,LineLength==76) {strEncode+="\r\n";LineLength=0;}
	}
	//对剩余数据进行编码
	int Mod = DataByte % 3;
	if (Mod == 1)
	{
		Tmp[1] = *Data++;
		strEncode += EncodeTable[(Tmp[1] & 0xFC) >> 2];
		strEncode += EncodeTable[((Tmp[1] & 0x03) << 4)];
		strEncode += "==";
	}
	else if (Mod == 2)
	{
		Tmp[1] = *Data++;
		Tmp[2] = *Data++;
		strEncode += EncodeTable[(Tmp[1] & 0xFC) >> 2];
		strEncode += EncodeTable[((Tmp[1] & 0x03) << 4) | ((Tmp[2] & 0xF0) >> 4)];
		strEncode += EncodeTable[((Tmp[2] & 0x0F) << 2)];
		strEncode += "=";
	}

	return strEncode;
}