#include "stdafx.h"
#include "HttpRequest.h"
#include <sys/stat.h>
#include <io.h>

size_t NetCallback( void *ptr, size_t size, size_t nmemb, void *stream )
{
	int len = size * nmemb;  
	int written = len;  
	string* sCxt = (string *)stream;
	if ( sCxt )
	{
		sCxt->append((const char*)ptr, len);
	}

	return written;
}

size_t HeaderCallback( void *ptr, size_t size, size_t nmemb, void *stream)
{
	int len = size * nmemb;  
	int written = len;  
	string* sCxt = (string *)stream;
	if ( sCxt )
	{
		sCxt->append((const char*)ptr, len);
	}

	return written;
}

size_t DownFileCallback(void *buffer, size_t size, size_t nmemb, void *user_data)
{
	if ( NULL == user_data )
	{
		return 0;
	}

	FILE* f = (FILE*)user_data;
	int nWrite = fwrite(buffer, size, nmemb, f);
	fflush(f);

	return nWrite;
}

int ProgressCallback(void *clientp,double dltotal,double dlnow,double ultotal,double ulnow)
{
	if ( dltotal > -0.1 && dltotal < 0.1 )  
	{
		return 0;  
	}
	int nPos = (int) ( (dlnow/dltotal)*100 );
	printf("\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\bDownloading:%03d%%", nPos);

	return 0;
}

HttpRequest::HttpRequest(void):
m_Curl(NULL),
	m_Headerlist(NULL),
	m_dwNetCode(200),
	m_nTimeout(60)
{
	memset(m_pErrorBuff, 0, CURL_ERROR_SIZE);
}

HttpRequest::~HttpRequest(void)
{
}

BOOL HttpRequest::Init( LPCTSTR str_SessionName )
{
	//curl_global_init(CURL_GLOBAL_DEFAULT);
	m_Curl = curl_easy_init();
	//curl_easy_setopt(m_Curl, CURLOPT_PROXY, "192.168.81.35:8888");
	return true;
}

void HttpRequest::SetHttpProxy(std::string p, std::string auth)
{
	if (p.length()>0)
	{
		curl_easy_setopt(m_Curl, CURLOPT_PROXY, p.c_str());
	}

	if (auth.length()>0)
	{
		curl_easy_setopt(m_Curl, CURLOPT_PROXYUSERPWD, auth.c_str());
	}
	//curl_easy_setopt(m_Curl, CURLOPT_PROXYTYPE, CURLPROXY_SOCKS5);
	//curl_easy_setopt(m_Curl, CURLOPT_HTTPPROXYTUNNEL, 1L);
}

void HttpRequest::AddHeader( CString key, CString value )
{
	key.AppendFormat(_T(": %s"), value);
	string sHeader = CT2A(key);
	m_Headerlist = curl_slist_append(m_Headerlist, sHeader.c_str());
}

CString HttpRequest::GetHeadersAsString()
{
	CString strHeader = CA2T(m_sHeader.c_str());
	return strHeader;
}
	

DWORD HttpRequest::GetContentLength()
{
	return m_sResult.length();
}

BOOL HttpRequest::DownloadToFile( LPCTSTR strUrl, LPCTSTR strPath ,void *AppleThis)
{
	string sUrl = CT2A(strUrl);
	m_sUrl = sUrl;

	FILE* pFile = _tfopen(strPath, _T("wb+"));
	if ( NULL == pFile )
	{
		return FALSE;
	}
	curl_easy_setopt(m_Curl, CURLOPT_HTTPHEADER, m_Headerlist);
	curl_easy_setopt(m_Curl, CURLOPT_HTTPGET,1);
	curl_easy_setopt(m_Curl, CURLOPT_URL, sUrl.c_str());
	curl_easy_setopt(m_Curl, CURLOPT_WRITEFUNCTION, DownFileCallback);
	curl_easy_setopt(m_Curl, CURLOPT_WRITEDATA, pFile);
	curl_easy_setopt(m_Curl, CURLOPT_NOPROGRESS, true);//设为false 下面才能设置进度响应函数
	curl_easy_setopt(m_Curl, CURLOPT_PROGRESSFUNCTION, ProgressCallback);
	curl_easy_setopt(m_Curl, CURLOPT_PROGRESSDATA, AppleThis);
	curl_easy_setopt(m_Curl, CURLOPT_FOLLOWLOCATION, 1);
	curl_easy_setopt(m_Curl, CURLOPT_TIMEOUT, 9999); //设置连接超时时间
	curl_easy_setopt(m_Curl, CURLOPT_ERRORBUFFER, m_pErrorBuff);

	CURLcode res = curl_easy_perform(m_Curl);

	fclose( pFile );

	if ( NULL != m_Curl )
	{
		curl_easy_cleanup(m_Curl);
	}

	curl_global_cleanup();

	if ( res != CURLE_OK )
	{
		return FALSE;
	}
	printf("\n");
	return TRUE;
}


