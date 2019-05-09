// xhiTunes.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"

#define BRANK_API extern "C" _declspec(dllexport)

#include "xhiTunes.h"
#include "enterpoint.h"
#include "cryptAlgorithm.h"
#include "hook.h"
#include "gsaservices.h"
#include "base64.h"

typedef BOOL (WINAPI *LPFN_ISWOW64PROCESS) (HANDLE, PBOOL);

LPFN_ISWOW64PROCESS fnIsWow64Process;

BOOL IsWow64()
{
    BOOL bIsWow64 = FALSE;

    fnIsWow64Process = (LPFN_ISWOW64PROCESS)GetProcAddress(
        GetModuleHandle(TEXT("kernel32")),"IsWow64Process");
  
    if (NULL != fnIsWow64Process)
    {
        if (!fnIsWow64Process(GetCurrentProcess(),&bIsWow64))
        {
            // handle error
        }
    }
    return bIsWow64;
}


#define  SUPPORTEDPATH32        TEXT("C:\\Program Files\\Common Files\\Apple\\Apple Application Support\\")
#define  SUPPORTEDMOBILEPATH32  TEXT("C:\\Program Files\\Common Files\\Apple\\Mobile Device Support\\")
#define  STRITUNESPATH32        TEXT("C:\\Program Files\\iTunes\\")

#define  SUPPORTEDPATH64        TEXT("C:\\Program Files (x86)\\Common Files\\Apple\\Apple Application Support\\")
#define  SUPPORTEDMOBILEPATH64  TEXT("C:\\Program Files (x86)\\Common Files\Apple\\Mobile Device Support\\")
#define  STRITUNESPATH64        TEXT("C:\\Program Files (x86)\\iTunes\\")


///////////////////////////////////////////////////////////定义全局常量,变量
char kbValue[] = {0x41,0x42,0x43,0x44,0x45,0x46,0x47,0x48,0x49,0x4a,0x4b,0x4c,
				   0x4d,0x4e,0x4f,0x50,0x51,0x52,0x53,0x54,0x55,0x56,0x57,0x58,
				   0x59,0x5a,0x61,0x62,0x63,0x64,0x65,0x66,0x67,0x68,0x69,0x6a,
				   0x6b,0x6c,0x6d,0x6e,0x6f,0x70,0x71,0x72,0x73,0x74,0x75,0x76,
				   0x77,0x78,0x79,0x7a,0x30,0x31,0x32,0x33,0x34,0x35,0x36,0x37,
				   0x38,0x39,0x2b,0x2f};


char strCheck[] = {0x0ff,0x0ff,0x0ff,0x0ff,0x0ff,0x0ff,0x0ff,0x0ff,0x0ff,0x0ff,
				   0x0ff,0x0ff,0x0ff,0x0ff,0x0ff,0x0ff,0x0ff,0x0ff,0x0ff,0x0ff,
				   0x0ff,0x0ff,0x0ff,0x0ff,0x0ff,0x0ff,0x0ff,0x0ff,0x0ff,0x0ff,
				   0x0ff,0x0ff,0x0ff,0x0ff,0x0ff,0x0ff,0x0ff,0x0ff,0x0ff,0x0ff,
                   0x0ff,0x0ff,0x0ff,0x3e,0x0ff,0x0ff,0x0ff,0x3f,0x34,0x35,0x36,
                   0x37,0x38,0x39,0x3a,0x3b,0x3c,0x3d,0x0ff,0x0ff,0x0ff,0x00,0x0ff,
				   0x0ff,0x0ff,0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,
                   0x0a,0x0b,0x0c,0x0d,0x0e,0x0f,0x10,0x11,0x12,0x13,0x14,0x15,0x16,
                   0x17,0x18,0x19,0x0ff,0x0ff,0x0ff,0x0ff,0x0ff,0x0ff,0x1a,0x1b,0x1c,
                   0x1d,0x1e,0x1f,0x20,0x21,0x22,0x23,0x24,0x25,0x26,0x27,0x28,0x29,0x2a,
                   0x2b,0x2c,0x2d,0x2e,0x2f,0x30,0x31,0x32,0x33,0x0ff,0x0ff,0x0ff,
				   0x0ff,0x0ff,0x0ff,0x0ff,0x0ff,0x0ff,0x0ff,0x0ff,0x0ff,0x0ff,
                   0x0ff,0x0ff,0x0ff,0x0ff,0x0ff,0x0ff,0x0ff,0x0ff,0x0ff,0x0ff,
                   0x0ff,0x0ff,0x0ff,0x0ff,0x0ff,0x0ff,0x0ff,0x0ff,0x0ff,0x0ff,
                   0x0ff,0x0ff,0x0ff,0x0ff,0x0ff,0x0ff,0x0ff,0x0ff,0x0ff,0x0ff,
				   0x0ff,0x0ff,0x0ff,0x0ff,0x0ff,0x0ff,0x0ff,0x0ff,0x0ff,0x0ff,
				   0x0ff,0x0ff,0x0ff,0x0ff,0x0ff,0x0ff,0x0ff,0x0ff,0x0ff,0x0ff,
				   0x0ff,0x0ff,0x0ff,0x0ff,0x0ff,0x0ff,0x0ff,0x0ff,0x0ff,0x0ff,
				   0x0ff,0x0ff,0x0ff,0x0ff,0x0ff,0x0ff,0x0ff,0x0ff,0x0ff,0x0ff,
				   0x0ff,0x0ff,0x0ff,0x0ff,0x0ff,0x0ff,0x0ff,0x0ff,0x0ff,0x0ff,
				   0x0ff,0x0ff,0x0ff,0x0ff,0x0ff,0x0ff,0x0ff,0x0ff,0x0ff,0x0ff,
				   0x0ff,0x0ff,0x0ff,0x0ff,0x0ff,0x0ff,0x0ff,0x0ff,0x0ff,0x0ff,
				   0x0ff,0x0ff,0x0ff,0x0ff,0x0ff,0x0ff,0x0ff,0x0ff,0x0ff,0x0ff,
				   0x0ff,0x0ff,0x0ff,0x0ff,0x0ff,0x0ff,0x0ff,0x0ff,0x0ff,0x0ff};

char strBoundary[] = {0x30,0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38,0x39,0x41,0x42,0x43,0x44,0x45,0x46};

int g_firstkbsyncParamFunAddr = 0;

HMODULE g_hModule = NULL;
CRITICAL_SECTION g_cs;
DWORD g_paramAddr_1;
static int g_itune8ParamAddr = 0;
char* pvcRM = NULL;
BOOL g_bRegister = FALSE;
static int g_hw_info[6] = {0};
static int g_sessionId = 0;

///////////////////////////////////////////////////////////函数声明
BOOL InitializeSDKManager(void);//初始化sdk管理器，调用了很多sdk里面的函数
int GetFunAddr1(char *hModule);//sub_1084EEE0
int GetFunAddr2(char *hModule);//sub_1084F000

int GetFunAddr4(char *hModule);//sub_10DEF1D0

int GetvcRMAddr(char *hModule);//sub_10862D30



int GetFunctionAddress_1(char *hModule);//返回的实际数据一个函数的地址值，通过调用这个函数地址，得到一个重要的参数
int GetFunctionAddress_2(char *hModule);
BOOL generateSecondThirdData(char* lpDsid,int* paramData);
void getKbsyncFirstParameterValue(void);
//得到生成kbsync算法的第二个参数的值的函数地址
int AllMulAddr(char* hModule);///其实这个就是得到的kbsync的第二个参数是dsid的值的16进制，第三个参数是一个整数
int getGenerateMD_AMDAlgorithmAddress(char* hModule);
int getMDInitAlgorithmAddress(char* hModule);
int getItunesBase64AlgorithmAddress(char* hModule);
int getItunesBase64AllocateMemAddress(char* hModule);
int getMDSecondInitAlgorithmAddress(char* hModule);
int getFreeMD_AMD_AlgorithmAddress(char* hModule);

void itunesBase64(char* lpSrcData,int srcLen,char* lpOutData);

///////////////////////////////////////////////////////////////////////
//**************得到生成kbsync的第一个参数的函数地址******************/
int getFirstgenerateParamAddress(char* hModule);
int getSecondgenerateParamAddress(char* hModule);
int getgenerateFirstKbsyncParamAddress(char* hModule);
///////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////
//*************生成X-JS-SP-TOKEN需要的函数地址************************/
int getFirstGenerateAlgorithmAddr(char* hModule);
int getSecondGenerateAlgorithmAddr(char* hModule);
int getBase64Addr(char* hModule);
///////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////
//*************生成checkacksar和rbsync的函数地址*********************//
int getGenerateCheckacksarAlgorithmAddr(char* hModule);
int getGenerateRbsyncAlgorithmAddr(char* hModule);
int releaseAllocateAddr(char* hModule);
///////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////
/*************生成sbsync值的算法函数地址******************************/
int getGenerateSbsyncValueAddr(char* hModule);
///////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////
//******************生成mid和otp的函数地址***************************//
int getGenerateMidandOtpAlgorithmAddr(char* hModule);
int releaseMidandOtpMemAddr(char* hModule);
///////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////
//******************生成clientData的函数地址*************************//
int getGenerateClientDataAlgorithmAddr(char* hModule);
int getReleaseClientDataMemAddr(char* hModule);
///////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////
//*******************gsa机器验证算法函数地址*************************//
int getFirstAlgorithmExchangeDataAddr(char* hModule);
int getSetMachineProvisionDataAddr(char* hModule);
///////////////////////////////////////////////////////////////////////


unsigned int GetFunctionAddress_3(char *hModule);
int NewItunesFunctionAddress_1(char *hModule);			 //100827E4
int NewItunesFunctionAddress_2(HMODULE hModule, int& a2);//100827E0
int NewItunesFunctionAddress_3(char *hModule);			 //100827E8

//注册的时候需要调用的两个函数,这两个函数实际上是iTunes里的函数（也就是函数所在PE文件的地址）
int NewItunesRegistyAddress_1(char* hModule);
int NewItunesRegistyAddress_2(char* hModule);
int NewItunesRegistyAddress_3(char* hModule);
int NewITunesEncyptRegisty_Address_4(char* hModule);
int NewItunesEncyptMainRegistyParam_Addr_5(char* hModule);
int iTunesRegisterSecondXAppleSignatureAddr_1(char* hModule);
int iTunesRegisterSecondXAppleSignatureAddr_2(char* hModule);
int iTunesRegisterSecondXAppleSignatureAddr_3(char* hModule);
int iTunesRegisterSecondGenerateAlgorithmFun(char* hModule);
int iTunesRegisterDealFunAfterXSignature(char* hModule);
int Get8ParamterDeformAlgorithmFun(char* hModule);
int GetgenerateBoundaryRandomAddr(char* hModule);
int KeAllocateFunAddr(char* hModule);
int KeDeAllocateFunAddr(char* hModule);
int KeAllocateRealocateFunAddr(char* hModule);
int KeDecodeGzipFunAddr(char* hModule);
int Generate_ic_info_value(char* hModule);

////得到注册时，进入继续页面后，服务器返回的X-Apple-ActionSignature的值进行变形的算法函数地址
int GetRegistySingatureDeformAlgorithmAddr(char* hModule);
int GetFreeMemoryAlgorithmAddr(char* hModule);
/////OutBufParam是一个重要的输出参数，_size是该生成的大小，这个参数是是注册之前生成X-Apple-Signature的一个重要值
int GetRegisterMainParameter();

char* generateMachineGuid(int Address);
int GetArgument(int _Address_1);
////////////////////////////////////////////////////////
//登陆操作生成的kbsync dsid为空 flag＝0xB
//购买操作生成的kbsync dsid有值 flag＝0x1
int GetGenerateKbsyncPargam(int& _OutParam_1,int& _OutParam_2,long _dsid,int thirdParam,int _address_2,int flag);//_address_1为GetArgumnet函数的返回结果
////////////////////////////////////////////////////////
int GenerateRealKbsyncValue(int a1, int a2, int a3);
int GetEachChar(int a1, int a2, signed int a3);
int GetGenerateSignSapParam(int& _OutParam_1,int& _OutParam_2);
int SignSapCertSignature(int a1, int a2, int a3);
/////注册时第一次需要发送的sign-sap-setup-buffer的生成算法
int AsmGenerateRegistyFirstkeyValue(int a1, int a2,char* a3);

///////////////////////////////////////////////////////////////////////////////////对外接口函数定义


