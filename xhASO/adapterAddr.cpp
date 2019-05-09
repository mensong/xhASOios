#include "adapterAddr.h"

AdaptersAddress::AdaptersAddress()
{
    pAdapterAddr = (PIP_ADAPTER_ADDRESSES)MALLOC(sizeof(IP_ADAPTER_ADDRESSES));
    flags = GAA_FLAG_INCLUDE_PREFIX;
    sizePointer = 0;
}
AdaptersAddress::~AdaptersAddress()
{
    if(pAdapterAddr)
    {
        FREE(pAdapterAddr);
    }
}

DWORD AdaptersAddress::GetAddressInfo(DWORD family, PMY_ADDRESS &addr_info)
{
    int Try = 0;
    if(pAdapterAddr == NULL)
    {
        SetErrMsg(TEXT("Memory allocation failed!"));
        return FALSE;
    }
    do
    {
        dwError = GetAdaptersAddresses(family, flags, 0, pAdapterAddr, &sizePointer);
        if(dwError == ERROR_BUFFER_OVERFLOW)
        {
            if(pAdapterAddr != NULL)
            {    
                FREE(pAdapterAddr);
                pAdapterAddr = NULL;
            }
            pAdapterAddr = (PIP_ADAPTER_ADDRESSES)MALLOC(sizePointer);
        }
    }
	while((dwError == ERROR_BUFFER_OVERFLOW) && (Try < 3));
    if(dwError == NO_ERROR)
    {
        TCHAR addr_Format[] = TEXT("%d.%d.%d.%d");
        TCHAR  mac_Format[] = TEXT("%.2x-%.2x-%.2x-%.2x"); 
        pCurrentAddr = pAdapterAddr;
        addr_info->AdapterAddresses = (PIP_ADDRESS)MALLOC(sizeof(IP_ADDRESS));
        addr_info->AdapterAddresses->NEXT = NULL;
        addr_info->AdapterDNSs = (PDNS_ADDRESS)MALLOC(sizeof(DNS_ADDRESS));
        addr_info->AdapterDNSs->NEXT = NULL;
        addr_info->NEXT = NULL;
        PMY_ADDRESS tmp_my = NULL;
        PMY_ADDRESS current_my = NULL;
        PIP_ADAPTER_UNICAST_ADDRESS tmp_unicast;
        
        while(pCurrentAddr)
        {

            if(current_my == NULL)
            {
                PIP_ADDRESS current_ip = NULL;
                PIP_ADDRESS tmp_ip = NULL;
                StringCchCopy(addr_info->AdapterName, 20, pCurrentAddr->FriendlyName);
                StringCchPrintf(addr_info->AdapterMac, 14, mac_Format,
                    pCurrentAddr->PhysicalAddress[0], pCurrentAddr->PhysicalAddress[1],
                    pCurrentAddr->PhysicalAddress[2], pCurrentAddr->PhysicalAddress[3]);
                current_my = addr_info;
                tmp_unicast = pCurrentAddr->FirstUnicastAddress;
                pDnsAddr = pCurrentAddr->FirstDnsServerAddress;
                PDNS_ADDRESS current_dns = addr_info->AdapterDNSs; 
                StringCchPrintf(addr_info->AdapterDNSs->AdapterDNS, 24, addr_Format, 
                    (BYTE)pDnsAddr->Address.lpSockaddr->sa_data[2], (BYTE)pDnsAddr->Address.lpSockaddr->sa_data[3],
                    (BYTE)pDnsAddr->Address.lpSockaddr->sa_data[4], (BYTE)pDnsAddr->Address.lpSockaddr->sa_data[5]);
                while(pDnsAddr->Next)
                {
                    pDnsAddr = pDnsAddr->Next;
                    PDNS_ADDRESS tmp_dns = (PDNS_ADDRESS)MALLOC(sizeof(DNS_ADDRESS));
                    tmp_dns->NEXT = NULL;
                    StringCchPrintf(tmp_dns->AdapterDNS, 24, addr_Format, 
                    (BYTE)pDnsAddr->Address.lpSockaddr->sa_data[2], (BYTE)pDnsAddr->Address.lpSockaddr->sa_data[3],
                    (BYTE)pDnsAddr->Address.lpSockaddr->sa_data[4], (BYTE)pDnsAddr->Address.lpSockaddr->sa_data[5]);
                    current_dns->NEXT = tmp_dns;
                    current_dns = tmp_dns;
                }
                while(tmp_unicast)
                {
                    if(current_ip == NULL)
                    {
                        StringCchPrintf(addr_info->AdapterAddresses->AdapterAddress, 24, addr_Format,
                            (BYTE)tmp_unicast->Address.lpSockaddr->sa_data[2], (BYTE)tmp_unicast->Address.lpSockaddr->sa_data[3],
                            (BYTE)tmp_unicast->Address.lpSockaddr->sa_data[4], (BYTE)tmp_unicast->Address.lpSockaddr->sa_data[5]);
                        current_ip = addr_info->AdapterAddresses;
                    
                    }
                    else
                    {
                        tmp_ip = (PIP_ADDRESS)MALLOC(sizeof(IP_ADDRESS));
                        tmp_ip->NEXT = NULL;
                        StringCchPrintf(tmp_ip->AdapterAddress, 24, addr_Format,
                            (BYTE)tmp_unicast->Address.lpSockaddr->sa_data[2], (BYTE)tmp_unicast->Address.lpSockaddr->sa_data[3],
                            (BYTE)tmp_unicast->Address.lpSockaddr->sa_data[4], (BYTE)tmp_unicast->Address.lpSockaddr->sa_data[5]);
                        current_ip->NEXT = tmp_ip;
                        current_ip = tmp_ip;
                    }
                    tmp_unicast = tmp_unicast->Next;
                }
                
            }
            else
            {
                PIP_ADDRESS tmp_ip = NULL;
                PIP_ADDRESS current_ip = NULL;
                tmp_my = (PMY_ADDRESS)MALLOC(sizeof(MY_ADDRESS));
                tmp_my->NEXT = NULL;
                tmp_my->AdapterAddresses = (PIP_ADDRESS)MALLOC(sizeof(IP_ADDRESS));
                tmp_my->AdapterAddresses->NEXT = NULL;
                tmp_my->AdapterDNSs = (PDNS_ADDRESS)MALLOC(sizeof(DNS_ADDRESS));
                tmp_my->AdapterDNSs->NEXT = NULL;
                StringCchCopy(tmp_my->AdapterName, 20, pCurrentAddr->FriendlyName);
                StringCchPrintf(tmp_my->AdapterMac, 14, mac_Format,
                    pCurrentAddr->PhysicalAddress[0], pCurrentAddr->PhysicalAddress[1],
                    pCurrentAddr->PhysicalAddress[2], pCurrentAddr->PhysicalAddress[3]);
                pDnsAddr = pCurrentAddr->FirstDnsServerAddress;
                if(pDnsAddr)
                {
                    StringCchPrintf(tmp_my->AdapterDNSs->AdapterDNS, 24, addr_Format, 
                        (BYTE)pDnsAddr->Address.lpSockaddr->sa_data[2], (BYTE)pDnsAddr->Address.lpSockaddr->sa_data[3],
                        (BYTE)pDnsAddr->Address.lpSockaddr->sa_data[4], (BYTE)pDnsAddr->Address.lpSockaddr->sa_data[5]);
                }
                tmp_unicast = pCurrentAddr->FirstUnicastAddress;
                while(tmp_unicast)
                {
                    if(current_ip == NULL)
                    {
                        StringCchPrintf(tmp_my->AdapterAddresses->AdapterAddress, 24, addr_Format,
                            (BYTE)tmp_unicast->Address.lpSockaddr->sa_data[2], (BYTE)tmp_unicast->Address.lpSockaddr->sa_data[3],
                            (BYTE)tmp_unicast->Address.lpSockaddr->sa_data[4], (BYTE)tmp_unicast->Address.lpSockaddr->sa_data[5]);
                        current_ip = addr_info->AdapterAddresses;
                    }
                    else
                    {
                        tmp_ip = (PIP_ADDRESS)MALLOC(sizeof(IP_ADDRESS));
                        tmp_ip->NEXT = NULL;
                        StringCchPrintf(tmp_ip->AdapterAddress, 24, addr_Format,
                            (BYTE)tmp_unicast->Address.lpSockaddr->sa_data[2], (BYTE)tmp_unicast->Address.lpSockaddr->sa_data[3],
                            (BYTE)tmp_unicast->Address.lpSockaddr->sa_data[4], (BYTE)tmp_unicast->Address.lpSockaddr->sa_data[5]);
                        current_ip->NEXT = tmp_ip;
                        current_ip = tmp_ip;
                    }
                    tmp_unicast = tmp_unicast->Next;
                }
                current_my->NEXT = tmp_my;
                current_my = tmp_my;
            }
            pCurrentAddr = pCurrentAddr->Next;
        }
    }
    else if(dwError == ERROR_NO_DATA)
    {
        SetErrMsg(TEXT("No data!"));
    }
    return dwError;
}
void AdaptersAddress::SetErrMsg(TCHAR Msg[])
{
    StringCchCopy(szErrMsg, 260, Msg);
}