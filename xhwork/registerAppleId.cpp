#include "stdafx.h"
#include "registerAppleId.h"
#include "Util.h"
#include "secdata.h"
#include "statusData.h"
#include "GZipHelper.h"
#pragma   comment   (lib,"ZLIB.LIB")


ManagerRegister::ManagerRegister(TAGREGISTERINFO& tagRegisterInfo)
{
	m_lpSignSapSetupBuffer = new char[ALLOCATE_SIZE+1];
	m_lpXAppleActionSignature = new char[SIGNATURE_SIZE];

	m_tagRegisterInfo = tagRegisterInfo;
}

ManagerRegister::~ManagerRegister(void)
{
	if (m_lpSignSapSetupBuffer)
	{
		delete []m_lpSignSapSetupBuffer;
		m_lpSignSapSetupBuffer = NULL;
	}
	if (m_lpXAppleActionSignature)
	{
		delete []m_lpXAppleActionSignature;
		m_lpXAppleActionSignature = NULL;
	}
}

char* ManagerRegister::getSignSapSetupData(void)
{
	return m_lpSignSapSetupBuffer;
}

char* ManagerRegister::getXAppleActionsignatureData(void)
{
	return m_lpXAppleActionSignature;
}

CString& ManagerRegister::getAgreeRegWebActionName(void)
{
	return m_strAgreeRegWebActionName;
}

CString& ManagerRegister::getRichInfoWebActionName(void)
{
	return m_strRichInfoWebActionName;
}

CString& ManagerRegister::getCreditWebActionName(void)
{
	return m_strCreditWebActionName;
}

CString& ManagerRegister::getSubmitCreditInfoWebActionName(void)
{
	return m_strSubmitCreditInfoWebActionName;
}
CString& ManagerRegister::getNextStepName(void)
{
	return m_strNextStepName;
}

CString& ManagerRegister::getMachineGUID(void)
{
	return m_tagRegisterInfo.strMachineGuid;
}
CString& ManagerRegister::getRefererUrl(void)
{
	return m_strRefererUrl;
}

CString& ManagerRegister::getAgreeWebPageUUID(void)
{
	return m_strSrcAgreeWebPageUUID;
}
CString& ManagerRegister::getRichInfoWebPageUUID(void)
{
	return m_richWebPageUUID;
}
CString& ManagerRegister::getCreditInfoWebPageUUID(void)
{
	return m_creditWebPageUUID;
}

CString& ManagerRegister::getRegisterAppleId(void)
{
	CString strAppleId;
	for (AccountInfo::iterator ptr = m_tagRegisterInfo.accountInfo.begin(); ptr != m_tagRegisterInfo.accountInfo.end(); ptr++)
	{
		strAppleId = ptr->strAppleId;
	}
	return strAppleId;
}

CStringA& ManagerRegister::generateBasePostData(char* lpXAppleActionSignature)
{
	CStringA strDataA;
	USES_CONVERSION;
	CString strXAppleActionSignature = Util::URLEncode2(A2T(lpXAppleActionSignature));
	strDataA.Format("%s=%E5%90%8C%E6%84%8F&machineGUID=%s&xAppleActionSignature=%s&mzPageUUID=%s",
		T2A(m_strAgreeName),T2A(m_tagRegisterInfo.strMachineGuid),T2A(strXAppleActionSignature),T2A(m_strAgreeWebPageUUID));

	return strDataA;
}

CStringA& ManagerRegister::generateRichInfoPostData(char* lpXAppleActionSignature)
{
	CStringA strDataA;
	USES_CONVERSION;
	CString strAppleId;
	CString strPwd;
	for (AccountInfo::iterator ptr = m_tagRegisterInfo.accountInfo.begin(); ptr != m_tagRegisterInfo.accountInfo.end(); ptr++)
	{
		strAppleId = Util::URLEncode2(ptr->strAppleId);
		strPwd = ptr->strPassword;
	}
	CString strXAppleActionSignature = Util::URLEncode2(A2T(lpXAppleActionSignature));
	strDataA.Format("mzPageUUID=%s&machineGUID=%s&xAppleActionSignature=%s&%s=%s&%s=%s&%s=%s&%s=142&%s=%s&%s=135&%s=%s&%s=137&%s=%s&%s=&%s=%s&%s=%d&%s=%d&%s=%E4%B8%8B%E4%B8%80%E6%AD%A5",
		T2A(m_regRichWebInfo.strPageUUIDValue),
		T2A(m_tagRegisterInfo.strMachineGuid),
		T2A(strXAppleActionSignature),
		T2A(m_regRichWebInfo.strAppleIdName),
		T2A(strAppleId),
		T2A(m_regRichWebInfo.strPasswordName),
		T2A(strPwd),
		T2A(m_regRichWebInfo.strVerifyPwdName),
		T2A(strPwd),
		T2A(m_regRichWebInfo.strQuestion1Name),
		T2A(m_regRichWebInfo.strAnswer1Name),
		T2A(m_tagRegisterInfo.strAnswer1),
		T2A(m_regRichWebInfo.strQuestion2Name),
		T2A(m_regRichWebInfo.strAnswer2Name),
		T2A(m_tagRegisterInfo.strAnswer2),
		T2A(m_regRichWebInfo.strQuestion3Name),
		T2A(m_regRichWebInfo.strAnswer3Name),
		T2A(m_tagRegisterInfo.strAnswer3),
		T2A(m_tagRegisterInfo.strRescueMail),
		T2A(m_regRichWebInfo.strBirthYearName),
		T2A(m_tagRegisterInfo.strYear),
		T2A(m_regRichWebInfo.strBirthMonthName),
		m_tagRegisterInfo.nMonth,	
		T2A(m_regRichWebInfo.strBirthDayName),
		m_tagRegisterInfo.nDay,
		T2A(m_regRichWebInfo.strNextStepName));

	return strDataA;
}