///////////////////////////////注册的时候需要生成signature算法////////////////////////////////////////////////////////////////////
int EncyptFunForRegistySignature(int a1, int a2, int a3, int a4)
{
  int v4; 
  int v5; 
  int result; 
  unsigned int v7; 
  unsigned int v8;
  int v9; 
  char v10; 
  unsigned int v11;
  int v12; 
  int v13; 
  char v14[68]; 
  unsigned int v15; 
  int v16; 
  char aAbcdefghijklmn[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
  v15 = (unsigned int)&v16 ^ 0x0BB40E64E;
  memcpy(v14, "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/", 0x40u);
  v14[64] = aAbcdefghijklmn[64];
  v4 = 0;
  v5 = a1;
  v13 = a2;
  if ( (a1 || !a2) && a3 && ((unsigned int)a2 <= 0xBFFFFFFD ? (v7 = 4 * (a2 + 2) / 3u) : (v7 = -1), a4 >= v7) )
  {
    if ( (unsigned int)a2 >= 3 )
    {
      v8 = a2 / 3u;
      do
      {
        v9 = *(BYTE *)(v5 + 2) | ((*(BYTE *)(v5 + 1) | (*(BYTE *)v5 << 8)) << 8);
        *(BYTE *)(a3 + v4) = v14[((*(BYTE *)(v5 + 2) | ((*(BYTE *)(v5 + 1) | ((unsigned int)*(BYTE *)v5 << 8)) << 8)) >> 18) & 0x3F];
        v13 -= 3;
        *(BYTE *)(a3 + v4 + 1) = v14[((unsigned int)v9 >> 12) & 0x3F];
        v10 = v14[((unsigned int)v9 >> 6) & 0x3F];
        //LOBYTE(v9) = v14[v9 & 0x3F];
		LOBYTE(v9);
		v9 = v14[v9 & 0x3F];
        *(BYTE *)(a3 + v4 + 2) = v10;
        *(BYTE *)(a3 + v4 + 3) = v9;
        v4 += 4;
        v5 += 3;
        --v8;
      }
      while ( v8 );
    }
    if ( v13 )
    {
      v11 = *(BYTE *)v5 << 16;
      if ( v13 == 2 )
        v11 |= *(BYTE *)(v5 + 1) << 8;
      *(BYTE *)(a3 + v4) = v14[(v11 >> 18) & 0x3F];
      *(BYTE *)(a3 + v4 + 1) = v14[(v11 >> 12) & 0x3F];
      if ( v13 == 2 )
        *(BYTE *)(a3 + v4 + 2) = v14[(v11 >> 6) & 0x3F];
      else
        *(BYTE *)(a3 + v4 + 2) = 61;
      v12 = v4 + 3;
      *(BYTE *)(a3 + v12) = 61;
      v4 = v12 + 1;
    }
    result = v4;
  }
  else
  {
    result = 0;
  }
  return result;
}

BOOL InitializeModule(LPVOID lpBuffer)
{
	char key[] = "xhiTunes";
	char srcData[128] = {0};
	TCHAR dwData[MAX_PATH] = {0};
	TCHAR PathBuffer[MAX_PATH];
	TCHAR EnvBuffer[32768];
	DWORD dwNum = 0;
	
	memset(PathBuffer,0x0000,sizeof(PathBuffer));
	
	UINT pathLength = GetWindowsDirectory(PathBuffer,MAX_PATH);
	
	if (pathLength)
	{
		dwNum = GetEnvironmentVariable(TEXT("PATH"),EnvBuffer,32768);
		if (!dwNum)
		{
			return FALSE;
		}

		CString strEnvironmentPath(EnvBuffer);
		if (IsWow64())
		{
			CString strAppSupportPath(SUPPORTEDPATH64);
			CString strAppMobilePath(SUPPORTEDMOBILEPATH64);
			CString strItunesPath(STRITUNESPATH64);

			strAppSupportPath += _T(";");
			strAppSupportPath += strAppMobilePath;
			strAppSupportPath += _T(";");
			strAppSupportPath += strItunesPath;
			strAppSupportPath += _T(";");
			strAppSupportPath += strEnvironmentPath;
			
			BOOL bSuccess = SetEnvironmentVariable(_T("PATH"),strAppSupportPath);
			if (!bSuccess)
			{
				return FALSE;
			}
		}
		else
		{
			CString strAppSupportPath(SUPPORTEDPATH32);
			CString strAppMobilePath(SUPPORTEDMOBILEPATH32);
			CString strItunesPath(STRITUNESPATH32);

			strAppSupportPath += _T(";");
			strAppSupportPath += strAppMobilePath;
			strAppSupportPath += _T(";");
			strAppSupportPath += strItunesPath;
			strAppSupportPath += _T(";");
			strAppSupportPath += strEnvironmentPath;
			
			BOOL bSuccess = SetEnvironmentVariable(_T("PATH"),strAppSupportPath);
			if (!bSuccess)
			{
				return FALSE;
			}
		}

		HMODULE hKModule = GetModuleHandle(_T("Kernel32.dll"));
		if (!hKModule)
		{
			return FALSE;
		}
		BOOL bSet = SetDllDirectory(_T(""));
		if (!bSet)
		{
			return FALSE;
		}
		
		//解密字符串
		strcpy(srcData,(char*)lpBuffer);
		KDecodeData(srcData,key);
		MultiByteToWideChar(CP_ACP,0,srcData,-1,dwData,MAX_PATH);
		
		g_hModule = LoadLibrary(dwData);
		if (!g_hModule)
		{
			return FALSE;
		}
		else
		{
			g_bRegister = TRUE;
		}
		//hookmac地址的函数和SerialNumber的函数值
		ModifyMacAddressValue();
		ModifySerialNumValue();

		int ic_Info = Generate_ic_info_value((char*)g_hModule);
		

	}
	else
	{
		return FALSE;
	}
test_1:
	
	InitializeCriticalSection(&g_cs);
	return TRUE;
}

int Generate_ic_info_value(char* hModule)
{
	///获取得到生成kbsync的第一个参数值
	int firstgenerateParamAddr = getFirstgenerateParamAddress((char*)hModule);
	int secondgenerateParamAddr = getSecondgenerateParamAddress((char*)hModule);
	int generateFirstKbsyncParamAddr = getgenerateFirstKbsyncParamAddress((char*)hModule);
	char strSCInfoPath[] = "C:\\ProgramData\\Apple Computer\\iTunes\\SC Info";
	char firstParam[MAX_PATH] = {0};
	char secondParam[MAX_PATH] = {0};
	int outParam = 0;
	_asm
	{
		lea ecx,firstParam[0]
		mov edx,firstgenerateParamAddr
		call edx
	}
	_asm
	{
		lea ecx,secondParam[0]
		mov edx,secondgenerateParamAddr
		call edx
	}
	
	_asm
	{
		lea edi,outParam
		push edi
		lea eax,strSCInfoPath[0]
		push eax
		lea eax,secondParam[0]
		push eax
		lea eax,firstParam[0]
		push eax
		mov edx,generateFirstKbsyncParamAddr
		call edx
		add esp,0x10
	}

	g_firstkbsyncParamFunAddr = outParam;//计算kbsync和sbsync的一个魔术，有些人表示成了ic_info

	return g_firstkbsyncParamFunAddr;
}

void getKbsyncFirstParameterValue(void)
{
	HMODULE hModule = NULL;
	hModule = ::GetModuleHandle(_T("iTunesCore.dll"));
	if (!hModule)
	{
		return;
	}

	///获取得到生成kbsync的第一个参数值
	int firstgenerateParamAddr = getFirstgenerateParamAddress((char*)hModule);
	int secondgenerateParamAddr = getSecondgenerateParamAddress((char*)g_hModule);
	int generateFirstKbsyncParamAddr = getgenerateFirstKbsyncParamAddress((char*)hModule);
	char strSCInfoPath[] = "C:\\ProgramData\\Apple Computer\\iTunes\\SC Info";
	char firstParam[MAX_PATH] = {0};
	char secondParam[MAX_PATH] = {0};
	int outParam = 0;
	_asm
	{
		lea ecx,firstParam[0]
		mov edx,firstgenerateParamAddr
		call edx
	}
	_asm
	{
		lea ecx,secondParam[0]
		mov edx,secondgenerateParamAddr
		call edx
	}
	
	_asm
	{
		lea edi,outParam
		push edi
		lea eax,strSCInfoPath[0]
		push eax
		lea eax,secondParam[0]
		push eax
		lea eax,firstParam[0]
		push eax
		mov edx,generateFirstKbsyncParamAddr
		call edx
		add esp,0x10
	}

	g_firstkbsyncParamFunAddr = outParam;
}

BOOL generateX_JS_SP_TOKEN_Value(IN LPVOID lpGsaServices,LPVOID lpSrcData,int dataLen,LPVOID lpOutData)
{
	HMODULE hModule = NULL;
	char szData[MAX_PATH] = {0};
	char szBase64[MAX_PATH] = {0};

	CGsaservices* pGsaObj = (CGsaservices*)lpGsaServices;
	if (!pGsaObj)
	{
		return FALSE;
	}

	hModule = ::GetModuleHandle(_T("iTunesCore.dll"));
	if (!hModule)
	{
		return FALSE;
	}

	//获得生成X-JS-SP-TOKEN需要调用的函数地址
	int address1 = getFirstGenerateAlgorithmAddr((char*)hModule);
	int address2 = getSecondGenerateAlgorithmAddr((char*)hModule);
	int base64addr = getBase64Addr((char*)hModule);

	_asm
	{
		mov eax,dataLen
		lea ecx,szData[0]
		push ecx
		push eax
		mov edx,lpSrcData
		mov ecx,2
		call address1
		add esp,0x8

		lea eax,szData[8]
		push eax
		push 2
		call address2
		add esp,0x8

		lea ecx,szData[8]
		lea eax,szBase64[0]
		mov edx,0x10
		push 0x0ff
		push eax
		call base64addr
		add esp,0x8

	}

	memcpy(lpOutData,szBase64,strlen(szBase64));

	return TRUE;
}

BOOL generateSPXAppleActionSignature(IN LPVOID lpGsaServices,
									 IN LPVOID lpDsid,
									 IN LPVOID lpSaleId,
									 IN LPVOID lpTimeStamp,
									 IN LPVOID lpXAppleStoreFront,
									 IN LPVOID lpSignData,
									 IN int	signLength,
									 OUT LPVOID lpOutSignature,
									 OUT int& outSignLength,
									 IN BOOL bHighVersion)
{
	char	szRecvSignBuf[2048] = {0};
	char	sendBuf[512] = {0};
	int		sendLength = 0;
	int		recvLength = 0;
	//组装数据
	strcat(sendBuf,(char*)lpDsid);
	strcat(sendBuf,(char*)lpXAppleStoreFront);
	strcat(sendBuf,(char*)lpTimeStamp);
	if (bHighVersion)
	{
		strcat(sendBuf,"+0800");
	}
	else
	{
		strcat(sendBuf," +0800");
	}
	strcat(sendBuf,"offer");
	strcat(sendBuf,(char*)lpSaleId);

	sendLength = strlen(sendBuf);

	//计算签名值
	int res = GetSignsapSetupBuffer(lpGsaServices,
									lpSignData,
									signLength,
									szRecvSignBuf,
									recvLength,
									TRUE,
									sendBuf,
									sendLength);

	memcpy(lpOutSignature,szRecvSignBuf,recvLength);

	return TRUE;
}

BOOL generateCheckinacksarValue(IN LPVOID lpGsaServices,
								IN LPVOID lpRentalbagResponse,
								int bagResLen,
								IN LPVOID lpCheckinsar,
								int checksarLen,
								OUT LPVOID lpCheckacksar)
{
	CGsaservices* pGsaObj = (CGsaservices*)lpGsaServices;
	if (!pGsaObj)
	{
		return FALSE;
	}
	char szCheckinacksar[MAX_PATH] = {0};
	int checkinacksar = 0;
	int checkinacksarLen = 0;
	int decodeRentalBagDataLen = 0;
	int decodeCheckinsarLen = 0;

	HMODULE hModule = NULL;
	hModule = ::GetModuleHandle(_T("iTunes.dll"));
	if (!hModule)
	{
		return FALSE;
	}

	if (!lpRentalbagResponse || !lpCheckinsar)
	{
		return FALSE;
	}

	int checkacksarAlgorithmAddr = getGenerateCheckacksarAlgorithmAddr((char*)hModule);
	int relesAllocateAddr = releaseAllocateAddr((char*)hModule);

	char* lpDecodeRentalBag = (char*)VirtualAlloc(NULL,1024*4,MEM_COMMIT | MEM_RESERVE,PAGE_EXECUTE_READWRITE);
	if (!lpDecodeRentalBag)
	{
		return FALSE;
	}

	char* lpDecodeCheckinsar = (char*)VirtualAlloc(NULL,512,MEM_COMMIT | MEM_RESERVE,PAGE_EXECUTE_READWRITE);
	if (!lpDecodeCheckinsar)
	{
		return FALSE;
	}

	//base64解码原始数据
	decodeRentalBagDataLen = SignSapCertSignature((int)lpRentalbagResponse,bagResLen,(int)lpDecodeRentalBag);
	decodeCheckinsarLen = SignSapCertSignature((int)lpCheckinsar,checksarLen,(int)lpDecodeCheckinsar);

	_asm
	{
		lea ecx,checkinacksarLen
		push ecx
		lea ecx,checkinacksar
		push ecx
		mov edx,decodeCheckinsarLen
		push edx
		mov edx,lpDecodeCheckinsar
		push edx
		mov eax,decodeRentalBagDataLen
		push eax
		mov eax,lpDecodeRentalBag
		push eax
		mov edx,g_firstkbsyncParamFunAddr
		push edx
		call checkacksarAlgorithmAddr
		add esp,0x1c
	}

	//base64计算
	int res = GenerateRealKbsyncValue(checkinacksar,checkinacksarLen,(int)szCheckinacksar);

	memcpy(lpCheckacksar,szCheckinacksar,strlen(szCheckinacksar));

	//释放
	_asm
	{
		push checkinacksar
		call relesAllocateAddr
		add esp,0x4
	}

	if (lpDecodeRentalBag)
	{
		VirtualFree(lpDecodeRentalBag,0,MEM_RELEASE);
		lpDecodeRentalBag = NULL;
	}

	if (lpDecodeCheckinsar)
	{
		VirtualFree(lpDecodeCheckinsar,0,MEM_RELEASE);
		lpDecodeCheckinsar = NULL;
	}

	return TRUE;
}


BOOL generateRbsyncValue(IN LPVOID lpGsaServices,OUT LPVOID lpRbsyncValue)
{
	CGsaservices* pGsaObj = (CGsaservices*)lpGsaServices;
	if (!pGsaObj)
	{
		return FALSE;
	}
	char szRbsyncValue[MAX_PATH] = {0};
	int rbsyncValue = 0;
	int rbsyncLen = 0;

	int param_9 = 0;
	int param_10 = 0;

	HMODULE hModule = NULL;
	hModule = ::GetModuleHandle(_T("iTunes.dll"));
	if (!hModule)
	{
		return FALSE;
	}

	int rbsyncAlgorithmAddr = getGenerateRbsyncAlgorithmAddr((char*)hModule);
	int relesAllocateAddr = releaseAllocateAddr((char*)hModule);

	_asm
	{
		lea ecx,param_10
		push ecx
		lea ecx,param_9
		push ecx
		lea ecx,rbsyncLen
		push ecx
		lea ecx,rbsyncValue
		push ecx
		push 0
		push 0x78
		push 0
		push 0
		push 0xc9
		mov edx,g_firstkbsyncParamFunAddr
		push edx
		call rbsyncAlgorithmAddr
		add esp,0x28

	}

	//base64计算
	int res = GenerateRealKbsyncValue(rbsyncValue,rbsyncLen,(int)szRbsyncValue);
	memcpy(lpRbsyncValue,szRbsyncValue,strlen(szRbsyncValue));

	//释放
	_asm
	{
		push rbsyncValue
		call relesAllocateAddr
		add esp,0x4
	}

	return TRUE;
}

BOOL generateClientData(IN LPVOID lpGsaServices,
						IN LPVOID lpXappleMdData,
						IN int xAppleMdDataLen,
						IN LPVOID lpDsid,
						OUT LPVOID lpClientData,
						OUT int& nvContext)
{
	char szClientData[512] = {0};
	int clientData = 0;
	int clientDataLen = 0;
	int decodeMdDataLen = 0;
	int kbsyncSecondThridParam[2] = {0};
	int param_7 = 0;
	HMODULE hModule = NULL;

	CGsaservices* pGsaObj = (CGsaservices*)lpGsaServices;
	if (!pGsaObj)
	{
		return FALSE;
	}

	hModule = ::GetModuleHandle(_T("iTunesCore.dll"));
	if (!hModule)
	{
		return FALSE;
	}

	if (!lpXappleMdData)
	{
		return FALSE;
	}

	int clientDataAlgorithmAddr = getGenerateClientDataAlgorithmAddr((char*)hModule);
	int releaseClientDataMemAddr  = getReleaseClientDataMemAddr((char*)hModule);

	char* lpDecodeMdData = (char*)VirtualAlloc(NULL,1024,MEM_COMMIT | MEM_RESERVE,PAGE_EXECUTE_READWRITE);
	if (!lpDecodeMdData)
	{
		return FALSE;
	}

	memset(lpDecodeMdData,0,1024);
	//解密原始数据
	decodeMdDataLen = SignSapCertSignature((int)lpXappleMdData,xAppleMdDataLen,(int)lpDecodeMdData);
	//去掉结束符,结束符'\0','ff','ff','ff'在该算法中必须去掉
	if (*(DWORD*)(lpDecodeMdData+decodeMdDataLen-4) == 0xffffff00)
	{
		decodeMdDataLen = decodeMdDataLen - 4;
	}
	else
	{
		decodeMdDataLen = decodeMdDataLen - 1;
	}
	EnterCriticalSection(&g_cs);
	if (lpDsid)
	{
		
		
		generateSecondThirdData((char*)lpDsid,kbsyncSecondThridParam);
		
		_asm
		{
			lea ecx,param_7
			push ecx
			lea ecx,clientDataLen
			push ecx
			lea ecx,clientData
			push ecx
			mov eax,decodeMdDataLen
			push eax
			mov edx,lpDecodeMdData
			push edx
			mov ecx,kbsyncSecondThridParam[1]
			push ecx
			mov ecx,kbsyncSecondThridParam[0]
			push ecx
			call clientDataAlgorithmAddr
			add esp,0x1c
		}
	}
	else
	{
		_asm
		{
			lea ecx,param_7
			push ecx
			lea ecx,clientDataLen
			push ecx
			lea ecx,clientData
			push ecx
			mov eax,decodeMdDataLen
			push eax
			mov edx,lpDecodeMdData
			push edx
			mov ecx,-1
			push ecx
			mov ecx,-1
			push ecx
			call clientDataAlgorithmAddr
			add esp,0x1c
		}
	}

	//base64加密
	int res = GenerateRealKbsyncValue(clientData,clientDataLen,(int)szClientData);

	memcpy(lpClientData,szClientData,strlen(szClientData));

	nvContext= param_7;

	//释放
	_asm
	{
		push clientData
		call releaseClientDataMemAddr
		add esp,0x4
	}
	LeaveCriticalSection(&g_cs);

	if (lpDecodeMdData)
	{
		VirtualFree(lpDecodeMdData,0,MEM_RELEASE);
		lpDecodeMdData = NULL;
	}

	return TRUE;
}

BOOL generateCpimData(IN LPVOID lpGsaServices,
					  IN LPVOID lpSpimData,
					  IN int spimLen,
					  OUT LPVOID lpCpimData,
					  OUT int& nvContext)
{
	char		szCpimData[512] = {0};
	HMODULE		hModule = NULL;
	int			res = 0;
	int			cpimLength = 0;
	int			cpimData = 0;
	int			decodeDataLen = 0;
	int			param_7 = 0;

	hModule = ::GetModuleHandle(_T("iTunesCore.dll"));
	if (!hModule)
	{
		return FALSE;
	}
	if (!lpSpimData)
	{
		return FALSE;
	}
	int algorithmAddr = getGenerateClientDataAlgorithmAddr((char*)hModule);
	int releaseMemAddr  = getReleaseClientDataMemAddr((char*)hModule);

	char* lpDecodeData = (char*)VirtualAlloc(NULL,512,MEM_COMMIT | MEM_RESERVE,PAGE_EXECUTE_READWRITE);
	if (!lpDecodeData)
	{
		return FALSE;
	}
	memset(lpDecodeData,0,512);
	//解密原始数据
	decodeDataLen = SignSapCertSignature((int)lpSpimData,spimLen,(int)lpDecodeData);
	//去掉结束符,结束符'\0'在该算法中必须去掉
	decodeDataLen = decodeDataLen - 1;

	EnterCriticalSection(&g_cs);
	_asm
	{
		lea ecx,param_7
		push ecx
		lea ecx,cpimLength
		push ecx
		lea ecx,cpimData
		push ecx
		mov eax,decodeDataLen
		push eax
		mov edx,lpDecodeData
		push edx
		mov ecx,-1
		push ecx
		mov ecx,-2
		push ecx
		call algorithmAddr
		add esp,0x1c
	}

	//base64加密
	res = GenerateRealKbsyncValue(cpimData,cpimLength,(int)szCpimData);

	memcpy(lpCpimData,szCpimData,strlen(szCpimData));

	nvContext = param_7;

	//释放
	_asm
	{
		push cpimData
		call releaseMemAddr
		add esp,0x4
	}
	LeaveCriticalSection(&g_cs);

	if (lpDecodeData)
	{
		VirtualFree(lpDecodeData,0,MEM_RELEASE);
		lpDecodeData = NULL;
	}
	return TRUE;
}

BOOL SetFinishProvisionData(IN LPVOID lpGsaServices,
							IN LPVOID lpTKTransportKeyData,
							IN int tkTransportKeyLen,
							IN LPVOID lpPtmSettingInfoData,
							IN int ptmSettingInfoLen,
							IN int nvContext,
							IN BOOL bGSAProvision)
{
	HMODULE		hModule = NULL;
	char		szTKTransportKeyData[MAX_PATH] = {0};
	int			decodeTKTransprotKeyLen = 0;
	int			decodePtmSettingInfoLen = 0;

	hModule = ::GetModuleHandle(_T("iTunesCore.dll"));
	if (!hModule)
	{
		return FALSE;
	}

	int exchangeDataAddr = getFirstAlgorithmExchangeDataAddr((char*)hModule);
	int machineProvisionDataAddr = getSetMachineProvisionDataAddr((char*)hModule);

	char* lpDecodeData = (char*)VirtualAlloc(NULL,512,MEM_COMMIT | MEM_RESERVE,PAGE_EXECUTE_READWRITE);
	if (!lpDecodeData)
	{
		return FALSE;
	}
	memset(lpDecodeData,0,512);
	//解密lpTKTransportKeyData原始数据
	decodeTKTransprotKeyLen = SignSapCertSignature((int)lpTKTransportKeyData,tkTransportKeyLen,(int)szTKTransportKeyData);
	if (decodeTKTransprotKeyLen > 0x10)
	{
		decodeTKTransprotKeyLen = 0x10;
	}
	if (decodeTKTransprotKeyLen < 0x10)
	{
		decodeTKTransprotKeyLen = 0x10;
	}
	//解密lpPtmSettingInfoData原始数据
	decodePtmSettingInfoLen = SignSapCertSignature((int)lpPtmSettingInfoData,ptmSettingInfoLen,(int)lpDecodeData);
	if (bGSAProvision)
	{
		//去掉结束符,结束符'\0'在该算法中必须去掉
		decodePtmSettingInfoLen = decodePtmSettingInfoLen - 1;
	}

	EnterCriticalSection(&g_cs);
	if (bGSAProvision)
	{
		_asm
		{
			push 0
			push 0x01050550  //这个值是通过ple-I-MD-RINFO的值计算出来的，实际上始终是一个常数
			push -1
			push -2
			call exchangeDataAddr
			add esp,0x10
		}
	}
	
	//设置机器授权验证的数据
	_asm
	{
		push decodeTKTransprotKeyLen
		lea edx,szTKTransportKeyData[0]
		push edx
		push decodePtmSettingInfoLen
		mov edx,lpDecodeData
		push edx
		mov ecx,nvContext
		push ecx
		call machineProvisionDataAddr
		add esp,0x14
	}
	LeaveCriticalSection(&g_cs);

	//释放内存
	if (lpDecodeData)
	{
		VirtualFree(lpDecodeData,0,MEM_RELEASE);
		lpDecodeData = NULL;
	}

	return TRUE;
}

LPVOID CreateGsaServicesObject()
{
	CGsaservices* pObject = new CGsaservices();
	if (!pObject)
	{
		return NULL;
	}
	return pObject;
}

VOID ReleaseGsaServicesObject(LPVOID lpGsaServices)
{
	CGsaservices* pObject = (CGsaservices*)lpGsaServices;
	if (pObject)
	{
		delete pObject;
		pObject = NULL;
	}
}

BOOL generateA2kValue(IN LPVOID lpGsaServices,OUT LPVOID lpA2kValue,OUT int& a2kDataLen)
{
	char localParam[8] = {0};
	int localParam1 = 0;
	int localParam_ebp_8 = 0;
	int localParam_ebp_20 = 0;
	int localAddr = 0;
	int localParam_ebp_1c = 0;
	char localParamA2k[MAX_PATH] = {0};
	char szS2k[] = "s2k";
	char szS2k_Info[] = "s2k_fo";
	char szDot[] = ",";
	char szVerticalLine[] = "|";
	int A2kDataLen = 0;
	char szA2kValue[512] = {0};
	LPVOID lpAllocPtr = NULL;
	LPVOID lpSaveAllocPtr = NULL;
	int res = 0;
	int s2kLen = strlen(szS2k);
	int s2k_InfoLen = strlen(szS2k_Info);

	CGsaservices* pGsaObj = (CGsaservices*)lpGsaServices;
	if (!pGsaObj)
	{
		return FALSE;
	}

	//获取需要调用的itunes中的函数地址
	int do_Call_addr = pGsaObj->getDo_Call_addr();
	int call_OnceEx_addr = pGsaObj->getCall_OnceEx_addr();
	int array1_addr = pGsaObj->getArray1_addr();						//dword_11C529AC
	int array2_addr = pGsaObj->getArray2_addr();						//off_117F481C
	int array3_addr = pGsaObj->getArray3_addr();						//unk_11C529C0
	int allocateMemory_addr = pGsaObj->getAllocatMemmory_addr();
	int encodeData1_addr = pGsaObj->getEncodeData1_addr();			//sub_112E0E00
	int encodeData2_addr = pGsaObj->getEncodeData2_addr();			//sub_112E1680
	int genA2kData_addr = pGsaObj->getGenerateA2kData_addr();			//sub_112DD530
	int arrayData1_addr = pGsaObj->getArrayData1_addr();				//11694290
	int arrayData2_addr = pGsaObj->getArrayData2_addr();				//11694B60
	int setData_addr = pGsaObj->getSetData_addr();					//sub_112D7B30
	int InitSetData_addr = pGsaObj->getInitSetData_addr();		    //sub_112D7AE0

	EnterCriticalSection(&g_cs);
	//分配空间
	/*_asm
	{
		push 0x6c
		push 1
		call allocateMemory_addr
		add esp,0x8
		mov lpSaveAllocPtr,eax

	}*/
	//begin  自己分配内存，不要调用itunes中的函数分配
	lpSaveAllocPtr = VirtualAlloc(NULL,0x6c,MEM_COMMIT | MEM_RESERVE,PAGE_EXECUTE_READWRITE);
	if (!lpSaveAllocPtr)
	{
		return FALSE;
	}
	memset(lpSaveAllocPtr,0,0x6c);
	//end
	pGsaObj->SetSaveAllocPtr(lpSaveAllocPtr);
	_asm
	{
		push lpSaveAllocPtr
		mov ecx,arrayData1_addr
		call InitSetData_addr
		add esp,0x4

		lea esi,szS2k[0]
		push esi
		push lpSaveAllocPtr
		mov edx,s2kLen
		mov ecx,arrayData1_addr
		call setData_addr
		add esp,0x8

		lea esi,szDot[0]
		push esi
		push lpSaveAllocPtr
		mov edx,1
		mov ecx,arrayData1_addr
		call setData_addr
		add esp,0x8

		lea esi,szS2k_Info[0]
		push esi
		push lpSaveAllocPtr
		mov edx,s2k_InfoLen
		mov ecx,arrayData1_addr
		call setData_addr
		add esp,0x8
	}
	
	memcpy(&localParam[0],&array2_addr,4);
	localParam_ebp_8 = array3_addr;
	localParam_ebp_20 = (int)&localParam_ebp_8;
	localAddr = (int)&localParam_ebp_20;
	memcpy(&localParam[4],&localAddr,4);

	_asm
	{
		lea eax,dword ptr localParam[0]
		push eax
		push do_Call_addr
		push array1_addr
		call call_OnceEx_addr
		add esp,0xc
		mov eax,array3_addr
		mov eax,[eax+4]
		mov localParam1,eax //ebp_10
	}

	//分配空间
	/*_asm
	{
		push 0x390
		push 1
		call allocateMemory_addr
		add esp,0x8
		mov esi,eax
		mov lpAllocPtr,eax
	}
	*/
	//begin 自己分配内存，不要调用itunes中的函数分配
	lpAllocPtr = VirtualAlloc(NULL,0x390,MEM_COMMIT | MEM_RESERVE,PAGE_EXECUTE_READWRITE);
	if (!lpAllocPtr)
	{
		return FALSE;
	}
	//end
	pGsaObj->SetAllocPtr(lpAllocPtr);
	memset(lpAllocPtr,0,0x390);
	_asm
	{
		mov esi,lpAllocPtr
		and dword ptr [esi+8],0xfffffffe
		mov ebx,arrayData1_addr
		mov [esi],ebx
		mov edi,arrayData2_addr
		mov [esi+4],edi
		or [esi+8],2
		mov eax,[esi+4]
		mov eax,[eax]
		shl eax,2
		mov localParam_ebp_1c,eax
	
		//call __alloca_probe_16
		mov eax,[esi+4]
		mov edx,localParam1
		//mov localParamA2k,esp

		push dword ptr [esi+4]
		mov eax,[eax]
		add eax,9
		lea ecx,[esi+eax*4]
		call encodeData1_addr
		add esp,0x4
		
		mov eax,[esi+4]
		lea edi,[esi+0x24]
		mov ecx,[eax]
		add ecx,9
		lea ecx,[esi+ecx*4]
		push ecx
		mov ecx,[eax]
		add eax,0x10
		push dword ptr [esi+4]
		lea edx,[eax+ecx*8]
		mov ecx,edi
		call encodeData2_addr
		mov eax,[esi+4]
		mov edx,edi
		lea esi,localParamA2k[0]//输出A2k的值
		push esi
		mov ecx,[eax]
		lea eax,ds:0[ecx*4]
		push eax
		call genA2kData_addr //计算A2k的算法函数
		add esp,0x10
	}

	//base64编码
	A2kDataLen = localParam_ebp_1c;
	res = GenerateRealKbsyncValue((int)localParamA2k,A2kDataLen,(int)szA2kValue);
	strcpy((char*)lpA2kValue,szA2kValue);
	a2kDataLen = strlen(szA2kValue);

	_asm
	{
		lea edi,szVerticalLine[0]
		push edi
		push lpSaveAllocPtr
		mov edx,1
		mov ecx,arrayData1_addr
		call setData_addr
		add esp,0x8

	}
	LeaveCriticalSection(&g_cs);

	return TRUE;
}


BOOL generateM1Value(IN LPVOID lpGsaServices,
					 IN LPVOID lpAppleId,
					 IN LPVOID lpPassword,
					 IN int dw_i_data,
					 IN LPVOID lp_s_data,
					 IN int s_data_Len,
					 IN LPVOID lp_B_data,
					 IN int B_data_Len,
					 OUT LPVOID lpM1,
					 OUT int& m1DataLen)
{
	char szString1[33] = {0}; //实际数据长度为32字节
	char szString2[33] = {0};//实际数据长度为32字节
	char szM1Value[33] = {0};
	char szBase64M1Value[MAX_PATH] = {0};
	char szDecodeBase64_s[33] = {0};
	char szDecodeBase64_B[512] = {0};
	char szS2k[] = "s2k";
	char szVerticalLine[] = "|";
	int s_Length = 0;
	int B_Length = 0;
	int strLen = 0x20;
	int res = 0;
	LPVOID lpAllocPtr = NULL;
	LPVOID lpSaveAllocPtr = NULL;

	CGsaservices* pGsaObj = (CGsaservices*)lpGsaServices;
	if (!pGsaObj)
	{
		return FALSE;
	}

	int s2kLen = strlen(szS2k);
	
	if (!lpAppleId)
	{
		return FALSE;
	}

	if (!lpPassword)
	{
		return FALSE;
	}

	int pwdLen = strlen((char*)lpPassword);
	
	int encodeString1_addr = pGsaObj->getEncodeString1_addr();						//sub_112D7A10,对密码第一次加密的函数地址
	int encodeString2_addr = pGsaObj->getEncodeString2_addr();						//sub_112DE340
	int generateM1Value_addr = pGsaObj->getGenerateM1Value_addr();					//sub_112DDE20
	int arrayData1_addr = pGsaObj->getArrayData1_addr();								//11694290
	int setData_addr = pGsaObj->getSetData_addr();									//sub_112D7B30

	//解密s的base64编码
	s_Length = SignSapCertSignature((int)lp_s_data,s_data_Len,(int)szDecodeBase64_s);
	s_Length = s_Length - 2;
	B_Length = SignSapCertSignature((int)lp_B_data,B_data_Len,(int)szDecodeBase64_B);
	B_Length = B_Length - 2;

	lpSaveAllocPtr = pGsaObj->GetSaveAllocPtr();

	EnterCriticalSection(&g_cs);
	_asm
	{
		lea esi,szVerticalLine[0]
		push esi
		push lpSaveAllocPtr
		mov edx,1
		mov ecx,arrayData1_addr
		call setData_addr
		add esp,0x8

		lea esi,szS2k[0]
		push esi
		push lpSaveAllocPtr
		mov ecx,arrayData1_addr
		call setData_addr
		add esp,0x8

	}

	//第一次加密
	_asm
	{
		mov edx,pwdLen
		lea eax,szString1[0]
		push eax
		push lpPassword
		mov ecx,arrayData1_addr
		call encodeString1_addr
		add esp,0x8
	}

	//设置保存第一次密码加密值
	pGsaObj->setEncodeString1(szString1);

	//第二次加密
	_asm
	{
		lea eax,szString2[0]
		push eax
		mov esi,arrayData1_addr
		push dword ptr [esi]
		push dw_i_data
		lea eax,szDecodeBase64_s[0]
		push eax
		mov edx,strLen
		mov ecx,arrayData1_addr
		push s_Length
		lea eax,szString1[0]
		push eax
		call encodeString2_addr
		add esp,0x18
	}

	lpAllocPtr = pGsaObj->GetAllocPtr();

	//生成M1
	_asm
	{
		lea eax,szM1Value[0]
		push eax
		lea eax,szDecodeBase64_B[0]
		push eax
		lea eax,szDecodeBase64_s
		push eax
		push s_Length
		lea eax,szString2[0]
		push eax
		mov ecx,lpAppleId
		mov edx,strLen
		push lpAllocPtr
		call generateM1Value_addr
		add esp,0x18

	}

	//base64编码
	res = GenerateRealKbsyncValue((int)szM1Value,strLen,(int)szBase64M1Value);
	strcpy((char*)lpM1,szBase64M1Value);
	m1DataLen = strlen(szBase64M1Value);

	LeaveCriticalSection(&g_cs);

	return TRUE;
}


BOOL setSrpReponseData(IN LPVOID lpGsaServices,
					   IN LPVOID lpSpd,
					   IN int spdLen,
					   IN LPVOID lpM2,
					   IN int M2Len,
					   IN LPVOID lpNp,
					   IN int npLen)
{
	CGsaservices* pGsaObj = (CGsaservices*)lpGsaServices;
	if (!pGsaObj)
	{
		return FALSE;
	}
	char szVerticalLine[] = "|";
	char szHMacKey[] = "HMAC key:";
	char szExtraDatazkey[] = "extra data key:";
	char szExtraDataIv[] = "extra data iv:";
	char szM2[64] = {0};
	char szNp[64] = {0};
	char szParam1[64] = {0};
	char sz32BytesData[64] = {0};
	char sz32BytesData2[64] = {0};
	char szAlloc32BytesData[64] = {0};
	char sz32BytesExtraData[64] = {0};
	char sz32BytesExtraIvData[64] = {0};
	char sz16BytesData[32] = {0};
	char szGetRecordData[0x111] = {0};
	int res = 0;
	int spdLength = 0;
	int m2Length = 0;
	int npLength = 0;
	int macKeyLen = 0;
	int extraDataLen = 0;
	int extraDataIvLen = 0;
	LPVOID lpAllocPtr = NULL;
	LPVOID lpSaveAllocPtr = NULL;
	LPVOID lpPtr = NULL;

	if (!lpSpd)
	{
		return FALSE;
	}
	
	char* lpDecodeBase64Spd = new char[spdLen+1];
	if (!lpDecodeBase64Spd)
	{
		return FALSE;
	}
	memset(lpDecodeBase64Spd,0,spdLen+1);

	char* lpDecodeXmlData = new char[spdLen+1];
	if (!lpDecodeXmlData)
	{
		return FALSE;
	}
	memset(lpDecodeXmlData,0,spdLen+1);

	int arrayData1_addr = pGsaObj->getArrayData1_addr();								//11694290
	int setData_addr = pGsaObj->getSetData_addr();									//sub_112D7B30
	int checkM2Value_addr = pGsaObj->getCheckM2Value_addr();							//sub_112DE0C0
	int arrayData3_addr = pGsaObj->getArrayData3_addr();								//sub_112D9FF0
	int gen32BytesData_addr = pGsaObj->getGen32BytesData_addr();						//sub_112DE120  
	int arrayData4_addr = pGsaObj->getArrayData4_addr();								//unk_11694B4C
	int setData2_addr = pGsaObj->getSetData2_addr();									//off_11694B54
	int setData3_addr = pGsaObj->getSetData3_addr();									//sub_112E4C70
	int decodeSpdData_addr = pGsaObj->getDecodeSpdData_addr();						//sub_112DE4B0
	//解码spd的值
	spdLength = SignSapCertSignature((int)lpSpd,spdLen,(int)lpDecodeBase64Spd);
	spdLength = spdLength - 1;
	//解码M2的值
	m2Length = SignSapCertSignature((int)lpM2,M2Len,(int)szM2);
	m2Length = m2Length - 1;
	//解码np的值
	npLength = SignSapCertSignature((int)lpNp,npLen,(int)szNp);
	npLength = npLength - 1;

	lpSaveAllocPtr = pGsaObj->GetSaveAllocPtr();
	lpAllocPtr = pGsaObj->GetAllocPtr();

	try
	{

		//设置M2值
		_asm
		{
			push lpAllocPtr
			lea eax,szM2[0]
			mov ecx,eax
			call checkM2Value_addr
			add esp,0x4

			lea esi,szVerticalLine[0]
			push esi
			push lpSaveAllocPtr
			mov edx,1
			mov ecx,arrayData1_addr
			call setData_addr
			add esp,0x8
		}
		//设置spd值
		_asm
		{
			mov ecx,arrayData1_addr
			mov edx,4
			lea eax,spdLength
			push eax
			push lpSaveAllocPtr
			call setData_addr
			add esp,0x8
		
			lea eax,lpDecodeBase64Spd[0]
			push eax
			mov ecx,arrayData1_addr
			mov edx,spdLength       //0x6E0
			push lpSaveAllocPtr
			call setData_addr
			add esp,0x8

			lea esi,szVerticalLine[0]
			push esi
			push lpSaveAllocPtr
			mov edx,1
			mov ecx,arrayData1_addr
			call setData_addr
			add esp,0x8
		}

		//设置sc值(调用函数和spd的一样) ios中sc的值为空，没有这个字段
		_asm
		{
			//
			lea esi,szVerticalLine[0]
			push esi
			push lpSaveAllocPtr
			mov edx,1
			mov ecx,arrayData1_addr
			call setData_addr
			add esp,0x8
		}

		_asm
		{
			lea eax,szParam1[0]
			push eax           //输出参数
			push lpSaveAllocPtr
			push arrayData1_addr
			call arrayData3_addr
			add esp,0xc
		}

		lpPtr = (LPVOID)((DWORD)lpAllocPtr + 0x324);
		memcpy(szAlloc32BytesData,lpPtr,0x20);

		//处理HMACKey:
		macKeyLen = strlen(szHMacKey);
		_asm
		{
			lea eax,sz32BytesData[0]
			push eax                 //输出参数
			lea edi,szHMacKey[0]
			push edi
			mov eax,macKeyLen
			push eax
			lea eax,szAlloc32BytesData[0];
			push eax
			mov edx,0x20
			mov ecx,arrayData1_addr
			call gen32BytesData_addr
			add esp,0x10
		}

		_asm
		{
			lea ebx,sz32BytesData2[0]
			push ebx
			lea eax,szParam1[0]
			push eax
			push 0x20  //[ebp+var_18] 
			lea eax,sz32BytesData[0]
			push eax
			mov edx,0x20
			mov ecx,arrayData1_addr
			call gen32BytesData_addr
			add esp,0x10
		}

		//sz32BytesData2中的值和np值校验HMAC的争取性

		//处理extra data key:
		memcpy(szAlloc32BytesData,lpPtr,0x20);
		extraDataLen = strlen(szExtraDatazkey);
		_asm
		{
			lea eax,sz32BytesExtraData[0]
			push eax                 //输出参数
			lea edi,szExtraDatazkey[0]
			push edi
			mov eax,extraDataLen
			push eax
			lea eax,szAlloc32BytesData[0];
			push eax
			mov edx,0x20
			mov ecx,arrayData1_addr
			call gen32BytesData_addr
			add esp,0x10
		}
		//处理extra data iv:
		memcpy(szAlloc32BytesData,lpPtr,0x20);
		extraDataIvLen = strlen(szExtraDataIv);
		_asm
		{
			lea eax,sz32BytesExtraIvData[0]
			push eax                 //输出参数
			lea edi,szExtraDatazkey[0]
			push edi
			mov eax,extraDataIvLen
			push eax
			lea eax,szAlloc32BytesData[0];
			push eax
			mov edx,0x20
			mov ecx,arrayData1_addr
			call gen32BytesData_addr
			add esp,0x10
		}

		memcpy(sz16BytesData,sz32BytesExtraIvData,0x10);
		
		_asm
		{
			mov edx,0x20
			lea eax,sz32BytesExtraData[0]
			mov ecx,eax
			lea esi,szGetRecordData[0]
			push esi
			call setData3_addr
			add esp,0x4
			mov dword ptr [esi+0x104],1
		}
		//解析数据
		_asm
		{
			push lpDecodeXmlData
			push lpDecodeBase64Spd
			lea esi,szGetRecordData[0]
			mov edx,esi
			mov ecx,arrayData4_addr
			push spdLength
			lea ebx,sz16BytesData[0]
			push ebx
			call decodeSpdData_addr
			add esp,0x10
		}

		//解析数据
		pGsaObj->parseDecryptData(lpDecodeXmlData);

		//释放内存空间
		if (lpDecodeBase64Spd)
		{
			delete []lpDecodeBase64Spd;
		}

		if (lpDecodeXmlData)
		{
			delete []lpDecodeXmlData;
		}
	}
	catch(exception& e)
	{
	}
	return TRUE;
}

BOOL generateSrpPETPassword(IN LPVOID lpGsaServices,OUT LPVOID lpPetPassword,OUT int& petPasswordLen)
{
	char szPetPwd[512] = {0};
	CGsaservices* pGsaObj = (CGsaservices*)lpGsaServices;
	if (!pGsaObj)
	{
		return FALSE;
	}
	string strPetPassword = pGsaObj->getPetPasswordToken();
	memcpy(szPetPwd,strPetPassword.c_str(),strPetPassword.length());

	petPasswordLen = strlen(szPetPwd);

	memcpy(lpPetPassword,szPetPwd,petPasswordLen);
	
	return TRUE;
}

BOOL generateSrpHBToken(IN LPVOID lpGsaServices,OUT LPVOID lpHBToken,OUT int& hbTokenLen)
{
	char szHbToken[512] = {0};
	CGsaservices* pGsaObj = (CGsaservices*)lpGsaServices;
	if (!pGsaObj)
	{
		return FALSE;
	}
	string strHBToken = pGsaObj->getHBToken();
	memcpy(szHbToken,strHBToken.c_str(),strHBToken.length());

	hbTokenLen = strlen(szHbToken);
	memcpy(lpHBToken,szHbToken,hbTokenLen);
	return TRUE;
}

BOOL generateSrpAdsid(IN LPVOID lpGsaServices,OUT LPVOID lpAdsid,OUT int& adsidLen)
{
	char szAdsid[MAX_PATH] = {0};
	CGsaservices* pGsaObj = (CGsaservices*)lpGsaServices;
	if (!pGsaObj)
	{
		return FALSE;
	}
	string strAdsid = pGsaObj->getAdsid();
	if (strAdsid.empty())
	{
		return FALSE;
	}
	memcpy(szAdsid,strAdsid .c_str(),strAdsid.length());

	adsidLen = strlen(szAdsid);
	memcpy(lpAdsid,szAdsid,adsidLen);
	
	return TRUE;
}

BOOL generateXAppleHBToken(IN LPVOID lpGsaServices,OUT LPVOID lpXAppleHBToken,OUT int& hbTokenLen)
{
	char szXAppleHBToken[512] = {0};
	char szBase64HBToken[512] = {0};

	CGsaservices* pGsaObj = (CGsaservices*)lpGsaServices;
	if (!pGsaObj)
	{
		return FALSE;
	}

	string strAdsid = pGsaObj->getAdsid();
	string strHBToken = pGsaObj->getHBToken();
	sprintf(szXAppleHBToken,"%s:%s",strAdsid.c_str(),strHBToken.c_str());
	
	int res = GenerateRealKbsyncValue((int)szXAppleHBToken,strlen(szXAppleHBToken),(int)szBase64HBToken);

	hbTokenLen = strlen(szBase64HBToken);

	memcpy(lpXAppleHBToken,szBase64HBToken,hbTokenLen);

	return TRUE;
}

void releaseGsaAllocate(IN LPVOID lpGsaServices)
{
	try
	{
		CGsaservices* pGSA = (CGsaservices*)lpGsaServices;
		if (!pGSA)
		{
			return;
		}
		LPVOID lpAllocPtr = pGSA->GetAllocPtr();
		LPVOID lpSavePtr = pGSA->GetSaveAllocPtr();

		if (lpAllocPtr)
		{
			VirtualFree(lpAllocPtr,0,MEM_RELEASE);
			lpAllocPtr = NULL;
		}

		if (lpSavePtr)
		{
			VirtualFree(lpSavePtr,0,MEM_RELEASE);
			lpSavePtr = NULL;
		}


		pGSA->SetAllocPtr(NULL);


		pGSA->SetSaveAllocPtr(NULL);
	}
	catch (exception& e)
	{
		OutputDebugStringA(e.what());
	}

	
	return;
}

BOOL generateMidOtpValue(IN LPVOID lpGsaServices,IN LPVOID lpMidValue,IN LPVOID lpOtpValue)
{
	char szMidValue[MAX_PATH] = {0};
	char szOtpValue[MAX_PATH] = {0};
	int midValue = 0;
	int otpValue = 0;
	HMODULE hModule = NULL;

	CGsaservices* pGsaObj = (CGsaservices*)lpGsaServices;
	if (!pGsaObj)
	{
		return FALSE;
	}

	hModule = ::GetModuleHandle(_T("iTunes.dll"));
	if (!hModule)
	{
		return FALSE;
	}

	int midandotpAlgorithmAddr = getGenerateMidandOtpAlgorithmAddr((char*)hModule);
	int releaseMidandOtpMemaddr = releaseMidandOtpMemAddr((char*)hModule);

	EnterCriticalSection(&g_cs);
	_asm
	{
		lea ecx,otpValue
		push ecx
		lea eax,midValue
		push eax
		call midandotpAlgorithmAddr
		add esp,0x8
	}


	//base64加密
	int res = GenerateRealKbsyncValue(midValue,strlen((char*)midValue),(int)szMidValue);
	res = GenerateRealKbsyncValue(otpValue,strlen((char*)otpValue),(int)szOtpValue);

	memcpy(lpMidValue,szMidValue,strlen(szMidValue));
	memcpy(lpOtpValue,szOtpValue,strlen(szOtpValue));

	//释放
	_asm
	{
		push otpValue
		push midValue
		call releaseMidandOtpMemaddr
		add esp,0x8
	}
	LeaveCriticalSection(&g_cs);

	return TRUE;
}

void itunesBase64(char* lpSrcData,int srcLen,char* lpOutData)
{
	int lpBuffer = 0;
	HMODULE hModule = GetModuleHandle(_T("iTunesCore.dll"));
	if (!hModule)
	{
		return;
	}

	int itunesBase64Addr = getItunesBase64AlgorithmAddress((char*)hModule);
	int itunesBase64AllocateMemAddr = getItunesBase64AllocateMemAddress((char*)hModule);

	_asm
	{
		lea ecx,lpBuffer
		mov edx,srcLen
		push ecx
		push ecx
		push 0
		push 0
		mov ecx,lpSrcData
		call itunesBase64Addr
		mov edi,eax
		push edi
		push 1
		call itunesBase64AllocateMemAddr
		mov lpBuffer,eax

		mov ebx,lpBuffer

	}
	return;
}

BOOL generateX_Apple_AMD_Value(IN LPVOID lpGsaServices,char* lpDsid,char* lpXAppleAMDM,char* lpXAppleAMD)
{
	char szAMDMValue[512] = {0};
	char szAMDValue[MAX_PATH] = {0};
	int kbsyncSecondThridParam[2];
	int lpX_Apple_I_AMD_M = 0;
	int  amd_mLen = 0;
	int lpX_Apple_I_AMD = 0;
	int amdLen = 0;
	int param1 = -1;
	int param2 = -1;
	int res = 0;

	CGsaservices* pGsaObj = (CGsaservices*)lpGsaServices;
	if (!pGsaObj)
	{
		return FALSE;
	}

	HMODULE hModule = GetModuleHandle(_T("iTunesCore.dll"));
	if (!hModule)
	{
		return FALSE;
	}

	int algorithAddress = getGenerateMD_AMDAlgorithmAddress((char*)hModule);
	int freeAddress = getFreeMD_AMD_AlgorithmAddress((char*)hModule);

	EnterCriticalSection(&g_cs);
	if(lpDsid)
	{
		generateSecondThirdData(lpDsid,kbsyncSecondThridParam);	
		_asm
		{
			lea eax,amdLen
			push eax
			lea eax,lpX_Apple_I_AMD
			push eax
			lea eax,amd_mLen
			push eax
			lea eax,lpX_Apple_I_AMD_M
			push eax
			mov eax,kbsyncSecondThridParam[1]
			push eax
			mov eax,kbsyncSecondThridParam[0]
			push eax
			call algorithAddress
			add esp,0x18
			
		}
	}
	_asm
	{
		lea eax,amdLen
		push eax
		lea eax,lpX_Apple_I_AMD
		push eax
		lea eax,amd_mLen
		push eax
		lea eax,lpX_Apple_I_AMD_M
		push eax
		push param2
		push param1
		call algorithAddress
		add esp,0x18
	}

	//base64计算
	res = GenerateRealKbsyncValue(lpX_Apple_I_AMD_M,amd_mLen,(int)szAMDMValue);
	memcpy(lpXAppleAMDM,szAMDMValue,strlen(szAMDMValue));

	res = GenerateRealKbsyncValue(lpX_Apple_I_AMD,amdLen,(int)szAMDValue);
	memcpy(lpXAppleAMD,szAMDValue,strlen(szAMDValue));

	//释放内存
	_asm
	{
		mov eax,lpX_Apple_I_AMD_M
		push eax
		call freeAddress
		add esp,0x4

		mov eax,lpX_Apple_I_AMD
		push eax
		call freeAddress
		add esp,0x4
	}
	LeaveCriticalSection(&g_cs);


	return TRUE;
}

BOOL generateX_Apple_I_MD_Value(IN LPVOID lpGsaServices,OUT LPVOID lpXAppleIMDM,OUT LPVOID lpXAppleIMD)
{
	char szIMDMValue[512] = {0};
	char szIMDValue[MAX_PATH] = {0};
	int param1 = -2;
	int param2 = -1;
	int lpX_Apple_I_MD_M = 0;
	int  md_mLen = 0;
	int lpX_Apple_I_MD = 0;
	int mdLen = 0;
	int res = 0;
	int paramEBP_60H[20] = {0};

	CGsaservices* pGsaObj = (CGsaservices*)lpGsaServices;
	if (!pGsaObj)
	{
		return FALSE;
	}

	HMODULE hModule = GetModuleHandle(_T("iTunesCore.dll"));
	if (!hModule)
	{
		return FALSE;
	}

	int mdInitAlgorithmAddr = getMDInitAlgorithmAddress((char*)hModule);
    int mdSecondInitAlgorithmAddr = getMDSecondInitAlgorithmAddress((char*)hModule);

	int algorithAddress = getGenerateMD_AMDAlgorithmAddress((char*)hModule);
	int freeAddress = getFreeMD_AMD_AlgorithmAddress((char*)hModule);
	EnterCriticalSection(&g_cs);
	_asm
	{
		mov eax,param2
		push eax
		mov esi,param1
		push esi
		call mdInitAlgorithmAddr
		add esp,0x8
	}

	_asm
	{
		lea eax,paramEBP_60H[0]
		push eax
		mov eax,param2
		push eax
		mov eax,param1
		push eax
		call mdSecondInitAlgorithmAddr
		add esp,0xc
		mov esi,eax
	}

	
	_asm
	{
		lea eax,mdLen
		push eax
		lea eax,lpX_Apple_I_MD
		push eax
		lea eax,md_mLen
		push eax
		lea eax,lpX_Apple_I_MD_M
		push eax
		mov eax,param2
		push eax
		mov eax,param1
		push eax
		call algorithAddress
		add esp,0x18
	}

	//base64计算
	res = GenerateRealKbsyncValue(lpX_Apple_I_MD_M,md_mLen,(int)szIMDMValue);
	memcpy(lpXAppleIMDM,szIMDMValue,strlen(szIMDMValue));

	//修改lpX_Apple_I_MD最后一个字节为0x03
//	*(int*)(lpX_Apple_I_MD+mdLen-4) = 0x03000000;
	
	res = GenerateRealKbsyncValue(lpX_Apple_I_MD,mdLen,(int)szIMDValue);
	memcpy(lpXAppleIMD,szIMDValue,strlen(szIMDValue));
	
	//释放内存
	_asm
	{
		mov eax,lpX_Apple_I_MD_M
		push eax
		call freeAddress
		add esp,0x4

		mov eax,lpX_Apple_I_MD
		push eax
		call freeAddress
		add esp,0x4
	}
	LeaveCriticalSection(&g_cs);

	return TRUE;
}

BOOL generateX_Apple_MD_Value(IN LPVOID lpGsaServices,IN LPVOID lpDsid,OUT LPVOID lpXAppleMDM,OUT LPVOID lpXAppleMD)
{
	char		szMDMValue[512] = {0};
	char		szMDValue[MAX_PATH] = {0};
	int			kbsyncSecondThridParam[2] = {0};
	int			lpX_Apple_MD_M = 0;
	int			md_mLen = 0;
	int			lpX_Apple_MD = 0;
	int			mdLen = 0;
	int			res = 0;

	CGsaservices* pGsaObj = (CGsaservices*)lpGsaServices;
	if (!pGsaObj)
	{
		return FALSE;
	}

	HMODULE hModule = GetModuleHandle(_T("iTunesCore.dll"));
	if (!hModule)
	{
		return FALSE;
	}

	int algorithAddress = getGenerateMD_AMDAlgorithmAddress((char*)hModule);
	int freeAddress = getFreeMD_AMD_AlgorithmAddress((char*)hModule);

	EnterCriticalSection(&g_cs);
	if (lpDsid)
	{
		generateSecondThirdData((char*)lpDsid,kbsyncSecondThridParam);
	}

	_asm
	{
		lea eax,mdLen
		push eax
		lea eax,lpX_Apple_MD
		push eax
		lea eax,md_mLen
		push eax
		lea eax,lpX_Apple_MD_M
		push eax
		mov eax,kbsyncSecondThridParam[1]
		push eax
		mov eax,kbsyncSecondThridParam[0]
		push eax
		call algorithAddress
		add esp,0x18
		
	}

	//base64计算
	res = GenerateRealKbsyncValue(lpX_Apple_MD_M,md_mLen,(int)szMDMValue);
	memcpy(lpXAppleMDM,szMDMValue,strlen(szMDMValue));

	res = GenerateRealKbsyncValue(lpX_Apple_MD,mdLen,(int)szMDValue);
	memcpy(lpXAppleMD,szMDValue,strlen(szMDValue));

	//释放内存
	_asm
	{
		mov eax,lpX_Apple_MD_M
		push eax
		call freeAddress
		add esp,0x4

		mov eax,lpX_Apple_MD
		push eax
		call freeAddress
		add esp,0x4
	}
	LeaveCriticalSection(&g_cs);

	return TRUE;
}

BOOL generateSbsyncValue(IN LPVOID lpGsaServices,
						 IN LPVOID lpDsid,
						 IN LPVOID lpXApple_amd_m,
						 IN int amd_m_Len,
						 OUT LPVOID lpSbsyncValue,
						 OUT int& sbsyncLen)
{
	HMODULE hModule = NULL;
	int sbsyncValue = 0;
	int sbsyncLength = 0;
	int res = 0;
	char szSbsync[1024] = {0};
	int kbsyncSecondThridParam[2] = {0};

	CGsaservices* pGsaObj = (CGsaservices*)lpGsaServices;
	if (!pGsaObj)
	{
		return FALSE;
	}

	if (!lpDsid)
	{
		return FALSE;
	}
	if (!lpXApple_amd_m)
	{
		return FALSE;
	}
	
	hModule = ::GetModuleHandle(_T("iTunesCore.dll"));
	if (!hModule)
	{
		return FALSE;
	}

	if (lpDsid)
	{
		generateSecondThirdData((char*)lpDsid,kbsyncSecondThridParam);
	}

	int genSbsyncValueAddr = getGenerateSbsyncValueAddr((char*)hModule);
	int MemFreeAlgorithmAddr = GetFreeMemoryAlgorithmAddr((char*)hModule);

	_asm
	{
		lea edx,sbsyncLength
		push edx
		lea edx,sbsyncValue
		push edx
		push amd_m_Len
		push lpXApple_amd_m
		push 0x12E
		push kbsyncSecondThridParam[1]
		push kbsyncSecondThridParam[0]
		push g_firstkbsyncParamFunAddr
		call genSbsyncValueAddr
		add esp,0x20
	}

	//base64计算
	res = GenerateRealKbsyncValue(sbsyncValue,sbsyncLength,(int)szSbsync);

	//释放内存
	_asm
	{
		push sbsyncValue
		call MemFreeAlgorithmAddr
		add esp,0x4
	}

	sbsyncLen = strlen(szSbsync);
	memcpy(lpSbsyncValue,szSbsync,sbsyncLen);

	return TRUE;
}
void setKeyBagDataEx(IN LPVOID lpGsaServices,IN LONGLONG dsid,IN LPVOID lpKeyBagData,IN int keyBagDataLen)
{
	int retValue = 0;
	HMODULE hModule = NULL;
	CGsaservices* pGsaObj = (CGsaservices*)lpGsaServices;
	if (!pGsaObj)
	{
		return;
	}
	if (!lpKeyBagData)
	{
		return;
	}
	hModule = ::GetModuleHandle(_T("iTunesCore.dll"));
	if (!hModule)
	{
		return;
	}

	int dsid_low = 0xFFFFFFFF & dsid;
    int dsid_hi = (0xFFFFFFFF00000000 & dsid)>>32;

	int setkeyBagFunction_addr = pGsaObj->getImportKeyBagData_addr();
	int setDsid_ic_info_addr = pGsaObj->getSet_dsid_ic_Info_addr();
	

	//对keybag数据base64解开
	char* lpDecodeData = (char*)VirtualAlloc(NULL,keyBagDataLen,MEM_COMMIT | MEM_RESERVE,PAGE_EXECUTE_READWRITE);
	if (!lpDecodeData)
	{
		return;
	}
	memset(lpDecodeData,0,keyBagDataLen);
	//解密原始数据

	int decodeDataLen = base64_decode((const char*)lpKeyBagData,(uint8_t *)lpDecodeData,keyBagDataLen);

	//调用算法设置keybag的数据，用来计算kbsync值有效
	EnterCriticalSection(&g_cs);
	_asm
	{
		push 0
		push dsid_hi
		push dsid_low
		push g_firstkbsyncParamFunAddr
		call setDsid_ic_info_addr
		add esp,0x10
		mov retValue,eax
	}
	_asm
	{
		push decodeDataLen
		mov edi,lpDecodeData
		push edi
		push g_firstkbsyncParamFunAddr
		mov edx,setkeyBagFunction_addr
		call edx
		add esp,0xc
		mov retValue,eax
	}

	LeaveCriticalSection(&g_cs);

	//重新计算魔数值
	int ic_Info = Generate_ic_info_value((char*)hModule);

	if (lpDecodeData)
	{
		VirtualFree(lpDecodeData,0,MEM_RELEASE);
		lpDecodeData = NULL;
	}
	return;

}
void setKeyBagData(IN LPVOID lpGsaServices,IN LPVOID lpKeyBagData,IN int keyBagDataLen)
{
	char szSC_InfoPath[4096] = {0};
	int retValue = 0;
	int pOutBuffer = 0;
	HMODULE hModule = NULL;
	CGsaservices* pGsaObj = (CGsaservices*)lpGsaServices;
	if (!pGsaObj)
	{
		return;
	}
	if (!lpKeyBagData)
	{
		return;
	}
	hModule = ::GetModuleHandle(_T("iTunesCore.dll"));
	if (!hModule)
	{
		return;
	}
	int setkeyBagFunction_addr = pGsaObj->getImportKeyBagData_addr();
	int set_ic_Info_addr = pGsaObj->getSet_ic_Info_value_addr();
	int set_SC_Info_addr = pGsaObj->getSet_SC_Info_Path_addr();
	int getGenerate_ic_info_value_addr = pGsaObj->getGenerate_ic_info_value_addr();
	int set_ic_Info_addr2 = pGsaObj->getSet_ic_Info_Value_addr2();

	//对keybag数据base64解开
	char* lpDecodeData = (char*)VirtualAlloc(NULL,keyBagDataLen,MEM_COMMIT | MEM_RESERVE,PAGE_EXECUTE_READWRITE);
	if (!lpDecodeData)
	{
		return;
	}
	memset(lpDecodeData,0,keyBagDataLen);
	//解密原始数据

	int decodeDataLen = base64_decode((const char*)lpKeyBagData,(uint8_t *)lpDecodeData,keyBagDataLen);

	//调用算法设置keybag的数据，用来计算kbsync值有效
	EnterCriticalSection(&g_cs);
	//等于0的时候
	_asm
	{
		push decodeDataLen
		mov edi,lpDecodeData
		push edi
		push g_firstkbsyncParamFunAddr
		mov edx,setkeyBagFunction_addr
		call edx
		add esp,0xc
		mov retValue,eax
	}
	if (0x0FFFF5BEF == retValue)
	{
		_asm
		{
			push g_firstkbsyncParamFunAddr
			mov edx,set_ic_Info_addr
			call edx
			add esp,0x4
			mov retValue,eax
		}
		_asm
		{
			push g_firstkbsyncParamFunAddr
			mov edx,set_ic_Info_addr2
			call edx
			add esp,0x4
			mov retValue,eax
		}
		_asm
		{
			lea ecx,szSC_InfoPath[0x31]
			mov edx,set_SC_Info_addr
			call edx
			mov retValue,eax
		}
		if (retValue == 0)
		{ 
			_asm
			{
				lea eax,pOutBuffer
				mov edx,lpDecodeData
				xor dl,dl
				push eax
				push 0
				push 0
				lea ecx,szSC_InfoPath[0x31]
				call getGenerate_ic_info_value_addr
				add esp,0xc
			}
			_asm
			{
				push decodeDataLen
				mov edi,lpDecodeData
				push edi
				push g_firstkbsyncParamFunAddr
				mov edx,setkeyBagFunction_addr
				call edx
				add esp,0xc
				mov retValue,eax
			}
		}
	}

	//等于1的时候
	_asm
	{
		push g_firstkbsyncParamFunAddr
		mov edx,set_ic_Info_addr
		call edx
		add esp,0x4
		mov retValue,eax
	}
	if(retValue != 0x0FFFF5B57)
	{
		_asm
		{
			push decodeDataLen
			mov edi,lpDecodeData
			push edi
			push g_firstkbsyncParamFunAddr
			mov edx,setkeyBagFunction_addr
			call edx
			add esp,0xc
			mov retValue,eax
		}

	}
	if (retValue == 0x0FFFF5BEF)
	{
		_asm
		{
			push g_firstkbsyncParamFunAddr
			mov edx,set_ic_Info_addr
			call edx
			add esp,0x4
			mov retValue,eax
		}

		if (retValue != 0x0FFFF5B57)
		{
			_asm
			{
				push decodeDataLen
				mov edi,lpDecodeData
				push edi
				push g_firstkbsyncParamFunAddr
				mov edx,setkeyBagFunction_addr
				call edx
				add esp,0xc
				mov retValue,eax
			}

		}
	}
	else
	{
	}
	LeaveCriticalSection(&g_cs);

	//重新计算魔数值
	int ic_Info = Generate_ic_info_value((char*)hModule);

	if (lpDecodeData)
	{
		VirtualFree(lpDecodeData,0,MEM_RELEASE);
		lpDecodeData = NULL;
	}
	
}
/////////初始化模块管理器
BOOL InitializeSDKManager(void)
{
	HMODULE hModule = NULL;
	hModule = ::GetModuleHandle(_T("iTunesCore.dll"));
	if (!hModule)
	{
		return FALSE;
	}

	/////hook函数
	int funAddr1 = GetFunAddr1((char*)hModule);
	int funAddr2 = GetFunAddr2((char*)hModule);
	int funAddr4 = GetFunAddr4((char*)hModule);

	int funGetvcRMAddr = GetvcRMAddr((char*)hModule);

	pvcRM = new char[1208];
	if (!pvcRM)
	{
		pvcRM = NULL;
		pvcRM = new char[1208];
	}
	_asm
	{
		mov eax,pvcRM
		push eax
		mov edx,funGetvcRMAddr
		call edx

	}

	return TRUE;
}

void UnInitializeModule(void)
{
	//RestoreMacAddressValue();
	//RestoreSerialNumValue();
	if (g_hModule)
	{
		FreeLibrary(g_hModule);
		g_hModule = NULL;
	}
}

BOOL GetMachineGUID(IN LPVOID lpGsaServices,LPVOID lpBuffer)
{
	CGsaservices* pGsaObj = (CGsaservices*)lpGsaServices;
	if (!pGsaObj)
	{
		return FALSE;
	}
	HMODULE hModule = NULL;
	int Result = 0;
	char* pGUID = NULL;
	hModule = ::GetModuleHandle(_T("iTunesCore.dll"));
	if (!hModule)
	{
		return FALSE;

	}

	Result = GetFunctionAddress_3((char*)hModule);

	pGUID = generateMachineGuid(Result);

	memset(lpBuffer,0,sizeof(lpBuffer));
	memcpy(lpBuffer,pGUID+9,62);

	return TRUE;
}

int  GetiTunesFunAddr_1(IN LPVOID lpGsaServices)
{
	HMODULE hModule = NULL;
	hModule = ::GetModuleHandle(_T("iTunesCore.dll"));
	int _adr_1 = NewItunesFunctionAddress_1((char*)hModule);
	_asm
	{
		call _adr_1
	}
}

int  GetiTunesFunAddr_2(IN LPVOID lpGsaServices,char* lpUdid)
{
	HMODULE hModule_1 = NULL;

	CGsaservices* pGsaObj = (CGsaservices*)lpGsaServices;
	if (!pGsaObj)
	{
		return FALSE;
	}

	hModule_1 = ::GetModuleHandle(_T("iTunesCore.dll"));
	
	int _address_;
	int _adr_2 = NewItunesFunctionAddress_2(hModule_1,_address_);//0x0fffffff
	//begin
	unsigned char *p = (unsigned char *)(g_hw_info+1);
	for(int i=0,j=0; i<strlen(lpUdid); i+=2,j++)
    {
        unsigned char c = lpUdid[i];
        if ( c>='0' && c<='9')
            p[j] = (c-'0')*16;
        else if( c>='a' && c<='f')
            p[j] = (c-'a'+10)*16;
        else
            printf("error\r\n");
        
        unsigned char c1 = lpUdid[i+1];
        if ( c1>='0' && c1<='9')
            p[j] += c1-'0';
        else if( c1>='a' && c1<='f')
            p[j] += c1-'a'+10;
        else
            printf("error\r\n");
    }
    g_hw_info[0] = 0x14;
	//end

	int sessionId = 0;
	::EnterCriticalSection(&g_cs);
	_asm
	{
		mov eax,_adr_2
		mov edx,eax
		lea eax,g_hw_info[0]//[_var_1]
		push eax
		lea ecx,[sessionId] //sessionId
		push ecx
		call edx
		add esp,0x8
	}
	g_itune8ParamAddr = _address_;//sign-sap-setup-buffer算法地址，需要给它传8个参数
	g_sessionId = sessionId;
	::LeaveCriticalSection(&g_cs);
	return 0;
}



int SignSapCertSignature(int a1, int a2, int a3)
{
  int v3;
  int v5; 
  int i; 

  v5 = 0;
  for ( i = 0; i < a2; i += 4 )
  {
    *(BYTE *)(v5 + a3) = ((signed int)(unsigned __int8)strCheck[*(BYTE *)(i + a1 + 1)] >> 4) | 4 * strCheck[*(BYTE *)(i + a1)];
    v3 = v5 + 1;
    *(BYTE *)(v3++ + a3) = ((signed int)(unsigned __int8)strCheck[*(BYTE *)(i + a1 + 2)] >> 2) | 16 * strCheck[*(BYTE *)(i + a1 + 1)];
    *(BYTE *)(v3 + a3) = strCheck[*(BYTE *)(i + a1 + 3)] | (strCheck[*(BYTE *)(i + a1 + 2)] << 6);
    v5 = v3 + 1;
  }
  *(BYTE *)(v5 + a3) = 0;
  return v5;
}

//登录的时候调用的函数，得到登录需要的sign-sap-setup-buffer的值
int GetSignsapSetupBuffer(IN LPVOID lpGsaServices,
						  LPVOID lpSignSap,
						  int signSize,
						  LPVOID lpBuffer,
						  int& OutDataLength,
						  BOOL bSet,
						  LPVOID lpUserXmlInfo,
						  int userXmlSize)
{
	HMODULE hModule = NULL;
	int Result_1 = 0;
	int OutParam_1 = 0;
	int OutParam_2 = 0;
	int strSignBuf[1024];
	int IntSignsap[1024];   //ebp-2044
	int nValue = 0;
	int lastparam;
	char tempBuf[1024] = {0};
	int OutBuf_1 = 0;
	int OutBuf_2 = 0;

	CGsaservices* pGsaObj = (CGsaservices*)lpGsaServices;
	if (!pGsaObj)
	{
		return -1;
	}
	if (!g_bRegister)
	{
		return -1;
	}
	
	hModule = GetModuleHandle(_T("iTunesCore.dll"));
	if (!hModule)
	{
		return -1;
	}
	int MemFreeAlgorithmAddr = GetFreeMemoryAlgorithmAddr((char*)hModule);
	if (hModule)
	{
		nValue = SignSapCertSignature((int)lpSignSap,signSize,(int)IntSignsap);//nValue = ebp-204C	
		::EnterCriticalSection(&g_cs);
		_asm
		{
			mov lastparam,0xff//0
			lea edx,lastparam
			push edx
			lea eax,OutParam_1
			push eax
			lea ecx,OutParam_2
			push ecx
			mov edx,nValue
			push edx
			lea eax,IntSignsap
			push eax
			mov ecx,g_sessionId
			push ecx
			lea eax,g_hw_info[0]
			push eax
			push 0x0C8
			mov edx,g_itune8ParamAddr
			call edx
			add esp,0x20
		}
		::LeaveCriticalSection(&g_cs);

		int InParam_1 = OutParam_1;
		int InParam_2 = OutParam_2;

		if (!bSet)
		{
			memset(strSignBuf,0x00,sizeof(strSignBuf));
			int Res = GenerateRealKbsyncValue(InParam_2,InParam_1,(int)strSignBuf);// strSignBuf为得到的字符串地址
			memcpy(lpBuffer,strSignBuf,1024);

			memcpy(tempBuf,strSignBuf,1024);
			OutDataLength = strlen(tempBuf);
			//释放空间
			_asm
			{
				mov eax,OutParam_2
				push eax
				mov edx,MemFreeAlgorithmAddr
				call edx
				add esp,0x4
			}
		}
		else
		{
			int tempAddress = NewItunesFunctionAddress_3((char*)hModule);
			::EnterCriticalSection(&g_cs);
			_asm
			{
				lea eax,OutBuf_1
				push eax
				lea ecx,OutBuf_2
				push ecx
				mov edx,userXmlSize
				push edx
				mov eax,lpUserXmlInfo
				push eax
				mov ecx,g_sessionId
				push ecx
				mov edx,tempAddress
				call edx
				add esp,0x14
			}
			::LeaveCriticalSection(&g_cs);
			
			memset(strSignBuf,0x00,sizeof(strSignBuf));
			int Res = GenerateRealKbsyncValue(OutBuf_2,OutBuf_1,(int)strSignBuf);// strSignBuf为得到的字符串地址
			memcpy(lpBuffer,strSignBuf,1024);

			memcpy(tempBuf,strSignBuf,1024);
			OutDataLength = strlen(tempBuf);
		}

	}

	return 0;
}

BOOL generateSecondThirdData(char* lpDsid,int* paramData)
{
	HMODULE hModule = ::GetModuleHandle(_T("iTunesCore.dll"));
	if (!hModule)
	{
		return FALSE;
		
	}

	int addr =  AllMulAddr((char*)hModule);
	int ebp_1AC = 0;
	int ebp_1C8 = 0;

	int len = strlen(lpDsid);
	for (int i=0;i<len; i++)
	{
		int tmp = (int)lpDsid[i];
		_asm
		{
			
			mov ebx,tmp
			mov ecx,ebp_1AC
			push 0
			push 0x0a
			push ecx
			mov ecx,ebp_1C8
			push ecx
			call addr
			mov ecx,eax
			mov ebp_1AC,edx

			lea eax,[ebx-0x30]
			cdq
			add ecx,eax
			mov eax,ebp_1AC
			mov ebp_1C8,ecx
			adc eax,edx
			mov ebp_1AC,eax
		}
	}

	paramData[0] = ebp_1C8;
	paramData[1] = ebp_1AC;

	return TRUE;
}

BOOL GetEncryptKbsyncValue(IN LPVOID lpGsaServices,LPVOID lpBuffer,char* lpDsid,LONGLONG dsid,bool bAuthLogin)
{
	HMODULE hModule = NULL;
	int Result_1 = 0;
	int Result_2 = 0;
	int Address_1 = 0;
	int kbsyncArg = 0;
	int OutParam_1 = 0;
	int OutParam_2 = 0;
	int IntKbysnc = 0;
	int kbsyncAddress[1024] = {0};
	int kbsyncSecondThridParam[2];

	CGsaservices* pGsaObj = (CGsaservices*)lpGsaServices;
	if (!pGsaObj)
	{
		return FALSE;
	}

	if (!g_bRegister)
	{
		return FALSE;
	}
	
	hModule = ::GetModuleHandle(_T("iTunesCore.dll"));
	if (hModule)
	{
		//获取用来计算kbsync值的第二个参数(其实就是dsid的16进制表示)，第三个参数
		
		if (dsid != 0)
		{	
			generateSecondThirdData(lpDsid,kbsyncSecondThridParam);	
		}

		Result_2 = GetFunctionAddress_2((char*)hModule);
		if (bAuthLogin)
		{
			//登录kbsync_auth
			kbsyncArg = GetGenerateKbsyncPargam(OutParam_1,OutParam_2,kbsyncSecondThridParam[0],kbsyncSecondThridParam[1],Result_2,0xB);
		}
		else
		{
			//购买kbsync
			kbsyncArg = GetGenerateKbsyncPargam(OutParam_1,OutParam_2,kbsyncSecondThridParam[0],kbsyncSecondThridParam[1],Result_2,1);
		}

		memset(kbsyncAddress,0x0000,sizeof(kbsyncAddress));		
		IntKbysnc = GenerateRealKbsyncValue(OutParam_2,OutParam_1,(int)kbsyncAddress);	// kbsyncAddress为kbsync值的首地址

		memset(lpBuffer,0x00,sizeof(lpBuffer));
		memcpy(lpBuffer,kbsyncAddress,1024);
	}
	else
	{
		return FALSE;
	}

	return TRUE;
}

////////////////////////////////////////////////////////////////////////////////内部函数定义

char* generateMachineGuid(int Address)
{
	char buffer[80] = {0};
	_asm
	{
		pushad
		mov edx,Address
		xor ecx,ecx
		call edx
		mov esi,eax
		lea edi,buffer
		mov ecx,0x48    //重复次数
		cld             //每次esi和edi都递增方向
		rep movsb
		popad
	}

	return buffer;

}

int GetArgument(int _Address_1)
{
	int temp = 0;
	_asm
	{
		push 0
		push 0
		push 0
		lea edx,dword ptr temp
		mov eax,_Address_1
		call eax
		mov eax,temp
		add esp, 0xC

	}
}


int GetFunAddr1(char *hModule)
{
	HANDLE v1; 
	int result;
	int v3;
	unsigned int i; 
	unsigned int j;
	char *v8;
	struct _MODULEINFO modinfo;

	 v1 = GetCurrentProcess();
	 if ( GetModuleInformation(v1,(HMODULE)hModule, &modinfo, 0xCu) == TRUE )
	 {
		v8 = &hModule[modinfo.SizeOfImage];
		v3 = 0;
		for ( i = (unsigned int)hModule; i < (unsigned int)(v8 - 4096); ++i )
		{
			if ( *(DWORD*)i == 0x8DF8D1C2 && *(DWORD*)(i+4) == 0x8D51000C)
			{
				v3 = i + 20;
				break;
			}
		}

		if (v3)
		{
			j = v3;
			if ( *(DWORD*)j== 0xC48301B3 && *(DWORD*)(j+4) == 0xFC4D8B04 && *(DWORD*)(j+12) == 0xE85BCD33)
			{
				result = j + 64;
			}
		}
	 }

	return result;
}

int GetFunAddr2(char *hModule)
{
	HANDLE v1; 
	int result;
	int v3;
	int v4;
	unsigned int i; 
	unsigned int j;
	char *v8;
	struct _MODULEINFO modinfo;
	 v1 = GetCurrentProcess();
	 if ( GetModuleInformation(v1,(HMODULE)hModule, &modinfo, 0xCu) == TRUE )
	 {
		v8 = &hModule[modinfo.SizeOfImage];
		v3 = 0;
		for ( i = (unsigned int)hModule; i < (unsigned int)(v8 - 4096); ++i )
		{
			if (*(DWORD*)i == 0x036AFFFF && *(DWORD*)(i+4) == 0xCE8B006A)
			{
				v3 = i + 32;
				break;
			}
		}

		if (v3)
		{
			v4 = 0;
			for (j = v3; ; j++)
			{
				if (*(DWORD*)j == 0x158B0F89 && *(DWORD*)(j+8) == 0xA1045789)
				{
					v4 = j + 48;
					break;
				}
			}

			if (*(DWORD*)v4 == 0x01B0CD33)
			{
				result = v4 + 15;
			}
		}
	 }

	return result;
}

int GetFunAddr4(char *hModule)
{
	HANDLE v1; 
	int result;
	int v3;
	unsigned int i; 

	char *v8;
	struct _MODULEINFO modinfo;

	 v1 = GetCurrentProcess();
	 if ( GetModuleInformation(v1,(HMODULE)hModule, &modinfo, 0xCu) == TRUE )
	 {
		v8 = &hModule[modinfo.SizeOfImage];
		v3 = 0;
		for ( i = (unsigned int)hModule; i < (unsigned int)(v8 - 4096); ++i )
		{
			if (*(DWORD*)i == 0x5753F0E4 && *(DWORD*)(i+4) == 0x84EC8156 && *(DWORD*)(i+8) == 0x0F000002 && *(DWORD*)(i+12) == 0x89C68931)
			{
				v3 = i;
				break;
			}
		}

		if (v3)
		{
			result = v3 - 4;
		}
	 }

	 return result;
}


int GetvcRMAddr(char *hModule)
{
	HANDLE v1; 
	int result;
	int v3;
	int v4;
	unsigned int i; 
	unsigned int j;
	char *v8;
	struct _MODULEINFO modinfo;

	 v1 = GetCurrentProcess();
	 if ( GetModuleInformation(v1,(HMODULE)hModule, &modinfo, 0xCu) == TRUE )
	 {
		v8 = &hModule[modinfo.SizeOfImage];
		v3 = 0;
		for ( i = (unsigned int)hModule; i < (unsigned int)(v8 - 4096); ++i )
		{
			if (*(DWORD*)i == 0x0004B868 && *(DWORD*)(i+12) == 0xF0458908)
			{
				v3 = i + 12;
				break;
			}
		}

		if (v3)
		{
			v4 = 0;
			for (j=v3; ; j++)
			{
				if (*(DWORD*)j == 0x028B168B && *(DWORD*)(j+4) == 0xCE8B016A)
				{
					v4 = j + 60;
					break;
				}
			}
		}

		result = v4;
	 }

	 return result;
}

//////////////////
int GetFunctionAddress_1(char *hModule)
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
      if ( *(DWORD*)i == 0x4589C533 &&
           *(DWORD*)(i+ 4) == 0x8B5653FC &&
           *(DWORD*)(i + 8) == 0x85D98AF2)
      {
		result = i - 14;
        break;
      }

    }// end for
  
  }//end if



  return result;

}


