#pragma once
#include <windows.h>
class CxhLog
{
public:
	CxhLog(void);
	virtual ~CxhLog(void);
	void xhLogPrintf(char* lpLogInfo,int line,char* pFunName,char* pFileName);
	void writeLogFile(char* pLogData);//»»ÐÐ·û'\n'½áÎ²
	static DWORD WINAPI writeLogThread(LPVOID lpParam);
protected:
	int    m_WriteLog;
	HANDLE  m_hMutex;
	CString m_logPath;
	CString m_configPath;
};