BOOL HttpRequest::RecvContentToFile( LPCTSTR strUrl, LPCTSTR strPath )
{
	string sUrl = CT2A(strUrl);
	m_sUrl = sUrl;

	FILE* pFile = _tfopen(strPath, _T("wb+"));
	if ( NULL == pFile )
	{
		return FALSE;
	}

	curl_easy_setopt(m_Curl, CURLOPT_URL, sUrl.c_str());
	curl_easy_setopt(m_Curl, CURLOPT_WRITEFUNCTION, DownFileCallback);
	curl_easy_setopt(m_Curl, CURLOPT_WRITEDATA, pFile);
	curl_easy_setopt(m_Curl, CURLOPT_FOLLOWLOCATION, 1);
	curl_easy_setopt(m_Curl, CURLOPT_TIMEOUT, 60); //设置连接超时时间
	curl_easy_setopt(m_Curl, CURLOPT_ERRORBUFFER, m_pErrorBuff);

	CURLcode res = curl_easy_perform(m_Curl);

	fclose( pFile );

	if ( NULL != m_Curl )
	{
		curl_easy_cleanup(m_Curl);
	}

	curl_global_cleanup();

	if ( res != CURLE_OK )
	{
		return FALSE;
	}

	return TRUE;
}

BOOL HttpRequest::RecvContent( CStringA & strResult )
{
	BOOL bSuccess = FALSE;

	__try
	{
		int nTry = 0;
Try:
		printf("RecvContent\n");
		CURLcode res = curl_easy_perform(m_Curl);
		printf("curl_easy_perform ret=%d\n",res);
		if ( CURLE_OK != res )
		{
			m_dwNetCode = 909;
			curl_easy_strerror(res);
			m_sResult.clear();
			m_sHeader.clear();

			//if ( ++nTry < 1 )
			//{
			//	goto Try;
			//}

		}
		else
		{
			char * pp = strResult.GetBuffer(m_sResult.length());
			::memcpy(pp, m_sResult.c_str(), m_sResult.size());
			strResult.ReleaseBuffer(m_sResult.length());

			DWORD dwCode = 0;
			CURLcode res;
			res = curl_easy_getinfo(m_Curl, CURLINFO_RESPONSE_CODE, &dwCode);
			if ( CURLE_OK == res )
			{
				m_dwNetCode = dwCode;
			}

			DWORD dwCount = 0;
			curl_easy_getinfo(m_Curl, CURLINFO_REDIRECT_COUNT, &dwCount);

			char redirectUrl[2048] = {0};
			if ( dwCount > 0 )
			{
				int nLen = sizeof("location:");
				size_t nPos = m_sHeader.find("location:");
				if ( string::npos != nPos )
				{
					nPos += nLen;
					int n = 0;
					char ch = m_sHeader.at(nPos++);
					while ( ch != '\n' )
					{
						redirectUrl[n++] = ch;
						ch = m_sHeader.at(nPos++);
					}

					m_sRedirectUrl = redirectUrl;
				}
			}

			if ( NULL != m_Headerlist )
			{
				curl_slist_free_all(m_Headerlist);
				m_Headerlist = NULL;
			}

			bSuccess = TRUE;
		}

		curl_easy_cleanup(m_Curl);
	}
	__except(1)
	{

	}
	return bSuccess;
}