int GetFunctionAddress_2(char *hModule)
{
  HANDLE v1; 
  int result;
  unsigned int i; 

  char *v8;
  struct _MODULEINFO modinfo;

  v1 = GetCurrentProcess();
  if ( GetModuleInformation(v1, (HMODULE)hModule, &modinfo, 0xCu) )
  {
    v8 = &hModule[modinfo.SizeOfImage];
 
    for ( i = (unsigned int)hModule; i < (unsigned int)(v8 - 4096); ++i )
    {
      if ( *(DWORD *)i == 0xCCCCCDB9 && 
		  *(DWORD *)(i+4) == 0xC1E1F7CC && 
		  *(DWORD*)(i+8) == 0x048D02EA &&
		  *(DWORD*)(i+12) == 0x8DD8F792 &&
		  *(DWORD*)(i+22) == 0x8904E0C1 &&
		  *(DWORD*)(i+26) == 0x8D482444 &&
		  *(DWORD*)(i+30) == 0x89502444 &&
		  *(DWORD*)(i+34) == 0x44C72404 && 
		  *(DWORD*)(i+38) == 0x00800824)
      {
        result = i - 25;
        break;
      }
    }
  }

  return result;
}

//得到生成kbsync算法的第二个参数，第三个参数的值的函数地址
int AllMulAddr(char* hModule)
{
  HANDLE v1; 
  int result;
  unsigned int i; 

  char *v8;
  struct _MODULEINFO modinfo;

  v1 = GetCurrentProcess();
  if ( GetModuleInformation(v1, (HMODULE)hModule, &modinfo, 0xCu) )
  {
    v8 = &hModule[modinfo.SizeOfImage];
 
    for ( i = (unsigned int)hModule+0x2D800; i < (unsigned int)(v8 - 4096); ++i )
    {
      if ( *(DWORD*)i == 0x0824448B && 
		  *(DWORD*)(i+4) == 0x10244C8B && 
		  *(DWORD*)(i+8) == 0x4C8BC80B &&
		  *(DWORD*)(i+16) == 0x0424448B &&
		  *(DWORD*)(i+20) == 0x10C2E1F7)
      {
        result = i;
        break;
      }
    }
  }
	return result;
}

