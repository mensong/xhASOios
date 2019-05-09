#ifndef _SPLOOKUPOFFER_H
#define _SPLOOKUPOFFER_H
#include "statusData.h"

BOOL ReadResponseData(HINTERNET hOpenReq,LPVOID lpBuffer,LPDWORD lpdwSize);
BOOL getSignSapSetup(CString strUserAgent,char* lpBuffer,char* lpOutBuf,int& dataLen,char* lpDsid,CString strCookie,TCHAR* lpX_Apple_I_md_m,TCHAR* lpX_Apple_I_md,TCHAR* lpProxy,int netType);
BOOL sendSpLookupOffer(CString strUserAgent,char* lpSignature,char* lpDsid,char* lpSaleId,char* lpTimeStamp,CString strCookie,TCHAR* lpX_Apple_I_md_m,TCHAR* lpX_Apple_I_md,TCHAR* lpProxy,int netType);
CString GetSPClientTime();

#endif