DWORD WINAPI HttpRequest::Easy_Perform_Multi(void *p)
{
	CURL *m_Curl = (CURL*)p;
	return curl_easy_perform(m_Curl);
}

BOOL HttpRequest::RecvContent_Multi( CStringA & strResult )
{
	BOOL bSuccess = FALSE;

	__try
	{
		int nTry = 0;
Try:
		printf("RecvContent_Multi\n");
		CURLcode res = CURLE_FAILED_INIT;

		DWORD dwRet;
		HANDLE perform_handle = CreateThread(NULL, 0, Easy_Perform_Multi, m_Curl, NULL, NULL);

		dwRet = WaitForSingleObject(perform_handle, m_nTimeout * 1000);

		if (dwRet == WAIT_OBJECT_0)
		{
			DWORD dwExitCode;
			bool bl = GetExitCodeThread(perform_handle, &dwExitCode);
			res = (CURLcode)dwExitCode;
			printf("RecvContent_Multi finish ret = %d\n",res);
		}
		else if (dwRet == WAIT_TIMEOUT)
		{
			bool bl = TerminateThread(perform_handle, 0);
			res = CURLE_FTP_ACCEPT_TIMEOUT;
			printf("RecvContent_Multi finish timeout\n");
		}
		else if (dwRet == WAIT_FAILED)
		{

		}
		CloseHandle(perform_handle);

			
		if ( CURLE_OK != res )
		{
			m_dwNetCode = 909;
			curl_easy_strerror(res);
			m_sResult.clear();
			m_sHeader.clear();

			//if ( ++nTry < 1 )
			//{
			//	goto Try;
			//}

		}
		else
		{
			char * pp = strResult.GetBuffer(m_sResult.length());
			::memcpy(pp, m_sResult.c_str(), m_sResult.size());
			strResult.ReleaseBuffer(m_sResult.length());

			DWORD dwCode = 0;
			CURLcode res;
			res = curl_easy_getinfo(m_Curl, CURLINFO_RESPONSE_CODE, &dwCode);
			if ( CURLE_OK == res )
			{
				m_dwNetCode = dwCode;
			}

			DWORD dwCount = 0;
			curl_easy_getinfo(m_Curl, CURLINFO_REDIRECT_COUNT, &dwCount);

			char redirectUrl[2048] = {0};
			if ( dwCount > 0 )
			{
				int nLen = sizeof("location:");
				size_t nPos = m_sHeader.find("location:");
				if ( string::npos != nPos )
				{
					nPos += nLen;
					int n = 0;
					char ch = m_sHeader.at(nPos++);
					while ( ch != '\n' )
					{
						redirectUrl[n++] = ch;
						ch = m_sHeader.at(nPos++);
					}

					m_sRedirectUrl = redirectUrl;
				}
			}

			if ( NULL != m_Headerlist )
			{
				curl_slist_free_all(m_Headerlist);
				m_Headerlist = NULL;
			}

			bSuccess = TRUE;
		}

		curl_easy_cleanup(m_Curl);
	}
	__except(1)
	{

	}
	return bSuccess;
}

/**
	* 使用select函数监听multi curl文件描述符的状态
	* 监听成功返回0，监听失败返回-1
	*/
int curl_multi_select(CURLM * curl_m)
{
	int ret = 0;

	struct timeval timeout_tv;
	fd_set  fd_read;
	fd_set  fd_write;
	fd_set  fd_except;
	int     max_fd = -1;

	// 注意这里一定要清空fdset,curl_multi_fdset不会执行fdset的清空操作  //
	FD_ZERO(&fd_read);
	FD_ZERO(&fd_write);
	FD_ZERO(&fd_except);

	// 设置select超时时间  //
	timeout_tv.tv_sec = 1;
	timeout_tv.tv_usec = 0;

	// 获取multi curl需要监听的文件描述符集合 fd_set //
	curl_multi_fdset(curl_m, &fd_read, &fd_write, &fd_except, &max_fd);

	/**
		* When max_fd returns with -1,
		* you need to wait a while and then proceed and call curl_multi_perform anyway.
		* How long to wait? I would suggest 100 milliseconds at least,
		* but you may want to test it out in your own particular conditions to find a suitable value.
		*/
	if (-1 == max_fd)
	{
		return -1;
	}

	/**
		* 执行监听，当文件描述符状态发生改变的时候返回
		* 返回0，程序调用curl_multi_perform通知curl执行相应操作
		* 返回-1，表示select错误
		* 注意：即使select超时也需要返回0，具体可以去官网看文档说明
		*/
	int ret_code = ::select(max_fd + 1, &fd_read, &fd_write, &fd_except, &timeout_tv);
	switch(ret_code)
	{
		case -1:
			/* select error */
			ret = -1;
			break;
		case 0:
		/* select timeout */
		default:
			/* one or more of curl's file descriptors say there's data to read or write*/
			ret = 0;
			break;
	}

	return ret;
}