int getGenerateMD_AMDAlgorithmAddress(char* hModule)
{
  HANDLE v1; 
  int result;
  unsigned int i; 
  char *v8;
  struct _MODULEINFO modinfo;

  v1 = GetCurrentProcess();
  if ( GetModuleInformation(v1, (HMODULE)hModule, &modinfo, 0xCu) )
  {
    v8 = &hModule[modinfo.SizeOfImage];
 
    for ( i = (unsigned int)hModule+0x2D800; i < (unsigned int)(v8 - 4096); ++i )
    {
      if ( *(DWORD*)i == 0x8B0C5D8B &&
		  *(DWORD*)(i+4) == 0x4D8B1875 &&
		  *(DWORD*)(i+8) == 0x0FC98510 &&
		  *(DWORD*)(i+12) == 0x7D83C094 &&
		  *(DWORD*)(i+16) == 0x940F0014 && 
		  *(DWORD*)(i+20) == 0x85C208C2 &&
		  *(DWORD*)(i+24) == 0x0FF789F6 &&
		  *(DWORD*)(i+28) == 0xD408C494 )
      {
        result = i - 22;
        break;
      }
    }
  }
	return result;
}

int getMDInitAlgorithmAddress(char* hModule)
{
  HANDLE v1; 
  int result;
  unsigned int i; 
  char *v8;
  struct _MODULEINFO modinfo;

  v1 = GetCurrentProcess();
  if ( GetModuleInformation(v1, (HMODULE)hModule, &modinfo, 0xCu) )
  {
    v8 = &hModule[modinfo.SizeOfImage];
 
    for ( i = (unsigned int)hModule+0x2D800; i < (unsigned int)(v8 - 4096); ++i )
	{
		if (*(DWORD*)i == 0x08245C8D &&
			*(DWORD*)(i+4) == 0xF181D989 &&
			*(DWORD*)(i+8) == 0x0F59D8DC &&
			*(DWORD*)(i+12) == 0xA291C969 &&
			*(DWORD*)(i+16) == 0xCA89504D &&
			*(DWORD*)(i+20) == 0x5489DAF7 &&
			*(DWORD*)(i+24) == 0xCF290C24)
		{
			result = i - 45;
			break;
		}
	}
  }

  return result;
}
int getMDSecondInitAlgorithmAddress(char* hModule)
{
  HANDLE v1; 
  int result;
  unsigned int i; 
  char *v8;
  struct _MODULEINFO modinfo;

  v1 = GetCurrentProcess();
  if ( GetModuleInformation(v1, (HMODULE)hModule, &modinfo, 0xCu) )
  {
    v8 = &hModule[modinfo.SizeOfImage];
 
    for ( i = (unsigned int)hModule+0x2D800; i < (unsigned int)(v8 - 4096); ++i )
	{
		if (*(DWORD*)i == 0x8B0C5D8B &&
			*(DWORD*)(i+4) == 0xD1890855 &&
			*(DWORD*)(i+8) == 0x8908C183 &&
			*(DWORD*)(i+12) == 0x00D683DE &&
			*(DWORD*)(i+16) == 0xF083C889 &&
			*(DWORD*)(i+20) == 0x0FF00901 &&
			*(DWORD*)(i+24) == 0xF983C094 &&
			*(DWORD*)(i+28) == 0xC1970F06)
		{
			result = i - 22;
			break;
		}
	}
  }

  return result;
}
int getItunesBase64AlgorithmAddress(char* hModule)
{
  HANDLE v1; 
  int result;
  unsigned int i; 
  char *v8;
  struct _MODULEINFO modinfo;

  v1 = GetCurrentProcess();
  if ( GetModuleInformation(v1, (HMODULE)hModule, &modinfo, 0xCu) )
  {
    v8 = &hModule[modinfo.SizeOfImage];
 
    for ( i = (unsigned int)hModule+0x2D800; i < (unsigned int)(v8 - 4096); ++i )
	{
		if (*(DWORD*)i == 0xB808758B &&
			*(DWORD*)(i+4) == 0xAAAAAAAB &&
			*(DWORD*)(i+8) == 0x027A8D57 &&
			*(DWORD*)(i+12) == 0xF7F45589 &&
			*(DWORD*)(i+16) == 0xF84D89E7 &&
			*(DWORD*)(i+20) == 0x8B144D8B &&
			*(DWORD*)(i+24) == 0xC1EBD1DA &&
			*(DWORD*)(i+28) == 0x4D8902E3)
		{
			result = i - 18;
			break;
		}
	}
  }

  return result;
}
int getItunesBase64AllocateMemAddress(char* hModule)
{
  HANDLE v1; 
  int result;
  unsigned int i; 
  char *v8;
  struct _MODULEINFO modinfo;

  v1 = GetCurrentProcess();
  if ( GetModuleInformation(v1, (HMODULE)hModule, &modinfo, 0xCu) )
  {
    v8 = &hModule[modinfo.SizeOfImage];
 
    for ( i = (unsigned int)hModule+0x2D800; i < (unsigned int)(v8 - 4096); ++i )
	{
		if (*(DWORD*)i == 0xFC658351 &&
			*(DWORD*)(i+4) == 0xFC458D00 &&
			*(DWORD*)(i+8) == 0x75FF5056 &&
			*(DWORD*)(i+12) == 0x0875FF0C)
		{
			result = i - 3;
			break;
		}
	}
  }

  return result;
}
int getFreeMD_AMD_AlgorithmAddress(char* hModule)
{
  HANDLE v1; 
  int result;
  unsigned int i; 
  char *v8;
  struct _MODULEINFO modinfo;

  v1 = GetCurrentProcess();
  if ( GetModuleInformation(v1, (HMODULE)hModule, &modinfo, 0xCu) )
  {
    v8 = &hModule[modinfo.SizeOfImage];
 
    for ( i = (unsigned int)hModule+0x2D800; i < (unsigned int)(v8 - 4096); ++i )
	{
		if( *(DWORD*)i == 0x89E87D8D &&
		  *(DWORD*)(i+4) == 0xC9F181F9 &&
		  *(DWORD*)(i+8) == 0x69967BFF &&
		  *(DWORD*)(i+12) == 0xFF5A27C9 &&
		  *(DWORD*)(i+23) == 0x7589C831 &&
		  *(DWORD*)(i+27) == 0x01F183F4)
		{
			  result = i - 33;
			  break;
		}
	}
  }
  return result;
}

