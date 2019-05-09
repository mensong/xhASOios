#ifndef __REQUESTINTERFACE_H
#define __REQUESTINTERFACE_H

BOOL xhNetworkCheck(char* pIPData);
BOOL xhRequestDataFromServer(char* pData,TCHAR* lpClientId);
BOOL xhReportDataToServer(char* lpReportContext,char* lpOutData);

#endif