CStringA& ManagerRegister::generateCreditPostData(char* lpXAppleActionSignature)
{
	CStringA strDataA;
	CString strNdp_wkr;

	strNdp_wkr.Format(TEXT("%d"),(310000+rand()%700000));
	USES_CONVERSION;
	CString strCardTypeUrl = Util::URLEncode2(TEXT("https://play.itunes.apple.com/WebObjects/MZPlay.woa/wa/getCardTypeSrv"));
	CString strXAppleActionSignature = Util::URLEncode2(A2T(lpXAppleActionSignature));

	strDataA.Format("mzPageUUID=%s&machineGUID=%s&xAppleActionSignature=%s&getCardTypeUrl=%s&card-type-id=19&%s=&res=&ccv=&cc-number=&ndpd-wkr=%s&%s=%s&%s=%s&%s=%s&%s=%s&%s=%s&%s=%s&%s=%s&%s=%s&state=%d&longName=%s%s&captchaMode=VIDEO&ndpd-di=d1-244b2c81ae427263&ndpd-ipr=&cc-type-hddn=PEAS&ndpd-bi=b1.320x568 320x548 32 32.-480.zh-cn&ndpd-vk=25691&ndpd-wk=p&sp=&cc-expr-year=%s&cc-expr-month=%s&ndpd-w=%s&ndpd-s=%s&ndpd-fm=%s&ndpd-f=%s&%s=%E4%B8%8B%E4%B8%80%E6%AD%A5",
		T2A(m_regCreditWebInfo.strPageUUIDValue),
		T2A(m_tagRegisterInfo.strMachineGuid),
		T2A(strXAppleActionSignature),
		T2A(strCardTypeUrl),
		T2A(m_regCreditWebInfo.strCoderedemptName),
		T2A(strNdp_wkr),
		T2A(m_regCreditWebInfo.strPhoneNumberName),
		T2A(m_tagRegisterInfo.strPhoneNum),
		T2A(m_regCreditWebInfo.strLastName),
		T2A(m_tagRegisterInfo.strLastName),
		T2A(m_regCreditWebInfo.strFirstName),
		T2A(m_tagRegisterInfo.strFisrtName),
		T2A(m_regCreditWebInfo.strStreet1Name),
		T2A(m_tagRegisterInfo.submitInfo.strStreet1),
		T2A(m_regCreditWebInfo.strStreet2Name),
		T2A(m_tagRegisterInfo.submitInfo.strStreet2),
		T2A(m_regCreditWebInfo.strStreet3Name),
		T2A(m_tagRegisterInfo.submitInfo.strStreet3),
		T2A(m_regCreditWebInfo.strPostalCodeName),
		T2A(m_tagRegisterInfo.submitInfo.strPostNum),
		T2A(m_regCreditWebInfo.strCityName),
		T2A(m_tagRegisterInfo.submitInfo.strCity),
		m_tagRegisterInfo.nProvince,
		T2A(m_tagRegisterInfo.strLastName),
		T2A(m_tagRegisterInfo.strFisrtName),
		T2A(m_regCreditWebInfo.strCCExprYearValue),
		T2A(m_regCreditWebInfo.strCCExprYearValue),
		T2A(m_regCreditWebInfo.strNdpd_w_Value),
		T2A(m_regCreditWebInfo.strNdpd_s_Value),
		T2A(m_regCreditWebInfo.strNdpd_fm_Value),
		T2A(m_regCreditWebInfo.strNdpd_f_Value),
		T2A(m_regCreditWebInfo.strNextStepName)
		);

	return strDataA;
}

void ManagerRegister::extractSignSapSetup(HINTERNET hOpenReq)
{
	LPGZIP			lpgzipBuf = NULL;
	DWORD			dataLength = 0;
	DWORD			dwReadLength = 0;
	DWORD			dwMaxDataLength = 200;
	BOOL			bReadFile = FALSE;
	char			DataBuffer[200] = {0};
	char*			pDecodeData = NULL;
	int				decodeLen = 0;
	char			firstSignKey[] = "<data>";
	char			lastSignKey[] = "</data>";
	char*			ptr_1 = NULL;
	char*			ptr_2 = NULL;
	int				signSapLength = 0;
	int				signKeyLength = strlen(firstSignKey);
	DWORD			dwContentLength = 0;
	
	if ( !InternetQueryDataAvailable(hOpenReq,&dwContentLength,0,0) )
	{
		dwContentLength = 1024*4;//分配4K的虚拟内存
	}

	//分配虚拟内存保存页面数据
	lpgzipBuf =  (LPGZIP)VirtualAlloc(NULL,dwContentLength,MEM_COMMIT | MEM_RESERVE,PAGE_EXECUTE_READWRITE);
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
			return;

		}

		ptr_1 = strstr(pDecodeData,firstSignKey);
		ptr_1 = ptr_1 + signKeyLength;			
		ptr_2 = strstr(pDecodeData,lastSignKey);

		signSapLength = ptr_2 - ptr_1;

		//第一次从服务器读到的sign-sap-setup-cert
		memset(m_lpSignSapSetupBuffer,0,ALLOCATE_SIZE+1);
		memcpy(m_lpSignSapSetupBuffer,ptr_1,signSapLength);
		
	}

	if (lpgzipBuf)
	{
		VirtualFree(lpgzipBuf,0,MEM_RELEASE);
		lpgzipBuf = NULL;
	}

	return;
}


void ManagerRegister::extractResponseXAppleActionsignature(HINTERNET hOpenReq)
{
	char		szSignDataBuffer[MAX_PATH] = {0};
	TCHAR		uzBuffer[512] = {0};
	TCHAR*		pHeaderBuf = NULL;
	DWORD		dwLen = 0;
	TCHAR*		pBegin = NULL;
	TCHAR*		pEnd = NULL;
	TCHAR*      pTmp = NULL;
	TCHAR		uzSignKey[] = TEXT("x-apple-actionsignature: ");

	BOOL bRet = HttpQueryInfo(hOpenReq,HTTP_QUERY_RAW_HEADERS_CRLF,NULL,&dwLen,NULL);
	if (!bRet)
	{
		pHeaderBuf = new TCHAR[dwLen*2+2];
		memset(pHeaderBuf,0,dwLen*2+2);
		HttpQueryInfo(hOpenReq,HTTP_QUERY_RAW_HEADERS_CRLF,pHeaderBuf,&dwLen,NULL);
		pTmp = pHeaderBuf + 100;
		pBegin = wcsstr(pTmp,uzSignKey);
		if (pBegin > 0)
		{
			pBegin = pBegin + wcslen(uzSignKey);
			pEnd = wcsstr(pBegin,TEXT("\n"));
			if (pEnd > 0)
			{
				dwLen = pEnd - pBegin;
				wmemcpy(uzBuffer,pBegin,dwLen);
			}

		}

		WideCharToMultiByte(CP_ACP,0,uzBuffer,dwLen,szSignDataBuffer,MAX_PATH,NULL,NULL);

		memcpy(m_lpXAppleActionSignature,szSignDataBuffer,strlen(szSignDataBuffer));

		if (pHeaderBuf)
		{
			delete []pHeaderBuf;
			pHeaderBuf = NULL;
		}
	}
	
	return;
}