int getFirstgenerateParamAddress(char* hModule)
{
  HANDLE v1; 
  int result;
  unsigned int i; 

  char *v8;
  struct _MODULEINFO modinfo;

  v1 = GetCurrentProcess();
  if ( GetModuleInformation(v1, (HMODULE)hModule, &modinfo, 0xCu) )
  {
    v8 = &hModule[modinfo.SizeOfImage];
 
    for ( i = (unsigned int)hModule+0x2D800; i < (unsigned int)(v8 - 4096); ++i )
    {
      if (*(DWORD*)i == 0x83D0FF56 && 
		  *(DWORD*)(i+4) == 0x016A0CC4 && 
		  *(DWORD*)(i+8) == 0xCE8BD232 &&
		  *(DWORD*)(i+17) == 0x3204C483 &&
		  *(DWORD*)(i+21) == 0x6ACE8BD2)
      {
        result = i - 121;
        break;
      }
    }
  }
	return result;
}
int getSecondgenerateParamAddress(char* hModule)
{
  HANDLE v1; 
  int result;
  unsigned int i; 

  char *v8;
  struct _MODULEINFO modinfo;

  v1 = GetCurrentProcess();
  if ( GetModuleInformation(v1, (HMODULE)hModule, &modinfo, 0xCu) )
  {
    v8 = &hModule[modinfo.SizeOfImage];
 
    for ( i = (unsigned int)hModule+0x2D800; i < (unsigned int)(v8 - 4096); ++i )
    {
      if (*(DWORD*)i == 0x458D046A && 
		  *(DWORD*)(i+4) == 0x458D50DC && 
		  *(DWORD*)(i+16) == 0x558D0CC4 &&
		  *(DWORD*)(i+20) == 0x6ACE8BA4)
      {
        result = i - 177;
        break;
      }
    }
  }
	return result;
}
int getgenerateFirstKbsyncParamAddress(char* hModule)
{
  HANDLE v1; 
  int result;
  unsigned int i; 

  char *v8;
  struct _MODULEINFO modinfo;

  v1 = GetCurrentProcess();
  if ( GetModuleInformation(v1, (HMODULE)hModule, &modinfo, 0xCu) )
  {
    v8 = &hModule[modinfo.SizeOfImage];
 
    for ( i = (unsigned int)hModule; i < (unsigned int)(v8 - 4096); ++i )
    {
      if (*(DWORD*)i == 0xFFF0E481 && 
		  *(DWORD*)(i+4) == 0xEC81FFFF && 
		  *(DWORD*)(i+8) == 0x000001F0 &&
		  *(DWORD*)(i+12) == 0xC689310F &&
		  *(DWORD*)(i+16) == 0xCCCCCDB9 &&
		  *(DWORD*)(i+20) == 0xC1E1F7CC &&
		  *(DWORD*)(i+24) == 0x048D02EA &&
		  *(DWORD*)(i+28) == 0xC1C62992)
      {
        result = i - 6;
        break;
      }
    }
  }
	return result;
}

