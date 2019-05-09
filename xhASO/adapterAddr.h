#ifndef __ADPATER_ADDR_H
#define __ADPATER_ADDR_H

#include <WinSock2.h>
#include <IPHlpApi.h>
#include <tchar.h>
#include <strsafe.h>
#pragma comment(lib, "IPHlpApi.lib")

typedef struct IP_ADDRESS
{
    IP_ADDRESS *NEXT;
    TCHAR AdapterAddress[24];
}*PIP_ADDRESS;
typedef struct DNS_ADDRESS
{
    DNS_ADDRESS *NEXT;
    TCHAR AdapterDNS[24];
}*PDNS_ADDRESS;
typedef struct MY_ADDRESS
{
    PIP_ADDRESS AdapterAddresses; 
    PDNS_ADDRESS AdapterDNSs;
    TCHAR AdapterMac[14];
    TCHAR AdapterName[20];
    MY_ADDRESS *NEXT;
}*PMY_ADDRESS;

#define MALLOC(x) HeapAlloc(GetProcessHeap(), 0, (x))
#define FREE(x) HeapFree(GetProcessHeap(), 0, (x))

class AdaptersAddress
{
public:
    AdaptersAddress();
	~AdaptersAddress();
	 DWORD GetAddressInfo(DWORD family, PMY_ADDRESS &addr_info);
private:
    void SetErrMsg(TCHAR Msg[]);
	PIP_ADAPTER_ADDRESSES pAdapterAddr;
    PIP_ADAPTER_ADDRESSES pCurrentAddr;
    PIP_ADAPTER_UNICAST_ADDRESS pUnicastAddr;
    PIP_ADAPTER_DNS_SERVER_ADDRESS pDnsAddr;
    DWORD dwError;
    DWORD flags;
    ULONG sizePointer;
    TCHAR szErrMsg[260];
};

#endif