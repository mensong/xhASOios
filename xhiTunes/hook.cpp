#include "stdafx.h"
#include "hook.h"
#include <Iphlpapi.h>
#pragma comment(lib,"Iphlpapi.lib")

OLDHOOKDATA g_oldHookData = {0};
#define RANDOM(x) (rand()%x)
#define MAC_ADDR_LENGTH 12
DWORD g_SerialNum = 0;
char g_strMac[7] = {0};

char HEXCHAR[16]={'0','1','2','3','4','5','6','7','8','9','A','B','C',
'D','E','F'};

char genMACAddr[MAC_ADDR_LENGTH];

int getMacAddrHookPoint(char* hModule)
{
   HANDLE v1; 
   int result = 0;
   unsigned int i;
   struct _MODULEINFO modinfo;

   v1 = GetCurrentProcess();
   if ( GetModuleInformation(v1, (HMODULE)hModule, &modinfo, 0xCu) )
   {
		for ( i = (unsigned int)hModule; i < (unsigned int)&hModule[modinfo.SizeOfImage - 4096]; ++i )
		{
			if (*(DWORD*)i == 0xC7F63356
				&& *(DWORD*)(i+4) == 0x0798FC45
				&& *(DWORD*)(i+8) == 0xC0330000
				&& *(DWORD*)(i+12) == 0x006FBF57)
			{
				result = i + 72;
				break;
			}
		}

   }
   return result;
}

int getSerialNumHookPoint(char* hModule)
{
   HANDLE v1; 
   int result = 0;
   unsigned int i;
   struct _MODULEINFO modinfo;

   v1 = GetCurrentProcess();
   if ( GetModuleInformation(v1, (HMODULE)hModule, &modinfo, 0xCu) )
   {
		for ( i = (unsigned int)hModule; i < (unsigned int)&hModule[modinfo.SizeOfImage - 4096]; ++i )
		{
			if (*(DWORD*)i == 0x006A006A
				&& *(DWORD*)(i+4) == 0x006A006A
				&& *(DWORD*)(i+8) == 0x50D0458D
				&& *(DWORD*)(i+12) == 0x006A006A
				&& *(DWORD*)(i+49) == 0x02001968)
			{
				result = i + 21;
				break;
			}
		}

   }
   return result;
}

DWORD WINAPI customGetAdapterInfo(__out PIP_ADAPTER_INFO pAdapterInfo,PULONG pOutBufLen)
{
	char strMac[13] = {0};
	char ch[4] = {0};
	PIP_ADAPTER_INFO pAdapter = NULL;

	DWORD nRet = GetAdaptersInfo(pAdapterInfo,pOutBufLen);
	if (nRet == NO_ERROR)
	{
		unsigned short i=0;
		unsigned short n=0;
		srand(getpid());
		for(int i=0;i<MAC_ADDR_LENGTH;i++)
		{
		  n=RANDOM(16);
		  genMACAddr[i]=HEXCHAR[n];
		}
		//16进制字符串转换成字符串
		for (int i=0; i<strlen((char*)genMACAddr)/2; i++)
		{
			
			sscanf(genMACAddr+i*2,"%02X",ch);
			strMac[i] = (char)(ch[0] & 0xFFu);
		}

		pAdapter = pAdapterInfo;
		if (g_strMac[0] != '\0')
		{
			memcpy(pAdapter->Address,g_strMac,6);
		}
		else
		{
			memcpy(pAdapter->Address,strMac,6);
			strcpy(g_strMac,strMac);
		}
		
		
		
	}
	return nRet;
}

void ModifyMacAddressValue(void)
{
	char jmpAddr[6] = {0};
	jmpAddr[0] = 0xe8;
	HMODULE hModule = ::GetModuleHandle(TEXT("iTunes.dll"));
	if (!hModule)
	{
		return;
	}

	int macAddressPoint = getMacAddrHookPoint((char*)hModule);
	//保存旧的hook数据
	g_oldHookData.macAddrData.dwOldHookPoint = macAddressPoint;
	memcpy(g_oldHookData.macAddrData.OldbyteValue,(PVOID)macAddressPoint,6);
	//计算跳转地址的后四字节,后四字节 = 目的地址 - 当前地址 - 5
	DWORD fourByte = (DWORD)customGetAdapterInfo - macAddressPoint - 5;

	for (int i=0; i<4; i++)
	{
		jmpAddr[i+1] = *((char*)&fourByte + i);
	}
	jmpAddr[5] = 0x90;
	//替换
	DWORD oldProtect;
	VirtualProtect((LPVOID)macAddressPoint,6,PAGE_READWRITE,&oldProtect);

	memcpy((PVOID)macAddressPoint,jmpAddr,6);

	VirtualProtect((LPVOID)macAddressPoint,6,oldProtect,&oldProtect);

	return;

}