int getFirstGenerateAlgorithmAddr(char* hModule)
{
   int result = 0;
   result = ((int)hModule+0x1000+0x53A6D0);
   return result;
}

int getSecondGenerateAlgorithmAddr(char* hModule)
{
	int result;
	result = ((int)hModule+0x1000+0xE4AD0);
	return result;
}

int getBase64Addr(char* hModule)
{
  HANDLE v1; 
  int result;
  unsigned int i; 

  char *v8;
  struct _MODULEINFO modinfo;

  v1 = GetCurrentProcess();
  if ( GetModuleInformation(v1, (HMODULE)hModule, &modinfo, 0xCu) )
  {
    v8 = &hModule[modinfo.SizeOfImage];
 
    for ( i = (unsigned int)hModule; i < (unsigned int)(v8 - 4096); ++i )
    {
      if (*(DWORD*)i == 0xAAAAABB8 && 
		  *(DWORD*)(i+4) == 0xD1E7F7AA && 
		  *(DWORD*)(i+8) == 0xB45589EA &&
		  *(DWORD*)(i+12) == 0x8DB0558B &&
		  *(DWORD*)(i+16) == 0x0F002464 &&
		  *(DWORD*)(i+20) == 0xEF8332B6 &&
		  *(DWORD*)(i+24) == 0x42B60F03 &&
		  *(DWORD*)(i+28) == 0x08E6C101)
      {
        result = i - 125;
        break;
      }
    }
  }
	return result;
}

int getGenerateCheckacksarAlgorithmAddr(char* hModule)
{
  HANDLE v1; 
  int result;
  unsigned int i; 

  char *v8;
  struct _MODULEINFO modinfo;

	  v1 = GetCurrentProcess();
	  if ( GetModuleInformation(v1, (HMODULE)hModule, &modinfo, 0xCu) )
	  {
		v8 = &hModule[modinfo.SizeOfImage];
	 
		for ( i = (unsigned int)hModule; i < (unsigned int)(v8 - 4096); ++i )
		{
		  if (*(DWORD*)i == 0x8C2484C7 && 
			  *(DWORD*)(i+4) == 0x63000001 && 
			  *(DWORD*)(i+8) == 0x8940AE80 &&
			  *(DWORD*)(i+12) == 0x0190249C &&
			  *(DWORD*)(i+16) == 0x848D0000 &&
			  *(DWORD*)(i+20) == 0x00018824 &&
			  *(DWORD*)(i+24) == 0x24848900 &&
			  *(DWORD*)(i+28) == 0x000001A4)
		  {
			result = i - 62;
			break;
		  }
		}
	  }
	return result;
}

int getGenerateRbsyncAlgorithmAddr(char* hModule)
{
  HANDLE v1; 
  int result;
  unsigned int i; 

  char *v8;
  struct _MODULEINFO modinfo;

	  v1 = GetCurrentProcess();
	  if ( GetModuleInformation(v1, (HMODULE)hModule, &modinfo, 0xCu) )
	  {
		v8 = &hModule[modinfo.SizeOfImage];
	 
		for ( i = (unsigned int)hModule; i < (unsigned int)(v8 - 4096); ++i )
		{
		  if (*(DWORD*)i == 0x20248489 && 
			  *(DWORD*)(i+4) == 0x8D000002 && 
			  *(DWORD*)(i+8) == 0x0220249C &&
			  *(DWORD*)(i+12) == 0x92B80000 &&
			  *(DWORD*)(i+16) == 0x29000000 &&
			  *(DWORD*)(i+20) == 0x248489D8 &&
			  *(DWORD*)(i+24) == 0x00000224 &&
			  *(DWORD*)(i+28) == 0x782404C7)
		  {
			result = i - 98;
			break;
		  }
		}
	  }
	return result;
}
int releaseAllocateAddr(char* hModule)
{
	HANDLE v1; 
	int result = 0;
	unsigned int i; 

	char *v8;
	struct _MODULEINFO modinfo;

	v1 = GetCurrentProcess();
  if ( GetModuleInformation(v1, (HMODULE)hModule, &modinfo, 0xCu) )
  {
	v8 = &hModule[modinfo.SizeOfImage];
 
	for ( i = (unsigned int)hModule; i < (unsigned int)(v8 - 4096); ++i )
	{
	  if (*(DWORD*)i == 0x4589F001 && 
		  *(DWORD*)(i+4) == 0x21458DF0 && 
		  *(DWORD*)(i+8) == 0x8BF84589 &&
		  *(DWORD*)(i+12) == 0x45890845 &&
		  *(DWORD*)(i+16) == 0x2404C7EC &&
		  *(DWORD*)(i+20) == 0x524A8A6F)
	  {
		result = i - 15;
		break;
	  }
	}
  }
	return result;
}

int getGenerateSbsyncValueAddr(char* hModule)
{
	return ((int)hModule + 0x1000 + 0x341B0);
}

int getGenerateMidandOtpAlgorithmAddr(char* hModule)
{
  HANDLE v1; 
  int result;
  unsigned int i; 

  char *v8;
  struct _MODULEINFO modinfo;

	  v1 = GetCurrentProcess();
	  if ( GetModuleInformation(v1, (HMODULE)hModule, &modinfo, 0xCu) )
	  {
		v8 = &hModule[modinfo.SizeOfImage];
	 
		for ( i = (unsigned int)hModule; i < (unsigned int)(v8 - 4096); ++i )
		{
		  if (*(DWORD*)i == 0x302484C7 && 
			  *(DWORD*)(i+4) == 0x6D000010 && 
			  *(DWORD*)(i+8) == 0xC7543661 &&
			  *(DWORD*)(i+12) == 0x10342484 &&
			  *(DWORD*)(i+16) == 0xB1A40000 &&
			  *(DWORD*)(i+20) == 0x448D5435 &&
			  *(DWORD*)(i+24) == 0x44891C24 &&
			  *(DWORD*)(i+28) == 0x4C8D1C24)
		  {
			result = i - 19;
			break;
		  }
		}
	  }
	return result;
}

int releaseMidandOtpMemAddr(char* hModule)
{
	 HANDLE v1; 
  int result;
  unsigned int i; 

  char *v8;
  struct _MODULEINFO modinfo;

	  v1 = GetCurrentProcess();
	  if ( GetModuleInformation(v1, (HMODULE)hModule, &modinfo, 0xCu) )
	  {
		v8 = &hModule[modinfo.SizeOfImage];
	 
		for ( i = (unsigned int)hModule; i < (unsigned int)(v8 - 4096); ++i )
		{
		  if (*(DWORD*)i == 0x302484C7 && 
			  *(DWORD*)(i+4) == 0x04000010 && 
			  *(DWORD*)(i+8) == 0x8B000000 &&
			  *(DWORD*)(i+12) == 0x102C2484 &&
			  *(DWORD*)(i+16) == 0xF8830000 &&
			  *(DWORD*)(i+20) == 0xB182B908 &&
			  *(DWORD*)(i+28) == 0x36616DB9)
		  {
			result = i - 63;
			break;
		  }
		}
	  }
	return result;
	return 0;
}

int getGenerateClientDataAlgorithmAddr(char* hModule)
{
  HANDLE v1; 
  int result;
  unsigned int i; 

  char *v8;
  struct _MODULEINFO modinfo;

	  v1 = GetCurrentProcess();
	  if ( GetModuleInformation(v1, (HMODULE)hModule, &modinfo, 0xCu) )
	  {
		v8 = &hModule[modinfo.SizeOfImage];
	 
		for ( i = (unsigned int)hModule; i < (unsigned int)(v8 - 4096); ++i )
		{
		  if (*(DWORD*)i == 0xA5A3BFB8 && 
			  *(DWORD*)(i+4) == 0x47E6FF36 && 
			  *(DWORD*)(i+8) == 0x18458B92 &&
			  *(DWORD*)(i+12) == 0x000000C7 &&
			  *(DWORD*)(i+16) == 0xF9890000 )
		  {
			result = i - 69;
			break;
		  }
		}
	  }
	return result;

}

int getReleaseClientDataMemAddr(char* hModule)
{
  HANDLE v1; 
  int result;
  unsigned int i; 

  char *v8;
  struct _MODULEINFO modinfo;

	  v1 = GetCurrentProcess();
	  if ( GetModuleInformation(v1, (HMODULE)hModule, &modinfo, 0xCu) )
	  {
		v8 = &hModule[modinfo.SizeOfImage];
	 
		for ( i = (unsigned int)hModule; i < (unsigned int)(v8 - 4096); ++i )
		{
			 if( *(DWORD*)i == 0x89E87D8D &&
			  *(DWORD*)(i+4) == 0xC9F181F9 &&
			  *(DWORD*)(i+8) == 0x69967BFF &&
			  *(DWORD*)(i+12) == 0xFF5A27C9 &&
			  *(DWORD*)(i+23) == 0x7589C831 &&
			  *(DWORD*)(i+27) == 0x01F183F4)
			{
				  result = i - 33;
				  break;
			}
		}
	  }
	return result;
}

int getFirstAlgorithmExchangeDataAddr(char* hModule)
{
  HANDLE v1; 
  int result;
  unsigned int i; 

  char *v8;
  struct _MODULEINFO modinfo;

	  v1 = GetCurrentProcess();
	  if ( GetModuleInformation(v1, (HMODULE)hModule, &modinfo, 0xCu) )
	  {
		v8 = &hModule[modinfo.SizeOfImage];
	 
		for ( i = (unsigned int)hModule; i < (unsigned int)(v8 - 4096); ++i )
		{
			 if( *(DWORD*)i == 0xC083C889 &&
			  *(DWORD*)(i+4) == 0x83DE8908 &&
			  *(DWORD*)(i+8) == 0xC28900D6 &&
			  *(DWORD*)(i+12) == 0x0901F283 &&
			  *(DWORD*)(i+16) == 0xC2940FF2 &&
			  *(DWORD*)(i+20) == 0x0F06F883 &&
			  *(DWORD*)(i+24) == 0xF685C097)
			{
				  result = i - 28;
				  break;
			}
		}
	  }
	return result;
}

int getSetMachineProvisionDataAddr(char* hModule)
{
	 HANDLE v1; 
  int result;
  unsigned int i; 

  char *v8;
  struct _MODULEINFO modinfo;

	  v1 = GetCurrentProcess();
	  if ( GetModuleInformation(v1, (HMODULE)hModule, &modinfo, 0xCu) )
	  {
		v8 = &hModule[modinfo.SizeOfImage];
	 
		for ( i = (unsigned int)hModule; i < (unsigned int)(v8 - 4096); ++i )
		{
			 if( *(DWORD*)i == 0xE8C1D089 &&
			  *(DWORD*)(i+4) == 0x61F08318 &&
			  *(DWORD*)(i+8) == 0x10244488 &&
			  *(DWORD*)(i+12) == 0xE8C1D089 &&
			  *(DWORD*)(i+16) == 0x00D23510 &&
			  *(DWORD*)(i+20) == 0x44880000 &&
			  *(DWORD*)(i+24) == 0xD0891124)
			{
				  result = i - 101;
				  break;
			}
		}
	  }
	return result;
}