BOOL HttpRequest::Recv_Multi_Perform( CStringA & strResult )
{
	BOOL bSuccess = FALSE;
	/* init a multi stack */ 
	CURLM *multi_handle = curl_multi_init();
	__try
	{
		int nTry = 0;
Try:
		int still_running; /* keep number of running handles */ 
		int i;
 
		CURLMsg *msg; /* for picking up messages with the transfer status */ 
		int msgs_left; /* how many messages are left */ 


		curl_multi_add_handle(multi_handle, m_Curl);

		/* we start some action by calling perform right away */ 
		curl_multi_perform(multi_handle, &still_running);

		do {
			struct timeval timeout;
			int rc; /* select() return code */ 
			CURLMcode mc; /* curl_multi_fdset() return code */ 

			fd_set fdread;
			fd_set fdwrite;
			fd_set fdexcep;
			int maxfd = -1;

			long curl_timeo = -1;

			FD_ZERO(&fdread);
			FD_ZERO(&fdwrite);
			FD_ZERO(&fdexcep);

			/* set a suitable timeout to play around with */ 
			timeout.tv_sec = 1;
			timeout.tv_usec = 0;

			curl_multi_timeout(multi_handle, &curl_timeo);
			if(curl_timeo >= 0) {
				timeout.tv_sec = curl_timeo / 1000;
				if(timeout.tv_sec > 1)
					timeout.tv_sec = 1;
				else
					timeout.tv_usec = (curl_timeo % 1000) * 1000;
			}

			/* get file descriptors from the transfers */ 
			mc = curl_multi_fdset(multi_handle, &fdread, &fdwrite, &fdexcep, &maxfd);

			if(mc != CURLM_OK) {
				fprintf(stderr, "curl_multi_fdset() failed, code %d.\n", mc);
				break;
			}

			/* On success the value of maxfd is guaranteed to be >= -1. We call
			select(maxfd + 1, ...); specially in case of (maxfd == -1) there are
			no fds ready yet so we call select(0, ...) --or Sleep() on Windows--
			to sleep 100ms, which is the minimum suggested value in the
			curl_multi_fdset() doc. */ 

			if(maxfd == -1) {
#ifdef _WIN32
				Sleep(100);
				rc = 0;
#else
				/* Portable sleep for platforms other than Windows. */ 
				struct timeval wait = { 0, 100 * 1000 }; /* 100ms */ 
				rc = select(0, NULL, NULL, NULL, &wait);
#endif
			}
			else {
				/* Note that on some platforms 'timeout' may be modified by select().
				If you need access to the original value save a copy beforehand. */ 
				rc = select(maxfd+1, &fdread, &fdwrite, &fdexcep, &timeout);
			}

			switch(rc) {
			case -1:
				/* select error */ 
				break;
			case 0: /* timeout */ 
			default: /* action */ 
				curl_multi_perform(multi_handle, &still_running);
				break;
			}
		} while(still_running);

		CURLcode res = CURLE_FAILED_INIT;
		/* See how the transfers went */ 
		while((msg = curl_multi_info_read(multi_handle, &msgs_left))) 
		{
			if(msg->msg == CURLMSG_DONE) 
			{
				res = msg->data.result;
			}
		}

		if ( CURLE_OK != res )
		{
			m_dwNetCode = 909;
			curl_easy_strerror(res);
			m_sResult.clear();
			m_sHeader.clear();

			//if ( ++nTry < 1 )
			//{
			//	goto Try;
			//}

		}
		else
		{
			char * pp = strResult.GetBuffer(m_sResult.length());
			::memcpy(pp, m_sResult.c_str(), m_sResult.size());
			strResult.ReleaseBuffer(m_sResult.length());

			DWORD dwCode = 0;
			CURLcode res;
			res = curl_easy_getinfo(m_Curl, CURLINFO_RESPONSE_CODE, &dwCode);
			if ( CURLE_OK == res )
			{
				m_dwNetCode = dwCode;
			}

			DWORD dwCount = 0;
			curl_easy_getinfo(m_Curl, CURLINFO_REDIRECT_COUNT, &dwCount);

			char redirectUrl[2048] = {0};
			if ( dwCount > 0 )
			{
				int nLen = sizeof("location:");
				size_t nPos = m_sHeader.find("location:");
				if ( string::npos != nPos )
				{
					nPos += nLen;
					int n = 0;
					char ch = m_sHeader.at(nPos++);
					while ( ch != '\n' )
					{
						redirectUrl[n++] = ch;
						ch = m_sHeader.at(nPos++);
					}

					m_sRedirectUrl = redirectUrl;
				}
			}

			if ( NULL != m_Headerlist )
			{
				curl_slist_free_all(m_Headerlist);
				m_Headerlist = NULL;
			}

			bSuccess = TRUE;
		}

		curl_multi_remove_handle(multi_handle, m_Curl);
		curl_easy_cleanup(m_Curl);
		curl_multi_cleanup(multi_handle);
	}
	__except(1)
	{
		curl_multi_remove_handle(multi_handle, m_Curl);
		curl_easy_cleanup(m_Curl);
		curl_multi_cleanup(multi_handle);
	}
	return bSuccess;
}