BOOL WINAPI CustomGetVolumeInformation(
  __in          LPCTSTR lpRootPathName,
  __out         LPTSTR lpVolumeNameBuffer,
  __in          DWORD nVolumeNameSize,
  __out         LPDWORD lpVolumeSerialNumber,
  __out         LPDWORD lpMaximumComponentLength,
  __out         LPDWORD lpFileSystemFlags,
  __out         LPTSTR lpFileSystemNameBuffer,
  __in          DWORD nFileSystemNameSize
)
{

	LARGE_INTEGER timeRand; 
	BOOL bRet = FALSE;
	if (g_SerialNum != 0)
	{
		*lpVolumeSerialNumber = g_SerialNum;
		bRet = TRUE;
	}
	else
	{
		bRet = GetVolumeInformation(lpRootPathName,
									 lpVolumeNameBuffer,
									 nVolumeNameSize,
									 lpVolumeSerialNumber,
									 lpMaximumComponentLength,
									 lpFileSystemFlags,
									 lpFileSystemNameBuffer,
									 nFileSystemNameSize);

		if (bRet)
		{	
			QueryPerformanceCounter(&timeRand);
			*lpVolumeSerialNumber = timeRand.LowPart;
			g_SerialNum = timeRand.LowPart;
		}
	}
	

	return bRet;
}


void ModifySerialNumValue(void)
{
	char jmpAddr[6] = {0};
	jmpAddr[0] = 0xe8;
	HMODULE hModule = ::GetModuleHandle(TEXT("iTunes.dll"));
	if (!hModule)
	{
		return;
	}

	int serialNumAddrPoint = getSerialNumHookPoint((char*)hModule);
	//保存旧的hook数据
	g_oldHookData.serialNumberData.dwOldHookPoint = serialNumAddrPoint;
	memcpy(g_oldHookData.serialNumberData.OldbyteValue,(PVOID)serialNumAddrPoint,6);
	//计算跳转地址的后四字节,后四字节 = 目的地址 - 当前地址 - 5
	DWORD fourByte = (DWORD)CustomGetVolumeInformation - serialNumAddrPoint - 5;

	for (int i=0; i<4; i++)
	{
		jmpAddr[i+1] = *((char*)&fourByte + i);
	}
	jmpAddr[5] = 0x90;
	//替换
	DWORD oldProtect;
	VirtualProtect((LPVOID)serialNumAddrPoint,6,PAGE_READWRITE,&oldProtect);

	memcpy((PVOID)serialNumAddrPoint,jmpAddr,6);

	VirtualProtect((LPVOID)serialNumAddrPoint,6,oldProtect,&oldProtect);

	return;

}


void RestoreMacAddressValue(void)
{
	DWORD oldProtect;
	VirtualProtect((LPVOID)g_oldHookData.macAddrData.dwOldHookPoint,6,PAGE_READWRITE,&oldProtect);
	memcpy((LPVOID)g_oldHookData.macAddrData.dwOldHookPoint,g_oldHookData.macAddrData.OldbyteValue,6);
	VirtualProtect((LPVOID)g_oldHookData.macAddrData.dwOldHookPoint,6,oldProtect,&oldProtect);
	return;
}


void RestoreSerialNumValue(void)
{
	DWORD oldProtect;
	VirtualProtect((LPVOID)g_oldHookData.serialNumberData.dwOldHookPoint,6,PAGE_READWRITE,&oldProtect);
	memcpy((LPVOID)g_oldHookData.serialNumberData.dwOldHookPoint,g_oldHookData.serialNumberData.OldbyteValue,6);
	VirtualProtect((LPVOID)g_oldHookData.serialNumberData.dwOldHookPoint,6,oldProtect,&oldProtect);
	return;
}