unsigned int GetFunctionAddress_3(char *hModule)// Get GUID Address.....
{
  HANDLE v1;
  unsigned int result; 
  unsigned int v3; 
  unsigned int i;
  unsigned int j;
  unsigned int k;
  unsigned int l;
  char *v8;
  struct _MODULEINFO modinfo;
  unsigned int v10;
  int v11;

  v1 = GetCurrentProcess();
  if ( GetModuleInformation(v1, (HMODULE)hModule, &modinfo, 0xCu) )
  {
    v8 = &hModule[modinfo.SizeOfImage];
    v3 = 0;
    for ( i = (unsigned int)hModule; i < (unsigned int)(v8 - 4096); ++i )
    {
      if ( *(DWORD*)i == 0x8489C433 &&
           *(DWORD*)(i+4) == 0x0000F824 &&
           *(DWORD*)(i+8) == 0xC0325300 &&
           *(DWORD*)(i+12) == 0x08244C89 &&
		   *(DWORD*)(i+16) == 0x84885756 &&
		   *(DWORD*)(i+20) == 0x00008024 &&
		   *(DWORD*)(i+24) == 0x8DDB3300 &&
		   *(DWORD*)(i+28) == 0x84002464 )
      {
        result = i - 17;
        break;
      }
    }
  }
  
  return result;

}



int  NewItunesFunctionAddress_1(char *hModule)
{
  HANDLE v1;
  int result;
  unsigned int v3; 
  unsigned int i; 
  struct _MODULEINFO modinfo;

  v1 = GetCurrentProcess();
  if ( GetModuleInformation(v1, (HMODULE)hModule, &modinfo, 0xCu) )
  {
    v3 = 0;
    for ( i = (unsigned int)hModule; i < (unsigned int)&hModule[modinfo.SizeOfImage - 4096]; ++i )
    {
		  if ( *(DWORD *)i == 0x89F04D8D && 
		   *(DWORD*)(i+4) == 0xB7F281CA &&
		   *(DWORD*)(i+8) == 0x691BC60A &&
		   *(DWORD*)(i+12) == 0x32565BD2 &&
		   *(DWORD*)(i+16) == 0x02728D7D)//5B57FE81
		  {
			  result = i - 35;
			  break;
		  }
    }
  }
  return result;
}

int NewItunesFunctionAddress_2(HMODULE hModule, int& a2)
{
  HANDLE v2;
  int result = 0;
  unsigned int i = 0;

  struct _MODULEINFO modinfo;

  v2 = GetCurrentProcess();
  if ( GetModuleInformation(v2, hModule, &modinfo, 0xCu) )
  {

    for ( i = (unsigned int)hModule; i < (unsigned int)&hModule[modinfo.SizeOfImage - 4096]; ++i )
    {
      if (*(DWORD*)i == 0x782484C7 && 
		 *(DWORD*)(i+4) == 0x42000001 && 
		 *(DWORD*)(i+8) == 0x894B4471 &&
		 *(DWORD*)(i+12) == 0x0174249C &&
		 *(DWORD*)(i+16) == 0x8C8D0000 &&
		 *(DWORD*)(i+20) == 0x00019024 &&
		 *(DWORD*)(i+24) == 0x89C88900 &&
		 *(DWORD*)(i+28) == 0xCBD735CF &&
		 *(DWORD*)(i+32) == 0xC8695B6E)
      {
		  result = i - 103;
		  break;
      }

    }//end for

	for ( i = (unsigned int)hModule; i < (unsigned int)&hModule[modinfo.SizeOfImage - 4096]; ++i )
	{
		if (*(DWORD*)i == 0xFFF0E481 && 
		 *(DWORD*)(i+4) == 0xEC81FFFF && 
		 *(DWORD*)(i+8) == 0x000001A0 &&
		 *(DWORD*)(i+12) == 0xC689310F &&
		 *(DWORD*)(i+16) == 0xAAAAABB9 &&
		 *(DWORD*)(i+20) == 0xC1E1F7AA &&
		 *(DWORD*)(i+24) == 0xC26B02EA &&
		 *(DWORD*)(i+28) == 0x29F18906 &&
		 *(DWORD*)(i+32) == 0x04E1C1C1)
      {
		  a2 = i - 6;
		  break;
      }

	}
	
  }

  return result;

}


int NewItunesFunctionAddress_3(char *hModule)
{
  HANDLE v1; 
  int result;
  unsigned int i;

  struct _MODULEINFO modinfo;

  v1 = GetCurrentProcess();
  if ( GetModuleInformation(v1, (HMODULE)hModule, &modinfo, 0xCu) )
  {
    for ( i = (unsigned int)hModule; i < (unsigned int)&hModule[modinfo.SizeOfImage - 4096]; ++i )
    {
      if (*(DWORD*)i == 0x9E75F869 && 
		  *(DWORD*)(i+4) == 0x7C894B59 && 
		  *(DWORD*)(i+8) == 0xD80D3824 && 
		  *(DWORD*)(i+12) == 0x8905C720 &&
		  *(DWORD*)(i+16) == 0x69342444 &&
		  *(DWORD*)(i+20) == 0xA6619BC0)
      {
		 result = i - 48;
         break;
      }
    }
  }
  return result;
}


int NewItunesRegistyAddress_1(char* hModule)
{
  HANDLE v1; 
  int result;
  unsigned int v4;
  unsigned int v5;
  unsigned int i;
  struct _MODULEINFO modinfo;

   v1 = GetCurrentProcess();
  if ( GetModuleInformation(v1, (HMODULE)hModule, &modinfo, 0xCu) )
  {
	v4 = 0;
    for ( i = (unsigned int)hModule; i < (unsigned int)&hModule[modinfo.SizeOfImage - 4096]; ++i )
	{
		if (*(DWORD*)i == 0x8BB84D8B &&
			*(DWORD*)(i+4) == 0x518D4041 &&
			*(DWORD*)(i+8) == 0x6C858952 &&
			*(DWORD*)(i+12) == 0x8BFFFFFF && 
			*(DWORD*)(i+16) == 0x4D8D4441 &&
			*(DWORD*)(i+20) == 0x70858994 &&
			*(DWORD*)(i+24) == 0xE8FFFFFF)
		{
			////目的地址 = 相对偏移地址 + 当前地址 + 5
			v4 = i + 56;
			v5 = i + 55;
			result = *(DWORD*)v4 + v5 + 5;

			/*v4 = i + 67;
			v5 = i + 66;
			*addr2 = *(DWORD*)v4 + v5 + 5;*/
			break;
		}
	}
  }

  return result;
}

int NewItunesRegistyAddress_2(char* hModule)
{
  HANDLE v1; 
  int result;
  unsigned int v4;
  unsigned int v5;
  unsigned int i;
  struct _MODULEINFO modinfo;

   v1 = GetCurrentProcess();
  if ( GetModuleInformation(v1, (HMODULE)hModule, &modinfo, 0xCu) )
  {
	v4 = 0;
    for ( i = (unsigned int)hModule; i < (unsigned int)&hModule[modinfo.SizeOfImage - 4096]; ++i )
	{
		if (*(DWORD*)i == 0x8BB84D8B &&
			*(DWORD*)(i+4) == 0x518D4041 &&
			*(DWORD*)(i+8) == 0x6C858952 &&
			*(DWORD*)(i+12) == 0x8BFFFFFF && 
			*(DWORD*)(i+16) == 0x4D8D4441 &&
			*(DWORD*)(i+20) == 0x70858994 &&
			*(DWORD*)(i+24) == 0xE8FFFFFF)
		{
			////目的地址 = 相对偏移地址 + 当前地址 + 5
			v4 = i + 67;
			v5 = i + 66;
			result = *(DWORD*)v4 + v5 + 5;
			break;
		}
	}
  }

  return result;
}

int NewItunesRegistyAddress_3(char* hModule)
{
	int Result = 0;
	Result = (int)hModule + 0x1000 + 0x2FE20;

	return Result;
}

int NewITunesEncyptRegisty_Address_4(char* hModule)
{
  HANDLE v1; 
  int result;
  int v3;
  unsigned int v4;
  unsigned int i;
 
  struct _MODULEINFO modinfo;
   v1 = GetCurrentProcess();
  if ( GetModuleInformation(v1, (HMODULE)hModule, &modinfo, 0xCu) )
  {
	v4 = 0;
	for ( i = (unsigned int)hModule; i < (unsigned int)&hModule[modinfo.SizeOfImage - 4096]; ++i )
	{
		if (*(DWORD*)i == 0xFFFFFD3D
			&& *(DWORD*)(i+4) == 0x830476BF
			&& *(DWORD*)(i+8) == 0x8DC3FFC8)
		{
			v4 = i + 16;
			break;
		}
	}

	if (v4)
	{
		if (*(DWORD*)v4 == 0xF7AAAAAA && *(DWORD*)(v4+4) == 0xD1C28BE1)
		{
			v3 = v4 + 16;
		}

		if (*(DWORD*)v3 == 0x76FCF883)
		{
			result = v3 + 32;
		}
	}
  }

  return result;
}

int NewItunesEncyptMainRegistyParam_Addr_5(char* hModule)
{
  HANDLE v1; 
  int result;
  int v3;
  unsigned int v4;
  unsigned int i;
  unsigned int j;
 
  struct _MODULEINFO modinfo;
  
  v1 = GetCurrentProcess();
  if ( GetModuleInformation(v1, (HMODULE)hModule, &modinfo, 0xCu) )
  {
	v4 = 0;
	for ( i = (unsigned int)hModule; i < (unsigned int)&hModule[modinfo.SizeOfImage - 4096]; ++i )
	{
		if (*(DWORD*)i == 0x51EC8B55
			&& *(DWORD*)(i+4) == 0x105D8B53
			&& *(DWORD*)(i+8) == 0x0A75DB85)
		{
			v4 = i + 32;
			break;
		}
	}

	if (v4)
	{
		v3 = 0;
		for (j=v4; ; ++j)
		{
			if (*(DWORD*)j == 0x10558BFC
				&& *(DWORD*)(j+4) == 0x9B89C8A1)
			{
				v3 = j + 32;
				break;
			}
		}

		if (*(DWORD*)v3 == 0x8D0B75F6)
		{
			result = v3 + 32;
		}
	}
  }

  return result;
}

//////////sub_104A3210 function
int iTunesRegisterSecondXAppleSignatureAddr_1(char* hModule)
{
   HANDLE v1; 
   int result;
   int v3;
   unsigned int v4;
   unsigned int i;
   unsigned int j;
   
   struct _MODULEINFO modinfo;
  
   v1 = GetCurrentProcess();
	if ( GetModuleInformation(v1, (HMODULE)hModule, &modinfo, 0xCu) )
	{
		v4 = 0;
		for ( i = (unsigned int)hModule; i < (unsigned int)&hModule[modinfo.SizeOfImage - 4096]; ++i )
		{
			if (*(DWORD*)i == 0xC4830098 &&
				*(DWORD*)(i+4) == 0x0C458B04 &&
				*(DWORD*)(i+8) == 0x840FC085)
			{
				v4 = i;
				break;
			}
		}

		if (v4)
		{
			v3 = 0;
			for (j=v4; ; j++)
			{
				if (*(DWORD*)j == 0x04C48300 &&
					*(DWORD*)(j+4) == 0x1024448B &&
					*(DWORD*)(j+8) == 0xC085F08B)
				{
					v3 = j + 32;
					break;
				}
			}

			if (*(DWORD*)v3  == 0x04C48300 && *(DWORD*)(v3+4) == 0xDB851B8B && *(DWORD*)(v3+8)==0x4C8B1774)
			{
				result = v3 + 48;
			}
		}
	 }
   return result;
}

///sub_104A44D0 function
int iTunesRegisterSecondXAppleSignatureAddr_2(char* hModule)
{
   HANDLE v1; 
   int result;
   int v3;
   unsigned int v4;
   unsigned int i;
   unsigned int j;
  
   struct _MODULEINFO modinfo;
  
   v1 = GetCurrentProcess();
   if ( GetModuleInformation(v1, (HMODULE)hModule, &modinfo, 0xCu) )
   {
		v4 = 0;
		for ( i = (unsigned int)hModule; i < (unsigned int)&hModule[modinfo.SizeOfImage - 4096]; ++i )
		{
			if (*(DWORD*)i == 0x8504C483 && *(DWORD*)(i+4) == 0x8B4F74F6 && *(DWORD*)(i+8) == 0x558DFC45)
			{
				v4 = i+48;
				break;
			}
		}

		if (v4)
		{
			v3 = 0;
			for (j=v4; ; ++j)
			{
				if (*(DWORD*)j == 0xE856C35D && *(DWORD*)(j+4) == 0x0097FC27)
				{
					v3 = j + 16;
					break;
				}
			}

			if (*(DWORD*)v3 == 0x8B5B08C0)
			{
				result = v3 + 16;
			}
		}
   }
   return result;
}

////sub_104A4410
int iTunesRegisterSecondXAppleSignatureAddr_3(char* hModule)
{
   HANDLE v1; 
   int result;
   int v3;
   unsigned int v4;
   unsigned int i;
   unsigned int j;
   int v7;
   int v8;
   struct _MODULEINFO modinfo;
  
   v1 = GetCurrentProcess();
   if ( GetModuleInformation(v1, (HMODULE)hModule, &modinfo, 0xCu) )
   {
		v4 = 0;
		for ( i = (unsigned int)hModule; i < (unsigned int)&hModule[modinfo.SizeOfImage - 4096]; ++i )
		{
			if (*(DWORD*)i == 0xFF500674 && *(DWORD*)(i+4) == 0x04C483D7 && *(DWORD*)(i+8) == 0x3BE4458B)
			{
				v4 = i+16;
				break;
			}
		}

		if (v4)
		{
			v3 = 0;
			for (j=v4; ; ++j)
			{
				if (*(DWORD*)j == 0x5010458D && *(DWORD*)(j+4) == 0x89D44D8D && *(DWORD*)(j+8) == 0x17E8105D)
				{
					v3 = j + 32;
					break;
				}
			}
			v7 = 0;
			if (*(DWORD*)v3 == 0x0098708C)
			{
				v7 = v3 + 32; 
				v8 = 0;
				if (*(DWORD*)v7 == 0x1274FF85 && *(DWORD*)(v7+4) == 0x40D3FF57)
				{
					v8 = v7 + 64;
				}
				if (*(DWORD*)v8 == 0xC085008B && *(DWORD*)(v8+4) == 0x78830974)
				{
					result = v8 + 64;
				}
			}
		}
   }
   return result;
}

int iTunesRegisterSecondGenerateAlgorithmFun(char* hModule)
{
   HANDLE v1; 
   int result = 0;
   int v3;
   unsigned int v4;
   unsigned int i;
   unsigned int j;
 
   struct _MODULEINFO modinfo;
  
   v1 = GetCurrentProcess();
   if ( GetModuleInformation(v1, (HMODULE)hModule, &modinfo, 0xCu) )
   {
		v4 = 0;
		for ( i = (unsigned int)hModule; i < (unsigned int)&hModule[modinfo.SizeOfImage - 4096]; ++i )
		{
			if (*(DWORD*)i == 0x458B5008 && *(DWORD*)(i+4) == 0x4E8B5108 && *(DWORD*)(i+8) == 0x51505224)
			{
				v4 = i+32;
				break;
			}
		}

		if (v4)
		{
			v3 = 0;
			for (j=v4; ; ++j)
			{
				if (*(DWORD*)j == 0x14C48300 && *(DWORD*)(j+4) == 0x636B3E81)
				{
					v3 = j + 48;
					break;
				}
			}

			if (*(DWORD*)v3 == 0x10C25D5B)
			{
				result = v3 + 16;
			}
		}
   }
   return result;
}


int iTunesRegisterDealFunAfterXSignature(char* hModule)
{
	HANDLE v1; 
   int result = 0;
   unsigned int v4;
   unsigned int i;
 
   struct _MODULEINFO modinfo;
  
   v1 = GetCurrentProcess();
   if ( GetModuleInformation(v1, (HMODULE)hModule, &modinfo, 0xCu) )
   {
		v4 = 0;
		for ( i = (unsigned int)hModule; i < (unsigned int)&hModule[modinfo.SizeOfImage - 4096]; ++i )
		{
			if (*(DWORD*)i == 0x104D8D0C && *(DWORD*)(i+4) == 0xC7505251 && *(DWORD*)(i+8) == 0x00001045)
			{
				v4 = i-32;
				break;
			}
		}

		if (v4)
		{	
			result = v4;
		}

   }
	return result;
}

////////////得到有具有8个参数的变形算法函数，这个函数来自itunes
int Get8ParamterDeformAlgorithmFun(char* hModule)
{
   HANDLE v1; 
   int result = 0;
   unsigned int v4;
   unsigned int v5;
   unsigned int i;
   struct _MODULEINFO modinfo;
   v1 = GetCurrentProcess();
   if ( GetModuleInformation(v1, (HMODULE)hModule, &modinfo, 0xCu) )
   {
		v4 = 0;
		for ( i = (unsigned int)hModule; i < (unsigned int)&hModule[modinfo.SizeOfImage - 4096]; ++i )
		{
			if (*(DWORD*)i== 0xFF047E89 && 
				*(DWORD*)(i+4) == 0x46FF2075 && 
				*(DWORD*)(i+8) == 0x1C75FF08 &&
				*(DWORD*)(i+12) == 0xFF1875FF &&
				*(DWORD*)(i+16) == 0x75FF1475 && 
				*(DWORD*)(i+20) == 0x2476FF10 && 
				*(DWORD*)(i+24) == 0xFF0C75FF)
			{
				v4 = i + 31;
				v5 = i + 30;
				result = *(DWORD*)v4 + v5 + 5;
				break;
			}
		}
   }
   return result;
}


int GetFreeMemoryAlgorithmAddr(char* hModule)
{
   HANDLE v1; 
   int result = 0;
   unsigned int v4;
   unsigned int i;
 
   struct _MODULEINFO modinfo;
   v1 = GetCurrentProcess();
   if ( GetModuleInformation(v1, (HMODULE)hModule, &modinfo, 0xCu) )
   {
		v4 = 0;
		for ( i = (unsigned int)hModule; i < (unsigned int)&hModule[modinfo.SizeOfImage - 4096]; ++i )
		{
			if (*(DWORD*)i == 0x89E87D8D && 
				*(DWORD*)(i+4) == 0x11F181F9 && 
				*(DWORD*)(i+8) == 0x691CE41B &&
				*(DWORD*)(i+12) == 0x77B0E5D1 &&
				*(DWORD*)(i+23) == 0x4589D031 && 
				*(DWORD*)(i+27) == 0xEC7589F0)
			{
				result = i - 33;
				break;
			}
		}
   }

   return result;
}

////得到注册时，进入继续页面后，服务器返回的X-Apple-ActionSignature的值进行变形的算法函数地址
int GetRegistySingatureDeformAlgorithmAddr(char* hModule)
{
   int result = 0;
   result = (int)hModule + 0x1000 + 0x46600;
   return result;
}

//////注册时，把从网页得到的XAppleActionSignature重新编码函数地址
int GetRegistyEncodeActionSignatureAddr(char* hModule)
{
   HANDLE v1; 
   int result = 0;
   unsigned int v4;
   unsigned int i;
   struct _MODULEINFO modinfo;
   v1 = GetCurrentProcess();
   if ( GetModuleInformation(v1, (HMODULE)hModule, &modinfo, 0xCu) )
   {
	   v4 = 0;
	   for ( i = (unsigned int)hModule; i < (unsigned int)&hModule[modinfo.SizeOfImage - 4096]; ++i )
	   {
			if (*(DWORD*)i == 0x44C605EB && *(DWORD*)(i+4) == 0x833D020B && *(DWORD*)(i+8) == 0x04C603C1)
			{
				v4 = i + 12;
				break;
			}
	   }

	   if (*(DWORD*)v4 == 0x8B413D0B)
	   {
		   result = v4 + 32;
	   }
   }
   return result;
}