BOOL HttpRequest::Request( LPCTSTR strUrl )
{
	string sUrl = CT2A(strUrl);
	m_sUrl = sUrl;

	curl_easy_setopt(m_Curl, CURLOPT_URL, sUrl.c_str());
	curl_easy_setopt(m_Curl, CURLOPT_ACCEPT_ENCODING, "");
	curl_easy_setopt(m_Curl, CURLOPT_HTTPHEADER, m_Headerlist);
	curl_easy_setopt(m_Curl, CURLOPT_WRITEFUNCTION, NetCallback);
	curl_easy_setopt(m_Curl, CURLOPT_WRITEDATA, &m_sResult);
	curl_easy_setopt(m_Curl, CURLOPT_HEADERFUNCTION, &HeaderCallback);
	curl_easy_setopt(m_Curl, CURLOPT_WRITEHEADER, &m_sHeader);
	//curl_easy_setopt(m_Curl, CURLOPT_FOLLOWLOCATION, 1);
	curl_easy_setopt(m_Curl, CURLOPT_TIMEOUT, m_nTimeout); //设置连接超时时间
	curl_easy_setopt(m_Curl, CURLOPT_ERRORBUFFER, m_pErrorBuff);
	curl_easy_setopt(m_Curl,CURLOPT_COOKIEFILE,"");
	curl_easy_setopt(m_Curl,CURLOPT_VERBOSE,1);

	if ( !m_sCookie.empty() )
	{
		curl_easy_setopt(m_Curl, CURLOPT_COOKIE, m_sCookie.c_str());
	}

	if ( NULL != m_Headerlist )
	{
		curl_easy_setopt(m_Curl, CURLOPT_POSTQUOTE, &m_Headerlist);
	}

	if ( 0 == strnicmp(m_sUrl.c_str(), "https", 5) )
	{
		curl_easy_setopt(m_Curl, CURLOPT_SSL_VERIFYPEER, 0L);
		curl_easy_setopt(m_Curl, CURLOPT_SSL_VERIFYHOST, 0L);
// 			curl_easy_setopt(m_Curl, CURLOPT_SSLVERSION, CURL_SSLVERSION_TLSv1_2);
// 			curl_easy_setopt(m_Curl, CURLOPT_SSL_SESSIONID_CACHE, 1L);
//		curl_easy_setopt(m_Curl, CURLOPT_SSL_VERIFYPEER, 0L);
//		curl_easy_setopt(m_Curl, CURLOPT_SSL_FALSESTART, 0L);
	}

	return TRUE;
}

