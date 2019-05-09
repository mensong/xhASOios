#include "stdafx.h"
#include "urlcode.h"
#include "viewpage.h"
#include "GZipHelper.h"
#include "Util.h"
#include <iphlpapi.h>
#include <shlobj.h>
#include "seitunes.h"
#include "urlcode.h"
#include <iostream>
#include <vector>
#include <string>
#include <strsafe.h>
#include <algorithm>
#include "json.h"
#include <time.h>
#include <atltime.h>
#include "gzip.h"
using namespace std;
#pragma   comment   (lib,"ZLIB.LIB")
#pragma   comment   (lib,"shell32.lib")

#pragma warning(disable:4996)
#pragma warning(disable:4995)

#define    ALLOCATE_SIZE      4096
#define    MAX_TITLE          64
#define    MAX_ALLOCATE_SIZE  4096*2
#define    USER_AGENT         TEXT("AppStore/2.0 iOS/10.1.1 model/iPhone9,1 hwp/s5l9950x build/14B100 (6; dt:97)")//TEXT("AppStore/2.0 iOS/10.0.2 model/iPhone7,1 hwp/s5l8950x build/13F69 (6; dt:82)")
#define    UISTORE_USER_AGENT  TEXT("itunesstored/1.0 iOS/10.1.1 model/iPhone9,1 hwp/s5l9950x build/14B100 (6; dt:97)")//TEXT("itunesstored/1.0 iOS/10.0.2 model/iPhone7,1 hwp/s5l8950x build/13F69 (6; dt:82)")

char g_szMz_at[MAX_PATH] = {0};
char g_szMz_at_ssl[MAX_PATH] = {0};


CViewPage::CViewPage()
{
	s_FirstlocationPosition = 0;
	m_strFirstAppId = "";
	s_SecondlocationPosition = 0;
	m_strSecondAppId = "";
	s_locationPosition = 0;
	m_currentPos = 0;
	m_strPreAppId = "";
	s_NextlocationPosition = 0;
	m_strNextAppId = "";
	s_PrelocationPosition = 0;
	m_strNextAppName = "";
	m_strpreAppName = "";
	m_strFirstAppName = "";
	m_strSecondAppName = "";
	m_viewMainEnvironmentDataCenter = "";
	m_viewMainServerInstance = "";
	m_viewMainRevNum = "";

	m_termEnvironmentDataCenter = "";
	m_termServerInstance = "";
	m_termRevNum = "";
	m_termStoreFrontHeader = "";
	m_termPlatformName = "";
	m_termPlatformId = "";
	m_termPage = "";
	m_termPageId = "";
	m_strSerialNumber = "";
	m_strClientCorrelationKey = "";
	m_strPageRequestedTime = "";
	m_strTimeStamp = "";
	m_appleId = "";
	m_password = "";

	m_strAppleIDClientIdentifier = getLaunchCorrelationKey();

	LARGE_INTEGER timeRand;
	QueryPerformanceCounter(&timeRand);

	m_timeSpan = timeRand.LowPart%1000 - 300000;//300秒，以毫秒为单位


}
CViewPage::~CViewPage()
{
}

BOOL CViewPage::parseViewAppMainJsonData(char* lpSrcData)
{
	Json::Reader reader;  
	Json::Value root; 
	////字符串转为JSON格式存于root
	if ( !reader.parse(lpSrcData,root) )
	{
		return FALSE;
	}
	Json::Value pageData = root["pageData"];
	Json::Value metricsBase = pageData["metricsBase"];

	std::string strEnvironmentDataCenter = metricsBase["environmentDataCenter"].asString();
	std::string strServerInstance = metricsBase["serverInstance"].asString();
	std::string strStoreFrontHeader = metricsBase["storeFrontHeader"].asString();
	std::string strPlatformName = metricsBase["platformName"].asString();
	std::string strPlatformId = metricsBase["platformId"].asString();

	m_viewMainEnvironmentDataCenter = strEnvironmentDataCenter;
	m_viewMainServerInstance = strServerInstance;
	m_viewMainStoreFrontHeader = strStoreFrontHeader;
	m_viewMainPlatformName = strPlatformName;
	m_viewMaintPlatformId = strPlatformId;


	Json::Value properties = root["properties"];
	std::string strRevNum = properties["revNum"].asString();

	m_viewMainRevNum = strRevNum;

	return TRUE;
}

BOOL CViewPage::parseJsonData(char* lpSrcData,char* lpSaleId,char* lpAppExtVrsId)
{
	//解析json数据
	char strSaleId[MAX_TITLE] = {0}; 
	char strBuf[512] = "&pricingParameters=STDQ&pg=default&appExtVrsId=";
	char strEnd[] = "\",\"actionText\"";
	char strEnd2[] = "\",\"version\"";
	char tmpBuf[MAX_PATH] = {0};
	Json::Reader reader;  
	Json::Value root; 
	////字符串转为JSON格式存于root
	if ( !reader.parse(lpSrcData,root) )
	{
		return FALSE;
	}

	Json::Value pageData = root["pageData"];
	Json::Value metricsBase = pageData["metricsBase"];

	std::string strEnvironmentDataCenter = metricsBase["environmentDataCenter"].asString();
	std::string strPageDetails = metricsBase["pageDetails"].asString();
	std::string strServerInstance = metricsBase["serverInstance"].asString();
	std::string strStoreFrontHeader = metricsBase["storeFrontHeader"].asString();
	std::string strPlatformName = metricsBase["platformName"].asString();
	std::string strPlatformId = metricsBase["platformId"].asString();

	Json::Value properties = root["properties"];
	std::string strRevNum = properties["revNum"].asString();

	Json::Value storePlatformData = root["storePlatformData"];
	Json::Value product_dv_product = storePlatformData["product-dv-product"];
	if (product_dv_product.isNull())
	{
		product_dv_product = storePlatformData["product-dv"];
	}
	Json::Value results = product_dv_product["results"];

	strcpy(strSaleId,lpSaleId);
	Json::Value saleId = results[strSaleId];
	std::string strBunldeId = saleId["bundleId"].asString();
	std::string strName = saleId["name"].asString();

	//检索关键数据 搜索AppExtVrsId的值
	char* pPosBegin = strstr(lpSrcData,strBuf);
	if (!pPosBegin)
	{
		return FALSE;
	}
	pPosBegin = pPosBegin + strlen(strBuf);
	char* pPosEnd = strstr(pPosBegin,strEnd);
	if (!pPosEnd)
	{
		pPosEnd = strstr(pPosBegin,strEnd2);
		if (!pPosEnd)
		{
			return FALSE;
		}
	}

	int length = pPosEnd - pPosBegin;//获取长度
	memcpy(tmpBuf,pPosBegin,length);
	strcpy(lpAppExtVrsId,tmpBuf);

	m_targetAppAppExtVrsId = tmpBuf;
	m_targetAppEnvironmentDataCenter = strEnvironmentDataCenter;
	m_targetAppPageDetails = strPageDetails;
	m_targetAppServerInstance = strServerInstance;
	m_targetAppStoreFrontHeader = strStoreFrontHeader;
	m_targetAppPlatformName = strPlatformName;
	m_targetAppPlatformId = strPlatformId;
	m_targetAppRevNum = strRevNum;
	m_targetAppBunldeId = strBunldeId;
	m_targetAppName = strName;

	return TRUE;

}

BOOL CViewPage::parseRreAppJsonData(char* lpSrcData,char* lpSaleId,int appLocation)
{
	char strSaleId[MAX_TITLE] = {0}; 
	Json::Reader reader;  
	Json::Value root; 
	////字符串转为JSON格式存于root
	if ( !reader.parse(lpSrcData,root) )
	{
		return FALSE;
	}

	Json::Value storePlatformData = root["storePlatformData"];
	Json::Value product_dv = storePlatformData["product-dv"];
	Json::Value results = product_dv["results"];

	strcpy(strSaleId,lpSaleId);
	Json::Value saleId = results[strSaleId];
	std::string strName = saleId["name"].asString();
	switch(appLocation)
	{
	case FIRST_APP:
		{
			m_strFirstAppName = strName;
		}break;
	case SECOND_APP:
		{
			m_strSecondAppName = strName;
		}break;
	case PREV_APP:
		{
			m_strpreAppName = strName;
		}break;
	case NEXT_APP:
		{
			m_strNextAppName = strName;
		}break;
	default:
		break;
	}
	return TRUE;
}

BOOL CViewPage::parseJsonForLookupCaller(char* lpSrcData,char* lpSaleId)
{
	int currentPos = 0;
	BOOL bFind = FALSE;
	Json::Reader reader;  
	Json::Value root; 

	if (!lpSrcData)
	{
		return FALSE;
	}
	//字符串转为JSON格式存于root
	if ( !reader.parse(lpSrcData,root) )
	{
		return FALSE;
	}

	Json::Value pageData = root["pageData"];
	Json::Value bubbles = pageData["bubbles"];
	Json::Value arrayNull = bubbles[NULL];

	//获取数组长度
	int size = arrayNull["results"].size();

	//遍历数组
	for (int i=0; i<size; i++)//注意i从7开始，跳过第一次翻页的8个appId的值
	{
		Json::Value resultId = arrayNull["results"][i];
		std::string strId = resultId["id"].asString();

		if (i>7)
		{
			m_strVect.push_back(strId);
		}
		//查找指定的app Id
		if (strstr(lpSaleId,strId.c_str()))
		{
			bFind = TRUE;
			if (i<=7)
			{
				currentPos = 8;
			}
			else
			{
				currentPos = i+1;
			}
			break;
		}
	}

	//如果找到，向后再多遍历30个翻页app Id数据
	if (bFind)
	{
		for (int index=currentPos; index<currentPos+30; index++ )
		{
			Json::Value resultId = arrayNull["results"][index];
			std::string strId = resultId["id"].asString();

			m_strVect.push_back(strId);
		}
	}

	return TRUE;
}

BOOL CViewPage::parsejsonAppIdSortData(char* lpSrcData,char* lpSaleId)
{
	int currentPos = 0;
	BOOL bFind = FALSE;
	Json::Reader reader;  
	Json::Value root; 
	int j = 0;

	if (!lpSrcData)
	{
		return FALSE;
	}
	//字符串转为JSON格式存于root
	if ( !reader.parse(lpSrcData,root) )
	{
		return FALSE;
	}

	Json::Value pageData = root["pageData"];
	Json::Value bubbles = pageData["bubbles"];
	Json::Value arrayNull = bubbles[NULL];

	//获取数组长度
	int size = arrayNull["results"].size();

	if (lpSaleId && lpSaleId[0] != '\0')
	{
		//遍历数组
		for (int i=0; i<size; i++)
		{
			Json::Value resultId = arrayNull["results"][i];
			std::string strId = resultId["id"].asString();

			//////////////begin 2017-7-1  增加获取应用排名第1,2的appID的值
			if (i == 0)//排名第二名的appId
			{
				s_FirstlocationPosition = i;
				m_strFirstAppId = strId;

				//记录下一个的值
				j = i+1;
				Json::Value NextResultId = arrayNull["results"][j];
				std::string strNextAppId = NextResultId["id"].asString();
				m_strSecondAppId = strNextAppId;
				s_SecondlocationPosition = j;
			}


			//查找指定的app Id
			if (strstr(lpSaleId,strId.c_str()))
			{
				s_locationPosition = i;
				m_currentPos = i;
				//记录下一个的值
				j = i+1;
				Json::Value NextResultId = arrayNull["results"][j];
				std::string strNextAppId = NextResultId["id"].asString();
				m_strNextAppId = strNextAppId;
				s_NextlocationPosition = j;
				break;
			}
			else
			{
				s_PrelocationPosition = i;
				m_strPreAppId = strId;

			}
		}//end for

		//往下继续翻页多记录20个
		m_currentPos = m_currentPos + 20;
		for (int i=0; i < m_currentPos; i++)
		{
			Json::Value resultId = arrayNull["results"][i];
			std::string strId = resultId["id"].asString();
			m_vecIdsList.push_back(strId);
		}
	}

	Json::Value metricsBase = pageData["metricsBase"];
	std::string strEnvironmentDataCenter = metricsBase["environmentDataCenter"].asString();
	std::string strServerInstance = metricsBase["serverInstance"].asString();
	std::string strStoreFrontHeader = metricsBase["storeFrontHeader"].asString();
	std::string strPlatformName = metricsBase["platformName"].asString();
	std::string strPlatformId = metricsBase["platformId"].asString();
	std::string strPage = metricsBase["page"].asString();
	std::string strPageId = metricsBase["pageId"].asString();

	Json::Value properties = root["properties"];
	std::string strRevNum = properties["revNum"].asString();

	m_termEnvironmentDataCenter = strEnvironmentDataCenter;
	m_termServerInstance = strServerInstance;
	m_termRevNum = strRevNum;
	m_termStoreFrontHeader = strStoreFrontHeader;
	m_termPlatformName = strPlatformName;
	m_termPlatformId = strPlatformId;
	m_termPage = strPage;
	m_termPageId = strPageId;

	return TRUE;
}


BOOL CViewPage::parseCookieFile(TCHAR* strCookieFile,char* lpMz_at,char* lpMz_at_ssl,BOOL *lpbgetData)
{
	HANDLE		hCookieFile = NULL;
	DWORD		cookieSize = 0;
	DWORD		numOfByteRead = 0;
	char*       lpReadBuffer = NULL;
	char*		ptrBegin = NULL;
	char*		ptrEnd = NULL;
	char		szMz_atValue[MAX_PATH] = {0};
	char		szMz_at_sslValue[MAX_PATH] = {0};


	hCookieFile = CreateFileW(strCookieFile, 
		GENERIC_READ,
		FILE_SHARE_READ,
		NULL,
		OPEN_EXISTING,
		0,
		NULL);
	if (!hCookieFile)
	{
		return FALSE;
	}

	cookieSize = GetFileSize(hCookieFile,NULL);

	lpReadBuffer = new char[cookieSize+1];
	if (!lpReadBuffer)
	{
		CloseHandle(hCookieFile);
		return FALSE;
	}
	memset(lpReadBuffer,0,cookieSize+1);

	if ( !ReadFile(hCookieFile,lpReadBuffer,cookieSize,&numOfByteRead,NULL) )
	{
		CloseHandle(hCookieFile);
		return FALSE;
	}

	//提取字符串
	ptrBegin = strstr(lpReadBuffer,lpMz_at);
	if (!ptrBegin)
	{
		if (lpReadBuffer)
		{
			delete []lpReadBuffer;
			lpReadBuffer = NULL;
		}
		CloseHandle(hCookieFile);
		return FALSE;
	}
	ptrBegin = ptrBegin + strlen(lpMz_at) + 1; //加'\n'的长度,然后取到值
	ptrEnd = strchr(ptrBegin,'\n');
	memcpy(szMz_atValue,ptrBegin,ptrEnd-ptrBegin);

	ptrBegin = strstr(lpReadBuffer,lpMz_at_ssl);
	if (!ptrBegin)
	{
		if (lpReadBuffer)
		{
			delete []lpReadBuffer;
			lpReadBuffer = NULL;
		}
		CloseHandle(hCookieFile);
		return FALSE;
	}
	ptrBegin = ptrBegin + strlen(lpMz_at_ssl) + 1; //加'\n'的长度,然后取到值
	ptrEnd = strchr(ptrBegin,'\n');
	memcpy(szMz_at_sslValue,ptrBegin,ptrEnd-ptrBegin);

	strcpy(g_szMz_at,lpMz_at);
	strcat(g_szMz_at,"=");
	strcat(g_szMz_at,szMz_atValue);

	strcpy(g_szMz_at_ssl,lpMz_at_ssl);
	strcat(g_szMz_at_ssl,"=");
	strcat(g_szMz_at_ssl,szMz_at_sslValue);

	if (lpReadBuffer)
	{
		delete []lpReadBuffer;
		lpReadBuffer = NULL;
	}

	CloseHandle(hCookieFile);

	return TRUE;
}

BOOL CViewPage::getConnectCookie(char* lpDsid)
{
	TCHAR				szPath[MAX_PATH] = {0};
	WIN32_FIND_DATAW	find_data = {0};
	HANDLE				hFind = NULL;
	char strMz_at[MAX_PATH] = "mz_at0-";
	char strMz_at_ssl[MAX_PATH] = "mz_at_ssl-";

	//连接
	strcat(strMz_at,lpDsid);
	strcat(strMz_at_ssl,lpDsid);

	if (SHGetSpecialFolderPath(NULL, szPath, CSIDL_COOKIES, FALSE))
	{
		CString strWildcard(szPath);
		strWildcard += _T("\\*.*");

		//开始cookie文件夹遍历
		hFind = FindFirstFile(strWildcard, &find_data);
		if (hFind == INVALID_HANDLE_VALUE)
		{
			return FALSE;
		}

		do 
		{
			if (find_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) 
			{
				continue;
			}
			CString strTmp(szPath);
			strTmp += _T("\\");
			strTmp += find_data.cFileName;
			if (strTmp.Find(_T(".txt")) < 0 )
			{
				//continue;
			}
			//解析Cookie,提取mz_at0-*字段的值和mz_at_ssl-*字段的值
			BOOL bgetData = FALSE;
			parseCookieFile(strTmp.GetBuffer(),strMz_at,strMz_at_ssl,&bgetData);
			if (bgetData)
			{
				break;
			}

		} while (FindNextFileW(hFind, &find_data));

		FindClose(hFind);

	}

	return TRUE;
}

BOOL CViewPage::sendCheckAppIdPageDataToServer(CString strUserAgent,
											   char* lpDsid,
											   char* lpToken,
											   char* lpMfz_inst,
											   CString strCookie,
											   TCHAR* lpX_Apple_I_md_m,
											   TCHAR* lpX_Apple_I_md,
											   TCHAR* lpProxy,
											   int netType)
{
	char			strSendAppIdData[ALLOCATE_SIZE] = {0};
	char			strLockupData[ALLOCATE_SIZE] = {0};
	unsigned char	strEncodeData[ALLOCATE_SIZE] = {0};
	char			strCookieData[512] = {0};
	char			strTimeStamp[64] = {0}; 
	int				sendAppIdCounts = 12;
	vector<__int64> vecTmp;

	for (int i=0; i<sendAppIdCounts; i++)
	{
		__int64 appId = _atoi64(m_strVect.at(i).c_str());
		vecTmp.push_back(appId);
	}
	//从小到大排序
	sort(vecTmp.begin(),vecTmp.end());

	//第一次翻页发送12个app Id
	for (int i=0; i<sendAppIdCounts; i++)
	{
		char strAppId[64] = {0};
		_i64toa(vecTmp.at(i),strAppId,10);
		strcat(strSendAppIdData,strAppId);
		if (i != sendAppIdCounts-1)
		{
			strcat(strSendAppIdData,",");
		}
	}

	if ( !getConnectCookie(lpDsid) )
	{
		return FALSE;
	}

	//这里自己设置cookie发送
	strcpy(strCookieData,g_szMz_at);
	strcat(strCookieData,"; ");
	strcat(strCookieData,g_szMz_at_ssl);
	strcat(strCookieData,"; ");
	strcat(strCookieData,lpMfz_inst);
	strcat(strCookieData,"; ");

	if ( !clientapiLookup(strUserAgent,strSendAppIdData,lpDsid,lpToken,strCookieData,lpProxy,netType) )
	{
		return FALSE;
	}
	urlencode((unsigned char*)strSendAppIdData,strlen(strSendAppIdData),strEncodeData,ALLOCATE_SIZE);
	sprintf(strLockupData,"native-search-lockup=%s",strEncodeData);
	if ( !sendBuyButtonMetaData(strUserAgent,strLockupData,lpDsid,lpToken,strCookie,lpX_Apple_I_md_m,lpX_Apple_I_md,lpProxy,netType) )
	{
		return FALSE;
	}

	//后面每次发送20个app id

	return TRUE;
}

BOOL CViewPage::ReadStringToUnicode(CString &str,int type)
{
	char *szBuf = new char[ str.GetLength()+1]; //数量要加1

	for (int i = 0; i<str.GetLength(); i++)
	{
		szBuf[i] = (CHAR)str.GetAt(i);
	}  
	szBuf[str.GetLength()]='\0';   //这里，必须要加上，否则会在结尾片显示乱码

	BOOL bok= CharToUnicode(szBuf,type, &str);
	if (szBuf)
	{
		delete []szBuf;
		szBuf = NULL;
	}

	return bok;
}

// 将Char型字符转换为Unicode字符
int CViewPage::CharToUnicode(char *pchIn,int type,CString *pstrOut)
{
	int nLen;
	WCHAR *ptch;

	if(pchIn == NULL)
	{
		return 0;
	}

	nLen = MultiByteToWideChar(type, 0, pchIn, -1, NULL, 0);//取得所需缓存的多少
	//申请缓存空间
	ptch = new WCHAR[nLen];
	MultiByteToWideChar(type,0,pchIn,-1,ptch,nLen);//转码
	pstrOut->Format(_T("%s"), ptch);
	if (ptch)
	{
		delete [] ptch;

		ptch = NULL;
	}

	return nLen;
}

int CViewPage::ConvertUtf8ToGBK(char* pChangeStr,int nLength)
{
	int nLen = nLength*2;

	LPWSTR lpwsz = new WCHAR[nLen+2]; 
	if( lpwsz == NULL)
	{
		return 0;
	}
	MultiByteToWideChar( CP_UTF8, 0, pChangeStr, -1, lpwsz, nLen );//转换的结果是UCS2格式
	int nLen1 = WideCharToMultiByte( CP_ACP, 0, lpwsz, -1, pChangeStr, nLen, NULL, NULL );//转换完毕

	delete [] lpwsz; 
	*(pChangeStr + nLen1) = '\0';

	return nLen1;
}