int GetgenerateBoundaryRandomAddr(char* hModule)
{
   HANDLE v1; 
   int result = 0;
   unsigned int i;
   unsigned int v4;
   unsigned int v5;
   struct _MODULEINFO modinfo;
   v1 = GetCurrentProcess();
   if ( GetModuleInformation(v1, (HMODULE)hModule, &modinfo, 0xCu) )
   {
	   v4 = 0;
	   for ( i = (unsigned int)hModule; i < (unsigned int)&hModule[modinfo.SizeOfImage - 4096]; ++i )
	   {
			if (*(DWORD*)i == 0xFCB48589 && 
				*(DWORD*)(i+4) == 0x85C7FFFF &&
				*(DWORD*)(i+8) == 0xFFFFFCA8 &&
				*(DWORD*)(i+12) == 0x0C108047 &&
				*(DWORD*)(i+16) == 0xFCAC9D89 &&
				*(DWORD*)(i+20) == 0x478BFFFF)
			{
				v4 = i + 81;
				v5 = i + 80;
				result = *(DWORD*)v4 + v5 + 5;
				break;
			}
	   }

   }
   return result;
}

int KeAllocateFunAddr(char* hModule)
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
			if (*(DWORD*)i == 0x8BEC8B55
				&& *(DWORD*)(i+4) == 0xAF0F0C45
				&& *(DWORD*)(i+8) == 0xC0851045
				&& *(DWORD*)(i+18) == 0x50006A00)
			{
				result = i;
				break;
			}
		}

   }

   return result;
}


int KeDeAllocateFunAddr(char* hModule)
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
			if (*(DWORD*)i == 0x8BEC8B55 && *(DWORD*)(i+4) == 0x0D8B0C45 && *(DWORD*)(i+12) == 0x5250118B)
			{
				result = i;
				break;
			}
		}

   }
   return result;
}

int KeAllocateRealocateFunAddr(char* hModule)
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
			if (*(DWORD*)i == 0x8BEC8B55
				&& *(DWORD*)(i+4) == 0xAF0F1045
				&& *(DWORD*)(i+8) == 0xC0851445
				&& *(DWORD*)(i+28) == 0x458B5000
				&& *(DWORD*)(i+32) == 0xFF52500C)
			{
				result = i;
				break;
			}
		}

   }
   return result;
}

int KeDecodeGzipFunAddr(char* hModule)
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
			if (*(DWORD*)i == 0x83EC8B55
				&& *(DWORD*)(i+4) == 0x56533CEC
				&& *(DWORD*)(i+8) == 0x0000BB57
				&& *(DWORD*)(i+12) == 0x6A530010
				&& *(DWORD*)(i+16) == 0x6AF08B01)
			{
				result = i;
				break;
			}
		}

   }
   return result;
}

/////////////////得到boundary的值
char* GetBoundaryValue()
{
	HMODULE hModule = NULL;
	hModule = GetModuleHandle(_T("iTunesCore.dll"));
	if (!hModule)
	{
		return NULL;
	}

	int iTunesgetBoundaryRandomAddr = GetgenerateBoundaryRandomAddr((char*)hModule);
	_asm
	{
		xor eax,eax
		lea ecx,dword ptr ss:[ebp-0x21c]
		mov dword ptr ss:[ebp-0x22c],eax
		mov dword ptr ss:[ebp-0x228],eax
		mov dword ptr ss:[ebp-0x224],eax
		mov dword ptr ss:[ebp-0x220],eax
	//	mov edx,iTunesgetBoundaryRandomAddr
	//	call edx
		mov byte ptr ss:[ebp-0x20c],0x20
		lea eax,dword ptr ss:[ebp-0x20b]
		lea ecx,dword ptr ss:[ebp-0x21a]
		mov edi,0x4
		mov edi,edi
loop_fun:
		movzx esi,byte ptr ds:[ecx-0x2]
		mov edx,esi
		shr edx,0x4
		movzx edx,byte ptr ds:[strBoundary+edx]
		mov byte ptr ds:[eax],dl
		and esi,0x0f
		movzx edx,byte ptr ds:[strBoundary+esi]
		movzx esi,byte ptr ds:[ecx-1]
		mov byte ptr ds:[eax+1],dl
		mov edx,esi
		shr edx,0x4
		movzx edx,byte ptr ds:[strBoundary+edx]
		mov byte ptr ds:[eax+2],dl
		and esi,0x0f
		movzx edx,byte ptr ds:[strBoundary+esi]
		movzx esi,byte ptr ds:[ecx]
		mov byte ptr ds:[eax+3],dl
		mov edx,esi
		shr edx,0x4
		movzx edx,byte ptr ds:[strBoundary+edx]
		mov byte ptr ds:[eax+4],dl
		and esi,0x0f
		movzx edx,byte ptr ds:[strBoundary+esi]
		movzx esi,byte ptr ds:[ecx+1]
		mov byte ptr ds:[eax+5],dl
		mov edx,esi
		shr edx,0x4
		movzx edx,byte ptr ds:[strBoundary+edx]
		mov byte ptr ds:[eax+6],dl
		and esi,0x0f
		movzx edx,byte ptr ds:[strBoundary+esi]
		mov byte ptr ds:[eax+7],dl
		add eax,0x8
		add ecx,0x4
		dec edi
		jnz loop_fun
		lea eax,dword ptr ss:[ebp-0x20b]
	}
}


//得到准备注册之前的SignsapSetupBuffer值
int GetRegisterSignsapSetupBuffer(IN LPVOID lpGsaServices,
								  LPVOID lpBuffer,
								  int signSize,
							      LPVOID lpXAppleActionSignature,
							      int& outSignLen)
{
	HMODULE hModule = NULL;
	int nValue = 0;
	int lastparam = 0;
	DWORD paramAddr_1;
	int OutParam_1 = 0;
	int OutParam_2 = 0;
	int paramAddr_2[2] = {0};//这个参数值很重要的
	int IntSignsap[1024] = {0};
	char strSignBuf[1024] = {0};
	int itunes8ParamAddr = 0;
	hModule = GetModuleHandle(_T("iTunesCore.dll"));
	if (!hModule)
	{
		return -1;
	}

	///计算这两个函数的地址，之后再调用
	int MemFreeAlgorithmAddr = GetFreeMemoryAlgorithmAddr((char*)hModule);

	EnterCriticalSection(&g_cs);
	//decodeBase64，解码成参与计算的数据
	nValue = SignSapCertSignature((int)lpBuffer,signSize,(int)IntSignsap);

	_asm
	{
		mov lastparam,0x601
		lea edx,lastparam
		push edx
		lea eax,OutParam_1
		push eax
		lea ecx,OutParam_2
		push ecx
		mov edx,nValue
		push edx
		lea eax,IntSignsap
		push eax
		mov ecx,g_sessionId
		push ecx
		lea eax,g_hw_info[0]
		push eax
		push 0xD2
		mov edx,itunes8ParamAddr
		call edx
		add esp,0x20
	}
	
	//base64加密数据
	int Res = GenerateRealKbsyncValue(OutParam_2,OutParam_1,(int)strSignBuf);// strSignBuf为得到的字符串地址，这个函数是用的登录时候的，注册的时候能不能通过，目前还不知道？
	memcpy(lpXAppleActionSignature,strSignBuf,strlen(strSignBuf));
	outSignLen = strlen(strSignBuf);
	
	_asm
	{
		mov eax,OutParam_2
		push eax
		mov edx,MemFreeAlgorithmAddr
		call edx
		add esp,0x4
		mov ecx,eax
	}

	LeaveCriticalSection(&g_cs);
	
	return 0;  //为零为正常返回
}


///得到注册时真正的X-apple-signature的值，只有这个值正确了，注册才能跳转到继续页面往下进行注册
int GetRegisterSignupWizardXappleSignature(IN LPVOID lpGsaServices,
										   IN LPVOID lpBuffer,
										   IN int inLength,
										   OUT LPVOID sXappleSignature,
										   OUT int& signDataLen)
{

	HMODULE hModule = NULL;
	int lastparam;
	int nValue = 0;
	int OutParam_1 = 0;
	int OutParam_2 = 0;
	int paramAddr_2[2] = {0};//这个参数值很重要的
	int IntSignsap[2048] = {0};
	char strSignBuf[2048] = {0};
	char OutSignatureBuf[2048] = {0};//真正生成signature字符串的缓冲区
	int OutBuf_1 = 0;
	int OutBuf_2 = 0;
	
	hModule = ::GetModuleHandle(_T("iTunesCore.dll"));
	if (!hModule)
	{
		return -1;
	}

	//得到函数地址
	int itunes8ParamAddr = g_itune8ParamAddr;//
	int iTunesFunForRegistyAddr_3 = NewItunesRegistyAddress_3((char*)hModule);
	int MemFreeAlgorithmAddr = GetFreeMemoryAlgorithmAddr((char*)hModule);

	EnterCriticalSection(&g_cs);
	//decodeBase64数据
	nValue = SignSapCertSignature((int)lpBuffer,inLength,(int)IntSignsap);//这里需要详细注意下，这个算法和登录的是相同的

	_asm
	{
		mov lastparam,0x601
		lea edx,lastparam
		push edx
		lea eax,OutParam_1
		push eax
		lea ecx,OutParam_2
		push ecx
		mov edx,nValue
		push edx
		lea eax,IntSignsap
		push eax
		mov ecx,g_sessionId
		push ecx
		mov eax,g_paramAddr_1
		push eax
		push 0xD2
		mov edx,itunes8ParamAddr
		call edx
		add esp,0x20
	}

	_asm
	{
		lea ecx,OutBuf_2
		push ecx
		lea eax,OutBuf_1
		push eax
		push 0
		push 0
		push 0x64
		mov ecx,g_sessionId
		push ecx
		mov edx,iTunesFunForRegistyAddr_3
		call edx
		add esp,0x18
	}

	
	memset(OutSignatureBuf,0x0000,sizeof(OutSignatureBuf));
	//调用加密算法生成signature字符串
	int Res = GenerateRealKbsyncValue(OutBuf_1,OutBuf_2,(int)OutSignatureBuf);
	//这里处理得到的字符串，回传给参数供外部使用
	memcpy(sXappleSignature,OutSignatureBuf,strlen(OutSignatureBuf));
	signDataLen = strlen(OutSignatureBuf);

	LeaveCriticalSection(&g_cs);

	return 0;

}

/////设置从网页来的X-Apple-ActionSignature的值
int SetXappleActionSignatureForPage(IN LPVOID lpGsaServices,LPVOID lpXAppleActionSignature,int signLen)
{
	HMODULE hModule = NULL;
	int IntSignsap[1024] = {0};
	int nValue = 0;
	int param_1 = 0;
	int param_2 = 0;

	hModule = GetModuleHandle(_T("iTunesCore.dll"));
	if (!hModule)
	{
		return -1;
	}

	int DeformAlgorithmAddr = GetRegistySingatureDeformAlgorithmAddr((char*)hModule);	
	nValue = SignSapCertSignature((int)lpXAppleActionSignature,signLen,(int)IntSignsap);
	
	EnterCriticalSection(&g_cs);

	_asm
	{
		mov eax,param_1
		push eax
		mov ecx,param_2
		push ecx
		mov edx,nValue
		push edx
		lea eax,IntSignsap
		push eax
		mov ecx,g_sessionId
		push ecx
		call DeformAlgorithmAddr
		add esp,0x14
		mov edi,eax
	}

	LeaveCriticalSection(&g_cs);

	return 0;

}

//同意页面发送需要的xAppleActionSignature的值
int GenerateAgreeWebPageXAppleActionSignature(IN LPVOID lpGsaServices,
											  IN LPVOID lpAgreeWebPageUUID,
											  IN LPVOID lpUdid,
											  IN LPVOID lpAppleId,
											  OUT LPVOID lpXAppleSignature,
											  OUT int& signDataLen)
{
	HMODULE hModule = NULL;
	char strSignBuf[2048] = {0};//分配空间
	int OutBuf_1 = 0;
	int OutBuf_2 = 0;
	char szData[MAX_PATH] = {0};
	int dataLen = 0;


	hModule = GetModuleHandle(_T("iTunesCore.dll"));
	if (!hModule)
	{
		return -1;
	}

	if (!lpAgreeWebPageUUID)
	{
		return -1;
	}

	if (!lpAppleId)
	{
		sprintf(szData,"%s%s",(char*)lpAgreeWebPageUUID,(char*)lpUdid);
	}
	else
	{
		sprintf(szData,"%s%s%s",(char*)lpAgreeWebPageUUID,(char*)lpUdid,(char*)lpAppleId);
	}

	
	dataLen = strlen(szData);

	EnterCriticalSection(&g_cs);

	int tempAddress = NewItunesFunctionAddress_3((char*)hModule);
	_asm
	{
		lea eax,OutBuf_1
		push eax
		lea ecx,OutBuf_2
		push ecx
		mov edx,dataLen
		push edx
		lea eax,szData[0]
		push eax
		mov ecx,g_sessionId
		push ecx
		mov edx,tempAddress
		call edx
		add esp,0x14
	}	
	int Res = GenerateRealKbsyncValue(OutBuf_2,OutBuf_1,(int)strSignBuf);// strSignBuf为得到的字符串地址
	
	dataLen = strlen(strSignBuf);
	memcpy(lpXAppleSignature,strSignBuf,dataLen);

	LeaveCriticalSection(&g_cs);

	return 0;
}


/////////////////////////////////////////
int GetGenerateKbsyncPargam(int& _OutParam_1,int& _OutParam_2,long _dsid,int thirdParam,int _address_2,int flag)
{

	int _Param_1 = _OutParam_1;
	int _Param_2 = _OutParam_2;
	_asm
	{
		lea ecx,_Param_1
		push ecx
		lea eax,_Param_2
		push eax
		push flag//1
		push 0
		push thirdParam//2
		push _dsid
		mov ecx,g_firstkbsyncParamFunAddr
		push ecx
		mov edx,_address_2
		call edx
		add esp, 0x1C
	}
	_OutParam_1 = _Param_1;
	_OutParam_2 = _Param_2;
}

int GetEachChar(int a1, int a2, signed int a3)
{
  int result;
  char v4;
  char v5; 

  *(BYTE *)a2 = kbValue[(signed int)*(BYTE *)a1 >> 2];
  *(BYTE *)(a2 + 1) = kbValue[((*(BYTE *)(a1 + 1) & 0xF0) >> 4) | 16 * (*(BYTE *)a1 & 3)];
  if ( a3 <= 1 )
  {
	  v5 = 61;
  }
  else
    v5 = kbValue[((*(BYTE *)(a1 + 2) & 0xC0) >> 6) | 4 * (*(BYTE *)(a1 + 1) & 0xF)];
  *(BYTE *)(a2 + 2) = v5;
  if ( a3 <= 2 )
    v4 = 61;
  else
    v4 = kbValue[*(BYTE *)(a1 + 2) & 0x3F];
  result = a2;
  *(BYTE *)(a2 + 3) = v4;
  return result;
}


int GenerateRealKbsyncValue(int a1, int a2, int a3)
{
  int result;
  int v4;
  signed int v5;
  char v6[4];
  int v7;

  result = a3;
  v7 = a3;
  while ( a2 > 0 )
  {
    v4 = 0;
    v5 = 0;
    while ( v5 < 3 )
    {
      if ( a2 <= 0 )
      {
        v6[v5] = 0;
      }
      else
      {
        result = v4++ + 1;
		LOBYTE(result);
        result = *(BYTE *)(v5 + a1);
	    v6[v5] = result;
      }
      ++v5;
      --a2;
    }
    a1 += 3;
    if ( v4 )
    {
      int bResult = GetEachChar((int)v6, v7,v4);
      result = v7 + 4;
      v7 += 4;
    }
  }
  *(BYTE *)v7 = 0;
  return result;
}

//注册时第一次需要发送的sign-sap-setup-buffer的生成算法
int AsmGenerateRegistyFirstkeyValue(int a1, int a2, char* a3)
{
  int arg4 = 1;
  char* p = a3;
  char dataValue[70];
	_asm
	{
		mov esi,a1
		mov eax,a2   //字符串长度
		xor ecx,ecx
		xor edi,edi
		mov dword ptr ss:[ebp-0x64],eax
		mov eax,arg4
		shl eax,3
		mov dword ptr ss:[ebp-0x60],0x4c
		sub dword ptr ss:[ebp-0x60],eax
		push ebx
LABEL_4:
		mov eax,edi
		push 3
		cdq
		pop ebx
		idiv ebx
		sub edx,0
		je LABEL_1
		dec edx
		je LABEL_2
		dec edx
		jnz LABEL_7
		xor eax,eax
		mov ah,byte ptr ds:[esi-1]
		mov al,byte ptr ds:[esi]
		sar eax,6
		and eax,0x3f
		mov al,byte ptr ds:[eax+kbValue]
		mov byte ptr ss:[ecx+dataValue],al
		movzx eax,byte ptr ds:[esi]
		inc ecx
LABEL_5:
		and eax,0x3f
		jmp LABEL_6
LABEL_2:
		xor eax,eax
		mov ah,byte ptr ds:[esi-1]
		mov al,byte ptr ds:[esi]
		sar eax,4
        jmp LABEL_5
LABEL_1:
		movzx eax,byte ptr ds:[esi]
		shr eax,2
LABEL_6:		
		mov al,byte ptr ds:[eax+kbValue]
		mov byte ptr ss:[ecx+dataValue],al
		inc ecx
LABEL_7:
		cmp ecx,0x44///这里和0x44比较
		jl LABEL_3
		mov byte ptr ds:[ecx+dataValue],0x0a
		mov byte ptr ds:[ecx+dataValue+1],0 //字符串结束标志
		lea eax,dword ptr ss:[dataValue]    //生成一段加密字符串，这里很重要，加密字符串的值要从这里取得
		jmp LABEL_11
LABEL_12:
		mov ecx,0x44////////增加这行代码作用：解决跳转到LABEL_11执行C++代码后，寄存器ecx值变化的问题
		mov byte ptr ds:[ecx+dataValue-1],0 //dataValue数组清零
		xor ecx,ecx
LABEL_3:
		inc edi
		inc esi
		cmp edi,a2 //和a2 = 0x0166字符串的长度进行比较
		jl LABEL_4
		pop ebx
		mov eax,edi
		push 3
		cdq
		pop edi
		idiv edi
		dec edx
		je LABEL_8
		dec edx
		jnz LABEL_9
		movzx eax,byte ptr ds:[esi-1]
		and eax,0x0f
		mov al,byte ptr ds:[eax*4+kbValue]
		mov byte ptr ss:[ecx+dataValue],al
		jmp LABEL_9
LABEL_8:
		movzx eax,byte ptr ds:[esi-1]
		and eax,3
		shl eax,4
		mov al,byte ptr ds:[eax+kbValue]
		mov byte ptr ss:[ecx+dataValue],al
		inc ecx
		mov byte ptr ss:[ecx+dataValue],0x3d
LABEL_9:
		inc ecx
		mov byte ptr ss:[ecx+dataValue],0x3d
		inc ecx
		test ecx,ecx
		pop edi
		pop esi
		jle LABEL_10
		mov byte ptr ss:[ecx+dataValue],0x0a
		mov byte ptr ds:[ecx+dataValue+1],0
		lea eax,dword ptr ss:[dataValue]  ///最后的四个字符
		jmp LABEL_13
LABEL_14:
		mov byte ptr ds:[ecx+dataValue-1],0
		xor ecx,ecx
		mov eax,p
		mov a3,eax//把p首地址赋值给a3
LABEL_10:
		mov ecx,dword ptr ss:[ebp-4]
		leave
		retn 
	}

LABEL_11:
	int length = strlen(dataValue);
	memcpy(a3,dataValue,length);
	a3 = a3 + 0x44;
	goto LABEL_12;
LABEL_13:
	int lastLength = strlen(dataValue);
	memcpy(a3,dataValue,lastLength);
	goto LABEL_14;
}

////////////////////////////////////////////////////////////////////////////////////////////
