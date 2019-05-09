#include "stdafx.h"
#include "CxhLog.h"
#include <iostream>
#include <fstream>
using namespace std;

#define		MUTEX_PROCESS_NAME		TEXT("__MutexMutilProcessName__")
#define     LOG_DATA_SIZE			512
#define		LOG_ANSII_DATA_SIZE		256


typedef struct _multiProc_logInfo
{
	HANDLE hMutex;
	TCHAR  dwLogPath[MAX_PATH];
	char   szLogData[LOG_DATA_SIZE];

}MULTIPROC_LOGINFO,*PMULTIPROC_LOGINFO;

CxhLog::CxhLog(void)
{
	TCHAR strPath[MAX_PATH] = {0};
	CString strFilePath;
	DWORD dwResult = GetModuleFileName(NULL,strPath,MAX_PATH);
	strFilePath = strPath;

	int Pos = strFilePath.ReverseFind(_T('\\'));
	strFilePath.Truncate(Pos);
	strFilePath += TEXT("\\");
	

	m_logPath += strFilePath;
	m_logPath += TEXT("errorInfo.log");

	m_configPath = strFilePath;
	m_configPath += TEXT("xhconfig.ini");
	m_WriteLog = GetPrivateProfileInt(TEXT("LogSwitch"),TEXT("writeLog"),0,m_configPath.GetBuffer());
	
	m_hMutex = NULL;
	
}

CxhLog::~CxhLog(void)
{
	if (m_hMutex)
	{
		ReleaseMutex(m_hMutex);
	}
}

void CxhLog::xhLogPrintf(char* lpLogInfo,int line,char* pFunName,char* pFileName)
{
	if (!m_WriteLog)
	{
		return;
	}
	try
	{
		char _xhLogInfo[LOG_ANSII_DATA_SIZE] = {0};

		sprintf(_xhLogInfo,"dump:%s;at line %d in %s; file: %s\n", lpLogInfo, line, pFunName, pFileName);

		writeLogFile(_xhLogInfo);
	}
	catch(...)
	{
	}
	return;
}

void CxhLog::writeLogFile(char* pLogData)
{
	try
	{
		DWORD dwThreadId = 0;
		HANDLE hThread = NULL;
		MULTIPROC_LOGINFO multiProc_logInfo = {0};
		//创建进程互斥量
		m_hMutex = CreateMutex(NULL,FALSE,MUTEX_PROCESS_NAME);
		if (!m_hMutex)
		{
			return;
		}
		int logDataLen = strlen(pLogData);
		multiProc_logInfo.hMutex = m_hMutex;
		
		_tcscpy(multiProc_logInfo.dwLogPath,m_logPath.GetBuffer());
		
		if (logDataLen <= LOG_DATA_SIZE)
		{
			strcpy(multiProc_logInfo.szLogData,pLogData);
		}

		hThread = CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)writeLogThread,(LPVOID)&multiProc_logInfo,0,&dwThreadId);
		
		WaitForSingleObject(hThread,INFINITE);
		
		CloseHandle(hThread);
		CloseHandle(m_hMutex);
	}
	catch(...)
	{
	}
	return;
}

DWORD WINAPI CxhLog::writeLogThread(LPVOID lpParam)
{
	PMULTIPROC_LOGINFO pMultiProc_logInfo = (PMULTIPROC_LOGINFO)lpParam;
	_ASSERTE(pMultiProc_logInfo != NULL);
	try
	{
		char szPath[LOG_DATA_SIZE] = {0};
		WideCharToMultiByte(CP_ACP,0,pMultiProc_logInfo->dwLogPath,_tcslen(pMultiProc_logInfo->dwLogPath),szPath,LOG_DATA_SIZE,NULL,NULL);
		
		fstream file(szPath,ios::out | ios::binary | ios::app);

		file.write((char*)pMultiProc_logInfo->szLogData,strlen((char*)pMultiProc_logInfo->szLogData));

		file.flush();

		file.close();

		ReleaseMutex(pMultiProc_logInfo->hMutex);

	}
	catch(...)
	{
	}

	return 0;

}