BOOL CViewPage::getPreAndNextAppName(CString strUserAgent,char* lpAppId,int appLocation)
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

	TCHAR strAppId[MAX_PATH] = {0};

	LPGZIP lpgzipBuf = NULL;

	CString strUrl = _T("https://itunes.apple.com");
	CString strAgent = strUserAgent;
	CString strHostName = _T("/cn/app/id");

	::MultiByteToWideChar(CP_ACP,0,lpAppId,strlen(lpAppId),strAppId,MAX_PATH);

	strHostName += strAppId;

	BOOL bCrackUrl = ::InternetCrackUrl(strUrl,0,0,&uc);
	if (bCrackUrl)
	{

		hSession = InternetOpen(strAgent,0,0,0,0);
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

		hOpenReq = HttpOpenRequest(hConnect,_T("GET"),strHostName,0,0,0,0x84801000,0);//0x84A01000是一个常数,注意
		if (!hOpenReq)
		{
			InternetCloseHandle(hConnect);
			InternetCloseHandle(hSession);
			return FALSE;
		}

		CString strHeaders = _T("User-Agent: ");
		strHeaders += strAgent;
		strHeaders += _T("\r\nAccept-Language: zh-cn");
		strHeaders += _T("\r\nX-Apple-Store-Front: 143465-19,29");
		strHeaders += _T("\r\nAccept-Encoding: gzip, deflate\r\nConnection: keep-alive\r\n\r\n");

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
			///分配虚拟内存保存页面数据
			DWORD dataLength = 0;
			lpgzipBuf =  (LPGZIP)VirtualAlloc(NULL,1024*1300,MEM_COMMIT | MEM_RESERVE,PAGE_EXECUTE_READWRITE);//分配800K的虚拟内存
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
				if (!pDecodeData)
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

				//解析提取出app的名字
				parseRreAppJsonData(pDecodeData,lpAppId,appLocation);

				if (lpgzipBuf)
				{
					VirtualFree(lpgzipBuf,0,MEM_RELEASE);
					lpgzipBuf = NULL;
				}
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

BOOL CViewPage::openViewAppMain(CString strUserAgent,
								char* strPasswordToken,
								char* lpDsid,
								CString strCookie,
								TCHAR* lpX_Apple_I_md_m,
								TCHAR* lpX_Apple_I_md,
								TCHAR* lpProxy,
								int netType)
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

	LPGZIP lpgzipBuf = NULL;
	TCHAR strDsid[MAX_PATH] = {0};

	CString strUrl = _T("https://itunes.apple.com");
	CString strAgent = strUserAgent;
	TCHAR strHostName[MAX_PATH] = _T("/WebObjects/MZStore.woa/wa/viewGrouping?cc=cn&id=29099");

	if (lpDsid && lpDsid[0] != '\0')
	{
		MultiByteToWideChar(CP_ACP,0,(LPCSTR)lpDsid,strlen(lpDsid),strDsid,MAX_PATH);
	}

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

		CString strHeaders = _T("User-Agent: ");
		strHeaders += strAgent;
		strHeaders += _T("\r\nAccept-Language: zh-cn");
		strHeaders += _T("\r\nX-Apple-Store-Front: 143465-19,29");
		strHeaders += _T("\r\nAccept: */*");
		if (lpDsid && lpDsid[0] != '\0')
		{
			strHeaders += _T("\r\nX-Dsid: ");
			strHeaders += strDsid;
		}

		strHeaders += TEXT("\r\nX-Apple-Connection-Type: WiFi");
		if (lpX_Apple_I_md_m && lpX_Apple_I_md_m[0] != TEXT('\0'))
		{
			strHeaders += _T("\r\nX-Apple-I-MD-RINFO: 17106176");
			strHeaders += _T("\r\nX-Apple-I-MD-M: ");
			strHeaders += lpX_Apple_I_md_m;
			strHeaders += _T("\r\nX-Apple-I-MD: ");
			strHeaders += lpX_Apple_I_md;
		}
		strHeaders += TEXT("\r\nX-Apple-I-Client-Time: ");
		strHeaders += GetClientTime();

		if (!strCookie.IsEmpty())
		{
			strHeaders += TEXT("\r\nCookie: ");
			strHeaders += strCookie;

		}

		strHeaders += _T("\r\nX-Apple-Tz: 28800");
		strHeaders += _T("\r\nAccept-Encoding: gzip, deflate\r\nConnection: keep-alive\r\n\r\n");

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

			//分配虚拟内存保存页面数据
			DWORD dataLength = 0;
			lpgzipBuf =  (LPGZIP)VirtualAlloc(NULL,1024*1300,MEM_COMMIT | MEM_RESERVE,PAGE_EXECUTE_READWRITE);//分配800K的虚拟内存
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
				if (!pDecodeData)
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
				//提取需要的字段
				parseViewAppMainJsonData(pDecodeData);
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

BOOL CViewPage::appBootStrap(CString strUserAgent,char* lpPasswordToken,char* lpDsid,CString strCookie,TCHAR* lpX_Apple_I_md_m,TCHAR* lpX_Apple_I_md,TCHAR* lpProxy,int netType)
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

	TCHAR strDsid[MAX_PATH] = {0};

	CString strUrl = _T("https://itunes.apple.com");
	CString strAgent = strUserAgent;
	TCHAR strHostName[MAX_PATH] = _T("/WebObjects/MZStore.woa/wa/appBootstrap");
	if (lpDsid && lpDsid[0] != '\0')
	{
		MultiByteToWideChar(CP_ACP,0,(LPCSTR)lpDsid,strlen(lpDsid),strDsid,MAX_PATH);
	}

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

		CString strHeaders = _T("User-Agent: ");
		strHeaders += strUserAgent;
		strHeaders += _T("\r\nAccept-Language: zh-Hans");
		strHeaders += _T("\r\nAccept: */*");
		strHeaders += _T("\r\nX-Apple-Connection-Type: WiFi");
		strHeaders += _T("\r\nX-Apple-Store-Front: 143465-19,29");
		strHeaders += _T("\r\nX-Apple-I-Client-Time: ");
		strHeaders += GetClientTime();
		if (lpX_Apple_I_md && lpX_Apple_I_md[0] != TEXT('\0'))
		{
			strHeaders += _T("\r\nX-Apple-I-MD-RINFO: 17106176");
			strHeaders += _T("\r\nX-Apple-I-MD-M: ");
			strHeaders += lpX_Apple_I_md_m;
			strHeaders += _T("\r\nX-Apple-I-MD: ");
			strHeaders += lpX_Apple_I_md;
		}
		if (lpDsid && lpDsid[0] != '\0')
		{
			strHeaders += _T("\r\nX-Dsid: ");
			strHeaders += strDsid;
		}

		if (!strCookie.IsEmpty())
		{
			strHeaders += TEXT("\r\nCookie: ");
			strHeaders += strCookie;

		}
		strHeaders += _T("\r\nX-Apple-Tz: 28800");
		strHeaders += _T("\r\nProxy-Connection: keep-alive");
		strHeaders += _T("\r\nAccept-Encoding: gzip, deflate\r\nConnection: keep-alive\r\n\r\n");

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

BOOL CViewPage::searchHotWordHint(CString strUserAgent,LPTSTR pSearchHotWord,char* lpPasswordToken,char* lpDsid,CString strCookie,TCHAR* lpX_Apple_I_md_m,TCHAR* lpX_Apple_I_md,TCHAR* lpProxy,int netType)
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

	LPGZIP lpgzipBuf = NULL;
	TCHAR strDsid[MAX_PATH] = {0};

	char szFormatTime[MAX_PATH] = {0};
	TCHAR wzFormatTime[512] = {0};

	CString strUrl = _T("https://search.itunes.apple.com");
	CString strAgent = strUserAgent;
	unsigned char* pBuf = NULL;
	unsigned char encodeKeyword[MAX_PATH];
	int wHotwordLen = wcslen(pSearchHotWord);
	int strLen = WideCharToMultiByte(CP_UTF8,0,pSearchHotWord,wHotwordLen,NULL,0,NULL,NULL);
	pBuf = new unsigned char[strLen+1];
	if (!pBuf)
	{
		return FALSE;
	}
	memset(pBuf,'\0',strLen+1);

	WideCharToMultiByte(CP_UTF8,0,pSearchHotWord,wHotwordLen,(LPSTR)pBuf,strLen,NULL,NULL);
	if (lpDsid && lpDsid[0] != '\0')
	{
		MultiByteToWideChar(CP_ACP,0,(LPCSTR)lpDsid,strlen(lpDsid),strDsid,MAX_PATH);
	}

	//Title url编码
	urlencode(pBuf,strLen,encodeKeyword,MAX_TITLE);

	TCHAR strPasswordToken[MAX_PATH] = {0};
	TCHAR strHostName[MAX_PATH] = {0};
	char buf[MAX_PATH] = {0};
	sprintf(buf,"/WebObjects/MZSearchHints.woa/wa/hints?clientApplication=Software&term=%s",encodeKeyword);
	MultiByteToWideChar(CP_ACP,0,buf,strlen(buf),strHostName,MAX_PATH);
	if (lpPasswordToken && lpPasswordToken[0] != '\0')
	{
		MultiByteToWideChar(CP_ACP,0,(LPCSTR)lpPasswordToken,strlen((char*)lpPasswordToken),strPasswordToken,MAX_PATH);
	}

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

		getFormatTime(szFormatTime);
		::MultiByteToWideChar(CP_ACP,0,szFormatTime,strlen(szFormatTime),wzFormatTime,512);

		CString strHeaders = _T("Accept: */*");
		strHeaders += _T("\r\nUser-Agent: ");
		strHeaders += strAgent;
		if (lpPasswordToken && lpPasswordToken[0] != '\0')
		{
			strHeaders += _T("\r\nX-Token: ");
			strHeaders += strPasswordToken;
		}
		if (lpDsid && lpDsid[0] != '\0')
		{
			strHeaders += _T("\r\nX-Dsid: ");
			strHeaders += strDsid;
		}
		strHeaders += _T("\r\nAccept-Language: zh-cn");
		strHeaders += _T("\r\nX-Apple-Store-Front: 143465-19,29");
		strHeaders += _T("\r\nAccept-Encoding: gzip, deflate");
		strHeaders += _T("\r\nX-Apple-I-Client-Time: ");
		strHeaders += wzFormatTime;
		if (lpX_Apple_I_md_m && lpX_Apple_I_md_m[0] != TEXT('\0'))
		{
			strHeaders += _T("\r\nX-Apple-I-MD-RINFO: 17106176");
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
		strHeaders += _T("\r\nX-Apple-Tz: 28800");
		strHeaders += _T("\r\nX-Apple-Partner: origin.0");
		strHeaders += _T("\r\nProxy-Connection: keep-alive");
		strHeaders += _T("\r\nConnection: keep-alive\r\n\r\n");

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

BOOL CViewPage::searchHotWordSubmit(CString strUserAgent,
									LPTSTR pSearchHotWord,
									char* lpSaleId,
									char* lpDsid,
									char* lpPasswordToken,
									char* lpPageSearchUrl,
									CString strCookie,
									TCHAR* lpX_Apple_I_md_m,
									TCHAR* lpX_Apple_I_md,
									TCHAR* lpProxy,
									int netType)
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

	LPGZIP lpgzipBuf = NULL;
	TCHAR strPasswordToken[MAX_PATH] = {0};
	TCHAR strHostName[MAX_PATH] = {0};
	char szPageSearchUrl[MAX_PATH] = {0};
	char buf[MAX_PATH] = {0};
	TCHAR strPageUrl[MAX_PATH] = {0};
	TCHAR strDsid[MAX_PATH] = {0};
	TCHAR szCookieData[512] = {0};
	DWORD dwSize = 512;

	char szFormatTime[MAX_PATH] = {0};
	TCHAR wzFormatTime[512] = {0};

	CString strUrl = _T("https://search.itunes.apple.com");
	CString strAgent = strUserAgent;
	unsigned char* pBuf = NULL;
	unsigned char encodeKeyword[MAX_PATH];
	int wHotwordLen = wcslen(pSearchHotWord);
	int strLen = WideCharToMultiByte(CP_UTF8,0,pSearchHotWord,wHotwordLen,NULL,0,NULL,NULL);
	pBuf = new unsigned char[strLen+1];
	if (!pBuf)
	{
		return FALSE;
	}
	memset(pBuf,'\0',strLen+1);

	WideCharToMultiByte(CP_UTF8,0,pSearchHotWord,wHotwordLen,(LPSTR)pBuf,strLen,NULL,NULL);
	if (lpDsid && lpDsid[0] != '\0')
	{
		MultiByteToWideChar(CP_ACP,0,(LPCSTR)lpDsid,strlen(lpDsid),strDsid,MAX_PATH);
	}

	//Title url编码
	urlencode(pBuf,strLen,encodeKeyword,MAX_TITLE);

	if (pBuf)
	{
		delete []pBuf;
		pBuf = NULL;
	}


	sprintf(buf,"/WebObjects/MZStore.woa/wa/search?clientApplication=Software&term=%s&caller=com.apple.AppStore&version=1",encodeKeyword);
	sprintf(szPageSearchUrl,"https://search.itunes.apple.com/WebObjects/MZStore.woa/wa/search?clientApplication=Software&term=%s",encodeKeyword);

	memcpy(lpPageSearchUrl,szPageSearchUrl,strlen(szPageSearchUrl));

	MultiByteToWideChar(CP_ACP,0,buf,strlen(buf),strHostName,MAX_PATH);
	if (lpPasswordToken && lpPasswordToken[0] != '\0')
	{
		MultiByteToWideChar(CP_ACP,0,(LPCSTR)lpPasswordToken,strlen((char*)lpPasswordToken),strPasswordToken,MAX_PATH);
	}

	MultiByteToWideChar(CP_ACP,0,(LPCSTR)szPageSearchUrl,strlen(szPageSearchUrl),strPageUrl,MAX_PATH);


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

		getFormatTime(szFormatTime);
		::MultiByteToWideChar(CP_ACP,0,szFormatTime,strlen(szFormatTime),wzFormatTime,512);

		CString strHeaders = _T("User-Agent: ");
		strHeaders += strAgent;
		if (lpPasswordToken && lpPasswordToken[0] != '\0')
		{
			strHeaders += _T("\r\nX-Token: ");
			strHeaders += strPasswordToken;
		}
		if (lpDsid && lpDsid[0] != '\0')
		{
			strHeaders += _T("\r\nX-Dsid: ");
			strHeaders += strDsid;
		}
		strHeaders += _T("\r\nAccept: */*");
		strHeaders += _T("\r\nX-Apple-Connection-Type: WiFi");
		strHeaders += _T("\r\nX-Apple-Store-Front: 143465-19,29");
		strHeaders += _T("\r\nX-Apple-Client-Versions: GameCenter/2.0");
		strHeaders += _T("\r\nAccept-Language: zh-Hans");
		strHeaders += _T("\r\nX-Apple-I-Client-Time: ");
		strHeaders += wzFormatTime;

		if (lpX_Apple_I_md_m && lpX_Apple_I_md_m[0] != TEXT('\0'))
		{
			strHeaders += _T("\r\nX-Apple-I-MD-RINFO: 17106176");
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
		strHeaders += _T("\r\nX-Apple-Tz: 28800");
		strHeaders += _T("\r\nAccept-Encoding: gzip, deflate");
		strHeaders += _T("\r\nX-Apple-Partner: origin.0");
		strHeaders += _T("\r\nConnection: keep-alive\r\n\r\n");

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
			///分配虚拟内存保存页面数据
			DWORD dataLength = 0;
			lpgzipBuf =  (LPGZIP)VirtualAlloc(NULL,1024*1300,MEM_COMMIT | MEM_RESERVE,PAGE_EXECUTE_READWRITE);//分配800K的虚拟内存
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
				if (!pDecodeData)
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

				//解析数据，得到需要的字段
				parsejsonAppIdSortData(pDecodeData,lpSaleId);


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

BOOL CViewPage::showInfoAccountSummary(CString strUserAgent,
									   char* lpDsid,
									   char* lpPasswordToken,
									   CString strCookie,
									   TCHAR* lpX_Apple_I_md_m,
									   TCHAR* lpX_Apple_I_md,
									   TCHAR* lpProxy,
									   int netType)
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

	LPGZIP lpgzipBuf = NULL;
	TCHAR strPasswordToken[MAX_PATH] = {0};
	TCHAR szCookieData[512] = {0};
	DWORD dwSize = 512;

	CString strHostName;

	char szFormatTime[MAX_PATH] = {0};
	TCHAR wzFormatTime[512] = {0};

	USES_CONVERSION;

	CString strUrl = _T("https://p37-buy.itunes.apple.com");
	CString strAgent = strUserAgent;

	if (lpPasswordToken && lpPasswordToken[0] != '\0')
	{
		MultiByteToWideChar(CP_ACP,0,(LPCSTR)lpPasswordToken,strlen((char*)lpPasswordToken),strPasswordToken,MAX_PATH);
	}

	strHostName = _T("/WebObjects/MZFinance.woa/wa/accountSummary?guid=");
	strHostName += A2T(m_strUdid.c_str());

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

		getFormatTime(szFormatTime);
		::MultiByteToWideChar(CP_ACP,0,szFormatTime,strlen(szFormatTime),wzFormatTime,512);

		CString strHeaders = _T("User-Agent: ");
		strHeaders += strAgent;
		if (lpPasswordToken && lpPasswordToken[0] != '\0')
		{
			strHeaders += _T("\r\nX-Token: ");
			strHeaders += strPasswordToken;
		}
		if (lpDsid && lpDsid[0] != '\0')
		{
			strHeaders += _T("\r\nX-Dsid: ");
			strHeaders += A2T(lpDsid);
		}
		strHeaders += _T("\r\nAccept: */*");
		strHeaders += _T("\r\nX-Apple-Connection-Type: WiFi");
		strHeaders += _T("\r\nX-Apple-Store-Front: 143465-19,29");
		strHeaders += _T("\r\nX-Apple-Client-Versions: GameCenter/2.0");
		strHeaders += _T("\r\nAccept-Language: zh-Hans");
		strHeaders += _T("\r\nX-Apple-I-Client-Time: ");
		strHeaders += wzFormatTime;

		if (lpX_Apple_I_md_m && lpX_Apple_I_md_m[0] != TEXT('\0'))
		{
			strHeaders += _T("\r\nX-Apple-I-MD-RINFO: 17106176");
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
		strHeaders += _T("\r\nX-Apple-Tz: 28800");
		strHeaders += _T("\r\nAccept-Encoding: gzip, deflate");
		strHeaders += _T("\r\nX-Apple-Partner: origin.0");
		strHeaders += _T("\r\nConnection: keep-alive\r\n");

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
			///分配虚拟内存保存页面数据
			DWORD dataLength = 0;
			lpgzipBuf =  (LPGZIP)VirtualAlloc(NULL,1024*1300,MEM_COMMIT | MEM_RESERVE,PAGE_EXECUTE_READWRITE);//分配800K的虚拟内存
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
				char * pDecodeData = gzip2A.psz;//解密出来的网页数据指针
				if (!pDecodeData)
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

				//解析数据，得到账户余额
				ParseAccountBlanceData(UTF8ToGBK(pDecodeData));


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

BOOL CViewPage::showInfoAccountFieldsSrv(CString strUserAgent,
										 char* lpDsid,
										 char* lpPasswordToken,
										 CString strCookie,
										 TCHAR* lpX_Apple_I_md_m,
										 TCHAR* lpX_Apple_I_md,
										 TCHAR* lpProxy,
										 int netType)
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

	LPGZIP lpgzipBuf = NULL;
	TCHAR strPasswordToken[MAX_PATH] = {0};
	TCHAR szCookieData[512] = {0};
	DWORD dwSize = 512;

	USES_CONVERSION;

	char szFormatTime[MAX_PATH] = {0};
	TCHAR wzFormatTime[512] = {0};

	CString strUrl = _T("https://p37-buy.itunes.apple.com");
	CString strAgent = strUserAgent;

	CString strHostName = _T("/WebObjects/MZFinance.woa/wa/editAccountFieldsSrv");

	if (lpPasswordToken && lpPasswordToken[0] != '\0')
	{
		MultiByteToWideChar(CP_ACP,0,(LPCSTR)lpPasswordToken,strlen((char*)lpPasswordToken),strPasswordToken,MAX_PATH);
	}

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

		getFormatTime(szFormatTime);
		::MultiByteToWideChar(CP_ACP,0,szFormatTime,strlen(szFormatTime),wzFormatTime,512);

		CString strHeaders = _T("User-Agent: ");
		strHeaders += strAgent;
		if (lpPasswordToken && lpPasswordToken[0] != '\0')
		{
			strHeaders += _T("\r\nX-Token: ");
			strHeaders += strPasswordToken;
		}
		if (lpDsid && lpDsid[0] != '\0')
		{
			strHeaders += _T("\r\nX-Dsid: ");
			strHeaders += A2T(lpDsid);;
		}
		strHeaders += _T("\r\nAccept: */*");
		strHeaders += _T("\r\nX-Apple-Connection-Type: WiFi");
		strHeaders += _T("\r\nX-Apple-Store-Front: 143465-19,29");
		strHeaders += _T("\r\nX-Apple-Client-Versions: GameCenter/2.0");
		strHeaders += _T("\r\nAccept-Language: zh-Hans");
		strHeaders += _T("\r\nX-Apple-I-Client-Time: ");
		strHeaders += wzFormatTime;

		if (lpX_Apple_I_md_m && lpX_Apple_I_md_m[0] != TEXT('\0'))
		{
			strHeaders += _T("\r\nX-Apple-I-MD-RINFO: 17106176");
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
		strHeaders += _T("\r\nX-Apple-Tz: 28800");
		strHeaders += _T("\r\nAccept-Encoding: gzip, deflate");
		strHeaders += _T("\r\nX-Apple-Partner: origin.0");
		strHeaders += _T("\r\nConnection: keep-alive\r\n\r\n");

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
			///分配虚拟内存保存页面数据
			DWORD dataLength = 0;
			lpgzipBuf =  (LPGZIP)VirtualAlloc(NULL,1024*1300,MEM_COMMIT | MEM_RESERVE,PAGE_EXECUTE_READWRITE);//分配800K的虚拟内存
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
				if (!pDecodeData)
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

				//解析数据，得到需要的字段
				ParseWebData(UTF8ToGBK(pDecodeData));


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

BOOL CViewPage::registerXpReport(CString strUserAgent,
								 char* lpDsid,
								 string &strClientId,
								 string &strXpab,
								 string &strXpabc,
								 string &strClientId_value,
								 string &strXpab_value,
								 string &strXpabc_value,
								 CString strCookie,
								 BOOL bReadData,
								 TCHAR* lpX_Apple_I_md_m,
								 TCHAR* lpX_Apple_I_md,
								 TCHAR* lpProxy,
								 int netType)
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

	string strxpci_ = "";
	string strxpab_ = "";
	string strxpabc_ = "";
	string strxpci_value = "";
	string strxpab_value = "";
	string strxpabc_value = "";

	CString strUrl = _T("https://xp.apple.com");
	CString strAgent = strUserAgent;

	TCHAR strDsid[64] = {0};
	TCHAR strHostName[MAX_PATH] = _T("/register");

	if (lpDsid && lpDsid[0] != '\0')
	{
		MultiByteToWideChar(CP_ACP,0,(LPCSTR)lpDsid,strlen(lpDsid),strDsid,64);
	}

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

		CString strHeaders = _T("User-Agent: ");
		strHeaders += strAgent;
		if (lpDsid && lpDsid[0] != '\0')
		{
			strHeaders += TEXT("\r\nX-Dsid: ");
			strHeaders += strDsid;
		}
		strHeaders += TEXT("\r\nAccept-Language: zh-cn");
		strHeaders += TEXT("\r\nX-Apple-Connection-Type: WiFi");
		strHeaders += TEXT("\r\nX-Apple-Store-Front: 143465-19,29");
		strHeaders += TEXT("\r\nX-Apple-Partner: origin.0");
		strHeaders += TEXT("\r\nX-Apple-I-Client-Time: ");
		strHeaders += GetClientTime();
		if (lpX_Apple_I_md_m[0] != TEXT('\0'))
		{
			strHeaders += _T("\r\nX-Apple-I-MD-RINFO: 17106176");
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
		strHeaders += TEXT("\r\nX-Apple-Tz: 28800");
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
		else if (!bSendRequest)
		{
			InternetCloseHandle(hOpenReq);
			InternetCloseHandle(hConnect);
			InternetCloseHandle(hSession);
			return FALSE;
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
			if (bReadData)
			{
				getxpClientIdValue(hOpenReq,strxpci_,strxpab_,strxpabc_,strxpci_value,strxpab_value,strxpabc_value);

				strClientId = strxpci_;
				strXpab = strxpab_;
				strXpabc = strxpabc_;

				strClientId_value = strxpci_value;
				strXpab_value = strxpab_value;
				strXpabc_value = strxpabc_value;
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

BOOL CViewPage::getTargetIdWebData(CString strUserAgent,
								   LPTSTR pSearchHotWord,
								   char* lpTargetUrl,
								   char* lpSaleId,
								   char* lpDsid,
								   char* lpAppExtVrsId,
								   char* lpPasswordToken,
								   CString strCookie,
								   TCHAR* lpX_Apple_I_md_m,
								   TCHAR* lpX_Apple_I_md,
								   TCHAR* lpProxy,
								   int netType)
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

	LPGZIP lpgzipBuf = NULL;

	CString strUrl = _T("https://itunes.apple.com");
	CString strAgent = strUserAgent;
	unsigned char* pBuf = NULL;
	unsigned char encodeKeyword[MAX_PATH];
	int wHotwordLen = wcslen(pSearchHotWord);
	int strLen = WideCharToMultiByte(CP_UTF8,0,pSearchHotWord,wHotwordLen,NULL,0,NULL,NULL);
	pBuf = new unsigned char[strLen+1];
	if (!pBuf)
	{
		return FALSE;
	}
	memset(pBuf,'\0',strLen+1);

	WideCharToMultiByte(CP_UTF8,0,pSearchHotWord,wHotwordLen,(LPSTR)pBuf,strLen,NULL,NULL);

	//Title url编码
	urlencode(pBuf,strLen,encodeKeyword,MAX_TITLE);
	if (pBuf)
	{
		delete []pBuf;
		pBuf = NULL;
	}

	TCHAR strPasswordToken[MAX_PATH] = {0};
	TCHAR strHostName[MAX_PATH] = {0};
	TCHAR strReferer[MAX_PATH] = {0};

	char buf[MAX_PATH] = {0};
	sprintf(buf,"https://itunes.apple.com/WebObjects/MZStore.woa/wa/search?clientApplication=Software&term=%s",encodeKeyword);
	MultiByteToWideChar(CP_ACP,0,buf,strlen(buf),strReferer,MAX_PATH);

	MultiByteToWideChar(CP_ACP,0,(LPCSTR)lpPasswordToken,strlen((char*)lpPasswordToken),strPasswordToken,MAX_PATH);

	MultiByteToWideChar(CP_ACP,0,lpTargetUrl,strlen(lpTargetUrl),strHostName,MAX_PATH);

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

		CString strHeaders = _T("User-Agent: ");
		strHeaders += strAgent;
		strHeaders += _T("\r\nX-Token: ");
		strHeaders += strPasswordToken;
		strHeaders += _T("\r\nAccept-Language: zh-Hans");
		strHeaders += _T("\r\nX-Apple-Store-Front: 143465-19,29");
		strHeaders += _T("\r\nX-Apple-Connection-Type: WiFi");
		strHeaders += _T("\r\nX-Apple-Partner: origin.0");
		if (lpX_Apple_I_md_m[0] != TEXT('\0'))
		{
			strHeaders += _T("\r\nX-Apple-I-MD-RINFO: 17106176");
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
		strHeaders += GetClientTime(m_strTimeStamp);

		strHeaders += _T("\r\nX-Apple-Tz: 28800");
		strHeaders += _T("\r\nAccept-Encoding: gzip, deflate");
		strHeaders += _T("\r\nConnection: keep-alive\r\n\r\n");

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
			///分配虚拟内存保存页面数据
			DWORD dataLength = 0;
			lpgzipBuf =  (LPGZIP)VirtualAlloc(NULL,1024*1300,MEM_COMMIT | MEM_RESERVE,PAGE_EXECUTE_READWRITE);//分配800K的虚拟内存
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
				if (!pDecodeData)
				{
					if (lpgzipBuf)
					{
						VirtualFree(lpgzipBuf,0,MEM_RELEASE);
						lpgzipBuf = NULL;
					}

				}

				//解析数据，得到需要的字段
				parseJsonData(pDecodeData,lpSaleId,lpAppExtVrsId);

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

BOOL CViewPage::reLoginAppstoreXpReport(CString strUserAgent,
										CString strXpReportUrl,
										CString strXpReportHostName,
										TCHAR* lpDsid,
										CString strCookie,
										TCHAR* lpX_Apple_I_md_m,
										TCHAR* lpX_Apple_I_md,
										TCHAR* lpProxy,
										int netType)
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

	CString strAgent = strUserAgent;

	BOOL bCrackUrl = ::InternetCrackUrl(strXpReportUrl,0,0,&uc);
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

		hOpenReq = HttpOpenRequest(hConnect,_T("GET"),strXpReportHostName,0,0,0,0x84801000|INTERNET_FLAG_NO_COOKIES,0);//0x84A01000是一个常数,注意
		if (!hOpenReq)
		{
			InternetCloseHandle(hConnect);
			InternetCloseHandle(hSession);
			return FALSE;
		}

		CString strHeaders = _T("\r\nUser-Agent: ");
		strHeaders += strAgent;
		strHeaders += _T("\r\nAccept-Language: zh-cn");
		strHeaders += _T("\r\nX-Apple-Store-Front: 143465-19,29");
		strHeaders += _T("\r\nX-Apple-Connection-Type: WiFi");
		strHeaders += _T("\r\nX-Apple-Partner: origin.0");
		strHeaders += _T("\r\nAccept: */*");
		strHeaders += _T("\r\nX-Dsid: ");
		strHeaders += lpDsid;
		strHeaders += _T("\r\nAccept-Encoding: gzip, deflate");
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
		strHeaders += GetClientTime();
		strHeaders += _T("\r\nX-Apple-Tz: 28800");

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

BOOL CViewPage::itmsAppsReport(CString strUserAgent,
							   CString strBuyUrl,
							   CString strItmsAppsHostName,
							   TCHAR* lpDsid,
							   TCHAR* lpAgreeAction,
							   TCHAR* lpAgreeKey,
							   TCHAR* lpTermsId,
							   TCHAR* lpWosid_lite,
							   CString strCookie,
							   TCHAR* lpProxy,
							   int netType)
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

	CString strAgent = strUserAgent;
	char* lpDecodeData = NULL;
	DWORD dwSize = 0;

	BOOL bCrackUrl = ::InternetCrackUrl(strBuyUrl,0,0,&uc);
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

		hOpenReq = HttpOpenRequest(hConnect,_T("GET"),strItmsAppsHostName,0,0,0,0x84801000|INTERNET_FLAG_NO_COOKIES,0);//0x84A01000是一个常数,注意
		if (!hOpenReq)
		{
			InternetCloseHandle(hConnect);
			InternetCloseHandle(hSession);
			return FALSE;
		}

		CString strHeaders = _T("\r\nUser-Agent: ");
		strHeaders += strAgent;
		strHeaders += _T("\r\nAccept-Language: zh-cn");
		strHeaders += _T("\r\nX-Apple-Store-Front: 143465-19,29");
		strHeaders += _T("\r\nX-Apple-Connection-Type: WiFi");
		strHeaders += _T("\r\nX-Apple-Partner: origin.0");
		strHeaders += _T("\r\nX-Apple-Client-Versions: GameCenter/2.0");
		strHeaders += _T("\r\nAccept: text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8");
		strHeaders += _T("\r\nX-Dsid: ");
		strHeaders += lpDsid;
		if (!strCookie.IsEmpty())
		{
			strHeaders += TEXT("\r\nCookie: ");
			strHeaders += strCookie;

		}
		strHeaders += _T("\r\nAccept-Encoding: gzip, deflate");
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
			//分配内存空间
			lpDecodeData = (char*)VirtualAlloc(NULL,1024*100,MEM_COMMIT | MEM_RESERVE,PAGE_EXECUTE_READWRITE);//分配50K的虚拟内存
			if (!lpDecodeData)
			{
				InternetCloseHandle(hOpenReq);
				InternetCloseHandle(hConnect);
				InternetCloseHandle(hSession);
				return FALSE;
			}

			//解码从服务器过来的数据
			decodeServerData(hOpenReq,lpDecodeData,&dwSize);

			//解析数据
			parseAgreeWebData(lpDecodeData,lpAgreeAction,lpAgreeKey,lpTermsId,lpWosid_lite);

			if (lpDecodeData)
			{
				VirtualFree(lpDecodeData,0,MEM_RELEASE);
				lpDecodeData = NULL;
			}

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

BOOL CViewPage::getSecurityVerifyWebReport(CString strUserAgent,
										   CString strBuyUrl,
										   CString strItmsAppsHostName,
										   TCHAR* lpDsid,
										   TCHAR* lpSecurityAction,
										   TCHAR* lpSubmitKey,
										   SECQUESTIONTONAME& securityMap,
										   CString strCookie,
										   TCHAR* lpProxy,
										   int netType)

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

	CString strAgent = strUserAgent;
	char* lpDecodeData = NULL;
	DWORD dwSize = 0;

	BOOL bCrackUrl = ::InternetCrackUrl(strBuyUrl,0,0,&uc);
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

		hOpenReq = HttpOpenRequest(hConnect,_T("GET"),strItmsAppsHostName,0,0,0,0x84801000|INTERNET_FLAG_NO_COOKIES,0);//0x84A01000是一个常数,注意
		if (!hOpenReq)
		{
			InternetCloseHandle(hConnect);
			InternetCloseHandle(hSession);
			return FALSE;
		}

		CString strHeaders = _T("\r\nUser-Agent: ");
		strHeaders += strUserAgent;
		strHeaders += _T("\r\nAccept-Language: zh-cn");
		strHeaders += _T("\r\nX-Apple-Store-Front: 143465-19,29");
		strHeaders += _T("\r\nX-Apple-Connection-Type: WiFi");
		strHeaders += _T("\r\nX-Apple-Partner: origin.0");
		strHeaders += _T("\r\nX-Apple-Client-Versions: GameCenter/2.0");
		strHeaders += _T("\r\nAccept: text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8");
		strHeaders += _T("\r\nX-Dsid: ");
		strHeaders += lpDsid;

		if (!strCookie.IsEmpty())
		{
			strHeaders += TEXT("\r\nCookie: ");
			strHeaders += strCookie;

		}

		strHeaders += _T("\r\nAccept-Encoding: gzip, deflate");
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
			//分配内存空间
			lpDecodeData = (char*)VirtualAlloc(NULL,1024*100,MEM_COMMIT | MEM_RESERVE,PAGE_EXECUTE_READWRITE);//分配50K的虚拟内存
			if (!lpDecodeData)
			{
				InternetCloseHandle(hOpenReq);
				InternetCloseHandle(hConnect);
				InternetCloseHandle(hSession);
				return FALSE;
			}

			//解码从服务器过来的数据
			decodeServerData(hOpenReq,lpDecodeData,&dwSize);

			//解析需要回答的安全问题
			parseSecurityQustionData(lpDecodeData,lpSecurityAction,lpSubmitKey,securityMap);

			if (lpDecodeData)
			{
				VirtualFree(lpDecodeData,0,MEM_RELEASE);
				lpDecodeData = NULL;
			}

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


BOOL CViewPage::securityAnswerReport(CString strUserAgent,
									 CString strBuyUrl,
									 CString strSecurityActinHostName,
									 CString strRefererUrl,
									 TCHAR* lpDsid,
									 TCHAR* lpSubmitKey,
									 NAMETOANSWER* pNameToAnswer,
									 char* lpXAppleMdData,
									 CString strCookie,
									 TCHAR* lpProxy,
									 int netType)
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

	CString strAgent = strUserAgent;

	char szSubmitKey[128] = {0};
	char szSendData[512] = {0};
	char szAnswerName[128] = {0};
	unsigned char szAnswer[128] = {0};
	unsigned char szEncodeAnswer[MAX_PATH] = {0};
	int encodeLen = MAX_PATH;

	WideCharToMultiByte(CP_UTF8,0,lpSubmitKey,wcslen(lpSubmitKey),szSubmitKey,128,NULL,NULL);

	strcat(szSendData,szSubmitKey);
	strcat(szSendData,"=%E6%8F%90%E4%BA%A4");

	for (NAMETOANSWER::iterator ptr = pNameToAnswer->begin(); ptr != pNameToAnswer->end(); ptr ++)
	{
		strcat(szSendData,"&");

		CString strFirst = ptr->first;
		WideCharToMultiByte(CP_UTF8,0,strFirst.GetBuffer(),strFirst.GetLength(),szAnswerName,128,NULL,NULL);
		strcat(szSendData,szAnswerName);

		strcat(szSendData,"=");
		WideCharToMultiByte(CP_UTF8,0,ptr->second.GetBuffer(),ptr->second.GetLength(),(LPSTR)szAnswer,128,NULL,NULL);
		urlencode(szAnswer,strlen((char*)szAnswer),szEncodeAnswer,encodeLen);
		strcat(szSendData,(char*)szEncodeAnswer);

		memset(szAnswer,0,128);
		memset(szEncodeAnswer,0,MAX_PATH);
		memset(szAnswerName,0,128);
	}

	int sendLen = strlen(szSendData);

	BOOL bCrackUrl = ::InternetCrackUrl(strBuyUrl,0,0,&uc);
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

		hOpenReq = HttpOpenRequest(hConnect,_T("POST"),strSecurityActinHostName,0,0,0,0x84801000|INTERNET_FLAG_NO_COOKIES,0);//0x84A01000是一个常数,注意
		if (!hOpenReq)
		{
			InternetCloseHandle(hConnect);
			InternetCloseHandle(hSession);
			return FALSE;
		}

		CString strHeaders = _T("\r\nUser-Agent: ");
		strHeaders += strAgent;
		strHeaders += _T("\r\nAccept-Language: zh-cn");
		strHeaders += _T("\r\nX-Apple-Store-Front: 143465-19,29");
		strHeaders += _T("\r\nX-Apple-Connection-Type: WiFi");
		strHeaders += _T("\r\nX-Apple-Partner: origin.0");
		strHeaders += _T("\r\nX-Apple-Client-Versions: GameCenter/2.0");
		strHeaders += _T("\r\nContent-Type: application/x-www-form-urlencoded");
		strHeaders += _T("\r\nAccept: text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8");
		strHeaders += _T("\r\nX-Dsid: ");
		strHeaders += lpDsid;

		if (!strCookie.IsEmpty())
		{
			strHeaders += TEXT("\r\nCookie: ");
			strHeaders += strCookie;

		}

		strHeaders += _T("\r\nReferer: ");
		strHeaders += strRefererUrl;
		strHeaders += _T("\r\nAccept-Encoding: gzip, deflate");
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

		bSendRequest = HttpSendRequestW(hOpenReq,NULL,-1,szSendData,sendLen);//第三个参数设置为负一
		if (!bSendRequest && GetLastError() == ERROR_INTERNET_INVALID_CA)
		{
			DWORD dwFlags;
			DWORD dwBuffLen = sizeof(dwFlags);
			InternetQueryOption (hOpenReq, INTERNET_OPTION_SECURITY_FLAGS,(LPVOID)&dwFlags, &dwBuffLen);
			dwFlags |= SECURITY_FLAG_IGNORE_UNKNOWN_CA;
			InternetSetOption (hOpenReq,INTERNET_OPTION_SECURITY_FLAGS,&dwFlags,sizeof (dwFlags) );

			//再次发送
			bSendRequest = HttpSendRequest(hOpenReq,NULL,-1,szSendData,sendLen);
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
			//获取X-Apple-md-data的值
			getXAppleMdDataValue(hOpenReq,lpXAppleMdData);
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


BOOL CViewPage::ProvisioningReport(CString strUserAgent,
								   CString strBuyUrl,
								   CString strHostName,
								   TCHAR* lpPasswordToken,
								   TCHAR* lpDsid,
								   CString strCookie,
								   TCHAR* lpX_Apple_I_md_m,
								   TCHAR* lpX_Apple_I_md,
								   TCHAR* lpX_Apple_amd_m,
								   TCHAR* lpX_Apple_amd,
								   char* lpSendData,
								   int sendLength,
								   char* lpSettingInfo,
								   char* lpTransportKey,
								   TCHAR* lpProxy,
								   int netType)
{
	char* lpDecodeData = NULL;
	DWORD dwSize = 0;
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

	BOOL bCrackUrl = InternetCrackUrl(strBuyUrl,0,0,&uc);
	if (bCrackUrl)
	{
		if (netType == PROXY_NETWORK)
		{
			hSession = InternetOpen(strUserAgent,INTERNET_OPEN_TYPE_PROXY,lpProxy,0,0);
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

		DWORD dwFlag = INTERNET_FLAG_KEEP_CONNECTION | INTERNET_FLAG_NO_CACHE_WRITE | INTERNET_FLAG_NO_COOKIES; 

		hOpenReq = HttpOpenRequest(hConnect,TEXT("POST"),strHostName,0,0,0,0x84801000|INTERNET_FLAG_NO_COOKIES,0); //0x84A01000是一个常数
		if (!hOpenReq)
		{
			InternetCloseHandle(hConnect);
			InternetCloseHandle(hSession);
			return FALSE;
		}

		CString strHeaders = _T("User-Agent: ");
		strHeaders += strUserAgent;
		strHeaders += _T("\r\nAccept: */*");
		strHeaders += _T("\r\nContent-Type: application/x-apple-plist");
		strHeaders += _T("\r\nAccept-Language: zh-cn");
		strHeaders += _T("\r\nX-Apple-Store-Front: 143465-19,29");
		strHeaders += _T("\r\nX-Apple-Tz: 28800");
		strHeaders += _T("\r\nX-Apple-Connection-Type: WiFi");
		strHeaders += _T("\r\nX-Apple-I-Client-Time: ");
		strHeaders += GetClientTime();
		if (lpPasswordToken)
		{
			strHeaders += _T("\r\nX-Token: ");
			strHeaders += lpPasswordToken;
		}
		if (lpDsid)
		{
			strHeaders += _T("\r\nX-Dsid: ");
			strHeaders += lpDsid;
		}
		if (lpX_Apple_I_md_m[0] != TEXT('\0'))
		{
			strHeaders += TEXT("\r\nX-Apple-I-MD-RINFO: 17106176");
			strHeaders += _T("\r\nX-Apple-I-MD-M: ");
			strHeaders += lpX_Apple_I_md_m;
			strHeaders += TEXT("\r\nX-Apple-I-MD: ");
			strHeaders += lpX_Apple_I_md;
		}
		if ( (lpX_Apple_amd != NULL) && (lpX_Apple_amd_m[0] != TEXT('\0')) )
		{
			strHeaders += TEXT("\r\nX-Apple-AMD-M: ");
			strHeaders += lpX_Apple_amd_m;
			strHeaders += TEXT("\r\n\r\nX-Apple-AMD: ");
			strHeaders += lpX_Apple_amd;
		}
		if (!strCookie.IsEmpty())
		{
			strHeaders += TEXT("\r\nCookie: ");
			strHeaders += strCookie;

		}
		strHeaders += _T("\r\nAccept-Encoding: gzip, deflate");
		strHeaders += _T("\r\nConnection: Keep-Alive\r\n\r\n");	

		DWORD dwHeaderLength = wcslen(strHeaders);

		bAddRequestHeader = HttpAddRequestHeaders(hOpenReq,strHeaders,dwHeaderLength,0xA0000000);//0xA0000000是一个常量
		if (!bAddRequestHeader)
		{
			InternetCloseHandle(hOpenReq);
			InternetCloseHandle(hConnect);
			InternetCloseHandle(hSession);
			return FALSE;
		}

		//准备发送的数据
		bSendRequest = HttpSendRequest(hOpenReq,NULL,-1,lpSendData,sendLength);
		if (!bSendRequest && GetLastError() == ERROR_INTERNET_INVALID_CA)
		{
			DWORD dwFlags;
			DWORD dwBuffLen = sizeof(dwFlags);
			InternetQueryOption (hOpenReq, INTERNET_OPTION_SECURITY_FLAGS,(LPVOID)&dwFlags, &dwBuffLen);
			dwFlags |= SECURITY_FLAG_IGNORE_UNKNOWN_CA;
			InternetSetOption (hOpenReq,INTERNET_OPTION_SECURITY_FLAGS,&dwFlags,sizeof (dwFlags) );

			//再次发送
			bSendRequest = HttpSendRequest(hOpenReq,NULL,-1,lpSendData,sendLength);
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
		if (dwQueryBuf == 302)
		{
			//重定向
			TCHAR location[MAX_PATH] = {0};
			DWORD locationLength = sizeof(location);

			bQueryInfo = HttpQueryInfo(hOpenReq,HTTP_QUERY_LOCATION,location,&locationLength,0);
			if (!bQueryInfo)
			{
				InternetCloseHandle(hOpenReq);
				InternetCloseHandle(hConnect);
				InternetCloseHandle(hSession);
				return FALSE;
			}

			CString tmpBuf(location);
			CString strRedirectUrl;
			CString strRedirectHostName;
			int Index = tmpBuf.Find(_T(".com"));
			strRedirectUrl = tmpBuf.Mid(0,Index+4);
			strRedirectHostName = tmpBuf.Mid(Index+4);
			//重定向后再次发送
			ProvisioningReport(strUserAgent,
				strRedirectUrl,
				strRedirectHostName,
				lpPasswordToken,
				lpDsid,
				strCookie,
				lpX_Apple_I_md_m,
				lpX_Apple_I_md,
				lpX_Apple_amd_m,
				lpX_Apple_amd,
				lpSendData,
				sendLength,
				lpSettingInfo,
				lpTransportKey,
				lpProxy,
				netType);


		}
		else if (dwQueryBuf == 200)//下面是数据接收处理部分
		{
			//分配内存空间
			lpDecodeData = (char*)VirtualAlloc(NULL,1024*100,MEM_COMMIT | MEM_RESERVE,PAGE_EXECUTE_READWRITE);//分配50K的虚拟内存
			if (!lpDecodeData)
			{
				InternetCloseHandle(hOpenReq);
				InternetCloseHandle(hConnect);
				InternetCloseHandle(hSession);
				return FALSE;
			}

			//解码从服务器过来的数据
			decodeServerData(hOpenReq,lpDecodeData,&dwSize);

			//提取settinginfo的值和transportKey的值
			CViewPage::getSettingInfoAndtransportKeyValue(lpDecodeData,lpSettingInfo,lpTransportKey);


			if (lpDecodeData)
			{
				VirtualFree(lpDecodeData,0,MEM_RELEASE);
				lpDecodeData = NULL;
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

BOOL CViewPage::startMachineProvisionReport(TCHAR* lpUdid,
											char* lpSpimData,
											CString strCookie,
											CString strMME_Client_Info,
											CString strSerialNo,
											TCHAR* lpProxy,
											int netType)
{
	char szSpimData[512] = {0};
	char* lpDecodeData = NULL;
	DWORD dwSize = 0;
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

	CString strHostName = TEXT("/grandslam/MidService/startMachineProvisioning");
	CString strGsaUrl = TEXT("https://gsa.apple.com");
	CString strUserAgent = GSA_USER_AGENT;
	int error = 0;

	BOOL bCrackUrl = InternetCrackUrl(strGsaUrl,0,0,&uc);
	if (bCrackUrl)
	{
		if (netType == PROXY_NETWORK)
		{
			hSession = InternetOpen(strUserAgent,INTERNET_OPEN_TYPE_PROXY,lpProxy,0,0);
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

		DWORD dwFlag = INTERNET_FLAG_KEEP_CONNECTION | INTERNET_FLAG_NO_CACHE_WRITE | INTERNET_FLAG_NO_COOKIES; 

		hOpenReq = HttpOpenRequest(hConnect,TEXT("POST"),strHostName,0,0,0,0x84801000|INTERNET_FLAG_NO_COOKIES,0); //0x84A01000是一个常数
		if (!hOpenReq)
		{
			InternetCloseHandle(hConnect);
			InternetCloseHandle(hSession);
			return FALSE;
		}

		CString strHeaders = _T("User-Agent: ");
		strHeaders += strUserAgent;
		strHeaders += _T("\r\nAccept: */*");
		strHeaders += _T("\r\nContent-Type: application/x-www-form-urlencoded");
		strHeaders += _T("\r\nAccept-Language: zh-cn");
		strHeaders += _T("\r\nX-Apple-I-SRL-NO: ");
		strHeaders += strSerialNo;
		strHeaders += _T("\r\nX-MMe-Client-Info: ");
		strHeaders += strMME_Client_Info;
		strHeaders += _T("\r\nAccept-Encoding: gzip, deflate");
		strHeaders += _T("\r\nX-Mme-Device-Id: ");
		strHeaders += lpUdid;

		if (!strCookie.IsEmpty())
		{
			strHeaders += TEXT("\r\nCookie: ");
			strHeaders += strCookie;

		}

		strHeaders += _T("\r\nConnection: Keep-Alive\r\n\r\n");	

		DWORD dwHeaderLength = wcslen(strHeaders);

		bAddRequestHeader = HttpAddRequestHeaders(hOpenReq,strHeaders,dwHeaderLength,0xA0000000);//0xA0000000是一个常量
		if (!bAddRequestHeader)
		{
			InternetCloseHandle(hOpenReq);
			InternetCloseHandle(hConnect);
			InternetCloseHandle(hSession);
			return FALSE;
		}

		char szSendData[] = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\
							<!DOCTYPE plist PUBLIC \"-//Apple//DTD PLIST 1.0//EN\" \"http://www.apple.com/DTDs/PropertyList-1.0.dtd\">\
							<plist version=\"1.0\">\
							<dict>\
							<key>Header</key>\
							<dict/>\
							<key>Request</key>\
							<dict/>\
							</dict>\
							</plist>";

		int sendLength = strlen(szSendData);

		//准备发送的数据
		DWORD dwTimeOut = 1000*60;
		InternetSetOption(NULL,INTERNET_OPTION_CONNECT_TIMEOUT,&dwTimeOut,sizeof(dwTimeOut));  
		bSendRequest = HttpSendRequest(hOpenReq,NULL,-1,szSendData,sendLength);
		if (!bSendRequest && GetLastError() == ERROR_INTERNET_INVALID_CA)
		{
			DWORD dwFlags;
			DWORD dwBuffLen = sizeof(dwFlags);
			InternetQueryOption (hOpenReq, INTERNET_OPTION_SECURITY_FLAGS,(LPVOID)&dwFlags, &dwBuffLen);
			dwFlags |= SECURITY_FLAG_IGNORE_UNKNOWN_CA;
			InternetSetOption(hOpenReq,INTERNET_OPTION_SECURITY_FLAGS,&dwFlags,sizeof (dwFlags) );

			//再次发送
			bSendRequest = HttpSendRequest(hOpenReq,NULL,-1,szSendData,sendLength);
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
			error = GetLastError();
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
		if (dwQueryBuf == 200)//下面是数据接收处理部分
		{
			//分配内存空间
			lpDecodeData = (char*)VirtualAlloc(NULL,1024*100,MEM_COMMIT | MEM_RESERVE,PAGE_EXECUTE_READWRITE);//分配50K的虚拟内存
			if (!lpDecodeData)
			{
				InternetCloseHandle(hOpenReq);
				InternetCloseHandle(hConnect);
				InternetCloseHandle(hSession);
				return FALSE;
			}

			//解码从服务器过来的数据
			decodeServerData(hOpenReq,lpDecodeData,&dwSize);

			//提取spim的值
			getSpimValue(lpDecodeData,szSpimData);
			memcpy(lpSpimData,szSpimData,strlen(szSpimData));

			if (lpDecodeData)
			{
				VirtualFree(lpDecodeData,0,MEM_RELEASE);
				lpDecodeData = NULL;
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

BOOL CViewPage::finishMachineProvisionReport(TCHAR* lpUdid,
											 char* lpCpimData,
											 char* lpTKData,
											 char* lpPtmData,
											 CString strCookie,
											 CString strMME_Client_Info,
											 CString strSerialNo,
											 TCHAR* lpProxy,
											 int netType)
{
	char szPtmData[512] = {0};
	char szSendData[1024] = {0};
	char szTKData[32] = {0};
	char* lpDecodeData = NULL;
	int sendLength = 0;
	DWORD dwSize = 0;
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

	CString strHostName = TEXT("/grandslam/MidService/finishMachineProvisioning");
	CString strGsaUrl = TEXT("https://gsa.apple.com");
	CString strUserAgent = GSA_USER_AGENT;

	BOOL bCrackUrl = InternetCrackUrl(strGsaUrl,0,0,&uc);
	if (bCrackUrl)
	{
		if (netType == PROXY_NETWORK)
		{
			hSession = InternetOpen(strUserAgent,INTERNET_OPEN_TYPE_PROXY,lpProxy,0,0);
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

		DWORD dwFlag = INTERNET_FLAG_KEEP_CONNECTION | INTERNET_FLAG_NO_CACHE_WRITE | INTERNET_FLAG_NO_COOKIES; 

		hOpenReq = HttpOpenRequest(hConnect,TEXT("POST"),strHostName,0,0,0,0x84801000|INTERNET_FLAG_NO_COOKIES,0); //0x84A01000是一个常数
		if (!hOpenReq)
		{
			InternetCloseHandle(hConnect);
			InternetCloseHandle(hSession);
			return FALSE;
		}

		CString strHeaders = _T("User-Agent: ");
		strHeaders += strUserAgent;
		strHeaders += _T("\r\nAccept: */*");
		strHeaders += _T("\r\nContent-Type: application/x-www-form-urlencoded");
		strHeaders += _T("\r\nAccept-Language: zh-cn");
		strHeaders += _T("\r\nX-Apple-I-SRL-NO: ");
		strHeaders += strSerialNo;
		strHeaders += _T("\r\nX-MMe-Client-Info: ");
		strHeaders += strMME_Client_Info;
		strHeaders += _T("\r\nAccept-Encoding: gzip, deflate");
		strHeaders += _T("\r\nX-Mme-Device-Id: ");
		strHeaders += lpUdid;
		if (!strCookie.IsEmpty())
		{
			strHeaders += TEXT("\r\nCookie: ");
			strHeaders += strCookie;

		}
		strHeaders += _T("\r\nConnection: Keep-Alive\r\n\r\n");	

		DWORD dwHeaderLength = wcslen(strHeaders);

		bAddRequestHeader = HttpAddRequestHeaders(hOpenReq,strHeaders,dwHeaderLength,0xA0000000);//0xA0000000是一个常量
		if (!bAddRequestHeader)
		{
			InternetCloseHandle(hOpenReq);
			InternetCloseHandle(hConnect);
			InternetCloseHandle(hSession);
			return FALSE;
		}

		sprintf(szSendData,"<?xml version=\"1.0\" encoding=\"UTF-8\"?>\
						   <!DOCTYPE plist PUBLIC \"-//Apple//DTD PLIST 1.0//EN\" \"http://www.apple.com/DTDs/PropertyList-1.0.dtd\">\
						   <plist version=\"1.0\">\
						   <dict>\
						   <key>Header</key>\
						   <dict/>\
						   <key>Request</key>\
						   <dict>\
						   <key>cpim</key>\
						   <string>%s</string>\
						   </dict>\
						   </dict>\
						   </plist>",lpCpimData);

		sendLength = strlen(szSendData);

		//准备发送的数据
		bSendRequest = HttpSendRequest(hOpenReq,NULL,-1,szSendData,sendLength);
		if (!bSendRequest && GetLastError() == ERROR_INTERNET_INVALID_CA)
		{
			DWORD dwFlags;
			DWORD dwBuffLen = sizeof(dwFlags);
			InternetQueryOption (hOpenReq, INTERNET_OPTION_SECURITY_FLAGS,(LPVOID)&dwFlags, &dwBuffLen);
			dwFlags |= SECURITY_FLAG_IGNORE_UNKNOWN_CA;
			InternetSetOption (hOpenReq,INTERNET_OPTION_SECURITY_FLAGS,&dwFlags,sizeof (dwFlags) );

			//再次发送
			bSendRequest = HttpSendRequest(hOpenReq,NULL,-1,szSendData,sendLength);
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
		if (dwQueryBuf == 200)//下面是数据接收处理部分
		{
			//分配内存空间
			lpDecodeData = (char*)VirtualAlloc(NULL,1024*100,MEM_COMMIT | MEM_RESERVE,PAGE_EXECUTE_READWRITE);//分配50K的虚拟内存
			if (!lpDecodeData)
			{
				InternetCloseHandle(hOpenReq);
				InternetCloseHandle(hConnect);
				InternetCloseHandle(hSession);
				return FALSE;
			}

			//解码从服务器过来的数据
			decodeServerData(hOpenReq,lpDecodeData,&dwSize);

			//提取spim的值
			getTKandPTMValue(lpDecodeData,szTKData,szPtmData);
			memcpy(lpTKData,szTKData,strlen(szTKData));
			memcpy(lpPtmData,szPtmData,strlen(szPtmData));

			if (lpDecodeData)
			{
				VirtualFree(lpDecodeData,0,MEM_RELEASE);
				lpDecodeData = NULL;
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

int CViewPage::loginGsaServices2(char* lpSendData,int sendDataLen,CString strCookie,BOOL bA2kPackage,CString strMME_Client_Info,TCHAR* lpProxy,int netType)
{
	char* lpDecodeData = NULL;
	DWORD dwSize = 0;
	BOOL bResult = FALSE;
	int  status = STATUS_FAILED;
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

	CString strHostName = TEXT("/grandslam/GsService2");
	CString strGsaUrl = TEXT("https://gsa.apple.com");
	CString strUserAgent = GSA_USER_AGENT;

	BOOL bCrackUrl = InternetCrackUrl(strGsaUrl,0,0,&uc);
	if (bCrackUrl)
	{
		if (netType == PROXY_NETWORK)
		{
			hSession = InternetOpen(strUserAgent,INTERNET_OPEN_TYPE_PROXY,lpProxy,0,0);
		}
		else
		{
			hSession = InternetOpen(strUserAgent,0,0,0,0);
		}
		if (!hSession)
		{
			return status;
		}

		hConnect = InternetConnect(hSession,uc.lpszHostName,uc.nPort,uc.lpszUserName,uc.lpszPassword,3,0,0);
		if (!hConnect)
		{
			InternetCloseHandle(hSession);
			return status;
		}

		DWORD dwFlag = INTERNET_FLAG_KEEP_CONNECTION | INTERNET_FLAG_NO_CACHE_WRITE | INTERNET_FLAG_NO_COOKIES; 

		hOpenReq = HttpOpenRequest(hConnect,TEXT("POST"),strHostName,0,0,0,0x84801000|INTERNET_FLAG_NO_COOKIES,0); //0x84A01000是一个常数
		if (!hOpenReq)
		{
			InternetCloseHandle(hConnect);
			InternetCloseHandle(hSession);
			return status;
		}

		CString strHeaders = _T("User-Agent: ");
		strHeaders += strUserAgent;
		strHeaders += _T("\r\nAccept: */*");
		strHeaders += _T("\r\nContent-Type: text/x-xml-plist");
		strHeaders += _T("\r\nAccept-Language: zh-cn");
		strHeaders += _T("\r\nX-MMe-Client-Info: ");
		strHeaders += strMME_Client_Info;
		if (!strCookie.IsEmpty())
		{
			strHeaders += TEXT("\r\nCookie: ");
			strHeaders += strCookie;
		}
		strHeaders += _T("\r\nAccept-Encoding: gzip, deflate");
		strHeaders += _T("\r\nConnection: Keep-Alive\r\n\r\n");	

		DWORD dwHeaderLength = wcslen(strHeaders);

		bAddRequestHeader = HttpAddRequestHeaders(hOpenReq,strHeaders,dwHeaderLength,0xA0000000);//0xA0000000是一个常量
		if (!bAddRequestHeader)
		{
			InternetCloseHandle(hOpenReq);
			InternetCloseHandle(hConnect);
			InternetCloseHandle(hSession);
			return status;
		}

		//准备发送的数据
		bSendRequest = HttpSendRequest(hOpenReq,NULL,-1,lpSendData,sendDataLen);
		if (!bSendRequest && GetLastError() == ERROR_INTERNET_INVALID_CA)
		{
			DWORD dwFlags;
			DWORD dwBuffLen = sizeof(dwFlags);
			InternetQueryOption (hOpenReq, INTERNET_OPTION_SECURITY_FLAGS,(LPVOID)&dwFlags, &dwBuffLen);
			dwFlags |= SECURITY_FLAG_IGNORE_UNKNOWN_CA;
			InternetSetOption (hOpenReq,INTERNET_OPTION_SECURITY_FLAGS,&dwFlags,sizeof (dwFlags) );

			//再次发送
			bSendRequest = HttpSendRequest(hOpenReq,NULL,-1,lpSendData,sendDataLen);
			if (!bSendRequest)
			{
				int error = GetLastError();//ERROR_INTERNET_SEC_CERT_REV_FAILED 
				InternetCloseHandle(hOpenReq);
				InternetCloseHandle(hConnect);
				InternetCloseHandle(hSession);
				return status;
			}
		}
		else if (!bSendRequest)
		{
			InternetCloseHandle(hOpenReq);
			InternetCloseHandle(hConnect);
			InternetCloseHandle(hSession);
			return status;
		}

		DWORD dwQueryBuf = 200;
		DWORD dwQueryBufferLength = 4;
		bQueryInfo = HttpQueryInfo(hOpenReq,0x20000013,&dwQueryBuf,&dwQueryBufferLength,NULL);//HttpQueryInfo 获取文件大小后
		if (!bQueryInfo)
		{
			InternetCloseHandle(hOpenReq);
			InternetCloseHandle(hConnect);
			InternetCloseHandle(hSession);
			return status;
		}
		if (dwQueryBuf == 200)//下面是数据接收处理部分
		{
			//分配内存空间
			lpDecodeData = (char*)VirtualAlloc(NULL,1024*100,MEM_COMMIT | MEM_RESERVE,PAGE_EXECUTE_READWRITE);//分配50K的虚拟内存
			if (!lpDecodeData)
			{
				InternetCloseHandle(hOpenReq);
				InternetCloseHandle(hConnect);
				InternetCloseHandle(hSession);
				return status;
			}

			//解码从服务器过来的数据
			CViewPage::decodeServerData(hOpenReq,lpDecodeData,&dwSize);

			//分析数据的正确性
			status = checkPasswordIsCorrect(lpDecodeData);
			if (status == STATUS_SUCCESS)
			{
				if (bA2kPackage)
				{
					//提取i,s,c,B的值
					parseA2kResponsePacakage(lpDecodeData);
				}
				else
				{
					//提取spd, M2, np的值
					if (!parseM1ResponsePackage(lpDecodeData))
					{
						if (lpDecodeData)
						{
							VirtualFree(lpDecodeData,0,MEM_RELEASE);
							lpDecodeData = NULL;
						}
						InternetCloseHandle(hOpenReq);
						InternetCloseHandle(hConnect);
						InternetCloseHandle(hSession);
						return status;

					}
				}
			}
			else
			{
				if (lpDecodeData)
				{
					VirtualFree(lpDecodeData,0,MEM_RELEASE);
					lpDecodeData = NULL;
				}
				InternetCloseHandle(hOpenReq);
				InternetCloseHandle(hConnect);
				InternetCloseHandle(hSession);
				return status;

			}

			if (lpDecodeData)
			{
				VirtualFree(lpDecodeData,0,MEM_RELEASE);
				lpDecodeData = NULL;
			}

		}
		else
		{
			InternetCloseHandle(hOpenReq);
			InternetCloseHandle(hConnect);
			InternetCloseHandle(hSession);
			return status;
		}

	}
	else
	{
		return status;
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
	return status;
}

void CViewPage::getXAppleAMdDataValue(HINTERNET hOpenReq,char* lpXAppleAMdData)
{
	char		szAMdDataBuffer[1024] = {0};
	TCHAR		uzBuffer[2048] = {0};
	TCHAR*		pHeaderBuf = NULL;
	DWORD		dwLen = 0;
	TCHAR*		pBegin = NULL;
	TCHAR*		pEnd = NULL;
	TCHAR*      pTmp = NULL;
	TCHAR		uzAmdKey[] = TEXT("x-apple-amd-data: ");
	int			dataLength = 0;

	try
	{

		BOOL bRet = HttpQueryInfo(hOpenReq,HTTP_QUERY_RAW_HEADERS_CRLF,NULL,&dwLen,NULL);
		if (!bRet)
		{
			pHeaderBuf = new TCHAR[dwLen*2+2];
			memset(pHeaderBuf,0,dwLen*2+2);
			HttpQueryInfo(hOpenReq,HTTP_QUERY_RAW_HEADERS_CRLF,pHeaderBuf,&dwLen,NULL);
			pTmp = pHeaderBuf + 100;
			pBegin = wcsstr(pTmp,uzAmdKey);
			if (pBegin > 0)
			{
				pBegin = pBegin + wcslen(uzAmdKey);
				pEnd = wcsstr(pBegin,TEXT("\n"));
				if (pEnd > 0)
				{
					dataLength = pEnd - pBegin;
					wmemcpy(uzBuffer,pBegin,dataLength);
				}
				WideCharToMultiByte(CP_ACP,0,uzBuffer,dwLen,szAMdDataBuffer,1024,NULL,NULL);

				memcpy(lpXAppleAMdData,szAMdDataBuffer,strlen(szAMdDataBuffer));

			}
			if (pHeaderBuf)
			{
				delete []pHeaderBuf;
				pHeaderBuf = NULL;
			}
		}
	}
	catch(exception& e)
	{
		OutputDebugStringA(e.what());
	}
	return;
}

void CViewPage::getXAppleMdDataValue(HINTERNET hOpenReq,char* lpXAppleMdData)
{
	char		szMdDataBuffer[1024] = {0};
	TCHAR		uzBuffer[2048] = {0};
	TCHAR*		pHeaderBuf = NULL;
	DWORD		dwLen = 0;
	TCHAR*		pBegin = NULL;
	TCHAR*		pEnd = NULL;
	TCHAR*      pTmp = NULL;
	TCHAR		uzAmdKey[] = TEXT("x-apple-md-data: ");

	BOOL bRet = HttpQueryInfo(hOpenReq,HTTP_QUERY_RAW_HEADERS_CRLF,NULL,&dwLen,NULL);
	if (!bRet)
	{
		pHeaderBuf = new TCHAR[dwLen*2+2];
		memset(pHeaderBuf,0,dwLen+2);
		HttpQueryInfo(hOpenReq,HTTP_QUERY_RAW_HEADERS_CRLF,pHeaderBuf,&dwLen,NULL);
		pTmp = pHeaderBuf + 100;
		pBegin = wcsstr(pTmp,uzAmdKey);
		if (pBegin > 0)
		{
			pBegin = pBegin + wcslen(uzAmdKey);
			pEnd = wcsstr(pBegin,TEXT("\n"));
			if (pEnd > 0)
			{
				dwLen = pEnd - pBegin;
				wmemcpy(uzBuffer,pBegin,dwLen);
			}

			WideCharToMultiByte(CP_ACP,0,uzBuffer,dwLen,szMdDataBuffer,1024,NULL,NULL);

			memcpy(lpXAppleMdData,szMdDataBuffer,strlen(szMdDataBuffer));

		}
		if (pHeaderBuf)
		{
			delete []pHeaderBuf;
			pHeaderBuf = NULL;
		}
	}

	return;
}

void CViewPage::getxpClientIdValue(HINTERNET hOpenReq,
								   string &strXpClientId,
								   string &strXpab,
								   string &strXpabc,
								   string &strXpClientId_value,
								   string &strXpab_value,
								   string &strXpabc_value)
{
	string strxp_ci;
	string strxp_ab;
	string strxp_abc;
	string strxp_ci_value;
	string strxp_ab_value;
	string strxp_abc_value;
	string strResHeaderData;
	string strSubData;
	int beginPos = 0;
	int endPos = 0;
	DWORD headSize = 0;
	char* pAnsiiBuf = NULL;

	HttpQueryInfo(hOpenReq,HTTP_QUERY_RAW_HEADERS_CRLF,NULL,&headSize,NULL);

	LPVOID pHeaderBuf = VirtualAlloc(NULL,headSize,MEM_COMMIT | MEM_RESERVE,PAGE_EXECUTE_READWRITE);
	if (!pHeaderBuf)
	{
		return;
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
			return;
		}
		memset(pAnsiiBuf,0,actualSize);
		WideCharToMultiByte(CP_ACP,0,(LPCWSTR)pHeaderBuf,headSize,pAnsiiBuf,actualSize,NULL,NULL);
		strResHeaderData = pAnsiiBuf;
	}
	else//ANSII
	{
		strResHeaderData = (char*)pHeaderBuf;
	}

	beginPos = strResHeaderData.find("xp_ci=");
	if (beginPos >= 0)
	{
		strSubData = strResHeaderData.substr(beginPos);
		endPos = strSubData.find(";");
		strxp_ci = strSubData.substr(0,endPos);
		strxp_ci_value = strxp_ci.substr(6,endPos);

		strXpClientId = strxp_ci;
		strXpClientId_value = strxp_ci_value;

	}

	beginPos = strResHeaderData.find("xp_ab=");
	if (beginPos >= 0)
	{
		strSubData = strResHeaderData.substr(beginPos);
		endPos = strSubData.find(";");
		strxp_ab = strSubData.substr(0,endPos);
		strxp_ab_value = strxp_ab.substr(6,endPos);

		strXpab = strxp_ab;
		strXpab_value = strxp_ab_value;

		beginPos = strResHeaderData.find("xp_abc=");
		if (beginPos >= 0)
		{
			strSubData = strResHeaderData.substr(beginPos);
			endPos = strSubData.find(";");
			strxp_abc = strSubData.substr(0,endPos);
			strxp_abc_value = strxp_abc.substr(7,endPos);

			strXpabc = strxp_abc;
			strXpabc_value = strxp_abc_value;

		}
		else
		{
			//在xp_abc中截取xp_abc的值
			string strTmp = "";
			strxp_abc = "xp_abc=";
			beginPos = strxp_ab.rfind("+");

			strTmp = strxp_ab.substr(beginPos+1,strxp_ab.length());
			strXpabc_value = strTmp;

			strxp_abc += strTmp;
			strXpabc = strxp_abc;
		}

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

	return;
}

void CViewPage::getSpimValue(char* lpDecodeData,char* lpSpimData)
{
	CString strSrcData;
	CString strTargetValue;
	CString strTmp;
	CString strSpimKey = TEXT("<key>spim</key>");

	if (!lpDecodeData)
	{
		return;
	}
	int dataLen = strlen(lpDecodeData);
	TCHAR* lpwcData = new TCHAR[dataLen*2+2];
	if (!lpwcData)
	{
		return;
	}
	memset(lpwcData,0,dataLen*2+2);
	MultiByteToWideChar(CP_UTF8,0,lpDecodeData,dataLen,lpwcData,dataLen*2);

	strSrcData = lpwcData;

	int pos = strSrcData.Find(strSpimKey);
	if (pos < 0)
	{
		goto EXIT1;
	}
	strTmp = strSrcData.Right(strSrcData.GetLength()-pos-strSpimKey.GetLength());
	pos = strTmp.Find(TEXT("<string>"));
	if (pos < 0 )
	{
		goto EXIT1;
	}
	strTmp = strTmp.Right( strTmp.GetLength()-pos-wcslen(TEXT("<string>")) );
	pos = strTmp.Find(TEXT("</string>"));
	if (pos < 0)
	{
		goto EXIT1;
	}
	strTargetValue = strTmp.Left(pos);

	WideCharToMultiByte(CP_ACP,0,strTargetValue.GetBuffer(),strTargetValue.GetLength(),lpSpimData,512,NULL,NULL);

EXIT1:
	if (lpwcData)
	{
		delete []lpwcData;
		lpwcData = NULL;
	}

	return;
}

void CViewPage::getTKandPTMValue(char* lpDecodeData,char* lpTKData,char* lpPtmData)
{
	CString strSrcData;
	CString strTargetValue;
	CString strTmp;
	CString strTKKey = TEXT("<key>tk</key>");
	CString strPtmKey = TEXT("<key>ptm</key>");

	if (!lpDecodeData)
	{
		return;
	}
	int dataLen = strlen(lpDecodeData);
	TCHAR* lpwcData = new TCHAR[dataLen*2+2];
	if (!lpwcData)
	{
		return;
	}
	memset(lpwcData,0,dataLen*2+2);
	MultiByteToWideChar(CP_UTF8,0,lpDecodeData,dataLen,lpwcData,dataLen*2);

	strSrcData = lpwcData;

	//提取tk的值
	int pos = strSrcData.Find(strTKKey);
	if (pos < 0)
	{
		goto EXIT1;
	}
	strTmp = strSrcData.Right(strSrcData.GetLength()-pos-strTKKey.GetLength());
	pos = strTmp.Find(TEXT("<string>"));
	if (pos < 0 )
	{
		goto EXIT1;
	}
	strTmp = strTmp.Right( strTmp.GetLength()-pos-wcslen(TEXT("<string>")) );
	pos = strTmp.Find(TEXT("</string>"));
	if (pos < 0)
	{
		goto EXIT1;
	}
	strTargetValue = strTmp.Left(pos);

	WideCharToMultiByte(CP_ACP,0,strTargetValue.GetBuffer(),strTargetValue.GetLength(),lpTKData,32,NULL,NULL);

	//提取ptm的值
	pos = strSrcData.Find(strPtmKey);
	if (pos < 0)
	{
		goto EXIT1;
	}
	strTmp = strSrcData.Right(strSrcData.GetLength()-pos-strPtmKey.GetLength());
	pos = strTmp.Find(TEXT("<string>"));
	if (pos < 0 )
	{
		goto EXIT1;
	}
	strTmp = strTmp.Right( strTmp.GetLength()-pos-wcslen(TEXT("<string>")) );
	pos = strTmp.Find(TEXT("</string>"));
	if (pos < 0)
	{
		goto EXIT1;
	}
	strTargetValue = strTmp.Left(pos);

	WideCharToMultiByte(CP_ACP,0,strTargetValue.GetBuffer(),strTargetValue.GetLength(),lpPtmData,512,NULL,NULL);
EXIT1:
	if (lpwcData)
	{
		delete []lpwcData;
		lpwcData = NULL;
	}

	return;
}


void CViewPage::getSettingInfoAndtransportKeyValue(char* lpDecodeData,char* lpSettingInfo,char* lpTransportKey)
{
	CString strSrcData;
	CString strTargetValue;
	CString strTmp;
	CString strSettingInfoKey = TEXT("<key>settingInfo</key>");
	CString strTransportKey = TEXT("transportKey");

	if (!lpDecodeData)
	{
		return;
	}
	int dataLen = strlen(lpDecodeData);
	TCHAR* lpwcData = new TCHAR[dataLen*2+2];
	if (!lpwcData)
	{
		return;
	}
	memset(lpwcData,0,dataLen*2+2);
	MultiByteToWideChar(CP_UTF8,0,lpDecodeData,dataLen,lpwcData,dataLen*2);

	strSrcData = lpwcData;

	//提取SettingInfo的值
	int pos = strSrcData.Find(strSettingInfoKey);
	if (pos < 0)
	{
		goto EXIT1;
	}
	strTmp = strSrcData.Right(strSrcData.GetLength()-pos-strSettingInfoKey.GetLength());
	pos = strTmp.Find(TEXT("<string>"));
	if (pos < 0 )
	{
		goto EXIT1;
	}
	strTmp = strTmp.Right( strTmp.GetLength()-pos-wcslen(TEXT("<string>")) );
	pos = strTmp.Find(TEXT("</string>"));
	if (pos < 0)
	{
		goto EXIT1;
	}
	strTargetValue = strTmp.Left(pos);

	WideCharToMultiByte(CP_ACP,0,strTargetValue.GetBuffer(),strTargetValue.GetLength(),lpSettingInfo,512,NULL,NULL);

	//提取strTransportKey的值
	pos = strSrcData.Find(strTransportKey);
	if (pos < 0)
	{
		goto EXIT1;
	}
	strTmp = strSrcData.Right(strSrcData.GetLength()-pos-strTransportKey.GetLength());
	pos = strTmp.Find(TEXT("<string>"));
	if (pos < 0 )
	{
		goto EXIT1;
	}
	strTmp = strTmp.Right( strTmp.GetLength()-pos-wcslen(TEXT("<string>")) );
	pos = strTmp.Find(TEXT("</string>"));
	if (pos < 0)
	{
		goto EXIT1;
	}
	strTargetValue = strTmp.Left(pos);

	WideCharToMultiByte(CP_ACP,0,strTargetValue.GetBuffer(),strTargetValue.GetLength(),lpTransportKey,MAX_PATH,NULL,NULL);
EXIT1:
	if (lpwcData)
	{
		delete []lpwcData;
		lpwcData = NULL;
	}

	return;
}

void CViewPage::WriteDataToFile(string strSrcData)
{
	HANDLE hFile = NULL;
	CString strPath;
	TCHAR strFileName[MAX_PATH] = {0};
	DWORD dwNumOfByWritten = 0;
	WORD wFormat = 0xfeef; //unicode: 0xfeef; utf-8:0xefbbbf

	USES_CONVERSION;

	//创建文件路径
	GetModuleFileName(NULL, strFileName, MAX_PATH);
	strPath = strFileName;
	strPath = strPath.Left(strPath.ReverseFind(_T('\\')));
	strPath += _T("\\appleIdInfo.txt");

	//判断文件是否存在
	if (!PathFileExists(strPath))  
	{
		//创建文件
		hFile = CreateFile(strPath, GENERIC_WRITE|GENERIC_READ, FILE_SHARE_WRITE | FILE_SHARE_READ, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
		if (hFile == INVALID_HANDLE_VALUE)
		{
			DWORD err = GetLastError();
			goto Exit0;
		}

		//以Unicode方式打开解决中文乱码问题
		if ( !WriteFile(hFile, &wFormat, sizeof(wFormat), &dwNumOfByWritten, NULL) )
		{
			goto Exit0;
		}

	}
	else
	{
		hFile = CreateFile(strPath, GENERIC_WRITE|GENERIC_READ, FILE_SHARE_WRITE | FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		if (hFile == INVALID_HANDLE_VALUE)
		{
			DWORD err = GetLastError();
			goto Exit0;
		}
	}

	SetFilePointer(hFile, 0, NULL, FILE_END);

	if (!WriteFile(hFile, strSrcData.c_str(), strSrcData.length(), &dwNumOfByWritten, NULL))
	{
		goto Exit0;
	}
Exit0:
	if (hFile)
	{
		CloseHandle(hFile);
		hFile = NULL;
	}
}

string CViewPage::UTF8ToGBK(string strUtf8)
{
	string strOutGBK = "";
	int len = MultiByteToWideChar(CP_UTF8, 0, strUtf8.c_str(), -1, NULL, 0);
	WCHAR *wszGBK = new WCHAR[len + 1];
	memset(wszGBK, 0, len * 2 + 2);
	MultiByteToWideChar(CP_UTF8, 0, strUtf8.c_str(), -1, wszGBK, len);
	len = WideCharToMultiByte(CP_ACP, 0, wszGBK, -1, NULL, 0, NULL, NULL);
	char *pszGBK = new char[len + 1];
	memset(pszGBK, 0, len + 1);
	WideCharToMultiByte(CP_ACP, 0, wszGBK, -1, pszGBK, len, NULL, NULL);
	strOutGBK = pszGBK;
	delete[]pszGBK;
	delete[]wszGBK;
	return strOutGBK;
}

void CViewPage::ParseAccountBlanceData(string strWebData)
{
	CStringA strKeyBagInfo = "<p class=\"instruction accountBalance\">";
	CStringA strEndTag = "</p>";
	CStringA strSrcData = strWebData.c_str();
	CStringA strTmp;
	int endPos = 0;
	int keyStrPos = strSrcData.Find(strKeyBagInfo);
	if (keyStrPos >= 0)
	{
		strTmp = strSrcData.Right(strSrcData.GetLength()-keyStrPos-strKeyBagInfo.GetLength());
		endPos = strTmp.Find(strEndTag);
		m_accountBalance = strTmp.Left(endPos);
	}
	return;	
}

void CViewPage::ParseWebData(string strWebData)
{
	CMemLock memLock;
	string strWriteInfo = "begin----->";
	Json::Reader reader;  
	Json::Value root;  

	//字符串转为JSON格式存于root
	if (!reader.parse(strWebData.c_str(), root))
	{
		return;
	}

	Json::Value data = root["data"];
	Json::Value attributes = data["attributes"];
	Json::Value account_Info = attributes["accountInfo"];

	Json::Value accountInfo =  account_Info["accountInfo"];
	Json::Value addressInfo = account_Info["addressInfo"];

	string strAccountName = accountInfo["acAccountName"].asString();
	string strAccountPassword = accountInfo["acAccountPassword"].asString();
	string strEmail = accountInfo["email"].asString();
	string strFirstName = accountInfo["firstName"].asString();
	string strLastName = accountInfo["lastName"].asString();
	bool hasValidCC = accountInfo["hasValidCC"].asBool();
	bool hasValidCCInDS = accountInfo["hasValidCCInDS"].asBool();

	string strAddressOfficialCity = addressInfo["addressOfficialCity"].asString();//市行政区
	string strAddressOfficialCountryCode = addressInfo["addressOfficialCountryCode"].asString();//国家代码 
	string strAddressOfficialLineFirst = addressInfo["addressOfficialLineFirst"].asString();//街道1
	string strAddressOfficialLineSecond = addressInfo["addressOfficialLineSecond"].asString();//街道2
	string strAddressOfficialLineThird = addressInfo["addressOfficialLineThird"].asString();  //街道3
	string strAddressOfficialPostalCode = addressInfo["addressOfficialPostalCode"].asString();//邮政编码
	string strAddressOfficialStateProvince = addressInfo["addressOfficialStateProvince"].asString();//省份
	string strPaymentMethodType = addressInfo["paymentMethodType=None"].asString();
	string strPhoneOfficeNumber = addressInfo["phoneOfficeNumber"].asString();//电话
	string strTranslatedDisplayNameForCC = addressInfo["translatedDisplayNameForCC"].asString();//付款方式

	//组装一个用户的信息
	strWriteInfo += m_appleId;
	strWriteInfo += ",";
	strWriteInfo += m_password;
	strWriteInfo += ",";
	if (!m_accountBalance.empty())
	{
		strWriteInfo += m_accountBalance;
	}

	if (!strAccountName.empty())
	{
		strWriteInfo += ",";
		strWriteInfo += strAccountName;
	}
	if (!strEmail.empty())
	{
		strWriteInfo += ",";
		strWriteInfo += strEmail;
	}
	if (!strFirstName.empty())
	{
		strWriteInfo += ",";
		strWriteInfo += strFirstName;
	}
	if (!strLastName.empty())
	{
		strWriteInfo += ",";
		strWriteInfo += strLastName;
	}

	strWriteInfo += ",";
	strWriteInfo += hasValidCC ? "有CC" : "没有CC";

	strWriteInfo += ",";
	strWriteInfo += hasValidCCInDS ? "有CCInDS" : "没有CCInDS";
	
	if (!strAddressOfficialCity.empty())
	{
		strWriteInfo += ",";
		strWriteInfo += strAddressOfficialCity;
	}
	if (!strAddressOfficialCountryCode.empty())
	{
		strWriteInfo += ",";
		strWriteInfo += strAddressOfficialCountryCode;
	}
	if (!strAddressOfficialLineFirst.empty())
	{
		strWriteInfo += ",";
		strWriteInfo += strAddressOfficialLineFirst;
	}
	if (!strAddressOfficialLineSecond.empty())
	{
		strWriteInfo += ",";
		strWriteInfo += strAddressOfficialLineSecond;
	}
	if (!strAddressOfficialLineThird.empty())
	{
		strWriteInfo += ",";
		strWriteInfo += strAddressOfficialLineThird;
	}
	if (!strAddressOfficialPostalCode.empty())
	{
		strWriteInfo += ",";
		strWriteInfo += strAddressOfficialPostalCode;
	}
	if (!strAddressOfficialStateProvince.empty())
	{
		strWriteInfo += ",";
		strWriteInfo += strAddressOfficialStateProvince;
	}
	if (!strPaymentMethodType.empty())
	{
		strWriteInfo += ",";
		strWriteInfo += strPaymentMethodType;
	}
	if (!strPhoneOfficeNumber.empty())
	{
		strWriteInfo += ",";
		strWriteInfo += strPhoneOfficeNumber;
	}
	if (!strTranslatedDisplayNameForCC.empty())
	{
		strWriteInfo += ",";
		strWriteInfo += strTranslatedDisplayNameForCC;
	}
	
	strWriteInfo += "<----end\r\n";

	//写文件
	memLock.Lock();
	WriteDataToFile(strWriteInfo);
	memLock.Unlock();
}

void CViewPage::parseA2kResponsePacakage(char* lpDecodeData)
{
	CString strSrcData;
	CString strTmp;
	CString strIKey = TEXT("<key>i</key>");
	CString strSKey = TEXT("<key>s</key>");
	CString strCKey = TEXT("<key>c</key>");
	CString strBKey = TEXT("<key>B</key>");

	if (!lpDecodeData)
	{
		return;
	}

	int dataLen = strlen(lpDecodeData);
	TCHAR* lpwcData = new TCHAR[dataLen*2+2];
	if (!lpwcData)
	{
		return;
	}
	memset(lpwcData,0,dataLen*2+2);
	MultiByteToWideChar(CP_UTF8,0,lpDecodeData,dataLen,lpwcData,dataLen*2);

	strSrcData = lpwcData;

	//提取i
	int pos = strSrcData.Find(strIKey);
	if (pos < 0)
	{
		goto EXIT1;
	}
	strTmp = strSrcData.Right(strSrcData.GetLength()-pos-strIKey.GetLength());
	pos = strTmp.Find(TEXT("<integer>"));
	if (pos < 0)
	{
		goto EXIT1;
	}
	strTmp = strTmp.Right(strTmp.GetLength()-pos-wcslen(TEXT("<integer>")));
	pos = strTmp.Find(TEXT("</integer>"));
	if (pos < 0)
	{
		goto EXIT1;
	}
	m_a2kResponseData.strI = strTmp.Left(pos);

	//提取s
	pos = strSrcData.Find(strSKey);
	if (pos < 0)
	{
		goto EXIT1;
	}
	strTmp = strSrcData.Right(strSrcData.GetLength()-pos-strSKey.GetLength());
	pos = strTmp.Find(TEXT("<data>"));
	if (pos < 0)
	{
		goto EXIT1;
	}
	strTmp = strTmp.Right(strTmp.GetLength()-pos-wcslen(TEXT("<data>")));
	pos = strTmp.Find(TEXT("</data>"));
	if (pos < 0)
	{
		goto EXIT1;
	}
	m_a2kResponseData.strS = strTmp.Left(pos);

	//提取c
	pos = strSrcData.Find(strCKey);
	if (pos < 0)
	{
		goto EXIT1;
	}
	strTmp = strSrcData.Right(strSrcData.GetLength()-pos-strCKey.GetLength());
	pos = strTmp.Find(TEXT("<string>"));
	if (pos < 0)
	{
		goto EXIT1;
	}
	strTmp = strTmp.Right(strTmp.GetLength()-pos-wcslen(TEXT("<string>")));
	pos = strTmp.Find(TEXT("</string>"));
	if (pos < 0)
	{
		goto EXIT1;
	}
	m_a2kResponseData.strC = strTmp.Left(pos);

	//提取B
	pos = strSrcData.Find(strBKey);
	if (pos < 0)
	{
		goto EXIT1;
	}
	strTmp = strSrcData.Right(strSrcData.GetLength()-pos-strBKey.GetLength());
	pos = strTmp.Find(TEXT("<data>"));
	if (pos < 0)
	{
		goto EXIT1;
	}
	strTmp = strTmp.Right(strTmp.GetLength()-pos-wcslen(TEXT("<data>")));
	pos = strTmp.Find(TEXT("</data>"));
	if (pos < 0)
	{
		goto EXIT1;
	}
	m_a2kResponseData.strB = strTmp.Left(pos);


EXIT1:
	if (lpwcData)
	{
		delete []lpwcData;
		lpwcData = NULL;
	}
	return;

}

BOOL CViewPage::parseM1ResponsePackage(char* lpDecodeData)
{
	CString strSrcData;
	CString strTmp;
	CString strCPDKey = TEXT("<key>spd</key>");
	CString strM2Key = TEXT("<key>M2</key>");
	CString strNPKey = TEXT("<key>np</key>");
	BOOL bResult = FALSE;

	if (!lpDecodeData)
	{
		return bResult;
	}

	int dataLen = strlen(lpDecodeData);
	TCHAR* lpwcData = new TCHAR[dataLen*2+2];
	if (!lpwcData)
	{
		return bResult;
	}
	memset(lpwcData,0,dataLen*2+2);
	MultiByteToWideChar(CP_UTF8,0,lpDecodeData,dataLen,lpwcData,dataLen*2);

	strSrcData = lpwcData;
	//提取spd
	int pos = strSrcData.Find(strCPDKey);
	if (pos < 0)
	{
		goto EXIT1;
	}
	strTmp = strSrcData.Right(strSrcData.GetLength()-pos-strCPDKey.GetLength());
	pos = strTmp.Find(TEXT("<data>"));
	if (pos < 0)
	{
		goto EXIT1;
	}
	strTmp = strTmp.Right(strTmp.GetLength()-pos-wcslen(TEXT("<data>")));
	pos = strTmp.Find(TEXT("</data>"));
	if (pos < 0)
	{
		goto EXIT1;
	}
	m_m1ResponseData.strCPD = strTmp.Left(pos);

	//提取M2
	pos = strSrcData.Find(strM2Key);
	if (pos < 0)
	{
		goto EXIT1;
	}
	strTmp = strSrcData.Right(strSrcData.GetLength()-pos-strM2Key.GetLength());
	pos = strTmp.Find(TEXT("<data>"));
	if (pos < 0)
	{
		goto EXIT1;
	}
	strTmp = strTmp.Right(strTmp.GetLength()-pos-wcslen(TEXT("<data>")));
	pos = strTmp.Find(TEXT("</data>"));
	if (pos < 0)
	{
		goto EXIT1;
	}
	m_m1ResponseData.strM2 = strTmp.Left(pos);

	//提取np
	pos = strSrcData.Find(strNPKey);
	if (pos < 0)
	{
		goto EXIT1;
	}
	strTmp = strSrcData.Right(strSrcData.GetLength()-pos-strNPKey.GetLength());
	pos = strTmp.Find(TEXT("<data>"));
	if (pos < 0)
	{
		goto EXIT1;
	}
	strTmp = strTmp.Right(strTmp.GetLength()-pos-wcslen(TEXT("<data>")));
	pos = strTmp.Find(TEXT("</data>"));
	if (pos < 0)
	{
		goto EXIT1;
	}
	m_m1ResponseData.strNP = strTmp.Left(pos);
	bResult = TRUE;


EXIT1:
	if (lpwcData)
	{
		delete []lpwcData;
		lpwcData = NULL;
	}
	return bResult;


}

BOOL CViewPage::agreeDataReport(CString strUserAgent,
								CString strBuyUrl,
								CString strActionHostName,
								CString strRefererUrl,
								TCHAR* lpDsid,
								TCHAR* lpAgreeKey,
								TCHAR* lpTermsId,
								TCHAR* lpWosid_lite,
								CString strCookie,
								TCHAR* lpProxy,
								int netType)
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

	CString strAgent = strUserAgent;

	char szSendData[MAX_PATH] = {0};
	char szAgreeKey[MAX_PATH] = {0};
	char szTermsId[32] = {0};
	char szWosid_lite[MAX_PATH] = {0};

	WideCharToMultiByte(CP_ACP,0,lpAgreeKey,-1,szAgreeKey,MAX_PATH,NULL,NULL);
	WideCharToMultiByte(CP_ACP,0,lpTermsId,-1,szTermsId,32,NULL,NULL);
	WideCharToMultiByte(CP_ACP,0,lpWosid_lite,-1,szWosid_lite,MAX_PATH,NULL,NULL);
	strcat(szSendData,"wosid-lite=");
	strcat(szSendData,szWosid_lite);
	strcat(szSendData,"&termsId=");
	strcat(szSendData,szTermsId);
	strcat(szSendData,"&");
	strcat(szSendData,szAgreeKey);
	strcat(szSendData,"=%E5%90%8C%E6%84%8F");

	int sendLen = strlen(szSendData);

	BOOL bCrackUrl = ::InternetCrackUrl(strBuyUrl,0,0,&uc);
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

		hOpenReq = HttpOpenRequest(hConnect,_T("POST"),strActionHostName,0,0,0,0x84801000|INTERNET_FLAG_NO_COOKIES,0);//0x84A01000是一个常数,注意
		if (!hOpenReq)
		{
			InternetCloseHandle(hConnect);
			InternetCloseHandle(hSession);
			return FALSE;
		}

		CString strHeaders = _T("\r\nUser-Agent: ");
		strHeaders += strAgent;
		strHeaders += _T("\r\nAccept-Language: zh-cn");
		strHeaders += _T("\r\nX-Apple-Store-Front: 143465-19,29");
		strHeaders += _T("\r\nX-Apple-Connection-Type: WiFi");
		strHeaders += _T("\r\nX-Apple-Partner: origin.0");
		strHeaders += _T("\r\nX-Apple-Client-Versions: GameCenter/2.0");
		strHeaders += _T("\r\nContent-Type: application/x-www-form-urlencoded");
		strHeaders += _T("\r\nAccept: text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8");
		strHeaders += _T("\r\nX-Dsid: ");
		strHeaders += lpDsid;
		strHeaders += _T("\r\nReferer: ");
		strHeaders += strRefererUrl;
		strHeaders += _T("\r\nOrigin: https://buy.itunes.apple.com");
		if (!strCookie.IsEmpty())
		{
			strHeaders += TEXT("\r\nCookie: ");
			strHeaders += strCookie;

		}
		strHeaders += _T("\r\nAccept-Encoding: gzip, deflate");
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

		bSendRequest = HttpSendRequestW(hOpenReq,NULL,-1,szSendData,sendLen);//第三个参数设置为负一
		if (!bSendRequest && GetLastError() == ERROR_INTERNET_INVALID_CA)
		{
			DWORD dwFlags;
			DWORD dwBuffLen = sizeof(dwFlags);
			InternetQueryOption (hOpenReq, INTERNET_OPTION_SECURITY_FLAGS,(LPVOID)&dwFlags, &dwBuffLen);
			dwFlags |= SECURITY_FLAG_IGNORE_UNKNOWN_CA;
			InternetSetOption (hOpenReq,INTERNET_OPTION_SECURITY_FLAGS,&dwFlags,sizeof (dwFlags) );

			//再次发送
			bSendRequest = HttpSendRequest(hOpenReq,NULL,-1,szSendData,sendLen);
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

BOOL CViewPage::secureTermsPageReport(CString strUserAgent,
									  CString strXpUrl,
									  CString strRefererUrl,
									  TCHAR* lpDsid,
									  CString strCookie,
									  TCHAR* lpProxy,
									  int netType)
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

	TCHAR eventtime[MAX_PATH] = {0};
	SYSTEMTIME sysTime;
	time_t unixTime;
	GetLocalTime(&sysTime);
	time(&unixTime);
	wsprintf(eventtime,TEXT("%ld%d%ld"),unixTime,sysTime.wMinute,sysTime.wMilliseconds);

	CString strAgent = strUserAgent;
	CString strXpReportHostName = TEXT("/report/2/xp_its_main?responseType=image&eventType=page&app=AppStore&eventSource=server&page=finance_MXSecureTermsPage&pageType=finance&dsid=");
	strXpReportHostName += lpDsid;
	strXpReportHostName += TEXT("&pageId=MXSecureTermsPage&eventTime=");
	strXpReportHostName += eventtime;


	BOOL bCrackUrl = ::InternetCrackUrl(strXpUrl,0,0,&uc);
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

		hOpenReq = HttpOpenRequest(hConnect,_T("GET"),strXpReportHostName,0,0,0,0x84801000|INTERNET_FLAG_NO_COOKIES,0);//0x84A01000是一个常数,注意
		if (!hOpenReq)
		{
			InternetCloseHandle(hConnect);
			InternetCloseHandle(hSession);
			return FALSE;
		}

		CString strHeaders = _T("\r\nUser-Agent: ");
		strHeaders += strAgent;
		strHeaders += _T("\r\nAccept-Language: zh-cn");
		strHeaders += _T("\r\nX-Apple-Store-Front: 143465-19,29");
		strHeaders += _T("\r\nX-Apple-Connection-Type: WiFi");
		strHeaders += _T("\r\nX-Apple-Partner: origin.0");
		strHeaders += _T("\r\nX-Apple-Client-Versions: GameCenter/2.0");
		strHeaders += _T("\r\nAccept: */*");
		strHeaders += _T("\r\nReferer: ");
		strHeaders += strRefererUrl;
		strHeaders += _T("\r\nX-Dsid: ");
		strHeaders += lpDsid;
		if (!strCookie.IsEmpty())
		{
			strHeaders += TEXT("\r\nCookie: ");
			strHeaders += strCookie;

		}
		strHeaders += _T("\r\nAccept-Encoding: gzip, deflate");
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

		if (dwQueryBuf != 204)
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

BOOL CViewPage::decodeServerData(HINTERNET hOpenReq,LPVOID lpBuffer,LPDWORD lpdwSize)
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
	lpgzipBuf =  (LPGZIP)VirtualAlloc(NULL,1024*80,MEM_COMMIT | MEM_RESERVE,PAGE_EXECUTE_READWRITE);//分配25K的虚拟内存
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

int CViewPage::checkPasswordIsCorrect(char* lpSrcData)
{
	CString		strSrcData;
	CString		strTmp;
	int			beginPos = 0;
	int			endPos = 0;
	int			status = STATUS_FAILED;
	TCHAR strErrorInfo[] = TEXT("Your Apple ID or password is incorrect");
	TCHAR strAccountLocked[] = TEXT("iForgotAppleIdLocked");

	if (!lpSrcData)
	{
		goto Exit0;
	}

	int dataLen = strlen(lpSrcData);
	TCHAR* lpwcData = new TCHAR[dataLen*2+2];
	if (!lpwcData)
	{
		goto Exit0;
	}
	memset(lpwcData,0,dataLen*2+2);
	MultiByteToWideChar(CP_ACP,0,lpSrcData,dataLen,lpwcData,dataLen*2);

	strSrcData = lpwcData;

	beginPos = strSrcData.Find(strErrorInfo);
	if (beginPos >= 0)
	{
		status = PASSWORD_ERROR;
		goto Exit0;
	}
	else
	{
		status = STATUS_SUCCESS;
	}

	beginPos = strSrcData.Find(strAccountLocked);
	if (beginPos >= 0)
	{
		status = ACCOUNT_LOCKED;
		goto Exit0;
	}
	else
	{
		status = STATUS_SUCCESS;
	}

Exit0:
	//释放内存
	if (lpwcData)
	{
		delete []lpwcData;
		lpwcData = NULL;
	}
	return status;

}

BOOL CViewPage::parseAgreeWebData(char* lpDecodeData,
								  TCHAR* lpAgreeWebActionHostName,
								  TCHAR* lpAgreeKey,
								  TCHAR* lpTermsId,
								  TCHAR* lpWosid_lite)
{
	CString strSrcData;
	CString strTargetValue;
	CString strActionHostName = TEXT("method=\"post\" novalidate=\"novalidate\" action=\"");
	CString strAgreeKey = TEXT("id=\"hiddenBottomRightButtonId\"");
	CString strAgreeKey2 = TEXT("class=\"pk-navbar-right\" type=\"submit\"");
	CString strMid = TEXT("name=\"");
	CString strTermsIdKey = TEXT("\" name=\"termsId\"");
	CString strWosid_liteKey = TEXT("\" name=\"wosid-lite\"");

	if(!lpDecodeData)
	{
		return FALSE;
	}

	int dataLen = strlen(lpDecodeData);
	TCHAR* lpwcData = new TCHAR[dataLen*2+2];
	if (!lpwcData)
	{
		return FALSE;
	}
	memset(lpwcData,0,dataLen*2+2);
	MultiByteToWideChar(CP_ACP,0,lpDecodeData,dataLen,lpwcData,dataLen*2);

	strSrcData = lpwcData;

	//开始检索提取页面"同意"动作的链接数据
	int beginPos = strSrcData.Find(strActionHostName);
	if (beginPos < 0)
	{
		if (lpwcData)
		{
			delete []lpwcData;
			lpwcData = NULL;
		}
		return FALSE;
	}
	CString strTmp = strSrcData.Right(strSrcData.GetLength()-beginPos-strActionHostName.GetLength());
	int endPos = strTmp.Find(TEXT("\">"));
	if (endPos < 0)
	{
		if (lpwcData)
		{
			delete []lpwcData;
			lpwcData = NULL;
		}
		return FALSE;
	}
	strTargetValue = strTmp.Left(endPos);
	wcscpy(lpAgreeWebActionHostName,strTargetValue.GetBuffer());

	//开始检索提取"同意"时的name "1.0.0.1.1.57.21.2.1.1.0.1.11.1.5.1"
	beginPos = strSrcData.Find(strAgreeKey);
	if (beginPos < 0)
	{
		beginPos = strSrcData.Find(strAgreeKey2);
		if (beginPos < 0)
		{
			if (lpwcData)
			{
				delete []lpwcData;
				lpwcData = NULL;
			}
			return FALSE;
		}
		strTmp = strSrcData.Right(strSrcData.GetLength()-beginPos-strAgreeKey2.GetLength());
	}
	else
	{
		strTmp = strSrcData.Right(strSrcData.GetLength()-beginPos-strAgreeKey.GetLength());
	}

	//再次提取
	beginPos = strTmp.Find(strMid);
	if (beginPos < 0)
	{
		if (lpwcData)
		{
			delete []lpwcData;
			lpwcData = NULL;
		}
		return FALSE;
	}
	strTmp = strTmp.Right(strTmp.GetLength()-beginPos-strMid.GetLength());
	endPos = strTmp.Find(TEXT("\" />"));
	if (endPos < 0)
	{
		if (lpwcData)
		{
			delete []lpwcData;
			lpwcData = NULL;
		}
		return TRUE;
	}
	strTargetValue = strTmp.Left(endPos);
	wcscpy(lpAgreeKey,strTargetValue.GetBuffer());

	//获取wosid-lite的值,反方向搜索
	endPos = strSrcData.Find(strWosid_liteKey);
	if (endPos < 0)
	{
		if (lpwcData)
		{
			delete []lpwcData;
			lpwcData = NULL;
		}
		return TRUE;
	}
	strTmp = strSrcData.Left(endPos);
	beginPos = strTmp.ReverseFind(TEXT('"'));
	strTargetValue = strTmp.Right(strTmp.GetLength()-beginPos-wcslen(TEXT("\"")));
	wcscpy(lpWosid_lite,strTargetValue.GetBuffer());


	//获取termsId的值
	endPos = strSrcData.Find(strTermsIdKey);
	if (endPos < 0)
	{
		if (lpwcData)
		{
			delete []lpwcData;
			lpwcData = NULL;
		}
		return TRUE;
	}
	strTmp = strSrcData.Left(endPos);
	beginPos = strTmp.ReverseFind(TEXT('"'));
	strTargetValue = strTmp.Right(strTmp.GetLength()-beginPos-wcslen(TEXT("\"")));
	wcscpy(lpTermsId,strTargetValue.GetBuffer());

	if (lpwcData)
	{
		delete []lpwcData;
		lpwcData = NULL;
	}

	return TRUE;
}

BOOL CViewPage::parseSecurityQustionData(char* lpDecodeData,TCHAR* lpSecurityActionHostName,TCHAR* lpSubmitKey,SECQUESTIONTONAME& securityMap)
{
	CString strSrcData;
	CString strTargetValue;
	CString strQuestion;
	CString strTextName;
	CString strAuthKey = TEXT("authenticationQuestions");
	CString strActionHostName = TEXT("method=\"post\" novalidate=\"novalidate\" action=\"");
	CString strSubmitKey = TEXT("id=\"hiddenFirstButtonId\" type=\"submit\" name=\"");
	CString strTextQuestionKey = TEXT("<label class=\"text question\"><span>");
	CString strTextNameKey = TEXT("type=\"text\" name=\"");

	if(!lpDecodeData)
	{
		return FALSE;
	}

	int dataLen = strlen(lpDecodeData);
	TCHAR* lpwcData = new TCHAR[dataLen*2+2];
	if (!lpwcData)
	{
		return FALSE;
	}
	memset(lpwcData,0,dataLen*2+2);
	MultiByteToWideChar(CP_UTF8,0,lpDecodeData,dataLen,lpwcData,dataLen*2);

	strSrcData = lpwcData;

	//开始检索提取页面"安全"问题发送动作的链接数据
	int beginPos = strSrcData.Find(strActionHostName);
	if (beginPos < 0)
	{
		if (lpwcData)
		{
			delete []lpwcData;
			lpwcData = NULL;
		}
		return FALSE;
	}
	CString strTmp = strSrcData.Right(strSrcData.GetLength()-beginPos-strActionHostName.GetLength());
	int endPos = strTmp.Find(TEXT("\">"));
	if (endPos < 0)
	{
		if (lpwcData)
		{
			delete []lpwcData;
			lpwcData = NULL;
		}
		return FALSE;
	}
	strTargetValue = strTmp.Left(endPos);
	wcscpy(lpSecurityActionHostName,strTargetValue.GetBuffer());

	//开始提取提交名字
	beginPos = strTmp.Find(strSubmitKey);
	if (beginPos < 0)
	{
		if (lpwcData)
		{
			delete []lpwcData;
			lpwcData = NULL;
		}
		return FALSE;
	}
	strTmp = strTmp.Right(strTmp.GetLength()-beginPos-strSubmitKey.GetLength());
	endPos = strTmp.Find(TEXT("\" />"));
	if (endPos < 0)
	{
		if (lpwcData)
		{
			delete []lpwcData;
			lpwcData = NULL;
		}
		return FALSE;
	}
	strTargetValue = strTmp.Left(endPos);
	wcscpy(lpSubmitKey,strTargetValue.GetBuffer());


	//开始提取第一个安全问题
	beginPos = strSrcData.Find(strAuthKey);
	if (beginPos < 0)
	{
		if (lpwcData)
		{
			delete []lpwcData;
			lpwcData = NULL;
		}
		return FALSE;
	}
	strTmp = strSrcData.Right(strSrcData.GetLength()-beginPos-strAuthKey.GetLength());
	beginPos = strTmp.Find(strTextQuestionKey);
	if (beginPos < 0)
	{
		if (lpwcData)
		{
			delete []lpwcData;
			lpwcData = NULL;
		}
		return FALSE;
	}
	strTmp = strTmp.Right(strTmp.GetLength()-beginPos-strTextQuestionKey.GetLength());
	endPos = strTmp.Find(TEXT("/span>"));
	if (endPos < 0)
	{
		if (lpwcData)
		{
			delete []lpwcData;
			lpwcData = NULL;
		}
		return FALSE;
	}
	strQuestion = strTmp.Left(endPos-1);
	//ReadStringToUnicode(strQuestion,CP_UTF8);
	//开始提取第一个安全问题的名字
	beginPos = strTmp.Find(strTextNameKey);
	if (beginPos < 0)
	{
		if (lpwcData)
		{
			delete []lpwcData;
			lpwcData = NULL;
		}
		return FALSE;
	}
	strTmp = strTmp.Right(strTmp.GetLength()-beginPos-strTextNameKey.GetLength());
	endPos = strTmp.Find(TEXT("\" />"));
	if (endPos < 0)
	{
		if (lpwcData)
		{
			delete []lpwcData;
			lpwcData = NULL;
		}
		return FALSE;	
	}
	strTextName = strTmp.Left(endPos);
	securityMap.insert(make_pair(strQuestion,strTextName));

	//开始提取第二个安全问题
	beginPos = strTmp.Find(strTextQuestionKey);
	if (beginPos < 0)
	{
		if (lpwcData)
		{
			delete []lpwcData;
			lpwcData = NULL;
		}
		return FALSE;
	}
	strTmp = strTmp.Right(strTmp.GetLength()-beginPos-strTextQuestionKey.GetLength());
	endPos = strTmp.Find(TEXT("</span>"));
	if (endPos < 0)
	{
		if (lpwcData)
		{
			delete []lpwcData;
			lpwcData = NULL;
		}
		return FALSE;
	}
	strQuestion = strTmp.Left(endPos);
	//ReadStringToUnicode(strQuestion,CP_UTF8);
	//开始提取第二个安全问题的名字
	beginPos = strTmp.Find(strTextNameKey);
	if (beginPos < 0)
	{
		if (lpwcData)
		{
			delete []lpwcData;
			lpwcData = NULL;
		}
		return FALSE;
	}
	strTmp = strTmp.Right(strTmp.GetLength()-beginPos-strTextNameKey.GetLength());
	endPos = strTmp.Find(TEXT("\" />"));
	if (endPos < 0)
	{
		if (lpwcData)
		{
			delete []lpwcData;
			lpwcData = NULL;
		}
		return FALSE;	
	}
	strTextName = strTmp.Left(endPos);
	securityMap.insert(make_pair(strQuestion,strTextName));

	if (lpwcData)
	{
		delete []lpwcData;
		lpwcData = NULL;
	}
	return TRUE;
}
BOOL CViewPage::xpReportTargetAppMain(CString strUserAgent,
									  char* lpSendData,
									  char* lpPasswordToken,
									  char* lpDsid,
									  CString strCookie,
									  CString strXAppleSignature,
									  TCHAR* lpX_Apple_I_md_m,
									  TCHAR* lpX_Apple_I_md,
									  TCHAR* lpProxy,
									  int netType)
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


	CString strUrl = _T("https://xp.apple.com");
	CString strAgent = strUserAgent;

	TCHAR strPasswordToken[MAX_PATH] = {0};
	TCHAR strDsid[MAX_PATH] = {0};
	TCHAR strHostName[MAX_PATH] = _T("/report/2/xp_its_main");
	char szFormatTime[MAX_PATH] = {0};
	TCHAR wzFormatTime[512] = {0};

	//压缩要发送内容
	CA2GZIP gzip(lpSendData,-1);
	LPGZIP pgzip = gzip.pgzip;
	int gzipLength = gzip.Length;

	MultiByteToWideChar(CP_ACP,0,(LPCSTR)lpPasswordToken,strlen(lpPasswordToken),strPasswordToken,MAX_PATH);
	MultiByteToWideChar(CP_ACP,0,(LPCSTR)lpDsid,strlen(lpDsid),strDsid,MAX_PATH);

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

		hOpenReq = HttpOpenRequest(hConnect,_T("POST"),strHostName,0,0,0,0x84801000|INTERNET_FLAG_NO_COOKIES,0);//0x84A01000是一个常数,注意
		if (!hOpenReq)
		{
			InternetCloseHandle(hConnect);
			InternetCloseHandle(hSession);
			return FALSE;
		}

		getFormatTime(szFormatTime);
		::MultiByteToWideChar(CP_ACP,0,szFormatTime,strlen(szFormatTime),wzFormatTime,512);

		CString strHeaders = _T("Host: xp.apple.com");	
		strHeaders += _T("\r\nAccept-Language: zh-Hans-CN");
		strHeaders += _T("\r\nUser-Agent: ");
		strHeaders += strAgent;
		strHeaders += _T("\r\nAccept: */*");
		strHeaders += _T("\r\nX-Apple-Store-Front: 143465-19,29");
		strHeaders += _T("\r\nX-Apple-Connection-Type: WiFi");
		strHeaders += _T("\r\nContent-Type: application/json; charset=utf-8");
		strHeaders += _T("\r\nX-Apple-I-Client-Time: ");
		strHeaders += wzFormatTime;
		strHeaders += _T("\r\nX-Dsid: ");
		strHeaders += strDsid;
		if (!strCookie.IsEmpty())
		{
			strHeaders += TEXT("\r\nCookie: ");
			strHeaders += strCookie;

		}
		if (!strXAppleSignature.IsEmpty())
		{
			strHeaders += TEXT("\r\nX-Apple-ActionSignature: ");
			strHeaders += strXAppleSignature;
		}
		strHeaders += _T("\r\nX-Apple-Partner: origin.0");
		strHeaders += _T("\r\nAccept-Encoding: gzip, deflate");
		strHeaders += _T("\r\nContent-Encoding: gzip");
		if (lpX_Apple_I_md_m[0] != TEXT('\0'))
		{
			strHeaders += _T("\r\nX-Apple-I-MD-RINFO: 17106176");
			strHeaders += _T("\r\nX-Apple-I-MD-M: ");
			strHeaders += lpX_Apple_I_md_m;
			strHeaders += _T("\r\nX-Apple-I-MD: ");
			strHeaders += lpX_Apple_I_md;
		}
		strHeaders += _T("\r\nX-Apple-Tz: 28800");
		strHeaders += _T("\r\nConnection: keep-alive\r\n\r\n");

		DWORD dwHeaderLength = wcslen(strHeaders);

		bAddRequestHeader = HttpAddRequestHeaders(hOpenReq,strHeaders,dwHeaderLength,0xA0000000);//0xA0000000是一个常量
		if (!bAddRequestHeader)
		{
			InternetCloseHandle(hOpenReq);
			InternetCloseHandle(hConnect);
			InternetCloseHandle(hSession);
			return FALSE;
		}

		bSendRequest = HttpSendRequest(hOpenReq,NULL,-1,pgzip,gzipLength);//第三个参数设置为负一
		if (!bSendRequest && GetLastError() == ERROR_INTERNET_INVALID_CA)
		{
			DWORD dwFlags;
			DWORD dwBuffLen = sizeof(dwFlags);
			InternetQueryOption (hOpenReq, INTERNET_OPTION_SECURITY_FLAGS,(LPVOID)&dwFlags, &dwBuffLen);
			dwFlags |= SECURITY_FLAG_IGNORE_UNKNOWN_CA;
			InternetSetOption (hOpenReq,INTERNET_OPTION_SECURITY_FLAGS,&dwFlags,sizeof (dwFlags) );

			//再次发送
			bSendRequest = HttpSendRequest(hOpenReq,NULL,-1,pgzip,gzipLength);
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

BOOL CViewPage::xpAmpClientPerf(CString strUserAgent,
								char* lpSendData,
								char* lpDsid,
								CString strCookie,
								CString strXAppleSignature,
								TCHAR* lpX_Apple_I_md_m,
								TCHAR* lpX_Apple_I_md,
								TCHAR* lpProxy,
								int netType)
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


	CString strUrl = _T("https://xp.apple.com");
	CString strAgent = strUserAgent;

	TCHAR strPasswordToken[MAX_PATH] = {0};
	TCHAR strDsid[MAX_PATH] = {0};
	TCHAR strHostName[MAX_PATH] = _T("/report/2/xp_amp_clientperf");
	char szFormatTime[MAX_PATH] = {0};
	TCHAR wzFormatTime[512] = {0};

	//压缩要发送内容
	CA2GZIP gzip(lpSendData,-1);
	LPGZIP pgzip = gzip.pgzip;
	int gzipLength = gzip.Length;

	MultiByteToWideChar(CP_ACP,0,(LPCSTR)lpDsid,strlen(lpDsid),strDsid,MAX_PATH);

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

		hOpenReq = HttpOpenRequest(hConnect,_T("POST"),strHostName,0,0,0,0x84801000|INTERNET_FLAG_NO_COOKIES,0);//0x84A01000是一个常数,注意
		if (!hOpenReq)
		{
			InternetCloseHandle(hConnect);
			InternetCloseHandle(hSession);
			return FALSE;
		}

		getFormatTime(szFormatTime);
		::MultiByteToWideChar(CP_ACP,0,szFormatTime,strlen(szFormatTime),wzFormatTime,512);

		CString strHeaders = _T("Host: xp.apple.com");	
		strHeaders += _T("\r\nAccept-Language: zh-Hans-CN");
		strHeaders += _T("\r\nUser-Agent: ");
		strHeaders += strAgent;
		strHeaders += _T("\r\nAccept: */*");
		strHeaders += _T("\r\nX-Apple-Store-Front: 143465-19,29");
		strHeaders += _T("\r\nX-Apple-Connection-Type: WiFi");
		strHeaders += _T("\r\nContent-Type: application/json; charset=utf-8");
		strHeaders += _T("\r\nX-Apple-I-Client-Time: ");
		strHeaders += wzFormatTime;
		strHeaders += _T("\r\nX-Dsid: ");
		strHeaders += strDsid;
		if (!strCookie.IsEmpty())
		{
			strHeaders += TEXT("\r\nCookie: ");
			strHeaders += strCookie;

		}
		if (!strXAppleSignature.IsEmpty())
		{
			strHeaders += TEXT("\r\nX-Apple-ActionSignature: ");
			strHeaders += strXAppleSignature;
		}
		strHeaders += _T("\r\nAccept-Encoding: gzip, deflate");
		strHeaders += _T("\r\nContent-Encoding: gzip");
		if (lpX_Apple_I_md_m[0] != TEXT('\0'))
		{
			strHeaders += _T("\r\nX-Apple-I-MD-RINFO: 17106176");
			strHeaders += _T("\r\nX-Apple-I-MD-M: ");
			strHeaders += lpX_Apple_I_md_m;
			strHeaders += _T("\r\nX-Apple-I-MD: ");
			strHeaders += lpX_Apple_I_md;
		}
		strHeaders += _T("\r\nX-Apple-Tz: 28800");
		strHeaders += _T("\r\nConnection: keep-alive\r\n\r\n");

		DWORD dwHeaderLength = wcslen(strHeaders);

		bAddRequestHeader = HttpAddRequestHeaders(hOpenReq,strHeaders,dwHeaderLength,0xA0000000);//0xA0000000是一个常量
		if (!bAddRequestHeader)
		{
			InternetCloseHandle(hOpenReq);
			InternetCloseHandle(hConnect);
			InternetCloseHandle(hSession);
			return FALSE;
		}

		bSendRequest = HttpSendRequest(hOpenReq,NULL,-1,pgzip,gzipLength);//第三个参数设置为负一
		if (!bSendRequest && GetLastError() == ERROR_INTERNET_INVALID_CA)
		{
			DWORD dwFlags;
			DWORD dwBuffLen = sizeof(dwFlags);
			InternetQueryOption (hOpenReq, INTERNET_OPTION_SECURITY_FLAGS,(LPVOID)&dwFlags, &dwBuffLen);
			dwFlags |= SECURITY_FLAG_IGNORE_UNKNOWN_CA;
			InternetSetOption (hOpenReq,INTERNET_OPTION_SECURITY_FLAGS,&dwFlags,sizeof (dwFlags) );

			//再次发送
			bSendRequest = HttpSendRequest(hOpenReq,NULL,-1,pgzip,gzipLength);
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

BOOL CViewPage::xpAppBuyReport(CString strUserAgent, char* lpSaleId,TCHAR* lpProxy,int netType)
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


	CString strUrl = _T("https://xp.apple.com");
	CString strAgent = strUserAgent;

	TCHAR strHostName[MAX_PATH] = {0};
	TCHAR szSaleId[64] = {0};

	MultiByteToWideChar(CP_ACP,0,lpSaleId,strlen(lpSaleId),szSaleId,64);

	swprintf(strHostName,_T("/report/2/xp_app_buy?clientId=0&sf=143465&adamId=%s"),szSaleId);


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

		CString strHeaders = _T("Host: xp.apple.com");	
		strHeaders += _T("\r\nAccept-Language: zh-Hans-CN");
		strHeaders += _T("\r\nUser-Agent: ");
		strHeaders += strAgent;
		strHeaders += _T("\r\nAccept: */*");
		strHeaders += _T("\r\nAccept-Encoding: gzip, deflate");
		strHeaders += _T("\r\nX-Apple-I-Client-Time: ");
		strHeaders += GetClientTime();
		strHeaders += _T("\r\nX-Apple-Tz: 28800");
		strHeaders += _T("\r\nConnection: keep-alive\r\n\r\n");

		DWORD dwHeaderLength = wcslen(strHeaders);

		bAddRequestHeader = HttpAddRequestHeaders(hOpenReq,strHeaders,dwHeaderLength,0xA0000000);//0xA0000000是一个常量
		if (!bAddRequestHeader)
		{
			InternetCloseHandle(hOpenReq);
			InternetCloseHandle(hConnect);
			InternetCloseHandle(hSession);
			return FALSE;
		}

		bSendRequest = HttpSendRequest(hOpenReq,NULL,-1,NULL,0);//第三个参数设置为负一
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

BOOL CViewPage::SendGsasServicesPostData(CString strUserAgent,
										 CString strMmeClientInfo,
										 char* lpSendData,
										 CString strXAppleHBToken,
										 CString strUdid,
										 CString strSerialNo,
										 CString strCookie,
										 TCHAR* lpX_Apple_I_md_m,
										 TCHAR* lpX_Apple_I_md,
										 TCHAR* lpProxy,
										 int netType)
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


	CString strUrl = _T("https://gsas.apple.com");
	CString strAgent = strUserAgent;

	TCHAR strPasswordToken[MAX_PATH] = {0};
	TCHAR strHostName[MAX_PATH] = _T("/grandslam/GsService2/postdata");
	char szFormatTime[MAX_PATH] = {0};
	TCHAR wzFormatTime[512] = {0};

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

		hOpenReq = HttpOpenRequest(hConnect,_T("POST"),strHostName,0,0,0,0x84801000|INTERNET_FLAG_NO_COOKIES,0);//0x84A01000是一个常数,注意
		if (!hOpenReq)
		{
			InternetCloseHandle(hConnect);
			InternetCloseHandle(hSession);
			return FALSE;
		}

		getFormatTime(szFormatTime);
		::MultiByteToWideChar(CP_ACP,0,szFormatTime,strlen(szFormatTime),wzFormatTime,512);

		CString strHeaders = _T("Host: gsas.apple.com");	
		strHeaders += _T("\r\nAccept-Language: zh-cn");
		strHeaders += _T("\r\nUser-Agent: ");
		strHeaders += strAgent;
		strHeaders += _T("\r\nX-Apple-I-SRL-NO: ");
		strHeaders += strSerialNo;
		strHeaders += _T("\r\nAccept: */*");
		strHeaders += _T("\r\nX-Apple-Store-Front: 143465-19,29");
		strHeaders += _T("\r\nContent-Type: application/x-www-form-urlencoded");
		strHeaders += _T("\r\nX-Apple-I-Client-Time: ");
		strHeaders += wzFormatTime;
		if (!strCookie.IsEmpty())
		{
			strHeaders += TEXT("\r\nCookie: ");
			strHeaders += strCookie;

		}

		strHeaders += _T("\r\nAccept-Encoding: gzip, deflate");
		if (lpX_Apple_I_md_m[0] != TEXT('\0'))
		{
			strHeaders += _T("\r\nX-Apple-I-MD-RINFO: 17106176");
			strHeaders += _T("\r\nX-Apple-I-MD-M: ");
			strHeaders += lpX_Apple_I_md_m;
			strHeaders += _T("\r\nX-Apple-I-MD: ");
			strHeaders += lpX_Apple_I_md;
		}
		strHeaders += _T("\r\nX-MMe-Client-Info: ");
		strHeaders += strMmeClientInfo;
		strHeaders += _T("\r\nX-Mme-Device-Id: ");
		strHeaders += strUdid;
		strHeaders += _T("\r\nX-Apple-HB-Token: ");
		strHeaders += strXAppleHBToken;
		strHeaders += _T("\r\nConnection: keep-alive\r\n\r\n");

		DWORD dwHeaderLength = wcslen(strHeaders);

		bAddRequestHeader = HttpAddRequestHeaders(hOpenReq,strHeaders,dwHeaderLength,0xA0000000);//0xA0000000是一个常量
		if (!bAddRequestHeader)
		{
			InternetCloseHandle(hOpenReq);
			InternetCloseHandle(hConnect);
			InternetCloseHandle(hSession);
			return FALSE;
		}

		bSendRequest = HttpSendRequest(hOpenReq,NULL,-1,lpSendData, strlen(lpSendData));//第三个参数设置为负一
		if (!bSendRequest && GetLastError() == ERROR_INTERNET_INVALID_CA)
		{
			DWORD dwFlags;
			DWORD dwBuffLen = sizeof(dwFlags);
			InternetQueryOption (hOpenReq, INTERNET_OPTION_SECURITY_FLAGS,(LPVOID)&dwFlags, &dwBuffLen);
			dwFlags |= SECURITY_FLAG_IGNORE_UNKNOWN_CA;
			InternetSetOption (hOpenReq,INTERNET_OPTION_SECURITY_FLAGS,&dwFlags,sizeof (dwFlags) );

			//再次发送
			bSendRequest = HttpSendRequest(hOpenReq, NULL, -1, lpSendData, strlen(lpSendData));
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

BOOL CViewPage::sendBuyButtonMetaData(CString strUserAgent,
									  char* lpSendContext,
									  char* lpDsid,
									  char* lpToken,
									  CString strCookie,
									  TCHAR* lpX_Apple_I_md_m,
									  TCHAR* lpX_Apple_I_md,
									  TCHAR* lpProxy,int netType)
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

	CString strUrl = _T("https://se.itunes.apple.com");
	CString strAgent = strUserAgent;
	TCHAR strHostName[MAX_PATH] = _T("/WebObjects/MZStoreElements.woa/wa/buyButtonMetaData");

	TCHAR strPasswordToken[MAX_PATH] = {0};
	TCHAR strReferer[MAX_PATH] = {0};
	TCHAR strDsid[MAX_PATH] = {0};
	char szFormatTime[MAX_PATH] = {0};
	TCHAR wzFormatTime[512] = {0};

	if (lpToken && lpToken[0] != '\0')
	{
		MultiByteToWideChar(CP_ACP,0,(LPCSTR)lpToken,strlen(lpToken),strPasswordToken,MAX_PATH);
	}
	MultiByteToWideChar(CP_ACP,0,(LPCSTR)lpDsid,strlen(lpDsid),strDsid,MAX_PATH);

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

		hOpenReq = HttpOpenRequest(hConnect,_T("POST"),strHostName,0,0,0,0x84801000|INTERNET_FLAG_NO_COOKIES,0);//0x84A01000是一个常数,注意
		if (!hOpenReq)
		{
			InternetCloseHandle(hConnect);
			InternetCloseHandle(hSession);
			return FALSE;
		}

		CViewPage::getFormatTime(szFormatTime);
		::MultiByteToWideChar(CP_ACP,0,szFormatTime,strlen(szFormatTime),wzFormatTime,512);

		CString strHeaders = _T("User-Agent: ");
		strHeaders += strAgent;
		if (lpToken && lpToken[0] != '\0')
		{
			strHeaders += _T("\r\nX-Token: ");
			strHeaders += strPasswordToken;
		}
		strHeaders += _T("\r\nContent-Type: application/x-www-form-urlencoded");
		strHeaders += _T("\r\nX-Apple-Store-Front: 143465-19,29");
		strHeaders += _T("\r\nAccept: */*");
		strHeaders += _T("\r\nX-Apple-I-Client-Time: ");
		strHeaders += wzFormatTime;

		if (lpX_Apple_I_md_m[0] != TEXT('\0'))
		{
			strHeaders += _T("\r\nX-Apple-I-MD-RINFO: 17106176");
			strHeaders += _T("\r\nX-Apple-I-MD-M: ");
			strHeaders += lpX_Apple_I_md_m;
			strHeaders += _T("\r\nX-Apple-I-MD: ");
			strHeaders += lpX_Apple_I_md;
		}

		strHeaders += _T("\r\nAccept-Language: zh-Hans");
		strHeaders += _T("\r\nX-Dsid: ");
		strHeaders += strDsid;

		if (!strCookie.IsEmpty())
		{
			strHeaders += TEXT("\r\nCookie: ");
			strHeaders += strCookie;

		}
		strHeaders += _T("\r\nX-Apple-Tz: 28800");
		strHeaders += _T("\r\nX-Apple-Partner: origin.0");
		strHeaders += TEXT("\r\nX-Apple-Connection-Type: WiFi");
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

		bSendRequest = HttpSendRequestW(hOpenReq,NULL,-1,lpSendContext,strlen(lpSendContext));//第三个参数设置为负一
		if (!bSendRequest && GetLastError() == ERROR_INTERNET_INVALID_CA)
		{
			DWORD dwFlags;
			DWORD dwBuffLen = sizeof(dwFlags);
			InternetQueryOption (hOpenReq, INTERNET_OPTION_SECURITY_FLAGS,(LPVOID)&dwFlags, &dwBuffLen);
			dwFlags |= SECURITY_FLAG_IGNORE_UNKNOWN_CA;
			InternetSetOption (hOpenReq,INTERNET_OPTION_SECURITY_FLAGS,&dwFlags,sizeof (dwFlags) );

			//再次发送
			bSendRequest = HttpSendRequest(hOpenReq,NULL,-1,lpSendContext,strlen(lpSendContext));
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

vector<string>& CViewPage::GetIdsList()
{
	return m_vecIdsList;
}


int CViewPage::gzcompress22(unsigned char* data,unsigned long ndata,unsigned char* zdata,unsigned nzdata)
{
	z_stream c_stream;
	/*  int err = 0;

	if(data && ndata > 0) {
	c_stream.zalloc = NULL;
	c_stream.zfree = NULL;
	c_stream.opaque = NULL;
	//只有设置为MAX_WBITS + 16才能在在压缩文本中带header和trailer
	if(deflateInit2(&c_stream, Z_DEFAULT_COMPRESSION, Z_DEFLATED,MAX_WBITS + 16, 8, Z_DEFAULT_STRATEGY) != Z_OK)
	{
	return -1;
	}
	c_stream.next_in  = data;
	c_stream.avail_in  = ndata;
	c_stream.next_out = zdata;
	c_stream.avail_out  = *nzdata;
	while(c_stream.avail_in != 0 && c_stream.total_out < *nzdata) 
	{
	if(deflate(&c_stream, Z_NO_FLUSH) != Z_OK) 
	{
	return -1;
	}
	}
	if(c_stream.avail_in != 0) return c_stream.avail_in;
	for(;;) {
	if((err = deflate(&c_stream, Z_FINISH)) == Z_STREAM_END) break;
	if(err != Z_OK) return -1;
	}
	if(deflateEnd(&c_stream) != Z_OK) return -1;
	*nzdata = c_stream.total_out;
	return 0;
	}*/
	return -1;
}

void CViewPage::getFormatTime(char* lpFormatTime)
{
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

	strcpy(lpFormatTime,szTimeStamp);
	return;

}

CString CViewPage::GetClientTime(string& strTimeStamp)
{
	CString strTime;
	char szTimeStamp[MAX_PATH] = {0};
	int day = 0;
	int hour = 0;
	CTime tDateTime= CTime::GetCurrentTime();
	strTimeStamp = getTimeStamp();

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

CString CViewPage::GetClientTime()
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


void CViewPage::SetSerialNumber(string strSerialNo)
{
	m_strSerialNumber = strSerialNo;
}

string CViewPage::getLaunchCorrelationKey()
{
	string strLaunchKey = "";
	char guidBuf[64] = {0};
	GUID guid;
	CoInitialize(NULL);
	if(S_OK == CoCreateGuid(&guid))
	{
		sprintf_s(
			guidBuf,
			"%08X-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X",
			guid.Data1, guid.Data2, guid.Data3,
			guid.Data4[0], guid.Data4[1],
			guid.Data4[2], guid.Data4[3],
			guid.Data4[4], guid.Data4[5],
			guid.Data4[6], guid.Data4[7]);
	}

	CoUninitialize();
	strLaunchKey = guidBuf;

	return strLaunchKey;
}


void CViewPage::setXpEventTime()
{
	string strEventtime = "";
	char eventtime[128] = {0};
	SYSTEMTIME sysTime;
	time_t unixTime;
	GetLocalTime(&sysTime);
	time(&unixTime);

	__int64 timestamp = unixTime*1000 + sysTime.wMilliseconds;
	strEventtime= _i64toa(timestamp,eventtime,10);

	m_vecEventtime.push_back(strEventtime);

	return;
}

void CViewPage::getEventTime(char* lpTimeData,BOOL IsSecond,BOOL IsMiSecond)
{
	LARGE_INTEGER timeRand; 
	int nRand = 0;
	char eventtime[128] = {0};
	SYSTEMTIME sysTime;
	time_t unixTime;

	if (IsMiSecond)//毫秒间隔计算时间戳
	{
		QueryPerformanceCounter(&timeRand);
		//取整数的高2位
		nRand = timeRand.LowPart%100;
		Sleep(nRand);
	}

	if (IsSecond)
	{
		QueryPerformanceCounter(&timeRand);
		nRand = timeRand.LowPart%1000;
		Sleep(nRand);
	}

	GetLocalTime(&sysTime);
	time(&unixTime);
	__int64 timestamp = unixTime*1000 + sysTime.wMilliseconds;

	_i64toa(timestamp,eventtime,10);
	strcpy(lpTimeData,eventtime);
	return;
}

string CViewPage::getTimeStamp()
{
	string strEventtime = "";
	char eventtime[128] = {0};
	SYSTEMTIME sysTime;
	time_t unixTime;
	GetLocalTime(&sysTime);
	time(&unixTime);

	__int64 timestamp = unixTime*1000 + sysTime.wMilliseconds;
	strEventtime = _i64toa(timestamp,eventtime,10);

	return strEventtime;
}

void CViewPage::getMtRequestId(char* lpRequestId,char* lpClientId,char* lpTimeStamp)
{
	char strRequestId[MAX_PATH] = {0};
	char eventtime[MAX_PATH] = {0};
	char strRand[128]  = {0};
	char strData[128] = {0};
	errno_t err = 0;
	SYSTEMTIME sysTime;
	time_t unixTime;
	if (!lpTimeStamp)
	{
		GetLocalTime(&sysTime);
		time(&unixTime);
		__int64 dwEventTime = unixTime*1000 + sysTime.wMilliseconds;

		err =  _i64toa_s(dwEventTime,strData,sizeof(strData),36);
	}
	else
	{
		__int64 dwEventTime = _atoi64(lpTimeStamp);
		err =  _i64toa_s(dwEventTime,strData,sizeof(strData),36);
	}
	strupr(strData);

	strcat(strRequestId,lpClientId);
	strcat(strRequestId,"z");
	strcat(strRequestId,strData);
	strcat(strRequestId,"z");

	//产生一个5位随机数
	srand((unsigned)time(NULL));
	int n = 5;
	int sum_5=0,w=n;
	while (n--)
	{
		int c=rand()%10;
		while(w==n&&!c)
		{
			c=rand()%10;
		}//最高为不能为0

		sum_5 = sum_5*10;
		sum_5 += c;
	}

	err = _i64toa_s(sum_5,strRand,sizeof(strRand),36);
	strupr(strRand);
	strcat(strRequestId,strRand);

	strcpy(lpRequestId,strRequestId);

	return;
}

void CViewPage::xpAppLaunchEvent(string strUserAgent,
								 char* lpVersion,
								 int width,
								 int height,
								 char* lpSrcData,
								 char* lpClientId,
								 char* lpDsid,
								 char* lpXpab)
{
	char processStartTime[128] = {0};
	char eventTime[128] = {0};
	char mainTime[128] = {0};
	char bootstrapStartTime[128] = {0};
	char jsResourcesStartTime[128] = {0};
	char initialTabRequestStartTime[128] = {0};
	char bootstrapEndTime[128] = {0};
	char initialTabResponseStartTime[128] = {0};
	char initialTabResponseEndTime[128] = {0};
	char jsResourcesEndTime[128] = {0};
	char launchEndTime[128] = {0};
	char postTime[128] = {0};
	TCHAR strPath[MAX_PATH] = {0};

	char buffer[ALLOCATE_SIZE] = {0};

	//计算时间戳
	getEventTime(processStartTime, FALSE, TRUE);
	getEventTime(eventTime, FALSE, TRUE);
	getEventTime(mainTime, FALSE, TRUE);
	getEventTime(bootstrapStartTime, TRUE, FALSE);
	getEventTime(jsResourcesStartTime, FALSE, TRUE);
	getEventTime(initialTabRequestStartTime, FALSE, TRUE);
	getEventTime(bootstrapEndTime, FALSE, TRUE);
	getEventTime(initialTabResponseStartTime, FALSE, TRUE);
	getEventTime(initialTabResponseEndTime, FALSE, TRUE);
	getEventTime(jsResourcesEndTime, FALSE, TRUE);
	getEventTime(launchEndTime, TRUE, FALSE);

	DWORD dwResult = GetModuleFileName(NULL,strPath,MAX_PATH);
	CString filePath(strPath);
	int Pos = filePath.ReverseFind(_T('\\'));
	filePath.Truncate(Pos);
	filePath += _T("\\protocol\\appLaunchEvent.txt");


	HANDLE hFile = CreateFile(filePath,GENERIC_READ,FILE_SHARE_READ,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL, NULL);
	if (!hFile)
	{
		return;
	}

	DWORD readLength = 1024;
	DWORD numOfByteToRead;
	DWORD totalLength = 0;

	char* pRecvBuf = new char[ALLOCATE_SIZE];
	if (!pRecvBuf)
	{
		CloseHandle(hFile);
		return;
	}

	memset(pRecvBuf,0x0,ALLOCATE_SIZE);

	char* pBuffer = new char[ALLOCATE_SIZE];
	if (!pBuffer)
	{
		CloseHandle(hFile);
		return;
	}

	memset(pBuffer,0x0,ALLOCATE_SIZE);
	char* pTmpBuf = buffer;
	BOOL bRead = FALSE;

	while (true)
	{
		bRead = ReadFile(hFile,pTmpBuf,readLength,&numOfByteToRead,NULL);
		if (bRead && numOfByteToRead != 0)
		{
			pTmpBuf = pTmpBuf + numOfByteToRead;
			totalLength += numOfByteToRead;
		}
		else
		{
			pBuffer[totalLength] = '\0';
			break;
		}
	}

	getEventTime(postTime,FALSE,TRUE);

	//组装数据
	pTmpBuf = &buffer[0];
	sprintf(pRecvBuf,
		pTmpBuf,
		postTime,
		initialTabResponseStartTime,
		initialTabResponseEndTime,
		bootstrapStartTime,
		eventTime,
		lpDsid,
		launchEndTime,
		(char*)strUserAgent.c_str(),
		(char*)m_viewMainStoreFrontHeader.c_str(),
		(char*)m_strAppleIDClientIdentifier.c_str(),
		width,
		jsResourcesStartTime,
		(char*)m_viewMainEnvironmentDataCenter.c_str(),
		jsResourcesEndTime,
		lpVersion,
		width,
		height,
		mainTime,
		height,
		initialTabRequestStartTime,
		bootstrapEndTime,
		processStartTime);

	memcpy(lpSrcData,pRecvBuf,strlen(pRecvBuf));

	if (hFile)
	{
		CloseHandle(hFile);
	}

	if (pRecvBuf)
	{
		delete []pRecvBuf;
		pRecvBuf = NULL;
	}

	if (pBuffer)
	{
		delete []pBuffer;
		pBuffer = NULL;
	}
	return;

}

void CViewPage::xpSearchSumbitEvent(string strUserAgent,
									char* lpVersion,
									int width,
									int height,
									char* lpSrcData,
									char* lpSearchword,
									char* lpActionUrl,
									char* lpClientId,
									char* lpSaleId,
									char* lpDsid,
									char* lpXpab,
									char* lpXpabc)
{
	char eventtime1[128] = {0};
	char requestStartTime[128] = {0};
	char responseStartTime[128] = {0};
	char responseEndTime[128] = {0};
	char pageloadTime[128] = {0};
	char eventtime2[128] = {0};
	char eventtime3[128] = {0};
	char pageloadTime2[128] = {0};
	char eventtime4[128] = {0};
	char eventtime5[128] = {0};
	char eventtime6[128] = {0};
	char eventtime7[128] = {0};
	char posttime[128] = {0};
	char szPageDetails[512] = {0};
	char buffer[MAX_ALLOCATE_SIZE] = {0};
	char szClientCorrelationKey[MAX_PATH] = {0};
	TCHAR strPath[MAX_PATH] = {0};

	getEventTime(eventtime1,FALSE,TRUE);

	DWORD dwResult = GetModuleFileName(NULL,strPath,MAX_PATH);
	CString filePath(strPath);
	int Pos = filePath.ReverseFind(_T('\\'));
	filePath.Truncate(Pos);
	filePath += _T("\\protocol\\searchSumbitEvent.txt");

	getEventTime(requestStartTime,FALSE,TRUE);
	getEventTime(responseStartTime,FALSE,TRUE);
	getEventTime(responseEndTime,FALSE,TRUE);
	getEventTime(pageloadTime,FALSE,TRUE);
	getEventTime(eventtime2,FALSE,TRUE);

	getEventTime(eventtime3,TRUE,FALSE);
	getEventTime(pageloadTime2,FALSE,TRUE);
	getEventTime(eventtime4,FALSE,TRUE);

	getEventTime(eventtime5,TRUE,FALSE);
	getEventTime(eventtime6,TRUE,FALSE);
	getEventTime(eventtime7,TRUE,FALSE);


	HANDLE hFile = CreateFile(filePath,GENERIC_READ,FILE_SHARE_READ,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL, NULL);
	if (!hFile)
	{
		return;
	}

	DWORD readLength = 1024;
	DWORD numOfByteToRead;
	DWORD totalLength = 0;

	char* pRecvBuf = new char[MAX_ALLOCATE_SIZE];
	if (!pRecvBuf)
	{
		CloseHandle(hFile);
		return;
	}

	getMtRequestId(szClientCorrelationKey,lpClientId,requestStartTime);
	memset(pRecvBuf,0x0,MAX_ALLOCATE_SIZE);

	char* pBuffer = new char[MAX_ALLOCATE_SIZE];
	if (!pBuffer)
	{
		CloseHandle(hFile);
		return;
	}

	memset(pBuffer,0x0,MAX_ALLOCATE_SIZE);
	char* pTmpBuf = buffer;
	BOOL bRead = FALSE;

	while (true)
	{
		bRead = ReadFile(hFile,pTmpBuf,readLength,&numOfByteToRead,NULL);
		if (bRead && numOfByteToRead != 0)
		{
			pTmpBuf = pTmpBuf + numOfByteToRead;
			totalLength += numOfByteToRead;
		}
		else
		{
			pBuffer[totalLength] = '\0';
			break;
		}
	}

	getEventTime(posttime,FALSE,TRUE);


	//组装数据
	pTmpBuf = &buffer[0] + 3;
	sprintf(pRecvBuf,
		pTmpBuf,
		lpClientId,
		(char*)m_viewMainEnvironmentDataCenter.c_str(),
		eventtime1,
		lpVersion,
		(char*)m_viewMaintPlatformId.c_str(),
		(char*)m_viewMainPlatformName.c_str(),
		(char*)m_viewMainRevNum.c_str(),
		height,
		width,
		(char*)m_viewMainStoreFrontHeader.c_str(),
		(char*)strUserAgent.c_str(),
		lpXpab,
		lpXpabc,

		lpXpab,
		lpXpabc,
		eventtime2,
		szClientCorrelationKey,
		lpDsid,
		responseEndTime,
		(char*)m_viewMainStoreFrontHeader.c_str(),
		(char*)strUserAgent.c_str(),
		width,
		responseStartTime,
		(char*)m_viewMainEnvironmentDataCenter.c_str(),	
		lpVersion,
		height,
		(char*)m_viewMainRevNum.c_str(),
		requestStartTime,
		pageloadTime,
		(char*)m_termServerInstance.c_str(),
		lpClientId,

		lpClientId,
		lpDsid,
		(char*)m_viewMainEnvironmentDataCenter.c_str(),
		eventtime3,
		lpVersion,
		(char*)m_viewMaintPlatformId.c_str(),
		(char*)m_viewMainPlatformName.c_str(),
		(char*)m_viewMainRevNum.c_str(),
		height,
		width,
		(char*)m_termServerInstance.c_str(),
		(char*)m_viewMainStoreFrontHeader.c_str(),
		(char*)strUserAgent.c_str(),
		lpXpab,
		lpXpabc,

		szClientCorrelationKey,
		lpClientId,
		lpDsid,
		(char*)m_viewMainEnvironmentDataCenter.c_str(),
		eventtime4,
		lpVersion,
		pageloadTime2,
		(char*)m_viewMaintPlatformId.c_str(),
		(char*)m_viewMainPlatformName.c_str(),
		(char*)m_viewMainRevNum.c_str(),
		height,
		width,
		(char*)m_viewMainStoreFrontHeader.c_str(),
		(char*)strUserAgent.c_str(),
		lpXpab,
		lpXpabc,

		lpActionUrl,
		lpClientId,
		lpDsid,
		(char*)m_termEnvironmentDataCenter.c_str(),
		eventtime5,
		lpVersion,
		(char*)m_viewMaintPlatformId.c_str(),
		(char*)m_viewMainPlatformName.c_str(),
		(char*)m_viewMainRevNum.c_str(),
		height,
		width,
		(char*)m_viewMainStoreFrontHeader.c_str(),
		lpSearchword,
		(char*)strUserAgent.c_str(),
		lpXpab,
		lpXpabc,

		lpActionUrl,
		lpClientId,
		lpDsid,
		(char*)m_viewMainEnvironmentDataCenter.c_str(),
		eventtime7,
		lpVersion,
		(char*)m_viewMaintPlatformId.c_str(),
		(char*)m_viewMainPlatformName.c_str(),
		(char*)m_viewMainRevNum.c_str(),
		height,
		width,
		(char*)m_viewMainStoreFrontHeader.c_str(),
		lpSearchword,
		(char*)strUserAgent.c_str(),
		lpXpab,
		lpXpabc,

		posttime);

	memcpy(lpSrcData,pRecvBuf,strlen(pRecvBuf));

	if (hFile)
	{
		CloseHandle(hFile);
	}

	if (pRecvBuf)
	{
		delete []pRecvBuf;
		pRecvBuf = NULL;
	}

	if (pBuffer)
	{
		delete []pBuffer;
		pBuffer = NULL;
	}
	return;
}

void CViewPage::xpPageRanderAmpClientPerfEvent(string strUserAgent,
											   char* lpVersion,
											   int width,
											   int height,
											   char* lpSrcData,
											   char* lpSearchword,
											   char* lpActionUrl,
											   char* lpClientId,
											   char* lpSaleId,
											   char* lpDsid,
											   char* lpXpab,
											   char* lpXpabc)
{
	char pageRequestedTime[128] = {0};
	char platformRequestStartTime[128] = {0};
	char platformResponseStartTime[128] = {0};
	char platformResponseEndTime[128] = {0};
	char pageUserReadyTime[128] = {0};
	char eventtime[128] = {0};
	char posttime[128] = {0};

	char buffer[MAX_ALLOCATE_SIZE] = {0};
	char szClientCorrelationKey[MAX_PATH] = {0};
	TCHAR strPath[MAX_PATH] = {0};

	//getEventTime(pageRequestedTime,FALSE,TRUE);
	getEventTime(platformRequestStartTime,FALSE,TRUE);
	getEventTime(platformResponseStartTime,TRUE,FALSE);
	getEventTime(platformResponseEndTime,FALSE,TRUE);
	getEventTime(pageUserReadyTime,FALSE,TRUE);
	getEventTime(eventtime,FALSE,TRUE);


	DWORD dwResult = GetModuleFileName(NULL,strPath,MAX_PATH);
	CString filePath(strPath);
	int Pos = filePath.ReverseFind(_T('\\'));
	filePath.Truncate(Pos);
	filePath += _T("\\protocol\\pageRanderAmpClientPerfEvent.txt");

	HANDLE hFile = CreateFile(filePath,GENERIC_READ,FILE_SHARE_READ,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL, NULL);
	if (!hFile)
	{
		return;
	}


	char* pRecvBuf = new char[MAX_ALLOCATE_SIZE];
	if (!pRecvBuf)
	{
		CloseHandle(hFile);
		return;
	}
	memset(pRecvBuf,0x0,MAX_ALLOCATE_SIZE);

	DWORD readLength = 1024;
	DWORD numOfByteToRead;
	DWORD totalLength = 0;

	char* pBuffer = new char[MAX_ALLOCATE_SIZE];
	if (!pBuffer)
	{
		CloseHandle(hFile);
		return;
	}

	memset(pBuffer,0x0,MAX_ALLOCATE_SIZE);
	char* pTmpBuf = buffer;
	BOOL bRead = FALSE;
	while (true)
	{
		bRead = ReadFile(hFile,pTmpBuf,readLength,&numOfByteToRead,NULL);
		if (bRead && numOfByteToRead != 0)
		{
			pTmpBuf = pTmpBuf + numOfByteToRead;
			totalLength += numOfByteToRead;
		}
		else
		{
			pBuffer[totalLength] = '\0';
			break;
		}
	}
	string strLaunchKey = getLaunchCorrelationKey();

	getEventTime(posttime,TRUE,FALSE);
	//组装数据
	pTmpBuf = &buffer[0] + 3;
	sprintf(pRecvBuf,
		pTmpBuf,
		posttime,
		lpXpab,
		lpXpabc,
		platformResponseEndTime,
		platformResponseStartTime,
		eventtime,
		(char*)m_strClientCorrelationKey.c_str(),
		lpDsid,
		(char*)m_termPageId.c_str(),
		(char*)strUserAgent.c_str(),
		(char*)m_termStoreFrontHeader.c_str(),
		lpSearchword,
		(char*)m_strPageRequestedTime.c_str(),//pageRequestedTime,
		width,
		(char*)m_termEnvironmentDataCenter.c_str(),
		pageUserReadyTime,
		lpVersion,
		(char*)m_termPage.c_str(),
		height,
		(char*)m_termRevNum.c_str(),
		(char*)m_termServerInstance.c_str(),
		lpClientId,
		platformRequestStartTime
		);

	memcpy(lpSrcData,pRecvBuf,strlen(pRecvBuf));

	if (hFile)
	{
		CloseHandle(hFile);
	}

	if (pRecvBuf)
	{
		delete []pRecvBuf;
		pRecvBuf = NULL;
	}

	if (pBuffer)
	{
		delete []pBuffer;
		pBuffer = NULL;
	}

	return;
}

void CViewPage::xpImpressionsEvent(string strUserAgent,
								   char* lpVersion,
								   int width,
								   int height,
								   char* lpSrcData,
								   char* lpSearchword,
								   char* lpActionUrl,
								   char* lpClientId,
								   char* lpSaleId,
								   char* lpDsid,
								   char* lpXpab,
								   char* lpXpabc)
{
	char timeStamp[128] = {0};
	char eventtime1[128] = {0};
	char navEventtime[128] = {0};
	char impressionNavPreTime[128] = {0};
	char impressionNavTime[128] = {0};
	char pageLoadtime[128] = {0};
	char posttime[128] = {0};
	char impEventTime[128] = {0};
	char impressionFirstTime[128] = {0};
	char impressionSecondTime[128] = {0};
	char impressionPreTime[128] = {0};
	char impressionNextTime[128] = {0};
	char impTargetEventtime[128] = {0};
	char impEventtime2[128] = {0};
	char impEventtime[128] = {0};
	char buffer[MAX_ALLOCATE_SIZE] = {0};
	char szClientCorrelationKey[MAX_PATH] = {0};
	TCHAR strPath[MAX_PATH] = {0};

	getEventTime(eventtime1,TRUE,FALSE);
	getMtRequestId(szClientCorrelationKey,lpClientId,timeStamp);

	getEventTime(pageLoadtime,TRUE,FALSE);
	DWORD dwResult = GetModuleFileName(NULL,strPath,MAX_PATH);
	CString filePath(strPath);
	int Pos = filePath.ReverseFind(_T('\\'));
	filePath.Truncate(Pos);
	filePath += _T("\\protocol\\impressionsEvent.txt");

	HANDLE hFile = CreateFile(filePath,GENERIC_READ,FILE_SHARE_READ,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL, NULL);
	if (!hFile)
	{
		return;
	}

	//以下时间戳的调用顺序非常重要

	//APP展示时间
	getEventTime(impressionFirstTime,TRUE,FALSE);
	getEventTime(impressionSecondTime,FALSE,TRUE);
	//这里跨度比较大，在获取排名第1,2的app应用展示时间后，跨度到目标app的前一个app和后一个app
	getEventTime(impressionPreTime,TRUE,FALSE);
	getEventTime(impTargetEventtime,FALSE,TRUE);
	getEventTime(impressionNextTime,FALSE,TRUE);

	//这里的时间间隔开始加大了10秒或者一个比较大的时间间隔，用来作为app展示的时间间隔，这个时间间隔不能太短
	getEventTime(impressionNavPreTime,TRUE,FALSE);
	getEventTime(impressionNavTime,FALSE,TRUE);

	getEventTime(navEventtime,FALSE,TRUE);

	getEventTime(impEventTime,FALSE,TRUE);


	char* pRecvBuf = new char[MAX_ALLOCATE_SIZE];
	if (!pRecvBuf)
	{
		CloseHandle(hFile);
		return;
	}
	memset(pRecvBuf,0x0,MAX_ALLOCATE_SIZE);

	DWORD readLength = 1024;
	DWORD numOfByteToRead;
	DWORD totalLength = 0;

	char* pBuffer = new char[MAX_ALLOCATE_SIZE];
	if (!pBuffer)
	{
		CloseHandle(hFile);
		return;
	}

	memset(pBuffer,0x0,MAX_ALLOCATE_SIZE);
	char* pTmpBuf = buffer;
	BOOL bRead = FALSE;
	while (true)
	{
		bRead = ReadFile(hFile,pTmpBuf,readLength,&numOfByteToRead,NULL);
		if (bRead && numOfByteToRead != 0)
		{
			pTmpBuf = pTmpBuf + numOfByteToRead;
			totalLength += numOfByteToRead;
		}
		else
		{
			pBuffer[totalLength] = '\0';
			break;
		}
	}

	getEventTime(posttime,FALSE,TRUE);

	//组装数据
	pTmpBuf = &buffer[0] + 3;
	sprintf(pRecvBuf,
		pTmpBuf,
		/*szClientCorrelationKey,
		lpClientId,
		lpDsid,
		(char*)m_termEnvironmentDataCenter.c_str(),
		eventtime1,
		lpVersion,

		pageLoadtime,
		(char*)m_termPlatformId.c_str(),
		(char*)m_termPlatformName.c_str(),
		(char*)m_termRevNum.c_str(),
		height,
		width,
		lpSearchword,
		(char*)m_termServerInstance.c_str(),
		(char*)m_termStoreFrontHeader.c_str(),
		szUserAgent,
		lpXpab,
		lpXpabc,*/
		lpActionUrl,
		lpClientId,
		lpDsid,
		(char*)m_termEnvironmentDataCenter.c_str(),
		navEventtime,

		impressionNavTime,

		impressionNavTime,
		lpSaleId,
		(char*)m_targetAppName.c_str(),
		s_locationPosition,

		impressionNextTime,
		(char*)m_strNextAppId.c_str(),
		(char*)m_strNextAppName.c_str(),
		s_NextlocationPosition,

		impressionNavTime,
		lpSaleId,
		(char*)m_targetAppName.c_str(),
		s_locationPosition,

		impressionNextTime,
		(char*)m_strNextAppId.c_str(),
		(char*)m_strNextAppName.c_str(),
		s_NextlocationPosition,

		(char*)m_targetAppName.c_str(),
		lpSaleId,
		s_locationPosition,
		lpVersion,
		(char*)m_termPage.c_str(),
		(char*)m_termPageId.c_str(),

		(char*)m_termPlatformId.c_str(),
		(char*)m_termPlatformName.c_str(),
		(char*)m_termRevNum.c_str(),
		height,
		width,
		(char*)m_termServerInstance.c_str(),
		(char*)m_termStoreFrontHeader.c_str(),
		lpSaleId,
		lpSaleId,
		(char*)strUserAgent.c_str(),
		lpXpab,
		lpXpabc,
		lpClientId,
		lpDsid,
		(char*)m_termEnvironmentDataCenter.c_str(),
		impEventTime,

		impressionFirstTime,

		impTargetEventtime,
		lpSaleId,
		(char*)m_targetAppName.c_str(),
		s_locationPosition,

		impressionPreTime,
		(char*)m_strPreAppId.c_str(),
		(char*)m_strpreAppName.c_str(),
		s_PrelocationPosition,

		impressionNextTime,
		(char*)m_strNextAppId.c_str(),
		(char*)m_strNextAppName.c_str(),
		s_NextlocationPosition,

		lpVersion,
		(char*)m_termPage.c_str(),
		(char*)m_termPageId.c_str(),
		(char*)m_termPlatformId.c_str(),
		(char*)m_termPlatformName.c_str(),
		(char*)m_termRevNum.c_str(),
		height,
		width,
		lpSearchword,
		(char*)m_termServerInstance.c_str(),
		(char*)m_termStoreFrontHeader.c_str(),
		(char*)strUserAgent.c_str(),
		lpXpab,
		lpXpabc,
		posttime
		);

	memcpy(lpSrcData,pRecvBuf,strlen(pRecvBuf));

	if (hFile)
	{
		CloseHandle(hFile);
	}

	if (pRecvBuf)
	{
		delete []pRecvBuf;
		pRecvBuf = NULL;
	}

	if (pBuffer)
	{
		delete []pBuffer;
		pBuffer = NULL;
	}

	return;

}

void CViewPage::xpFinishImpressionEvent(CString strUserAgent,
										char* lpVersion,
										int width,
										int height,
										char* lpSrcData,
										char* lpSearchword,
										char* lpClientId,
										char* lpSaleId,
										char* lpDsid,
										char* lpXpab,
										char* lpXpabc)
{
	char posttime[128] = {0};
	char impEventTime[128] = {0};
	char impressionFirstTime[128] = {0};
	char impressionNextTime[128] = {0};
	char impTargetEventtime[128] = {0};

	char buffer[MAX_ALLOCATE_SIZE] = {0};
	char szUserAgent[MAX_PATH] = {0};
	TCHAR strPath[MAX_PATH] = {0};
	CString strAgent = strUserAgent;

	WideCharToMultiByte(CP_ACP,0,strAgent.GetBuffer(),strAgent.GetLength(),szUserAgent,MAX_PATH,NULL,NULL);

	DWORD dwResult = GetModuleFileName(NULL,strPath,MAX_PATH);
	CString filePath(strPath);
	int Pos = filePath.ReverseFind(_T('\\'));
	filePath.Truncate(Pos);
	filePath += _T("\\protocol\\finishImpressionEvent.txt");

	HANDLE hFile = CreateFile(filePath,GENERIC_READ,FILE_SHARE_READ,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL, NULL);
	if (!hFile)
	{
		return;
	}

	//以下时间戳的调用顺序非常重要

	//APP展示时间
	getEventTime(impressionFirstTime,FALSE,TRUE);
	getEventTime(impTargetEventtime,FALSE,TRUE);
	getEventTime(impressionNextTime,FALSE,TRUE);
	getEventTime(impEventTime,FALSE,TRUE);


	char* pRecvBuf = new char[MAX_ALLOCATE_SIZE];
	if (!pRecvBuf)
	{
		CloseHandle(hFile);
		return;
	}
	memset(pRecvBuf,0x0,MAX_ALLOCATE_SIZE);

	DWORD readLength = 1024;
	DWORD numOfByteToRead;
	DWORD totalLength = 0;

	char* pBuffer = new char[MAX_ALLOCATE_SIZE];
	if (!pBuffer)
	{
		CloseHandle(hFile);
		return;
	}

	memset(pBuffer,0x0,MAX_ALLOCATE_SIZE);
	char* pTmpBuf = buffer;
	BOOL bRead = FALSE;
	while (true)
	{
		bRead = ReadFile(hFile,pTmpBuf,readLength,&numOfByteToRead,NULL);
		if (bRead && numOfByteToRead != 0)
		{
			pTmpBuf = pTmpBuf + numOfByteToRead;
			totalLength += numOfByteToRead;
		}
		else
		{
			pBuffer[totalLength] = '\0';
			break;
		}
	}

	getEventTime(posttime,FALSE,TRUE);

	//组装数据
	pTmpBuf = &buffer[0] + 3;
	sprintf(pRecvBuf,
		pTmpBuf,
		posttime,
		lpXpab,
		lpXpabc,
		impEventTime,
		lpDsid,
		(char*)m_termPageId.c_str(),
		szUserAgent,
		lpSearchword,
		width,
		(char*)m_termEnvironmentDataCenter.c_str(),
		lpVersion,
		(char*)m_termPage.c_str(),
		height,

		impressionFirstTime,

		s_locationPosition,
		lpSaleId,
		impTargetEventtime,
		(char*)m_targetAppName.c_str(),

		s_NextlocationPosition,
		(char*)m_strNextAppId.c_str(),
		impressionNextTime,
		(char*)m_strNextAppName.c_str(),

		(char*)m_termRevNum.c_str(),
		(char*)m_termServerInstance.c_str(),
		lpClientId
		);

	memcpy(lpSrcData,pRecvBuf,strlen(pRecvBuf));

	if (hFile)
	{
		CloseHandle(hFile);
	}

	if (pRecvBuf)
	{
		delete []pRecvBuf;
		pRecvBuf = NULL;
	}

	if (pBuffer)
	{
		delete []pBuffer;
		pBuffer = NULL;
	}

	return;
}

void CViewPage::xpPageSearchEvent(string strUserAgent,
								  char* lpVersion,
								  int width,
								  int height,
								  char* lpSrcData,
								  char* lpSearchTerm,
								  char* lpClientId,
								  char* lpDsid,
								  char* lpXpab,
								  char* lpXpabc)
{
	char pageloadTime[128] = {0};
	char timeStamp[128] = {0};
	char eventtime[128] = {0};
	char posttime[128] = {0};
	char buffer[4096] = {0};
	char szClientCorrelationKey[MAX_PATH] = {0};
	TCHAR strPath[MAX_PATH] = {0};

	getEventTime(timeStamp,TRUE,FALSE);
	getEventTime(pageloadTime,TRUE,FALSE);
	getEventTime(eventtime,FALSE,TRUE);

	getMtRequestId(szClientCorrelationKey,lpClientId,timeStamp);
	m_strClientCorrelationKey = szClientCorrelationKey;
	m_strPageRequestedTime = timeStamp;


	DWORD dwResult = GetModuleFileName(NULL,strPath,MAX_PATH);
	CString filePath(strPath);
	int Pos = filePath.ReverseFind(_T('\\'));
	filePath.Truncate(Pos);
	filePath += _T("\\protocol\\pageSearchEvent.txt");

	HANDLE hFile = CreateFile(filePath,GENERIC_READ,FILE_SHARE_READ,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL, NULL);
	if (!hFile)
	{
		return;
	}

	char* pRecvBuf = new char[ALLOCATE_SIZE];
	if (!pRecvBuf)
	{
		CloseHandle(hFile);
		return;
	}
	memset(pRecvBuf,0x0,ALLOCATE_SIZE);


	DWORD readLength = 1024;
	DWORD numOfByteToRead;
	DWORD totalLength = 0;
	char* pBuffer = new char[ALLOCATE_SIZE];
	if (!pBuffer)
	{
		CloseHandle(hFile);
		return;
	}
	memset(pBuffer,0x0,ALLOCATE_SIZE);
	char* pTmpBuf = buffer;
	BOOL bRead = FALSE;
	while (true)
	{
		bRead = ReadFile(hFile,pTmpBuf,readLength,&numOfByteToRead,NULL);
		if (bRead && numOfByteToRead != 0)
		{
			pTmpBuf = pTmpBuf + numOfByteToRead;
			totalLength += numOfByteToRead;
		}
		else
		{
			pBuffer[totalLength] = '\0';
			break;
		}
	}

	getEventTime(posttime,FALSE,TRUE);

	//组装数据
	pTmpBuf = &buffer[0];
	sprintf(pRecvBuf,
		pTmpBuf,
		posttime,
		lpXpab,
		lpXpabc,
		eventtime,
		szClientCorrelationKey,
		lpDsid,
		(char*)m_termPageId.c_str(),
		(char*)strUserAgent.c_str(),
		(char*)m_termStoreFrontHeader.c_str(),
		lpSearchTerm,
		width,
		(char*)m_termEnvironmentDataCenter.c_str(),
		lpVersion,
		(char*)m_termPlatformName.c_str(),
		(char*)m_termPage.c_str(),
		height,
		(char*)m_termRevNum.c_str(),
		pageloadTime,
		(char*)m_termServerInstance.c_str(),
		lpClientId,
		(char*)m_termPlatformId.c_str()
		);

	memcpy(lpSrcData,pRecvBuf,strlen(pRecvBuf));

	if (hFile)
	{
		CloseHandle(hFile);
	}

	if (pRecvBuf)
	{
		delete []pRecvBuf;
		pRecvBuf = NULL;
	}

	if (pBuffer)
	{
		delete []pBuffer;
		pBuffer = NULL;
	}
	return;
}

void CViewPage:: xpTargetAppPageDetailEvent(string strUserAgent,
											char* lpVersion,
											int width,
											int height,
											char* lpSrcData,
											char* lpPageUrl,
											char* lpClientId,
											char* lpSaleId,
											char* lpDsid,
											char* lpSearchTerm,
											char* lpPageloadTime,
											char* lpXpab,
											char* lpXpabc)
{
	char eventtime[128] = {0};
	char pageloadTime[128] = {0};
	char posttime[128] = {0};
	char strRequestStartTime[128] = {0};
	char strResponseStartTime[128] = {0};
	char strResponseEndTime[128] = {0};
	char buffer[4096] = {0};
	char szClientCorrelationKey[MAX_PATH] = {0};
	TCHAR strPath[MAX_PATH] = {0};


	getEventTime(strRequestStartTime,FALSE,TRUE);

	getMtRequestId(szClientCorrelationKey,lpClientId,(char*)m_strTimeStamp.c_str());

	DWORD dwResult = GetModuleFileName(NULL,strPath,MAX_PATH);
	CString filePath(strPath);
	int Pos = filePath.ReverseFind(_T('\\'));
	filePath.Truncate(Pos);
	filePath += _T("\\protocol\\targetAppPageDetailEvent.txt");

	getEventTime(strResponseStartTime,FALSE,TRUE);
	getEventTime(strResponseEndTime,FALSE,TRUE);
	getEventTime(pageloadTime,FALSE,TRUE);
	getEventTime(eventtime,FALSE,TRUE);

	HANDLE hFile = CreateFile(filePath,GENERIC_READ,FILE_SHARE_READ,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL, NULL);
	if (!hFile)
	{
		return;
	}

	char* pRecvBuf = new char[ALLOCATE_SIZE];
	if (!pRecvBuf)
	{
		CloseHandle(hFile);
		return;
	}
	memset(pRecvBuf,0x0,ALLOCATE_SIZE);

	DWORD readLength = 1024;
	DWORD numOfByteToRead;
	DWORD totalLength = 0;
	char* pBuffer = new char[ALLOCATE_SIZE];
	if (!pBuffer)
	{
		CloseHandle(hFile);
		return;
	}

	memset(pBuffer,0x0,ALLOCATE_SIZE);
	char* pTmpBuf = buffer;
	BOOL bRead = FALSE;
	while (true)
	{
		bRead = ReadFile(hFile,pTmpBuf,readLength,&numOfByteToRead,NULL);
		if (bRead && numOfByteToRead != 0)
		{
			pTmpBuf = pTmpBuf + numOfByteToRead;
			totalLength += numOfByteToRead;
		}
		else
		{
			pBuffer[totalLength] = '\0';
			break;
		}
	}

	getEventTime(posttime,TRUE,FALSE);

	//组装数据
	pTmpBuf = &buffer[0] + 3;
	sprintf(pRecvBuf,
		pTmpBuf,
		posttime,
		lpXpab,
		lpXpabc,
		eventtime,
		szClientCorrelationKey,
		lpDsid,
		strResponseEndTime,
		lpSaleId,
		(char*)m_targetAppStoreFrontHeader.c_str(),
		(char*)strUserAgent.c_str(),
		lpSearchTerm,
		width,
		strResponseStartTime,
		(char*)m_targetAppEnvironmentDataCenter.c_str(),
		lpVersion,
		(char*)m_targetAppPageDetails.c_str(),
		lpSaleId,
		height,
		(char*)m_targetAppRevNum.c_str(),
		lpPageUrl,
		strRequestStartTime,
		pageloadTime,
		(char*)m_targetAppServerInstance.c_str(),
		lpClientId);

	memcpy(lpSrcData,pRecvBuf,strlen(pRecvBuf));

	if (hFile)
	{
		CloseHandle(hFile);
	}

	if (pRecvBuf)
	{
		delete []pRecvBuf;
		pRecvBuf = NULL;
	}

	if (pBuffer)
	{
		delete []pBuffer;
		pBuffer = NULL;
	}
	return;
}

void CViewPage::xpBuyEvent(CString strUserAgent,
						   char* lpVersion,
						   int width,
						   int height,
						   char* lpSrcData,
						   char* lpSearchword,
						   char* lpClientId,
						   char* lpSaleId,
						   char* lpDsid,
						   char* lpMtRequestId,
						   char* lpPliIds,
						   char* lpXpab,
						   char* lpXpabc)
{

	char eventtime1[MAX_PATH] = {0};
	char eventtime2[MAX_PATH] = {0};
	char eventtime3[MAX_PATH] = {0};
	char ImpressionTime[MAX_PATH] = {0};
	char posttime[MAX_PATH] = {0};
	char szPageDetails[512] = {0};
	char buffer[MAX_ALLOCATE_SIZE] = {0};
	char szUserAgent[MAX_PATH] = {0};
	char szClientCorrelationKey[MAX_PATH] = {0};
	TCHAR strPath[MAX_PATH] = {0};
	CString strAgent = strUserAgent;

	getEventTime(eventtime1,TRUE,FALSE);

	WideCharToMultiByte(CP_ACP,0,strAgent.GetBuffer(),strAgent.GetLength(),szUserAgent,MAX_PATH,NULL,NULL);

	DWORD dwResult = GetModuleFileName(NULL,strPath,MAX_PATH);
	CString filePath(strPath);
	int Pos = filePath.ReverseFind(_T('\\'));
	filePath.Truncate(Pos);
	filePath += _T("\\protocol\\buyEvent.txt");

	getEventTime(eventtime2,TRUE,FALSE);

	HANDLE hFile = CreateFile(filePath,GENERIC_READ,FILE_SHARE_READ,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL, NULL);
	if (!hFile)
	{
		return;
	}

	getEventTime(ImpressionTime,TRUE,FALSE);

	char* pRecvBuf = new char[MAX_ALLOCATE_SIZE];
	if (!pRecvBuf)
	{
		CloseHandle(hFile);
		return;
	}
	memset(pRecvBuf,0x0,MAX_ALLOCATE_SIZE);

	DWORD readLength = 1024;
	DWORD numOfByteToRead;
	DWORD totalLength = 0;
	char* pBuffer = new char[MAX_ALLOCATE_SIZE];
	if (!pBuffer)
	{
		CloseHandle(hFile);
		return;
	}

	getEventTime(eventtime3,TRUE,FALSE);
	memset(pBuffer,0x0,MAX_ALLOCATE_SIZE);
	char* pTmpBuf = buffer;
	BOOL bRead = FALSE;
	while (true)
	{
		bRead = ReadFile(hFile,pTmpBuf,readLength,&numOfByteToRead,NULL);
		if (bRead && numOfByteToRead != 0)
		{
			pTmpBuf = pTmpBuf + numOfByteToRead;
			totalLength += numOfByteToRead;
		}
		else
		{
			pBuffer[totalLength] = '\0';
			break;
		}
	}

	getEventTime(posttime,FALSE,TRUE);

	//组装数据
	pTmpBuf = &buffer[0] + 3;
	sprintf(pRecvBuf,
		pTmpBuf,
		lpSaleId,
		(char*)m_targetAppAppExtVrsId.c_str(),
		lpClientId,
		lpDsid,
		(char*)m_targetAppEnvironmentDataCenter.c_str(),
		eventtime1,
		ImpressionTime,
		ImpressionTime,
		lpSaleId,
		(char*)m_targetAppName.c_str(),
		s_locationPosition,

		(char*)m_targetAppName.c_str(),
		lpSaleId,

		(char*)m_targetAppName.c_str(),
		lpSaleId,

		(char*)m_targetAppName.c_str(),
		lpSaleId,
		s_locationPosition,
		lpVersion,

		(char*)m_targetAppPlatformId.c_str(),
		(char*)m_targetAppPlatformName.c_str(),
		(char*)m_targetAppRevNum.c_str(),
		height,
		width,
		(char*)m_targetAppServerInstance.c_str(),
		(char*)m_targetAppStoreFrontHeader.c_str(),
		lpSaleId,
		lpSaleId,
		szUserAgent,
		lpXpab,
		lpXpabc,
		lpClientId,
		lpDsid,
		(char*)m_targetAppEnvironmentDataCenter.c_str(),
		eventtime2,

		ImpressionTime,
		ImpressionTime,
		lpSaleId,
		(char*)m_targetAppName.c_str(),
		s_locationPosition,
		lpVersion,

		(char*)m_targetAppPlatformId.c_str(),
		(char*)m_targetAppPlatformName.c_str(),
		(char*)m_targetAppRevNum.c_str(),
		height,
		width,

		lpSearchword,
		(char*)m_targetAppServerInstance.c_str(),
		(char*)m_targetAppStoreFrontHeader.c_str(),
		szUserAgent,
		lpXpab,
		lpXpabc,
		lpClientId,
		lpDsid,
		(char*)m_targetAppEnvironmentDataCenter.c_str(),
		eventtime3,
		lpSaleId,
		lpMtRequestId,
		lpVersion,
		(char*)m_targetAppPlatformId.c_str(),
		(char*)m_targetAppPlatformName.c_str(),
		lpPliIds,
		(char*)m_targetAppRevNum.c_str(),
		height,
		width,
		(char*)m_targetAppServerInstance.c_str(),
		(char*)m_targetAppStoreFrontHeader.c_str(),
		szUserAgent,
		lpXpab,
		lpXpabc,
		posttime
		);

	memcpy(lpSrcData,pRecvBuf,strlen(pRecvBuf));

	if (hFile)
	{
		CloseHandle(hFile);
	}

	if (pRecvBuf)
	{
		delete []pRecvBuf;
		pRecvBuf = NULL;
	}

	if (pBuffer)
	{
		delete []pBuffer;
		pBuffer = NULL;
	}
	return;
}


void CViewPage::xpBuyConfirmEvent(string strUserAgent,
								  char* lpVersion,
								  int width,
								  int height,
								  char* lpSrcData,
								  char* lpPageUrl,
								  char* lpClientId,
								  char* lpSaleId,
								  char* lpDsid,
								  char* lpMtRequestId,
								  char* lpPliIds,
								  char* lpXpab,
								  char* lpXpabc)
{
	char responstStartTime[128] = {0};
	char requestStartTime[128] = {0};
	char responseEndTime[128] = {0};
	char eventtime[MAX_PATH] = {0};
	char posttime[MAX_PATH] = {0};
	char szPageDetails[512] = {0};
	char buffer[4096] = {0};
	char szClientCorrelationKey[MAX_PATH] = {0};
	TCHAR strPath[MAX_PATH] = {0};

	getEventTime(requestStartTime,TRUE,FALSE);

	DWORD dwResult = GetModuleFileName(NULL,strPath,MAX_PATH);
	CString filePath(strPath);
	int Pos = filePath.ReverseFind(_T('\\'));
	filePath.Truncate(Pos);
	filePath += _T("\\protocol\\buyConfirmEvent.txt");

	HANDLE hFile = CreateFile(filePath,GENERIC_READ,FILE_SHARE_READ,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL, NULL);
	if (!hFile)
	{
		return;
	}
	getEventTime(responstStartTime,FALSE,TRUE);

	char* pRecvBuf = new char[ALLOCATE_SIZE];
	if (!pRecvBuf)
	{
		CloseHandle(hFile);
		return;
	}
	memset(pRecvBuf,0x0,ALLOCATE_SIZE);

	DWORD readLength = 1024;
	DWORD numOfByteToRead;
	DWORD totalLength = 0;
	char* pBuffer = new char[ALLOCATE_SIZE];
	if (!pBuffer)
	{
		CloseHandle(hFile);
		return;
	}
	memset(pBuffer,0x0,ALLOCATE_SIZE);
	char* pTmpBuf = buffer;
	BOOL bRead = FALSE;
	getEventTime(responseEndTime,FALSE,TRUE);
	while (true)
	{
		bRead = ReadFile(hFile,pTmpBuf,readLength,&numOfByteToRead,NULL);
		if (bRead && numOfByteToRead != 0)
		{
			pTmpBuf = pTmpBuf + numOfByteToRead;
			totalLength += numOfByteToRead;
		}
		else
		{
			pBuffer[totalLength] = '\0';
			break;
		}
	}
	getEventTime(eventtime,FALSE,TRUE);

	getEventTime(posttime,FALSE,TRUE);

	//组装数据
	pTmpBuf = &buffer[0] + 3;
	sprintf(pRecvBuf,
		pTmpBuf,
		posttime,
		lpXpab,
		lpXpabc,
		eventtime,
		lpDsid,
		(char*)m_termPageId.c_str(),
		(char*)m_targetAppStoreFrontHeader.c_str(),
		(char*)strUserAgent.c_str(),
		lpSaleId,
		lpMtRequestId,
		width,
		responstStartTime,
		requestStartTime,
		responseEndTime,
		(char*)m_termEnvironmentDataCenter.c_str(),
		lpVersion,
		(char*)m_targetAppPageDetails.c_str(),
		(char*)m_termPage.c_str(),
		(char*)m_targetAppRevNum.c_str(),
		lpPliIds,
		//lpPageUrl,
		height,
		(char*)m_termServerInstance.c_str(),
		lpClientId
		);

	memcpy(lpSrcData,pRecvBuf,strlen(pRecvBuf));

	if (hFile)
	{
		CloseHandle(hFile);
	}

	if (pRecvBuf)
	{
		delete []pRecvBuf;
		pRecvBuf = NULL;
	}

	if (pBuffer)
	{
		delete []pBuffer;
		pBuffer = NULL;
	}
	return;
}

void CViewPage::xpActiveAppEvent(string strUserAgent,
								 char* lpVersion,
								 int width,
								 int height,
								 char* lpSrcData,
								 char* lpClientId,
								 char* lpSaleId,
								 char* lpDsid,
								 char* lpImpressionTime,
								 char* lpXpab,
								 char* lpXpabc)
{
	char eventtime[MAX_PATH] = {0};
	char posttime[MAX_PATH] = {0};
	char szPageDetails[512] = {0};
	char buffer[4096] = {0};
	char szClientCorrelationKey[MAX_PATH] = {0};
	TCHAR strPath[MAX_PATH] = {0};


	getEventTime(eventtime,TRUE,FALSE);


	DWORD dwResult = GetModuleFileName(NULL,strPath,MAX_PATH);
	CString filePath(strPath);
	int Pos = filePath.ReverseFind(_T('\\'));
	filePath.Truncate(Pos);
	filePath += _T("\\protocol\\activeAppEvent.txt");

	HANDLE hFile = CreateFile(filePath,GENERIC_READ,FILE_SHARE_READ,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL, NULL);
	if (!hFile)
	{
		return;
	}

	char* pRecvBuf = new char[ALLOCATE_SIZE];
	if (!pRecvBuf)
	{
		CloseHandle(hFile);
		return;
	}
	memset(pRecvBuf,0x0,ALLOCATE_SIZE);


	DWORD readLength = 1024;
	DWORD numOfByteToRead;
	DWORD totalLength = 0;
	char* pBuffer = new char[ALLOCATE_SIZE];
	if (!pBuffer)
	{
		CloseHandle(hFile);
		return;
	}
	memset(pBuffer,0x0,ALLOCATE_SIZE);
	char* pTmpBuf = buffer;
	BOOL bRead = FALSE;
	while (true)
	{
		bRead = ReadFile(hFile,pTmpBuf,readLength,&numOfByteToRead,NULL);
		if (bRead && numOfByteToRead != 0)
		{
			pTmpBuf = pTmpBuf + numOfByteToRead;
			totalLength += numOfByteToRead;
		}
		else
		{
			pBuffer[totalLength] = '\0';
			break;
		}
	}

	getEventTime(posttime,FALSE,TRUE);

	//组装数据
	pTmpBuf = &buffer[0] + 3;
	sprintf(pRecvBuf,
		pTmpBuf,
		posttime,
		lpImpressionTime,
		lpImpressionTime,
		lpSaleId,
		(char*)m_targetAppName.c_str(),
		s_locationPosition,
		lpSaleId,
		width,
		lpClientId,
		(char*)m_targetAppBunldeId.c_str(),
		(char*)m_targetAppStoreFrontHeader.c_str(),
		(char*)m_targetAppPageDetails.c_str(),
		lpSaleId,
		(char*)m_targetAppPlatformId.c_str(),
		lpDsid,
		(char*)m_targetAppServerInstance.c_str(),
		(char*)m_targetAppName.c_str(),
		lpSaleId,
		lpXpab,
		lpXpabc,
		height,
		eventtime,
		(char*)m_targetAppPlatformName.c_str(),
		(char*)m_termPage.c_str(),
		(char*)m_termPageId.c_str(),
		lpVersion,
		(char*)strUserAgent.c_str(),
		(char*)m_targetAppEnvironmentDataCenter.c_str(),
		(char*)m_targetAppRevNum.c_str(),
		(char*)m_targetAppEnvironmentDataCenter.c_str(),
		(char*)strUserAgent.c_str(),
		lpClientId,
		(char*)m_targetAppRevNum.c_str(),
		width,
		height,
		(char*)m_targetAppPlatformName.c_str(),
		(char*)m_targetAppPlatformId.c_str(),
		lpVersion,
		lpDsid,
		(char*)m_targetAppStoreFrontHeader.c_str(),
		eventtime);

	memcpy(lpSrcData,pRecvBuf,strlen(pRecvBuf));

	if (hFile)
	{
		CloseHandle(hFile);
	}

	if (pRecvBuf)
	{
		delete []pRecvBuf;
		pRecvBuf = NULL;
	}

	if (pBuffer)
	{
		delete []pBuffer;
		pBuffer = NULL;
	}
	return;
}

void CViewPage::xpOpenAppEvent(string strUserAgent,
							   char* lpVersion,
							   int width,
							   int height,
							   char* lpSrcData,
							   char* lpClientId,
							   char* lpSaleId,
							   char* lpDsid,
							   char* lpXpab,
							   char* lpXpabc)
{
	char eventtime1[MAX_PATH] = {0};
	char eventtime2[MAX_PATH] = {0};
	char posttime[MAX_PATH] = {0};
	char szPageDetails[512] = {0};
	char buffer[4096] = {0};
	char szClientCorrelationKey[MAX_PATH] = {0};
	TCHAR strPath[MAX_PATH] = {0};

	getEventTime(eventtime1,TRUE,FALSE);


	DWORD dwResult = GetModuleFileName(NULL,strPath,MAX_PATH);
	CString filePath(strPath);
	int Pos = filePath.ReverseFind(_T('\\'));
	filePath.Truncate(Pos);
	filePath += _T("\\protocol\\openAppEvent.txt");

	HANDLE hFile = CreateFile(filePath,GENERIC_READ,FILE_SHARE_READ,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL, NULL);
	if (!hFile)
	{
		return;
	}

	char* pRecvBuf = new char[ALLOCATE_SIZE];
	if (!pRecvBuf)
	{
		CloseHandle(hFile);
		return;
	}
	memset(pRecvBuf,0x0,ALLOCATE_SIZE);


	DWORD readLength = 1024;
	DWORD numOfByteToRead;
	DWORD totalLength = 0;
	char* pBuffer = new char[ALLOCATE_SIZE];
	if (!pBuffer)
	{
		CloseHandle(hFile);
		return;
	}

	getEventTime(eventtime2,TRUE,FALSE);
	memset(pBuffer,0x0,ALLOCATE_SIZE);
	char* pTmpBuf = buffer;
	BOOL bRead = FALSE;
	while (true)
	{
		bRead = ReadFile(hFile,pTmpBuf,readLength,&numOfByteToRead,NULL);
		if (bRead && numOfByteToRead != 0)
		{
			pTmpBuf = pTmpBuf + numOfByteToRead;
			totalLength += numOfByteToRead;
		}
		else
		{
			pBuffer[totalLength] = '\0';
			break;
		}
	}

	getEventTime(posttime,FALSE,TRUE);

	//组装数据
	pTmpBuf = &buffer[0] + 3;
	sprintf(pRecvBuf,
		pTmpBuf,
		posttime,
		lpXpab,
		lpXpabc,
		lpSaleId,
		lpSaleId,
		(char*)m_targetAppName.c_str(),
		lpDsid,
		(char*)m_termPageId.c_str(),
		eventtime1,
		(char*)m_targetAppStoreFrontHeader.c_str(),
		(char*)strUserAgent.c_str(),
		width,
		lpSaleId,
		(char*)m_targetAppEnvironmentDataCenter.c_str(),
		lpVersion,
		(char*)m_targetAppPageDetails.c_str(),
		height,
		(char*)m_termPage.c_str(),
		(char*)m_targetAppRevNum.c_str(),
		(char*)m_targetAppServerInstance.c_str(),
		lpClientId,

		lpVersion,
		lpDsid,
		height,
		(char*)m_targetAppStoreFrontHeader.c_str(),
		eventtime2,
		(char*)strUserAgent.c_str(),
		(char*)m_targetAppEnvironmentDataCenter.c_str(),
		lpXpab,
		lpXpabc,
		width,
		lpClientId,
		(char*)m_targetAppRevNum.c_str()
		);

	memcpy(lpSrcData,pRecvBuf,strlen(pRecvBuf));

	if (hFile)
	{
		CloseHandle(hFile);
	}

	if (pRecvBuf)
	{
		delete []pRecvBuf;
		pRecvBuf = NULL;
	}

	if (pBuffer)
	{
		delete []pBuffer;
		pBuffer = NULL;
	}
	return;
}

void CViewPage::xpBuyInitiateEvent(string strUserAgent,
								   char* lpVersion,
								   int width,
								   int height,
								   char* lpSearchTerm,
								   char* lpSrcData,
								   char* lpClientId,
								   char* lpSaleId,
								   char* lpDsid,
								   char* lpXpab,
								   char* lpXpabc)
{
	char eventtime1[MAX_PATH] = {0};
	char eventtime2[MAX_PATH] = {0};
	char contexttime[128] = {0};
	char contImpTime[128] = {0};
	char impressionTime[128] = {0};
	char impEventTime[128] = {0};
	char exittime[128] = {0};
	char posttime[MAX_PATH] = {0};
	char szPageDetails[512] = {0};
	char szClientCorrelationKey[MAX_PATH] = {0};
	TCHAR strPath[MAX_PATH] = {0};

	getEventTime(impressionTime,FALSE,TRUE);
	getEventTime(eventtime1,TRUE,FALSE);


	DWORD dwResult = GetModuleFileName(NULL,strPath,MAX_PATH);
	CString filePath(strPath);
	int Pos = filePath.ReverseFind(_T('\\'));
	filePath.Truncate(Pos);
	filePath += _T("\\protocol\\buyInitiateEvent.txt");

	HANDLE hFile = CreateFile(filePath,GENERIC_READ,FILE_SHARE_READ,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL, NULL);
	if (!hFile)
	{
		return;
	}

	char* pRecvBuf = new char[MAX_ALLOC_SIZE*2];
	if (!pRecvBuf)
	{
		CloseHandle(hFile);
		return;
	}
	memset(pRecvBuf,0x0,MAX_ALLOC_SIZE*2);


	DWORD readLength = 1024;
	DWORD numOfByteToRead;
	DWORD totalLength = 0;
	char* pBuffer = new char[MAX_ALLOC_SIZE*2];
	if (!pBuffer)
	{
		CloseHandle(hFile);
		return;
	}

	getEventTime(eventtime2,TRUE,FALSE*2);
	memset(pBuffer,0x0,MAX_ALLOC_SIZE*2);
	char* pTmpBuf = pBuffer;
	BOOL bRead = FALSE;
	while (true)
	{
		bRead = ReadFile(hFile,pTmpBuf,readLength,&numOfByteToRead,NULL);
		if (bRead && numOfByteToRead != 0)
		{
			pTmpBuf = pTmpBuf + numOfByteToRead;
			totalLength += numOfByteToRead;
		}
		else
		{
			pBuffer[totalLength] = '\0';
			break;
		}
	}

	getEventTime(contImpTime,FALSE,TRUE);
	getEventTime(contexttime,TRUE,FALSE);
	getEventTime(impEventTime,FALSE,TRUE);
	getEventTime(exittime,FALSE,TRUE);
	getEventTime(posttime,FALSE,TRUE);

	//组装数据
	pTmpBuf = &pBuffer[0] + 3;
	sprintf(pRecvBuf,
		pTmpBuf,
		posttime,
		lpXpab,
		lpXpabc,
		lpSaleId,

		lpSaleId,
		(char*)m_targetAppName.c_str(),

		lpSaleId,
		(char*)m_targetAppName.c_str(),

		lpSaleId,
		(char*)m_targetAppName.c_str(),
		s_locationPosition,

		lpDsid,
		(char*)m_termPageId.c_str(),
		eventtime1,
		(char*)m_termStoreFrontHeader.c_str(),
		(char*)strUserAgent.c_str(),
		width,
		lpSaleId,
		(char*)m_termEnvironmentDataCenter.c_str(),
		lpVersion,
		(char*)m_targetAppPageDetails.c_str(),
		height,

		s_locationPosition,
		lpSaleId,
		eventtime1,//InitBuyImpTime,
		(char*)m_targetAppName.c_str(),

		(char*)m_termPage.c_str(),
		(char*)m_targetAppRevNum.c_str(),
		(char*)m_termServerInstance.c_str(),
		lpClientId,

		lpXpab,
		lpXpabc,
		lpSaleId,

		lpSaleId,
		(char*)m_targetAppName.c_str(),

		lpSaleId,
		(char*)m_targetAppName.c_str(),

		lpSaleId,
		(char*)m_targetAppName.c_str(),
		s_locationPosition,

		lpDsid,
		(char*)m_termPageId.c_str(),
		eventtime2,
		(char*)m_targetAppStoreFrontHeader.c_str(),
		lpSaleId,
		(char*)m_targetAppAppExtVrsId.c_str(),
		(char*)strUserAgent.c_str(),
		width,
		lpSaleId,
		(char*)m_termEnvironmentDataCenter.c_str(),
		lpVersion,
		(char*)m_targetAppPageDetails.c_str(),
		height,

		s_locationPosition,
		lpSaleId,
		eventtime2,//buyImpTime,
		(char*)m_targetAppName.c_str(),
		(char*)m_termPage.c_str(),
		(char*)m_termRevNum.c_str(),
		(char*)m_termServerInstance.c_str(),
		lpClientId,

		lpXpab,
		lpXpabc,
		eventtime2,
		lpDsid,
		(char*)m_termPageId.c_str(),
		(char*)strUserAgent.c_str(),
		(char*)m_termStoreFrontHeader.c_str(),
		lpSearchTerm,
		width,
		(char*)m_termEnvironmentDataCenter.c_str(),
		lpVersion,
		(char*)m_targetAppPageDetails.c_str(),
		(char*)m_termPage.c_str(),
		height,

		s_locationPosition,
		lpSaleId,
		contImpTime,
		(char*)m_targetAppName.c_str(),

		(char*)m_termRevNum.c_str(),
		(char*)m_termServerInstance.c_str(),
		lpClientId,

		//展示包
		lpVersion,
		lpDsid,
		(char*)m_termPageId.c_str(),
		impressionTime,
		s_locationPosition,
		lpSaleId,
		impressionTime,
		(char*)m_targetAppName.c_str(),
		height,
		(char*)m_termServerInstance.c_str(),
		(char*)m_termStoreFrontHeader.c_str(),
		impEventTime,
		(char*)strUserAgent.c_str(),
		(char*)m_termEnvironmentDataCenter.c_str(),
		lpXpab,
		lpXpabc,
		width,
		lpSearchTerm,
		(char*)m_termRevNum.c_str(),
		(char*)m_termPage.c_str(),
		lpClientId,

		//退出包
		lpVersion,
		lpDsid,
		height,
		(char*)m_termStoreFrontHeader.c_str(),
		exittime,
		(char*)strUserAgent.c_str(),
		(char*)m_termEnvironmentDataCenter.c_str(),
		lpXpab,
		lpXpabc,
		width,
		lpClientId,
		(char*)m_termRevNum.c_str()
		);

	memcpy(lpSrcData,pRecvBuf,strlen(pRecvBuf));

	if (hFile)
	{
		CloseHandle(hFile);
	}

	if (pRecvBuf)
	{
		delete []pRecvBuf;
		pRecvBuf = NULL;
	}

	if (pBuffer)
	{
		delete []pBuffer;
		pBuffer = NULL;
	}
	return;
}

void CViewPage::xpBuyAuthSuccessEvent(string strUserAgent,
									  char* lpVersion,
									  int width,
									  int height,
									  char* lpSrcData,
									  char* lpClientId,
									  char* lpSaleId,
									  char* lpDsid,
									  char* lpXpab,
									  char* lpXpabc,
									  char* lpMtRequestId,
									  char* lpSearchTerm,
									  char* lpAppExtVrsId,
									  char* lpEventtime)
{
	char eventtime1[MAX_PATH] = {0};
	char eventtime2[MAX_PATH] = {0};
	char responsetime[MAX_PATH] = {0};
	char posttime[MAX_PATH] = {0};
	char szPageDetails[512] = {0};
	char buffer[4096] = {0};
	char szClientCorrelationKey[MAX_PATH] = {0};
	TCHAR strPath[MAX_PATH] = {0};

	getEventTime(eventtime1,TRUE,FALSE);

	DWORD dwResult = GetModuleFileName(NULL,strPath,MAX_PATH);
	CString filePath(strPath);
	int Pos = filePath.ReverseFind(_T('\\'));
	filePath.Truncate(Pos);
	filePath += _T("\\protocol\\buyAuthSuccessEvent.txt");

	getEventTime(eventtime2,TRUE,FALSE);
	HANDLE hFile = CreateFile(filePath,GENERIC_READ,FILE_SHARE_READ,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL, NULL);
	if (!hFile)
	{
		return;
	}

	char* pRecvBuf = new char[ALLOCATE_SIZE];
	if (!pRecvBuf)
	{
		CloseHandle(hFile);
		return;
	}
	memset(pRecvBuf,0x0,ALLOCATE_SIZE);


	DWORD readLength = 1024;
	DWORD numOfByteToRead;
	DWORD totalLength = 0;
	char* pBuffer = new char[ALLOCATE_SIZE];
	if (!pBuffer)
	{
		CloseHandle(hFile);
		return;
	}

	getEventTime(responsetime,TRUE,FALSE);
	memset(pBuffer,0x0,ALLOCATE_SIZE);
	char* pTmpBuf = buffer;
	BOOL bRead = FALSE;
	while (true)
	{
		bRead = ReadFile(hFile,pTmpBuf,readLength,&numOfByteToRead,NULL);
		if (bRead && numOfByteToRead != 0)
		{
			pTmpBuf = pTmpBuf + numOfByteToRead;
			totalLength += numOfByteToRead;
		}
		else
		{
			pBuffer[totalLength] = '\0';
			break;
		}
	}

	getEventTime(posttime,FALSE,TRUE);

	//组装数据
	pTmpBuf = &buffer[0] + 3;
	sprintf(pRecvBuf,
		pTmpBuf,
		posttime,
		lpVersion,
		lpDsid,
		height,
		(char*)m_targetAppStoreFrontHeader.c_str(),
		eventtime1,
		(char*)strUserAgent.c_str(),
		(char*)m_termEnvironmentDataCenter.c_str(),
		lpXpab,
		lpXpabc,
		width,
		lpClientId,
		(char*)m_targetAppRevNum.c_str(),
		lpVersion,
		m_dwSignInScreenHeight,
		responsetime,
		m_dwSignInScreenHeight,
		(char*)m_targetAppStoreFrontHeader.c_str(),
		(char*)m_termPageId.c_str(),
		lpAppExtVrsId,
		lpSaleId,
		lpEventtime,
		lpMtRequestId,
		(char*)m_termPage.c_str(),
		lpClientId,
		lpSaleId,
		lpSearchTerm,
		eventtime2,
		m_dwSignInScreenWidth,
		(char*)strUserAgent.c_str(),
		(char*)m_termEnvironmentDataCenter.c_str(),
		lpXpab,
		m_dwSignInScreenWidth,
		lpClientId
		);

	memcpy(lpSrcData,pRecvBuf,strlen(pRecvBuf));

	if (hFile)
	{
		CloseHandle(hFile);
	}

	if (pRecvBuf)
	{
		delete []pRecvBuf;
		pRecvBuf = NULL;
	}

	if (pBuffer)
	{
		delete []pBuffer;
		pBuffer = NULL;
	}
	return;
}

void CViewPage::xpCommentAppEvent(CString strUserAgent,
								  char* lpVersion,
								  int width,
								  int height,
								  char* lpSrcData,
								  char* lpClientId,
								  char* lpSaleId,
								  char* lpDsid,
								  char* lpImpressionTime,
								  char* lpXpab,
								  char* lpXpabc)
{
	char eventtime[MAX_PATH] = {0};
	char posttime[MAX_PATH] = {0};
	char szPageDetails[512] = {0};
	char buffer[4096] = {0};
	char szUserAgent[MAX_PATH] = {0};
	char szClientCorrelationKey[MAX_PATH] = {0};
	TCHAR strPath[MAX_PATH] = {0};
	CString strAgent = strUserAgent;

	getEventTime(eventtime,TRUE,FALSE);

	WideCharToMultiByte(CP_ACP,0,strAgent.GetBuffer(),strAgent.GetLength(),szUserAgent,MAX_PATH,NULL,NULL);

	DWORD dwResult = GetModuleFileName(NULL,strPath,MAX_PATH);
	CString filePath(strPath);
	int Pos = filePath.ReverseFind(_T('\\'));
	filePath.Truncate(Pos);
	filePath += _T("\\protocol\\commentEvent.txt");

	HANDLE hFile = CreateFile(filePath,GENERIC_READ,FILE_SHARE_READ,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL, NULL);
	if (!hFile)
	{
		return;
	}

	char* pRecvBuf = new char[ALLOCATE_SIZE];
	if (!pRecvBuf)
	{
		CloseHandle(hFile);
		return;
	}
	memset(pRecvBuf,0x0,ALLOCATE_SIZE);


	DWORD readLength = 1024;
	DWORD numOfByteToRead;
	DWORD totalLength = 0;
	char* pBuffer = new char[ALLOCATE_SIZE];
	if (!pBuffer)
	{
		CloseHandle(hFile);
		return;
	}
	memset(pBuffer,0x0,ALLOCATE_SIZE);
	char* pTmpBuf = buffer;
	BOOL bRead = FALSE;
	while (true)
	{
		bRead = ReadFile(hFile,pTmpBuf,readLength,&numOfByteToRead,NULL);
		if (bRead && numOfByteToRead != 0)
		{
			pTmpBuf = pTmpBuf + numOfByteToRead;
			totalLength += numOfByteToRead;
		}
		else
		{
			pBuffer[totalLength] = '\0';
			break;
		}
	}

	getEventTime(posttime,FALSE,TRUE);

	//组装数据
	pTmpBuf = &buffer[0] + 3;
	sprintf(pRecvBuf,
		pTmpBuf,
		posttime,
		lpImpressionTime,
		lpImpressionTime,
		lpSaleId,
		(char*)m_targetAppName.c_str(),
		s_locationPosition,
		width,
		lpClientId,
		(char*)m_targetAppStoreFrontHeader.c_str(),
		(char*)m_targetAppPageDetails.c_str(),
		(char*)m_targetAppPlatformId.c_str(),
		lpDsid,
		(char*)m_targetAppServerInstance.c_str(),
		lpXpab,
		lpXpabc,
		height,
		eventtime,
		(char*)m_targetAppPlatformName.c_str(),
		(char*)m_termPage.c_str(),
		(char*)m_termPageId.c_str(),
		lpVersion,
		szUserAgent,
		(char*)m_targetAppEnvironmentDataCenter.c_str(),
		(char*)m_targetAppRevNum.c_str());

	memcpy(lpSrcData,pRecvBuf,strlen(pRecvBuf));

	if (hFile)
	{
		CloseHandle(hFile);
	}

	if (pRecvBuf)
	{
		delete []pRecvBuf;
		pRecvBuf = NULL;
	}

	if (pBuffer)
	{
		delete []pBuffer;
		pBuffer = NULL;
	}
	return;
}

void CViewPage::loginXmlData(char* lpSrcData,char* lpAppleId,char* lpPassword,char* lpUdid,char* lpKbsync,BOOL bReLogin)
{
	char buffer[4096] = {0};
	TCHAR strPath[MAX_PATH] = {0};

	DWORD dwResult = GetModuleFileName(NULL,strPath,MAX_PATH);
	CString filePath(strPath);
	int Pos = filePath.ReverseFind(_T('\\'));
	filePath.Truncate(Pos);
	if (!bReLogin)
	{
		filePath += _T("\\protocol\\loginXmlData.txt");
	}
	else
	{
		filePath += _T("\\protocol\\relogindata.txt");
	}
	HANDLE hFile = CreateFile(filePath,GENERIC_READ,FILE_SHARE_READ,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL, NULL);
	if (!hFile)
	{
		return;
	}

	char* pRecvBuf = new char[ALLOCATE_SIZE];
	if (!pRecvBuf)
	{
		CloseHandle(hFile);
		return;
	}
	memset(pRecvBuf,0x0,ALLOCATE_SIZE);


	DWORD readLength = 1024;
	DWORD numOfByteToRead;
	DWORD totalLength = 0;
	char* pBuffer = new char[ALLOCATE_SIZE];
	if (!pBuffer)
	{
		CloseHandle(hFile);
		return;
	}
	memset(pBuffer,0x0,ALLOCATE_SIZE);
	char* pTmpBuf = buffer;
	BOOL bRead = FALSE;
	while (true)
	{
		bRead = ReadFile(hFile,pTmpBuf,readLength,&numOfByteToRead,NULL);
		if (bRead && numOfByteToRead != 0)
		{
			pTmpBuf = pTmpBuf + numOfByteToRead;
			totalLength += numOfByteToRead;
		}
		else
		{
			pBuffer[totalLength] = '\0';
			break;
		}
	}

	//组装数据
	pTmpBuf = &buffer[0];
	sprintf(pRecvBuf,
		pTmpBuf,
		lpAppleId,
		lpUdid,
		lpKbsync,
		lpPassword);

	memcpy(lpSrcData,pRecvBuf,strlen(pRecvBuf));

	if (hFile)
	{
		CloseHandle(hFile);
	}

	if (pRecvBuf)
	{
		delete []pRecvBuf;
		pRecvBuf = NULL;
	}

	if (pBuffer)
	{
		delete []pBuffer;
		pBuffer = NULL;
	}
	return;
}

void CViewPage::buyProductPostData(char* lpSrcData,
								   char* lpAgreeKey, 
								   char* lpAgreeValue,
								   char* lpSubmitKey,
								   char* lpSubmitValue,
								   char* lpSecName1,
								   char* lpSecAnswer1,
								   char* lpSecName2,
								   char* lpSecAnswer2,
								   char* lpPasswordToken,
								   char* lpAppExtVrsId,
								   char* lpCreditDisplay,
								   char* lpPrice,
								   char* lpUdid,
								   char* lpKbsyncValue,
								   char* lpClientId,
								   char* lpEventtime,
								   char* lpSaleId,
								   char* lpRequestId,
								   char* lpSearchTerm,
								   char* lpDsid,
								   char* lpTermsId,
								   char* lpSbsync,
								   char* lpVid,
								   char* lpWosid_lite,
								   BOOL bPaid,
								   int buyEventType)
{
	char buffer[4096] = {0};
	TCHAR strPath[MAX_PATH] = {0};

	DWORD dwResult = GetModuleFileName(NULL,strPath,MAX_PATH);
	CString filePath(strPath);
	int Pos = filePath.ReverseFind(_T('\\'));
	filePath.Truncate(Pos);
	switch(buyEventType)
	{
	case CHANGE_CONDITION:
		{
			if (!bPaid)
			{
				filePath += _T("\\protocol\\agreeBuyProductData.txt");
			}
			else
			{
				filePath += _T("\\protocol\\agreePaidBuyProductData.txt");
			}
		}break;
	case BUY_LOGIN_VERIFY:
		{
			if (!bPaid)
			{
				filePath += _T("\\protocol\\reloginBuyData.txt");
			}
			else
			{
				filePath += _T("\\protocol\\reloginPaidBuyData.txt");
			}
		}break;
	case NEED_ALOWAYS_PWD:
		{
			if (!bPaid)
			{
				filePath += _T("\\protocol\\reloginBuyAlwayNeedPwdData.txt");
			}
			else
			{
				filePath += _T("\\protocol\\reloginPaidBuyAlwayNeedPwdData.txt");
			}
		}break;
	case SAVE_PWD_FOR_FREE_APP:
		{
			filePath += _T("\\protocol\\ForFreeAppSavePwdData.txt");
		}
	case SUBMIT_PAID_BUY_VERIFY:
		{
			filePath += _T("\\protocol\\submitPaidBuyProductData.txt");
		}break;
	case CAPTCHA_REQUIRED:
		{
			filePath += _T("\\protocol\\captchaRequiredBuyProductData.txt");
		}break;
	default:
		{
			if (!bPaid)
			{
				filePath += _T("\\protocol\\buyProductData.txt");
			}
			else
			{
				filePath += _T("\\protocol\\paidBuyProductData.txt");
			}
		}
		break;
	}

	HANDLE hFile = CreateFile(filePath,GENERIC_READ,FILE_SHARE_READ,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL, NULL);
	if (!hFile)
	{
		return;
	}

	char* pRecvBuf = new char[ALLOCATE_SIZE];
	if (!pRecvBuf)
	{
		CloseHandle(hFile);
		return;
	}
	memset(pRecvBuf,0x0,ALLOCATE_SIZE);


	DWORD readLength = 1024;
	DWORD numOfByteToRead;
	DWORD totalLength = 0;
	char* pBuffer = new char[ALLOCATE_SIZE];
	if (!pBuffer)
	{
		CloseHandle(hFile);
		return;
	}
	memset(pBuffer,0x0,ALLOCATE_SIZE);
	char* pTmpBuf = buffer;
	BOOL bRead = FALSE;
	while (true)
	{
		bRead = ReadFile(hFile,pTmpBuf,readLength,&numOfByteToRead,NULL);
		if (bRead && numOfByteToRead != 0)
		{
			pTmpBuf = pTmpBuf + numOfByteToRead;
			totalLength += numOfByteToRead;
		}
		else
		{
			pBuffer[totalLength] = '\0';
			break;
		}
	}

	//组装数据
	pTmpBuf = &buffer[0];
	switch(buyEventType)
	{
	case CHANGE_CONDITION:
		{
			if (!bPaid)
			{
				sprintf(pRecvBuf,
					pTmpBuf,
					lpAgreeKey,
					lpAgreeValue,
					lpAppExtVrsId,
					lpUdid,
					lpKbsyncValue,
					lpClientId,
					lpEventtime,
					lpSaleId,
					lpRequestId,
					lpSearchTerm,
					lpSaleId,
					lpDsid,
					lpSaleId,
					lpTermsId,
					lpSbsync,
					lpVid,
					lpWosid_lite,
					lpPasswordToken);
			}
			else
			{
				sprintf(pRecvBuf,
					pTmpBuf,
					lpAgreeKey,
					lpAgreeValue,
					lpAppExtVrsId,
					lpCreditDisplay,
					lpUdid,
					lpKbsyncValue,
					lpClientId,
					lpEventtime,
					lpSaleId,
					lpRequestId,
					lpSearchTerm,
					lpSaleId,
					lpDsid,
					lpPrice,
					lpSaleId,
					lpTermsId,
					lpSbsync,
					lpVid,
					lpWosid_lite,
					lpPasswordToken);
			}
		}break;
	case BUY_LOGIN_VERIFY:
		{
			if (!bPaid)
			{
				sprintf(pRecvBuf,
					pTmpBuf,
					lpAppExtVrsId,
					lpUdid,
					lpKbsyncValue,
					lpClientId,
					lpEventtime,
					(char*)m_termPageId.c_str(),
					lpSaleId,
					lpRequestId,
					lpSearchTerm,
					(char*)m_termPage.c_str(),
					lpSaleId,
					lpSbsync);
			}
			else
			{
				sprintf(pRecvBuf,
					pTmpBuf,
					lpAppExtVrsId,
					lpCreditDisplay,
					lpUdid,
					lpKbsyncValue,
					lpClientId,
					lpEventtime,
					lpSaleId,
					lpRequestId,
					lpSearchTerm,
					lpSaleId,
					lpPrice,
					lpSaleId,
					lpSbsync,
					lpVid);
			}
		}break;
	case NEED_ALOWAYS_PWD:
		{
			if (!bPaid)
			{
				sprintf(pRecvBuf,
					pTmpBuf,
					lpAppExtVrsId,
					m_asn,
					lpUdid,
					lpKbsyncValue,
					lpClientId,
					lpEventtime,
					(char*)m_termPageId.c_str(),
					lpSaleId,
					lpRequestId,
					lpSearchTerm,
					(char*)m_termPage.c_str(),
					lpSaleId,
					lpSbsync);
			}
			else
			{
				sprintf(pRecvBuf,
					pTmpBuf,
					lpAppExtVrsId,
					lpCreditDisplay,
					lpUdid,
					lpKbsyncValue,
					lpClientId,
					lpEventtime,
					lpSaleId,
					lpRequestId,
					lpSearchTerm,
					lpSaleId,
					lpPrice,
					lpSaleId,
					lpSbsync,
					lpVid);
			}
		}break;
	case SAVE_PWD_FOR_FREE_APP:
		{
			sprintf(pRecvBuf,
				pTmpBuf,
				lpAppExtVrsId,
				m_asn,
				lpUdid,
				lpKbsyncValue,
				lpClientId,
				lpEventtime,
				(char*)m_termPageId.c_str(),
				lpSaleId,
				lpRequestId,
				lpSearchTerm,
				(char*)m_termPage.c_str(),
				lpSaleId,
				lpSbsync);
		}break;
	case SUBMIT_PAID_BUY_VERIFY:
		{
			sprintf(pRecvBuf,
				pTmpBuf,
				lpSubmitKey,
				lpSubmitValue,
				lpSecName1,
				lpSecAnswer1,
				lpSecName2,
				lpSecAnswer2,
				lpAppExtVrsId,
				lpCreditDisplay,
				lpUdid,
				lpKbsyncValue,
				lpClientId,
				lpEventtime,
				lpSaleId,
				lpRequestId,
				lpSaleId,
				lpDsid,
				lpPrice,
				lpSaleId,
				lpTermsId,
				lpSbsync,
				lpVid,
				lpWosid_lite,
				lpPasswordToken);	
		}break;
	case CAPTCHA_REQUIRED:
		{
			sprintf(pRecvBuf,
				pTmpBuf,
				lpAppExtVrsId,
				lpUdid,
				lpKbsyncValue,
				lpClientId,
				lpEventtime,
				lpRequestId,
				lpSearchTerm,
				lpSaleId,
				lpSbsync,
				lpPasswordToken);

		}break;
	default:
		{
			if (!bPaid)
			{
				sprintf(pRecvBuf,
					pTmpBuf,
					lpAppExtVrsId,
					lpUdid,
					lpKbsyncValue,
					lpClientId,
					lpEventtime,
					(char*)m_termPageId.c_str(),
					lpSaleId,//mtPrePage
					lpRequestId,
					lpSearchTerm,
					(char*)m_termPage.c_str(),
					lpSaleId,
					lpSbsync);
			}
			else
			{
				sprintf(pRecvBuf,
					pTmpBuf,
					lpAppExtVrsId,
					lpCreditDisplay,
					lpUdid,
					lpKbsyncValue,
					lpClientId,
					lpEventtime,
					lpSaleId,
					lpRequestId,
					lpSearchTerm,
					lpSaleId,
					lpPrice,
					lpSaleId,
					lpSbsync,
					lpVid);
			}
		}
		break;
	}
	memcpy(lpSrcData,pRecvBuf,strlen(pRecvBuf));

	if (hFile)
	{
		CloseHandle(hFile);
	}

	if (pRecvBuf)
	{
		delete []pRecvBuf;
		pRecvBuf = NULL;
	}

	if (pBuffer)
	{
		delete []pBuffer;
		pBuffer = NULL;
	}
	return;
}

void CViewPage::getRegisterData(char* lpSrcData,
								char* lpSerialNumber,
								char* lpRegisterToken,
								char* lpUdid,
								char* lpDeviceNameData)
{
	char buffer[1024] = {0};
	TCHAR strPath[MAX_PATH] = {0};

	DWORD dwResult = GetModuleFileName(NULL,strPath,MAX_PATH);
	CString filePath(strPath);
	int Pos = filePath.ReverseFind(_T('\\'));
	filePath.Truncate(Pos);

	filePath += _T("\\protocol\\registerData.txt");

	HANDLE hFile = CreateFile(filePath,GENERIC_READ,FILE_SHARE_READ,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL, NULL);
	if (!hFile)
	{
		return;
	}

	char* pRecvBuf = new char[ALLOCATE_SIZE];
	if (!pRecvBuf)
	{
		CloseHandle(hFile);
		return;
	}
	memset(pRecvBuf,0x0,ALLOCATE_SIZE);


	DWORD readLength = 1024;
	DWORD numOfByteToRead;
	DWORD totalLength = 0;
	char* pBuffer = new char[ALLOCATE_SIZE];
	if (!pBuffer)
	{
		CloseHandle(hFile);
		return;
	}
	memset(pBuffer,0x0,ALLOCATE_SIZE);
	char* pTmpBuf = buffer;
	BOOL bRead = FALSE;
	while (true)
	{
		bRead = ReadFile(hFile,pTmpBuf,readLength,&numOfByteToRead,NULL);
		if (bRead && numOfByteToRead != 0)
		{
			pTmpBuf = pTmpBuf + numOfByteToRead;
			totalLength += numOfByteToRead;
		}
		else
		{
			pBuffer[totalLength] = '\0';
			break;
		}
	}

	//组装数据
	pTmpBuf = &buffer[0];
	sprintf(pRecvBuf,
		pTmpBuf,
		lpDeviceNameData,
		lpUdid,
		lpSerialNumber,
		lpRegisterToken);

	memcpy(lpSrcData,pRecvBuf,strlen(pRecvBuf));

	if (hFile)
	{
		CloseHandle(hFile);
	}

	if (pRecvBuf)
	{
		delete []pRecvBuf;
		pRecvBuf = NULL;
	}

	if (pBuffer)
	{
		delete []pBuffer;
		pBuffer = NULL;
	}
	return;
}

string CViewPage::getShowPageIdsData()
{
	string strPageIdsData = "";
	char buffer[2048] = {0};
	TCHAR strPath[MAX_PATH] = {0};

	DWORD dwResult = GetModuleFileName(NULL,strPath,MAX_PATH);
	CString filePath(strPath);
	int Pos = filePath.ReverseFind(_T('\\'));
	filePath.Truncate(Pos);

	filePath += _T("\\protocol\\showPageIds.txt");

	HANDLE hFile = CreateFile(filePath,GENERIC_READ,FILE_SHARE_READ,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL, NULL);
	if (!hFile)
	{
		return "";
	}

	DWORD readLength = 1024;
	DWORD numOfByteToRead;
	DWORD totalLength = 0;
	char* pTmpBuf = buffer;
	BOOL bRead = FALSE;
	while (true)
	{
		bRead = ReadFile(hFile,pTmpBuf,readLength,&numOfByteToRead,NULL);
		if (bRead && numOfByteToRead != 0)
		{
			pTmpBuf = pTmpBuf + numOfByteToRead;
			totalLength += numOfByteToRead;
		}
		else
		{
			pTmpBuf[totalLength] = '\0';
			break;
		}
	}
	strPageIdsData = buffer;

	return strPageIdsData;
}

void CViewPage::viewAccountXmlData(char* lpSrcData,char* lpAppleId,char* lpPassword,char* lpUdid,char* lpKbsync)
{
	char buffer[4096] = {0};
	TCHAR strPath[MAX_PATH] = {0};

	DWORD dwResult = GetModuleFileName(NULL,strPath,MAX_PATH);
	CString filePath(strPath);
	int Pos = filePath.ReverseFind(_T('\\'));
	filePath.Truncate(Pos);
	filePath += _T("\\protocol\\viewAccountXmlData.txt");
	HANDLE hFile = CreateFile(filePath,GENERIC_READ,FILE_SHARE_READ,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL, NULL);
	if (!hFile)
	{
		return;
	}

	char* pRecvBuf = new char[ALLOCATE_SIZE];
	if (!pRecvBuf)
	{
		CloseHandle(hFile);
		return;
	}
	memset(pRecvBuf,0x0,ALLOCATE_SIZE);


	DWORD readLength = 1024;
	DWORD numOfByteToRead;
	DWORD totalLength = 0;
	char* pBuffer = new char[ALLOCATE_SIZE];
	if (!pBuffer)
	{
		CloseHandle(hFile);
		return;
	}
	memset(pBuffer,0x0,ALLOCATE_SIZE);
	char* pTmpBuf = buffer;
	BOOL bRead = FALSE;
	while (true)
	{
		bRead = ReadFile(hFile,pTmpBuf,readLength,&numOfByteToRead,NULL);
		if (bRead && numOfByteToRead != 0)
		{
			pTmpBuf = pTmpBuf + numOfByteToRead;
			totalLength += numOfByteToRead;
		}
		else
		{
			pBuffer[totalLength] = '\0';
			break;
		}
	}

	//组装数据
	pTmpBuf = &buffer[0];
	sprintf(pRecvBuf,
		pTmpBuf,
		lpAppleId,
		lpUdid,
		lpKbsync,
		lpPassword);

	memcpy(lpSrcData,pRecvBuf,strlen(pRecvBuf));

	if (hFile)
	{
		CloseHandle(hFile);
	}

	if (pRecvBuf)
	{
		delete []pRecvBuf;
		pRecvBuf = NULL;
	}

	if (pBuffer)
	{
		delete []pBuffer;
		pBuffer = NULL;
	}
	return;
}

void CViewPage::getFinishMachineProvisioningData(char* lpSrcData,
												 char* lpClientData,
												 char* lpDsid,
												 char* lpUdid,
												 char* lpKbsync)
{
	char buffer[4096] = {0};
	TCHAR strPath[MAX_PATH] = {0};

	DWORD dwResult = GetModuleFileName(NULL,strPath,MAX_PATH);
	CString filePath(strPath);
	int Pos = filePath.ReverseFind(_T('\\'));
	filePath.Truncate(Pos);
	if (lpKbsync)
	{
		filePath += _T("\\protocol\\finishMachineProvisionData.txt");
	}
	else
	{
		filePath += _T("\\protocol\\anonymousFinishProvisionData.txt");
	}
	HANDLE hFile = CreateFile(filePath,GENERIC_READ,FILE_SHARE_READ,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL, NULL);
	if (!hFile)
	{
		return;
	}

	char* pRecvBuf = new char[ALLOCATE_SIZE];
	if (!pRecvBuf)
	{
		CloseHandle(hFile);
		return;
	}
	memset(pRecvBuf,0x0,ALLOCATE_SIZE);


	DWORD readLength = 1024;
	DWORD numOfByteToRead;
	DWORD totalLength = 0;
	char* pBuffer = new char[ALLOCATE_SIZE];
	if (!pBuffer)
	{
		CloseHandle(hFile);
		return;
	}
	memset(pBuffer,0x0,ALLOCATE_SIZE);
	char* pTmpBuf = buffer;
	BOOL bRead = FALSE;
	while (true)
	{
		bRead = ReadFile(hFile,pTmpBuf,readLength,&numOfByteToRead,NULL);
		if (bRead && numOfByteToRead != 0)
		{
			pTmpBuf = pTmpBuf + numOfByteToRead;
			totalLength += numOfByteToRead;
		}
		else
		{
			pBuffer[totalLength] = '\0';
			break;
		}
	}

	//组装数据
	pTmpBuf = &buffer[0];
	if (lpKbsync)
	{
		sprintf(pRecvBuf,
			pTmpBuf,
			lpClientData,
			lpDsid,
			lpUdid,
			lpKbsync);
	}
	else
	{
		sprintf(pRecvBuf,
			pTmpBuf,
			lpClientData,
			lpUdid);
	}

	memcpy(lpSrcData,pRecvBuf,strlen(pRecvBuf));

	if (hFile)
	{
		CloseHandle(hFile);
	}

	if (pRecvBuf)
	{
		delete []pRecvBuf;
		pRecvBuf = NULL;
	}

	if (pBuffer)
	{
		delete []pBuffer;
		pBuffer = NULL;
	}
	return;
}

void CViewPage::getA2kPostData(char* lpSrcData,
							   char* lpA2k,
							   char* lpAppleIDClientIdentifier,
							   char* lpX_Apple_I_Client_Time,
							   char* lpX_Apple_I_md,
							   char* lpX_Apple_I_md_m,
							   char* lpUdid,
							   char* lpAppleId)
{
	char buffer[4096] = {0};
	TCHAR strPath[MAX_PATH] = {0};

	DWORD dwResult = GetModuleFileName(NULL,strPath,MAX_PATH);
	CString filePath(strPath);
	int Pos = filePath.ReverseFind(_T('\\'));
	filePath.Truncate(Pos);

	filePath += _T("\\protocol\\a2kPostData.txt");


	HANDLE hFile = CreateFile(filePath,GENERIC_READ,FILE_SHARE_READ,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL, NULL);
	if (!hFile)
	{
		return;
	}

	char* pRecvBuf = new char[ALLOCATE_SIZE];
	if (!pRecvBuf)
	{
		CloseHandle(hFile);
		return;
	}
	memset(pRecvBuf,0x0,ALLOCATE_SIZE);


	DWORD readLength = 1024;
	DWORD numOfByteToRead;
	DWORD totalLength = 0;
	char* pBuffer = new char[ALLOCATE_SIZE];
	if (!pBuffer)
	{
		CloseHandle(hFile);
		return;
	}
	memset(pBuffer,0x0,ALLOCATE_SIZE);
	char* pTmpBuf = buffer;
	BOOL bRead = FALSE;
	while (true)
	{
		bRead = ReadFile(hFile,pTmpBuf,readLength,&numOfByteToRead,NULL);
		if (bRead && numOfByteToRead != 0)
		{
			pTmpBuf = pTmpBuf + numOfByteToRead;
			totalLength += numOfByteToRead;
		}
		else
		{
			pBuffer[totalLength] = '\0';
			break;
		}
	}

	//组装数据
	pTmpBuf = &buffer[0];

	sprintf(pRecvBuf,
		pTmpBuf,
		lpA2k,
		(char*)m_strAppleIDClientIdentifier.c_str(),
		lpX_Apple_I_Client_Time,
		lpX_Apple_I_md,
		lpX_Apple_I_md_m,
		(char*)m_strSerialNumber.c_str(),
		lpUdid,
		lpAppleId);


	memcpy(lpSrcData,pRecvBuf,strlen(pRecvBuf));

	if (hFile)
	{
		CloseHandle(hFile);
	}

	if (pRecvBuf)
	{
		delete []pRecvBuf;
		pRecvBuf = NULL;
	}

	if (pBuffer)
	{
		delete []pBuffer;
		pBuffer = NULL;
	}
	return;
}

void CViewPage::getM1PostData(char* lpSrcData,
							  char* lpC,
							  char* lpM1,
							  char* lpAppleIDClientIdentifier,
							  char* lpX_Apple_I_Client_Time,
							  char* lpX_Apple_I_md,
							  char* lpX_Apple_I_md_m,
							  char* lpUdid,
							  char* lpAppleId)
{
	char buffer[4096] = {0};
	TCHAR strPath[MAX_PATH] = {0};

	DWORD dwResult = GetModuleFileName(NULL,strPath,MAX_PATH);
	CString filePath(strPath);
	int Pos = filePath.ReverseFind(_T('\\'));
	filePath.Truncate(Pos);

	filePath += _T("\\protocol\\m1PostData.txt");


	HANDLE hFile = CreateFile(filePath,GENERIC_READ,FILE_SHARE_READ,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL, NULL);
	if (!hFile)
	{
		return;
	}

	char* pRecvBuf = new char[ALLOCATE_SIZE];
	if (!pRecvBuf)
	{
		CloseHandle(hFile);
		return;
	}
	memset(pRecvBuf,0x0,ALLOCATE_SIZE);


	DWORD readLength = 1024;
	DWORD numOfByteToRead;
	DWORD totalLength = 0;
	char* pBuffer = new char[ALLOCATE_SIZE];
	if (!pBuffer)
	{
		CloseHandle(hFile);
		return;
	}
	memset(pBuffer,0x0,ALLOCATE_SIZE);
	char* pTmpBuf = buffer;
	BOOL bRead = FALSE;
	while (true)
	{
		bRead = ReadFile(hFile,pTmpBuf,readLength,&numOfByteToRead,NULL);
		if (bRead && numOfByteToRead != 0)
		{
			pTmpBuf = pTmpBuf + numOfByteToRead;
			totalLength += numOfByteToRead;
		}
		else
		{
			pBuffer[totalLength] = '\0';
			break;
		}
	}

	//组装数据
	pTmpBuf = &buffer[0];

	sprintf(pRecvBuf,
		pTmpBuf,
		lpM1,
		lpC,
		(char*)m_strAppleIDClientIdentifier.c_str(),
		lpX_Apple_I_Client_Time,
		lpX_Apple_I_md,
		lpX_Apple_I_md_m,
		(char*)m_strSerialNumber.c_str(),
		lpUdid,
		lpAppleId);


	memcpy(lpSrcData,pRecvBuf,strlen(pRecvBuf));

	if (hFile)
	{
		CloseHandle(hFile);
	}

	if (pRecvBuf)
	{
		delete []pRecvBuf;
		pRecvBuf = NULL;
	}

	if (pBuffer)
	{
		delete []pBuffer;
		pBuffer = NULL;
	}
	return;
}

void CViewPage::getGsasPostData(char* lpSrcData, 
								char* lpIMEI, 
								char* lpMEID, 
								char* lpSerialNo)
{
	char  buffer[4096] = {0};
	TCHAR strPath[MAX_PATH] = {0};

	DWORD dwResult = GetModuleFileName(NULL,strPath,MAX_PATH);
	CString filePath(strPath);
	int Pos = filePath.ReverseFind(_T('\\'));
	filePath.Truncate(Pos);

	filePath += _T("\\protocol\\gsasPostData.txt");


	HANDLE hFile = CreateFile(filePath,GENERIC_READ,FILE_SHARE_READ,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL, NULL);
	if (!hFile)
	{
		return;
	}

	char* pRecvBuf = new char[ALLOCATE_SIZE];
	if (!pRecvBuf)
	{
		CloseHandle(hFile);
		return;
	}
	memset(pRecvBuf,0x0,ALLOCATE_SIZE);


	DWORD readLength = 1024;
	DWORD numOfByteToRead;
	DWORD totalLength = 0;
	char* pBuffer = new char[ALLOCATE_SIZE];
	if (!pBuffer)
	{
		CloseHandle(hFile);
		return;
	}
	memset(pBuffer,0x0,ALLOCATE_SIZE);
	char* pTmpBuf = buffer;
	BOOL bRead = FALSE;
	while (true)
	{
		bRead = ReadFile(hFile,pTmpBuf,readLength,&numOfByteToRead,NULL);
		if (bRead && numOfByteToRead != 0)
		{
			pTmpBuf = pTmpBuf + numOfByteToRead;
			totalLength += numOfByteToRead;
		}
		else
		{
			pBuffer[totalLength] = '\0';
			break;
		}
	}

	//组装数据
	pTmpBuf = &buffer[0];

	sprintf(pRecvBuf,
		pTmpBuf,
		lpIMEI,
		lpMEID,
		lpSerialNo);

	memcpy(lpSrcData,pRecvBuf,strlen(pRecvBuf));

	if (hFile)
	{
		CloseHandle(hFile);
	}

	if (pRecvBuf)
	{
		delete []pRecvBuf;
		pRecvBuf = NULL;
	}

	if (pBuffer)
	{
		delete []pBuffer;
		pBuffer = NULL;
	}
	return;
}
