#include "stdafx.h"
#include "enterpoint.h"


BYTE oldBytes[16] = {0};

void InitItunes(HMODULE hModule)
{

	LARGE_INTEGER PerformanceCount;
	ULONG LowPart;
    LONG HighPart;

	QueryPerformanceCounter(&PerformanceCount);

	LowPart = PerformanceCount.LowPart;
	HighPart = PerformanceCount.HighPart;

	char itunesPath[] = "C:\\Program Files\\iTunes\\iTunes.exe";
	int length = strlen(itunesPath);
	char* pEnd = &itunesPath[0] + length;

	HMODULE hExeMod = ::GetModuleHandle(NULL);

	int iTunesMainEnterPoint = (int)::GetProcAddress(hModule,"_iTunesMainEntryPoint@24");
	
	//给itunes中的iTunesMainEnterPoint函数打内存补丁
	PatchItunes(iTunesMainEnterPoint);
	//调用iTunesMainEnterPoint函数
	_asm
	{
		mov eax,HighPart
		mov ecx,LowPart
		push eax
		push ecx
		push 0xa
		mov edx,pEnd
		push edx
		push 0
		mov edx,hExeMod
		push edx
		mov edx,iTunesMainEnterPoint
		call edx

	}

}

void PatchItunes(int lpAddress)
{
	DWORD oldProtect;

	BYTE patchBytes[16] = {0x33,0xC0,0x85,0xF6,0x0F,0x95,0xC0,0x5F,0x5E,0x5B,0x8B,0xE5,0x5D,0xC2,0x18,0x00};
	BYTE patchByte2[1] = {0x84};

	HMODULE hModule = ::GetModuleHandle(_T("itunes.dll"));

	int addrHookPoint1 = getHookPoint1(lpAddress);

	int addrHookPoint2 = getHookPoint2((char*)hModule);

	///第一个补丁
	VirtualProtect((LPVOID)addrHookPoint1,16,PAGE_READWRITE,&oldProtect);
	//保存原始字节码
	memcpy((LPVOID)oldBytes,(LPVOID)addrHookPoint1,16);
	//复制新的字节码
	memcpy((LPVOID)addrHookPoint1,(LPVOID)patchBytes,16);

	VirtualProtect((LPVOID)addrHookPoint1,16,oldProtect,&oldProtect);

	///第二个补丁只需要把0x85修改成0x84即可
	VirtualProtect((LPVOID)addrHookPoint2,1,PAGE_READWRITE,&oldProtect);

	memcpy((LPVOID)addrHookPoint2,patchByte2,1);

	VirtualProtect((LPVOID)addrHookPoint2,1,oldProtect,&oldProtect);


}

void restorePatch()
{

}

int getHookPoint1(int lpAddr)
{
	int hookpoint;
	for (int i=lpAddr;; i++)
	{
		if (*(int*)i == 0xCF8B078B &&
			*(int*)(i+4) == 0xFF0875FF &&
			*(int*)(i+8) == 0x078B6050 &&
			*(int*)(i+12) == 0x75FFCF8B &&
			*(int*)(i+16) == 0x6450FF14 &&
			*(int*)(i+20) == 0xCF8B078B &&
			*(int*)(i+24) == 0x8B5C50FF
			)
		{
			hookpoint = i + 27;
			break;
		}
	}

	return hookpoint;
}
int getHookPoint2(char* hModule)
{
	HANDLE v1; 
	int result; 
	unsigned int i; 
	struct _MODULEINFO modinfo;

	v1 = GetCurrentProcess();
	if ( GetModuleInformation(v1,(HMODULE)hModule, &modinfo, 0xCu) == TRUE )
	{
		for ( i = (unsigned int)hModule; i < (unsigned int)&hModule[modinfo.SizeOfImage - 4096]; ++i )
		{
		  if ( *(DWORD*)i == 0xCE8B31FF &&
			   *(DWORD*)(i+4) == 0xE87477FF &&
			   *(DWORD*)(i+48) == 0xCF8B078B &&
			   *(DWORD*)(i+52) == 0x8B4850FF &&
			   *(DWORD*)(i+56) == 0xE8D88BCF)
		  {
			result = i - 54;
			break;
		  }

		}// end for
	  
	}//end if
	return result;

}