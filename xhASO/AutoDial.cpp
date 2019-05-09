#include "StdAfx.h"
#include "AutoDial.h"

#define ENTRY		L"¿í´øÁ¬½Ó"
#pragma comment(lib, "Rasapi32.lib")

CAutoDial::CAutoDial(void)
{
}


CAutoDial::~CAutoDial(void)
{
}

BOOL CAutoDial::Connect(CString strEntryName, CString strUserName, CString strPassword)
{
  STARTUPINFO stStartUpInfo;
  memset(&stStartUpInfo, 0, sizeof(STARTUPINFO));
  stStartUpInfo.cb			= sizeof(STARTUPINFO);
  stStartUpInfo.dwFlags		= STARTF_USESHOWWINDOW;
  stStartUpInfo.wShowWindow	= SW_HIDE;

  PROCESS_INFORMATION stProcessInfo;
  stProcessInfo.hProcess	= NULL;
  stProcessInfo.hThread	= NULL;

  CString strCmdLine;
  strCmdLine.Format(_T("rasdial %s %s %s"), strEntryName, strUserName, strPassword);
  BOOL bRet = CreateProcess( NULL, (LPTSTR) (LPCTSTR)strCmdLine, 
    NULL, NULL, FALSE, NORMAL_PRIORITY_CLASS, NULL, NULL, 
    &stStartUpInfo, &stProcessInfo);

  WaitForSingleObject(stProcessInfo.hProcess, 1000*10);
  CloseHandle(stProcessInfo.hProcess);
  CloseHandle(stProcessInfo.hThread);
  return TRUE;
}

BOOL CAutoDial::Disconnect()
{
  //WinExec("rasdial /DISCONNECT", SW_HIDE);

  STARTUPINFO stStartUpInfo;
  memset(&stStartUpInfo, 0, sizeof(STARTUPINFO));
  stStartUpInfo.cb			= sizeof(STARTUPINFO);
  stStartUpInfo.dwFlags		= STARTF_USESHOWWINDOW;
  stStartUpInfo.wShowWindow	= SW_HIDE;

  PROCESS_INFORMATION stProcessInfo;
  stProcessInfo.hProcess	= NULL;
  stProcessInfo.hThread	= NULL;

  CString strCmdLine = _T("rasdial /DISCONNECT");
  BOOL bRet = CreateProcess( NULL, (LPTSTR) (LPCTSTR)strCmdLine, 
    NULL, NULL, FALSE, NORMAL_PRIORITY_CLASS, NULL, NULL, 
    &stStartUpInfo, &stProcessInfo);

  WaitForSingleObject(stProcessInfo.hProcess, 1000*10);
  CloseHandle(stProcessInfo.hProcess);
  CloseHandle(stProcessInfo.hThread);
  return TRUE;
}

std::string CAutoDial::GetLocalIpAddress()
{
  WORD wVersionRequested = MAKEWORD(2, 2);   
  WSADATA wsaData;   
  if (WSAStartup(wVersionRequested, &wsaData) != 0)   
    return "";   
  char local[255] = {0};   
  gethostname(local, sizeof(local));   
  hostent* ph = gethostbyname(local);   
  if (ph == NULL)   
    return "";   
  in_addr addr;   
  memcpy(&addr, ph->h_addr_list[0], sizeof(in_addr));   
  std::string localIP;   
  localIP.assign(inet_ntoa(addr));   
  WSACleanup();   
  return localIP;
}