BOOL ManagerRegister::sendSignSapSetupCert(CString strUrl,CString strRefererUrl,CString strVerb,CString strUserAgent,CString strHostName)
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


	BOOL bCrackUrl = InternetCrackUrl(strUrl,0,0,&uc);
	if (bCrackUrl)
	{
		if (m_netType == PROXY_NETWORK)
		{
			hSession = InternetOpen(strUserAgent,INTERNET_OPEN_TYPE_PROXY,m_strProxy,0,0);
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
		hOpenReq = HttpOpenRequest(hConnect,strVerb,strHostName,0,0,0,0x84801000,0);//0x84A01000是一个常数,注意
		if (!hOpenReq)
		{
			InternetCloseHandle(hConnect);
			InternetCloseHandle(hSession);
			return FALSE;
		}

		CString strHeaders = _T("User-Agent: ");
		strHeaders += strUserAgent;
		strHeaders += _T("\r\nReferer: ");
		strHeaders += strRefererUrl;
		strHeaders += _T("\r\nAccept-Language: zh-cn, zh;q=0.5");
		strHeaders += _T("\r\nX-Apple-Store-Front: 143465-19,29");
		strHeaders += _T("\r\nX-Apple-Tz: 28800");
		strHeaders += _T("\r\nAccept-Encoding: gzip");
		strHeaders += _T("\r\nConnection: Close\r\n\r\n");	
		
		DWORD dwHeaderLength = wcslen(strHeaders);	//194

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
		DWORD dwSize;

		bQueryInfo = HttpQueryInfo(hOpenReq,0x20000013,&dwQueryBuf,&dwQueryBufferLength,NULL);//HttpQueryInfo 获取文件大小后
		if (bQueryInfo)
		{
			if (dwQueryBuf == 200)
			{
				//提取sign-sap-setup-cert的值
				extractSignSapSetup(hOpenReq);
			}

		}
		else
		{
			InternetCloseHandle(hOpenReq);
			InternetCloseHandle(hConnect);
			InternetCloseHandle(hSession);
			bResult = FALSE;
		}

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

BOOL ManagerRegister::sendSignSapSetup(CString strUrl,CString strRefererUrl,CString strVerb,CString strUserAgent,CString strHostName,char* lpSapBuf)
{
	char SendData[2048] = {0};
	DWORD sendLength = 0;
	char constStr[] = "<?xml version=\"1.0\" encoding=\"UTF-8\"?><plist version=\"1.0\"><dict><key>sign-sap-setup-buffer</key><data>%s</data></dict></plist>";
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


	BOOL bCrackUrl = InternetCrackUrl(strUrl,0,0,&uc);
	if (bCrackUrl)
	{
		if (m_netType == PROXY_NETWORK)
		{
			hSession = InternetOpen(strUserAgent,INTERNET_OPEN_TYPE_PROXY,m_strProxy,0,0);
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

		hOpenReq = HttpOpenRequest(hConnect,strVerb,strHostName,0,0,0,0x84801000,0); //0x84A01000是一个常数
		if (!hOpenReq)
		{
			InternetCloseHandle(hConnect);
			InternetCloseHandle(hSession);
			return FALSE;
		}

		CString strHeaders = _T("User-Agent: ");
		strHeaders += strUserAgent;
		strHeaders += _T("\r\nContent-Type: application/x-apple-plist");
		strHeaders += _T("\r\nReferer: ");
		strHeaders += strRefererUrl;
		strHeaders += _T("\r\nAccept-Language: zh-cn, zh;q=0.5");
		strHeaders += _T("\r\nX-Apple-Store-Front: 143465-19,29");
		strHeaders += _T("\r\nX-Apple-Tz: 28800");
		strHeaders += _T("\r\nAccept-Encoding: gzip");
		strHeaders += _T("\r\nConnection: Close\r\n\r\n");	
		
		DWORD dwHeaderLength = wcslen(strHeaders);
					
		bAddRequestHeader = HttpAddRequestHeaders(hOpenReq,strHeaders,dwHeaderLength,0xA0000000);//0xA0000000是一个常量
		if (!bAddRequestHeader)
		{
			InternetCloseHandle(hOpenReq);
			InternetCloseHandle(hConnect);
			InternetCloseHandle(hSession);
			return FALSE;
		}

		///发送验证数据到服务器，如果验证通过，会返回加密的sign-sap-setup-buffer字符串，用这个字符串再去生成x-apple-signature的值
		sprintf(SendData,constStr,lpSapBuf);
		sendLength = strlen(SendData);

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
			//提取从服务器上来的sign-sap-setup-buffer值
			extractSignSapSetup(hOpenReq);
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

void ManagerRegister::decodeResponseServerData(HINTERNET hOpenReq,char* lpBuffer,LPDWORD lpdwSize)
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
	lpgzipBuf =  (LPGZIP)VirtualAlloc(NULL,1024*100,MEM_COMMIT | MEM_RESERVE,PAGE_EXECUTE_READWRITE);//分配100K的虚拟内存
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

	return;
}

void ManagerRegister::parseNextStepWebData(char* lpSrcWebData)
{
	CString strSrcData;
	CString strActionHostName = TEXT("method=\"post\" novalidate=\"novalidate\" action=\"");
	CString strNextStepKey = TEXT("id=\"hiddenBottomRightButtonId\"");
	CString strMid = TEXT("name=\"");

	if(!lpSrcWebData)
	{
		return;
	}

	int dataLen = strlen(lpSrcWebData);
	TCHAR* lpwcData = new TCHAR[dataLen*2+2];
	if (!lpwcData)
	{
		return;
	}
	memset(lpwcData,0,dataLen*2+2);
	MultiByteToWideChar(CP_ACP,0,lpSrcWebData,dataLen,lpwcData,dataLen*2);

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
		return;
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
		return;
	}
	m_strAgreeRegWebActionName = strTmp.Left(endPos);

	//开始检索提取"下一步"时的name "0.0.1.1.57.1.2.1.1.0.1.11.1.5.1"
	beginPos = strSrcData.Find(strNextStepKey);
	if (beginPos < 0)
	{
		if (lpwcData)
		{
			delete []lpwcData;
			lpwcData = NULL;
		}
		return;
	}
	strTmp = strSrcData.Right(strSrcData.GetLength()-beginPos-strNextStepKey.GetLength());

	//再次提取
	beginPos = strTmp.Find(strMid);
	if (beginPos < 0)
	{
		if (lpwcData)
		{
			delete []lpwcData;
			lpwcData = NULL;
		}
		return;
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
		return;
	}
	m_strNextStepName = strTmp.Left(endPos);

	if (lpwcData)
	{
		delete []lpwcData;
		lpwcData = NULL;
	}
	return;
}


void ManagerRegister::parseRegAgreeWebData(char* lpSrcWebData)
{
	CString strSrcData;
	CString strActionHostName = TEXT("method=\"post\" novalidate=\"novalidate\" action=\"");
	CString strPageUUIDKey = TEXT("name=\"mzPageUUID\" type=\"hidden\" value='");
	CString strAgreeKey = TEXT("id=\"hiddenBottomRightButtonId\"");
	CString strMid = TEXT("name=\"");

	if(!lpSrcWebData)
	{
		return;
	}

	int dataLen = strlen(lpSrcWebData);
	TCHAR* lpwcData = new TCHAR[dataLen*2+2];
	if (!lpwcData)
	{
		return;
	}
	memset(lpwcData,0,dataLen*2+2);
	MultiByteToWideChar(CP_ACP,0,lpSrcWebData,dataLen,lpwcData,dataLen*2);

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
		return;
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
		return;
	}
	m_strRichInfoWebActionName = strTmp.Left(endPos);

	//开始检索提取"同意"时的name "0.0.1.1.57.29.2.1.1.0.1.11.1.5.1"
	beginPos = strSrcData.Find(strAgreeKey);
	if (beginPos < 0)
	{
		if (lpwcData)
		{
			delete []lpwcData;
			lpwcData = NULL;
		}
		return;
	}
	strTmp = strSrcData.Right(strSrcData.GetLength()-beginPos-strAgreeKey.GetLength());

	//再次提取
	beginPos = strTmp.Find(strMid);
	if (beginPos < 0)
	{
		if (lpwcData)
		{
			delete []lpwcData;
			lpwcData = NULL;
		}
		return;
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
		return;
	}
	m_strAgreeName = strTmp.Left(endPos);

	//提取pageUUID
	beginPos = strSrcData.Find(strPageUUIDKey);
	if (beginPos < 0)
	{
		if (lpwcData)
		{
			delete []lpwcData;
			lpwcData = NULL;
		}
		return;
	}
	strTmp = strSrcData.Right(strSrcData.GetLength()-beginPos-strPageUUIDKey.GetLength());
	endPos = strTmp.Find(TEXT("' />"));
	if (endPos < 0)
	{
		if (lpwcData)
		{
			delete []lpwcData;
			lpwcData = NULL;
		}
		return;
	}
	m_strSrcAgreeWebPageUUID = strTmp.Left(endPos);
	m_strAgreeWebPageUUID = Util::URLEncode2(strTmp.Left(endPos));

	if (lpwcData)
	{
		delete []lpwcData;
		lpwcData = NULL;
	}

	return;
}

void ManagerRegister::parseRichInfoWebData(char* lpSrcWebData)
{
	CString		strSrcData;
	CString		strTmp;
	int			beginPos = 0;
	int			endPos = 0;
	if(!lpSrcWebData)
	{
		return;
	}

	int dataLen = strlen(lpSrcWebData);
	TCHAR* lpwcData = new TCHAR[dataLen*2+2];
	if (!lpwcData)
	{
		return;
	}
	memset(lpwcData,0,dataLen*2+2);
	MultiByteToWideChar(CP_ACP,0,lpSrcWebData,dataLen,lpwcData,dataLen*2);

	strSrcData = lpwcData;

	//提取"下一步"字段的名字
	beginPos = strSrcData.Find(NEXTSTEP_TAG);
	if (beginPos < 0)
	{
		goto EXIT1;
	}
	strTmp = strSrcData.Right(strSrcData.GetLength()-beginPos-wcslen(NEXTSTEP_TAG));
	beginPos = strTmp.Find(MID_TAG);
	if (beginPos < 0)
	{
		goto EXIT1;
	}
	strTmp = strTmp.Right(strTmp.GetLength()-beginPos-wcslen(MID_TAG));
	endPos = strTmp.Find(TEXT("\" />"));
	m_regRichWebInfo.strNextStepName = strTmp.Left(endPos);
	
	//提取pageUUID的值
	beginPos = strSrcData.Find(PAGEUUID_TAG);
	if (beginPos < 0)
	{
		goto EXIT1;
	}
	strTmp = strSrcData.Right(strSrcData.GetLength()-beginPos-wcslen(PAGEUUID_TAG));
	endPos = strTmp.Find(TEXT("' />"));
	if (endPos < 0)
	{
		goto EXIT1;
	}
	m_richWebPageUUID = strTmp.Left(endPos);
	m_regRichWebInfo.strPageUUIDValue = Util::URLEncode2(strTmp.Left(endPos));

	//提取出生月的名字
	beginPos = strSrcData.Find(BIRTHMONTH_TAG);
	if (beginPos < 0)
	{
		goto EXIT1;
	}
	strTmp = strSrcData.Right(strSrcData.GetLength()-beginPos-wcslen(BIRTHMONTH_TAG));
	endPos = strTmp.Find(TEXT("\">"));
	if (endPos < 0)
	{
		goto EXIT1;
	}
	m_regRichWebInfo.strBirthMonthName = strTmp.Left(endPos);

	//提取救援电子邮件名字
	beginPos = strSrcData.Find(RESCUEMAIL_TAG);
	if (beginPos < 0)
	{
		goto EXIT1;
	}
	strTmp = strSrcData.Right(strSrcData.GetLength()-beginPos-wcslen(RESCUEMAIL_TAG));
	endPos = strTmp.Find(TEXT("\" />"));
	if (endPos < 0)
	{
		goto EXIT1;
	}
	m_regRichWebInfo.strRescueEmailName = strTmp.Left(endPos);

	//提取出生年的名字
	beginPos = strSrcData.Find(BIRTHYEAR_TAG);
	if (beginPos < 0)
	{
		goto EXIT1;
	}
	strTmp = strSrcData.Right(strSrcData.GetLength()-beginPos-wcslen(BIRTHYEAR_TAG));
	beginPos = strTmp.Find(MID_TAG);
	if (beginPos < 0)
	{
		goto EXIT1;
	}
	strTmp = strTmp.Right(strTmp.GetLength()-beginPos-wcslen(MID_TAG));
	endPos = strTmp.Find(TEXT("\" />"));
	if (endPos < 0)
	{
		goto EXIT1;
	}
	m_regRichWebInfo.strBirthYearName = strTmp.Left(endPos);
	
	//提取出生日的名字
	beginPos = strSrcData.Find(BIRTHDAY_TAG);
	if (beginPos < 0)
	{
		goto EXIT1;
	}
	strTmp = strSrcData.Right(strSrcData.GetLength()-beginPos-wcslen(BIRTHDAY_TAG));
	endPos = strTmp.Find(TEXT("\">"));
	if (endPos < 0)
	{
		goto EXIT1;
	}
	m_regRichWebInfo.strBirthDayName = strTmp.Left(endPos);

	//提取答案3的名字
	beginPos = strSrcData.Find(ANSWER3_TAG);
	if (beginPos < 0)
	{
		goto EXIT1;
	}
	strTmp = strSrcData.Right(strSrcData.GetLength()-beginPos-wcslen(ANSWER3_TAG));
	beginPos = strTmp.Find(MID_TAG);
	if (beginPos < 0)
	{
		goto EXIT1;
	}
	strTmp = strTmp.Right(strTmp.GetLength()-beginPos-wcslen(MID_TAG));
	endPos = strTmp.Find(TEXT("\" />"));
	m_regRichWebInfo.strAnswer3Name = strTmp.Left(endPos);

	//提取答案2的名字
	beginPos = strSrcData.Find(ANSWER2_TAG);
	if (beginPos < 0)
	{
		goto EXIT1;
	}
	strTmp = strSrcData.Right(strSrcData.GetLength()-beginPos-wcslen(ANSWER2_TAG));
	beginPos = strTmp.Find(MID_TAG);
	if (beginPos < 0)
	{
		goto EXIT1;
	}
	strTmp = strTmp.Right(strTmp.GetLength()-beginPos-wcslen(MID_TAG));
	endPos = strTmp.Find(TEXT("\" />"));
	m_regRichWebInfo.strAnswer2Name = strTmp.Left(endPos);

	//提取答案1的名字
	beginPos = strSrcData.Find(ANSWER1_TAG);
	if (beginPos < 0)
	{
		goto EXIT1;
	}
	strTmp = strSrcData.Right(strSrcData.GetLength()-beginPos-wcslen(ANSWER1_TAG));
	beginPos = strTmp.Find(MID_TAG);
	if (beginPos < 0)
	{
		goto EXIT1;
	}
	strTmp = strTmp.Right(strTmp.GetLength()-beginPos-wcslen(MID_TAG));
	endPos = strTmp.Find(TEXT("\" />"));
	m_regRichWebInfo.strAnswer1Name = strTmp.Left(endPos);

	//提取问题3的名字
	beginPos = strSrcData.Find(QUESTION3_TAG);
	if (beginPos < 0)
	{
		goto EXIT1;
	}
	strTmp = strSrcData.Right(strSrcData.GetLength()-beginPos-wcslen(QUESTION3_TAG));
	endPos = strTmp.Find(TEXT("\" />"));
	if (endPos < 0)
	{
		goto EXIT1;
	}
	m_regRichWebInfo.strQuestion3Name = strTmp.Left(endPos);

	//提取问题2的名字
	beginPos = strSrcData.Find(QUESTION2_TAG);
	if (beginPos < 0)
	{
		goto EXIT1;
	}
	strTmp = strSrcData.Right(strSrcData.GetLength()-beginPos-wcslen(QUESTION2_TAG));
	endPos = strTmp.Find(TEXT("\" />"));
	if (endPos < 0)
	{
		goto EXIT1;
	}
	m_regRichWebInfo.strQuestion2Name = strTmp.Left(endPos);

	//提取问题1的名字
	beginPos = strSrcData.Find(QUESTION1_TAG);
	if (beginPos < 0)
	{
		goto EXIT1;
	}
	strTmp = strSrcData.Right(strSrcData.GetLength()-beginPos-wcslen(QUESTION1_TAG));
	endPos = strTmp.Find(TEXT("\" />"));
	if (endPos < 0)
	{
		goto EXIT1;
	}
	m_regRichWebInfo.strQuestion1Name = strTmp.Left(endPos);

	//提取密码的名字
	beginPos = strSrcData.Find(PASSWORD_TAG);
	if (beginPos < 0)
	{
		goto EXIT1;
	}
	strTmp = strSrcData.Right(strSrcData.GetLength()-beginPos-wcslen(PASSWORD_TAG));
	beginPos = strTmp.Find(MID_TAG);
	if (beginPos < 0)
	{
		goto EXIT1;
	}
	strTmp = strTmp.Right(strTmp.GetLength()-beginPos-wcslen(MID_TAG));
	endPos = strTmp.Find(TEXT("\" />"));
	if (endPos < 0)
	{
		goto EXIT1;
	}
	m_regRichWebInfo.strPasswordName = strTmp.Left(endPos);

	//提取确认密码的名字
	beginPos = strSrcData.Find(VERIFYPWD_TAG);
	if (beginPos < 0)
	{
		goto EXIT1;
	}
	strTmp = strSrcData.Right(strSrcData.GetLength()-beginPos-wcslen(VERIFYPWD_TAG));
	beginPos = strTmp.Find(MID_TAG);
	if (beginPos < 0)
	{
		goto EXIT1;
	}
	strTmp = strTmp.Right(strTmp.GetLength()-beginPos-wcslen(MID_TAG));
	endPos = strTmp.Find(TEXT("\" />"));
	if (endPos < 0)
	{
		goto EXIT1;
	}
	m_regRichWebInfo.strVerifyPwdName = strTmp.Left(endPos);

	//提取appleId 的名字
	beginPos = strSrcData.Find(APPLEID_TAG);
	if (beginPos < 0)
	{
		goto EXIT1;
	}
	strTmp = strSrcData.Right(strSrcData.GetLength()-beginPos-wcslen(APPLEID_TAG));
	beginPos = strTmp.Find(MID_TAG);
	if (beginPos < 0)
	{
		goto EXIT1;
	}
	strTmp = strTmp.Right(strTmp.GetLength()-beginPos-wcslen(MID_TAG));
	endPos = strTmp.Find(TEXT("\" />"));
	if (endPos < 0)
	{
		goto EXIT1;
	}
	m_regRichWebInfo.strAppleIdName = strTmp.Left(endPos);

EXIT1:
	//释放内存
	if (lpwcData)
	{
		delete []lpwcData;
		lpwcData = NULL;
	}
	return;
}

void ManagerRegister::parseCreditWebData(char* lpSrcWebData)
{
	CString		strSrcData;
	CString		strTmp;
	int			beginPos = 0;
	int			endPos = 0;
	if(!lpSrcWebData)
	{
		return;
	}

	int dataLen = strlen(lpSrcWebData);
	TCHAR* lpwcData = new TCHAR[dataLen*2+2];
	if (!lpwcData)
	{
		return;
	}
	memset(lpwcData,0,dataLen*2+2);
	MultiByteToWideChar(CP_ACP,0,lpSrcWebData,dataLen,lpwcData,dataLen*2);

	strSrcData = lpwcData;

	//提取输入代码的名字
	beginPos = strSrcData.Find(CODEREDEMPT_TAG);
	if (beginPos < 0)
	{
		goto EXIT1;
	}
	strTmp = strSrcData.Right(strSrcData.GetLength()-beginPos-wcslen(CODEREDEMPT_TAG));
	beginPos = strTmp.Find(MID_TAG);
	if (beginPos < 0)
	{
		goto EXIT1;
	}
	strTmp = strTmp.Right(strTmp.GetLength()-beginPos-wcslen(MID_TAG));
	endPos = strTmp.Find(TEXT("\" />"));
	if (endPos < 0)
	{
		goto EXIT1;
	}
	m_regCreditWebInfo.strCoderedemptName = strTmp.Left(endPos);

	//获取邮政编码的名字
	beginPos = strSrcData.Find(POSTALCODE_TAG);
	if (beginPos < 0)
	{
		goto EXIT1;
	}
	strTmp = strSrcData.Right(strSrcData.GetLength()-beginPos-wcslen(POSTALCODE_TAG));
	beginPos = strTmp.Find(MID_TAG);
	if (beginPos < 0)
	{
		goto EXIT1;
	}
	strTmp = strTmp.Right(strTmp.GetLength()-beginPos-wcslen(MID_TAG));
	endPos = strTmp.Find(TEXT("\" />"));
	if (endPos < 0)
	{
		goto EXIT1;
	}
	m_regCreditWebInfo.strPostalCodeName = strTmp.Left(endPos);

	//提取"下一步"字段的名字
	beginPos = strSrcData.Find(NEXTSTEP_TAG);
	if (beginPos < 0)
	{
		goto EXIT1;
	}
	strTmp = strSrcData.Right(strSrcData.GetLength()-beginPos-wcslen(NEXTSTEP_TAG));
	beginPos = strTmp.Find(MID_TAG);
	if (beginPos < 0)
	{
		goto EXIT1;
	}
	strTmp = strTmp.Right(strTmp.GetLength()-beginPos-wcslen(MID_TAG));
	endPos = strTmp.Find(TEXT("\" />"));
	if (endPos < 0)
	{
		goto EXIT1;
	}
	m_regCreditWebInfo.strNextStepName = strTmp.Left(endPos);

	//提取“市级行政区”名字
	beginPos = strSrcData.Find(CITY_TAG);
	if (beginPos < 0)
	{
		goto EXIT1;
	}
	strTmp = strSrcData.Right(strSrcData.GetLength()-beginPos-wcslen(CITY_TAG));
	beginPos = strTmp.Find(MID_TAG);
	if (beginPos < 0)
	{
		goto EXIT1;
	}
	strTmp = strTmp.Right(strTmp.GetLength()-beginPos-wcslen(MID_TAG));
	endPos = strTmp.Find(TEXT("\" />"));
	if (endPos < 0)
	{
		goto EXIT1;
	}
	m_regCreditWebInfo.strNextStepName = strTmp.Left(endPos);

	//提取"名" 字段名字
	beginPos = strSrcData.Find(FIRSTNAME_TAG);
	if (beginPos < 0)
	{
		goto EXIT1;
	}
	strTmp = strSrcData.Right(strSrcData.GetLength()-beginPos-wcslen(FIRSTNAME_TAG));
	beginPos = strTmp.Find(MID_TAG);
	if (beginPos < 0)
	{
		goto EXIT1;
	}
	strTmp = strTmp.Right(strTmp.GetLength()-beginPos-wcslen(MID_TAG));
	endPos = strTmp.Find(TEXT("\" />"));
	if (endPos < 0)
	{
		goto EXIT1;
	}
	m_regCreditWebInfo.strFirstName = strTmp.Left(endPos);

	//提取"姓"字段的名字
	beginPos = strSrcData.Find(LASTNAME_TAG);
	if (beginPos < 0)
	{
		goto EXIT1;
	}
	strTmp = strSrcData.Right(strSrcData.GetLength()-beginPos-wcslen(LASTNAME_TAG));
	beginPos = strTmp.Find(MID_TAG);
	if (beginPos < 0)
	{
		goto EXIT1;
	}
	strTmp = strTmp.Right(strTmp.GetLength()-beginPos-wcslen(MID_TAG));
	endPos = strTmp.Find(TEXT("\" />"));
	if (endPos < 0)
	{
		goto EXIT1;
	}
	m_regCreditWebInfo.strLastName = strTmp.Left(endPos);
	
	//提取"街1"字段的名字
	beginPos = strSrcData.Find(STREET1_TAG);
	if (beginPos < 0)
	{
		goto EXIT1;
	}
	strTmp = strSrcData.Right(strSrcData.GetLength()-beginPos-wcslen(STREET1_TAG));
	beginPos = strTmp.Find(MID_TAG);
	if (beginPos < 0)
	{
		goto EXIT1;
	}
	strTmp = strTmp.Right(strTmp.GetLength()-beginPos-wcslen(MID_TAG));
	endPos = strTmp.Find(TEXT("\" />"));
	if (endPos < 0)
	{
		goto EXIT1;
	}
	m_regCreditWebInfo.strStreet1Name = strTmp.Left(endPos);

	//提取"街2"字段的名字
	beginPos = strSrcData.Find(STREET2_TAG);
	if (beginPos < 0)
	{
		goto EXIT1;
	}
	strTmp = strSrcData.Right(strSrcData.GetLength()-beginPos-wcslen(STREET2_TAG));
	beginPos = strTmp.Find(MID_TAG);
	if (beginPos < 0)
	{
		goto EXIT1;
	}
	strTmp = strTmp.Right(strTmp.GetLength()-beginPos-wcslen(MID_TAG));
	endPos = strTmp.Find(TEXT("\" />"));
	if (endPos < 0)
	{
		goto EXIT1;
	}
	m_regCreditWebInfo.strStreet2Name = strTmp.Left(endPos);
	
	//提取"街3"字段的名字
	beginPos = strSrcData.Find(STREET3_TAG);
	if (beginPos < 0)
	{
		goto EXIT1;
	}
	strTmp = strSrcData.Right(strSrcData.GetLength()-beginPos-wcslen(STREET3_TAG));
	beginPos = strTmp.Find(MID_TAG);
	if (beginPos < 0)
	{
		goto EXIT1;
	}
	strTmp = strTmp.Right(strTmp.GetLength()-beginPos-wcslen(MID_TAG));
	endPos = strTmp.Find(TEXT("\" />"));
	if (endPos < 0)
	{
		goto EXIT1;
	}
	m_regCreditWebInfo.strStreet3Name = strTmp.Left(endPos);

	//提取"ndpd_vk"字段的值
	beginPos = strSrcData.Find(NDPD_VK_TAG);
	if (beginPos < 0)
	{
		goto EXIT1;
	}
	strTmp = strSrcData.Right(strSrcData.GetLength()-beginPos-wcslen(NDPD_VK_TAG));
	endPos = strTmp.Find(TEXT("\">"));
	if (endPos < 0)
	{
		goto EXIT1;
	}
	m_regCreditWebInfo.strNdpd_vk_Value = strTmp.Left(endPos);

	//提取"ndpd_w"字段的值
	beginPos = strSrcData.Find(NDPD_W_TAG);
	if (beginPos < 0)
	{
		goto EXIT1;
	}
	strTmp = strSrcData.Right(strSrcData.GetLength()-beginPos-wcslen(NDPD_W_TAG));
	endPos = strTmp.Find(TEXT("\">"));
	if (endPos < 0)
	{
		goto EXIT1;
	}
	m_regCreditWebInfo.strNdpd_w_Value = Util::URLEncode2(strTmp.Left(endPos));

	//提取"ndpd-s"字段的值
	beginPos = strSrcData.Find(NDPD_S_TAG);
	if (beginPos < 0)
	{
		goto EXIT1;
	}
	strTmp = strSrcData.Right(strSrcData.GetLength()-beginPos-wcslen(NDPD_S_TAG));
	endPos = strTmp.Find(TEXT("\">"));
	if (endPos < 0)
	{
		goto EXIT1;
	}
	m_regCreditWebInfo.strNdpd_s_Value = Util::URLEncode2(strTmp.Left(endPos));

	//提取"ndpd-f"字段的值
	beginPos = strSrcData.Find(NDPD_F_TAG);
	if (beginPos < 0)
	{
		goto EXIT1;
	}
	strTmp = strSrcData.Right(strSrcData.GetLength()-beginPos-wcslen(NDPD_F_TAG));
	endPos = strTmp.Find(TEXT("\">"));
	if (endPos < 0)
	{
		goto EXIT1;
	}
	m_regCreditWebInfo.strNdpd_f_Value = Util::URLEncode2(strTmp.Left(endPos));

	//提取"ndpd-fm"字段的值
	beginPos = strSrcData.Find(NDPD_FM_TAG);
	if (beginPos < 0)
	{
		goto EXIT1;
	}
	strTmp = strSrcData.Right(strSrcData.GetLength()-beginPos-wcslen(NDPD_FM_TAG));
	endPos = strTmp.Find(TEXT("\">"));
	if (endPos < 0)
	{
		goto EXIT1;
	}
	m_regCreditWebInfo.strNdpd_fm_Value = Util::URLEncode2(strTmp.Left(endPos));

	//获取phonenumber的名字
	beginPos = strSrcData.Find(PHONENUMBER_TAG);
	if (beginPos < 0)
	{
		goto EXIT1;
	}
	strTmp = strSrcData.Right(strSrcData.GetLength()-beginPos-wcslen(PHONENUMBER_TAG));
	beginPos = strTmp.Find(MID_TAG);
	if (beginPos < 0)
	{
		goto EXIT1;
	}
	strTmp = strTmp.Right(strTmp.GetLength()-beginPos-wcslen(MID_TAG));
	endPos = strTmp.Find(TEXT("\" />"));
	if (endPos < 0)
	{
		goto EXIT1;
	}
	m_regCreditWebInfo.strPhoneNumberName = strTmp.Left(endPos);

	//提取"cc_expr_year"的值
	beginPos = strSrcData.Find(EXPRYEAR_TAG);
	if (beginPos < 0)
	{
		goto EXIT1;
	}
	strTmp = strSrcData.Right(strSrcData.GetLength()-beginPos-wcslen(EXPRYEAR_TAG));
	endPos = strTmp.Find(TEXT("\">"));
	if (endPos < 0)
	{
		goto EXIT1;
	}
	m_regCreditWebInfo.strCCExprYearValue = strTmp.Left(endPos);

	//提取"mzPageUUID"的值
	beginPos = strSrcData.Find(PAGEUUID_TAG);
	if (beginPos < 0)
	{
		goto EXIT1;
	}
	strTmp = strSrcData.Right(strSrcData.GetLength()-beginPos-wcslen(PAGEUUID_TAG));
	endPos = strTmp.Find(TEXT("' />"));
	if (endPos < 0)
	{
		goto EXIT1;
	}
	m_creditWebPageUUID = strTmp.Left(endPos);
	m_regCreditWebInfo.strPageUUIDValue = Util::URLEncode2(strTmp.Left(endPos));

EXIT1:
	//释放内存
	if (lpwcData)
	{
		delete []lpwcData;
		lpwcData = NULL;
	}
	return;
}

BOOL ManagerRegister::sendSignupWizard(CString strUrl,CString strVerb,CString strUserAgent,CString strHostName,char* pXAppleSignature,int SignaureLength)
{
	TCHAR			strSignature[2048] = {0};
	char*			lpDecodeData = NULL;
	DWORD			dwSize = 0;

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

	BOOL bCrackUrl = InternetCrackUrl(strUrl,0,0,&uc);
	if (bCrackUrl)
	{
		if (m_netType == PROXY_NETWORK)
		{
			hSession = InternetOpen(strUserAgent,INTERNET_OPEN_TYPE_PROXY,m_strProxy,0,0);
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

		hOpenReq = HttpOpenRequest(hConnect,strVerb,strHostName,0,0,0,0x84A01000,0); //0x84A01000是一个常数
		if (!hOpenReq)
		{
			InternetCloseHandle(hConnect);
			InternetCloseHandle(hSession);
			return FALSE;
		}

		MultiByteToWideChar(CP_ACP,NULL,pXAppleSignature,SignaureLength,strSignature,2048);

		CString strHeaders = _T("User-Agent: ");
		strHeaders += strUserAgent;
		strHeaders += _T("\r\nAccept: text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8");
		strHeaders += _T("\r\nContent-Type: application/x-apple-plist");
		strHeaders += _T("\r\nAccept-Language: zh-Hans");
		strHeaders += _T("\r\nX-Apple-Store-Front: 143465-19,29 t:native");
		strHeaders += _T("\r\nX-Apple-Client-Versions: iBooks/4.3; GameCenter/2.0; Podcasts/2.2.1");
		strHeaders += _T("\r\nX-Apple-Partner: origin.0");
		strHeaders += _T("\r\nX-Apple-Connection-Type: WiFi");
		strHeaders += _T("\r\nX-Apple-ActionSignature: ");
		strHeaders += strSignature;
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
			m_strRefererUrl = location;
			int Index = tmpBuf.Find(_T(".com"));
			strRedirectUrl = tmpBuf.Mid(0,Index+4);
			strRedirectHostName = tmpBuf.Mid(Index+4);
			//重定向后再次发送
			sendSignupWizard(strRedirectUrl,strVerb,strUserAgent,strRedirectHostName,pXAppleSignature,SignaureLength);


		}
		else if (dwQueryBuf == 200)//下面是数据接收处理部分
		{
			m_strRefererUrl = strUrl + strHostName;
			extractResponseXAppleActionsignature(hOpenReq);
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
			decodeResponseServerData(hOpenReq,lpDecodeData,&dwSize);
			//解析数据
			parseNextStepWebData(lpDecodeData);

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

BOOL ManagerRegister::sendGetAgreeWebPage(CString strUrl,CString strRefererUrl,CString strVerb,CString strUserAgent,CString strAgreeWebActionName,CString strNextStepName)
{
	char			szNextStepName[64] = {0};
	char			szSendData[MAX_PATH] = {0};
	char*			lpDecodeData = NULL;
	DWORD			dataLen = 0;
	DWORD			dwSize = 0;

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

	WideCharToMultiByte(CP_ACP,0,strNextStepName.GetBuffer(),strNextStepName.GetLength(),szNextStepName,64,NULL,NULL);

	BOOL bCrackUrl = InternetCrackUrl(strUrl,0,0,&uc);
	if (bCrackUrl)
	{
		if (m_netType == PROXY_NETWORK)
		{
			hSession = InternetOpen(strUserAgent,INTERNET_OPEN_TYPE_PROXY,m_strProxy,0,0);
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

		hOpenReq = HttpOpenRequest(hConnect,strVerb,strAgreeWebActionName,0,0,0,0x84A01000,0); //0x84A01000是一个常数
		if (!hOpenReq)
		{
			InternetCloseHandle(hConnect);
			InternetCloseHandle(hSession);
			return FALSE;
		}

		CString strHeaders = _T("User-Agent: ");
		strHeaders += strUserAgent;
		strHeaders += _T("\r\nAccept: text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8");
		strHeaders += _T("\r\nContent-Type: application/x-apple-plist");
		strHeaders += _T("\r\nReferer: ");
		strHeaders += strRefererUrl;
		strHeaders += _T("\r\nAccept-Language: zh-cn");
		strHeaders += _T("\r\nX-Apple-Store-Front: 143465-19,29 t:native");
		strHeaders += _T("\r\nX-Apple-Client-Versions: iBooks/4.3; GameCenter/2.0; Podcasts/2.2.1");
		strHeaders += _T("\r\nX-Apple-Partner: origin.0");
		strHeaders += _T("\r\nX-Apple-Connection-Type: WiFi");
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
		sprintf(szSendData,"storeFront=143465&%s=%E4%B8%8B%E4%B8%80%E6%AD%A5",szNextStepName);
		dataLen = strlen(szSendData);
	
		bSendRequest = HttpSendRequest(hOpenReq,NULL,-1,szSendData,dataLen);
		if (!bSendRequest && GetLastError() == ERROR_INTERNET_INVALID_CA)
		{
			DWORD dwFlags;
			DWORD dwBuffLen = sizeof(dwFlags);
			InternetQueryOption (hOpenReq, INTERNET_OPTION_SECURITY_FLAGS,(LPVOID)&dwFlags, &dwBuffLen);
			dwFlags |= SECURITY_FLAG_IGNORE_UNKNOWN_CA;
			InternetSetOption (hOpenReq,INTERNET_OPTION_SECURITY_FLAGS,&dwFlags,sizeof (dwFlags) );

			//再次发送
			bSendRequest = HttpSendRequest(hOpenReq,NULL,-1,szSendData,dataLen);
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
			m_strRefererUrl = location;
			int Index = tmpBuf.Find(_T(".com"));
			strRedirectUrl = tmpBuf.Mid(0,Index+4);
			strRedirectHostName = tmpBuf.Mid(Index+4);
			//重定向后再次发送
			sendGetAgreeWebPage(strRedirectUrl,strRefererUrl,strVerb,strUserAgent,strRedirectHostName,strNextStepName);


		}
		else if (dwQueryBuf == 200)//下面是数据接收处理部分
		{
			m_strRefererUrl = strUrl + strAgreeWebActionName;
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
			decodeResponseServerData(hOpenReq,lpDecodeData,&dwSize);
			//解析数据
			parseRegAgreeWebData(lpDecodeData);

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

BOOL ManagerRegister::sendGetRichInfoWebPage(CString strUrl,
											 CString strRefererUrl,
											 CString strVerb,
											 CString strUserAgent,
											 CString strRichInfoWebActionName,
											 char* lpSendData,
											 int dataLen)
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

	BOOL bCrackUrl = InternetCrackUrl(strUrl,0,0,&uc);
	if (bCrackUrl)
	{
		if (m_netType == PROXY_NETWORK)
		{
			hSession = InternetOpen(strUserAgent,INTERNET_OPEN_TYPE_PROXY,m_strProxy,0,0);
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

		hOpenReq = HttpOpenRequest(hConnect,strVerb,strRichInfoWebActionName,0,0,0,0x84A01000,0); //0x84A01000是一个常数
		if (!hOpenReq)
		{
			InternetCloseHandle(hConnect);
			InternetCloseHandle(hSession);
			return FALSE;
		}

		CString strHeaders = _T("User-Agent: ");
		strHeaders += strUserAgent;
		strHeaders += _T("\r\nAccept: text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8");
		strHeaders += _T("\r\nContent-Type: application/x-apple-plist");
		strHeaders += _T("\r\nReferer: ");
		strHeaders += strRefererUrl;
		strHeaders += _T("\r\nAccept-Language: zh-cn");
		strHeaders += _T("\r\nX-Apple-Store-Front: 143465-19,29 t:native");
		strHeaders += _T("\r\nX-Apple-Client-Versions: iBooks/4.3; GameCenter/2.0; Podcasts/2.2.1");
		strHeaders += _T("\r\nX-Apple-Partner: origin.0");
		strHeaders += _T("\r\nX-Apple-Connection-Type: WiFi");
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
		bSendRequest = HttpSendRequest(hOpenReq,NULL,-1,lpSendData,dataLen);
		if (!bSendRequest && GetLastError() == ERROR_INTERNET_INVALID_CA)
		{
			DWORD dwFlags;
			DWORD dwBuffLen = sizeof(dwFlags);
			InternetQueryOption (hOpenReq, INTERNET_OPTION_SECURITY_FLAGS,(LPVOID)&dwFlags, &dwBuffLen);
			dwFlags |= SECURITY_FLAG_IGNORE_UNKNOWN_CA;
			InternetSetOption (hOpenReq,INTERNET_OPTION_SECURITY_FLAGS,&dwFlags,sizeof (dwFlags) );

			//再次发送
			bSendRequest = HttpSendRequest(hOpenReq,NULL,-1,lpSendData,dataLen);
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
			m_strRefererUrl = location;
			int Index = tmpBuf.Find(_T(".com"));
			strRedirectUrl = tmpBuf.Mid(0,Index+4);
			strRedirectHostName = tmpBuf.Mid(Index+4);
			//重定向后再次发送
			sendGetRichInfoWebPage(strRedirectUrl,strRefererUrl,strVerb,strUserAgent,strRedirectHostName,lpSendData,dataLen);


		}
		else if (dwQueryBuf == 200)//下面是数据接收处理部分
		{
			m_strRefererUrl = strUrl + strRichInfoWebActionName;
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
			decodeResponseServerData(hOpenReq,lpDecodeData,&dwSize);
			//解析数据
			parseRichInfoWebData(lpDecodeData);

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

BOOL ManagerRegister::sendGetCreditInfoWebPage(CString strUrl,
											   CString strRefererUrl,
											   CString strVerb,
											   CString strUserAgent,
											   CString strCreditWebActionName,
											   char* lpSendData,
											   int dataLen)
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

	BOOL bCrackUrl = InternetCrackUrl(strUrl,0,0,&uc);
	if (bCrackUrl)
	{
		if (m_netType == PROXY_NETWORK)
		{
			hSession = InternetOpen(strUserAgent,INTERNET_OPEN_TYPE_PROXY,m_strProxy,0,0);
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

		hOpenReq = HttpOpenRequest(hConnect,strVerb,strCreditWebActionName,0,0,0,0x84A01000,0); //0x84A01000是一个常数
		if (!hOpenReq)
		{
			InternetCloseHandle(hConnect);
			InternetCloseHandle(hSession);
			return FALSE;
		}

		CString strHeaders = _T("User-Agent: ");
		strHeaders += strUserAgent;
		strHeaders += _T("\r\nAccept: text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8");
		strHeaders += _T("\r\nContent-Type: application/x-apple-plist");
		strHeaders += _T("\r\nReferer: ");
		strHeaders += strRefererUrl;
		strHeaders += _T("\r\nAccept-Language: zh-cn");
		strHeaders += _T("\r\nX-Apple-Store-Front: 143465-19,29 t:native");
		strHeaders += _T("\r\nX-Apple-Client-Versions: iBooks/4.3; GameCenter/2.0; Podcasts/2.2.1");
		strHeaders += _T("\r\nX-Apple-Partner: origin.0");
		strHeaders += _T("\r\nX-Apple-Connection-Type: WiFi");
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
		bSendRequest = HttpSendRequest(hOpenReq,NULL,-1,lpSendData,dataLen);
		if (!bSendRequest && GetLastError() == ERROR_INTERNET_INVALID_CA)
		{
			DWORD dwFlags;
			DWORD dwBuffLen = sizeof(dwFlags);
			InternetQueryOption (hOpenReq, INTERNET_OPTION_SECURITY_FLAGS,(LPVOID)&dwFlags, &dwBuffLen);
			dwFlags |= SECURITY_FLAG_IGNORE_UNKNOWN_CA;
			InternetSetOption (hOpenReq,INTERNET_OPTION_SECURITY_FLAGS,&dwFlags,sizeof (dwFlags) );

			//再次发送
			bSendRequest = HttpSendRequest(hOpenReq,NULL,-1,lpSendData,dataLen);
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
			m_strRefererUrl = location;
			int Index = tmpBuf.Find(_T(".com"));
			strRedirectUrl = tmpBuf.Mid(0,Index+4);
			strRedirectHostName = tmpBuf.Mid(Index+4);
			//重定向后再次发送
			sendGetCreditInfoWebPage(strRedirectUrl,strRefererUrl,strVerb,strUserAgent,strRedirectHostName,lpSendData,dataLen);


		}
		else if (dwQueryBuf == 200)//下面是数据接收处理部分
		{
			m_strRefererUrl = strUrl + strCreditWebActionName;
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
			decodeResponseServerData(hOpenReq,lpDecodeData,&dwSize);
			//解析数据
			parseCreditWebData(lpDecodeData);

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
BOOL ManagerRegister::sendSumbitCreditInfo(CString strUrl,
										   CString strRefererUrl,
										   CString strVerb,
										   CString strUserAgent,
										   CString strSubmitCreditInfoWebActionName,
										   char* lpSendData,
										   int dataLen)
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

	BOOL bCrackUrl = InternetCrackUrl(strUrl,0,0,&uc);
	if (bCrackUrl)
	{
		if (m_netType == PROXY_NETWORK)
		{
			hSession = InternetOpen(strUserAgent,INTERNET_OPEN_TYPE_PROXY,m_strProxy,0,0);
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

		hOpenReq = HttpOpenRequest(hConnect,strVerb,strSubmitCreditInfoWebActionName,0,0,0,0x84A01000,0); //0x84A01000是一个常数
		if (!hOpenReq)
		{
			InternetCloseHandle(hConnect);
			InternetCloseHandle(hSession);
			return FALSE;
		}

		CString strHeaders = _T("User-Agent: ");
		strHeaders += strUserAgent;
		strHeaders += _T("\r\nAccept: text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8");
		strHeaders += _T("\r\nContent-Type: application/x-apple-plist");
		strHeaders += _T("\r\nReferer: ");
		strHeaders += strRefererUrl;
		strHeaders += _T("\r\nAccept-Language: zh-cn");
		strHeaders += _T("\r\nX-Apple-Store-Front: 143465-19,29 t:native");
		strHeaders += _T("\r\nX-Apple-Client-Versions: iBooks/4.3; GameCenter/2.0; Podcasts/2.2.1");
		strHeaders += _T("\r\nX-Apple-Partner: origin.0");
		strHeaders += _T("\r\nX-Apple-Connection-Type: WiFi");
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
		bSendRequest = HttpSendRequest(hOpenReq,NULL,-1,lpSendData,dataLen);
		if (!bSendRequest && GetLastError() == ERROR_INTERNET_INVALID_CA)
		{
			DWORD dwFlags;
			DWORD dwBuffLen = sizeof(dwFlags);
			InternetQueryOption (hOpenReq, INTERNET_OPTION_SECURITY_FLAGS,(LPVOID)&dwFlags, &dwBuffLen);
			dwFlags |= SECURITY_FLAG_IGNORE_UNKNOWN_CA;
			InternetSetOption (hOpenReq,INTERNET_OPTION_SECURITY_FLAGS,&dwFlags,sizeof (dwFlags) );

			//再次发送
			bSendRequest = HttpSendRequest(hOpenReq,NULL,-1,lpSendData,dataLen);
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
			m_strRefererUrl = location;
			int Index = tmpBuf.Find(_T(".com"));
			strRedirectUrl = tmpBuf.Mid(0,Index+4);
			strRedirectHostName = tmpBuf.Mid(Index+4);
			//重定向后再次发送
			sendSumbitCreditInfo(strRedirectUrl,strRefererUrl,strVerb,strUserAgent,strRedirectHostName,lpSendData,dataLen);


		}
		else if (dwQueryBuf == 200)//下面是数据接收处理部分
		{
			m_strRefererUrl = strUrl + strSubmitCreditInfoWebActionName;
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
			decodeResponseServerData(hOpenReq,lpDecodeData,&dwSize);
			//解析数据
			
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