DWORD HttpRequest::GetStatusCode()
{
	return m_dwNetCode;
}

BOOL HttpRequest::RecvRedirectLocation( CString& strResult )
{
	strResult = CA2T(m_sRedirectUrl.c_str());
	return TRUE;
}

CString HttpRequest::GetRealUrl()
{
	CString strResult = CA2T(m_sRedirectUrl.c_str());
	return strResult;
}

void HttpRequest::SetCookie( LPCSTR pszCookies )
{
	m_sCookie = pszCookies;
}

void HttpRequest::GetErrorBuff( string& sBuff )
{
	sBuff = m_pErrorBuff;
}

void HttpRequest::InitTimeout(int seconds)
{
	m_nTimeout = seconds;
}

void HttpRequest::SaveCookieFile( LPCSTR pszFile )
{
	m_sCookieFile = pszFile;
	curl_easy_setopt(m_Curl,CURLOPT_COOKIEJAR,m_sCookieFile.c_str());
}

void HttpRequest::SetCookieFile( LPCSTR pszFile )
{
	m_sCookieFile = pszFile;
	curl_easy_setopt(m_Curl,CURLOPT_COOKIEFILE,m_sCookieFile.c_str());
}

void HttpRequest::GlobalInit()
{
	curl_global_init(CURL_GLOBAL_DEFAULT);
}

void HttpRequest::GlobalUnInit()
{
	//curl_global_cleanup();
}

BOOL HttpRequestGet::Request( LPCTSTR strUrl )
{
	return __super::Request(strUrl);
}

BOOL HttpRequestPost::Request( LPCTSTR strUrl )
{
	string sUrl = CT2A(strUrl);
	m_sUrl = sUrl;

	curl_easy_setopt(m_Curl, CURLOPT_URL, sUrl.c_str());
	curl_easy_setopt(m_Curl, CURLOPT_HTTPHEADER, m_Headerlist);
	curl_easy_setopt(m_Curl, CURLOPT_WRITEFUNCTION, NetCallback);
	curl_easy_setopt(m_Curl, CURLOPT_WRITEDATA, &m_sResult);
	curl_easy_setopt(m_Curl, CURLOPT_HEADERFUNCTION, &HeaderCallback);
	curl_easy_setopt(m_Curl, CURLOPT_WRITEHEADER, &m_sHeader);
	curl_easy_setopt(m_Curl, CURLOPT_POST, 1);
	//curl_easy_setopt(m_Curl, CURLOPT_FOLLOWLOCATION, 1);
	curl_easy_setopt(m_Curl, CURLOPT_TIMEOUT, m_nTimeout); //设置连接超时时间
	curl_easy_setopt(m_Curl, CURLOPT_ERRORBUFFER, m_pErrorBuff);
	curl_easy_setopt(m_Curl, CURLOPT_ACCEPT_ENCODING, "gzip");
	curl_easy_setopt(m_Curl,CURLOPT_VERBOSE,1);

	if ( NULL != m_Headerlist )
	{
		curl_easy_setopt(m_Curl, CURLOPT_POSTQUOTE, &m_Headerlist);
	}

	if ( 0 == strnicmp(m_sUrl.c_str(), "https", 5) )
	{
		curl_easy_setopt(m_Curl, CURLOPT_SSL_VERIFYPEER, 0L);
		curl_easy_setopt(m_Curl, CURLOPT_SSL_VERIFYHOST, 0L);
	}

	if ( !m_PostData.IsEmpty() )
	{
		curl_easy_setopt(m_Curl, CURLOPT_POSTFIELDS, m_PostData.GetBuffer());
		curl_easy_setopt(m_Curl, CURLOPT_POSTFIELDSIZE, m_PostData.GetLength());
	}

	return TRUE;
}

