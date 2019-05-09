#include "stdafx.h"
#include "hook.h"
#include <Iphlpapi.h>
#pragma comment(lib,"Iphlpapi.lib")

OLDHOOKDATA g_oldHookData = {0};

int getMacAddrHookPoint(char* hModule)
{
	return 0;
}

int getSerialNumHookPoint(char* hModule)
{
	return 0;
}

DWORD WINAPI customGetAdapterInfo(__out PIP_ADAPTER_INFO pAdapterInfo,PULONG pOutBufLen)
{
	DWORD nRet = GetAdaptersInfo(pAdapterInfo,pOutBufLen);
	if (nRet == ERROR_SUCCESS)
	{
	}
	return nRet;
}

void ModifyMacAddressValue(void)
{
	char jmpAddr[6] = {0};
	jmpAddr[0] = 0xe8;
	HMODULE hModule = ::GetModuleHandle(TEXT(""));
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

	BOOL bRet = GetVolumeInformation(lpRootPathName,
									 lpVolumeNameBuffer,
									 nVolumeNameSize,
									 lpVolumeSerialNumber,
									 lpMaximumComponentLength,
									 lpFileSystemFlags,
									 lpFileSystemNameBuffer,
									 nFileSystemNameSize);

	if (bRet)
	{

	}

	return bRet;
}


void ModifySerialNumValue(void)
{
	char jmpAddr[6] = {0};
	jmpAddr[0] = 0xe8;
	HMODULE hModule = ::GetModuleHandle(TEXT(""));
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