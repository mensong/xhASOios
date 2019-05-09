#pragma once

#include <Windows.h>
#include <string>
#include <map>
using namespace std;

#include "curl.h"

class HttpRequest
{
public:
	HttpRequest(void);
	~HttpRequest(void);

public:
	BOOL Init(LPCTSTR str_SessionName);

	void SetHttpProxy(std::string p, std::string auth);

	/// 添加HTTP请求头
	void AddHeader(CString key, CString value);

	void SetCookie(LPCSTR pszCookies);

	void SaveCookieFile(LPCSTR pszFile);

	void SetCookieFile(LPCSTR pszFile);

	CString GetHeadersAsString();

	DWORD GetContentLength();

	BOOL RecvContent(CStringA & strResult);

	BOOL Recv_Multi_Perform( CStringA & strResult );

	BOOL Recv_Multi_Perform2( CStringA & strResult );

	//下载文件
	BOOL DownloadToFile( LPCTSTR strUrl, LPCTSTR strPath ,void *AppleThis);
	// 保存到文件
	BOOL RecvContentToFile( LPCTSTR strUrl, LPCTSTR strPath );

	BOOL RecvRedirectLocation(CString& strResult );

	CString GetRealUrl();

	void GetErrorBuff(string& sBuff);

	static DWORD WINAPI Easy_Perform_Multi(void *p);

	BOOL RecvContent_Multi( CStringA & strResult );
public:
	virtual void InitTimeout(int seconds);
	virtual BOOL Request(LPCTSTR strUrl);

	virtual DWORD GetStatusCode();

	virtual BOOL AddSendData(LPCSTR strSendData) { return FALSE; };
	virtual BOOL AddSendData(LPCWSTR lpszKey, LPCWSTR lpszValue){ return FALSE; };
	virtual BOOL AddSendData(LPCSTR lpszKey, LPCSTR lpszValue){ return FALSE; };

	static void GlobalInit();
	static void GlobalUnInit();
protected:
	CURL * m_Curl;
	struct curl_slist *m_Headerlist;

	string m_sResult;
	string m_sHeader;
	string m_sUrl;
	string m_sRedirectUrl;
	string m_sCookie;
	string m_sCookieFile;
	DWORD  m_dwNetCode;
	char   m_pErrorBuff[CURL_ERROR_SIZE];

	int m_nTimeout;
};

class HttpRequestGet : public HttpRequest
{
public:
	virtual BOOL Request(LPCTSTR strUrl);
};

class HttpRequestPost : public HttpRequest
{
public:

	virtual BOOL Request(LPCTSTR strUrl);

	BOOL AddSendData(LPCSTR strSendData); 

protected:
	CStringA m_PostData;

};

class HttpRequestPostForm : public HttpRequestPost
{
public:
	virtual BOOL Request(LPCTSTR strUrl);

	BOOL AddSendData(LPCWSTR lpszKey, LPCWSTR lpszValue);
	BOOL AddSendData(LPCSTR lpszKey, LPCSTR lpszValue);
};

typedef void (* PROCESS_CALLBACK)(double dltotal, double dlnow, void* user);

class DownloadInfo
{	
public:
	DownloadInfo()
	{
		newtask		= false;
		id			= 0;
		callback	= NULL;
		startpos	= 0;
		data		= NULL;
	}
public:
	bool				newtask;	// 是否断电续传，为true则不续传开启新任务，为false则续传
	unsigned long		id;			// 任务id,开启任务时需要设置，如果没有设置，下载开启调用时会给它赋值
	unsigned long		startpos;	// 任务开始位置
	string				url;		// 下载url
	string				savepath;	// 文件保存路径
	PROCESS_CALLBACK	callback;	// 进度回调函数
	void*				data;		// 用户数据
};

class HttpDownload
{
public:
	HttpDownload();

	virtual void Init();

	BOOL Download(DownloadInfo* pInfo);
private:
	CURL * m_Curl;

	DWORD m_dwCode;
};