BOOL HttpRequestPost::AddSendData( LPCSTR strSendData )
{
	if ( NULL == strSendData )
	{
		return FALSE;
	}

	m_PostData = strSendData;

	return TRUE;
}

BOOL HttpRequestPostForm::Request( LPCTSTR strUrl )
{
	AddHeader(_T("Content-Type"),_T("application/x-www-form-urlencoded"));
	return __super::Request(strUrl);
}

BOOL HttpRequestPostForm::AddSendData( LPCWSTR lpszKey, LPCWSTR lpszValue )
{
	CStringA tTempKey = CW2A(lpszKey, CP_UTF8);
	CStringA tTempValue = CW2A(lpszValue, CP_UTF8);
	if(m_PostData != "")
	{
		m_PostData += "&";
	}
	m_PostData += tTempKey;
	m_PostData += "=";
	m_PostData += tTempValue;

	return TRUE;
}

BOOL HttpRequestPostForm::AddSendData( LPCSTR lpszKey, LPCSTR lpszValue )
{
	if(m_PostData != "")
	{
		m_PostData += "&";
	}
	m_PostData += lpszKey;
	m_PostData += "=";
	m_PostData += lpszValue;

	return TRUE;
}


void HttpDownload::Init()
{

}

HttpDownload::HttpDownload():
m_Curl(NULL),
	m_dwCode( 0 )
{

}

BOOL HttpDownload::Download( DownloadInfo* pInfo )
{
	BOOL bRet = FALSE;
	do 
	{
		string sPath = pInfo->savepath + "_tmp";

		FILE* pFile = NULL;

		if ( pInfo->newtask )
			pFile = fopen(sPath.c_str(), "wb+");
		else
			pFile = fopen(sPath.c_str(), "ab+");

		if ( NULL == pFile )
		{
			break;
		}

		curl_off_t offset = 0;
		struct stat file_info;
		if ( 0 == stat(sPath.c_str(), &file_info) )
		{
			offset = file_info.st_size;
		}

		static DWORD id = GetTickCount();
		if ( 0 != pInfo->id )
		{
			pInfo->id = id++;
		}

		// 设置属性
		//curl_global_init(CURL_GLOBAL_DEFAULT);
		m_Curl = curl_easy_init();
		if ( NULL == m_Curl )
		{
			break;
		}

		curl_easy_setopt(m_Curl, CURLOPT_FOLLOWLOCATION, 1);
		curl_easy_setopt(m_Curl, CURLOPT_TIMEOUT, 300);
		curl_easy_setopt(m_Curl, CURLOPT_URL, pInfo->url.c_str());
		curl_easy_setopt(m_Curl, CURLOPT_WRITEFUNCTION, DownFileCallback);
		curl_easy_setopt(m_Curl, CURLOPT_WRITEDATA, pFile);
		curl_easy_setopt(m_Curl, CURLOPT_NOPROGRESS, false);
		curl_easy_setopt(m_Curl, CURLOPT_PROGRESSFUNCTION, ProgressCallback);
		curl_easy_setopt(m_Curl, CURLOPT_PROGRESSDATA, pInfo);
		curl_easy_setopt(m_Curl, CURLOPT_RESUME_FROM, pInfo->newtask ? 0 : offset);

		pInfo->startpos = (pInfo->newtask ? 0 : offset);

		// 提交数据
		CURLcode res = curl_easy_perform(m_Curl);

		fclose( pFile );

		// 如果原文件存在，删掉
		if ( 0 == _access(pInfo->savepath.c_str(), 0x00) )
		{
			DeleteFileA(pInfo->savepath.c_str());
		}

		rename(sPath.c_str(), pInfo->savepath.c_str());

		if ( res != CURLE_OK )
		{
			break;
		}

		// 获取返回码
		DWORD dwCode = 0;
		curl_easy_getinfo(m_Curl, CURLINFO_RESPONSE_CODE, &dwCode);
		m_dwCode = dwCode;

	} while ( false );

	// 释放资源
	if ( NULL != m_Curl )
	{
		curl_easy_cleanup(m_Curl);
	}

	curl_global_cleanup();

	return bRet;
}