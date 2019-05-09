// xhwork.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#define INTERNET_WORK_API extern "C" _declspec(dllexport)
#include "xhwork.h"
#include "urlcode.h"
#include "GZipHelper.h"
#include "comment.h"
#include "seitunes.h"
#include "splookupoffer.h"
#include <iostream>
#include <string>
#include <strsafe.h>
#include<locale.h> 
#include <time.h>
#include <atltime.h>
#include <algorithm>
#include "registerAppleId.h"
#include "Util.h"
#include "Sha1.h"
using namespace std;


#pragma warning(disable:4996)
#pragma warning(disable:4995)

#define    MAX_SEND_DATA           1024
#define    MAX_KEY_WORD            128
#define    MAX_ALLOCATE_BUFFER     1024*2000
#define    MAX_DOWNLOAD_ID         64
#define    MAX_DETAILS_SIZE        512
#define    MAX_SIZE				   1024
#define    MAX_ALLOCATE_SIZE       4096*2

#define		FM_QUESTION			   TEXT("你的父母是在哪里认识的？")
#define		TEACHER_QUESTIOIN      TEXT("你上小学时最喜欢的老师姓什么？")
#define		BOOK_QUESTION		   TEXT("你小时候最喜欢哪一本书？")
#define      USER_AGENT   TEXT("AppStore/2.0 iOS/10.1.1 model/iPhone9,1 hwp/s5l9950x build/14B100 (6; dt:97)")//TEXT("AppStore/2.0 iOS/10.0.2 model/iPhone7,1 hwp/s5l8950x build/13F69 (6; dt:82)")
#define      UISTORE_USER_AGENT  TEXT("itunesstored/1.0 iOS/10.1.1 model/iPhone9,1 hwp/s5l9950x build/14B100 (6; dt:97)")//TEXT("itunesstored/1.0 iOS/10.0.2 model/iPhone7,1 hwp/s5l8950x build/13F69 (6; dt:82)")
#define      STORE_PREFERECE_AGENT TEXT("com.apple.Preferences/1 iOS/10.1.1 model/iPhone9,1 hwp/s5l9950x build/14B100 (6; dt:97)")

/*****************多线程共享的全局变量*******************/
int g_DownloadNotiyErrorCount = 0;
CRITICAL_SECTION g_cs;
CString g_mmeClientInfo;
/********************************************************/
///////////////////////////////////////全局变量
typedef BOOL (* PGETMACHINEGUID)(LPVOID lpGsaServices,LPVOID lpBuffer);
typedef BOOL (* PKBSYNCVALUE)(LPVOID lpGsaServices,LPVOID lpBuffer,char* lpDsid,LONGLONG dsid,bool bAuthLogin);
typedef int (* PGETSIGNSAPSETUPBUFFER)(IN LPVOID lpGsaServices,
									   LPVOID lpSignSap,
									   int signSize,
									   LPVOID lpBuffer,
									   int& OutDataLength,
									   BOOL bSet,
									   LPVOID lpUserXmlInfo,
									   int userXmlSize);
typedef LPVOID (* PCREATEGSASERVICESOBJECT)();

typedef VOID (*PRELEASEGSASERVICESOBJECT)(LPVOID lpGsaServices);

typedef void (*PRELEASEGSAALLOCATE)(IN LPVOID lpGsaServices);

typedef int (* PNEWITUNESFUNADDR_1)(IN LPVOID lpGsaServices);
//得到boundary的值
typedef char* (*PGETBOUNDARYVALUE)();

typedef int (* PNEWITUNESFUNADDR_2)(IN LPVOID lpGsaServices,char* lpUdid);
//得到注册时候的数字签名的值的校验值
typedef int (*PGETREGISTERSIGNSAPSETUPBUFFER)(IN LPVOID lpGsaServices,
											  IN LPVOID lpBuffer,//输入参数，第一次从服务器得到的sign-sap-setup-cert字符串传进这个参数
											  IN int signSize,   //字符串长度
										      OUT LPVOID lpXAppleActionSignature,//输出参数，算法生成的输出sign-sap-setup-buffer值
											  OUT int& outSignLen); //输出参数，签名数据的长度

//得到注册是真正的X-apple-signature的值
typedef int (*PGETREGISTERSIGNUPWIZARDXAPPLESIGNATURE)(IN LPVOID lpGsaServices,
													   IN LPVOID lpBuffer,//输入参数,第二次交换从服务器得到的sign-sap-set-buffer字符串传进这个参数
													   IN int inLength,   //输入参数，sign-sap-set-buffer长度
													   OUT LPVOID sXappleSignature,//输出X-Apple-signature的值
													   OUT int& signDataLen);//X-Apple-signature的值的长度


//设置从网页来的数字签名X-Apple-ActionSignature的值
typedef int (*PSETXAPPLEACTIONSIGNATUREFORPAGE)(IN LPVOID lpGsaServices,LPVOID lpXAppleActionSignature,int signLen);



typedef int (*PGENERATEAGREEWEBPAGEXAPPLEACTIONSIGNATURE)(IN LPVOID lpGsaServices,
														  IN LPVOID lpAgreeWebPageUUID,//输入参数,同意页面的uuid
														  IN LPVOID lpUdid,              //输入参数,udid的值
													      IN LPVOID lpAppleId,           //输入参数，注册时输入的邮箱账号
													      OUT LPVOID lpXAppleSignature,  //输出参数,生成的XAppleActionSignature
													      OUT int& signDataLen);         //输出参数,XAppleActionSignature的长度


typedef BOOL (*PGENERATEXJSSPTOKENVALUE)(IN LPVOID lpGsaServices,LPVOID lpSrcData,int dataLen,LPVOID lpOutData);

typedef BOOL (*PGENERATESPXAPPLEACTIONSIGNATURE)(IN LPVOID lpGsaServices,
												 IN LPVOID lpDsid,    //输入参数disd
											   IN LPVOID lpSaleId,  //输入参数appId
											   IN LPVOID lpTimeStamp,//时间戳
											   IN LPVOID lpXAppleStoreFront,//字体
											   IN LPVOID lpSignData, //输入参数 signature的值
											   OUT LPVOID lpOutSignature,//输出参数actionsigature的值
											   OUT int& outSignLength,//输出参数的长度
											   IN BOOL bHighVersion);

typedef BOOL (*PGENERATECHECKINACKSARVALUE)(IN LPVOID lpGsaServices,
											IN LPVOID lpRentalbagResponse,
										  int bagResLen,
										  IN LPVOID lpCheckinsar,
										  int checksarLen,
										  OUT LPVOID lpCheckacksar);

typedef BOOL (*PGENERATERBSYNCVALUE)(IN LPVOID lpGsaServices,OUT LPVOID lpRbsyncValue);

typedef BOOL (*PGENERATECLIENTDATA)(IN LPVOID lpGsaServices,
									IN LPVOID lpXappleMdData,
								    IN int xAppleMdDataLen,
								    IN LPVOID lpDsid,
								    OUT LPVOID lpClientData,
								    OUT int& nvContext);

typedef BOOL (*PGENERATECPIMDATA)(IN LPVOID lpGsaServices,
								  IN LPVOID lpSpimData,
								  IN int spimLen,
								  OUT LPVOID lpCpimData,
								  OUT int& nvContext);

typedef BOOL (*PSETFINISHPROVISIONDATA)(IN LPVOID lpGsaServices,
										IN LPVOID lpTKTransportKeyData,
										IN int tkTransportKeyLen,
										IN LPVOID lpPtmSettingInfoData,
										IN int ptmSettingInfoLen,
										IN int nvContext,
										IN BOOL bGSAProvision);
typedef BOOL (*PGENERATEA2KVALUE)(IN LPVOID lpGsaServices,OUT LPVOID lpA2kValue,OUT int& a2kDataLen);

typedef BOOL (*PGENERATEM1VALUE)(IN LPVOID lpGsaServices,
								 IN LPVOID lpAppleId,
								 IN LPVOID lpPassword,
								 IN int dw_i_data,
								 IN LPVOID lp_s_data,
								 IN int s_data_Len,
								 IN LPVOID lp_B_data,
								 IN int B_data_Len,
								 OUT LPVOID lpM1,
								 OUT int& m1DataLen);

typedef BOOL (*PSETSRPREPONSEDATA)(IN LPVOID lpGsaServices,
								   IN LPVOID lpSpd,
								   IN int spdLen,
								   IN LPVOID lpM2,
								   IN int M2Len,
								   IN LPVOID lpNp,
								   IN int npLen);

typedef BOOL (*PGENERATESBSYNCVALUE)(IN LPVOID lpGsaServices,
									 IN LPVOID lpDsid,
								     IN LPVOID lpXApple_amd_m,
								     IN int amd_m_Len,
								     OUT LPVOID lpSbsyncValue,
								     OUT int& sbsyncLen);

typedef BOOL (*PGENERATEXAPPLEHBTOKEN)(IN LPVOID lpGsaServices,OUT LPVOID lpXAppleHBToken,OUT int& hbTokenLen);
typedef void (*PSETKEYBAGDATA)(IN LPVOID lpGsaServices,IN LPVOID lpKeyBagData,IN int keyBagDataLen);
typedef void (*PSETKEYBAGDATAEX)(IN LPVOID lpGsaServices,IN LONGLONG dsid,IN LPVOID lpKeyBagData,IN int keyBagDataLen);
typedef BOOL (*PGENERATESRPPETPASSWORD)(IN LPVOID lpGsaServices,OUT LPVOID lpPetPassword,OUT int& petPasswordLen);
typedef BOOL (*PGENERATESRPHBTOKEN)(IN LPVOID lpGsaServices,OUT LPVOID lpHBToken,OUT int& hbTokenLen);
typedef BOOL (*PGENERATESRPADSID)(IN LPVOID lpGsaServices,OUT LPVOID lpAdsid,OUT int& adsidLen);
typedef BOOL (*PGENERATEMIDOTPVALUE)(IN LPVOID lpGsaServices,IN LPVOID lpMidValue,IN LPVOID lpOtpValue);
typedef BOOL (*PGENERATEXAPPLEAMDVALUE)(IN LPVOID lpGsaServices,char* lpDsid,char* lpXAppleAMDM,char* lpXAppleAMD);
typedef BOOL (*PGENERATEXAPPLEIMDVALUE)(IN LPVOID lpGsaServices,OUT LPVOID lpXAppleIMDM,OUT LPVOID lpXAppleIMD);
typedef BOOL (*PGENERATEXAPPLEMDVALUE)(IN LPVOID lpGsaServices,IN LPVOID lpDsid,OUT LPVOID lpXAppleMDM,OUT LPVOID lpXAppleMD);

PCREATEGSASERVICESOBJECT pCreateGsaServicesObject = NULL;
PRELEASEGSASERVICESOBJECT pReleaseGsaServicesObject = NULL;
PRELEASEGSAALLOCATE pReleaseGsaAllocate = NULL;
PGENERATEXAPPLEAMDVALUE pGenerateXAppleAMDValue = NULL;
PGENERATEXAPPLEIMDVALUE pGenerateXAppleIMDValue = NULL;
PGENERATEXAPPLEMDVALUE  pGenerateXAppleMDValue = NULL;
PGENERATEXJSSPTOKENVALUE pGenerateXJSSPTOKENValue = NULL;
PGETMACHINEGUID pGetMachineGuid = NULL;
PKBSYNCVALUE pKbsyncValue = NULL;
PGETSIGNSAPSETUPBUFFER pGetSignsapSetupBuf = NULL;
PNEWITUNESFUNADDR_1 pNewItunesFunAdr_1 = NULL;
PNEWITUNESFUNADDR_2 pNewItunesFunAdr_2 = NULL;
PGETREGISTERSIGNSAPSETUPBUFFER pGetRegisterSignsapSetupBuffer = NULL;
PGETREGISTERSIGNUPWIZARDXAPPLESIGNATURE pGetRegisterSignupWizardXappleSignature = NULL;
PGETBOUNDARYVALUE pGetBoundaryValue = NULL;
PGENERATEAGREEWEBPAGEXAPPLEACTIONSIGNATURE pGenerateAgreeWebPageXAppleActionSignature = NULL;
PGENERATESPXAPPLEACTIONSIGNATURE pGenerateSPXAppleActioinSignature = NULL;
PGENERATECHECKINACKSARVALUE pGenerateCheckinAckSarValue = NULL;
PGENERATERBSYNCVALUE pGenerateRbsyncValue = NULL;
PGENERATECLIENTDATA pGenerateClientData = NULL;
PGENERATEMIDOTPVALUE pGenerateMidOtpValue = NULL;
PGENERATECPIMDATA pGenerateCpimData = NULL;
PSETFINISHPROVISIONDATA pSetFinishProvisionData = NULL;
PGENERATEA2KVALUE pGenerateA2kvalue = NULL;
PGENERATEM1VALUE pGenerateM1Value = NULL;
PSETSRPREPONSEDATA pSetSrpReponseData = NULL;
PGENERATESRPPETPASSWORD pGenerateSrpPETPassword = NULL;
PGENERATESRPHBTOKEN pGenerateSrpHBToken = NULL;
PGENERATESRPADSID pGenerateSrpAdsid = NULL;
PGENERATESBSYNCVALUE pGenerateSbsyncValue = NULL;
PGENERATEXAPPLEHBTOKEN pGenerateXAppleHBToken = NULL;
PSETXAPPLEACTIONSIGNATUREFORPAGE pSetXappleActionSignatureForPage = NULL;
PSETKEYBAGDATA pSetKeyBagData = NULL;
PSETKEYBAGDATAEX pSetKeyBagDataEx = NULL;

TCHAR g_Proxy[MAX_PATH] = {0};
int  g_netType = 0;

//int g_FunctionAddress;     //调用itunes里函数的一个很重要的地址。。。接口GetSignsapSetupBuffer需要调用两遍，都需要这个值
//int g_Address_2;           //需要的参数
//int g_Address_3;           //需要的参数
//////////////////////////////////////对外接口///////////////////////////////////////////////
//注册apple id接口
int xhRegisterAppleId(IN LPVOID lpNetwork,TAGREGISTERINFO& tagRegisterInfo)
{
	if (!lpNetwork)
	{
		return FALSE;
	}
	CxhNetwork* pNetworkObj = (CxhNetwork*)lpNetwork;
	return pNetworkObj->RegisterAppleId(tagRegisterInfo);

}
int CxhNetwork::RegisterAppleId(TAGREGISTERINFO& tagRegisterInfo)
{
	char*	pSignupBuf = NULL;
	char*	pSignSapSetupBuffer = NULL;
	int		signLength = 0;
	int		res = 0;
	BOOL	bResult = FALSE;
	CString strWizardHostName;
	CString	strRefererUrl;
	CString	strHostName;
	CString strNextStepName;
	CStringA	strMachineGUIDA;
	CStringA	strPageUUIDA;
	CStringA	strSendDataA;
	CStringA	strAppleIdA;

	USES_CONVERSION;
	pSignSapSetupBuffer = new char[2048];
	if (pSignSapSetupBuffer)
	{
		return ALLOCATEMEM_FAILED;
	}
	memset(pSignSapSetupBuffer,0,2048);

	HMODULE hBrank = GetModuleHandle(_T("xhiTunes.dll"));
	if (!hBrank)
	{
		if (pSignSapSetupBuffer)
		{
			delete []pSignSapSetupBuffer;
			pSignSapSetupBuffer = NULL;
		}
		return LOADMOD_FAILED;
	}

	pGetMachineGuid = (PGETMACHINEGUID)GetProcAddress(hBrank,"GetMachineGUID");
	pNewItunesFunAdr_1 = (PNEWITUNESFUNADDR_1)GetProcAddress(hBrank,"GetiTunesFunAddr_1");
	pNewItunesFunAdr_2 = (PNEWITUNESFUNADDR_2)GetProcAddress(hBrank,"GetiTunesFunAddr_2");
	pGetRegisterSignsapSetupBuffer = (PGETREGISTERSIGNSAPSETUPBUFFER)GetProcAddress(hBrank,"GetRegisterSignsapSetupBuffer");
	pGetBoundaryValue = (PGETBOUNDARYVALUE)GetProcAddress(hBrank,"GetBoundaryValue");
	pSetXappleActionSignatureForPage = (PSETXAPPLEACTIONSIGNATUREFORPAGE)GetProcAddress(hBrank,"SetXappleActionSignatureForPage");
	pGetRegisterSignupWizardXappleSignature = (PGETREGISTERSIGNUPWIZARDXAPPLESIGNATURE)GetProcAddress(hBrank,"GetRegisterSignupWizardXappleSignature");
	pGenerateAgreeWebPageXAppleActionSignature = (PGENERATEAGREEWEBPAGEXAPPLEACTIONSIGNATURE)GetProcAddress(hBrank,"GenerateAgreeWebPageXAppleActionSignature");
	

	//调用了几个外部接口
	res = pNewItunesFunAdr_1(m_pGsaService);
	res = pNewItunesFunAdr_2(m_pGsaService,(char*)m_strUdid.c_str());//获得函数地址和两个重要的参数

	//注册对象
	ManagerRegister* pRegAppleId = new ManagerRegister(tagRegisterInfo);
	
	bResult = pRegAppleId->sendSignSapSetupCert(TEXT("https://init.itunes.apple.com"),
												TEXT("http://itunes.apple.com/cn/"),
												TEXT("GET"),
												UA_ITUNES_12_1_1,	  
												TEXT("/WebObjects/MZInit.woa/wa/signSapSetupCert"));
	if (!bResult)
	{
		if (pSignSapSetupBuffer)
		{
			delete []pSignSapSetupBuffer;
			pSignSapSetupBuffer = NULL;
		}
		return NETCONNECT_FAILED;
	}

	//获取sign_sap_setup_buffer的值
	pSignupBuf = pRegAppleId->getSignSapSetupData();

	//计算新的sign-sap-setup-buffer的值
	res = pGetRegisterSignsapSetupBuffer(m_pGsaService,pSignupBuf,strlen(pSignupBuf),pSignSapSetupBuffer,signLength);

	bResult = pRegAppleId->sendSignSapSetup(TEXT("https://buy.itunes.apple.com"),
											TEXT("http://itunes.apple.com/cn/"),
											TEXT("POST"),
											UA_ITUNES_12_1_1,
											TEXT("/WebObjects/MZPlay.woa/wa/signSapSetup"),
											pSignSapSetupBuffer);
	if (!bResult)
	{
		if (pSignSapSetupBuffer)
		{
			delete []pSignSapSetupBuffer;
			pSignSapSetupBuffer = NULL;
		}
		return NETCONNECT_FAILED;
	}

	//获取从服务器过来的sign-sap-setup-buffer的值
	pSignupBuf = pRegAppleId->getSignSapSetupData();

	//计算X-Apple-ActionSignature的值
	memset(pSignSapSetupBuffer,0,2048);
	res = pGetRegisterSignupWizardXappleSignature(m_pGsaService,pSignupBuf,strlen(pSignupBuf),pSignSapSetupBuffer,signLength);

	strWizardHostName = TEXT("/WebObjects/MZFinance.woa/wa/signupWizard?guid=");
	strWizardHostName += pRegAppleId->getMachineGUID();
	//发送向导包
	bResult = pRegAppleId->sendSignupWizard(TEXT("https://p47-buy.itunes.apple.com"),
											TEXT("GET"),
											UA_8_4,
											strWizardHostName,
											pSignSapSetupBuffer,
											signLength);
	if (!bResult)
	{
		if (pSignSapSetupBuffer)
		{
			delete []pSignSapSetupBuffer;
			pSignSapSetupBuffer = NULL;
		}
		return NETCONNECT_FAILED;
	}
	//设置从服务器下来的X-Apple-ActionSignature的值
	pSignupBuf = pRegAppleId->getXAppleActionsignatureData();
	res = pSetXappleActionSignatureForPage(m_pGsaService,pSignupBuf,strlen(pSignupBuf));

	strRefererUrl = pRegAppleId->getRefererUrl();
	strHostName = pRegAppleId->getAgreeRegWebActionName();
	strNextStepName = pRegAppleId->getNextStepName();
	//发送获取同意页面包
	bResult = pRegAppleId->sendGetAgreeWebPage(TEXT("https://p47-buy.itunes.apple.com"),
											   strRefererUrl,
											   TEXT("POST"),
											   UA_8_4,
											   strHostName,
											   strNextStepName);
	if (!bResult)
	{
		if (pSignSapSetupBuffer)
		{
			delete []pSignSapSetupBuffer;
			pSignSapSetupBuffer = NULL;
		}
		return NETCONNECT_FAILED;
	}

	//计算X-Apple-ActionSignature的值
	strMachineGUIDA = T2A(pRegAppleId->getMachineGUID());
	if (strMachineGUIDA.IsEmpty())
	{
		//重新计算新的UDID
	}
	strPageUUIDA = T2A(pRegAppleId->getAgreeWebPageUUID());
	memset(pSignSapSetupBuffer,0,2048);
	res = pGenerateAgreeWebPageXAppleActionSignature(m_pGsaService,
													(LPVOID)strPageUUIDA.GetBuffer(),
													(LPVOID)strMachineGUIDA.GetBuffer(),
													NULL,
													pSignSapSetupBuffer,
													signLength);

	strHostName = pRegAppleId->getRichInfoWebActionName();
	strRefererUrl = pRegAppleId->getRefererUrl();
	//生成发送的基本数据信息
	strSendDataA = pRegAppleId->generateBasePostData(pSignSapSetupBuffer);
	//发送获取详细页面包
	bResult = pRegAppleId->sendGetRichInfoWebPage(TEXT("https://p47-buy.itunes.apple.com"),
												   strRefererUrl,
												   TEXT("POST"),
												   UA_8_4,
												   strHostName,
												   strSendDataA.GetBuffer(),
												   strSendDataA.GetLength());
	if (!bResult)
	{
		if (pSignSapSetupBuffer)
		{
			delete []pSignSapSetupBuffer;
			pSignSapSetupBuffer = NULL;
		}
		return NETCONNECT_FAILED;
	}

	//计算X-Apple-ActionSignature的值
	strPageUUIDA = T2A(pRegAppleId->getRichInfoWebPageUUID());
	strAppleIdA = T2A(pRegAppleId->getRegisterAppleId());
	memset(pSignSapSetupBuffer,0,2048);
	res = pGenerateAgreeWebPageXAppleActionSignature(m_pGsaService,
													(LPVOID)strPageUUIDA.GetBuffer(),
													(LPVOID)strMachineGUIDA.GetBuffer(),
													(LPVOID)strAppleIdA.GetBuffer(),
													pSignSapSetupBuffer,
													signLength);


	//生成填写详情页面需要发送的数据
	strSendDataA = pRegAppleId->generateRichInfoPostData(pSignSapSetupBuffer);
	strHostName = pRegAppleId->getCreditWebActionName();
	strRefererUrl = pRegAppleId->getRefererUrl();
	//发送获取信用卡也面包
	bResult = pRegAppleId->sendGetCreditInfoWebPage(TEXT("https://p47-buy.itunes.apple.com"),
													strRefererUrl,
													TEXT("POST"),
													UA_8_4,
													strHostName,
													strSendDataA.GetBuffer(),
													strSendDataA.GetLength());
	if (!bResult)
	{
		if (pSignSapSetupBuffer)
		{
			delete []pSignSapSetupBuffer;
			pSignSapSetupBuffer = NULL;
		}
		return NETCONNECT_FAILED;
	}

	//计算X-Apple-ActionSignature的值
	strPageUUIDA = T2A(pRegAppleId->getCreditInfoWebPageUUID());
	memset(pSignSapSetupBuffer,0,2048);
	res = pGenerateAgreeWebPageXAppleActionSignature(m_pGsaService,
													(LPVOID)strPageUUIDA.GetBuffer(),
													(LPVOID)strMachineGUIDA.GetBuffer(),
													NULL,
													pSignSapSetupBuffer,
													signLength);
	strSendDataA = pRegAppleId->generateCreditPostData(pSignSapSetupBuffer);
	strHostName = pRegAppleId->getSubmitCreditInfoWebActionName();
	strRefererUrl = pRegAppleId->getRefererUrl();
	//最后一步提交信用卡详细页面包，如果验证通过，收到验证邮箱账号包
	bResult = pRegAppleId->sendSumbitCreditInfo(TEXT(""),
												strRefererUrl,
												TEXT("POST"),
												UA_8_4,
												strHostName,
												strSendDataA.GetBuffer(),
												strSendDataA.GetLength());
	if (!bResult)
	{
		if (pSignSapSetupBuffer)
		{
			delete []pSignSapSetupBuffer;
			pSignSapSetupBuffer = NULL;
		}
		return NETCONNECT_FAILED;
	}

	if (pSignSapSetupBuffer)
	{
		delete []pSignSapSetupBuffer;
		pSignSapSetupBuffer = NULL;
	}

	if (pRegAppleId)
	{
		delete pRegAppleId;
		pRegAppleId = NULL;
	}
	
	return S_SUCCESS;
}
VOID xhInitCriticalSection()
{
	::InitializeCriticalSection(&g_cs);
}
VOID xhDeleteCriticalSection()
{
	::DeleteCriticalSection(&g_cs);
}
LPVOID xhCreateNetworkObject()
{
	HMODULE		hRankModule = NULL;
	CxhNetwork* pNetwork = new CxhNetwork();
	if (!pNetwork)
	{
		return NULL;
	}
	hRankModule = ::GetModuleHandle(_T("xhiTunes.dll"));
	if (!hRankModule)
	{
		return FALSE;
	}
	
	if (!pGetMachineGuid)
	{
		pGetMachineGuid = (PGETMACHINEGUID)GetProcAddress(hRankModule,"GetMachineGUID");
	}
	if (!pKbsyncValue)
	{
		pKbsyncValue = (PKBSYNCVALUE)GetProcAddress(hRankModule,"GetEncryptKbsyncValue");
	}
	if (!pNewItunesFunAdr_1)
	{
		pNewItunesFunAdr_1 = (PNEWITUNESFUNADDR_1)GetProcAddress(hRankModule,"GetiTunesFunAddr_1");
	}
	if (!pNewItunesFunAdr_2)
	{
		pNewItunesFunAdr_2 = (PNEWITUNESFUNADDR_2)GetProcAddress(hRankModule,"GetiTunesFunAddr_2");
	}
	if (!pGetSignsapSetupBuf)
	{
		pGetSignsapSetupBuf = (PGETSIGNSAPSETUPBUFFER)GetProcAddress(hRankModule,"GetSignsapSetupBuffer");
	}
	if (!pGenerateXJSSPTOKENValue)
	{
		pGenerateXJSSPTOKENValue = (PGENERATEXJSSPTOKENVALUE)GetProcAddress(hRankModule,"generateX_JS_SP_TOKEN_Value");
	}
	if (!pGenerateSPXAppleActioinSignature)
	{
		pGenerateSPXAppleActioinSignature = (PGENERATESPXAPPLEACTIONSIGNATURE)GetProcAddress(hRankModule,"generateSPXAppleActionSignature");
	}
	if (!pGenerateCheckinAckSarValue)
	{
		pGenerateCheckinAckSarValue = (PGENERATECHECKINACKSARVALUE)GetProcAddress(hRankModule,"generateCheckinacksarValue");
	}
	if (!pGenerateRbsyncValue)
	{
		pGenerateRbsyncValue = (PGENERATERBSYNCVALUE)GetProcAddress(hRankModule,"generateRbsyncValue");
	}
	if (!pGenerateClientData)
	{
		pGenerateClientData = (PGENERATECLIENTDATA)GetProcAddress(hRankModule,"generateClientData");
	}
	if (!pGenerateMidOtpValue)
	{
		pGenerateMidOtpValue = (PGENERATEMIDOTPVALUE)GetProcAddress(hRankModule,"generateMidOtpValue");
	}
	if (!pGenerateXAppleAMDValue)
	{
		pGenerateXAppleAMDValue = (PGENERATEXAPPLEAMDVALUE)GetProcAddress(hRankModule,"generateX_Apple_AMD_Value");
	}
	if (!pGenerateXAppleIMDValue)
	{
		pGenerateXAppleIMDValue = (PGENERATEXAPPLEIMDVALUE)GetProcAddress(hRankModule,"generateX_Apple_I_MD_Value");
	}
	if (!pGenerateXAppleMDValue)
	{
		pGenerateXAppleMDValue = (PGENERATEXAPPLEMDVALUE)GetProcAddress(hRankModule,"generateX_Apple_MD_Value");
	}
	if (!pGenerateCpimData)
	{
		pGenerateCpimData = (PGENERATECPIMDATA)GetProcAddress(hRankModule,"generateCpimData");
	}
	if (!pSetFinishProvisionData)
	{
		pSetFinishProvisionData = (PSETFINISHPROVISIONDATA)GetProcAddress(hRankModule,"SetFinishProvisionData");
	}
	if (!pGenerateA2kvalue)
	{
		pGenerateA2kvalue = (PGENERATEA2KVALUE)GetProcAddress(hRankModule,"generateA2kValue");
	}
	if (!pGenerateM1Value)
	{
		pGenerateM1Value = (PGENERATEM1VALUE)GetProcAddress(hRankModule,"generateM1Value");
	}
	if (!pSetSrpReponseData)
	{
		pSetSrpReponseData = (PSETSRPREPONSEDATA)GetProcAddress(hRankModule,"setSrpReponseData");
	}
	if (!pGenerateSrpPETPassword)
	{
		pGenerateSrpPETPassword = (PGENERATESRPPETPASSWORD)GetProcAddress(hRankModule,"generateSrpPETPassword");
	}
	if (!pGenerateSrpHBToken)
	{
		pGenerateSrpHBToken = (PGENERATESRPHBTOKEN)GetProcAddress(hRankModule,"generateSrpHBToken");
	}
	if (!pGenerateSrpAdsid)
	{
		pGenerateSrpAdsid = (PGENERATESRPADSID)GetProcAddress(hRankModule,"generateSrpAdsid");
	}
	if (!pGenerateSbsyncValue)
	{
		pGenerateSbsyncValue = (PGENERATESBSYNCVALUE)GetProcAddress(hRankModule,"generateSbsyncValue");
	}
	if (!pGenerateXAppleHBToken)
	{
		pGenerateXAppleHBToken = (PGENERATEXAPPLEHBTOKEN)GetProcAddress(hRankModule,"generateXAppleHBToken");
	}
	if (!pCreateGsaServicesObject)
	{
		pCreateGsaServicesObject = (PCREATEGSASERVICESOBJECT)GetProcAddress(hRankModule,"CreateGsaServicesObject");
	}
	if (!pReleaseGsaServicesObject)
	{
		pReleaseGsaServicesObject = (PRELEASEGSASERVICESOBJECT)GetProcAddress(hRankModule,"ReleaseGsaServicesObject");
	}
	if (!pReleaseGsaAllocate)
	{
		pReleaseGsaAllocate = (PRELEASEGSAALLOCATE)GetProcAddress(hRankModule,"releaseGsaAllocate");
	}
	if (!pSetKeyBagData)
	{
		pSetKeyBagData = (PSETKEYBAGDATA)GetProcAddress(hRankModule,"setKeyBagData");
	}
	if (!pSetKeyBagDataEx)
	{
		pSetKeyBagDataEx = (PSETKEYBAGDATAEX)GetProcAddress(hRankModule,"setKeyBagDataEx");
	}
	pNetwork->CreateNetGSaServiceObject();
	
	return pNetwork;
}
BOOL CxhNetwork::CreateNetGSaServiceObject()
{
	//创建gsaServices对象
	m_pGsaService = pCreateGsaServicesObject();
	if (!m_pGsaService)
	{
		return FALSE;
	}
	return TRUE;
}

VOID xhReleaseNewtworkObject(LPVOID lpNetwork)
{
	CxhNetwork* pNetwork = (CxhNetwork*)lpNetwork;
	if (pNetwork)
	{
		pNetwork->ReleaseNetGsaServiceObject();
		delete pNetwork;
		pNetwork = NULL;
	}
}

VOID CxhNetwork::ReleaseNetGsaServiceObject()
{
	if (m_pGsaService)
	{
		pReleaseGsaServicesObject(m_pGsaService);
	}
	m_pGsaService = NULL;
}

int xhGSA_Apple_Thread_NetworkVerifier(IN TCHAR* lpUdid,TCHAR* lpProxy,int netType)
{
	BOOL		bResult = FALSE;
	BOOL		res = FALSE;
	char		szSpim[512] = {0};
	char		szCpim[512] = {0};
	char		szTK[32] = {0};
	char		szPtm[512] = {0};
	int			nvContext = 0;

	HMODULE		hRankModule = NULL;
	hRankModule = ::GetModuleHandle(_T("xhiTunes.dll"));
	if (!hRankModule)
	{
		return FALSE;
	}
	
	pGetMachineGuid = (PGETMACHINEGUID)GetProcAddress(hRankModule,"GetMachineGUID");
	pKbsyncValue = (PKBSYNCVALUE)GetProcAddress(hRankModule,"GetEncryptKbsyncValue");

	pNewItunesFunAdr_1 = (PNEWITUNESFUNADDR_1)GetProcAddress(hRankModule,"GetiTunesFunAddr_1");
	pNewItunesFunAdr_2 = (PNEWITUNESFUNADDR_2)GetProcAddress(hRankModule,"GetiTunesFunAddr_2");
	pGetSignsapSetupBuf = (PGETSIGNSAPSETUPBUFFER)GetProcAddress(hRankModule,"GetSignsapSetupBuffer");
	pGenerateXJSSPTOKENValue = (PGENERATEXJSSPTOKENVALUE)GetProcAddress(hRankModule,"generateX_JS_SP_TOKEN_Value");
	pGenerateSPXAppleActioinSignature = (PGENERATESPXAPPLEACTIONSIGNATURE)GetProcAddress(hRankModule,"generateSPXAppleActionSignature");
	pGenerateCheckinAckSarValue = (PGENERATECHECKINACKSARVALUE)GetProcAddress(hRankModule,"generateCheckinacksarValue");
	pGenerateRbsyncValue = (PGENERATERBSYNCVALUE)GetProcAddress(hRankModule,"generateRbsyncValue");
	pGenerateClientData = (PGENERATECLIENTDATA)GetProcAddress(hRankModule,"generateClientData");
	pGenerateMidOtpValue = (PGENERATEMIDOTPVALUE)GetProcAddress(hRankModule,"generateMidOtpValue");
	pGenerateXAppleAMDValue = (PGENERATEXAPPLEAMDVALUE)GetProcAddress(hRankModule,"generateX_Apple_AMD_Value");
    pGenerateXAppleIMDValue = (PGENERATEXAPPLEIMDVALUE)GetProcAddress(hRankModule,"generateX_Apple_I_MD_Value");
	pGenerateXAppleMDValue = (PGENERATEXAPPLEMDVALUE)GetProcAddress(hRankModule,"generateX_Apple_MD_Value");
	pGenerateCpimData = (PGENERATECPIMDATA)GetProcAddress(hRankModule,"generateCpimData");
	pSetFinishProvisionData = (PSETFINISHPROVISIONDATA)GetProcAddress(hRankModule,"SetFinishProvisionData");
	pGenerateA2kvalue = (PGENERATEA2KVALUE)GetProcAddress(hRankModule,"generateA2kValue");
	pGenerateM1Value = (PGENERATEM1VALUE)GetProcAddress(hRankModule,"generateM1Value");
	pSetSrpReponseData = (PSETSRPREPONSEDATA)GetProcAddress(hRankModule,"setSrpReponseData");
	pGenerateSrpPETPassword = (PGENERATESRPPETPASSWORD)GetProcAddress(hRankModule,"generateSrpPETPassword");
	pGenerateSrpHBToken = (PGENERATESRPHBTOKEN)GetProcAddress(hRankModule,"generateSrpHBToken");
	pGenerateSrpAdsid = (PGENERATESRPADSID)GetProcAddress(hRankModule,"generateSrpAdsid");
	pGenerateSbsyncValue = (PGENERATESBSYNCVALUE)GetProcAddress(hRankModule,"generateSbsyncValue");
	pGenerateXAppleHBToken = (PGENERATEXAPPLEHBTOKEN)GetProcAddress(hRankModule,"generateXAppleHBToken");
	pCreateGsaServicesObject = (PCREATEGSASERVICESOBJECT)GetProcAddress(hRankModule,"CreateGsaServicesObject");
	pReleaseGsaServicesObject = (PRELEASEGSASERVICESOBJECT)GetProcAddress(hRankModule,"ReleaseGsaServicesObject");
	pReleaseGsaAllocate = (PRELEASEGSAALLOCATE)GetProcAddress(hRankModule,"releaseGsaAllocate");

	//查询相关服务器功能地址链接
	//开始设备授权
	CString strCookie;
	bResult = CViewPage::startMachineProvisionReport(lpUdid,szSpim,strCookie,g_mmeClientInfo,TEXT(""),lpProxy,netType);
	if (!bResult)
	{
		bResult = CViewPage::startMachineProvisionReport(lpUdid,szSpim,strCookie,g_mmeClientInfo,TEXT(""),lpProxy,netType);
		if (!bResult)
		{
			return bResult;
		}
	}
	//计算cpim的值
	if (szSpim[0] != '\0')
	{
		::EnterCriticalSection(&g_cs);
		res = pGenerateCpimData(NULL,szSpim,strlen(szSpim),szCpim,nvContext);
		::LeaveCriticalSection(&g_cs);
	}
	else
	{
		return FALSE;
	}

	//完成设备授权
	bResult = CViewPage::finishMachineProvisionReport(lpUdid,szCpim,szTK,szPtm,strCookie,g_mmeClientInfo,TEXT(""),lpProxy,netType);
	if (!bResult)
	{
		bResult = CViewPage::finishMachineProvisionReport(lpUdid,szCpim,szTK,szPtm,strCookie,g_mmeClientInfo,TEXT(""),lpProxy,netType);
		if (!bResult)
		{
			return bResult;
		}
	}

	//调用算法设置tk和ptm的值
	if (szTK[0] != '\0' && szPtm[0] != '\0' )
	{
		::EnterCriticalSection(&g_cs);
		res = pSetFinishProvisionData(NULL,szTK,strlen(szTK),szPtm,strlen(szPtm),nvContext,TRUE);
		//设置完成后调用算法生成 x-apple-i-md-m和x-apple-i-md的值
		::LeaveCriticalSection(&g_cs);
	}
	else
	{
		return FALSE;
	}

	return bResult;
}

int xhSetUAInfo(IN LPVOID lpNetwork,PMODELINFO pModInfo)
{
	TCHAR strModel[32] = {0};
	TCHAR strBuild[32] = {0};
	TCHAR strVersion[32] = {0};

	TCHAR strUserAgent[MAX_PATH] = {0};
	TCHAR strMmeClientInfo[MAX_PATH] = {0};
	TCHAR strItunesstoredUserAgent[MAX_PATH] = {0};
	TCHAR strStoreKitUIServiceAgent[MAX_PATH] = {0};
	char szVersion[64] = {0};

	if (!lpNetwork)
	{
		return OBJECT_NULL;
	}
	CxhNetwork* pNetworkObj = (CxhNetwork*)lpNetwork;

	::MultiByteToWideChar(CP_ACP,0,pModInfo->szModel,strlen(pModInfo->szModel),strModel,32);
	::MultiByteToWideChar(CP_ACP,0,pModInfo->szBuild,strlen(pModInfo->szBuild),strBuild,32);
	::MultiByteToWideChar(CP_ACP,0,pModInfo->szVersion,strlen(pModInfo->szVersion),strVersion,32);

	//设置保存ua信息值
	pNetworkObj->SetVersion(pModInfo->szVersion);
	pNetworkObj->SetWidth(pModInfo->width);
	pNetworkObj->SetHeight(pModInfo->height);

	//设置ua的值
	swprintf(strUserAgent,TEXT("AppStore/2.0 iOS/%s model/%s hwp/s5l9950x build/%s (6; dt:97)"),strVersion,strModel,strBuild);
	swprintf(strItunesstoredUserAgent,TEXT("itunesstored/1.0 iOS/%s model/%s hwp/s5l9950x build/%s (6; dt:97)"),strVersion,strModel,strBuild);
	swprintf(strMmeClientInfo,TEXT("<%s> <iPhone OS;%s;%s> <com.apple.akd/1.0 (com.apple.akd/1.0)>"),strModel,strVersion,strBuild);
	swprintf(strStoreKitUIServiceAgent,TEXT("StoreKitUIService/1.0 iOS/%s model/%s hwp/s5l8960x build/%s (6; dt:97)"),strVersion,strModel,strBuild);
	
	pNetworkObj->SetUserAgent(strUserAgent);
	pNetworkObj->SetMmeClientInfo(strMmeClientInfo);
	pNetworkObj->SetItunesstoredUserAgent(strItunesstoredUserAgent);
	pNetworkObj->SetStoreKitUIServiceAgent(strStoreKitUIServiceAgent);
	pNetworkObj->SetModel(strModel);
	return STATUS_SUCCESS;
}

//GSA网络验证设备授权机制
int xhGSA_Apple_NetworkVerifier(IN LPVOID lpNetwork,IN LPVOID lpUdid,PMODELINFO pModInfo,PDEVICEINFO pDeviceInfo,TCHAR* lpProxy,int netType)
{
	if (!lpNetwork)
	{
		return OBJECT_NULL;
	}
	CxhNetwork* pNetworkObj = (CxhNetwork*)lpNetwork;

	return pNetworkObj->GSA_Apple_NetworkVerifier(lpUdid,pModInfo,pDeviceInfo,lpProxy,netType);
}

BOOL CxhNetwork::GSA_Apple_NetworkVerifier(IN LPVOID lpUdid,PMODELINFO pModInfo,PDEVICEINFO pDeviceInfo,TCHAR* lpProxy,int netType)
{
	BOOL		bResult = FALSE;
	BOOL		res = FALSE;
	char		szSpim[512] = {0};
	char		szCpim[512] = {0};
	char		szTK[32] = {0};
	char		szPtm[512] = {0};
	TCHAR		uzUdid[512] = {0};
	TCHAR		strUserAgent[MAX_PATH] = {0};
	TCHAR		strMmeClientInfo[MAX_PATH] = {0};
	TCHAR		strItunesstoredUserAgent[MAX_PATH] = {0};
	TCHAR		strStoreKitUIServiceAgent[MAX_PATH] = {0};
	TCHAR		strApplePreferencesAgent[MAX_PATH] = {0};
	TCHAR		strComAppleAppStoredAgent[MAX_PATH] = {0};
	CString		strModel;
	CString		strBuild;
	CString     strGetHardwarePlatform;
	CString		strVersion;
	char		szVersion[64] = {0};
	char		szX_Apple_I_md[128] = {0};
	char		szX_Apple_I_md_m[MAX_PATH] = {0};
	int			nvContext = 0;

	try
	{
		USES_CONVERSION;	
		//设置版本信息  屏幕长宽
		SetVersion(pModInfo->szVersion);
		SetWidth(pModInfo->width);
		SetHeight(pModInfo->height);
		SetInnerHeight(pModInfo->innerHeight);
		SetInnerWidth(pModInfo->innerWidth);
		//设置ua的值 从服务器过来的硬件信息值
		strModel = A2T(pModInfo->szModel);
		strBuild = A2T(pModInfo->szBuild);
		strGetHardwarePlatform = A2T(pModInfo->szHardwarePlatform);
		strVersion = A2T(pModInfo->szVersion);
		swprintf(strUserAgent,TEXT("AppStore/2.0 iOS/%s model/%s hwp/%s build/%s (6; dt:97)"),strVersion,strModel,strGetHardwarePlatform,strBuild);
		swprintf(strItunesstoredUserAgent,TEXT("itunesstored/1.0 iOS/%s model/%s hwp/%s build/%s (6; dt:97)"),strVersion,strModel,strGetHardwarePlatform,strBuild);
		swprintf(strMmeClientInfo,TEXT("<%s> <iPhone OS;%s;%s> <com.apple.akd/1.0 (com.apple.akd/1.0)>"),strModel,strVersion,strBuild);
		swprintf(strStoreKitUIServiceAgent,TEXT("StoreKitUIService/1.0 iOS/%s model/%s hwp/%s build/%s (6; dt:97)"),strVersion,strModel,strGetHardwarePlatform,strBuild);
		swprintf(strApplePreferencesAgent,TEXT("com.apple.Preferences/1 iOS/%s model/%s hwp/%s build/%s (6; dt:97)"),strVersion,strModel,strGetHardwarePlatform,strBuild);
		swprintf(strComAppleAppStoredAgent,TEXT("com.apple.appstored/1.0 iOS/%s model/%s hwp/%s build/%s (6; dt:97)"),strVersion,strModel,strGetHardwarePlatform,strBuild);

		SetUserAgent(strUserAgent);
		SetMmeClientInfo(strMmeClientInfo);
		SetItunesstoredUserAgent(strItunesstoredUserAgent);
		SetStoreKitUIServiceAgent(strStoreKitUIServiceAgent);
		SetApplePreferencesAgent(strApplePreferencesAgent);
		SetComAppleAppStoredAgent(strComAppleAppStoredAgent);
		SetModel(strModel);
		EnterCriticalSection(&g_cs);
		g_mmeClientInfo = strMmeClientInfo;
		LeaveCriticalSection(&g_cs);

		m_strUdid = T2A(pDeviceInfo->strUdid);
		m_strSerialNo = pDeviceInfo->strSerialNumber;
		m_strMEID = pDeviceInfo->strMEID;
		m_strIMEI = pDeviceInfo->strIMEI;
	
		MultiByteToWideChar(CP_ACP,0,(LPCSTR)m_strUdid.c_str(),m_strUdid.length(),uzUdid,512);

		//begin这里随机生成udid的值
		m_strUdid = GenerateUdid();
		m_ViewPage.m_strUdid = m_strUdid;

		//end

		if (m_strX_Apple_I_md.empty() && m_strX_Apple_I_md_m.empty())
		{
			//查询相关服务器功能地址链接
			//开始设备授权
			CString strCookie = getSendCookie();
			bResult = m_ViewPage.startMachineProvisionReport(uzUdid,szSpim,strCookie,m_mmeClientInfo,m_strSerialNo,lpProxy,netType);
			if (!bResult)
			{
				bResult = m_ViewPage.startMachineProvisionReport(uzUdid,szSpim,strCookie,m_mmeClientInfo,m_strSerialNo,lpProxy,netType);
				if (!bResult)
				{
					m_log.xhLogPrintf(STARTMACHINEPROVISIONREPORT_FAILED,__LINE__,__FUNCTION__,__FILE__);
					return STATUS_FAILED;
				}
			}
			//计算cpim的值
			if (szSpim[0] != '\0')
			{
				::EnterCriticalSection(&g_cs);
				res = pGenerateCpimData(m_pGsaService,szSpim,strlen(szSpim),szCpim,nvContext);
				::LeaveCriticalSection(&g_cs);
			}
			else
			{
				m_log.xhLogPrintf(SPIM_VALUE_NULL,__LINE__,__FUNCTION__,__FILE__);
				return STATUS_FAILED;
			}

			//完成设备授权
			bResult = m_ViewPage.finishMachineProvisionReport(uzUdid,szCpim,szTK,szPtm,strCookie,m_mmeClientInfo,m_strSerialNo,lpProxy,netType);
			if (!bResult)
			{
				bResult = m_ViewPage.finishMachineProvisionReport(uzUdid,szCpim,szTK,szPtm,strCookie,m_mmeClientInfo,m_strSerialNo,lpProxy,netType);
				if (!bResult)
				{
					m_log.xhLogPrintf(FINISHMACHINEPROVISIONREPORT_FAILED,__LINE__,__FUNCTION__,__FILE__);
					return STATUS_FAILED;
				}
			}

			//调用算法设置tk和ptm的值
			if (szTK[0] != '\0' && szPtm[0] != '\0' )
			{
				::EnterCriticalSection(&g_cs);
				res = pSetFinishProvisionData(m_pGsaService,szTK,strlen(szTK),szPtm,strlen(szPtm),nvContext,TRUE);
				//设置完成后调用算法生成 x-apple-i-md-m和x-apple-i-md的值
				::LeaveCriticalSection(&g_cs);
			}
			else
			{
				m_log.xhLogPrintf(TK_PTM_VALUE_NULL,__LINE__,__FUNCTION__,__FILE__);
				return STATUS_FAILED;
			}

			//生成I_md_m 和I_md的值
			GenerateXApple_I_MDandMDMValue();
		}
		//发送xp注册包
		SendxpRegister(NULL,m_itunesstoredUserAgent);
		
	}
	catch(exception& e)
	{
		m_log.xhLogPrintf(EXECUTE_EXCEPTION,__LINE__,__FUNCTION__,__FILE__);
	}

	return STATUS_SUCCESS;
}

string CxhNetwork::GetStringSha1(string str)
{
	SHA1_Context c;
	int ret = SHA1_Init(&c);
	if (-1 == ret)return "";
	SHA1_Update(&c, (const unsigned char*)str.c_str(), str.size());
	unsigned char sha1[20];
	SHA1_Final(&c, sha1);
	SHA1_Clear_data(&c);
	char temp[256] = { 0 };
	for (int i = 0; i < 20; ++i)
	{
		sprintf(&temp[i * 2], "%02x", sha1[i]);
	}

	return temp;
}

void CxhNetwork::GenerateXApple_I_MDandMDMValue()
{
	char		szX_Apple_I_md[128] = {0};
	char		szX_Apple_I_md_m[MAX_PATH] = {0};

	//生成I_md_m 和I_md的值
	EnterCriticalSection(&g_cs);
	
	if ( !pGenerateXAppleIMDValue(m_pGsaService,szX_Apple_I_md_m,szX_Apple_I_md) )
	{
		m_log.xhLogPrintf(GENERATE_AMD_FAILED,__LINE__,__FUNCTION__,__FILE__);
		return;
	}

	if (szX_Apple_I_md_m[0] != '\0' && szX_Apple_I_md[0] != '\0')
	{
		setX_Apple_I_md_m(szX_Apple_I_md_m);
		setX_Apple_I_md(szX_Apple_I_md);
	}
	
	LeaveCriticalSection(&g_cs);

	return;
}

BOOL CxhNetwork::SendxpRegister(char* lpDsid,CString strUserAgent)
{
	string		strClientId = "";
	string		strXpab = "";
	string		strXpabc = "";
	string		strClientId_value = "";
	string		strXpab_value = "";
	string		strXpabc_value = "";
	TCHAR		uzX_Apple_I_md_m[512] = {0};
	TCHAR		uzX_Apple_I_md[MAX_PATH] = {0};
	CString		strSendCookie;
	BOOL        bResult = FALSE;

	if (!m_strX_Apple_I_md_m.empty())
	{
		MultiByteToWideChar(CP_ACP,0,m_strX_Apple_I_md_m.c_str(),m_strX_Apple_I_md_m.length(),uzX_Apple_I_md_m,512);
		MultiByteToWideChar(CP_ACP,0,m_strX_Apple_I_md.c_str(),m_strX_Apple_I_md_m.length(),uzX_Apple_I_md,MAX_PATH);
	}
	if (!m_strxp_ab.empty())
	{
		strSendCookie = getSendCookie();
	}
	//注册xp包report,第一次，需要提取clientId的值
	if (!m_ViewPage.registerXpReport(strUserAgent,
									lpDsid,
									strClientId,
									strXpab,
									strXpabc,
									strClientId_value,
									strXpab_value,
									strXpabc_value,
									strSendCookie,
									TRUE,
									uzX_Apple_I_md_m,
									uzX_Apple_I_md,
									g_Proxy,
									g_netType))
	{
		if (!m_ViewPage.registerXpReport(m_itunesstoredUserAgent,
											lpDsid,
											strClientId,
											strXpab,
											strXpabc,
											strClientId_value,
											strXpab_value,
											strXpabc_value,
											strSendCookie,
											TRUE,uzX_Apple_I_md_m,
											uzX_Apple_I_md,
											g_Proxy,g_netType))
		{
			m_log.xhLogPrintf(REGISTERXPREPORT_FAILED,__LINE__,__FUNCTION__,__FILE__);
			return bResult;
		}
	}
	if (!strClientId.empty())
	{
		setClientId(strClientId,strClientId_value);
	}

	if (!strXpab.empty())
	{
		setxpab_and_xpabc(strXpab,strXpabc,strXpab_value,strXpabc_value);
	}
	if (!m_strxp_ab.empty())
	{
		bResult = TRUE;
	}
	else
	{
		SendxpRegister(lpDsid,strUserAgent);
	}
	
	return bResult;

}
////////////登录App Store
int xhLoginAppStore(IN LPVOID lpNetwork,
					 IN LPVOID lpUserID,
					 IN LPVOID lpPassword,
					 IN LPTSTR lpwFatherAndMother,
				     IN LPTSTR lpwTeacher,
				     IN LPTSTR lpwBook,
					 IN PDEVICEINFO pDeviceInfo,
					 OUT LPVOID lpPasswordToken,
					 OUT LPVOID lpDsid,
					 TCHAR* lpProxy,
					 int netType)
{
	if (!lpNetwork)
	{
		return STATUS_FAILED;
	}
	CxhNetwork* pNetworkObj = (CxhNetwork*)lpNetwork;
	return pNetworkObj->LoginAppStore(lpUserID,lpPassword,lpwFatherAndMother,lpwTeacher,lpwBook,pDeviceInfo,lpPasswordToken,lpDsid,lpProxy,netType);
}
int CxhNetwork::LoginAppStore(IN LPVOID lpUserID,
							   IN LPVOID lpPassword,
							   IN LPTSTR lpwFatherAndMother,
							   IN LPTSTR lpwTeacher,
							   IN LPTSTR lpwBook,
							   IN PDEVICEINFO pDeviceInfo,
							   OUT LPVOID lpPasswordToken,
							   OUT LPVOID lpDsid,
							   TCHAR* lpProxy,
							   int netType)
{
	char kbsyncValue[4096] = {0};
	char szAppleActionSign[4096] = {0};
	char guidBuf[MAX_PATH] = {0};
	char szPasswordToken[MAX_PATH] = {0};
	char szSerialNumber[64] = {0};
	char szDeviceName[64] = {0};
	char szRegisterToken[MAX_PATH] = {0};
	char registerData[1024] = {0};
	char szAdsid[MAX_PATH] = {0};
	BOOL bRet = FALSE;
	int status = STATUS_FAILED;
	int actionSignLen = 0;
	int regDataLen = 0;
	int adsidLength = 0;
	int FunctionAddress = 0,Address_2 = 0,Address_3 = 0;
	
	CString strCookieData;

	USES_CONVERSION;
	string strSerialNumber = T2A(m_strSerialNo);
	m_ViewPage.SetSerialNumber(strSerialNumber);

	m_ViewPage.setXpEventTime();
	//判断付费安全问题
	if (lpwFatherAndMother || lpwTeacher || lpwBook )
	{
		m_secQuesAnswerMap[FM_QUESTION] =  lpwFatherAndMother;
		m_secQuesAnswerMap[TEACHER_QUESTIOIN] = lpwTeacher;
		m_secQuesAnswerMap[BOOK_QUESTION] = lpwBook;

	}
	
	if (lpProxy != NULL && lpProxy[0] != TEXT('\0'))
	{
		EnterCriticalSection(&g_cs);
		wcscpy(g_Proxy,lpProxy);
		g_netType = netType;
		LeaveCriticalSection(&g_cs);
	}
	
	setAppleId((char*)lpUserID);
	setPassword((char*)lpPassword);

	//屏蔽这部分代码，因为试可以不需 要gsaservice正常登陆

	status = preLoginGsaServicesVerifier((char*)lpUserID,(char*)lpPassword,(char*)m_strUdid.c_str(),(char*)m_strX_Apple_I_md.c_str(),(char*)m_strX_Apple_I_md_m.c_str());
	if (status != STATUS_SUCCESS)
	{
		pReleaseGsaAllocate(m_pGsaService);
		m_log.xhLogPrintf(LOGINGSASERVER_FAILED,__LINE__,__FUNCTION__,__FILE__);
		return status;
	}
	//获取adsid
	::EnterCriticalSection(&g_cs);
	bRet = pGenerateSrpAdsid(m_pGsaService,szAdsid,adsidLength);
	if (!bRet)
	{
		pReleaseGsaAllocate(m_pGsaService);
		m_log.xhLogPrintf(GENERATESRPADSID_FAILED,__LINE__,__FUNCTION__,__FILE__);
		return STATUS_FAILED;
	}
	::LeaveCriticalSection(&g_cs);
	setAdsid(szAdsid);

	::EnterCriticalSection(&g_cs);
	///调用了几个外部接口
	int Res_1 = pNewItunesFunAdr_1(m_pGsaService);
	int Res_2 = pNewItunesFunAdr_2(m_pGsaService,(char*)m_strUdid.c_str());//获得函数地址和两个重要的参数

	//生成kbsync的值
	BOOL bRes = pKbsyncValue(m_pGsaService,kbsyncValue,NULL,0,true);
	::LeaveCriticalSection(&g_cs);

	int kbsyncLength = strlen(kbsyncValue);
	
	//权限登录
	status = loginConnection(kbsyncValue);
	string strPdToken = getPasswordToken();
	string strDsid = getDsid();

	//释放gsa内存
	pReleaseGsaAllocate(m_pGsaService);

	string strServerSignBuf = getServerSignBuf();
	memcpy(lpPasswordToken,strPdToken.c_str(),strPdToken.length());
	memcpy(lpDsid,strDsid.c_str(),strDsid.length());

	//查看账号信息
	strCookieData = getSendCookie();
	m_ViewPage.showInfoAccountSummary(m_userAgent, 
										(char*)strDsid.c_str(), 
										(char*)strPdToken.c_str(),
										strCookieData, 
										A2T(m_strX_Apple_I_md_m.c_str()),
										A2T(m_strX_Apple_I_md.c_str()),
										g_Proxy,
										g_netType);

	m_ViewPage.showInfoAccountFieldsSrv(m_userAgent, 
										(char*)strDsid.c_str(), 
										(char*)strPdToken.c_str(),
										strCookieData, 
										A2T(m_strX_Apple_I_md_m.c_str()),
										A2T(m_strX_Apple_I_md.c_str()),
										g_Proxy,
										g_netType);

	return status;
}

int CxhNetwork::preLoginGsaServicesVerifier(char* lpUserId,char* lpUserPassword,char* lpUdid,char* lpXApple_I_md,char* lpXApple_I_md_m)
{
	TCHAR uzX_Apple_I_md_m[512] = {0};
	TCHAR uzX_Apple_I_md[MAX_PATH] = {0};
	char szGsaPostData[2048] = {0};
	char szXAppleHBToken[512] = {0};
	char szFormatTime[MAX_PATH] = {0};
	char szM1[MAX_PATH] = {0};
	char szA2k[512] = {0};
	int a2kLength = 0;
	int dword_i_data = 0;
	int m1Length = 0;
	int hbTokenLen = 0;
	int status = STATUS_FAILED;
	BOOL bRet = FALSE;


	CStringA strAB;
	CStringA strAI;
	CStringA strAS;
	CStringA strAC;

	CStringA strASpd;
	CStringA strAM2;
	CStringA strANp;

	CString strUdid;
	CString strSendCookie;
	CString strXAppleHBToken;

	strSendCookie = getSendCookie();
	
	m_ViewPage.getFormatTime(szFormatTime);

	//生成a2k的值
	::EnterCriticalSection(&g_cs);
	bRet = pGenerateA2kvalue(m_pGsaService,szA2k,a2kLength);
	::LeaveCriticalSection(&g_cs);

	char* lpSrcData = (char*)VirtualAlloc(NULL,1024*4,MEM_COMMIT | MEM_RESERVE,PAGE_EXECUTE_READWRITE);
	if (!lpSrcData)
	{
		return STATUS_FAILED;
	}
	memset(lpSrcData,0,4096);
	//获取a2k发送数据包
	m_ViewPage.getA2kPostData(lpSrcData,szA2k,NULL,szFormatTime,lpXApple_I_md,lpXApple_I_md_m,lpUdid,lpUserId);

	status = m_ViewPage.loginGsaServices2(lpSrcData,strlen(lpSrcData),strSendCookie,TRUE,m_mmeClientInfo,g_Proxy,g_netType);
	if (status != STATUS_SUCCESS)
	{	
		if (lpSrcData)
		{
			VirtualFree(lpSrcData,0,MEM_RELEASE);
			lpSrcData = NULL;
		}
		return status;
		
	}
	//生成M1的值
	strAB = Util::ConvertUnicodeToUTF8(m_ViewPage.m_a2kResponseData.strB);
	strAI = Util::ConvertUnicodeToUTF8(m_ViewPage.m_a2kResponseData.strI);
	strAS = Util::ConvertUnicodeToUTF8(m_ViewPage.m_a2kResponseData.strS);
	strAC = Util::ConvertUnicodeToUTF8(m_ViewPage.m_a2kResponseData.strC);
	dword_i_data = atoi(strAI);
	if (strAB.IsEmpty())
	{
		if (lpSrcData)
		{
			VirtualFree(lpSrcData,0,MEM_RELEASE);
			lpSrcData = NULL;
		}
		return STATUS_FAILED;

	}
	::EnterCriticalSection(&g_cs);
	bRet = pGenerateM1Value(m_pGsaService,
							lpUserId,
							lpUserPassword,
							dword_i_data,
							strAS.GetBuffer(),
							strAS.GetLength(),
							strAB.GetBuffer(),
							strAB.GetLength(),
							szM1,
							m1Length);
	::LeaveCriticalSection(&g_cs);

	m_ViewPage.getFormatTime(szFormatTime);

	//获取M1发送数据包
	memset(lpSrcData,0,4096);
	m_ViewPage.getM1PostData(lpSrcData,
							strAC.GetBuffer(),
							szM1,
							NULL,
							szFormatTime,
							lpXApple_I_md,
							lpXApple_I_md_m,
							lpUdid,
							lpUserId);

	status = m_ViewPage.loginGsaServices2(lpSrcData,strlen(lpSrcData),strSendCookie,FALSE,m_mmeClientInfo,g_Proxy,g_netType);
	if (status != STATUS_SUCCESS)
	{	
		if (lpSrcData)
		{
			VirtualFree(lpSrcData,0,MEM_RELEASE);
			lpSrcData = NULL;
		}
		return status;
		
	}
	//设置返回的数据
	strASpd = Util::ConvertUnicodeToUTF8(m_ViewPage.m_m1ResponseData.strCPD);
	strAM2 = Util::ConvertUnicodeToUTF8(m_ViewPage.m_m1ResponseData.strM2);
	strANp = Util::ConvertUnicodeToUTF8(m_ViewPage.m_m1ResponseData.strNP);

	if (strASpd.IsEmpty())
	{
		if (lpSrcData)
		{
			VirtualFree(lpSrcData,0,MEM_RELEASE);
			lpSrcData = NULL;
		}
		return STATUS_FAILED;
	}

	::EnterCriticalSection(&g_cs);
	bRet = pSetSrpReponseData(m_pGsaService,
								strASpd.GetBuffer(),
								strASpd.GetLength(),
								strAM2.GetBuffer(),
								strAM2.GetLength(),
								strANp.GetBuffer(),
								strANp.GetLength());

	::LeaveCriticalSection(&g_cs);

	USES_CONVERSION;
	//调用接口生成X-Apple-HB-Token的值
	bRet = pGenerateXAppleHBToken(m_pGsaService, szXAppleHBToken, hbTokenLen);
	if (bRet)
	{
		//发送gsas post data数据
		MultiByteToWideChar(CP_ACP,0,m_strX_Apple_I_md_m.c_str(),m_strX_Apple_I_md_m.length(),uzX_Apple_I_md_m,512);
		MultiByteToWideChar(CP_ACP,0,m_strX_Apple_I_md.c_str(),m_strX_Apple_I_md_m.length(),uzX_Apple_I_md,MAX_PATH);

		strUdid = A2T(lpUdid);
		strXAppleHBToken = A2T(szXAppleHBToken);
		string strIMEI = T2A(m_strIMEI);
		string strMEID = T2A(m_strMEID);
		string strSerialNo = T2A(m_strSerialNo);

		//获取gsas post data
		m_ViewPage.getGsasPostData(szGsaPostData, 
									(char*)strIMEI.c_str(), 
									(char*)strMEID.c_str(), 
									(char*)strSerialNo.c_str());

		//发送gsas post data
		bRet = m_ViewPage.SendGsasServicesPostData(GSA_USER_AGENT,
													m_mmeClientInfo,
													szGsaPostData,
													strXAppleHBToken,
													strUdid,
													m_strSerialNo,
													strSendCookie,
													uzX_Apple_I_md_m,
													uzX_Apple_I_md,
													g_Proxy,
													g_netType);


			                                       

	}

	if (lpSrcData)
	{
		VirtualFree(lpSrcData,0,MEM_RELEASE);
		lpSrcData = NULL;
	}
	

	return STATUS_SUCCESS;
}

//设备授权
int xhMachineAuthorize(IN LPVOID lpNetwork,IN LPVOID lpPasswordToken,IN LPVOID lpDsid)
{
	if (!lpNetwork)
	{
		return FALSE;
	}
	CxhNetwork* pNetworkObj = (CxhNetwork*)lpNetwork;
	return pNetworkObj->MachineAuthorize(lpPasswordToken,lpDsid);
}

int CxhNetwork::MachineAuthorize(IN LPVOID lpPasswordToken,IN LPVOID lpDsid)
{
	return TRUE;
}

int xhMachineDeAuthorize(IN LPVOID lpNetwork)
{
	if (!lpNetwork)
	{
		return FALSE;
	}
	CxhNetwork* pNetworkObj = (CxhNetwork*)lpNetwork;
	return pNetworkObj->MachineDeAuthorize();
}

int CxhNetwork::MachineDeAuthorize()
{
	return TRUE;
}

BOOL CxhNetwork::RegisterSuccess(char* lpDsid,char* lpSignature,char* lpPostData,int dataLen)
{
	CString strUrl = _T("https://buy.itunes.apple.com");
	CString strAgent = m_userAgent;
	TCHAR strHostName[MAX_PATH] = _T("/WebObjects/MZFinance.woa/wa/registerSuccess");

	URL_COMPONENTS uc;
	TCHAR scheme[MAX_PATH] = {0};
	TCHAR hostName[MAX_PATH] = {0};
	TCHAR userName[MAX_PATH] = {0};
	TCHAR password[MAX_PATH] = {0};
	TCHAR urlPath[MAX_PATH] = {0};
	TCHAR ExtraInfo[MAX_PATH] = {0};
	INTERNET_PORT nPort = 0;

	HINTERNET hSession = NULL;
	HINTERNET hConnect = NULL;
	HINTERNET hOpenReq = NULL;
	BOOL bAddRequestHeader = FALSE;
	BOOL bSendRequest = FALSE;
	BOOL bQueryInfo = FALSE;
	
	::ZeroMemory(&uc,sizeof(URL_COMPONENTS));
	uc.dwStructSize = sizeof(URL_COMPONENTS);
	uc.lpszScheme = scheme;
	uc.lpszHostName = hostName;
	uc.lpszUserName = userName;
	uc.lpszPassword = password;
	uc.lpszUrlPath = urlPath;
	uc.lpszExtraInfo = ExtraInfo;
	uc.nPort = nPort;

	uc.dwSchemeLength = MAX_PATH;
	uc.dwHostNameLength = MAX_PATH;
	uc.dwUserNameLength = MAX_PATH;
	uc.dwPasswordLength = MAX_PATH;
	uc.dwUrlPathLength = MAX_PATH;

	CString strSendCookie;
	
	USES_CONVERSION;
	CString strDsid = A2T(lpDsid);
	CString strActionSignature = A2T(lpSignature);

	BOOL bCrackUrl = ::InternetCrackUrl(strUrl,0,0,&uc);
	if (bCrackUrl)
	{
		if (g_netType == PROXY_NETWORK)
		{
			hSession = InternetOpen(strAgent,INTERNET_OPEN_TYPE_PROXY,g_Proxy,0,0);
		}
		else
		{
			hSession = InternetOpen(strAgent,0,0,0,0);
		}
		if (!hSession)
		{
			return FALSE;
		}

		hConnect = InternetConnect(hSession,uc.lpszHostName,uc.nPort,uc.lpszUserName,uc.lpszPassword,3,0,0);
		if (!hConnect)
		{
			InternetCloseHandle(hSession);
			return FALSE;
		}

		hOpenReq = HttpOpenRequest(hConnect,_T("POST"),strHostName,0,0,0,0x84801000|INTERNET_FLAG_NO_COOKIES,0);//0x84A01000是一个常数,注意
		if (!hOpenReq)
		{
			InternetCloseHandle(hConnect);
			InternetCloseHandle(hSession);
			return FALSE;
		}

		CString strHeaders = _T("\r\nAccept-Language: zh-cn");
		strHeaders += _T("\r\nUser-Agent: ");
		strHeaders += strAgent;
		strHeaders += _T("\r\nX-Apple-ActionSignature: ");
		strHeaders += strActionSignature;
		strHeaders += _T("\r\nAccept: */*");
		strHeaders += _T("\r\nX-Apple-Store-Front: 143465-19,29");
		strHeaders += _T("\r\nX-Apple-Connection-Type: WiFi");
		strHeaders += _T("\r\nContent-Type: application/x-apple-plist");
		strHeaders += _T("\r\nX-Apple-Tz: 28800");
		strHeaders += _T("\r\nX-Dsid: ");
		strHeaders += strDsid;
		strHeaders += _T("\r\nX-Apple-I-Client-Time: ");
		strHeaders += m_ViewPage.GetClientTime();

		//设置cookie值
		strSendCookie = getSendCookie();
		if (!strSendCookie.IsEmpty())
		{
			strHeaders += TEXT("\r\nCookie: ");
			strHeaders += strSendCookie;
		}

		strHeaders += _T("\r\nAccept-Encoding: gzip, deflate");
		strHeaders += _T("\r\nConnection: keep-alive");
		strHeaders += _T("\r\nProxy-Connection: keep-alive\r\n\r\n");

		DWORD dwHeaderLength = wcslen(strHeaders);

		bAddRequestHeader = HttpAddRequestHeaders(hOpenReq,strHeaders,dwHeaderLength,0xA0000000);//0xA0000000是一个常量
		if (!bAddRequestHeader)
		{
			InternetCloseHandle(hOpenReq);
			InternetCloseHandle(hConnect);
			InternetCloseHandle(hSession);
			return FALSE;
		}

		bSendRequest = HttpSendRequestW(hOpenReq,NULL,-1,lpPostData,dataLen);//第三个参数设置为负一
		if (!bSendRequest && GetLastError() == ERROR_INTERNET_INVALID_CA)
		{
			DWORD dwFlags;
			DWORD dwBuffLen = sizeof(dwFlags);
			InternetQueryOption (hOpenReq, INTERNET_OPTION_SECURITY_FLAGS,(LPVOID)&dwFlags, &dwBuffLen);
			dwFlags |= SECURITY_FLAG_IGNORE_UNKNOWN_CA;
			InternetSetOption (hOpenReq,INTERNET_OPTION_SECURITY_FLAGS,&dwFlags,sizeof (dwFlags) );

			//再次发送
			bSendRequest = HttpSendRequest(hOpenReq,NULL,-1,lpPostData,dataLen);
			if (!bSendRequest)
			{
				int error = GetLastError();//ERROR_INTERNET_SEC_CERT_REV_FAILED 
				InternetCloseHandle(hOpenReq);
				InternetCloseHandle(hConnect);
				InternetCloseHandle(hSession);
				return FALSE;
			}
		}
		else if (!bSendRequest)
		{
			InternetCloseHandle(hOpenReq);
			InternetCloseHandle(hConnect);
			InternetCloseHandle(hSession);
			return FALSE;
		}


		DWORD dwQueryBuf = 200;
		DWORD dwQueryBufferLength = 4;

		BOOL bQueryInfo = HttpQueryInfo(hOpenReq,0x20000013,&dwQueryBuf,&dwQueryBufferLength,NULL);//HttpQueryInfo 获取文件大小后
		if (!bQueryInfo)
		{
			InternetCloseHandle(hOpenReq);
			InternetCloseHandle(hConnect);
			InternetCloseHandle(hSession);
			return FALSE;
		}

		if (dwQueryBuf != 200)
		{
			InternetCloseHandle(hOpenReq);
			InternetCloseHandle(hConnect);
			InternetCloseHandle(hSession);
			return FALSE;
		}
	}
	else
	{
		return FALSE;
	}
	
	if (hOpenReq)
	{
		InternetCloseHandle(hOpenReq);
	}

	if (hConnect)
	{
		InternetCloseHandle(hConnect);
	}

	if (hSession)
	{
		InternetCloseHandle(hSession);
	}
	return TRUE;
}

void CxhNetwork::generateSerialNumber(char* lpSerialNumber)
{
}

BOOL CxhNetwork::reLoginAppStoreForDownload(char* lpAppleId,char* lpPassword,char* lpDsid,TCHAR* lpProxy,int netType)
{
	char szSignBuf[2048] = {0};
	char recvSignBuf[2048] = {0};
	char szXAppleSignBuf[2048] = {0};
	char SendBuffer[4096] = {0};
	char szKbsyncValue[1024] = {0};
	char szXAppleAMdData[1024] = {0};
	char szX_Apple_I_md[128] = {0};
	char szX_Apple_I_md_m[MAX_PATH] = {0};
	int outDataLen = 0;
	int signSapLength = 0;
	int sendLength = 0;
	BOOL res = 0;
	LONGLONG Dsid = 0;

	TCHAR X_Apple_I_md_m[512] = {0};
	TCHAR X_Apple_I_md[MAX_PATH] = {0};
	CString strCookieData;

	//生成I_md_m 和I_md的值
	if ( !pGenerateXAppleIMDValue(m_pGsaService,szX_Apple_I_md_m,szX_Apple_I_md) )
	{
		return FALSE;
	}
	if (szX_Apple_I_md_m[0] != '\0')
	{
		setX_Apple_I_md_m(szX_Apple_I_md_m);
		setX_Apple_I_md(szX_Apple_I_md);
	}
	else
	{
		strcpy(szX_Apple_I_md_m,m_strX_Apple_I_md_m.c_str());
		strcpy(szX_Apple_I_md,m_strX_Apple_I_md.c_str());
	}
	
	MultiByteToWideChar(CP_ACP,0,szX_Apple_I_md_m,strlen(szX_Apple_I_md_m),X_Apple_I_md_m,512);
	MultiByteToWideChar(CP_ACP,0,szX_Apple_I_md,strlen(szX_Apple_I_md),X_Apple_I_md,MAX_PATH);

	//生成kbsync的值
	if (lpDsid == NULL)
	{
		res = pKbsyncValue(m_pGsaService,szKbsyncValue,NULL,0,true);
		//发送sign-sap-setup-buffer的值到服务器
		res = ResponseServer(_T("https://buy.itunes.apple.com"),
							 _T("POST"),
							 _T("/WebObjects/MZPlay.woa/wa/signSapSetup"),
							 X_Apple_I_md_m,
							 X_Apple_I_md,
							 (char*)m_strXAppleSignBuf.c_str(),
							 recvSignBuf,
							 signSapLength);
		if (!res)
		{
			m_log.xhLogPrintf(RESPONSESERVER_FAILED,__LINE__,__FUNCTION__,__FILE__);
			return FALSE;
		}
	}
	else
	{
		Dsid = _atoi64((char*)lpDsid);
		res = pKbsyncValue(m_pGsaService,szKbsyncValue,(char*)lpDsid,Dsid,true);
		//发送sign-sap-setup-buffer的值
		strCookieData = getSendCookie();
		res = getSignSapSetup(m_itunesstoredUserAgent,(char*)m_strXAppleSignBuf.c_str(),recvSignBuf,signSapLength,(char*)lpDsid,strCookieData,X_Apple_I_md_m,X_Apple_I_md,lpProxy,netType);
		if (!res)
		{
			res = getSignSapSetup(m_itunesstoredUserAgent,(char*)m_strXAppleSignBuf.c_str(),recvSignBuf,signSapLength,(char*)lpDsid,strCookieData,X_Apple_I_md_m,X_Apple_I_md,lpProxy,netType);
			if (!res)
			{
				m_log.xhLogPrintf(GETSIGNSAPSETUP_FAILED,__LINE__,__FUNCTION__,__FILE__);
				return FALSE;
			}
		}
	}

	//组装用户xml数据
	m_ViewPage.loginXmlData(SendBuffer,lpAppleId,lpPassword,(char*)m_strUdid.c_str(),szKbsyncValue,TRUE);

	sendLength = strlen(SendBuffer);

	//调用接口，生成X-Apple-Signature的值
	res = pGetSignsapSetupBuf(m_pGsaService,
								recvSignBuf,
								signSapLength,
								szSignBuf,
								outDataLen,
								TRUE,
								SendBuffer,
								sendLength);

	//发送数据，然后从服务器获取数据
	res = ResponseServerWithSignature(m_strAuthUrl,m_userAgent,TEXT("POST"),m_strAuthHostName,(char*)lpDsid,SendBuffer,sendLength,szSignBuf,szXAppleAMdData);
	if (!res)
	{
		res = ResponseServerWithSignature(m_strAuthUrl,m_userAgent,TEXT("POST"),m_strAuthHostName,(char*)lpDsid,SendBuffer,sendLength,szSignBuf,szXAppleAMdData);
		if (!res)
		{
			m_log.xhLogPrintf(RESPONSESERVERWITHSIGNATURE_FAILED,__LINE__,__FUNCTION__,__FILE__);
			return FALSE;
		}
	}

	return TRUE;
}

int CxhNetwork::NeedVerifyData(char* lpSrcData)
{
	CString strSrcData;
	CString strUrl = TEXT("https://xp.apple.com");
	CString strItmsAppss = TEXT("s://buy.itunes.apple.com");
	CString strFinanceApp = TEXT("s://finance-app.itunes.apple.com");
	CString strHostName;
	CString strMsgCode_3038 = TEXT("<key>messageCode</key><string>3038</string>");
	CString strMsgCode_3079 = TEXT("<key>messageCode</key><string>3079</string>");
	CString strMsgCode_5002 = TEXT("<key>failureType</key><string>5002</string>");
	CString strMsgCode_1008 = TEXT("<key>failureType</key><string>1008</string>");
	CString strMsgCode_2072 = TEXT("<key>failureType</key><string>2072</string>");
	CString strMsgCode_2071 = TEXT("<key>messageCode</key><string>2071</string>");
	CString strMsgCode_2070 = TEXT("<key>messageCode</key><string>2070</string>");
	CString strMsgCode_2024 = TEXT("<key>messageCode</key><string>2024</string>");
	CString strMsgCode_2004 = TEXT("<key>messageCode</key><string>2004</string>");
	CString strMsgCode_2019 = TEXT("<key>messageCode</key><string>2019</string>");
	CString	strMsgAlwaysNeedPwd = TEXT("<key>dialogId</key><string>MZCommerce.ASN.AlwaysSometimes</string>");
	CString strMsgCaptchaRequired =  TEXT("<key>dialogId</key><string>MZCommerce.CaptchaRequired</string>");
	CString strMsgAlwaysSavePwd = TEXT("<key>dialogId</key><string>MZCommerce.ASN.AlwaysNever</string>");

	int bResult = 0;

	if(!lpSrcData)
	{
		return -1;
	}

	int dataLen = strlen(lpSrcData);
	TCHAR* lpwcData = new TCHAR[dataLen*2+2];
	if (!lpwcData)
	{
		return -1;
	}
	memset(lpwcData,0,dataLen*2+2);
	MultiByteToWideChar(CP_ACP,0,lpSrcData,dataLen,lpwcData,dataLen*2);

	strSrcData = lpwcData;

	//搜索关键字,判断是否条件更改
	int keyStrPos = strSrcData.Find(strMsgCode_3038);
	if (keyStrPos >= 0)
	{
		bResult = CHANGE_CONDITION;
	}

	keyStrPos = strSrcData.Find(strMsgCode_3079);
	if (keyStrPos >= 0)
	{
		bResult = SUBMIT_PAID_BUY_VERIFY;
	}

	keyStrPos = strSrcData.Find(strMsgCode_2072);
	if (keyStrPos >= 0)
	{
		bResult = BUY_LOGIN_VERIFY;
	}
	keyStrPos = strSrcData.Find(strMsgCode_2071);
	if (keyStrPos >= 0)
	{
		bResult = BUY_LOGIN_VERIFY;
	}
	keyStrPos = strSrcData.Find(strMsgCode_2070);
	if (keyStrPos >= 0)
	{
		bResult = BUY_LOGIN_VERIFY;
	}
	keyStrPos = strSrcData.Find(strMsgCaptchaRequired);
	if (keyStrPos >= 0)
	{
		bResult = CAPTCHA_REQUIRED;
	}
	//判断账号是否被停用
	keyStrPos = strSrcData.Find(strMsgCode_2004);
	if (keyStrPos >= 0)
	{
		bResult = ACCOUNT_STOP;
		if (lpwcData)
		{
			delete []lpwcData;
			lpwcData = NULL;
		}
		return bResult;
	}
	//购买价格不匹配
	keyStrPos = strSrcData.Find(strMsgCode_2019);
	if (keyStrPos >= 0)
	{
		bResult = PRICE_MISMATCH;
		if (lpwcData)
		{
			delete []lpwcData;
			lpwcData = NULL;
		}
		return bResult;
	}
	//判断是否始终需要密码
	keyStrPos = strSrcData.Find(strMsgAlwaysNeedPwd);
	if (keyStrPos >= 0)
	{
		bResult = NEED_ALOWAYS_PWD;
		m_ViewPage.m_asn = 1;//在此设备始终需要密码,2：不需要密码
	}
	keyStrPos = strSrcData.Find(strMsgAlwaysSavePwd);
	if (keyStrPos >= 0)
	{
		bResult = SAVE_PWD_FOR_FREE_APP;
		m_ViewPage.m_asn = 13; //13:为免费项目保存密码,5:为免费项目不保存密码
	}

	//判断是否有未知错误
	keyStrPos = strSrcData.Find(strMsgCode_5002);
	if (keyStrPos >= 0)
	{
		bResult = UNKNOWN_ERROR;
		if (lpwcData)
		{
			delete []lpwcData;
			lpwcData = NULL;
		}
		
		return bResult;
	}
	//判断是否无法判断设备
	keyStrPos = strSrcData.Find(strMsgCode_1008);
	if (keyStrPos >= 0)
	{
		bResult = UNKNOWN_DEVICE;
		if (lpwcData)
		{
			delete []lpwcData;
			lpwcData = NULL;
		}
		
		return bResult;
	}

	//判断购买是否继续
	keyStrPos = strSrcData.Find(strMsgCode_2024);
	if (keyStrPos >= 0)
	{
		bResult = AGREE_BUY_VERIFY;
	}
	//判断bResult是否为0
	if (bResult == 0)
	{
		if (lpwcData)
		{
			delete []lpwcData;
			lpwcData = NULL;
		}
		return bResult;
	}

	//开始检索提取xpmain时的数据
	int beginPos = strSrcData.Find(strUrl);
	if (beginPos < 0)
	{
		return -1;
	}
	CString strTmp = strSrcData.Right(strSrcData.GetLength()-beginPos-strUrl.GetLength());
	int endPos = strTmp.Find(TEXT("</string>"));
	if (endPos < 0)
	{
		if (lpwcData)
		{
			delete []lpwcData;
			lpwcData = NULL;
		}
		return -1;
	}
	strHostName = strTmp.Left(endPos);
	m_strXPVerifyHostName = strHostName;

	//判断是否需要登录验证
	if (bResult == BUY_LOGIN_VERIFY)
	{
		if (lpwcData)
		{
			delete []lpwcData;
			lpwcData = NULL;
		}
		return bResult;
	}
	//判断购买是否一致需要密码
	if (bResult == NEED_ALOWAYS_PWD)
	{
		if (lpwcData)
		{
			delete []lpwcData;
			lpwcData = NULL;
		}
		return bResult;
	}

	//开始检索提取itms-appss时的数据
	beginPos = strSrcData.Find(strItmsAppss);
	if (beginPos >= 0)
	{
		strTmp = strSrcData.Right(strSrcData.GetLength()-beginPos-strItmsAppss.GetLength());
		endPos = strTmp.Find(TEXT("</string>"));
		if (endPos >= 0)
		{
			strHostName = strTmp.Left(endPos);
		}
		
	}
	//开始检索提取https://finance-app.itunes.apple.com链接数据
	beginPos = strSrcData.Find(strFinanceApp);
	if (beginPos >= 0)
	{
		strTmp = strSrcData.Right(strSrcData.GetLength()-beginPos-strFinanceApp.GetLength());
		endPos = strTmp.Find(TEXT("</string>"));
		if (endPos >= 0)
		{
			strHostName = strTmp.Left(endPos);
		}
	}
	
	m_strItmsAppsHostName = strHostName;

	if (lpwcData)
	{
		delete []lpwcData;
		lpwcData = NULL;
	}

	return bResult;
}

void CxhNetwork::ExtractKeybagData(char* lpSrcData)
{
	CStringA strKeyBagInfo = "<key>keybag</key><data>";
	CStringA strEndTag = "</data>";
	CStringA strSrcData = lpSrcData;
	CStringA strTmp;
	int endPos = 0;
	int keyStrPos = strSrcData.Find(strKeyBagInfo);
	if (keyStrPos >= 0)
	{
		strTmp = strSrcData.Right(strSrcData.GetLength()-keyStrPos-strKeyBagInfo.GetLength());
		endPos = strTmp.Find(strEndTag);
		m_strKeybag = strTmp.Left(endPos);
	}
	return;
}
int CxhNetwork::ExtractErrorInfo(char* lpSrcData)
{
	CString strSrcData;
	CString strSPAuthError = TEXT("AMD-Action:authenticate:SP");
	CString strSPMDAuthError = TEXT("<string>MD-Action:buyProduct:SP");
	CString strUnknownError = TEXT("<key>failureType</key><string>5002</string>");
	CString strAMDBuyProductError = TEXT("<string>AMD-Action:buyProduct:SP");
	CString strUnknownError5000 = TEXT("<key>failureType</key><string>-5000</string>");
	CString strKeyBagInfo = TEXT("<key>keybag</key><data>");
	int bResult = 0;

	if(!lpSrcData)
	{
		return -1;
	}

	int dataLen = strlen(lpSrcData);
	TCHAR* lpwcData = new TCHAR[dataLen*2+2];
	if (!lpwcData)
	{
		return -1;
	}
	memset(lpwcData,0,dataLen*2+2);
	MultiByteToWideChar(CP_ACP,0,lpSrcData,dataLen,lpwcData,dataLen*2);

	strSrcData = lpwcData;

	int keyStrPos = strSrcData.Find(strSPAuthError);
	if (keyStrPos >= 0)
	{
		bResult = SP_AUTH_ERROR;
	}
	keyStrPos = strSrcData.Find(strSPMDAuthError);
	if (keyStrPos >= 0)
	{
		bResult = SP_MD_AUTH_ERROR;
	}
	keyStrPos = strSrcData.Find(strAMDBuyProductError);
	if (keyStrPos >= 0)
	{
		bResult = AMD_BUYPRODUCT_ERROR;
	}
	keyStrPos = strSrcData.Find(strUnknownError);
	if (keyStrPos >= 0)
	{
		bResult = AUNKNOWN_ERROR;
	}
	keyStrPos = strSrcData.Find(strUnknownError5000);
	if (keyStrPos >= 0)
	{
		bResult = AUNKNOWN_ERROR;
	}
	keyStrPos = strSrcData.Find(strKeyBagInfo);
	if (keyStrPos >= 0)
	{
		bResult = NEED_SET_KEYBAG;
	}
	if (lpwcData)
	{
		delete []lpwcData;
		lpwcData = NULL;
	}

	return bResult;
}
BOOL CxhNetwork::ReadDataFromServer(HINTERNET hOpenReq,LPVOID lpBuffer,LPDWORD lpdwSize)
{
	LPGZIP lpgzipBuf = NULL;
	DWORD dataLength = 0;
	DWORD dwReadLength = 0;
	DWORD dwMaxDataLength = 200;
	BOOL bReadFile = FALSE;
	char DataBuffer[200] = {0};
	char* pDecodeData = NULL;
	int decodeLen = 0;


	//分配虚拟内存保存页面数据
	lpgzipBuf =  (LPGZIP)VirtualAlloc(NULL,1024*4,MEM_COMMIT | MEM_RESERVE,PAGE_EXECUTE_READWRITE);//分配4K的虚拟内存
	if (lpgzipBuf != NULL)
	{
		while (true)//循环读取页面数据
		{
			bReadFile = InternetReadFile(hOpenReq,DataBuffer,dwMaxDataLength,&dwReadLength);
			if (bReadFile && dwReadLength != 0)
			{
				memcpy(lpgzipBuf + dataLength,DataBuffer,dwReadLength);
				dataLength += dwReadLength;														
				dwReadLength = 0;	

			}
			else
			{
				lpgzipBuf[dataLength] = '\0';
				break;
			}

		}//while end

		CGZIP2A gzip2A(lpgzipBuf,dataLength);
		pDecodeData = gzip2A.psz;//解密出来的网页Json数据指针
		if (!pDecodeData)
		{
			if (lpgzipBuf)
			{
				VirtualFree(lpgzipBuf,0,MEM_RELEASE);
				lpgzipBuf = NULL;
			}

		}
		decodeLen = strlen(pDecodeData);
		memcpy(lpBuffer,pDecodeData,decodeLen);
		*lpdwSize = decodeLen;
		
	}

	if (lpgzipBuf)
	{
		VirtualFree(lpgzipBuf,0,MEM_RELEASE);
		lpgzipBuf = NULL;
	}

	return TRUE;
}

BOOL CxhNetwork::InitSignSapSetupCert(CString strUrl,
						  CString strVerb,
						  CString strHostName,
						  TCHAR* lpX_Apple_I_md_m,
						  TCHAR* lpX_Apple_I_md)
{
	char		InBuf[4096] = {0};
	char		szRevBuffer[4097] = {0};
	char		firstSignKey[] = "<data>";
	char		lastSignKey[] = "</data>";
	TCHAR		strHeaders[1024] = _T("User-Agent: ");
	BOOL		bResult = FALSE;

	URL_COMPONENTS uc;
	TCHAR scheme[MAX_PATH] = {0};
	TCHAR hostName[MAX_PATH] = {0};
	TCHAR userName[MAX_PATH] = {0};
	TCHAR password[MAX_PATH] = {0};
	TCHAR urlPath[MAX_PATH] = {0};
	TCHAR ExtraInfo[MAX_PATH] = {0};
	INTERNET_PORT nPort = 0;
	
	CString strAgent;
	HINTERNET hSession = NULL;
	HINTERNET hConnect = NULL;
	HINTERNET hOpenReq = NULL;
	BOOL bAddRequestHeader = FALSE;
	BOOL bSendRequest = FALSE;
	BOOL bQueryInfo = FALSE;
	
	::ZeroMemory(&uc,sizeof(URL_COMPONENTS));
	uc.dwStructSize = sizeof(URL_COMPONENTS);
	uc.lpszScheme = scheme;
	uc.lpszHostName = hostName;
	uc.lpszUserName = userName;
	uc.lpszPassword = password;
	uc.lpszUrlPath = urlPath;
	uc.lpszExtraInfo = ExtraInfo;
	uc.nPort = nPort;

	uc.dwSchemeLength = MAX_PATH;
	uc.dwHostNameLength = MAX_PATH;
	uc.dwUserNameLength = MAX_PATH;
	uc.dwPasswordLength = MAX_PATH;
	uc.dwUrlPathLength = MAX_PATH;

	DWORD dwQueryBuf = 200;
	DWORD dwQueryBufferLength = 4;
	DWORD dwSize = 0;
	
	strAgent = m_itunesstoredUserAgent;

	BOOL bCrackUrl = InternetCrackUrl(strUrl,0,0,&uc);
	if (bCrackUrl)
	{
		if (g_netType == PROXY_NETWORK)
		{
			hSession = InternetOpen(strAgent,INTERNET_OPEN_TYPE_PROXY,g_Proxy,0,0);
		}
		else
		{
			hSession = InternetOpen(strAgent,0,0,0,0);
		}
		if (!hSession)
		{
			return FALSE;
		}

		hConnect = InternetConnect(hSession,uc.lpszHostName,uc.nPort,uc.lpszUserName,uc.lpszPassword,3,0,0);
		if (!hConnect)
		{
			InternetCloseHandle(hSession);
			return FALSE;
		}
		hOpenReq = HttpOpenRequest(hConnect,strVerb,strHostName,0,0,0,0x84801000,0);//0x84A01000是一个常数,注意
		if (!hOpenReq)
		{
			InternetCloseHandle(hConnect);
			InternetCloseHandle(hSession);
			return FALSE;
		}

		_tcscat(strHeaders,m_itunesstoredUserAgent);
		_tcscat(strHeaders,_T("\r\nAccept-Language: zh-Hans"));
		_tcscat(strHeaders,_T("\r\nAccept: */*"));
		_tcscat(strHeaders,_T("\r\nX-Apple-Connection-Type: WiFi"));
		_tcscat(strHeaders,_T("\r\nX-Apple-Store-Front: 143465-2,29"));
		_tcscat(strHeaders,_T("\r\nContent-Type: application/x-www-form-urlencoded"));
		_tcscat(strHeaders, _T("\r\nX-Apple-Client-Versions: GameCenter/2.0"));

		if (lpX_Apple_I_md_m[0] != TEXT('\0'))
		{
			_tcscat(strHeaders,_T("\r\nX-Apple-I-MD-RINFO: 17106176"));
			_tcscat(strHeaders,_T("\r\nX-Apple-I-MD-M: "));
			_tcscat(strHeaders,lpX_Apple_I_md_m);
			_tcscat(strHeaders,_T("\r\nX-Apple-I-MD: "));
			_tcscat(strHeaders,lpX_Apple_I_md);
		}

		_tcscat(strHeaders, _T("\r\nX-Apple-I-Client-Time: "));
		_tcscat(strHeaders,m_ViewPage.GetClientTime().GetBuffer());
		_tcscat(strHeaders,_T("\r\nX-Apple-Tz: 28800"));

		_tcscat(strHeaders,_T("\r\nAccept-Encoding: gzip, deflate"));
		_tcscat(strHeaders,_T("\r\nConnection: keep-alive"));
		_tcscat(strHeaders, _T("\r\nProxy-Connection: keep-alive\r\n\r\n"));	
		
		DWORD dwHeaderLength = wcslen(strHeaders);	//194

		bAddRequestHeader = HttpAddRequestHeaders(hOpenReq,strHeaders,dwHeaderLength,0xA0000000);//0xA0000000是一个常量
		if (!bAddRequestHeader)
		{
			InternetCloseHandle(hOpenReq);
			InternetCloseHandle(hConnect);
			InternetCloseHandle(hSession);
			return FALSE;
		}

		bSendRequest = HttpSendRequestW(hOpenReq,NULL,-1,NULL,0);//第三个参数设置为负一	
		if (!bSendRequest && GetLastError() == ERROR_INTERNET_INVALID_CA)
		{
			DWORD dwFlags;
			DWORD dwBuffLen = sizeof(dwFlags);
			InternetQueryOption (hOpenReq, INTERNET_OPTION_SECURITY_FLAGS,(LPVOID)&dwFlags, &dwBuffLen);
			dwFlags |= SECURITY_FLAG_IGNORE_UNKNOWN_CA;
			InternetSetOption (hOpenReq,INTERNET_OPTION_SECURITY_FLAGS,&dwFlags,sizeof (dwFlags) );

			//再次发送
			bSendRequest = HttpSendRequest(hOpenReq,NULL,-1,NULL,0);
			if (!bSendRequest)
			{
				int error = GetLastError();//ERROR_INTERNET_SEC_CERT_REV_FAILED 
				InternetCloseHandle(hOpenReq);
				InternetCloseHandle(hConnect);
				InternetCloseHandle(hSession);
				return FALSE;
			}
		}
		else if (!bSendRequest)
		{
			int error = GetLastError();//ERROR_INTERNET_SEC_CERT_REV_FAILED 
			InternetCloseHandle(hOpenReq);
			InternetCloseHandle(hConnect);
			InternetCloseHandle(hSession);
			return FALSE;
		}

		bQueryInfo = HttpQueryInfo(hOpenReq,0x20000013,&dwQueryBuf,&dwQueryBufferLength,NULL);//HttpQueryInfo 获取文件大小后
		if (bQueryInfo)
		{
			if (dwQueryBuf == 200)
			{
				///Read file from Internet.....
				ReadDataFromServer(hOpenReq,szRevBuffer,&dwSize);

				char* ptr_1 = strstr(szRevBuffer,firstSignKey);
				int signKeyLength = strlen(firstSignKey);
				ptr_1 = ptr_1 + signKeyLength;
				
				char* ptr_2 = strstr(szRevBuffer,lastSignKey);

				int signSapLength = ptr_2 - ptr_1;

				memcpy(InBuf,ptr_1,signSapLength);
				m_strServerSignBuf = InBuf;
			}

		}
		else
		{
			InternetCloseHandle(hOpenReq);
			InternetCloseHandle(hConnect);
			InternetCloseHandle(hSession);
			bResult = FALSE;
		}

	}
	else
	{
		return FALSE;
	}

	if (hOpenReq)
	{
		InternetCloseHandle(hOpenReq);
	}

	if (hConnect)
	{
		InternetCloseHandle(hConnect);
	}

	if (hSession)
	{
		InternetCloseHandle(hSession);
	}

	return TRUE;
}

//登录连接服务器
int CxhNetwork::loginConnection(char* lpKbsyncValue)
{
	char*		pServerSignsapsetupbuffer = NULL;
	char*		pSendData = NULL;
	char		SendBuffer[4096] = {0};
	char		szXAppleAMdData[1024] = {0};
	char		szXAppleSignBuf[2048] = {0};
	char		szClientData[512] = {0};
	char		szSettingInfo[512] = {0};
	char		szTransportKey[MAX_PATH] = {0};
	char		szPetPassword[1024] = {0};
	char		szX_Apple_amd_m[MAX_PATH] = {0};
	char		szX_Apple_amd[128] = {0};
	TCHAR		X_Apple_I_md[MAX_PATH] ={0};
	TCHAR		X_Apple_I_md_m[512] = {0};

	int			signSapLength = 0;
	int			outSignLength = 0;
	int			envContext;
	int			petPwdLen = 0;
	int         settingInfoLen  = 0;
	int			transportkeyLen = 0;
	int         status = STATUS_FAILED;
	BOOL		bResult = FALSE;
	BOOL		res = FALSE;
	int			Res = 0;

	MultiByteToWideChar(CP_ACP,0,m_strX_Apple_I_md_m.c_str(),m_strX_Apple_I_md_m.length(),X_Apple_I_md_m,512);
	MultiByteToWideChar(CP_ACP,0,m_strX_Apple_I_md.c_str(),m_strX_Apple_I_md.length(),X_Apple_I_md,MAX_PATH);

	//发送signSapSetupCert包
	if (m_strServerSignBuf.empty())
	{
		bResult = InitSignSapSetupCert(_T("https://init.itunes.apple.com"),
								   _T("GET"),
								   _T("/WebObjects/MZInit.woa/wa/signSapSetupCert"),
								   X_Apple_I_md_m,
								   X_Apple_I_md);
		if (!bResult)
		{
			m_log.xhLogPrintf(INITSIGNSAPSETUPCERT_FAILED,__LINE__,__FUNCTION__,__FILE__);
			return STATUS_FAILED;
		}
	}
	//调用接口,得到sign-sap-setup-buffer的值
	EnterCriticalSection(&g_cs);
	if (m_strXAppleSignBuf.empty())
	{
		signSapLength = m_strServerSignBuf.length();
		int Res = pGetSignsapSetupBuf(m_pGsaService,
									(char*)m_strServerSignBuf.c_str(),
									signSapLength,
									m_pSignsapsetupbuffer,
									outSignLength,
									FALSE,
									NULL,
									0); 

		//保存sign-sap-setup-buffer值到m_strXAppleSigBuf,这个值在购买之前发送sp包的时候会再次使用
		memcpy(szXAppleSignBuf,m_pSignsapsetupbuffer,outSignLength);

		m_strXAppleSignBuf = szXAppleSignBuf;
	}
	else
	{
		strcpy(szXAppleSignBuf,m_strXAppleSignBuf.c_str());
	}
	LeaveCriticalSection(&g_cs);
	
	pServerSignsapsetupbuffer = (char*)VirtualAlloc(NULL,1024*2,MEM_COMMIT | MEM_RESERVE,PAGE_EXECUTE_READWRITE);
	if (!pServerSignsapsetupbuffer)
	{
		m_log.xhLogPrintf(ALLOCATE_MEM_FAILED,__LINE__,__FUNCTION__,__FILE__);
		return STATUS_FAILED;
	}

	//发送signSapSetup包
	memset(pServerSignsapsetupbuffer,0,2048);
	bResult = ResponseServer(_T("https://buy.itunes.apple.com"),
							 _T("POST"),
							 _T("/WebObjects/MZPlay.woa/wa/signSapSetup"),
							 X_Apple_I_md_m,
							 X_Apple_I_md,
							 szXAppleSignBuf,
							 pServerSignsapsetupbuffer,
							 outSignLength);
	if (!bResult)
	{
		m_log.xhLogPrintf(RESPONSESERVER_FAILED,__LINE__,__FUNCTION__,__FILE__);
		goto EXIT1;
	}
	//获取Pet password
	res = pGenerateSrpPETPassword(m_pGsaService,szPetPassword,petPwdLen);
	if (szPetPassword[0] == '\0')
	{
		strcpy(szPetPassword,m_strPassword.c_str());
	}
							 
	//组装用户xml数据
	m_ViewPage.loginXmlData(SendBuffer,(char*)m_strAppleId.c_str(),szPetPassword,(char*)m_strUdid.c_str(),lpKbsyncValue,FALSE);
	int SendLength = strlen(SendBuffer);

	//计算X-Apple-ActionSignature的值
	EnterCriticalSection(&g_cs);

	signSapLength = outSignLength;
	memset(m_pSignsapsetupbuffer,0,2048);
	Res = pGetSignsapSetupBuf(m_pGsaService,
								pServerSignsapsetupbuffer,
								signSapLength,
								m_pSignsapsetupbuffer,
								outSignLength,
								TRUE,
								SendBuffer,
								SendLength); 

	m_strSignsapsetup = m_pSignsapsetupbuffer;

    LeaveCriticalSection(&g_cs);

	//发送登录权限包
	status = ResponseServerWithSignature(_T("https://buy.itunes.apple.com"),
										 m_ApplePreferencesAgent,
										_T("POST"),
										_T("/WebObjects/MZFinance.woa/wa/authenticate"),
										NULL,
										SendBuffer,
										strlen(SendBuffer),
										(char*)m_strSignsapsetup.c_str(),//m_pSignsapsetupbuffer,
										szXAppleAMdData);
	if (status == SP_AUTH_ERROR)
	{
		//根据返回的x-apple-amd-data的值给设备授权,第一次匿名授权，获取clientdata的值
		::EnterCriticalSection(&g_cs);
		res = pGenerateClientData(m_pGsaService,szXAppleAMdData,strlen(szXAppleAMdData),NULL,szClientData,envContext);
		::LeaveCriticalSection(&g_cs);

		//分配内存
		pSendData = (char*)VirtualAlloc(NULL,ALLOCATE_SIZE,MEM_COMMIT | MEM_RESERVE,PAGE_EXECUTE_READWRITE);
		if (!pSendData)
		{
			m_log.xhLogPrintf(ALLOCATE_MEM_FAILED,__LINE__,__FUNCTION__,__FILE__);
			status = STATUS_FAILED;
			goto EXIT1;
		}
		//得到发送的数据
		memset(pSendData,0,ALLOCATE_SIZE);
		m_ViewPage.getFinishMachineProvisioningData(pSendData,szClientData,NULL,(char*)m_strUdid.c_str(),NULL);

		CString strCookieData = getSendCookie();
		//发送校验数据
		bResult = m_ViewPage.ProvisioningReport(m_userAgent,
												TEXT("https://play.itunes.apple.com"),
												TEXT("/WebObjects/MZPlay.woa/wa/anonymousFinishProvisioning"),
												NULL,
												NULL,
												strCookieData,
												X_Apple_I_md_m,
												X_Apple_I_md,
												NULL,
												NULL,
												pSendData,
												strlen(pSendData),
												szSettingInfo,
												szTransportKey,
												g_Proxy,
												g_netType);
		if (!bResult)
		{
			bResult = m_ViewPage.ProvisioningReport(m_userAgent,
												TEXT("https://play.itunes.apple.com"),
												TEXT("/WebObjects/MZPlay.woa/wa/anonymousFinishProvisioning"),
												NULL,
												NULL,
												strCookieData,
												X_Apple_I_md_m,
												X_Apple_I_md,
												NULL,
												NULL,
												pSendData,
												strlen(pSendData),
												szSettingInfo,
												szTransportKey,
												g_Proxy,
												g_netType);
			if (!bResult)
			{
				m_log.xhLogPrintf(PROVISIONINGREPORT_FAILED,__LINE__,__FUNCTION__,__FILE__);
				status = STATUS_FAILED;
				goto EXIT1;
			}
		}

		//设置用于计算AMD值的SettingInfo和transportKey的值
		::EnterCriticalSection(&g_cs);
		settingInfoLen = strlen(szSettingInfo);
		transportkeyLen = strlen(szTransportKey);
		res = pSetFinishProvisionData(m_pGsaService,
									szTransportKey,
									transportkeyLen,
									szSettingInfo,
									settingInfoLen,
									envContext,
									FALSE);

		//计算AMD的值
		BOOL bRet = pGenerateXAppleAMDValue(m_pGsaService,NULL,szX_Apple_amd_m,szX_Apple_amd);
		if (szX_Apple_amd_m[0] != '\0')
		{
			m_strX_Apple_amd_m = szX_Apple_amd_m;
			m_strX_Apple_amd = szX_Apple_amd;
		}

		::LeaveCriticalSection(&g_cs);

		bResult = reLoginAppStoreForDownload((char*)m_strAppleId.c_str(), (char*)m_strPassword.c_str()/*szPetPassword*/,NULL,g_Proxy,g_netType);
		if (bResult)
		{
			status = LOGIN_SUCCESS;
		}
		else
		{
			m_log.xhLogPrintf(RELOGINAPPSTOREFORDOWNLOAD_FAILED,__LINE__,__FUNCTION__,__FILE__);
			status = STATUS_FAILED;
		}
		if (pSendData)
		{
			VirtualFree(pSendData,0,MEM_RELEASE);
			pSendData = NULL;
		}
	}//end if

EXIT1:	
	if (pServerSignsapsetupbuffer)
	{
		VirtualFree(pServerSignsapsetupbuffer,0,MEM_RELEASE);
		pServerSignsapsetupbuffer = NULL;
	}
	return status;
}

int CxhNetwork::ResponseServerWithSignature(CString strUrl,
											CString strAgent,
											CString strVerb,
											CString strHostName,
											char* lpDsid,
											char* strUserXmlData,
											int dataLength,
											char* lpSignBuf,
											char* lpXAppleAMdData)
{
	BOOL bResult = FALSE;
	URL_COMPONENTS uc;
	TCHAR scheme[MAX_PATH] = {0};
	TCHAR hostName[MAX_PATH] = {0};
	TCHAR userName[MAX_PATH] = {0};
	TCHAR password[MAX_PATH] = {0};
	TCHAR urlPath[MAX_PATH] = {0};
	TCHAR ExtraInfo[MAX_PATH] = {0};
	INTERNET_PORT nPort = 0;

	DWORD dwUrlLen = 0;
	HINTERNET hSession = NULL;
	HINTERNET hConnect = NULL;
	HINTERNET hOpenReq = NULL;
	BOOL bAddRequestHeader = FALSE;
	BOOL bSendRequest = FALSE;
	BOOL bQueryInfo = FALSE;
	
	::ZeroMemory(&uc,sizeof(URL_COMPONENTS));
	uc.dwStructSize = sizeof(URL_COMPONENTS);
	uc.lpszScheme = scheme;
	uc.lpszHostName = hostName;
	uc.lpszUserName = userName;
	uc.lpszPassword = password;
	uc.lpszUrlPath = urlPath;
	uc.lpszExtraInfo = ExtraInfo;
	uc.nPort = nPort;

	uc.dwSchemeLength = MAX_PATH;
	uc.dwHostNameLength = MAX_PATH;
	uc.dwUserNameLength = MAX_PATH;
	uc.dwPasswordLength = MAX_PATH;
	uc.dwUrlPathLength = MAX_PATH;

	TCHAR strX_Apple_I_md_m[512] = {0};
	TCHAR strX_Apple_I_md[MAX_PATH] = {0};
	TCHAR strX_Apple_amd_m[512] = {0};
	TCHAR strX_Apple_amd[MAX_PATH] = {0};
	char  szX_Apple_amd_m[MAX_PATH] = {0};
	char  szX_Apple_amd[128] = {0};
	LPGZIP lpgzipBuf = NULL;
	
	char*  ptrBegin = NULL;
	char*  ptrEnd = NULL;
	
	char szXAppleAMdData[1024] = {0};

	char szFormatTime[MAX_PATH] = {0};
	TCHAR wzFormatTime[512] = {0};
	
	CString strHeaders;
	CString strSendCookie;
	CString strAdsid;
	TCHAR strSignature[2048] = {0};
	int errorCode = 0;

	USES_CONVERSION;
	strAdsid = A2T((char*)m_strAdsid.c_str());

	//获取AMD-M和AMD的值
	EnterCriticalSection(&g_cs);
	if (m_strX_Apple_amd_m.empty())
	{
		BOOL bRet = pGenerateXAppleAMDValue(m_pGsaService,lpDsid,szX_Apple_amd_m,szX_Apple_amd);
		if (szX_Apple_amd_m[0] != '\0')
		{
			m_strX_Apple_amd_m = szX_Apple_amd_m;
			m_strX_Apple_amd = szX_Apple_amd;
		}
	}
	LeaveCriticalSection(&g_cs);

	m_ViewPage.getFormatTime(szFormatTime);
	MultiByteToWideChar(CP_ACP,0,szFormatTime,strlen(szFormatTime),wzFormatTime,512);

	BOOL bCrackUrl = InternetCrackUrl(strUrl,0,0,&uc);
	if (bCrackUrl)
	{
		if (g_netType == PROXY_NETWORK)
		{
			hSession = InternetOpen(strAgent,INTERNET_OPEN_TYPE_PROXY,g_Proxy,0,0);
		}
		else
		{
			hSession = InternetOpen(strAgent,0,0,0,0);
		}
		if (!hSession)
		{
			return LOGIN_FAILED;
		}

		hConnect = InternetConnect(hSession,uc.lpszHostName,uc.nPort,uc.lpszUserName,uc.lpszPassword,3,0,0);
		if (!hConnect)
		{
			InternetCloseHandle(hSession);
			return LOGIN_FAILED;
		}

		hOpenReq = HttpOpenRequest(hConnect,strVerb,strHostName,0,0,0,0x84801000|INTERNET_FLAG_NO_COOKIES,0); //0x84A01000是一个常数
		if (!hOpenReq)
		{
			InternetCloseHandle(hConnect);
			InternetCloseHandle(hSession);
			return LOGIN_FAILED;
		}
		
		MultiByteToWideChar(CP_ACP,NULL,lpSignBuf,strlen(lpSignBuf),strSignature,2048);
		MultiByteToWideChar(CP_ACP,0,m_strX_Apple_I_md_m.c_str(),m_strX_Apple_I_md_m.length(),strX_Apple_I_md_m,512);
		MultiByteToWideChar(CP_ACP,0,m_strX_Apple_I_md.c_str(),m_strX_Apple_I_md.length(),strX_Apple_I_md,MAX_PATH);
		MultiByteToWideChar(CP_ACP,0,m_strX_Apple_amd_m.c_str(),m_strX_Apple_amd_m.length(),strX_Apple_amd_m,512);
		MultiByteToWideChar(CP_ACP,0,m_strX_Apple_amd.c_str(),m_strX_Apple_amd.length(),strX_Apple_amd,512);

		strHeaders = TEXT("User-Agent: ");
		strHeaders += strAgent;
		strHeaders += TEXT("\r\nContent-Type: application/x-apple-plist");
		strHeaders += TEXT("\r\nAccept-Language: zh-Hans-CN");
		strHeaders += TEXT("\r\nAccept: */*");
		strHeaders += TEXT("\r\nAccept-Encoding: gzip, deflate");
		strHeaders += TEXT("\r\nX-Apple-Connection-Type: WiFi");
		strHeaders += TEXT("\r\nX-Apple-Store-Front: 143465-2,29");
		strHeaders += TEXT("\r\nX-Apple-Tz: 28800");
		strHeaders += TEXT("\r\nX-Apple-ActionSignature: ");
		strHeaders += strSignature;
		strHeaders += _T("\r\nX-Apple-I-Client-Time: ");
		strHeaders += wzFormatTime;
		
		if (strX_Apple_I_md_m[0] != TEXT('\0'))
		{
			strHeaders += TEXT("\r\nX-Apple-I-MD-RINFO: 17106176");
			strHeaders += TEXT("\r\nX-Apple-I-MD-M: ");
			strHeaders += strX_Apple_I_md_m;
			strHeaders += TEXT("\r\nX-Apple-I-MD: ");
			strHeaders += strX_Apple_I_md;
		}
		if (strX_Apple_amd_m[0] != TEXT('\0'))
		{
			strHeaders += TEXT("\r\nX-Apple-AMD-M: ");
			strHeaders += strX_Apple_amd_m;
			strHeaders += TEXT("\r\nX-Apple-AMD: ");
			strHeaders += strX_Apple_amd;
		}
		if (!strAdsid.IsEmpty())
		{
			strHeaders += TEXT("\r\nX-Apple-ADSID: ");
			strHeaders += strAdsid;
		}

		//设置cookie值
		strSendCookie = getSendCookie();
		if (!strSendCookie.IsEmpty())
		{
			strHeaders += TEXT("\r\nCookie: ");
			strHeaders += strSendCookie;
		}
		
		strHeaders += TEXT("\r\nConnection: keep-alive");
		strHeaders += TEXT("\r\nAccept-Encoding: gzip, deflate");
		strHeaders += TEXT("\r\nProxy-Connection: keep-alive\r\n\r\n");


		DWORD dwHeaderLength = strHeaders.GetLength();
					
		bAddRequestHeader = HttpAddRequestHeaders(hOpenReq,strHeaders.GetBuffer(),dwHeaderLength,0xA0000000);//0xA0000000是一个常量
		if (!bAddRequestHeader)
		{
			InternetCloseHandle(hOpenReq);
			InternetCloseHandle(hConnect);
			InternetCloseHandle(hSession);
			return LOGIN_FAILED;
		}


		bSendRequest = HttpSendRequest(hOpenReq,NULL,-1,strUserXmlData,dataLength);
		if (!bSendRequest && GetLastError() == ERROR_INTERNET_INVALID_CA)
		{
			DWORD dwFlags;
			DWORD dwBuffLen = sizeof(dwFlags);
			InternetQueryOption (hOpenReq, INTERNET_OPTION_SECURITY_FLAGS,(LPVOID)&dwFlags, &dwBuffLen);
			dwFlags |= SECURITY_FLAG_IGNORE_UNKNOWN_CA;
			InternetSetOption (hOpenReq,INTERNET_OPTION_SECURITY_FLAGS,&dwFlags,sizeof (dwFlags) );

			//再次发送
			bSendRequest = HttpSendRequest(hOpenReq,NULL,-1,strUserXmlData,dataLength);
			if (!bSendRequest)
			{
				int error = GetLastError();//ERROR_INTERNET_SEC_CERT_REV_FAILED 
				InternetCloseHandle(hOpenReq);
				InternetCloseHandle(hConnect);
				InternetCloseHandle(hSession);
				return LOGIN_FAILED;
			}
		}

		DWORD dwQueryBuf = 200;
		DWORD dwQueryBufferLength = 4;
		bQueryInfo = HttpQueryInfo(hOpenReq,0x20000013,&dwQueryBuf,&dwQueryBufferLength,NULL);//HttpQueryInfo 获取文件大小后
		if (!bQueryInfo)
		{
			InternetCloseHandle(hOpenReq);
			InternetCloseHandle(hConnect);
			InternetCloseHandle(hSession);
			return LOGIN_FAILED;
		}

		//查询获取cookie,获取需要的值
		parseResponseCookie(hOpenReq);
		if (dwQueryBuf == 200)
		{
			m_strAuthUrl = strUrl;
			m_strAuthHostName = strHostName;
			//分配虚拟内存保存页面数据
			DWORD dataLength = 0;
			lpgzipBuf =  (LPGZIP)VirtualAlloc(NULL,1024*50,MEM_COMMIT | MEM_RESERVE,PAGE_EXECUTE_READWRITE);//分配800K的虚拟内存
			if (lpgzipBuf != NULL)
			{
				DWORD dwReadLength = 0;
				DWORD dwMaxDataLength = 200;
				BOOL bReadFile = FALSE;
				char DataBuffer[200] = {0};

				while (true)//循环读取页面数据
				{
					bReadFile = InternetReadFile(hOpenReq,DataBuffer,dwMaxDataLength,&dwReadLength);
					if (bReadFile && dwReadLength != 0)
					{
						memcpy(lpgzipBuf + dataLength,DataBuffer,dwReadLength);
						dataLength += dwReadLength;														
						dwReadLength = 0;	

					}
					else
					{
						lpgzipBuf[dataLength] = '\0';
						break;
					}

				}//while end

				CGZIP2A gzip2A(lpgzipBuf,dataLength);
				char * pDecodeData = gzip2A.psz;//解密出来的网页Json数据指针
				if (!pDecodeData)
				{
					if (lpgzipBuf)
					{
						VirtualFree(lpgzipBuf,0,MEM_RELEASE);
						lpgzipBuf = NULL;
					}

				}

				//判断是错误
				errorCode = ExtractErrorInfo(pDecodeData);
				if (errorCode)
				{
					//判断提取x-apple-amd-data的值
					if (SP_AUTH_ERROR == errorCode)
					{
						//获取X-Apple-amd-data的值
						m_ViewPage.getXAppleAMdDataValue(hOpenReq,szXAppleAMdData);
						memcpy(lpXAppleAMdData,szXAppleAMdData,strlen(szXAppleAMdData));
						
					}
					if (hOpenReq)
					{
						InternetCloseHandle(hOpenReq);
					}
					if (hConnect)
					{
						InternetCloseHandle(hConnect);
					}
					if (hSession)
					{
						InternetCloseHandle(hSession);
					}

					if (lpgzipBuf)
					{
						VirtualFree(lpgzipBuf,0,MEM_RELEASE);
						lpgzipBuf = NULL;
					}
					return errorCode;
				}

				//Get key section and value like dsid, passworktoken and so on....
				if (!GetPasswordTokenDsid(pDecodeData))
				{
					InternetCloseHandle(hOpenReq);
					InternetCloseHandle(hConnect);
					InternetCloseHandle(hSession);

					if (lpgzipBuf)
					{
						VirtualFree(lpgzipBuf,0,MEM_RELEASE);
						lpgzipBuf = NULL;
					}

					return LOGIN_FAILED;
				}
			
			}	
			if (lpgzipBuf)
			{
				VirtualFree(lpgzipBuf,0,MEM_RELEASE);
				lpgzipBuf = NULL;
			}

		}
		else if (dwQueryBuf == 307 || dwQueryBuf == 302)//POST请求重定向，用location里的地址重新post请求连接
		{
			TCHAR location[MAX_PATH] = {0};
			DWORD locationLength = sizeof(location);

			bQueryInfo = HttpQueryInfo(hOpenReq,HTTP_QUERY_LOCATION,location,&locationLength,0);
			if (!bQueryInfo)
			{
				InternetCloseHandle(hOpenReq);
				InternetCloseHandle(hConnect);
				InternetCloseHandle(hSession);
				return LOGIN_FAILED;
			}

			CString tmpBuf(location);
			CString strRedirectUrl;
			CString strRedirectHostName;
			int Index = tmpBuf.Find(_T(".com"));
			strRedirectUrl = tmpBuf.Mid(0,Index+4);
			strRedirectHostName = tmpBuf.Mid(Index+4);
			//保存重定向后的地址
			m_strAuthUrl = strRedirectUrl;
			m_strAuthHostName = strRedirectHostName;

			errorCode = ResponseServerWithSignature(strRedirectUrl,strAgent,strVerb,strRedirectHostName,lpDsid,strUserXmlData,dataLength,lpSignBuf,lpXAppleAMdData); 
			if (hOpenReq)
			{
				InternetCloseHandle(hOpenReq);
			}

			if (hConnect)
			{
				InternetCloseHandle(hConnect);
			}

			if (hSession)
			{
				InternetCloseHandle(hSession);
			}
			
			return errorCode;
			
			

		}
		else
		{
			InternetCloseHandle(hOpenReq);
			InternetCloseHandle(hConnect);
			InternetCloseHandle(hSession);
			return LOGIN_FAILED;
		}

	}
	else
	{
		return LOGIN_FAILED;
	}
	if (hOpenReq)
	{
		InternetCloseHandle(hOpenReq);
	}

	if (hConnect)
	{
		InternetCloseHandle(hConnect);
	}

	if (hSession)
	{
		InternetCloseHandle(hSession);
	}

	return LOGIN_SUCCESS;
}

string CxhNetwork::FormatString(string strData)
{
	string strFormat = "";
	CStringA strFormatA = strData.c_str();
	strFormatA.Replace("/","\\/");
	strFormat = strFormatA.GetBuffer();
	return strFormat;
}

string CxhNetwork::GenerateUdid()
{
	string strUdid;
	strUdid = MakeRandomString(1,99);
	strUdid = GetStringSha1(strUdid);
	return strUdid;
}

string CxhNetwork::MakeRandomString( int nMinNum, int nMaxNum )
{
	char ASCIITABLE[] = "abcdefghijklmnopqrstuvwxyz";
    char NUMBERTABLE[] = "0123456789";
	string strRet;

	srand((unsigned) time(NULL));
	int nUserNameLen = GetRandomLength(nMinNum, nMaxNum);
	int nTableSize = sizeof(ASCIITABLE)/sizeof(char) - 1;
	for (int i = 0; i < nUserNameLen; i++)
	{
	    strRet += ASCIITABLE[rand()%nTableSize];
	}

    return strRet;
}

int CxhNetwork::GetRandomLength( int nMinLen, int nMaxLen )
{
  if (nMinLen >= nMaxLen || nMaxLen < 0 || nMinLen < 0)
  {
    return 0;
  }

  srand((unsigned) time(NULL));
  int nRet = rand()%(nMaxLen - nMinLen) + nMinLen;
  return nRet;
}

BOOL CxhNetwork::SendfuseNoCarrierBundleSubscription()
{
	BOOL bResult = FALSE;
	URL_COMPONENTS uc;
	TCHAR scheme[MAX_PATH] = {0};
	TCHAR hostName[MAX_PATH] = {0};
	TCHAR userName[MAX_PATH] = {0};
	TCHAR password[MAX_PATH] = {0};
	TCHAR urlPath[MAX_PATH] = {0};
	TCHAR ExtraInfo[MAX_PATH] = {0};
	INTERNET_PORT nPort = 0;

	HINTERNET hSession = NULL;
	HINTERNET hConnect = NULL;
	HINTERNET hOpenReq = NULL;
	BOOL bAddRequestHeader = FALSE;
	BOOL bSendRequest = FALSE;
	BOOL bQueryInfo = FALSE;
	
	::ZeroMemory(&uc,sizeof(URL_COMPONENTS));
	uc.dwStructSize = sizeof(URL_COMPONENTS);
	uc.lpszScheme = scheme;
	uc.lpszHostName = hostName;
	uc.lpszUserName = userName;
	uc.lpszPassword = password;
	uc.lpszUrlPath = urlPath;
	uc.lpszExtraInfo = ExtraInfo;
	uc.nPort = nPort;

	uc.dwSchemeLength = MAX_PATH;
	uc.dwHostNameLength = MAX_PATH;
	uc.dwUserNameLength = MAX_PATH;
	uc.dwPasswordLength = MAX_PATH;
	uc.dwUrlPathLength = MAX_PATH;

	CString strSendCookie;
	CString strUrl = _T("https://play.itunes.apple.com");
	CString strHostName = _T("/WebObjects/MZPlay.woa/wa/fuseNoCarrierBundleSubscription");
	
	char szFormatTime[128] = {0};
	TCHAR wzFormatTime[MAX_PATH] = {0};

	char szX_Apple_amd_m[MAX_PATH] = {0};
	char szX_Apple_amd[128] = {0};

	USES_CONVERSION;
	string strIMEI = T2A(m_strIMEI);
	CString strX_Apple_I_md_m = A2T(getX_Apple_I_md_m().c_str());
	CString strX_Apple_I_md = A2T(getX_Apple_I_md().c_str());

	CString strX_Apple_amd_m = A2T(getX_Apple_amd_m().c_str());
	CString strX_Apple_amd = A2T(getX_Apple_amd().c_str());

	::EnterCriticalSection(&g_cs);
	//生成x-apple-amd-m和x-apple-amd的值
	int res = pGenerateXAppleAMDValue(m_pGsaService,(char*)m_strDsid.c_str(),szX_Apple_amd_m,szX_Apple_amd);
	if (szX_Apple_amd_m[0] != '\0')
	{
		setX_Apple_amd_m(szX_Apple_amd_m);
		setX_Apple_amd(szX_Apple_amd);
		strX_Apple_amd_m = A2T(szX_Apple_amd_m);
		strX_Apple_amd = A2T(szX_Apple_amd);
	}
	::LeaveCriticalSection(&g_cs);

	char sendData[512] = {0};
	sprintf(sendData, "{\"MCC\":\"\",\"carrier\":\"中国移动\",\"IMEI\":\"%s\",\"MNC\":\"\",\"guid\":\"%s\"}",strIMEI.c_str(),m_strUdid.c_str());


	BOOL bCrackUrl = InternetCrackUrl(strUrl,0,0,&uc);
	if (bCrackUrl)
	{
		if (g_netType == PROXY_NETWORK)
		{
			hSession = InternetOpen(m_itunesstoredUserAgent,INTERNET_OPEN_TYPE_PROXY,g_Proxy,0,0);
		}
		else
		{
			hSession = InternetOpen(m_itunesstoredUserAgent,0,0,0,0);
		}
		if (!hSession)
		{
			return FALSE;
		}

		hConnect = InternetConnect(hSession,uc.lpszHostName,uc.nPort,uc.lpszUserName,uc.lpszPassword,3,0,0);
		if (!hConnect)
		{
			InternetCloseHandle(hSession);
			return FALSE;
		}

		hOpenReq = HttpOpenRequest(hConnect,_T("POST"),strHostName,0,0,0,0x84801000|INTERNET_FLAG_NO_COOKIES,0);//0x84A01000是一个常数,注意
		if (!hOpenReq)
		{
			InternetCloseHandle(hConnect);
			InternetCloseHandle(hSession);
			return FALSE;
		}

		CViewPage::getFormatTime(szFormatTime);
		MultiByteToWideChar(CP_ACP,0,szFormatTime,strlen(szFormatTime),wzFormatTime,MAX_PATH);

		CString strHeaders = _T("Host: play.itunes.apple.com");
		strHeaders += _T("\r\nUser-Agent: ");
		strHeaders += m_itunesstoredUserAgent;
		strHeaders += _T("\r\nAccept-Language: zh-cn\r\nX-Apple-Store-Front: 143465-19,29");
		strHeaders += _T("\r\nAccept: */*");
		strHeaders += TEXT("\r\nX-Apple-I-MD-RINFO: 17106176");
		strHeaders += TEXT("\r\nX-Apple-Connection-Type: WiFi");
		strHeaders += TEXT("\r\nX-Apple-I-MD: ");
		strHeaders += strX_Apple_I_md;
		strHeaders += TEXT("\r\nX-Apple-I-MD-M: ");
		strHeaders += strX_Apple_I_md_m;

		if (!strX_Apple_amd_m.IsEmpty())
		{
			strHeaders += TEXT("\r\nX-Apple-AMD-M: ");
			strHeaders += strX_Apple_amd_m;
			strHeaders += TEXT("\r\n\X-Apple-AMD: ");
			strHeaders += strX_Apple_amd;
		}

		//设置cookie值
		strSendCookie = getSendCookie();
		if (!strSendCookie.IsEmpty())
		{
			strHeaders += TEXT("\r\nCookie: ");
			strHeaders += strSendCookie;
		}
		strHeaders += _T("\r\nX-Apple-I-Client-Time: ");
		strHeaders += wzFormatTime;
		strHeaders += _T("\r\nAccept-Encoding: gzip, deflate");
		strHeaders += _T("\r\nX-Apple-Tz: 28800");
		strHeaders += _T("\r\nConnection: keep-alive\r\n\r\n");

		DWORD dwHeaderLength = wcslen(strHeaders);

		bAddRequestHeader = HttpAddRequestHeaders(hOpenReq,strHeaders,dwHeaderLength,0xA0000000);//0xA0000000是一个常量
		if (!bAddRequestHeader)
		{
			InternetCloseHandle(hOpenReq);
			InternetCloseHandle(hConnect);
			InternetCloseHandle(hSession);
			return FALSE;
		}

		bSendRequest = HttpSendRequestW(hOpenReq,NULL,-1,sendData,strlen(sendData));//第三个参数设置为负一
		if (!bSendRequest && GetLastError() == ERROR_INTERNET_INVALID_CA)
		{
			DWORD dwFlags;
			DWORD dwBuffLen = sizeof(dwFlags);
			InternetQueryOption (hOpenReq, INTERNET_OPTION_SECURITY_FLAGS,(LPVOID)&dwFlags, &dwBuffLen);
			dwFlags |= SECURITY_FLAG_IGNORE_UNKNOWN_CA;
			InternetSetOption (hOpenReq,INTERNET_OPTION_SECURITY_FLAGS,&dwFlags,sizeof (dwFlags) );

			//再次发送
			bSendRequest = HttpSendRequest(hOpenReq,NULL,-1,sendData,strlen(sendData));
			if (!bSendRequest)
			{
				int error = GetLastError();//ERROR_INTERNET_SEC_CERT_REV_FAILED 
				InternetCloseHandle(hOpenReq);
				InternetCloseHandle(hConnect);
				InternetCloseHandle(hSession);
				return FALSE;
			}
		}

		DWORD dwQueryBuf = 200;
		DWORD dwQueryBufferLength = 4;

		BOOL bQueryInfo = HttpQueryInfo(hOpenReq,0x20000013,&dwQueryBuf,&dwQueryBufferLength,NULL);//HttpQueryInfo 获取文件大小后
		if (!bQueryInfo)
		{
			InternetCloseHandle(hOpenReq);
			InternetCloseHandle(hConnect);
			InternetCloseHandle(hSession);
			return FALSE;
		}
		if (dwQueryBuf != 200)
		{
			InternetCloseHandle(hOpenReq);
			InternetCloseHandle(hConnect);
			InternetCloseHandle(hSession);
			return FALSE;
		}
		
	}
	else
	{
		return FALSE;
	}

	if (hOpenReq)
	{
		InternetCloseHandle(hOpenReq);
	}

	if (hConnect)
	{
		InternetCloseHandle(hConnect);
	}

	if (hSession)
	{
		InternetCloseHandle(hSession);
	}
	
	return TRUE;
}

BOOL CxhNetwork::SendSubscriptionStatusSrv()
{
	BOOL bResult = FALSE;
	URL_COMPONENTS uc;
	TCHAR scheme[MAX_PATH] = {0};
	TCHAR hostName[MAX_PATH] = {0};
	TCHAR userName[MAX_PATH] = {0};
	TCHAR password[MAX_PATH] = {0};
	TCHAR urlPath[MAX_PATH] = {0};
	TCHAR ExtraInfo[MAX_PATH] = {0};
	INTERNET_PORT nPort = 0;

	HINTERNET hSession = NULL;
	HINTERNET hConnect = NULL;
	HINTERNET hOpenReq = NULL;
	BOOL bAddRequestHeader = FALSE;
	BOOL bSendRequest = FALSE;
	BOOL bQueryInfo = FALSE;
	
	::ZeroMemory(&uc,sizeof(URL_COMPONENTS));
	uc.dwStructSize = sizeof(URL_COMPONENTS);
	uc.lpszScheme = scheme;
	uc.lpszHostName = hostName;
	uc.lpszUserName = userName;
	uc.lpszPassword = password;
	uc.lpszUrlPath = urlPath;
	uc.lpszExtraInfo = ExtraInfo;
	uc.nPort = nPort;

	uc.dwSchemeLength = MAX_PATH;
	uc.dwHostNameLength = MAX_PATH;
	uc.dwUserNameLength = MAX_PATH;
	uc.dwPasswordLength = MAX_PATH;
	uc.dwUrlPathLength = MAX_PATH;
	CString strSendCookie;
	CString strUrl = _T("https://play.itunes.apple.com");
	TCHAR strUdid[128] = {0};
	TCHAR strHostName[MAX_PATH] = {0};
	char szFormatTime[128] = {0};
	TCHAR wzFormatTime[MAX_PATH] = {0};

	USES_CONVERSION;
	CString strX_Apple_I_md_m = A2T(getX_Apple_I_md_m().c_str());
	CString strX_Apple_I_md = A2T(getX_Apple_I_md().c_str());

	MultiByteToWideChar(CP_ACP,0,(LPCSTR)m_strUdid.c_str(),m_strUdid.length(),strUdid,128);

	swprintf(strHostName,_T("/WebObjects/MZPlay.woa/wa/getSubscriptionStatusSrv?guid=%s"),strUdid);
	


	BOOL bCrackUrl = InternetCrackUrl(strUrl,0,0,&uc);
	if (bCrackUrl)
	{
		if (g_netType == PROXY_NETWORK)
		{
			hSession = InternetOpen(m_itunesstoredUserAgent,INTERNET_OPEN_TYPE_PROXY,g_Proxy,0,0);
		}
		else
		{
			hSession = InternetOpen(m_itunesstoredUserAgent,0,0,0,0);
		}
		if (!hSession)
		{
			return FALSE;
		}

		hConnect = InternetConnect(hSession,uc.lpszHostName,uc.nPort,uc.lpszUserName,uc.lpszPassword,3,0,0);
		if (!hConnect)
		{
			InternetCloseHandle(hSession);
			return FALSE;
		}

		hOpenReq = HttpOpenRequest(hConnect,_T("GET"),strHostName,0,0,0,0x84801000|INTERNET_FLAG_NO_COOKIES,0);//0x84A01000是一个常数,注意
		if (!hOpenReq)
		{
			InternetCloseHandle(hConnect);
			InternetCloseHandle(hSession);
			return FALSE;
		}

		CViewPage::getFormatTime(szFormatTime);
		MultiByteToWideChar(CP_ACP,0,szFormatTime,strlen(szFormatTime),wzFormatTime,MAX_PATH);

		CString strHeaders = _T("Host: play.itunes.apple.com");
		strHeaders += _T("\r\nUser-Agent: ");
		strHeaders += m_itunesstoredUserAgent;
		strHeaders += _T("\r\nAccept-Language: zh-cn\r\nX-Apple-Store-Front: 143465-19,29");
		strHeaders += _T("\r\nAccept: */*");
		strHeaders += TEXT("\r\nX-Apple-I-MD-RINFO: 17106176");
		strHeaders += TEXT("\r\nX-Apple-Connection-Type: WiFi");
		strHeaders += TEXT("\r\nX-Apple-I-MD: ");
		strHeaders += strX_Apple_I_md;
		strHeaders += TEXT("\r\nX-Apple-I-MD-M: ");
		strHeaders += strX_Apple_I_md_m;

		//设置cookie值
		strSendCookie = getSendCookie();
		if (!strSendCookie.IsEmpty())
		{
			strHeaders += TEXT("\r\nCookie: ");
			strHeaders += strSendCookie;
		}
		strHeaders += _T("\r\nX-Apple-I-Client-Time: ");
		strHeaders += wzFormatTime;
		strHeaders += _T("\r\nAccept-Encoding: gzip, deflate");
		strHeaders += _T("\r\nX-Apple-Tz: 28800");
		strHeaders += _T("\r\nConnection: keep-alive\r\n\r\n");

		DWORD dwHeaderLength = wcslen(strHeaders);

		bAddRequestHeader = HttpAddRequestHeaders(hOpenReq,strHeaders,dwHeaderLength,0xA0000000);//0xA0000000是一个常量
		if (!bAddRequestHeader)
		{
			InternetCloseHandle(hOpenReq);
			InternetCloseHandle(hConnect);
			InternetCloseHandle(hSession);
			return FALSE;
		}

		bSendRequest = HttpSendRequestW(hOpenReq,NULL,-1,NULL,0);//第三个参数设置为负一
		if (!bSendRequest && GetLastError() == ERROR_INTERNET_INVALID_CA)
		{
			DWORD dwFlags;
			DWORD dwBuffLen = sizeof(dwFlags);
			InternetQueryOption (hOpenReq, INTERNET_OPTION_SECURITY_FLAGS,(LPVOID)&dwFlags, &dwBuffLen);
			dwFlags |= SECURITY_FLAG_IGNORE_UNKNOWN_CA;
			InternetSetOption (hOpenReq,INTERNET_OPTION_SECURITY_FLAGS,&dwFlags,sizeof (dwFlags) );

			//再次发送
			bSendRequest = HttpSendRequest(hOpenReq,NULL,-1,NULL,0);
			if (!bSendRequest)
			{
				int error = GetLastError();//ERROR_INTERNET_SEC_CERT_REV_FAILED 
				InternetCloseHandle(hOpenReq);
				InternetCloseHandle(hConnect);
				InternetCloseHandle(hSession);
				return FALSE;
			}
		}

		DWORD dwQueryBuf = 200;
		DWORD dwQueryBufferLength = 4;

		BOOL bQueryInfo = HttpQueryInfo(hOpenReq,0x20000013,&dwQueryBuf,&dwQueryBufferLength,NULL);//HttpQueryInfo 获取文件大小后
		if (!bQueryInfo)
		{
			InternetCloseHandle(hOpenReq);
			InternetCloseHandle(hConnect);
			InternetCloseHandle(hSession);
			return FALSE;
		}
		//获取cookie中amp的值
		parseResponseCookie(hOpenReq);

		if (dwQueryBuf != 200)
		{
			InternetCloseHandle(hOpenReq);
			InternetCloseHandle(hConnect);
			InternetCloseHandle(hSession);
			return FALSE;
		}
		
	}
	else
	{
		return FALSE;
	}

	if (hOpenReq)
	{
		InternetCloseHandle(hOpenReq);
	}

	if (hConnect)
	{
		InternetCloseHandle(hConnect);
	}

	if (hSession)
	{
		InternetCloseHandle(hSession);
	}
	
	return TRUE;
}


BOOL CxhNetwork::SendDownloadDoneSuccessNotify(CString strUrl,
								   CString strArg,
								   char* pSongId,
								   char* pDownloadId,
								   CString strPwdToken,
								   CString strDsid,
								   CString strSaleId,
								   CString strRefUrl,
								   TCHAR* lpX_Apple_I_md_m,
								   TCHAR* lpX_Apple_I_md)
{
	BOOL bResult = FALSE;
	URL_COMPONENTS uc;
	TCHAR scheme[MAX_PATH] = {0};
	TCHAR hostName[MAX_PATH] = {0};
	TCHAR userName[MAX_PATH] = {0};
	TCHAR password[MAX_PATH] = {0};
	TCHAR urlPath[MAX_PATH] = {0};
	TCHAR ExtraInfo[MAX_PATH] = {0};
	INTERNET_PORT nPort = 0;

	CString strAgent;
	HINTERNET hSession = NULL;
	HINTERNET hConnect = NULL;
	HINTERNET hOpenReq = NULL;
	BOOL bAddRequestHeader = FALSE;
	BOOL bSendRequest = FALSE;
	BOOL bQueryInfo = FALSE;
	
	::ZeroMemory(&uc,sizeof(URL_COMPONENTS));
	uc.dwStructSize = sizeof(URL_COMPONENTS);
	uc.lpszScheme = scheme;
	uc.lpszHostName = hostName;
	uc.lpszUserName = userName;
	uc.lpszPassword = password;
	uc.lpszUrlPath = urlPath;
	uc.lpszExtraInfo = ExtraInfo;
	uc.nPort = nPort;

	uc.dwSchemeLength = MAX_PATH;
	uc.dwHostNameLength = MAX_PATH;
	uc.dwUserNameLength = MAX_PATH;
	uc.dwPasswordLength = MAX_PATH;
	uc.dwUrlPathLength = MAX_PATH;
	
	char strData[MAX_PATH] = {0};
	TCHAR* lpBuffer = NULL;

	sprintf(strData,"&songId=%s&guid=%s&download-id=%s",pSongId,m_strUdid.c_str(),pDownloadId);
	
	int dwLen = MultiByteToWideChar(CP_ACP,0,strData,strlen(strData),NULL,0);
	lpBuffer = new TCHAR[dwLen + 1];
	ZeroMemory(lpBuffer,dwLen+1);
	MultiByteToWideChar(CP_ACP,0,strData,strlen(strData),lpBuffer,dwLen);
	*(lpBuffer+dwLen) = _T('\0');

	CString strTmp(lpBuffer);
	
	strAgent = this->m_comAppleAppStoredAgent;	
	CString strSendCookie;

	strArg += strTmp;

	if (lpBuffer)
	{
		delete []lpBuffer;
		lpBuffer = NULL;
	}

	char szX_Apple_amd_m[MAX_PATH] = {0};
	char szX_Apple_amd[128] = {0};

	USES_CONVERSION;

	CString strX_Apple_amd_m = A2T(getX_Apple_amd_m().c_str());
	CString strX_Apple_amd = A2T(getX_Apple_amd().c_str());

	::EnterCriticalSection(&g_cs);
	//生成x-apple-amd-m和x-apple-amd的值
	int res = pGenerateXAppleAMDValue(m_pGsaService,(char*)m_strDsid.c_str(),szX_Apple_amd_m,szX_Apple_amd);
	if (szX_Apple_amd_m[0] != '\0')
	{
		setX_Apple_amd_m(szX_Apple_amd_m);
		setX_Apple_amd(szX_Apple_amd);
		strX_Apple_amd_m = A2T(szX_Apple_amd_m);
		strX_Apple_amd = A2T(szX_Apple_amd);
	}
	::LeaveCriticalSection(&g_cs);


	BOOL bCrackUrl = InternetCrackUrl(strUrl,0,0,&uc);
	if (bCrackUrl)
	{
		if (g_netType == PROXY_NETWORK)
		{
			hSession = InternetOpen(strAgent,INTERNET_OPEN_TYPE_PROXY,g_Proxy,0,0);
		}
		else
		{
			hSession = InternetOpen(strAgent,0,0,0,0);
		}
		if (!hSession)
		{
			return FALSE;
		}

		hConnect = InternetConnect(hSession,uc.lpszHostName,uc.nPort,uc.lpszUserName,uc.lpszPassword,3,0,0);
		if (!hConnect)
		{
			InternetCloseHandle(hSession);
			return FALSE;
		}

		hOpenReq = HttpOpenRequest(hConnect,_T("GET"),strArg,0,0,0,0x84801000|INTERNET_FLAG_NO_COOKIES,0);//0x84A01000是一个常数,注意
		if (!hOpenReq)
		{
			InternetCloseHandle(hConnect);
			InternetCloseHandle(hSession);
			return FALSE;
		}

		CString strHeaders = _T("\r\nUser-Agent: ");
		strHeaders += strAgent;
		strHeaders += _T("\r\nAccept-Language: zh-cn\r\nX-Apple-Store-Front: 143465-19,29\r\nX-Token: ");
		strHeaders += strPwdToken;
		strHeaders += _T("\r\nX-Dsid: ");
		strHeaders += strDsid;

		strHeaders += TEXT("\r\nX-Apple-I-MD-RINFO: 17106176");
		strHeaders += TEXT("\r\nX-Apple-Connection-Type: WiFi");
		strHeaders += TEXT("\r\nX-Apple-I-MD: ");
		strHeaders += lpX_Apple_I_md;
		strHeaders += TEXT("\r\nX-Apple-I-MD-M: ");
		strHeaders += lpX_Apple_I_md_m;

		if (!strX_Apple_amd_m.IsEmpty())
		{
			strHeaders += TEXT("\r\nX-Apple-AMD-M: ");
			strHeaders += strX_Apple_amd_m;
			strHeaders += TEXT("\r\n\X-Apple-AMD: ");
			strHeaders += strX_Apple_amd;
		}

		//设置cookie值
		strSendCookie = getSendCookie();
		if (!strSendCookie.IsEmpty())
		{
			strHeaders += TEXT("\r\nCookie: ");
			strHeaders += strSendCookie;
		}
		strHeaders += _T("\r\nX-Apple-I-Client-Time: ");
		strHeaders += m_ViewPage.GetClientTime();
		strHeaders += _T("\r\nX-Apple-Tz: 28800");
		strHeaders += _T("\r\nAccept-Encoding: gzip\r\nConnection: keep-alive\r\n\r\n");

		DWORD dwHeaderLength = wcslen(strHeaders);

		bAddRequestHeader = HttpAddRequestHeaders(hOpenReq,strHeaders,dwHeaderLength,0xA0000000);//0xA0000000是一个常量
		if (!bAddRequestHeader)
		{
			InternetCloseHandle(hOpenReq);
			InternetCloseHandle(hConnect);
			InternetCloseHandle(hSession);
			return FALSE;
		}

		bSendRequest = HttpSendRequestW(hOpenReq,NULL,-1,NULL,0);//第三个参数设置为负一
		if (!bSendRequest && GetLastError() == ERROR_INTERNET_INVALID_CA)
		{
			DWORD dwFlags;
			DWORD dwBuffLen = sizeof(dwFlags);
			InternetQueryOption (hOpenReq, INTERNET_OPTION_SECURITY_FLAGS,(LPVOID)&dwFlags, &dwBuffLen);
			dwFlags |= SECURITY_FLAG_IGNORE_UNKNOWN_CA;
			InternetSetOption (hOpenReq,INTERNET_OPTION_SECURITY_FLAGS,&dwFlags,sizeof (dwFlags) );

			//再次发送
			bSendRequest = HttpSendRequest(hOpenReq,NULL,-1,NULL,0);
			if (!bSendRequest)
			{
				int error = GetLastError();//ERROR_INTERNET_SEC_CERT_REV_FAILED 
				InternetCloseHandle(hOpenReq);
				InternetCloseHandle(hConnect);
				InternetCloseHandle(hSession);
				return FALSE;
			}
		}

		DWORD dwQueryBuf = 200;
		DWORD dwQueryBufferLength = 4;

		BOOL bQueryInfo = HttpQueryInfo(hOpenReq,0x20000013,&dwQueryBuf,&dwQueryBufferLength,NULL);//HttpQueryInfo 获取文件大小后
		if (!bQueryInfo)
		{
			InternetCloseHandle(hOpenReq);
			InternetCloseHandle(hConnect);
			InternetCloseHandle(hSession);
			return FALSE;
		}

		if (dwQueryBuf == 302 || dwQueryBuf == 307)
		{
			TCHAR location[MAX_PATH] = {0};
			DWORD locationLength = sizeof(location);

			bQueryInfo = HttpQueryInfo(hOpenReq,HTTP_QUERY_LOCATION ,location,&locationLength,0);
			if (!bQueryInfo)
			{
				InternetCloseHandle(hOpenReq);
				InternetCloseHandle(hConnect);
				InternetCloseHandle(hSession);
				return FALSE;
			}

			CString tmpBuf(location);
			CString strUrlRedirect;
			CString strHostName;

			int Index = tmpBuf.Find(_T(".com"));
			strUrlRedirect = tmpBuf.Mid(0,Index+4);
			strHostName = tmpBuf.Mid(Index+4);

			SendDownloadDoneSuccessNotify(strUrlRedirect,
											   strHostName,
											   pSongId,
											   pDownloadId,
											   strPwdToken,
											   strDsid,
											   strSaleId,
											   strRefUrl,
											   lpX_Apple_I_md_m,
											   lpX_Apple_I_md);

		}
		else if (dwQueryBuf == 503)
		{
			EnterCriticalSection(&g_cs);
			++g_DownloadNotiyErrorCount;
			LeaveCriticalSection(&g_cs);
			
			if (g_DownloadNotiyErrorCount > 1)
			{
				InternetCloseHandle(hOpenReq);
				InternetCloseHandle(hConnect);
				InternetCloseHandle(hSession);
				return FALSE;
			}
			else
			{
				//再给一次处理的机会
				SendDownloadDoneSuccessNotify(strUrl,
											   strArg,
											   pSongId,
											   pDownloadId,
											   strPwdToken,
											   strDsid,
											   strSaleId,
											   strRefUrl,
											   lpX_Apple_I_md_m,
											   lpX_Apple_I_md);
				
			}
			
		}
		
	}
	else
	{
		return FALSE;
	}

	if (hOpenReq)
	{
		InternetCloseHandle(hOpenReq);
	}

	if (hConnect)
	{
		InternetCloseHandle(hConnect);
	}

	if (hSession)
	{
		InternetCloseHandle(hSession);
	}
	
	return TRUE;
}

BOOL CxhNetwork::downloadAppFile(CString strDownloadUrlPath,CString strDownloadKey,CString strUrl)
{
	BOOL bResult = FALSE;
	URL_COMPONENTS uc;
	TCHAR scheme[MAX_PATH] = {0};
	TCHAR hostName[MAX_PATH] = {0};
	TCHAR userName[MAX_PATH] = {0};
	TCHAR password[MAX_PATH] = {0};
	TCHAR urlPath[MAX_PATH] = {0};
	TCHAR ExtraInfo[MAX_PATH] = {0};
	
	INTERNET_PORT nPort = 0;

	HINTERNET hSession = NULL;
	HINTERNET hConnect = NULL;
	HINTERNET hOpenReq = NULL;
	BOOL bAddRequestHeader = FALSE;
	BOOL bSendRequest = FALSE;
	BOOL bQueryInfo = FALSE;
	
	::ZeroMemory(&uc,sizeof(URL_COMPONENTS));
	uc.dwStructSize = sizeof(URL_COMPONENTS);
	uc.lpszScheme = scheme;
	uc.lpszHostName = hostName;
	uc.lpszUserName = userName;
	uc.lpszPassword = password;
	uc.lpszUrlPath = urlPath;
	uc.lpszExtraInfo = ExtraInfo;
	uc.nPort = nPort;

	uc.dwSchemeLength = MAX_PATH;
	uc.dwHostNameLength = MAX_PATH;
	uc.dwUserNameLength = MAX_PATH;
	uc.dwPasswordLength = MAX_PATH;
	uc.dwUrlPathLength = MAX_PATH;

	CString strAgent = m_userAgent;

	BOOL bCrackUrl = InternetCrackUrl(strUrl,0,0,&uc);
	if (bCrackUrl)
	{
		if (g_netType == PROXY_NETWORK)
		{
			hSession = InternetOpen(strAgent,INTERNET_OPEN_TYPE_PROXY,g_Proxy,0,0);
		}
		else
		{
			hSession = InternetOpen(strAgent,0,0,0,0);
		}
		if (!hSession)
		{
			return FALSE;
		}

		hConnect = InternetConnect(hSession,uc.lpszHostName,uc.nPort,uc.lpszUserName,uc.lpszPassword,3,0,0);
		if (!hConnect)
		{
			InternetCloseHandle(hSession);
			return FALSE;
		}

		hOpenReq = HttpOpenRequest(hConnect,_T("GET"),strDownloadUrlPath,0,0,0,INTERNET_FLAG_RELOAD |INTERNET_FLAG_NO_CACHE_WRITE|INTERNET_FLAG_NO_COOKIES,0);//0x84A01000是一个常数
		if (!hOpenReq)
		{
			InternetCloseHandle(hConnect);
			InternetCloseHandle(hSession);
			return FALSE;
		}

		CString strHeaders = _T("Accept: */*\r\nCookie: ");
		strHeaders += strDownloadKey;
		strHeaders += _T("\r\nConnection: close\r\n\r\n");

		DWORD dwHeaderLength = strHeaders.GetLength();

		bAddRequestHeader = HttpAddRequestHeaders(hOpenReq,strHeaders,dwHeaderLength,0xA0000000);//0xA0000000是一个常量
		if (!bAddRequestHeader)
		{
			InternetCloseHandle(hOpenReq);
			InternetCloseHandle(hConnect);
			InternetCloseHandle(hSession);
			return FALSE;
		}

		bSendRequest = HttpSendRequestW(hOpenReq,NULL,-1,NULL,0);//第三个参数设置为负一
		if (!bSendRequest && GetLastError() == ERROR_INTERNET_INVALID_CA)
		{
			DWORD dwFlags;
			DWORD dwBuffLen = sizeof(dwFlags);
			InternetQueryOption (hOpenReq, INTERNET_OPTION_SECURITY_FLAGS,(LPVOID)&dwFlags, &dwBuffLen);
			dwFlags |= SECURITY_FLAG_IGNORE_UNKNOWN_CA;
			InternetSetOption (hOpenReq,INTERNET_OPTION_SECURITY_FLAGS,&dwFlags,sizeof (dwFlags) );

			//再次发送
			bSendRequest = HttpSendRequest(hOpenReq,NULL,-1,NULL,0);
			if (!bSendRequest)
			{
				int error = GetLastError();//ERROR_INTERNET_SEC_CERT_REV_FAILED 
				InternetCloseHandle(hOpenReq);
				InternetCloseHandle(hConnect);
				InternetCloseHandle(hSession);
				return FALSE;
			}
		}

		DWORD dwQueryBuf = 200;
		DWORD dwQueryBufferLength = 4;

		BOOL bQueryInfo = HttpQueryInfo(hOpenReq,0x20000013,&dwQueryBuf,&dwQueryBufferLength,NULL);//HttpQueryInfo 获取文件大小后
		if (!bQueryInfo)
		{
			InternetCloseHandle(hOpenReq);
			InternetCloseHandle(hConnect);
			InternetCloseHandle(hSession);
			return FALSE;
		}

		if (dwQueryBuf == 302)
		{
			TCHAR location[MAX_SIZE] = {0};
			DWORD locationLength = sizeof(location);

			bQueryInfo = HttpQueryInfo(hOpenReq,HTTP_QUERY_LOCATION ,location,&locationLength,0);
			if (!bQueryInfo)
			{
				InternetCloseHandle(hOpenReq);
				InternetCloseHandle(hConnect);
				InternetCloseHandle(hSession);
				return FALSE;
			}

			CString tmpBuf(location);
			CString strRedUrl;
			CString strHostName;

			int Index = tmpBuf.Find(_T(".com"));
			strRedUrl = tmpBuf.Mid(0,Index+4);
			strHostName = tmpBuf.Mid(Index+4);	

			downloadAppFile(strHostName,strDownloadKey,strRedUrl);

		}
		else if (dwQueryBuf == 200)
		{
			//这里不从网络读取实际的app数据
			InternetCloseHandle(hOpenReq);
			InternetCloseHandle(hConnect);
			InternetCloseHandle(hSession);
			return TRUE;
		}
		else
		{
		}

	}
	else
	{
		return FALSE;
	}

	if (hOpenReq)
	{
		InternetCloseHandle(hOpenReq);
	}

	if (hConnect)
	{
		InternetCloseHandle(hConnect);
	}

	if (hSession)
	{
		InternetCloseHandle(hSession);
	}


	return TRUE;
}

int xhDownloadApp(IN LPVOID lpNetwork,
				   IN LPVOID lpPasswordToken,
				   IN LPVOID lpDsid,
				   IN LPVOID lpSaleId,
				   IN LPVOID lpAppPrice,
				   BOOL bSendReport,
				   BOOL bPaidApp)
{
	if (!lpNetwork)
	{
		return FALSE;
	}
	CxhNetwork* pNetworkObj = (CxhNetwork*)lpNetwork;
	return pNetworkObj->DownloadApp(lpPasswordToken,lpDsid,lpSaleId,lpAppPrice,bSendReport,bPaidApp);

}
int CxhNetwork::DownloadApp(IN LPVOID lpPasswordToken,
							 IN LPVOID lpDsid,
							 IN LPVOID lpSaleId,
							 IN LPVOID lpAppPrice,
							 BOOL bSendReport,
							 BOOL bPaidApp)
{
	char		szSignBuf[2048] = {0};
	char		strPageDetailEvent[4096] = {0};
	char		strPageloadTime[MAX_PATH] = {0};
	char		recvSignBuf[4096] = {0};
	char		SendBuffer[512] = {0};
	char		szTimeStamp[128] = {0};
	char        szXAppleMdData[1024] = {0};
	char		szXAppleAmdData[1024] = {0};
	char		szClientData[512] = {0};
	char		szSettingInfo[512] = {0};
	char		szTransportKey[MAX_PATH] = {0};
	char		szSendData[1024] = {0};
	char		szKbsyncValue[1024] = {0};
	char		szEventtime[MAX_PATH] = {0};
	char		szMzRequestId[MAX_PATH] = {0};
	char		szVid[MAX_PATH] = {0};
	char        szPetPassword[MAX_PATH] = {0};
	TCHAR		uzAgreeAction[MAX_PATH] = {0};
	TCHAR		uzAgreeKey[MAX_PATH] = {0};
	TCHAR		uzSecurityVerifyAction[MAX_PATH] = {0};
	TCHAR		uzSubmitKey[64] = {0};
	TCHAR		strGuid[128] = {0};
	int			outDataLen = 0;
	int			signSapLength = 0;
	int			envContext = 0;
	int         settingInfoLen  = 0;
	int			transportkeyLen = 0;
	LONGLONG	Dsid = 0;
	int			petPwdLen = 0;
	BOOL		bResult = FALSE;
	char*		pReLoginSignBuf = NULL;
	char*		pReLoginSendData = NULL;
	char*		pSendData = NULL;

	char        szBuyConfirmEvent[4096] = {0};
	char        szBuyInitiateEvent[MAX_ALLOC_SIZE*2] = {0};
	char		szX_Apple_amd_m[MAX_PATH] = {0};
	char		szX_Apple_amd[128] = {0};
	TCHAR		X_Apple_amd_m[512] = {0};
	TCHAR		X_Apple_amd[MAX_PATH] = {0};
	TCHAR		strDsid[128] = {0};
	TCHAR		strPwdToken[MAX_PATH] = {0};
	TCHAR		uzWebTermsId[32] = {0};
	TCHAR		uzWebWosid_lite[MAX_PATH] = {0};
	int			buyRes = 0;
	int			buyEventType = 0;
	int         status = STATUS_FAILED;
	BOOL		bNeedWebAgree = FALSE;
	BOOL		bNeedLoginVerify = FALSE;
	BOOL		bAlwaysNeedPwd = FALSE;

	CString		strXPVerifyHostName;
	CString		strItmsAppsHostName;
	CString		strRefererUrl;
	CString		strX_Apple_I_md_m;
	CString		strX_Apple_I_md;
	CString     strXpMainSignature;

	string strI_md_m = "";
	string strI_md = "";

	CString strBuyUrl = _T("https://buy.itunes.apple.com");
	CString strBuyHostName = _T("/WebObjects/MZBuy.woa/wa/buyProduct");

	SECQUESTIONTONAME securityQuestionData;
	NAMETOANSWER nameToAnswer;

	USES_CONVERSION;
	//生成I_md_m 和I_md的值
	GenerateXApple_I_MDandMDMValue();
	strI_md_m = getX_Apple_I_md_m();
	strI_md = getX_Apple_I_md();
	strX_Apple_I_md_m = A2T((char*)strI_md_m.c_str());
	strX_Apple_I_md = A2T((char*)strI_md.c_str());

	string strUdid = getUdid();
	string strXAppleSignBuf = getXAppleSignBuf();
	string strXApple_amd_m = getX_Apple_amd_m();
	string strXApple_amd = getX_Apple_amd();
	CString strCookieData;
	try
	{
		char* pRecvSignBuf = new char[ALLOCATE_SIZE];
		if (!pRecvSignBuf)
		{
			m_log.xhLogPrintf(ALLOCATE_MEM_FAILED,__LINE__,__FUNCTION__,__FILE__);
		   return status;
		}

		Dsid = _atoi64((char*)lpDsid);

		MultiByteToWideChar(CP_ACP,0,(char*)lpPasswordToken,strlen((char*)lpPasswordToken),strPwdToken,MAX_PATH);
		MultiByteToWideChar(CP_ACP,0,(char*)lpDsid,strlen((char*)lpDsid),strDsid,128);
		MultiByteToWideChar(CP_ACP,0,strUdid.c_str(),strUdid.length(),strGuid,128);

		strCookieData = getSendCookie();
		//发送sign-sap-setup-buffer的值
		memset(pRecvSignBuf,0,ALLOCATE_SIZE);
		BOOL res = getSignSapSetup(m_itunesstoredUserAgent,(char*)strXAppleSignBuf.c_str(),pRecvSignBuf,signSapLength,(char*)lpDsid,strCookieData,strX_Apple_I_md_m.GetBuffer(),strX_Apple_I_md.GetBuffer(),g_Proxy,g_netType);
		if (!res)
		{
			m_log.xhLogPrintf(GETSIGNSAPSETUP_FAILED,__LINE__,__FUNCTION__,__FILE__);
			if (pRecvSignBuf)
			{
				delete []pRecvSignBuf;
				pRecvSignBuf = NULL;
			}
			return status;
		}
		//获取时间戳
		CTime tDateTime= CTime::GetCurrentTime();
		sprintf(szTimeStamp,"%d-%d-%d %d:%d:%d",
								tDateTime.GetYear(),
								tDateTime.GetMonth(),
								tDateTime.GetDay(),
								tDateTime.GetHour(),
								tDateTime.GetMinute(),
								tDateTime.GetSecond());

		//组装数据
		strcat(SendBuffer,(char*)lpDsid);
		strcat(SendBuffer,"143465-19,29");
		strcat(SendBuffer,szTimeStamp);
		strcat(SendBuffer,"+0800");      //注意这里,ios8.0系统，这里是有空格的" +0800"
		strcat(SendBuffer,"offer");
		strcat(SendBuffer,(char*)lpSaleId);

		int sendLength = strlen(SendBuffer);

		HMODULE hRankModule = NULL;
		hRankModule = ::GetModuleHandle(_T("xhiTunes.dll"));
		if (!hRankModule)
		{
			if (pRecvSignBuf)
			{
				delete []pRecvSignBuf;
				pRecvSignBuf = NULL;
			}
			return status;
		}
		pGetSignsapSetupBuf = (PGETSIGNSAPSETUPBUFFER)GetProcAddress(hRankModule,"GetSignsapSetupBuffer");

		::EnterCriticalSection(&g_cs);
		//调用接口，生成X-Apple-Signature的值
		res = pGetSignsapSetupBuf(m_pGsaService,
									pRecvSignBuf,
									signSapLength,
									szSignBuf,
									outDataLen,
									TRUE,
									SendBuffer,
									sendLength);
		::LeaveCriticalSection(&g_cs);

		//生成I_md_m 和I_md的值
		GenerateXApple_I_MDandMDMValue();
		strI_md_m = getX_Apple_I_md_m();
		strI_md = getX_Apple_I_md();
		strX_Apple_I_md_m = A2T((char*)strI_md_m.c_str());
		strX_Apple_I_md = A2T((char*)strI_md.c_str());

		strCookieData = getSendCookie();
		res = sendSpLookupOffer(m_itunesstoredUserAgent,szSignBuf,(char*)lpDsid,(char*)lpSaleId,szTimeStamp,strCookieData,strX_Apple_I_md_m.GetBuffer(),strX_Apple_I_md.GetBuffer(),g_Proxy,g_netType);
		if (!res)
		{
			res = sendSpLookupOffer(m_itunesstoredUserAgent,szSignBuf,(char*)lpDsid,(char*)lpSaleId,szTimeStamp,strCookieData,strX_Apple_I_md_m.GetBuffer(),strX_Apple_I_md.GetBuffer(),g_Proxy,g_netType);
			if (!res)
			{
				m_log.xhLogPrintf(SENDSPLOOKUPOFFER_FAILED,__LINE__,__FUNCTION__,__FILE__);
				if (pRecvSignBuf)
				{
					delete []pRecvSignBuf;
					pRecvSignBuf = NULL;
				}
			
				return status;
			}
		}

		//发送xp注册包
		SendxpRegister((char*)m_strDsid.c_str(),m_itunesstoredUserAgent);


		//获取时间戳
		m_ViewPage.getEventTime(szEventtime,TRUE,FALSE);

		//获取MzRequestId的值
		generateMzRequestId(szEventtime,szMzRequestId);

		//获取Vid的值
		generateVid(szVid);

		m_strAdsid = getAdsid();
		string strPasswordToken = getPasswordToken();
		string strUserPassword = getPassword();

		while (true)
		{
			//生成I_md_m 和I_md的值
			GenerateXApple_I_MDandMDMValue();
			strI_md_m = getX_Apple_I_md_m();
			strI_md = getX_Apple_I_md();
			strX_Apple_I_md_m = A2T((char*)strI_md_m.c_str());
			strX_Apple_I_md = A2T((char*)strI_md.c_str());
			buyRes = onlyBuyProduct(strBuyUrl,
									strBuyHostName,
									(char*)strPasswordToken.c_str(),
									lpDsid,
									lpSaleId,
									lpAppPrice,
									szEventtime,
									szMzRequestId,
									szVid,
									strX_Apple_I_md_m.GetBuffer(),
									strX_Apple_I_md.GetBuffer(),
									uzAgreeKey,
									uzSubmitKey,
									uzWebTermsId,
									uzWebWosid_lite,
									bPaidApp,
									buyEventType,
									szXAppleMdData,
									szXAppleAmdData);
			switch(buyRes)
			{
			case CHANGE_CONDITION:
				{
					strXPVerifyHostName = m_strXPVerifyHostName;
					strXPVerifyHostName.Replace(TEXT("amp;"),TEXT(""));
					strItmsAppsHostName = m_strItmsAppsHostName;
					strItmsAppsHostName.Replace(TEXT("amp;"),TEXT(""));
					strItmsAppsHostName += TEXT("&");
					strItmsAppsHostName += TEXT("&guid=");
					strItmsAppsHostName += strGuid;

					strCookieData = getSendCookie();
					//发送汇报xp包
					res = m_ViewPage.reLoginAppstoreXpReport(m_itunesstoredUserAgent,
											TEXT("https://xp.apple.com"),
											strXPVerifyHostName,
											strDsid,
											strCookieData,
											strX_Apple_I_md_m.GetBuffer(),
											strX_Apple_I_md.GetBuffer(),
											g_Proxy,
											g_netType);

					strCookieData = getSendCookie();
					//条款更新要求同意条款页面包
					res = m_ViewPage.itmsAppsReport(m_userAgent,
													TEXT("https://buy.itunes.apple.com"),
												   strItmsAppsHostName,
												   strDsid,
												   uzAgreeAction,
												   uzAgreeKey,
												   uzWebTermsId,
												   uzWebWosid_lite,
												   strCookieData,
												   g_Proxy,
												   g_netType);
					if (!res)
					{
						m_log.xhLogPrintf(ITMSAPPSREPORT_FAILED,__LINE__,__FUNCTION__,__FILE__);
						status = STATUS_FAILED;
						goto EXIT1;
					}

					strRefererUrl = TEXT("https://buy.itunes.apple.com");
					strRefererUrl += strItmsAppsHostName;

					//发送事件包
					strCookieData = getSendCookie();
					res = m_ViewPage.secureTermsPageReport(m_userAgent,
															TEXT("https://xp.apple.com"),
														   strRefererUrl,
														   strDsid,
														   strCookieData,
														   g_Proxy,
														   g_netType);


					//提交"同意"数据包
					strCookieData = getSendCookie();
					res = m_ViewPage.agreeDataReport(m_userAgent,
													TEXT("https://buy.itunes.apple.com"),
													 uzAgreeAction,
													 strRefererUrl,
													 strDsid,
													 uzAgreeKey,
													 uzWebTermsId,
												     uzWebWosid_lite,
													 strCookieData,
													 g_Proxy,
													 g_netType);
					if (!res)
					{
						m_log.xhLogPrintf(AGREEDATAREPORT_FAILED,__LINE__,__FUNCTION__,__FILE__);
						status = STATUS_FAILED;
						goto EXIT1;
					}

					strRefererUrl = TEXT("https://buy.itunes.apple.com");
					strRefererUrl += uzAgreeAction;

					//发送事件包
					strCookieData = getSendCookie();
					res = m_ViewPage.secureTermsPageReport(m_userAgent,
															TEXT("https://xp.apple.com"),
														   strRefererUrl,
														   strDsid,
														   strCookieData,
														   g_Proxy,
														   g_netType);

					//发送同意购买协议包
					buyEventType = CHANGE_CONDITION;

				}break;
			case BUY_LOGIN_VERIFY:
				{
					//生成I_md_m 和I_md的值
					GenerateXApple_I_MDandMDMValue();
					strI_md_m = getX_Apple_I_md_m();
					strI_md = getX_Apple_I_md();
					strX_Apple_I_md_m = A2T((char*)strI_md_m.c_str());
					strX_Apple_I_md = A2T((char*)strI_md.c_str());

					strXPVerifyHostName = m_strXPVerifyHostName;
					strXPVerifyHostName.Replace(TEXT("amp;"),TEXT(""));
					strItmsAppsHostName = m_strItmsAppsHostName;
					strItmsAppsHostName.Replace(TEXT("amp;"),TEXT(""));
					strItmsAppsHostName += TEXT("&");
					strItmsAppsHostName += TEXT("&guid=");
					strItmsAppsHostName += strGuid;

					//获取购买初始化xp包
					strCookieData = getSendCookie();
					m_ViewPage.xpBuyInitiateEvent(m_FormatUserAgent,
												  (char*)m_version.c_str(),
												  m_width,
												  m_height,
												  (char*)m_strSearchKeyword.c_str(),
												  szBuyInitiateEvent,
												  (char*)m_strClientId.c_str(),							  
												  (char*)lpSaleId,
												  (char*)lpDsid,
												  (char*)m_strxp_ab_value.c_str(),
												  (char*)m_strxp_abc_value.c_str());
					m_strSHA1 = GetStringSha1(szBuyInitiateEvent);
					strXpMainSignature = GetxpMainXAppleActionSignature(m_strSHA1);
					//发送购买初始化xp包
					if (!m_ViewPage.xpReportTargetAppMain(m_itunesstoredUserAgent,szBuyInitiateEvent,(char*)lpPasswordToken,(char*)lpDsid,strCookieData,strXpMainSignature,strX_Apple_I_md_m.GetBuffer(),strX_Apple_I_md.GetBuffer(),g_Proxy,g_netType))
					{
						if (!m_ViewPage.xpReportTargetAppMain(m_itunesstoredUserAgent,szBuyInitiateEvent,(char*)lpPasswordToken,(char*)lpDsid,strCookieData,strXpMainSignature,strX_Apple_I_md_m.GetBuffer(),strX_Apple_I_md.GetBuffer(),g_Proxy,g_netType))
						{
							m_log.xhLogPrintf(XPREPORTTARGETAPPMAIN_FAILED,__LINE__,__FUNCTION__,__FILE__);
							status = STATUS_FAILED;
						}
					}

					//发送汇报xp事件通知包
					strCookieData = getSendCookie();
					res = m_ViewPage.reLoginAppstoreXpReport(m_itunesstoredUserAgent,
															TEXT("https://xp.apple.com"),
															strXPVerifyHostName,
															strDsid,
															strCookieData,
															strX_Apple_I_md_m.GetBuffer(),
															strX_Apple_I_md.GetBuffer(),
															g_Proxy,
															g_netType);
					//重新登录验证
					res = preLoginGsaServicesVerifier((char*)m_strAdsid.c_str(),(char*)strUserPassword.c_str(),(char*)strUdid.c_str(),(char*)strI_md.c_str(),(char*)strI_md_m.c_str());
					if (!res)
					{
						m_log.xhLogPrintf(LOGINGSASERVER_FAILED,__LINE__,__FUNCTION__,__FILE__);
						pReleaseGsaAllocate(m_pGsaService);
						status = STATUS_FAILED;
						goto EXIT1;
					}
					//获取Pet password
					res = pGenerateSrpPETPassword(m_pGsaService,szPetPassword,petPwdLen);
					if (szPetPassword[0] == '\0')
					{
						strcpy(szPetPassword,m_strPassword.c_str());
					}
					
					res = reLoginAppStoreForDownload((char*)m_strAppleId.c_str(),szPetPassword,(char*)lpDsid,g_Proxy,g_netType);
					if (!res)
					{
						m_log.xhLogPrintf(RELOGINAPPSTOREFORDOWNLOAD_FAILED,__LINE__,__FUNCTION__,__FILE__);
						status = STATUS_FAILED;
						goto EXIT1;
					}
					pReleaseGsaAllocate(m_pGsaService);

					//发送注册xp包
					SendxpRegister((char*)m_strAdsid.c_str(),m_userAgent);

					buyEventType = BUY_LOGIN_VERIFY;

				}break;
			case NEED_ALOWAYS_PWD:
				{
					//生成I_md_m 和I_md的值
					GenerateXApple_I_MDandMDMValue();
					strI_md_m = getX_Apple_I_md_m();
					strI_md = getX_Apple_I_md();
					strX_Apple_I_md_m = A2T((char*)strI_md_m.c_str());
					strX_Apple_I_md = A2T((char*)strI_md.c_str());

					strXPVerifyHostName = m_strXPVerifyHostName;
					strXPVerifyHostName.Replace(TEXT("amp;"),TEXT(""));
					strItmsAppsHostName = m_strItmsAppsHostName;
					strItmsAppsHostName.Replace(TEXT("amp;"),TEXT(""));
					//发送汇报xp事件通知包
					strCookieData = getSendCookie();
					res = m_ViewPage.reLoginAppstoreXpReport(m_itunesstoredUserAgent,
															TEXT("https://xp.apple.com"),
															strXPVerifyHostName,
															strDsid,
															strCookieData,
															strX_Apple_I_md_m.GetBuffer(),
															strX_Apple_I_md.GetBuffer(),
															g_Proxy,
															g_netType);
					//购买始终需要密码
					buyEventType = NEED_ALOWAYS_PWD;

				}break;
			case SAVE_PWD_FOR_FREE_APP:
				{
					//生成I_md_m 和I_md的值
					GenerateXApple_I_MDandMDMValue();
					strI_md_m = getX_Apple_I_md_m();
					strI_md = getX_Apple_I_md();
					strX_Apple_I_md_m = A2T((char*)strI_md_m.c_str());
					strX_Apple_I_md = A2T((char*)strI_md.c_str());

					strXPVerifyHostName = m_strXPVerifyHostName;
					strXPVerifyHostName.Replace(TEXT("amp;"),TEXT(""));
					strItmsAppsHostName = m_strItmsAppsHostName;
					strItmsAppsHostName.Replace(TEXT("amp;"),TEXT(""));
					//发送汇报xp事件通知包
					strCookieData = getSendCookie();
					res = m_ViewPage.reLoginAppstoreXpReport(m_itunesstoredUserAgent,
															TEXT("https://xp.apple.com"),
															strXPVerifyHostName,
															strDsid,
															strCookieData,
															strX_Apple_I_md_m.GetBuffer(),
															strX_Apple_I_md.GetBuffer(),
															g_Proxy,
															g_netType);
					//为免费项目保存密码
					buyEventType = SAVE_PWD_FOR_FREE_APP;
				}break;
			case AGREE_BUY_VERIFY:
				{
					//生成I_md_m 和I_md的值
					GenerateXApple_I_MDandMDMValue();
					strI_md_m = getX_Apple_I_md_m();
					strI_md = getX_Apple_I_md();
					strX_Apple_I_md_m = A2T((char*)strI_md_m.c_str());
					strX_Apple_I_md = A2T((char*)strI_md.c_str());

					strXPVerifyHostName = m_strXPVerifyHostName;
					strXPVerifyHostName.Replace(TEXT("amp;"),TEXT(""));
					strItmsAppsHostName = m_strItmsAppsHostName;
					strItmsAppsHostName.Replace(TEXT("amp;"),TEXT(""));
					strItmsAppsHostName += TEXT("&");
					strItmsAppsHostName += strGuid;
					//发送汇报需要验证xp事件通知包
					strCookieData = getSendCookie();
					res = m_ViewPage.reLoginAppstoreXpReport(m_itunesstoredUserAgent,
															TEXT("https://xp.apple.com"),
															strXPVerifyHostName,
															strDsid,
															strCookieData,
															strX_Apple_I_md_m.GetBuffer(),
															strX_Apple_I_md.GetBuffer(),
															g_Proxy,
															g_netType);
					if (!res)
					{
						status = STATUS_FAILED;
						goto EXIT1;
					}
					//发送验证继续页面数据包
					strCookieData = getSendCookie();
					res = m_ViewPage.getSecurityVerifyWebReport(m_userAgent,
																TEXT("https://buy.itunes.apple.com"),
															   strItmsAppsHostName,
															   strDsid,
															   uzSecurityVerifyAction,
															   uzSubmitKey,
															   securityQuestionData,
															   strCookieData,
															   g_Proxy,
															   g_netType);
					if (!res)
					{
						status = STATUS_FAILED;
						goto EXIT1;
					}
					//这里需要验证页面信息的账号，已经不能再用了，账号作废
					status = ACCOUNT_FAULT;//账号信息不全
					goto EXIT1;
				}break;
			case SUBMIT_PAID_BUY_VERIFY:
				{
					//生成I_md_m 和I_md的值
					GenerateXApple_I_MDandMDMValue();
					strI_md_m = getX_Apple_I_md_m();
					strI_md = getX_Apple_I_md();
					strX_Apple_I_md_m = A2T((char*)strI_md_m.c_str());
					strX_Apple_I_md = A2T((char*)strI_md.c_str());

					strXPVerifyHostName = m_strXPVerifyHostName;
					strXPVerifyHostName.Replace(TEXT("amp;"),TEXT(""));
					strItmsAppsHostName = m_strItmsAppsHostName;
					strItmsAppsHostName.Replace(TEXT("amp;"),TEXT(""));
					strItmsAppsHostName += TEXT("&");
					strItmsAppsHostName += strGuid;
					//发送汇报需要验证xp事件通知包
					strCookieData = getSendCookie();
					res = m_ViewPage.reLoginAppstoreXpReport(m_itunesstoredUserAgent,
															TEXT("https://xp.apple.com"),
															strXPVerifyHostName,
															strDsid,
															strCookieData,
															strX_Apple_I_md_m.GetBuffer(),
															strX_Apple_I_md.GetBuffer(),
															g_Proxy,
															g_netType);

					//发送获取安全验证页面数据包
					strCookieData = getSendCookie();
					res = m_ViewPage.getSecurityVerifyWebReport(m_userAgent,
																TEXT("https://buy.itunes.apple.com"),
															   strItmsAppsHostName,
															   strDsid,
															   uzSecurityVerifyAction,
															   uzSubmitKey,
															   securityQuestionData,
															   strCookieData,
															   g_Proxy,
															   g_netType);
					if (!res)
					{
						m_log.xhLogPrintf(GETSECURITYVERIFYWEBREPORT_FAILED,__LINE__,__FUNCTION__,__FILE__);
						status = STATUS_FAILED;
						goto EXIT1;
					}

					strRefererUrl = TEXT("https://buy.itunes.apple.com");
					strRefererUrl += strItmsAppsHostName;

					//发送事件包
					strCookieData = getSendCookie();
					res = m_ViewPage.secureTermsPageReport(m_userAgent,
														   TEXT("https://xp.apple.com"),
														   strRefererUrl,
														   strDsid,
														   strCookieData,
														   g_Proxy,
														   g_netType);

					//匹配安全问题答案
					for (QUESTIONTOANSWER::iterator ptr = m_secQuesAnswerMap.begin(); ptr!=m_secQuesAnswerMap.end(); ptr++)
					{
						for (SECQUESTIONTONAME::iterator pos = securityQuestionData.begin(); pos!=securityQuestionData.end(); pos++)
						{
							if (ptr->first.Compare(pos->first) == 0)
							{
								MATCHSECINFO tmpSecInfo = {0};

								nameToAnswer[pos->second] = ptr->second;

								wcscpy(tmpSecInfo.strSecName,pos->second.GetBuffer());
								wcscpy(tmpSecInfo.strSecAnswer,ptr->second.GetBuffer());
								m_vecMatchSecInfo.push_back(tmpSecInfo);				
							}
						}
					}

					//发送安全回答问题数据包
					strCookieData = getSendCookie();
					res = m_ViewPage.securityAnswerReport(m_userAgent,
														 TEXT("https://buy.itunes.apple.com"),
														  uzSecurityVerifyAction,
														  strRefererUrl,
														  strDsid,
														  uzSubmitKey,
														  &nameToAnswer,
														  szXAppleMdData,
														  strCookieData,
														  g_Proxy,
														  g_netType);
					if (!res)
					{
						m_log.xhLogPrintf(SECURITYANSWERREPORT_FAILED,__LINE__,__FUNCTION__,__FILE__);
						status = STATUS_FAILED;
						goto EXIT1;
					}

					//调用接口生成clientData的值
					::EnterCriticalSection(&g_cs);
					res = pGenerateClientData(m_pGsaService,szXAppleMdData,strlen(szXAppleMdData),lpDsid,szClientData,envContext);

					//计算kbsync的值
					res = pKbsyncValue(m_pGsaService,szKbsyncValue,(char*)lpDsid,Dsid,true);
					::LeaveCriticalSection(&g_cs);
					
					//组装finishMachineProvisioning包需要发送的数据
					m_ViewPage.getFinishMachineProvisioningData(szSendData,
																szClientData,
																(char*)lpDsid,
																(char*)strUdid.c_str(),
																szKbsyncValue);


					sendLength = strlen(szSendData);
					::EnterCriticalSection(&g_cs);
					//生成x-apple-md-m和x-apple-md的值
					res = pGenerateXAppleAMDValue(m_pGsaService,(char*)lpDsid,szX_Apple_amd_m,szX_Apple_amd);
					if (szX_Apple_amd_m[0] != '\0')
					{
						setX_Apple_amd_m(szX_Apple_amd_m);
						setX_Apple_amd(szX_Apple_amd);
						MultiByteToWideChar(CP_ACP,0,szX_Apple_amd_m,strlen(szX_Apple_amd_m),X_Apple_amd_m,512);
						MultiByteToWideChar(CP_ACP,0,szX_Apple_amd,strlen(szX_Apple_amd),X_Apple_amd,MAX_PATH);
					}
					else
					{
						MultiByteToWideChar(CP_ACP,0,strXApple_amd_m.c_str(),strXApple_amd_m.length(),X_Apple_amd_m,512);
						MultiByteToWideChar(CP_ACP,0,strXApple_amd.c_str(),strXApple_amd.length(),X_Apple_amd,MAX_PATH);
					}
					::LeaveCriticalSection(&g_cs);

					//发送finishMachineProvisioning包
					strCookieData = getSendCookie();
					res = m_ViewPage.ProvisioningReport(m_userAgent,
														   TEXT("https://buy.itunes.apple.com"),
														   TEXT("/WebObjects/MZFinance.woa/wa/finishMachineProvisioning"),
															strPwdToken,
															strDsid,
															strCookieData,
															strX_Apple_I_md_m.GetBuffer(),
															strX_Apple_I_md.GetBuffer(),
															X_Apple_amd_m,
															X_Apple_amd,
															szSendData,
															sendLength,
															szSettingInfo,
															szTransportKey,
															g_Proxy,
															g_netType);
					if (!res)
					{
						m_log.xhLogPrintf(PROVISIONINGREPORT_FAILED,__LINE__,__FUNCTION__,__FILE__);
						status = STATUS_FAILED;
						goto EXIT1;
					}
					::EnterCriticalSection(&g_cs);
					//设置用于计算X-Apple-MD-M和X-Apple-MD值的SettingInfo和transportKey的值
					if (szSettingInfo[0] != '\0')
					{
						settingInfoLen = strlen(szSettingInfo);
						transportkeyLen = strlen(szTransportKey);
						res = pSetFinishProvisionData(m_pGsaService,
													szTransportKey,
													transportkeyLen,
													szSettingInfo,
													settingInfoLen,
													envContext,
													FALSE);
					}
					::LeaveCriticalSection(&g_cs);

					//发送xp事件包
					strRefererUrl = TEXT("https://buy.itunes.apple.com");
					strRefererUrl += uzSecurityVerifyAction;
					strCookieData = getSendCookie();
					res = m_ViewPage.secureTermsPageReport(m_userAgent,
														   TEXT("https://xp.apple.com"),
														   strRefererUrl,
														   strDsid,
														   strCookieData,
														   g_Proxy,
														   g_netType);

					buyEventType = SUBMIT_PAID_BUY_VERIFY;

				}break;
			case SP_MD_AUTH_ERROR:
				{
					::EnterCriticalSection(&g_cs);
					//根据从服务器来的x-apple-md-data值，计算clientData的值
					res = pGenerateClientData(m_pGsaService,szXAppleMdData,strlen(szXAppleMdData),lpDsid,szClientData,envContext);
					::LeaveCriticalSection(&g_cs);

					//分配内存
					pSendData = new char[ALLOCATE_SIZE];
					if (!pSendData)
					{
						m_log.xhLogPrintf(ALLOCATE_MEM_FAILED,__LINE__,__FUNCTION__,__FILE__);
						status = STATUS_FAILED;
						goto EXIT1;
					}
					::EnterCriticalSection(&g_cs);
					//计算kbsync的值
					res = pKbsyncValue(m_pGsaService,szKbsyncValue,(char*)lpDsid,Dsid,true);
					::LeaveCriticalSection(&g_cs);
					//得到发送的数据
					memset(pSendData,0,ALLOCATE_SIZE);
					m_ViewPage.getFinishMachineProvisioningData(pSendData,szClientData,(char*)lpDsid,(char*)strUdid.c_str(),szKbsyncValue);

					::EnterCriticalSection(&g_cs);
					//生成x-apple-amd-m和x-apple-amd的值
					res = pGenerateXAppleAMDValue(m_pGsaService,(char*)lpDsid,szX_Apple_amd_m,szX_Apple_amd);
					if (szX_Apple_amd_m[0] != '\0')
					{
						setX_Apple_amd_m(szX_Apple_amd_m);
						setX_Apple_amd(szX_Apple_amd);
						MultiByteToWideChar(CP_ACP,0,szX_Apple_amd_m,strlen(szX_Apple_amd_m),X_Apple_amd_m,512);
						MultiByteToWideChar(CP_ACP,0,szX_Apple_amd,strlen(szX_Apple_amd),X_Apple_amd,MAX_PATH);
					}
					else
					{
						MultiByteToWideChar(CP_ACP,0,strXApple_amd_m.c_str(),strXApple_amd_m.length(),X_Apple_amd_m,512);
						MultiByteToWideChar(CP_ACP,0,strXApple_amd.c_str(),strXApple_amd.length(),X_Apple_amd,MAX_PATH);
					}
					::LeaveCriticalSection(&g_cs);


					//发送校验数据
					strCookieData = getSendCookie();
					m_ViewPage.ProvisioningReport(m_userAgent,
													TEXT("https://p35-buy.itunes.apple.com"),
														TEXT("/WebObjects/MZFinance.woa/wa/finishMachineProvisioning"),
														strPwdToken,
														strDsid,
														strCookieData,
														strX_Apple_I_md_m.GetBuffer(),
														strX_Apple_I_md.GetBuffer(),
														X_Apple_amd_m,
														X_Apple_amd,
														pSendData,
														strlen(pSendData),
														szSettingInfo,
														szTransportKey,
														g_Proxy,
														g_netType);

					if (szSettingInfo[0] == '\0')
					{
						m_log.xhLogPrintf(SETTINGINFOVALUEIS_NULL,__LINE__,__FUNCTION__,__FILE__);
						if (pSendData)
						{
							delete []pSendData;
							pSendData = NULL;
						}
						status = STATUS_FAILED;
						goto EXIT1;
					}

					//设置用于计算X-Apple-MD-M和X-Apple-MD值的SettingInfo和transportKey的值
					settingInfoLen = strlen(szSettingInfo);
					transportkeyLen = strlen(szTransportKey);
					res = pSetFinishProvisionData(m_pGsaService,
													szTransportKey,
													transportkeyLen,
													szSettingInfo,
													settingInfoLen,
													envContext,
													FALSE);

					if (pSendData)
					{
						delete []pSendData;
						pSendData = NULL;
					}

				}break;
			case AMD_BUYPRODUCT_ERROR:
				{
					//生成I_md_m 和I_md的值
					GenerateXApple_I_MDandMDMValue();
					strI_md_m = getX_Apple_I_md_m();
					strI_md = getX_Apple_I_md();
					strX_Apple_I_md_m = A2T((char*)strI_md_m.c_str());
					strX_Apple_I_md = A2T((char*)strI_md.c_str());

					::EnterCriticalSection(&g_cs);
					//根据从服务器来的x-apple-md-data值，计算clientData的值
					res = pGenerateClientData(m_pGsaService,szXAppleAmdData,strlen(szXAppleAmdData),NULL,szClientData,envContext);
					::LeaveCriticalSection(&g_cs);

					//分配内存
					pSendData = new char[ALLOCATE_SIZE];
					if (!pSendData)
					{
						m_log.xhLogPrintf(ALLOCATE_MEM_FAILED,__LINE__,__FUNCTION__,__FILE__);
						status = STATUS_FAILED;
						goto EXIT1;
					}
					::EnterCriticalSection(&g_cs);
					//计算kbsync的值
					res = pKbsyncValue(m_pGsaService,szKbsyncValue,(char*)lpDsid,Dsid,false);
					::LeaveCriticalSection(&g_cs);
					//得到发送的数据
					memset(pSendData,0,ALLOCATE_SIZE);
					m_ViewPage.getFinishMachineProvisioningData(pSendData,szClientData,(char*)lpDsid,(char*)strUdid.c_str(),szKbsyncValue);

					::EnterCriticalSection(&g_cs);
					//生成x-apple-amd-m和x-apple-amd的值
					res = pGenerateXAppleAMDValue(m_pGsaService,(char*)lpDsid,szX_Apple_amd_m,szX_Apple_amd);
					if (szX_Apple_amd_m[0] != '\0')
					{
						setX_Apple_amd_m(szX_Apple_amd_m);
						setX_Apple_amd(szX_Apple_amd);
						MultiByteToWideChar(CP_ACP,0,szX_Apple_amd_m,strlen(szX_Apple_amd_m),X_Apple_amd_m,512);
						MultiByteToWideChar(CP_ACP,0,szX_Apple_amd,strlen(szX_Apple_amd),X_Apple_amd,MAX_PATH);
					}
					else
					{
						MultiByteToWideChar(CP_ACP,0,strXApple_amd_m.c_str(),strXApple_amd_m.length(),X_Apple_amd_m,512);
						MultiByteToWideChar(CP_ACP,0,strXApple_amd.c_str(),strXApple_amd.length(),X_Apple_amd,MAX_PATH);
					}
					::LeaveCriticalSection(&g_cs);


					//发送校验数据
					strCookieData = getSendCookie();
					m_ViewPage.ProvisioningReport(m_userAgent,
														TEXT("https://buy.itunes.apple.com"),
														TEXT("/WebObjects/MZFinance.woa/wa/finishMachineProvisioning"),
														NULL,
														NULL,
														strCookieData,
														strX_Apple_I_md_m.GetBuffer(),
														strX_Apple_I_md.GetBuffer(),
														X_Apple_amd_m,
														X_Apple_amd,
														pSendData,
														strlen(pSendData),
														szSettingInfo,
														szTransportKey,
														g_Proxy,
														g_netType);

					if (szSettingInfo[0] == '\0')
					{
						m_log.xhLogPrintf(SETTINGINFOVALUEIS_NULL,__LINE__,__FUNCTION__,__FILE__);
						if (pSendData)
						{
							delete []pSendData;
							pSendData = NULL;
						}
						status = STATUS_FAILED;
						goto EXIT1;
					}
					::EnterCriticalSection(&g_cs);
					//设置用于计算X-Apple-AMD-M和X-Apple-AMD值的SettingInfo和transportKey的值
					settingInfoLen = strlen(szSettingInfo);
					transportkeyLen = strlen(szTransportKey);
					res = pSetFinishProvisionData(m_pGsaService,
													szTransportKey,
													transportkeyLen,
													szSettingInfo,
													settingInfoLen,
													envContext,
													FALSE);

					if (pSendData)
					{
						delete []pSendData;
						pSendData = NULL;
					}
					::LeaveCriticalSection(&g_cs);

				}break;
			case CAPTCHA_REQUIRED:
				{
					//生成I_md_m 和I_md的值
					GenerateXApple_I_MDandMDMValue();
					strI_md_m = getX_Apple_I_md_m();
					strI_md = getX_Apple_I_md();
					strX_Apple_I_md_m = A2T((char*)strI_md_m.c_str());
					strX_Apple_I_md = A2T((char*)strI_md.c_str());

					strXPVerifyHostName = m_strXPVerifyHostName;
					strXPVerifyHostName.Replace(TEXT("amp;"),TEXT(""));
					strItmsAppsHostName = m_strItmsAppsHostName;
					strItmsAppsHostName.Replace(TEXT("amp;"),TEXT(""));
					//发送汇报xp事件通知包
					strCookieData = getSendCookie();
					res = m_ViewPage.reLoginAppstoreXpReport(m_itunesstoredUserAgent,
															TEXT("https://xp.apple.com"),
															strXPVerifyHostName,
															strDsid,
															strCookieData,
															strX_Apple_I_md_m.GetBuffer(),
															strX_Apple_I_md.GetBuffer(),
															g_Proxy,
															g_netType);

					//发送https://finance-app.itunes.apple.com/challenge?事件包
					res = m_CaptchaPage.SendFinanceAppChallengeReport(m_userAgent,
						                                           TEXT("https://finance-app.itunes.apple.com"),
																   strItmsAppsHostName,
																   strDsid,
																   strCookieData,
																   strX_Apple_I_md_m.GetBuffer(),
																   strX_Apple_I_md.GetBuffer(),
																   g_Proxy,
																   g_netType);

					strRefererUrl = TEXT("https://finance-app.itunes.apple.com");
					strRefererUrl += strItmsAppsHostName;


					buyEventType = CAPTCHA_REQUIRED;
				}break;
			case BUY_SUCCESS:
				{
					bResult = TRUE;
					status = BUY_SUCCESS;
				}break;
			case PRICE_MISMATCH:
				{
					status = PRICE_MISMATCH;
					goto EXIT1;
				}break;
			case ACCOUNT_STOP:
				{
					status = ACCOUNT_STOP;
					goto EXIT1;
				}break;
			case BUY_FAILED:
				{
					status = BUY_FAILED;
					goto EXIT1;
				}break;
			case UNKNOWN_ERROR:
				{
					status = UNKNOWN_ERROR;
					goto EXIT1;
				}break;
			case UNKNOWN_DEVICE:
				{
					status = UNKNOWN_DEVICE;
					goto EXIT1;
				}
			default:
				{
					status = buyRes;
					CStringA str;
					str.Format("buy app fail status = %d",status);
					m_log.xhLogPrintf(str.GetBuffer(),__LINE__,__FUNCTION__,__FILE__);
					goto EXIT1;
				}
				break;
			}

			//结果为真，表示购买成功,退出循环
			if (bResult)
			{
				break;
			}

		}//end while

		if (bResult)//购买成功，发送购买xp包
		{
			//获取购买确认数据包
			strCookieData = getSendCookie();
			m_ViewPage.xpBuyConfirmEvent(m_FormatUserAgent,
										(char*)m_version.c_str(),
										  m_width,
										  m_height,
										  szBuyConfirmEvent,
										  (char*)m_strAppUrl.c_str(),
										  (char*)m_strClientId.c_str(),
										  (char*)lpSaleId,
										  (char*)lpDsid,
										  szMzRequestId,
										  (char*)m_strPliIds.c_str(),
										  (char*)m_strxp_ab_value.c_str(),
										  (char*)m_strxp_abc_value.c_str());
			m_strSHA1 = GetStringSha1(szBuyConfirmEvent);
			strXpMainSignature = GetxpMainXAppleActionSignature(m_strSHA1);
			//发送购买确认数据包
			if (!m_ViewPage.xpReportTargetAppMain(m_itunesstoredUserAgent,szBuyConfirmEvent,(char*)lpPasswordToken,(char*)lpDsid,strCookieData,strXpMainSignature,strX_Apple_I_md_m.GetBuffer(),strX_Apple_I_md.GetBuffer(),g_Proxy,g_netType))
			{
				if (!m_ViewPage.xpReportTargetAppMain(m_itunesstoredUserAgent,szBuyConfirmEvent,(char*)lpPasswordToken,(char*)lpDsid,strCookieData,strXpMainSignature,strX_Apple_I_md_m.GetBuffer(),strX_Apple_I_md.GetBuffer(),g_Proxy,g_netType))
				{
					m_log.xhLogPrintf(XPREPORTTARGETAPPMAIN_FAILED,__LINE__,__FUNCTION__,__FILE__);
					status = STATUS_FAILED;
				}
			}
		}
EXIT1:
		if (pRecvSignBuf)
		{
			delete []pRecvSignBuf;
			pRecvSignBuf = NULL;
		}
	}
	catch(exception& e)
	{
		m_log.xhLogPrintf(EXECUTE_EXCEPTION,__LINE__,__FUNCTION__,__FILE__);
	}

	return status;
}

void CxhNetwork::getEventTime(char* lpEventtime)
{
	SYSTEMTIME sysTime;
	time_t unixTime;
	char eventtime[MAX_PATH] = {0};
	///Build xml format data...
	GetLocalTime(&sysTime);
	time(&unixTime);
	sprintf(eventtime,"%ld%d%ld",unixTime,sysTime.wMinute,sysTime.wMilliseconds);
	strcpy(lpEventtime,eventtime);

}

void CxhNetwork::generateMzRequestId(char* lpEventtime,char* lpMzRequestId)
{
	char strData[128] = {0};
	char strRequestId[MAX_PATH] = {0};
	char strRand[128]  = {0};

	__int64 dwEventTime = _atoi64(lpEventtime);

	errno_t err =  _i64toa_s(dwEventTime,strData,sizeof(strData),36);
	
	strupr(strData);

	strcat(strRequestId,m_strClientId.c_str());
	strcat(strRequestId,"z");
	strcat(strRequestId,strData);
	strcat(strRequestId,"z");
		
	//产生一个5位随机数
	srand((unsigned)time(NULL));
	int n = 5;
	int sum_5=0,w=n;
	while (n--)
	{
		int c=rand()%10;
		while(w==n&&!c)
		{
		  c=rand()%10;
		}//最高为不能为0
    
		sum_5 = sum_5*10;
		sum_5 += c;
	}

	err = _i64toa_s(sum_5,strRand,sizeof(strRand),36);
	strupr(strRand);
	strcat(strRequestId,strRand);

	m_strMtRequestId = strRequestId;	
	
	strcpy(lpMzRequestId,strRequestId);

	return;
}

void CxhNetwork::generateVid(char* lpVid)
{
	GUID guid;
	char strVid[MAX_PATH] = {0};
	CoInitialize(NULL);
	if(S_OK == CoCreateGuid(&guid))
	{
		sprintf_s(
			 strVid,
			 "%08X-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X",
			 guid.Data1, guid.Data2, guid.Data3,
			 guid.Data4[0], guid.Data4[1],
			 guid.Data4[2], guid.Data4[3],
			 guid.Data4[4], guid.Data4[5],
			 guid.Data4[6], guid.Data4[7]);
	}
	CoUninitialize();

	strcpy(lpVid,strVid);
	return;
}

void CxhNetwork::generateUdid(char* lpUdid)
{
	char guidBuf[64] = {0};
	BOOL Result = pGetMachineGuid(m_pGsaService,guidBuf);

	//转换成小写字符串
	strlwr(guidBuf);
	//生成手机格式udid
	string strudid = guidBuf;
	string::iterator it;
	for (it =strudid.begin(); it != strudid.end(); ++it)
	{
		if ( *it == '.')
		{
			strudid.erase(it);
		}
	}
	
	memcpy(lpUdid,strudid.c_str(),strudid.length()-16);
	return;

}

int CxhNetwork::onlyBuyProduct(CString strBuyUrl,
							   CString strBuyHostName,
							   IN LPVOID lpPasswordToken,
							   IN LPVOID lpDsid,
							   IN LPVOID lpSaleId,
							   IN LPVOID lpAppPrice,
							   IN LPVOID lpEventtime,
							   IN LPVOID lpMzRequestId,
							   IN LPVOID lpVid,
							   TCHAR* lpX_Apple_I_md_m,
							   TCHAR* lpX_Apple_I_md,
							   TCHAR* lpAgreeKey,
							   TCHAR* lpSubmitKey,
							   TCHAR* lpTermsId,
							   TCHAR* lpWosid_lite,
							   BOOL bPaid,
							   int buyEventType,
							   char* lpXAppleMdData,
							   char* lpXAppleAmdData)
{
	URL_COMPONENTS uc;
	TCHAR scheme[MAX_PATH] = {0};
	TCHAR hostName[MAX_PATH] = {0};
	TCHAR userName[MAX_PATH] = {0};
	TCHAR password[MAX_PATH] = {0};
	TCHAR urlPath[MAX_PATH] = {0};
	TCHAR ExtraInfo[MAX_PATH] = {0};
	
	INTERNET_PORT nPort = 0;

	HINTERNET hSession = NULL;
	HINTERNET hConnect = NULL;
	HINTERNET hOpenReq = NULL;
	BOOL bAddRequestHeader = FALSE;
	BOOL bSendRequest = FALSE;
	BOOL bQueryInfo = FALSE;
	
	::ZeroMemory(&uc,sizeof(URL_COMPONENTS));
	uc.dwStructSize = sizeof(URL_COMPONENTS);
	uc.lpszScheme = scheme;
	uc.lpszHostName = hostName;
	uc.lpszUserName = userName;
	uc.lpszPassword = password;
	uc.lpszUrlPath = urlPath;
	uc.lpszExtraInfo = ExtraInfo;
	uc.nPort = nPort;

	uc.dwSchemeLength = MAX_PATH;
	uc.dwHostNameLength = MAX_PATH;
	uc.dwUserNameLength = MAX_PATH;
	uc.dwPasswordLength = MAX_PATH;
	uc.dwUrlPathLength = MAX_PATH;


	char szPiIds[64] = {0};
	char songId[64] = {0};
	char downloadId[128] = {0}; 
	CString downloadKey;
	CString downloadUrlPath;
	CString strHostName;

	char kbsyncValue[1024] = {0};
	char strXml[4096] = {0};

	char szX_Apple_md_m[MAX_PATH] = {0};
	char szX_Apple_md[128] = {0};

	char szX_Apple_amd_m[MAX_PATH] = {0};
	char szX_Apple_amd[128] = {0};
	
	TCHAR strdsid[64] = {0};
	TCHAR strPwdToken[128] = {0};

	TCHAR strX_Apple_amd_m[512] = {0};
	TCHAR strX_Apple_amd[MAX_PATH] = {0};

	TCHAR strX_Apple_md_m[512] = {0};
	TCHAR strX_Apple_md[MAX_PATH] = {0};

	CString strSaleId;
	CString strHeaders;
	CString tmpBuf;
	CString strRedirectUrl;
	CString strRedirectHostName;
	CString strClientAddress;
	CString strX_Apple_I_md_m;
	CString strX_Apple_I_md;
	CString strXpMainSignature;

	TCHAR location[512] = {0};
	DWORD locationLength = sizeof(location);

	LPGZIP lpgzipBuf = NULL;

	DWORD dwLength = 0;
	DWORD dwReadLength = 0;
	DWORD dwMaxDataLength = 200;
	BOOL bReadFile = FALSE;
	char DataBuffer[200] = {0};
	DWORD dwBytesWritten = 0;
	char* pDecodeData = NULL;
	char szAgreeKey[MAX_PATH] = {0};
	char szSubmitKey[64] = {0};
	char szTermsId[32] = {0};
	char szWosid_lite[MAX_PATH] = {0};
	char szAgreeValue[64] = {0};
	char szSubmitValue[64] = {0};
	char szSbsync[1024] = {0};
	char szSecName1[64] = {0};
	char szSecAnswer1[64] = {0};
	char szSecName2[64] = {0};
	char szSecAnswer2[64] = {0};
	char szProductContext[MAX_PATH] = {0};
	char strImpressionEvent[MAX_ALLOCATE_SIZE] = {0};
	char strPageDetailEvent[4096] = {0};
	char szBuyAuthSuccessEvent[4096] = {0};
	string strNativeSearchLockup = "";
	TCHAR uzAgreeValue[] = TEXT("同意");
	TCHAR uzSubmitValue[] = TEXT("提交");

	char szFormatTime[128] = {0};
	TCHAR wzFormatTime[MAX_PATH] = {0};

	int bResult = 0;
	int sbsyncLength = 0;
	int buyRes = 0;

	//计算Kbsync的值
    LONGLONG Dsid = _atoi64((char*)lpDsid);
	BOOL Res = pKbsyncValue(m_pGsaService,kbsyncValue,(char*)lpDsid,Dsid,false);
	if (!Res)
	{
		return BUY_FAILED;
	}

	unsigned int saleId = atoi((char*)lpSaleId);

	//计算amd-m和amd的值
	/*Res = pGenerateXAppleAMDValue(m_pGsaService,(char*)lpDsid,szX_Apple_amd_m,szX_Apple_amd);
	if (!Res)
	{
		return BUY_FAILED;
	}
	if (szX_Apple_amd_m[0] != '\0')
	{
		m_strX_Apple_amd_m = szX_Apple_amd_m;
		m_strX_Apple_amd = szX_Apple_amd;
	}*/

	//计算x-apple-md-m和x-apple-md的值
	Res = /*pGenerateXAppleMDValue*/pGenerateXAppleAMDValue(m_pGsaService,(char*)lpDsid,szX_Apple_md_m,szX_Apple_md);
	if (!Res)
	{
		return BUY_FAILED;
	}

	//计算sbsync的值
	Res = pGenerateSbsyncValue(m_pGsaService,lpDsid,(char*)m_strX_Apple_amd_m.c_str(),m_strX_Apple_amd_m.length(),szSbsync,sbsyncLength);
	if (!Res)
	{
		return BUY_FAILED;
	}

	MultiByteToWideChar(CP_ACP,0,m_strX_Apple_amd_m.c_str(),m_strX_Apple_amd_m.length(),strX_Apple_amd_m,512);
	MultiByteToWideChar(CP_ACP,0,m_strX_Apple_amd.c_str(),m_strX_Apple_amd.length(),strX_Apple_amd,MAX_PATH);

	MultiByteToWideChar(CP_ACP,0,szX_Apple_md_m,strlen(szX_Apple_md_m),strX_Apple_md_m,512);
	MultiByteToWideChar(CP_ACP,0,szX_Apple_md,strlen(szX_Apple_md),strX_Apple_md,MAX_PATH);


	CString _strAgent = m_userAgent;
	CString _verb = _T("POST");
	CString strSendCookie;

	BOOL bCrackUrl = InternetCrackUrl(strBuyUrl,0,0,&uc);
	if (bCrackUrl)
	{
		if (g_netType == PROXY_NETWORK)
		{
			hSession = InternetOpen(_strAgent,INTERNET_OPEN_TYPE_PROXY,g_Proxy,0,0);
		}
		else
		{
			hSession = InternetOpen(_strAgent,0,0,0,0);
		}
		if (!hSession)
		{
			return BUY_FAILED;
		}

		hConnect = InternetConnect(hSession,uc.lpszHostName,uc.nPort,uc.lpszUserName,uc.lpszPassword,3,0,0);
		if (!hConnect)
		{
			InternetCloseHandle(hSession);
			return BUY_FAILED;
		}

		hOpenReq = HttpOpenRequest(hConnect,_verb,strBuyHostName,0,0,0,0x84801000|INTERNET_FLAG_NO_COOKIES,0);//0x84A01000是一个常数
		if (!hOpenReq)
		{
			InternetCloseHandle(hConnect);
			InternetCloseHandle(hSession);
			return BUY_FAILED;
		}
		
		MultiByteToWideChar(CP_ACP,NULL,(char*)lpPasswordToken,-1,strPwdToken,128);
		MultiByteToWideChar(CP_ACP,NULL,(LPCSTR)lpDsid,strlen((char*)lpDsid),strdsid,64);

		CViewPage::getFormatTime(szFormatTime);
		MultiByteToWideChar(CP_ACP,0,szFormatTime,strlen(szFormatTime),wzFormatTime,MAX_PATH);
		
		strSaleId.Format(_T("%d"),atoi((char*)lpSaleId));

		strClientAddress = GetClientAddress();
	
		strHeaders = TEXT("\r\nUser-Agent: ");
		strHeaders += _strAgent;
		strHeaders += TEXT("\r\nAccept: */*");
		strHeaders += TEXT("\r\nAccept-Encoding: gzip, deflate");
		strHeaders += TEXT("\r\nAccept-Language: zh-Hans\r\nX-Apple-Store-Front: 143465-19,29\r\nX-Token: ");
		strHeaders.Append(strPwdToken);
		strHeaders += TEXT("\r\nX-Dsid: ");
		strHeaders += strdsid;
		strHeaders += TEXT("\r\nX-Apple-Connection-Type: WiFi");
		strHeaders += TEXT("\r\nX-Apple-Client-Application: Software");
		strHeaders += TEXT("\r\nContent-Type: application/x-apple-plist");

		if (lpX_Apple_I_md_m[0] != TEXT('\0'))
		{
			strHeaders += TEXT("\r\nX-Apple-I-MD-RINFO: 17106176");
			strHeaders += _T("\r\nX-Apple-I-MD-M: ");
			strHeaders += lpX_Apple_I_md_m;
			strHeaders += TEXT("\r\nX-Apple-I-MD: ");
			strHeaders += lpX_Apple_I_md;
		}
		if (strX_Apple_amd_m[0] != TEXT('\0'))
		{
			strHeaders += TEXT("\r\nX-Apple-AMD-M: ");
			strHeaders += strX_Apple_amd_m;
			strHeaders += TEXT("\r\n\r\nX-Apple-AMD: ");
			strHeaders += strX_Apple_amd;
		}
		
		if (strX_Apple_md_m[0] != TEXT('\0'))
		{
			strHeaders += TEXT("\r\nX-Apple-MD-M: ");
			strHeaders += strX_Apple_md_m;
			strHeaders += TEXT("\r\n\r\nX-Apple-MD: ");
			strHeaders += strX_Apple_md;
		}
		else
		{
			strHeaders += TEXT("\r\nX-Apple-MD-M: ");
			strHeaders += strX_Apple_amd_m;
			strHeaders += TEXT("\r\n\r\nX-Apple-MD: ");
			strHeaders += strX_Apple_amd;

		}


		//设置cookie值
		strSendCookie = getSendCookie();
		if (!strSendCookie.IsEmpty())
		{
			strHeaders += TEXT("\r\nCookie: ");
			strHeaders += strSendCookie;
		}
		strHeaders += TEXT("\r\nX-Apple-Client-Address: ");
		strHeaders += strClientAddress;
		strHeaders += TEXT("\r\nX-Apple-TA-Device: ");
		strHeaders += m_strModel;
		strHeaders += TEXT("\r\nX-Apple-I-Client-Time: ");
		strHeaders += wzFormatTime;
		strHeaders += TEXT("\r\nX-Apple-Partner: origin.0");
		strHeaders += TEXT("\r\nX-Apple-Tz: 28800");
		strHeaders += TEXT("\r\nConnection: keep-alive");
		strHeaders += TEXT("\r\nProxy-Connection: keep-alive\r\n\r\n");

		DWORD dwHeaderLength = strHeaders.GetLength();
		bAddRequestHeader = HttpAddRequestHeaders(hOpenReq,strHeaders,dwHeaderLength,0xA0000000);//0xA0000000是一个常量
		if (!bAddRequestHeader)
		{
			InternetCloseHandle(hOpenReq);
			InternetCloseHandle(hConnect);
			InternetCloseHandle(hSession);
			return BUY_FAILED;
		}

		if (lpAgreeKey[0] != TEXT('\0'))
		{
			WideCharToMultiByte(CP_ACP,0,lpAgreeKey,-1,szAgreeKey,MAX_PATH,NULL,NULL);
			WideCharToMultiByte(CP_UTF8,0,uzAgreeValue,-1,szAgreeValue,64,NULL,NULL);

			WideCharToMultiByte(CP_ACP,0,lpTermsId,-1,szTermsId,32,NULL,NULL);
			WideCharToMultiByte(CP_ACP,0,lpWosid_lite,-1,szWosid_lite,MAX_PATH,NULL,NULL);
		}

		if (lpSubmitKey[0] != TEXT('\0'))
		{
			WideCharToMultiByte(CP_ACP,0,lpSubmitKey,-1,szSubmitKey,64,NULL,NULL);
			WideCharToMultiByte(CP_UTF8,0,uzSubmitValue,-1,szSubmitValue,64,NULL,NULL);
			WideCharToMultiByte(CP_ACP,0,m_vecMatchSecInfo.at(0).strSecName,-1,szSecName1,64,NULL,NULL);
			WideCharToMultiByte(CP_UTF8,0,m_vecMatchSecInfo.at(0).strSecAnswer,-1,szSecAnswer1,64,NULL,NULL);
			WideCharToMultiByte(CP_ACP,0,m_vecMatchSecInfo.at(1).strSecName,-1,szSecName2,64,NULL,NULL);
			WideCharToMultiByte(CP_UTF8,0,m_vecMatchSecInfo.at(1).strSecAnswer,-1,szSecAnswer2,64,NULL,NULL);
		}
		
		//获取发送的数据
		m_ViewPage.buyProductPostData(strXml,
									  szAgreeKey,
									  szAgreeValue,
									  szSubmitKey,
									  szSubmitValue,
									  szSecName1,
									  szSecAnswer1,
									  szSecName2,
									  szSecAnswer2,
									  (char*)lpPasswordToken,
									  (char*)m_strAppExtVrsId.c_str(),
									  (char*)m_strCreditDisplay.c_str(),
									  (char*)lpAppPrice,
									  (char*)m_strUdid.c_str(),
									  kbsyncValue,
									  (char*)m_strClientId.c_str(),
									  (char*)lpEventtime,
									  (char*)lpSaleId,
									  (char*)lpMzRequestId,
									  (char*)m_strSearchKeyword.c_str(),
									  (char*)lpDsid,
									  szTermsId,
									  szSbsync,
									  (char*)lpVid,
									  szWosid_lite,
									  bPaid,
									  buyEventType);
		
		DWORD xmlLength = strlen(strXml);
		bSendRequest = HttpSendRequest(hOpenReq,strHeaders,-1,strXml,xmlLength);
		if (!bSendRequest && GetLastError() == ERROR_INTERNET_INVALID_CA)
		{
			DWORD dwFlags;
			DWORD dwBuffLen = sizeof(dwFlags);
			InternetQueryOption (hOpenReq, INTERNET_OPTION_SECURITY_FLAGS,(LPVOID)&dwFlags, &dwBuffLen);
			dwFlags |= SECURITY_FLAG_IGNORE_UNKNOWN_CA;
			InternetSetOption (hOpenReq,INTERNET_OPTION_SECURITY_FLAGS,&dwFlags,sizeof (dwFlags) );

			//再次发送
			bSendRequest = HttpSendRequest(hOpenReq,strHeaders,-1,strXml,xmlLength);
			if (!bSendRequest)
			{
				int error = GetLastError();//ERROR_INTERNET_SEC_CERT_REV_FAILED 
				InternetCloseHandle(hOpenReq);
				InternetCloseHandle(hConnect);
				InternetCloseHandle(hSession);
				return FALSE;
			}
		}

		DWORD dwQueryBuf = 200;
		DWORD dwQueryBufferLength = 4;
		bQueryInfo = HttpQueryInfo(hOpenReq,0x20000013,&dwQueryBuf,&dwQueryBufferLength,NULL);//HttpQueryInfo 获取文件大小后
		if (!bQueryInfo)
		{
			InternetCloseHandle(hOpenReq);
			InternetCloseHandle(hConnect);
			InternetCloseHandle(hSession);
			return BUY_FAILED;
		}

		//查询获取cookie,获取需要的值
		parseResponseCookie(hOpenReq);

		if (dwQueryBuf == 200)// get the size of file: 200
		{
			lpgzipBuf =  (LPGZIP)VirtualAlloc(NULL,1024*50,MEM_COMMIT | MEM_RESERVE,PAGE_EXECUTE_READWRITE);//分配50K的虚拟内存
			if (lpgzipBuf != NULL)
			{
				while (true)
				{
					bReadFile = InternetReadFile(hOpenReq,DataBuffer,dwMaxDataLength,&dwReadLength);
					if (bReadFile && dwReadLength != 0)
					{
						memcpy(lpgzipBuf + dwLength,DataBuffer,dwReadLength);
						
						dwLength += dwReadLength;
						
						dwReadLength = 0;
		
					}
					else
					{
						lpgzipBuf[dwLength] = '\0';
						break;
					}

				}//end while
	
			}

			CGZIP2A gzip2A(lpgzipBuf,dwLength);
			pDecodeData = gzip2A.psz;//解密出来的网页Json数据指针
			if (!pDecodeData)
			{
				if (lpgzipBuf)
				{
					VirtualFree(lpgzipBuf,0,MEM_RELEASE);
					lpgzipBuf = NULL;
				}

			}
			bResult = ExtractErrorInfo(pDecodeData);
			if (bResult)
			{
				if (SP_MD_AUTH_ERROR == bResult)
				{
					//获取x-apple-md-data的值			
					m_ViewPage.getXAppleMdDataValue(hOpenReq,lpXAppleMdData);
				}
				else if (AMD_BUYPRODUCT_ERROR == bResult)
				{
					m_ViewPage.getXAppleAMdDataValue(hOpenReq,lpXAppleAmdData);
				}
				else if (NEED_SET_KEYBAG == bResult)
				{
					//提取keybag的值
					ExtractKeybagData(pDecodeData);

					//调用设置keybag数据的接口函数
					pSetKeyBagDataEx(m_pGsaService,Dsid,m_strKeybag.GetBuffer(),m_strKeybag.GetLength());

					goto buySuccess;

				}
				InternetCloseHandle(hOpenReq);
				InternetCloseHandle(hConnect);
				InternetCloseHandle(hSession);

				if (lpgzipBuf)
				{
					VirtualFree(lpgzipBuf,0,MEM_RELEASE);
					lpgzipBuf = NULL;
				}

				return bResult;
			}

			//判断是否需要重新登录验证信息
			bResult = NeedVerifyData(pDecodeData);
			if ( bResult )
			{
				if (lpgzipBuf)
				{
					VirtualFree(lpgzipBuf,0,MEM_RELEASE);
					lpgzipBuf = NULL;
				}

				InternetCloseHandle(hOpenReq);
				InternetCloseHandle(hConnect);
				InternetCloseHandle(hSession);
				
				return bResult;

			}
buySuccess:
			//得到songId和download-Id的值
			if (!GetSongIdUrlPathdownloadId(pDecodeData,songId,downloadUrlPath,downloadKey,downloadId,szPiIds))
			{
				m_log.xhLogPrintf(GETSONGIDURLPATHDOWNLOADID_FAILED,__LINE__,__FUNCTION__,__FILE__);
				//释放内存
				if (lpgzipBuf)
				{
					VirtualFree(lpgzipBuf,0,MEM_RELEASE);
					lpgzipBuf = NULL;
				}

				InternetCloseHandle(hOpenReq);
				InternetCloseHandle(hConnect);
				InternetCloseHandle(hSession);

				return BUY_FAILED;

			}

			m_strPliIds = szPiIds;

			//释放内存
			if (lpgzipBuf)
			{
				VirtualFree(lpgzipBuf,0,MEM_RELEASE);
				lpgzipBuf = NULL;
			}
		}
		else if (dwQueryBuf == 307)//POST请求重定向，用location里的地址重新post请求连接
		{
			bQueryInfo = HttpQueryInfo(hOpenReq,HTTP_QUERY_LOCATION,location,&locationLength,0);
			if (!bQueryInfo)
			{
				InternetCloseHandle(hOpenReq);
				InternetCloseHandle(hConnect);
				InternetCloseHandle(hSession);
				return BUY_FAILED;
			}
			tmpBuf = location;
			int Index = tmpBuf.Find(_T(".com"));
			strRedirectUrl = tmpBuf.Mid(0,Index+4);
			strRedirectHostName = tmpBuf.Mid(Index+4);
			buyRes = onlyBuyProduct(strRedirectUrl,
									   strRedirectHostName,
									   lpPasswordToken,
									   lpDsid,
									   lpSaleId,
									   lpAppPrice,
									   lpEventtime,
									   lpMzRequestId,
									   lpVid,
									   lpX_Apple_I_md_m,
									   lpX_Apple_I_md,
									   lpAgreeKey,
									   lpSubmitKey,
									   lpTermsId,
									   lpWosid_lite,
									   bPaid,
									   buyEventType,
									   lpXAppleMdData,
									   lpXAppleAmdData);

			if (hOpenReq)
			{
				InternetCloseHandle(hOpenReq);
			}

			if (hConnect)
			{
				InternetCloseHandle(hConnect);
			}

			if (hSession)
			{
				InternetCloseHandle(hSession);
			}

			return buyRes;
		}
		else
		{
			InternetCloseHandle(hOpenReq);
			InternetCloseHandle(hConnect);
			InternetCloseHandle(hSession);
			return BUY_FAILED;
		}

		//发送xp购买通知
		m_ViewPage.xpAppBuyReport(this->m_itunesstoredUserAgent,(char*)lpSaleId,g_Proxy,g_netType);

		//生成I_md_m 和I_md的值
		USES_CONVERSION;
		GenerateXApple_I_MDandMDMValue();
		string strI_md_m = getX_Apple_I_md_m();
		string strI_md = getX_Apple_I_md();
		strX_Apple_I_md_m = A2T((char*)strI_md_m.c_str());
		strX_Apple_I_md = A2T((char*)strI_md.c_str());

		//获取购买成功权限xp包
		m_ViewPage.xpBuyAuthSuccessEvent(m_FormatUserAgent,
										(char*)m_version.c_str(),
										m_width,
										m_height,
										szBuyAuthSuccessEvent,
										(char*)m_strClientId.c_str(),
										(char*)lpSaleId,
										(char*)lpDsid,
										(char*)m_strxp_ab_value.c_str(),
										(char*)m_strxp_abc_value.c_str(),
										(char*)lpMzRequestId,
										(char*)m_strSearchKeyword.c_str(),
										(char*)m_strAppExtVrsId.c_str(),
										(char*)lpEventtime);

		m_strSHA1 = GetStringSha1(szBuyAuthSuccessEvent);
		CString strXpMainSignature = GetxpMainXAppleActionSignature(m_strSHA1);
		//发送购买成功权限xp包
		CString strCookieData = getSendCookie();
		if (!m_ViewPage.xpReportTargetAppMain(m_itunesstoredUserAgent,szBuyAuthSuccessEvent,(char*)lpPasswordToken,(char*)lpDsid,strCookieData,strXpMainSignature,strX_Apple_I_md_m.GetBuffer(),strX_Apple_I_md.GetBuffer(),g_Proxy,g_netType))
		{
			if (!m_ViewPage.xpReportTargetAppMain(m_itunesstoredUserAgent,szBuyAuthSuccessEvent,(char*)lpPasswordToken,(char*)lpDsid,strCookieData,strXpMainSignature,strX_Apple_I_md_m.GetBuffer(),strX_Apple_I_md.GetBuffer(),g_Proxy,g_netType))
			{
				m_log.xhLogPrintf(XPREPORTTARGETAPPMAIN_FAILED,__LINE__,__FUNCTION__,__FILE__);
			}
		}

		//下载文件
		wchar_t strDownloadUrl[] = _T("http://iosapps.itunes.apple.com");

		int pos = downloadUrlPath.Find(strDownloadUrl);
		strHostName = downloadUrlPath.Right(downloadUrlPath.GetLength()-pos-wcslen(strDownloadUrl));

		//打开真实下载
		downloadAppFile(strHostName,downloadKey,strDownloadUrl);//暂时注释掉实际下载

		//发送购买的Id到se.itunes.apple.com
		strSendCookie = getSendCookie();
		strNativeSearchLockup = "native-search-lockup=";
		vector<string> idsList = m_ViewPage.GetIdsList();
		//字符串从小到大排序
		sort(idsList.begin(),idsList.end());
		int size = idsList.size();
		for (int i=0; i<size; i++)
		{
			strNativeSearchLockup += idsList.at(i);
			if (i != size - 1)
			{
				strNativeSearchLockup += "%2C";
			}
		}
		m_ViewPage.sendBuyButtonMetaData(m_userAgent,
										(char*)strNativeSearchLockup.c_str(),
										(char*)lpDsid,
										(char*)lpPasswordToken,
										strSendCookie,
										lpX_Apple_I_md_m,
										lpX_Apple_I_md,
										g_Proxy,
										g_netType);
		string strShowPageIdsData = m_ViewPage.getShowPageIdsData();
		m_ViewPage.sendBuyButtonMetaData(m_userAgent,
										(char*)strShowPageIdsData.c_str(),
										(char*)lpDsid,
										(char*)lpPasswordToken,
										strSendCookie,
										lpX_Apple_I_md_m,
										lpX_Apple_I_md,
										g_Proxy,
										g_netType);

		sprintf(szProductContext,"product-dv=%s&native-search-lockup=%s",lpSaleId,lpSaleId);
		m_ViewPage.sendBuyButtonMetaData(m_userAgent,
										szProductContext,
										(char*)lpDsid,
										(char*)lpPasswordToken,
										strSendCookie,
										lpX_Apple_I_md_m,
										lpX_Apple_I_md,
										g_Proxy,
										g_netType);

		//发送xp注册包
		SendxpRegister((char*)lpDsid,m_userAgent);

		///发送下载成功通知
		CString strRefUrl = TEXT("/cn/app//id");
		strRefUrl += strSaleId;
		strRefUrl += TEXT("?mt=8");
		CString strUrl = _T("https://p36-buy.itunes.apple.com");
		CString strArg = _T("/WebObjects/MZFastFinance.woa/wa/songDownloadDone?Pod=36");
		if (!SendDownloadDoneSuccessNotify(strUrl,strArg,songId,downloadId,strPwdToken,strdsid,strSaleId,strRefUrl,lpX_Apple_I_md_m,lpX_Apple_I_md))
		{
			InternetCloseHandle(hOpenReq);
			InternetCloseHandle(hConnect);
			InternetCloseHandle(hSession);
			return BUY_FAILED;
		}
		

	}
	else
	{
		return BUY_FAILED;
	}

	if (hOpenReq)
	{
		InternetCloseHandle(hOpenReq);
	}

	if (hConnect)
	{
		InternetCloseHandle(hConnect);
	}

	if (hSession)
	{
		InternetCloseHandle(hSession);
	}

	return BUY_SUCCESS;
}

BOOL CxhNetwork::xpReportAppBuy(char* lpSaleId,char* lpDsid)
{
	URL_COMPONENTS uc;
	TCHAR scheme[MAX_PATH] = {0};
	TCHAR hostName[MAX_PATH] = {0};
	TCHAR userName[MAX_PATH] = {0};
	TCHAR password[MAX_PATH] = {0};
	TCHAR urlPath[MAX_PATH] = {0};
	TCHAR ExtraInfo[MAX_PATH] = {0};
	INTERNET_PORT nPort = 0;

	HINTERNET hSession = NULL;
	HINTERNET hConnect = NULL;
	HINTERNET hOpenReq = NULL;
	BOOL bAddRequestHeader = FALSE;
	BOOL bSendRequest = FALSE;
	BOOL bQueryInfo = FALSE;
	
	::ZeroMemory(&uc,sizeof(URL_COMPONENTS));
	uc.dwStructSize = sizeof(URL_COMPONENTS);
	uc.lpszScheme = scheme;
	uc.lpszHostName = hostName;
	uc.lpszUserName = userName;
	uc.lpszPassword = password;
	uc.lpszUrlPath = urlPath;
	uc.lpszExtraInfo = ExtraInfo;
	uc.nPort = nPort;

	uc.dwSchemeLength = MAX_PATH;
	uc.dwHostNameLength = MAX_PATH;
	uc.dwUserNameLength = MAX_PATH;
	uc.dwPasswordLength = MAX_PATH;
	uc.dwUrlPathLength = MAX_PATH;
	
	CString strUrl = _T("https://xp.apple.com");
	CString strAgent = m_itunesstoredUserAgent;

	TCHAR strDsid[64] = {0};
	TCHAR strSaleId[64] = {0};
	TCHAR strHostName[MAX_PATH] = {0};

	MultiByteToWideChar(CP_ACP,0,(LPCSTR)lpDsid,strlen(lpDsid),strDsid,64);
	MultiByteToWideChar(CP_ACP,0,(LPCSTR)lpSaleId,strlen(lpSaleId),strSaleId,64);

	wsprintf(strHostName,TEXT("/report/2/xp_app_buy?clientId=0&sf=143465&adamId=%s"),strSaleId);

	BOOL bCrackUrl = ::InternetCrackUrl(strUrl,0,0,&uc);
	if (bCrackUrl)
	{
		if (g_netType == 3)
		{
			hSession = InternetOpen(strAgent,INTERNET_OPEN_TYPE_PROXY,g_Proxy,0,0);
		}
		else
		{
			hSession = InternetOpen(strAgent,0,0,0,0);
		}
		if (!hSession)
		{
			return FALSE;
		}
		hConnect = InternetConnect(hSession,uc.lpszHostName,uc.nPort,uc.lpszUserName,uc.lpszPassword,3,0,0);
		if (!hConnect)
		{
			InternetCloseHandle(hSession);
			return FALSE;
		}

		hOpenReq = HttpOpenRequest(hConnect,_T("GET"),strHostName,0,0,0,0x84801000,0);//0x84A01000是一个常数,注意
		if (!hOpenReq)
		{
			InternetCloseHandle(hConnect);
			InternetCloseHandle(hSession);
			return FALSE;
		}
		CString strHeaders = _T("User-Agent: ");
		strHeaders += strAgent;
		strHeaders += TEXT("\r\nX-Dsid: ");
		strHeaders += strDsid;
		strHeaders += TEXT("\r\nAccept-Language: zh-cn");
		strHeaders += TEXT("\r\nX-Apple-Connection-Type: WiFi");
		strHeaders += TEXT("\r\nX-Apple-Store-Front: 143465-19,29");
		strHeaders += TEXT("\r\nX-Apple-Partner: origin.0");
		strHeaders += _T("\r\nX-Apple-I-Client-Time: ");
		strHeaders += m_ViewPage.GetClientTime();
		strHeaders += _T("\r\nX-Apple-Tz: 28800");
		strHeaders += TEXT("\r\nConnection: keep-alive");
		strHeaders += TEXT("\r\nProxy-Connection: keep-alive\r\n\r\n");

		DWORD dwHeaderLength = wcslen(strHeaders);

		bAddRequestHeader = HttpAddRequestHeaders(hOpenReq,strHeaders,dwHeaderLength,0xA0000000);//0xA0000000是一个常量
		if (!bAddRequestHeader)
		{
			InternetCloseHandle(hOpenReq);
			InternetCloseHandle(hConnect);
			InternetCloseHandle(hSession);
			return FALSE;
		}

		bSendRequest = HttpSendRequestW(hOpenReq,NULL,-1,NULL,0);//第三个参数设置为负一
		if (!bSendRequest && GetLastError() == ERROR_INTERNET_INVALID_CA)
		{
			DWORD dwFlags;
			DWORD dwBuffLen = sizeof(dwFlags);
			InternetQueryOption (hOpenReq, INTERNET_OPTION_SECURITY_FLAGS,(LPVOID)&dwFlags, &dwBuffLen);
			dwFlags |= SECURITY_FLAG_IGNORE_UNKNOWN_CA;
			InternetSetOption (hOpenReq,INTERNET_OPTION_SECURITY_FLAGS,&dwFlags,sizeof (dwFlags) );

			//再次发送
			bSendRequest = HttpSendRequest(hOpenReq,NULL,-1,NULL,0);
			if (!bSendRequest)
			{
				int error = GetLastError();//ERROR_INTERNET_SEC_CERT_REV_FAILED 
				InternetCloseHandle(hOpenReq);
				InternetCloseHandle(hConnect);
				InternetCloseHandle(hSession);
				return FALSE;
			}
		}

		DWORD dwQueryBuf = 200;
		DWORD dwQueryBufferLength = 4;

		BOOL bQueryInfo = HttpQueryInfo(hOpenReq,0x20000013,&dwQueryBuf,&dwQueryBufferLength,NULL);//HttpQueryInfo 获取文件大小后
		if (!bQueryInfo)
		{
			InternetCloseHandle(hOpenReq);
			InternetCloseHandle(hConnect);
			InternetCloseHandle(hSession);
			return FALSE;
		}
		if (dwQueryBuf != 200)
		{
			InternetCloseHandle(hOpenReq);
			InternetCloseHandle(hConnect);
			InternetCloseHandle(hSession);
			return FALSE;
		}

	}
	else
	{
		return FALSE;
	}

	if (hOpenReq)
	{
		InternetCloseHandle(hOpenReq);
	}

	if (hConnect)
	{
		InternetCloseHandle(hConnect);
	}

	if (hSession)
	{
		InternetCloseHandle(hSession);
	}

	return TRUE;
}

BOOL CxhNetwork::ResponseServer(CString strUrl,CString strVerb,
					CString strHostName,
					TCHAR* lpX_Apple_I_md_m,
					TCHAR* lpX_Apple_I_md,
					char* lpSignSapsetupBuffer,
					char* lpOutSignSapSetupBuffer,
					int& outSignDataLen)
{
	char SendData[2048] = {0};
	DWORD sendLength = 0;
	DWORD dwSize = 0;
	BOOL bResult = FALSE;
	char* lpRecvBuf = NULL;
	char firstSignKey[] = "<data>";
	char lastSignKey[] = "</data>";

	URL_COMPONENTS uc;
	TCHAR scheme[MAX_PATH] = {0};
	TCHAR hostName[MAX_PATH] = {0};
	TCHAR userName[MAX_PATH] = {0};
	TCHAR password[MAX_PATH] = {0};
	TCHAR urlPath[MAX_PATH] = {0};
	TCHAR ExtraInfo[MAX_PATH] = {0};
	INTERNET_PORT nPort = 0;

	CString strAgent;
	HINTERNET hSession = NULL;
	HINTERNET hConnect = NULL;
	HINTERNET hOpenReq = NULL;
	BOOL bAddRequestHeader = FALSE;
	BOOL bSendRequest = FALSE;
	BOOL bQueryInfo = FALSE;
	
	::ZeroMemory(&uc,sizeof(URL_COMPONENTS));
	uc.dwStructSize = sizeof(URL_COMPONENTS);
	uc.lpszScheme = scheme;
	uc.lpszHostName = hostName;
	uc.lpszUserName = userName;
	uc.lpszPassword = password;
	uc.lpszUrlPath = urlPath;
	uc.lpszExtraInfo = ExtraInfo;
	uc.nPort = nPort;

	uc.dwSchemeLength = MAX_PATH;
	uc.dwHostNameLength = MAX_PATH;
	uc.dwUserNameLength = MAX_PATH;
	uc.dwPasswordLength = MAX_PATH;
	uc.dwUrlPathLength = MAX_PATH;
	
	strAgent = m_itunesstoredUserAgent;
	CString strSendCookie;

	BOOL bCrackUrl = InternetCrackUrl(strUrl,0,0,&uc);
	if (bCrackUrl)
	{
		if (g_netType == PROXY_NETWORK)
		{
			hSession = InternetOpen(strAgent,INTERNET_OPEN_TYPE_PROXY,g_Proxy,0,0);
		}
		else
		{
			hSession = InternetOpen(strAgent,0,0,0,0);
		}
		if (!hSession)
		{
			return FALSE;
		}

		hConnect = InternetConnect(hSession,uc.lpszHostName,uc.nPort,uc.lpszUserName,uc.lpszPassword,3,0,0);
		if (!hConnect)
		{
			InternetCloseHandle(hSession);
			return FALSE;
		}

		hOpenReq = HttpOpenRequest(hConnect,strVerb,strHostName,0,0,0,0x84801000|INTERNET_FLAG_NO_COOKIES,0); //0x84A01000是一个常数
		if (!hOpenReq)
		{
			InternetCloseHandle(hConnect);
			InternetCloseHandle(hSession);
			return FALSE;
		}

		CString strHeaders = _T("User-Agent: ");
		strHeaders += strAgent;
		strHeaders += _T("\r\nAccept-Language: zh-Hans");
		strHeaders += _T("\r\nAccept: */*");
		strHeaders += _T("\r\nX-Apple-Connection-Type: WiFi");
		strHeaders += _T("\r\nX-Apple-Store-Front: 143465-2,29");
		strHeaders += _T("\r\nContent-Type: application/x-www-form-urlencoded");
		strHeaders += _T("\r\nX-Apple-I-Client-Time: ");
		strHeaders += m_ViewPage.GetClientTime();
		
		if (lpX_Apple_I_md_m[0] != TEXT('\0'))
		{
			strHeaders += _T("\r\nX-Apple-I-MD-RINFO: 17106176");
			strHeaders += _T("\r\nX-Apple-I-MD-M: ");
			strHeaders += lpX_Apple_I_md_m;
			strHeaders += _T("\r\nX-Apple-I-MD: ");
			strHeaders += lpX_Apple_I_md;
		}

		//设置cookie值
		strSendCookie = getSendCookie();
		if (!strSendCookie.IsEmpty())
		{
			strHeaders += TEXT("\r\nCookie: ");
			strHeaders += strSendCookie;
		}

		strHeaders += _T("\r\nAccept-Encoding: gzip, deflate");
		strHeaders += _T("\r\nProxy-Connection: keep-alive");
		strHeaders += _T("\r\nConnection: keep-alive\r\n\r\n");
		
		DWORD dwHeaderLength = strHeaders.GetLength();
					
		bAddRequestHeader = HttpAddRequestHeaders(hOpenReq,strHeaders.GetBuffer(),dwHeaderLength,0xA0000000);//0xA0000000是一个常量
		if (!bAddRequestHeader)
		{
			InternetCloseHandle(hOpenReq);
			InternetCloseHandle(hConnect);
			InternetCloseHandle(hSession);
			return FALSE;
		}

		///send data to apple server....
		char constStr[] = "<?xml version=\"1.0\" encoding=\"UTF-8\"?><plist version=\"1.0\"><dict><key>sign-sap-setup-buffer</key><data>%s</data></dict></plist>";
		sprintf(SendData,constStr,lpSignSapsetupBuffer);
		sendLength = strlen(SendData);
		bSendRequest = HttpSendRequest(hOpenReq,NULL,-1,SendData,sendLength);
		if (!bSendRequest && GetLastError() == ERROR_INTERNET_INVALID_CA)
		{
			DWORD dwFlags;
			DWORD dwBuffLen = sizeof(dwFlags);
			InternetQueryOption (hOpenReq, INTERNET_OPTION_SECURITY_FLAGS,(LPVOID)&dwFlags, &dwBuffLen);
			dwFlags |= SECURITY_FLAG_IGNORE_UNKNOWN_CA;
			InternetSetOption (hOpenReq,INTERNET_OPTION_SECURITY_FLAGS,&dwFlags,sizeof (dwFlags) );

			//再次发送
			bSendRequest = HttpSendRequest(hOpenReq,NULL,-1,SendData,sendLength);
			if (!bSendRequest)
			{
				int error = GetLastError();//ERROR_INTERNET_SEC_CERT_REV_FAILED 
				InternetCloseHandle(hOpenReq);
				InternetCloseHandle(hConnect);
				InternetCloseHandle(hSession);
				return FALSE;
			}
		}

		DWORD dwQueryBuf = 200;
		DWORD dwQueryBufferLength = 4;
		bQueryInfo = HttpQueryInfo(hOpenReq,0x20000013,&dwQueryBuf,&dwQueryBufferLength,NULL);//HttpQueryInfo 获取文件大小后
		if (!bQueryInfo)
		{
			InternetCloseHandle(hOpenReq);
			InternetCloseHandle(hConnect);
			InternetCloseHandle(hSession);
			return FALSE;
		}

		if (dwQueryBuf == 200)
		{
			lpRecvBuf = new char[ALLOCATE_SIZE];
			if (!lpRecvBuf)
			{
				InternetCloseHandle(hOpenReq);
				InternetCloseHandle(hConnect);
				InternetCloseHandle(hSession);
				return FALSE;
			}
			ZeroMemory(lpRecvBuf,ALLOCATE_SIZE);

			//读取数据
			ReadDataFromServer(hOpenReq,lpRecvBuf,&dwSize);

			//获取关键值
			char* ptr_1 = strstr(lpRecvBuf,firstSignKey);
			int signKeyLength = strlen(firstSignKey);
			ptr_1 = ptr_1 + signKeyLength;
			char* ptr_2 = strstr(lpRecvBuf,lastSignKey);
			int signSapLength = ptr_2 - ptr_1;

			memcpy(lpOutSignSapSetupBuffer,ptr_1,signSapLength);
			outSignDataLen = signSapLength;
			
			if (lpRecvBuf)
			{
				delete []lpRecvBuf;
				lpRecvBuf = NULL;
			}
		}
		else
		{
			InternetCloseHandle(hOpenReq);
			InternetCloseHandle(hConnect);
			InternetCloseHandle(hSession);
			return FALSE;
		}

	}
	else
	{
		return FALSE;
	}
	if (hOpenReq)
	{
		InternetCloseHandle(hOpenReq);
	}

	if (hConnect)
	{
		InternetCloseHandle(hConnect);
	}

	if (hSession)
	{
		InternetCloseHandle(hSession);
	}
	return TRUE;
}


BOOL CxhNetwork::GetPasswordTokenDsid(char* lpBuffer)
{
	char PasswordToken[MAX_PATH] = {0};
	char szDsid[64] = {0};
	char szCreditDisplay[64] = {0};
	BOOL bResult = FALSE;

	char consPwdToken[] = "<key>passwordToken</key><string>";
	char consPersonId[] = "<key>dsPersonId</key><string>";
	char consCreditdisplay[] = "<key>creditDisplay</key><string>";
	char consEndPersonId[] = "</string>";

	if (lpBuffer)
	{
		char* pPasswordToken = strstr(lpBuffer,consPwdToken);
		if (!pPasswordToken)
		{
			return FALSE;
		}
		int TokenLength = strlen(consPwdToken);
		pPasswordToken = pPasswordToken + TokenLength;
		char* pEnd = strstr(pPasswordToken,consEndPersonId);
		if (!pEnd)
		{
			return FALSE;
		}
		TokenLength = pEnd - pPasswordToken;
		memcpy(PasswordToken,pPasswordToken,TokenLength);
		m_strPasswordToken = PasswordToken;

		char* pDsPersonId = strstr(lpBuffer,consPersonId);
		if (!pDsPersonId)
		{
			return FALSE;
		}
		int personTokenLen = strlen(consPersonId);
		pDsPersonId = pDsPersonId + personTokenLen;
		char* pDsEndPersonId = strstr(pDsPersonId,consEndPersonId);
		if (!pDsEndPersonId)
		{
			return FALSE;
		}

		int personIdLength = pDsEndPersonId - pDsPersonId;

		memcpy(szDsid,pDsPersonId,personIdLength);
		m_strDsid = szDsid;

		char* pBeginCreditDisplay = strstr(pDsPersonId,consCreditdisplay);
		if (!pBeginCreditDisplay)
		{
			return FALSE;
		}

		int creditLen = strlen(consCreditdisplay);
		pBeginCreditDisplay = pBeginCreditDisplay + creditLen;
		char* pEndCreditDisplay = strstr(pBeginCreditDisplay,consEndPersonId);

		int creditDisValueLen = pEndCreditDisplay - pBeginCreditDisplay;
		memcpy(szCreditDisplay,pBeginCreditDisplay,creditDisValueLen);
		m_strCreditDisplay = szCreditDisplay;
	
		bResult = TRUE;
	}
	else
	{
		bResult = FALSE;
	}
	return bResult;
}

BOOL  CxhNetwork::getpliIdsValue(char* lpBuffer,char* lpPiIds)
{
	char strBegin[] = "<key>pliIds</key>";
	char strEnd[] = "</string>";

	if (!lpBuffer)
	{
		return FALSE;
	}

	char* pPosBegin = strstr(lpBuffer,strBegin);
	if (!pPosBegin)
	{
		return FALSE;
	}
	pPosBegin = pPosBegin + strlen(strBegin) + 23; //加21是为了定位到具体的值开始
	char* pPosEnd = strstr(pPosBegin,strEnd);
	if (!pPosEnd)
	{
		return FALSE;
	}

	int length = pPosEnd - pPosBegin;
	memcpy(lpPiIds,pPosBegin,length);


	return TRUE;
}


BOOL CxhNetwork::GetSongIdUrlPathdownloadId(char* lpBuffer,char* lpSongId,CString& lpDownloadUrlPath,CString& lpDownloadKey,char* lpDownloadId,char* lpPliIds)
{
	char strSongId[64] = {0};
	char strDownloadUrlPath[1024] = {0};
	char strDownloadKey[1024] = {0};
	char strDownloadId[128] = {0};
	char strPliIds[64] = {0};

	wchar_t szSongIdKey[] = TEXT("<key>songId</key><integer>");
	wchar_t szDownloadUrlPathKey[] = TEXT("<key>URL</key><string>");
	wchar_t szDownloadKey[] = TEXT("<key>downloadKey</key><string>");
	wchar_t szDownloadId[] = TEXT("<key>download-id</key><string>");
	wchar_t szPliIds[] = TEXT("<key>pliIds</key>");
	wchar_t szString[] = TEXT("<string>");

	if (!lpBuffer)
	{
		return FALSE;
	}
	int aLength = strlen(lpBuffer);
	TCHAR* pszBuf = new TCHAR[aLength*2+6];
	if (!pszBuf)
	{
		return FALSE;
	}
	MultiByteToWideChar(CP_ACP,0,lpBuffer,aLength,pszBuf,aLength*2+6);

	CString strData(pszBuf);


	int beginPos = strData.Find(szSongIdKey);
	if (beginPos < 0)
	{
		return FALSE;
	}
	CString strTmp = strData.Right(strData.GetLength()-beginPos-wcslen(szSongIdKey));
	int endPos = strTmp.Find(TEXT("</integer>"));
	if (endPos < 0)
	{
		return FALSE;
	}
	CString strdwSongId = strTmp.Left(endPos);

	beginPos = strData.Find(szDownloadUrlPathKey);
	if (beginPos < 0)
	{
		return FALSE;
	}
	strTmp = strData.Right(strData.GetLength()-beginPos-wcslen(szDownloadUrlPathKey));
	endPos = strTmp.Find(TEXT("</string>"));
	if (endPos < 0)
	{
		return FALSE;
	}
	CString strdwDownloadUrl = strTmp.Left(endPos);

	beginPos = strData.Find(szDownloadKey);
	if (beginPos < 0)
	{
		return FALSE;
	}
	strTmp = strData.Right(strData.GetLength()-beginPos-wcslen(szDownloadKey));
	endPos = strTmp.Find(TEXT("</string>"));
	if (endPos < 0)
	{
		return FALSE;
	}
	CString strdwDownloadKey = strTmp.Left(endPos);

	beginPos = strData.Find(szDownloadId);
	if (beginPos < 0)
	{
		return FALSE;
	}
	strTmp = strData.Right(strData.GetLength()-beginPos-wcslen(szDownloadId));
	endPos = strTmp.Find(TEXT("</string>"));
	if (endPos < 0)
	{
		return FALSE;
	}
	CString strdwDownloadId = strTmp.Left(endPos);

	beginPos = strData.Find(szPliIds);
	if (beginPos < 0)
	{
		return FALSE;
	}
	strTmp = strData.Right(strData.GetLength()-beginPos-wcslen(szPliIds));
	beginPos = strTmp.Find(szString);
	if (beginPos < 0)
	{
		return FALSE;
	}
	strTmp = strTmp.Right(strTmp.GetLength()-beginPos-wcslen(szString));
	endPos = strTmp.Find(TEXT("</string>"));
	if (endPos < 0)
	{
		return FALSE;
	}
	CString strdwPliIds = strTmp.Left(endPos);


	::WideCharToMultiByte(CP_ACP,0,strdwSongId.GetBuffer(),strdwSongId.GetLength(),strSongId,64,NULL,NULL);
	::WideCharToMultiByte(CP_ACP,0,strdwDownloadId.GetBuffer(),strdwDownloadKey.GetLength(),strDownloadId,128,NULL,NULL);
	::WideCharToMultiByte(CP_ACP,0,strdwPliIds.GetBuffer(),strdwPliIds.GetLength(),strPliIds,64,NULL,NULL);

	strcpy(lpSongId,strSongId);
	lpDownloadUrlPath = strdwDownloadUrl;
	lpDownloadKey = strdwDownloadKey;
	strcpy(lpDownloadId,strDownloadId);
	strcpy(lpPliIds,strPliIds);

	if (pszBuf)
	{
		delete []pszBuf;
		pszBuf = NULL;
	}
	
	return TRUE;
}

int xhCommentApp(IN LPVOID lpNetwork,
				  IN LPVOID lpPasswordToken,
				  IN LPVOID lpDsid,
				  IN LPVOID lpSaleId,
				  IN int rating,
				  IN LPTSTR lpwNickname,
				  IN LPTSTR lpwTitle,
				  IN LPTSTR lpwBody)
{
	if (!lpNetwork)
	{
		return FALSE;
	}
	CxhNetwork* pNetworkObj = (CxhNetwork*)lpNetwork;
	return pNetworkObj->CommentApp(lpPasswordToken,lpDsid,lpSaleId,rating,lpwNickname,lpwTitle,lpwBody);

}

int CxhNetwork::CommentApp(IN LPVOID lpPasswordToken,
							IN LPVOID lpDsid,
							IN LPVOID lpSaleId,
							IN int rating,
							IN LPTSTR lpwNickname,
							IN LPTSTR lpwTitle,
							IN LPTSTR lpwBody)
{
	BOOL res = FALSE;
	int  status = STATUS_FAILED;
	TCHAR strRand[MAX_PATH] = {0};
	TCHAR X_Apple_I_md_m[512] = {0};
	TCHAR X_Apple_I_md[MAX_PATH] = {0};
	CString strNickName;
	SYSTEMTIME sysTime;
	time_t unixTime;

	string strUdid = getUdid();
	string strAppExtVrsId = getAppExtVrsId();
	string strX_Apple_I_md_m = getX_Apple_I_md_m();
	string strX_Apple_I_md = getX_Apple_I_md();

	MultiByteToWideChar(CP_ACP,0,strX_Apple_I_md_m.c_str(),strX_Apple_I_md_m.length(),X_Apple_I_md_m,512);
	MultiByteToWideChar(CP_ACP,0,strX_Apple_I_md.c_str(),strX_Apple_I_md.length(),X_Apple_I_md,MAX_PATH);

	CString strCookie = getSendCookie();
	string strPasswordToken = getPasswordToken();

	res = AppStoreCustomerReviews(m_userAgent,
								strCookie,
								(char*)strPasswordToken.c_str(),
								(char*)lpDsid,
								(char*)lpSaleId,
								X_Apple_I_md_m,
								X_Apple_I_md,
								g_Proxy,
								g_netType);
	if (!res)
	{
		return status;
	}
	res = AppStoreUserReviewRow(m_userAgent,
								strCookie,
								(char*)strPasswordToken.c_str(),
								(char*)lpDsid,
								(char*)lpSaleId,
								X_Apple_I_md_m,
								X_Apple_I_md,
								g_Proxy,
								g_netType);
	if (!res)
	{
		return status;
	}
	res = AppStoreWriteUserReview(m_StoreKitUIServiceAgent,
									strCookie,
									m_strCommentCookie,
									(char*)strPasswordToken.c_str(),
									(char*)lpDsid,
									(char*)lpSaleId,
									(char*)strAppExtVrsId.c_str(),
									strNickName,
									X_Apple_I_md_m,
									X_Apple_I_md,
									g_Proxy,
									g_netType);
	if (!res)
	{
		return status;
	}

	//判断昵称是否为空
	if (strNickName.IsEmpty())
	{
		strNickName = lpwNickname;
	}

	//再次判断昵称是否为空
	if (strNickName.IsEmpty())
	{
		LARGE_INTEGER timeRand;
		QueryPerformanceCounter(&timeRand);
		strNickName.Format(TEXT("%d"),timeRand.LowPart);
	}

	res = AppStoreSaveUserReview(m_StoreKitUIServiceAgent,
								   strCookie,
								   m_strCommentCookie,
								  (char*)strPasswordToken.c_str(),
								  (char*)lpDsid,
								  (char*)lpSaleId,
								  (char*)strAppExtVrsId.c_str(),
								  rating,
								  strNickName.GetBuffer(),
								  lpwTitle,
								  lpwBody,
								  (char*)strUdid.c_str(),
								  X_Apple_I_md_m,
								  X_Apple_I_md,
								  g_Proxy,
								  g_netType);
	if (!res)
	{
		return status;
	}
	
	return STATUS_SUCCESS;

}

int xhGetAppExtVrsIdValue(IN LPVOID lpNetwork,
						   IN LPVOID lpPasswordToken,
						   IN LPVOID lpDsid,
						   IN LPVOID lpTargetUrl)
{
	if (!lpNetwork)
	{
		return FALSE;
	}

	CxhNetwork* pNetworkObj = (CxhNetwork*)lpNetwork;
	return pNetworkObj->GetAppExtVrsIdValue(lpPasswordToken,lpDsid,lpTargetUrl);

}
int CxhNetwork::GetAppExtVrsIdValue(IN LPVOID lpPasswordToken,
									IN LPVOID lpDsid,
									IN LPVOID lpTargetUrl)
{
	URL_COMPONENTS uc;
	TCHAR scheme[MAX_PATH] = {0};
	TCHAR hostName[MAX_PATH] = {0};
	TCHAR userName[MAX_PATH] = {0};
	TCHAR password[MAX_PATH] = {0};
	TCHAR urlPath[MAX_PATH] = {0};
	TCHAR ExtraInfo[MAX_PATH] = {0};
	INTERNET_PORT nPort = 0;

	HINTERNET hSession = NULL;
	HINTERNET hConnect = NULL;
	HINTERNET hOpenReq = NULL;
	BOOL bAddRequestHeader = FALSE;
	BOOL bSendRequest = FALSE;
	BOOL bQueryInfo = FALSE;
	
	::ZeroMemory(&uc,sizeof(URL_COMPONENTS));
	uc.dwStructSize = sizeof(URL_COMPONENTS);
	uc.lpszScheme = scheme;
	uc.lpszHostName = hostName;
	uc.lpszUserName = userName;
	uc.lpszPassword = password;
	uc.lpszUrlPath = urlPath;
	uc.lpszExtraInfo = ExtraInfo;
	uc.nPort = nPort;

	uc.dwSchemeLength = MAX_PATH;
	uc.dwHostNameLength = MAX_PATH;
	uc.dwUserNameLength = MAX_PATH;
	uc.dwPasswordLength = MAX_PATH;
	uc.dwUrlPathLength = MAX_PATH;

	LPGZIP lpgzipBuf = NULL;

	CString strUrl = _T("https://itunes.apple.com");
	CString strAgent = m_userAgent;

	TCHAR strPasswordToken[MAX_PATH] = {0};
	TCHAR strHostName[MAX_PATH] = {0};
	TCHAR strdsid[64] = {0};
	TCHAR X_Apple_I_md_m[512] = {0};
	TCHAR X_Apple_I_md[MAX_PATH] = {0};
	char  szAppExtVrsId[64] = {0};

	MultiByteToWideChar(CP_ACP,0,(LPCSTR)lpPasswordToken,strlen((char*)lpPasswordToken),strPasswordToken,MAX_PATH);

	MultiByteToWideChar(CP_ACP,0,(LPCSTR)lpTargetUrl,strlen((char*)lpTargetUrl),strHostName,MAX_PATH);
			
	MultiByteToWideChar(CP_ACP,NULL,(LPCSTR)lpDsid,strlen((char*)lpDsid),strdsid,64);

	BOOL bCrackUrl = ::InternetCrackUrl(strUrl,0,0,&uc);
	if (bCrackUrl)
	{
		if (g_netType == PROXY_NETWORK)
		{
			hSession = InternetOpen(strAgent,INTERNET_OPEN_TYPE_PROXY,g_Proxy,0,0);
		}
		else
		{
			hSession = InternetOpen(strAgent,0,0,0,0);
		}
		if (!hSession)
		{
			return FALSE;
		}

		hConnect = InternetConnect(hSession,uc.lpszHostName,uc.nPort,uc.lpszUserName,uc.lpszPassword,3,0,0);
		if (!hConnect)
		{
			InternetCloseHandle(hSession);
			return FALSE;
		}

		hOpenReq = HttpOpenRequest(hConnect,_T("GET"),strHostName,0,0,0,0x84801000,0);//0x84A01000是一个常数,注意
		if (!hOpenReq)
		{
			InternetCloseHandle(hConnect);
			InternetCloseHandle(hSession);
			return FALSE;
		}

		CString strHeaders = _T("User-Agent: ");
		strHeaders += strAgent;
		strHeaders += _T("\r\nX-Token: ");
		strHeaders += strPasswordToken;
		strHeaders += _T("\r\nX-Dsid: ");
		strHeaders += strdsid;
		strHeaders += _T("\r\nAccept-Language: zh-Hans");
		strHeaders += _T("\r\nAccept: */*");
		strHeaders += _T("\r\nX-Apple-Connection-Type: WiFi");
		strHeaders += _T("\r\nX-Apple-Store-Front: 143465-19,29 t:native");
		strHeaders += _T("\r\nX-Apple-Client-Versions: GameCenter/2.0");
		strHeaders += _T("\r\nX-Apple-Partner: origin.0");
		strHeaders += _T("\r\nAccept-Encoding: gzip, deflate");
		strHeaders += _T("\r\nConnection: keep-alive");
		strHeaders += _T("\r\nProxy-Connection: keep-alive\r\n\r\n");

		DWORD dwHeaderLength = wcslen(strHeaders);

		bAddRequestHeader = HttpAddRequestHeaders(hOpenReq,strHeaders,dwHeaderLength,0xA0000000);//0xA0000000是一个常量
		if (!bAddRequestHeader)
		{
			InternetCloseHandle(hOpenReq);
			InternetCloseHandle(hConnect);
			InternetCloseHandle(hSession);
			return FALSE;
		}

		bSendRequest = HttpSendRequestW(hOpenReq,NULL,-1,NULL,0);//第三个参数设置为负一
		if (!bSendRequest && GetLastError() == ERROR_INTERNET_INVALID_CA)
		{
			DWORD dwFlags;
			DWORD dwBuffLen = sizeof(dwFlags);
			InternetQueryOption (hOpenReq, INTERNET_OPTION_SECURITY_FLAGS,(LPVOID)&dwFlags, &dwBuffLen);
			dwFlags |= SECURITY_FLAG_IGNORE_UNKNOWN_CA;
			InternetSetOption (hOpenReq,INTERNET_OPTION_SECURITY_FLAGS,&dwFlags,sizeof (dwFlags) );

			//再次发送
			bSendRequest = HttpSendRequest(hOpenReq,NULL,-1,NULL,0);
			if (!bSendRequest)
			{
				int error = GetLastError();//ERROR_INTERNET_SEC_CERT_REV_FAILED 
				InternetCloseHandle(hOpenReq);
				InternetCloseHandle(hConnect);
				InternetCloseHandle(hSession);
				return FALSE;
			}
		}

		DWORD dwQueryBuf = 200;
		DWORD dwQueryBufferLength = 4;
		BOOL bQueryInfo = HttpQueryInfo(hOpenReq,0x20000013,&dwQueryBuf,&dwQueryBufferLength,NULL);//HttpQueryInfo 获取文件大小后
		if (!bQueryInfo)
		{
			InternetCloseHandle(hOpenReq);
			InternetCloseHandle(hConnect);
			InternetCloseHandle(hSession);
			return FALSE;
		}

		if (dwQueryBuf == 200)
		{
			///分配虚拟内存保存页面数据
			DWORD dataLength = 0;
			lpgzipBuf =  (LPGZIP)VirtualAlloc(NULL,1024*1300,MEM_COMMIT | MEM_RESERVE,PAGE_EXECUTE_READWRITE);//分配800K的虚拟内存
			if (lpgzipBuf != NULL)
			{
				DWORD dwReadLength = 0;
				DWORD dwMaxDataLength = 200;
				BOOL bReadFile = FALSE;
				char DataBuffer[200];
				memset(DataBuffer,0x00,200*sizeof(char));

				while (true)//循环读取页面数据
				{
					bReadFile = InternetReadFile(hOpenReq,DataBuffer,dwMaxDataLength,&dwReadLength);
					if (bReadFile && dwReadLength != 0)
					{
						memcpy(lpgzipBuf + dataLength,DataBuffer,dwReadLength);
						dataLength += dwReadLength;														
						dwReadLength = 0;	

					}
					else
					{
						InternetCloseHandle(hOpenReq);
						InternetCloseHandle(hConnect);
						InternetCloseHandle(hSession);
						break;
					}

				}//while end
				
			}
			else
			{
				InternetCloseHandle(hOpenReq);
				InternetCloseHandle(hConnect);
				InternetCloseHandle(hSession);
				return FALSE;
			}

			CGZIP2A gzip2A(lpgzipBuf,dataLength);
			char * pDecodeData = gzip2A.psz;//解密出来的网页头指针
			if (!pDecodeData)
			{
				if (lpgzipBuf)
				{
					VirtualFree(lpgzipBuf,0,MEM_RELEASE);
					lpgzipBuf = NULL;
				}
				InternetCloseHandle(hOpenReq);
				InternetCloseHandle(hConnect);
				InternetCloseHandle(hSession);
				return FALSE;
			}
			//检索关键数据 搜索AppExtVrsId的值
			char strBuf[512] = "&pricingParameters=STDQ&pg=default&appExtVrsId=";
			char strEnd[] = "\",\"actionText\"";
			char strEnd2[] = "\",\"version\"";
			char* pPosBegin = strstr(pDecodeData,strBuf);
			if (!pPosBegin)
			{
			
				if (lpgzipBuf)
				{
					VirtualFree(lpgzipBuf,0,MEM_RELEASE);
					lpgzipBuf = NULL;
				}
				InternetCloseHandle(hOpenReq);
				InternetCloseHandle(hConnect);
				InternetCloseHandle(hSession);
				return FALSE;
			}
			pPosBegin = pPosBegin + strlen(strBuf);
			char* pPosEnd = strstr(pPosBegin,strEnd);
			if (!pPosEnd)
			{
				pPosEnd = strstr(pPosBegin,strEnd2);
				if (!pPosEnd)
				{
					if (lpgzipBuf)
					{
						VirtualFree(lpgzipBuf,0,MEM_RELEASE);
						lpgzipBuf = NULL;
					}
					InternetCloseHandle(hOpenReq);
					InternetCloseHandle(hConnect);
					InternetCloseHandle(hSession);
					return FALSE;
				}
			}

			int length = pPosEnd - pPosBegin;      //获取长度
			char tmpBuf[MAX_PATH] = {0};
			memcpy(tmpBuf,pPosBegin,length);
			strcpy(szAppExtVrsId,tmpBuf);
			setAppExtVrsId(szAppExtVrsId);

		}
		else
		{
			InternetCloseHandle(hOpenReq);
			InternetCloseHandle(hConnect);
			InternetCloseHandle(hSession);
			return FALSE;
		}


	}
	else
	{
		return FALSE;
	}

	if (hOpenReq)
	{
		InternetCloseHandle(hOpenReq);
	}

	if (hConnect)
	{
		InternetCloseHandle(hConnect);
	}

	if (hSession)
	{
		InternetCloseHandle(hSession);
	}


	return TRUE;
}

int xhSearchKeyHotWord(IN LPVOID lpNetwork,
						IN LPTSTR strwKeyword,
					    IN LPVOID lpSaleId,
						IN LPVOID lpDsid,
					    IN LPVOID lpPasswordToken,
						IN LPVOID lpTargetUrl)
{
	if (!lpNetwork)
	{
		return FALSE;
	}
	CxhNetwork* pNetworkObj = (CxhNetwork*)lpNetwork;
	return pNetworkObj->SearchKeyHotWord(strwKeyword,lpSaleId,lpDsid,lpPasswordToken,lpTargetUrl);

}

int CxhNetwork::SearchKeyHotWord(IN LPTSTR strwKeyword,
								  IN LPVOID lpSaleId,
								  IN LPVOID lpDsid,
								  IN LPVOID lpPasswordToken,
								  IN LPVOID lpTargetUrl)
{
	char strAppExtVrsId[MAX_PATH] = {0};
	char strSearchSumbitEvent[MAX_ALLOCATE_SIZE] = {0};
	char strImpressionEvent[MAX_ALLOCATE_SIZE] = {0};
	char strPageDetailEvent[4096] = {0};
	char strPageSearhEvent[4096] = {0};
	char strAppLaunchEvent[ALLOCATE_SIZE] = {0};
	
	char strPageDetails[MAX_DETAILS_SIZE] = {0};
	char strPageSearchUrl[MAX_PATH] = {0};
	char strPageloadTime1[MAX_PATH] = {0};
	char strPageloadTime3[MAX_PATH] = {0};
	char strResponseStartTime[MAX_PATH] = {0};
	char strResponseEndTime[MAX_PATH] = {0};
	char strSearchword[MAX_PATH] = {0};
	char strPageUrl[MAX_PATH] = {0};
	char szProductContext[MAX_PATH] = {0};
	char szSearchlockupContext[MAX_PATH] = {0};
	char szImpressionTime[MAX_PATH] = {0};
	char szNativeSearchLockup[MAX_PATH] = {0};
	int  status = STATUS_FAILED;
	CString strX_Apple_I_md_m;
	CString strX_Apple_I_md;
	CString strXpMainSignature;

	string		strAppId = "";
	string		strClientId = "";;
	string		strXpab = "";;
	string		strXpabc = "";;
	string      strI_md_m = "";
	string      strI_md = "";

	m_strSaleId = (char*)lpSaleId;

	USES_CONVERSION;

	//生成I_md_m 和I_md的值
	GenerateXApple_I_MDandMDMValue();
	strI_md_m = getX_Apple_I_md_m();
	strI_md = getX_Apple_I_md();
	strX_Apple_I_md_m = A2T((char*)strI_md_m.c_str());
	strX_Apple_I_md = A2T((char*)strI_md.c_str());

	CString strSendCookie = getSendCookie();

	WideCharToMultiByte(CP_UTF8,0,strwKeyword,wcslen(strwKeyword),strSearchword,MAX_PATH,NULL,NULL);
	m_strSearchKeyword = strSearchword;

	strcat(strPageUrl,"https://itunes.apple.com");
	strcat(strPageUrl,(char*)lpTargetUrl);

	m_strAppUrl = strPageUrl;

	//发送硬件信息相关
	SendSubscriptionStatusSrv();
	SendfuseNoCarrierBundleSubscription();

	//发送注册xp包
	SendxpRegister((char*)lpDsid,m_itunesstoredUserAgent);

	//更新cookie
	strSendCookie = getSendCookie();

	//发送bootstrap数据
	m_ViewPage.appBootStrap(m_userAgent,(char*)lpPasswordToken,(char*)lpDsid,strSendCookie,strX_Apple_I_md_m.GetBuffer(),strX_Apple_I_md.GetBuffer(),g_Proxy,g_netType);

	//发送注册xp包
	SendxpRegister((char*)lpDsid,m_userAgent);
	SendxpRegister((char*)lpDsid,m_comAppleAppStoredAgent);

	//打开主界面
	m_ViewPage.openViewAppMain(m_userAgent,(char*)lpPasswordToken,(char*)lpDsid,strSendCookie,strX_Apple_I_md_m.GetBuffer(),strX_Apple_I_md.GetBuffer(),g_Proxy,g_netType);
	//搜索点击(出现推荐列表)
	if (!m_ViewPage.searchHotWordHint(m_userAgent,strwKeyword,(char*)lpPasswordToken,(char*)lpDsid,strSendCookie,strX_Apple_I_md_m.GetBuffer(),strX_Apple_I_md.GetBuffer(),g_Proxy,g_netType))
	{
		m_log.xhLogPrintf(SEARCHHOTWORDHINT_FAILED,__LINE__,__FUNCTION__,__FILE__);
		return status;
	}
	m_ViewPage.getEventTime(strResponseEndTime,FALSE,TRUE);
		                        
	//生成I_md_m 和I_md的值
	GenerateXApple_I_MDandMDMValue();
	strI_md_m = getX_Apple_I_md_m();
	strI_md = getX_Apple_I_md();
	strX_Apple_I_md_m = A2T((char*)strI_md_m.c_str());
	strX_Apple_I_md = A2T((char*)strI_md.c_str());
	//提交搜索关键词信息到服务器
	if (!m_ViewPage.searchHotWordSubmit(m_userAgent,
										strwKeyword,
										(char*)lpSaleId,
										(char*)lpDsid,
										(char*)lpPasswordToken,
										strPageSearchUrl,
										strSendCookie,
										strX_Apple_I_md_m.GetBuffer(),
										strX_Apple_I_md.GetBuffer(),
										g_Proxy,
										g_netType))
	{

		if (!m_ViewPage.searchHotWordSubmit(m_userAgent,
										strwKeyword,
										(char*)lpSaleId,
										(char*)lpDsid,
										(char*)lpPasswordToken,
										strPageSearchUrl,
										strSendCookie,
										strX_Apple_I_md_m.GetBuffer(),
										strX_Apple_I_md.GetBuffer(),
										g_Proxy,
										g_netType))
		{
			m_log.xhLogPrintf(SEARCHHOTWORDSUBMIT_FAILED,__LINE__,__FUNCTION__,__FILE__);
			return status;
		}
	}

	m_strPageSearchUrl = strPageSearchUrl;

	//获取启动appstore的数据包
	m_FormatUserAgent = FormatString(T2A(m_userAgent.GetBuffer()));
	m_ViewPage.xpAppLaunchEvent(m_FormatUserAgent,
								(char*)m_version.c_str(),
								m_width,
								m_height,
								strAppLaunchEvent,
								(char*)m_strClientId.c_str(),
								(char*)lpDsid,
								(char*)m_strxp_ab_value.c_str());
	m_strSHA1 = GetStringSha1(strAppLaunchEvent);
	strXpMainSignature = GetxpMainXAppleActionSignature(m_strSHA1);


	//生成I_md_m 和I_md的值
	GenerateXApple_I_MDandMDMValue();
	strI_md_m = getX_Apple_I_md_m();
	strI_md = getX_Apple_I_md();
	strX_Apple_I_md_m = A2T((char*)strI_md_m.c_str());
	strX_Apple_I_md = A2T((char*)strI_md.c_str());
	//发送amp_clientPerf数据包
	if ( !m_ViewPage.xpAmpClientPerf(m_itunesstoredUserAgent,
											strAppLaunchEvent,
											(char*)m_strDsid.c_str(),
											strSendCookie,
											strXpMainSignature,
											strX_Apple_I_md_m.GetBuffer(),
											strX_Apple_I_md.GetBuffer(),
											g_Proxy,
											g_netType) )
	{
		if ( !m_ViewPage.xpAmpClientPerf(m_itunesstoredUserAgent,
											strAppLaunchEvent,
											(char*)m_strDsid.c_str(),
											strSendCookie,
											strXpMainSignature,
											strX_Apple_I_md_m.GetBuffer(),
											strX_Apple_I_md.GetBuffer(),
											g_Proxy,
											g_netType) )
			{
				m_log.xhLogPrintf(XPREPORTTARGETAPPMAIN_FAILED,__LINE__,__FUNCTION__,__FILE__);
			}
	}


	//获取搜索提交数据包
	string strSearchUrl = FormatString(strPageSearchUrl);
	m_ViewPage.xpSearchSumbitEvent(m_FormatUserAgent,
									(char*)m_version.c_str(),
									m_width,
									m_height,
								   strSearchSumbitEvent,
								   strSearchword,
								   (char*)strSearchUrl.c_str(),
								   (char*)m_strClientId.c_str(),
								   (char*)lpSaleId,
								   (char*)lpDsid,
								   (char*)m_strxp_ab_value.c_str(),
								   (char*)m_strxp_abc_value.c_str());

	m_strSHA1 = GetStringSha1(strSearchSumbitEvent);
	strXpMainSignature = GetxpMainXAppleActionSignature(m_strSHA1);

	//生成I_md_m 和I_md的值
	GenerateXApple_I_MDandMDMValue();
	strI_md_m = getX_Apple_I_md_m();
	strI_md = getX_Apple_I_md();
	strX_Apple_I_md_m = A2T((char*)strI_md_m.c_str());
	strX_Apple_I_md = A2T((char*)strI_md.c_str());
	//发送搜索提交数据包
	if (!m_ViewPage.xpReportTargetAppMain(m_itunesstoredUserAgent,
											strSearchSumbitEvent,
											(char*)lpPasswordToken,
											(char*)lpDsid,
											strSendCookie,
											strXpMainSignature,
											strX_Apple_I_md_m.GetBuffer(),
											strX_Apple_I_md.GetBuffer(),
											g_Proxy,
											g_netType))
	{
		if (!m_ViewPage.xpReportTargetAppMain(m_itunesstoredUserAgent,
											strSearchSumbitEvent,
											(char*)lpPasswordToken,
											(char*)lpDsid,
											strSendCookie,
											strXpMainSignature,
											strX_Apple_I_md_m.GetBuffer(),
											strX_Apple_I_md.GetBuffer(),
											g_Proxy,
											g_netType))
		{
			m_log.xhLogPrintf(XPREPORTTARGETAPPMAIN_FAILED,__LINE__,__FUNCTION__,__FILE__);
			return status;
		}
	}
	//生成I_md_m 和I_md的值
	GenerateXApple_I_MDandMDMValue();
	strI_md_m = getX_Apple_I_md_m();
	strI_md = getX_Apple_I_md();
	strX_Apple_I_md_m = A2T((char*)strI_md_m.c_str());
	strX_Apple_I_md = A2T((char*)strI_md.c_str());
	//发送本地查询app id 数据包
	sprintf(szNativeSearchLockup,"native-search-lockup=%s",lpSaleId);
	m_ViewPage.sendBuyButtonMetaData(m_userAgent,
									szNativeSearchLockup,
									(char*)lpDsid,
									(char*)lpPasswordToken,
									strSendCookie,
									strX_Apple_I_md_m.GetBuffer(),
									strX_Apple_I_md.GetBuffer(),
									g_Proxy,
									g_netType);
				
	//获取搜索页面信息数据包
	m_ViewPage.xpPageSearchEvent(m_FormatUserAgent,
								(char*)m_version.c_str(),
								m_width,
								m_height,
								strPageSearhEvent,
								strSearchword,
								(char*)m_strClientId.c_str(),
								(char*)lpDsid,
								(char*)m_strxp_ab_value.c_str(),
								(char*)m_strxp_abc_value.c_str());
	m_strSHA1 = GetStringSha1(strPageSearhEvent);
	strXpMainSignature = GetxpMainXAppleActionSignature(m_strSHA1);
	//生成I_md_m 和I_md的值
	GenerateXApple_I_MDandMDMValue();
	strI_md_m = getX_Apple_I_md_m();
	strI_md = getX_Apple_I_md();
	strX_Apple_I_md_m = A2T((char*)strI_md_m.c_str());
	strX_Apple_I_md = A2T((char*)strI_md.c_str());
	//发送搜索就页面信息数据包
	if (!m_ViewPage.xpReportTargetAppMain(m_itunesstoredUserAgent,strPageSearhEvent,(char*)lpPasswordToken,(char*)lpDsid,strSendCookie,strXpMainSignature,strX_Apple_I_md_m.GetBuffer(),strX_Apple_I_md.GetBuffer(),g_Proxy,g_netType))
	{
		if (!m_ViewPage.xpReportTargetAppMain(m_itunesstoredUserAgent,strPageSearhEvent,(char*)lpPasswordToken,(char*)lpDsid,strSendCookie,strXpMainSignature,strX_Apple_I_md_m.GetBuffer(),strX_Apple_I_md.GetBuffer(),g_Proxy,g_netType))
		{
			m_log.xhLogPrintf(XPREPORTTARGETAPPMAIN_FAILED,__LINE__,__FUNCTION__,__FILE__);
			return status;
		}
	}

	//获取搜索结果显示出来的排名第一和第二的app名字
	strAppId = m_ViewPage.m_strFirstAppId;
	m_ViewPage.getPreAndNextAppName(m_userAgent,(char*)strAppId.c_str(),1);
	strAppId = m_ViewPage.m_strSecondAppId;
	m_ViewPage.getPreAndNextAppName(m_userAgent,(char*)strAppId.c_str(),2);

	//提取目标app上一个app和下一个app的名字
	strAppId = m_ViewPage.m_strPreAppId; 
	m_ViewPage.getPreAndNextAppName(m_userAgent,(char*)strAppId.c_str(),3);
	strAppId = m_ViewPage.m_strNextAppId;
	m_ViewPage.getPreAndNextAppName(m_userAgent,(char*)strAppId.c_str(),4);
	

	//生成I_md_m 和I_md的值
	GenerateXApple_I_MDandMDMValue();
	strI_md_m = getX_Apple_I_md_m();
	strI_md = getX_Apple_I_md();
	strX_Apple_I_md_m = A2T((char*)strI_md_m.c_str());
	strX_Apple_I_md = A2T((char*)strI_md.c_str());
	//获取目标id网页数据,提取需要的xp包信息字段
	if (!m_ViewPage.getTargetIdWebData(m_userAgent,
										strwKeyword,
										(char*)lpTargetUrl,
										(char*)lpSaleId,
										(char*)lpDsid,
										strAppExtVrsId,
										(char*)lpPasswordToken,
										strSendCookie,
										strX_Apple_I_md_m.GetBuffer(),
										strX_Apple_I_md.GetBuffer(),
										g_Proxy,
										g_netType))
	{
		if (!m_ViewPage.getTargetIdWebData(m_userAgent,
										strwKeyword,
										(char*)lpTargetUrl,
										(char*)lpSaleId,
										(char*)lpDsid,
										strAppExtVrsId,
										(char*)lpPasswordToken,
										strSendCookie,
										strX_Apple_I_md_m.GetBuffer(),
										strX_Apple_I_md.GetBuffer(),
										g_Proxy,
										g_netType))
		{
			m_log.xhLogPrintf(GETTARGETIDWEBDATA_FAILED,__LINE__,__FUNCTION__,__FILE__);
			return status;
		}
	}

	//生成I_md_m 和I_md的值
	GenerateXApple_I_MDandMDMValue();
	strI_md_m = getX_Apple_I_md_m();
	strI_md = getX_Apple_I_md();
	strX_Apple_I_md_m = A2T((char*)strI_md_m.c_str());
	strX_Apple_I_md = A2T((char*)strI_md.c_str());
	setAppExtVrsId(strAppExtVrsId);
	//发送指定购买Id的buyButtonMetaData的值
	sprintf(szProductContext,"product-dv=%s",lpSaleId);
	m_ViewPage.sendBuyButtonMetaData(m_userAgent,
									szProductContext,
									(char*)lpDsid,
									(char*)lpPasswordToken,
									strSendCookie,
									strX_Apple_I_md_m.GetBuffer(),
									strX_Apple_I_md.GetBuffer(),
									g_Proxy,
									g_netType);

	//发送xp注册包
	SendxpRegister((char*)m_strDsid.c_str(),m_itunesstoredUserAgent);

	//发送amp_clientPerf数据包
	Sendamp_clientPerfData();
	//这部分加到购买成功后发送
	//获取翻页展示impression事件数据包
	string strAppUrl = FormatString(m_strAppUrl);
	m_ViewPage.xpImpressionsEvent(m_FormatUserAgent,
								(char*)m_version.c_str(),
								  m_width,
								  m_height,
								  strImpressionEvent,
								  strSearchword,
								  (char*)strAppUrl.c_str(),
								  (char*)m_strClientId.c_str(),
								  (char*)lpSaleId,
								  (char*)lpDsid,
								  (char*)m_strxp_ab_value.c_str(),
								  (char*)m_strxp_abc_value.c_str());
	m_strSHA1 = GetStringSha1(strImpressionEvent);
	strXpMainSignature = GetxpMainXAppleActionSignature(m_strSHA1);

	//发送翻页展示impression事件数据包
	if (!m_ViewPage.xpReportTargetAppMain(m_itunesstoredUserAgent,strImpressionEvent,(char*)lpPasswordToken,(char*)lpDsid,strSendCookie,strXpMainSignature,strX_Apple_I_md_m.GetBuffer(),strX_Apple_I_md.GetBuffer(),g_Proxy,g_netType))
	{
		if (!m_ViewPage.xpReportTargetAppMain(m_itunesstoredUserAgent,strImpressionEvent,(char*)lpPasswordToken,(char*)lpDsid,strSendCookie,strXpMainSignature,strX_Apple_I_md_m.GetBuffer(),strX_Apple_I_md.GetBuffer(),g_Proxy,g_netType))
		{
			m_log.xhLogPrintf(XPREPORTTARGETAPPMAIN_FAILED,__LINE__,__FUNCTION__,__FILE__);
			return status;
		}
	}
	//获取目标app详细页面数据包
	m_ViewPage.getEventTime(strPageloadTime3,FALSE,TRUE);
	m_ViewPage.xpTargetAppPageDetailEvent(m_FormatUserAgent,
										    (char*)m_version.c_str(),
											m_width,
											m_height,
		                                    strPageDetailEvent,
											(char*)strAppUrl.c_str(),
											(char*)m_strClientId.c_str(),
											(char*)lpSaleId,
											(char*)lpDsid,
											(char*)m_strSearchKeyword.c_str(),
											strPageloadTime3,
											(char*)m_strxp_ab_value.c_str(),
											(char*)m_strxp_abc_value.c_str());
	m_strSHA1 = GetStringSha1(strPageDetailEvent);
	strXpMainSignature = GetxpMainXAppleActionSignature(m_strSHA1);
	//生成I_md_m 和I_md的值
	GenerateXApple_I_MDandMDMValue();
	strI_md_m = getX_Apple_I_md_m();
	strI_md = getX_Apple_I_md();
	strX_Apple_I_md_m = A2T((char*)strI_md_m.c_str());
	strX_Apple_I_md = A2T((char*)strI_md.c_str());
	//发送目标app详细页面数据包
	if (!m_ViewPage.xpReportTargetAppMain(m_itunesstoredUserAgent,strPageDetailEvent,(char*)lpPasswordToken,(char*)lpDsid,strSendCookie,strXpMainSignature,strX_Apple_I_md_m.GetBuffer(),strX_Apple_I_md.GetBuffer(),g_Proxy,g_netType))
	{
		if (!m_ViewPage.xpReportTargetAppMain(m_itunesstoredUserAgent,strPageDetailEvent,(char*)lpPasswordToken,(char*)lpDsid,strSendCookie,strXpMainSignature,strX_Apple_I_md_m.GetBuffer(),strX_Apple_I_md.GetBuffer(),g_Proxy,g_netType))
		{
			m_log.xhLogPrintf(XPREPORTTARGETAPPMAIN_FAILED,__LINE__,__FUNCTION__,__FILE__);
			return status;
		}
	}
	
	return STATUS_SUCCESS;
}

void CxhNetwork::Sendamp_clientPerfData()
{
	char strAmpClientPerfData[MAX_ALLOCATE_SIZE] = {0};
	TCHAR X_Apple_I_md_m[512] = {0};
	TCHAR X_Apple_I_md[MAX_PATH] = {0};


	string strX_Apple_I_md_m = getX_Apple_I_md_m();
	string strX_Apple_I_md = getX_Apple_I_md();

	MultiByteToWideChar(CP_ACP,0,strX_Apple_I_md_m.c_str(),strX_Apple_I_md_m.length(),X_Apple_I_md_m,512);
	MultiByteToWideChar(CP_ACP,0,strX_Apple_I_md.c_str(),strX_Apple_I_md.length(),X_Apple_I_md,MAX_PATH);

	//获取页面渲染amp_clientPerf数据包
	string strSearchUrl = FormatString(m_strPageSearchUrl);
	m_ViewPage.xpPageRanderAmpClientPerfEvent(m_FormatUserAgent,
											(char*)m_version.c_str(),
											m_width,
											m_height,
											strAmpClientPerfData,
											(char*)m_strSearchKeyword.c_str(),
											(char*)strSearchUrl.c_str(),
											(char*)m_strClientId.c_str(),
											(char*)m_strSaleId.c_str(),
											(char*)m_strDsid.c_str(),
											(char*)m_strxp_ab_value.c_str(),
											(char*)m_strxp_abc_value.c_str());


	m_strSHA1 = GetStringSha1(strAmpClientPerfData);
	CString strXpMainSignature = GetxpMainXAppleActionSignature(m_strSHA1);
	CString strSendCookie = getSendCookie();

	//发送amp_clientPerf数据包
	if ( !m_ViewPage.xpAmpClientPerf(m_itunesstoredUserAgent,
											strAmpClientPerfData,
											(char*)m_strDsid.c_str(),
											strSendCookie,
											strXpMainSignature,
											X_Apple_I_md_m,
										    X_Apple_I_md,
											g_Proxy,
											g_netType) )
	{
		if ( !m_ViewPage.xpAmpClientPerf(m_itunesstoredUserAgent,
											strAmpClientPerfData,
											(char*)m_strDsid.c_str(),
											strSendCookie,
											strXpMainSignature,
											X_Apple_I_md_m,
										    X_Apple_I_md,
											g_Proxy,
											g_netType) )
			{
				m_log.xhLogPrintf(XPREPORTTARGETAPPMAIN_FAILED,__LINE__,__FUNCTION__,__FILE__);
			}
	}

	return;

}

int xhOnlySearchKeyword(IN LPVOID lpNetwork,IN LPTSTR strwKeyword)
{
	if (!lpNetwork)
	{
		return FALSE;
	}
	CxhNetwork* pNetworkObj = (CxhNetwork*)lpNetwork;
	return pNetworkObj->OnlySearchKeyword(strwKeyword);
}

int CxhNetwork::OnlySearchKeyword(IN LPTSTR strwKeyword)
{
	int status = STATUS_FAILED;
	char strPageSearchUrl[MAX_PATH] = {0};
	//发送bootstrap数据
	CString strSendCookie;
	m_ViewPage.appBootStrap(m_userAgent,NULL,NULL,strSendCookie,NULL,NULL,g_Proxy,g_netType);

	//打开主界面
	m_ViewPage.openViewAppMain(m_userAgent,NULL,NULL,strSendCookie,NULL,NULL,g_Proxy,g_netType);


	//搜索点击(出现推荐列表)
	if (!m_ViewPage.searchHotWordHint(m_userAgent,strwKeyword,NULL,NULL,strSendCookie,NULL,NULL,g_Proxy,g_netType))
	{
		m_log.xhLogPrintf(SEARCHHOTWORDHINT_FAILED,__LINE__,__FUNCTION__,__FILE__);
		return status;
	}

	//提交搜索关键词信息到服务器
	if (!m_ViewPage.searchHotWordSubmit(m_userAgent,
										strwKeyword,
										NULL,
										NULL,
										NULL,
										strPageSearchUrl,
										strSendCookie,
										NULL,
										NULL,
										g_Proxy,
										g_netType))
	{

		if (!m_ViewPage.searchHotWordSubmit(m_userAgent,
										strwKeyword,
										NULL,
										NULL,
										NULL,
										strPageSearchUrl,
										strSendCookie,
										NULL,
										NULL,
										g_Proxy,
										g_netType))
		{
			m_log.xhLogPrintf(SEARCHHOTWORDSUBMIT_FAILED,__LINE__,__FUNCTION__,__FILE__);
			return status;
		}
	}

	status = SEARCHSUCCESS;
	return status;
}

int xhBuyConfirmActiveApp(IN LPVOID lpNetwork,
						   IN LPVOID lpPasswordToken,
						   IN LPVOID lpSaleId,
						   IN LPVOID lpDsid,
						   IN LPVOID lpTargetUrl)
{
	if (!lpNetwork)
	{
		return FALSE;
	}
	CxhNetwork* pNetworkObj = (CxhNetwork*)lpNetwork;
	return pNetworkObj->BuyConfirmActiveApp(lpPasswordToken,lpSaleId,lpDsid,lpTargetUrl);


}
int CxhNetwork::BuyConfirmActiveApp(IN LPVOID lpPasswordToken,
									 IN LPVOID lpSaleId,
									 IN LPVOID lpDsid,
									 IN LPVOID lpTargetUrl)
{
	char strPageUrl[MAX_PATH] = {0};
	char strBuyConfirmEvent[ALLOCATE_SIZE] = {0};
	char strActiveAppEvent[ALLOCATE_SIZE] = {0};
	char strCommentEvent[ALLOCATE_SIZE] = {0};
	char strOpenAppEvent[ALLOCATE_SIZE] = {0};
	char strFinishImpressionEvent[ALLOCATE_SIZE] = {0};
	char strImpressiontime[MAX_PATH] = {0};
	TCHAR X_Apple_I_md_m[512] = {0};
	TCHAR X_Apple_I_md[MAX_PATH] = {0};

	CString strXpMainSignature;

	string strClientId = getClientId();
	string strMtRequestId = getMtRequestId();
	string strPliIds = getPliIds();
	string strX_Apple_I_md_m = getX_Apple_I_md_m();
	string strX_Apple_I_md = getX_Apple_I_md();
	
	m_ViewPage.getEventTime(strImpressiontime,TRUE,FALSE);

	strcat(strPageUrl,"https://itunes.apple.com");
	strcat(strPageUrl,(char*)lpTargetUrl);

	CString strCookieData = getSendCookie();

	MultiByteToWideChar(CP_ACP,0,strX_Apple_I_md_m.c_str(),strX_Apple_I_md_m.length(),X_Apple_I_md_m,512);
	MultiByteToWideChar(CP_ACP,0,strX_Apple_I_md.c_str(),strX_Apple_I_md.length(),X_Apple_I_md,MAX_PATH);

	//获取打开应用的xp数据包
	m_ViewPage.xpOpenAppEvent(m_FormatUserAgent,
								(char*)m_version.c_str(),
								m_width,
								m_height,
								strOpenAppEvent,
								(char*)strClientId.c_str(),
								(char*)lpSaleId,
								(char*)lpDsid,
								(char*)m_strxp_ab_value.c_str(),
								(char*)m_strxp_abc_value.c_str());
	m_strSHA1 = GetStringSha1(strOpenAppEvent);
	strXpMainSignature = GetxpMainXAppleActionSignature(m_strSHA1);

	//发送打开应用的xp数据包
	if ( !m_ViewPage.xpReportTargetAppMain(m_itunesstoredUserAgent,strOpenAppEvent,(char*)lpPasswordToken,(char*)lpDsid,strCookieData,strXpMainSignature,X_Apple_I_md_m,X_Apple_I_md,g_Proxy,g_netType) )
	{
		if ( !m_ViewPage.xpReportTargetAppMain(m_itunesstoredUserAgent,strOpenAppEvent,(char*)lpPasswordToken,(char*)lpDsid,strCookieData,strXpMainSignature,X_Apple_I_md_m,X_Apple_I_md,g_Proxy,g_netType) )
		{
			return FALSE;
		}
	}

	//获取激活app数据包
	m_ViewPage.xpActiveAppEvent(m_FormatUserAgent,
							    (char*)m_version.c_str(),
								m_width,
								m_height,
								strActiveAppEvent,
								(char*)strClientId.c_str(),
								(char*)lpSaleId,
								(char*)lpDsid,
								strImpressiontime,
								(char*)m_strxp_ab_value.c_str(),
							    (char*)m_strxp_abc_value.c_str());
	m_strSHA1 = GetStringSha1(strActiveAppEvent);
	strXpMainSignature = GetxpMainXAppleActionSignature(m_strSHA1);

	//发送激活app数据包
	if ( !m_ViewPage.xpReportTargetAppMain(m_itunesstoredUserAgent,strActiveAppEvent,(char*)lpPasswordToken,(char*)lpDsid,strCookieData,strXpMainSignature,X_Apple_I_md_m,X_Apple_I_md,g_Proxy,g_netType) )
	{
		if ( !m_ViewPage.xpReportTargetAppMain(m_itunesstoredUserAgent,strActiveAppEvent,(char*)lpPasswordToken,(char*)lpDsid,strCookieData,strXpMainSignature,X_Apple_I_md_m,X_Apple_I_md,g_Proxy,g_netType) )
		{
			return FALSE;
		}
	}


	//获取评论xp数据包
	m_ViewPage.xpCommentAppEvent(m_userAgent,
								(char*)m_version.c_str(),
								m_width,
								m_height,
								strCommentEvent,
								(char*)strClientId.c_str(),
								(char*)lpSaleId,
								(char*)lpDsid,
								strImpressiontime,
								(char*)m_strxp_ab_value.c_str(),
							    (char*)m_strxp_abc_value.c_str());
	m_strSHA1 = GetStringSha1(strCommentEvent);
	strXpMainSignature = GetxpMainXAppleActionSignature(m_strSHA1);
	//发送评论xp数据包
	if (!m_ViewPage.xpReportTargetAppMain(m_itunesstoredUserAgent,strCommentEvent,(char*)lpPasswordToken,(char*)lpDsid,strCookieData,strXpMainSignature,X_Apple_I_md_m,X_Apple_I_md,g_Proxy,g_netType))
	{
		if (!m_ViewPage.xpReportTargetAppMain(m_itunesstoredUserAgent,strCommentEvent,(char*)lpPasswordToken,(char*)lpDsid,strCookieData,strXpMainSignature,X_Apple_I_md_m,X_Apple_I_md,g_Proxy,g_netType))
		{
			return FALSE;
		}
	}

	//获取完成展示xp包
	m_ViewPage.xpFinishImpressionEvent(m_userAgent,
								(char*)m_version.c_str(),
								m_width,
								m_height,
								strFinishImpressionEvent,
								(char*)m_strSearchKeyword.c_str(),
								(char*)m_strClientId.c_str(),
								(char*)lpSaleId,
								(char*)lpDsid,
								(char*)m_strxp_ab_value.c_str(),
							    (char*)m_strxp_abc_value.c_str());
	m_strSHA1 = GetStringSha1(strFinishImpressionEvent);
	strXpMainSignature = GetxpMainXAppleActionSignature(m_strSHA1);
	//发送评论xp数据包
	if (!m_ViewPage.xpReportTargetAppMain(m_itunesstoredUserAgent,strFinishImpressionEvent,(char*)lpPasswordToken,(char*)lpDsid,strCookieData,strXpMainSignature,X_Apple_I_md_m,X_Apple_I_md,g_Proxy,g_netType))
	{
		if (!m_ViewPage.xpReportTargetAppMain(m_itunesstoredUserAgent,strFinishImpressionEvent,(char*)lpPasswordToken,(char*)lpDsid,strCookieData,strXpMainSignature,X_Apple_I_md_m,X_Apple_I_md,g_Proxy,g_netType))
		{
			return FALSE;
		}
	}
	return TRUE;
}

CxhNetwork::CxhNetwork()
{
	m_strSignsapsetup = "";
	m_strAppUrl = "";
	m_strUdid = "";
	m_strPasswordToken = "";
	m_strDsid = "";
	m_strSaleId = "";
	m_strPageSearchUrl = "";
	m_strCreditDisplay = "";
	m_strAppleId = "";
	m_strPassword = "";
	m_strAdsid = "";
	m_strClientId = "";
	m_strAppExtVrsId = "";
	m_strMtRequestId = "";
	m_strPliIds = "";
	m_strMfz_inst = "";
	m_strSearchKeyword = "";

	m_strX_Apple_amd = "";
	m_strX_Apple_amd_m = "";
	m_strX_Apple_I_md = "";
	m_strX_Apple_I_md_m = "";

	m_strns_mzf_inst = "";
	m_strmzf_in = "";
	m_stritspod = "";
	m_strwoinst = "";
	m_strwosid = "";
	m_strwosid_lite = "";
	m_strsession_store_id = "";
	m_strX_Dsid = "";
	m_strhsaccnt = "";
	m_strmt_tkn_ = "";
	m_strmz_at0_ = "";
	m_stramp = "";
	m_strampt_ = "";
	m_strmz_at_ssl_ = "";
	m_strpldfltcid = "";
	m_strxt_b_ts_ = "";
	m_strxp_ci = "";
	m_strxp_ab = "";
	m_strxt_src = "";
	m_strxp_abc = "";

	m_strxp_ab_value = "";
	m_strxp_abc_value = "";
	m_strmt_asn = "";
	m_pSignsapsetupbuffer = new char[2048];
	memset(m_pSignsapsetupbuffer,0,2048);

	m_version = "";
	m_width = 0;
	m_height = 0;

	m_pGsaService = NULL;
}
CxhNetwork::~CxhNetwork()
{
	
	if (m_pSignsapsetupbuffer)
	{
		delete []m_pSignsapsetupbuffer;
		m_pSignsapsetupbuffer = NULL;
	}
}

CString CxhNetwork::GetClientAddress()
{
	CString strAddr;
	
	LARGE_INTEGER timeRand; 
	int nRand1 = 0;
	int nRand2 = 0;

	QueryPerformanceCounter(&timeRand);
	//取整数的高2位
	nRand1 =  timeRand.LowPart%255;

	QueryPerformanceCounter(&timeRand);

	nRand2 = (timeRand.LowPart + rand())%255;

	strAddr.Format(TEXT("192.168.%d.%d"),nRand1,nRand2);
	return strAddr;
}

CString CxhNetwork::GetxpMainXAppleActionSignature(string strXPEventData)
{
	CString strXAppleActionSignature;
	char szAppleActionSignature[4096] = {0};
	int  appleActionSignLen = 0;

	USES_CONVERSION;

	//计算xp包数字签名值
	::EnterCriticalSection(&g_cs);
	//计算注册需要的X-Apple-ActionSignature值
	int bRet = pGetSignsapSetupBuf(m_pGsaService,
									(char*)m_strServerSignBuf.c_str(),
									m_strServerSignBuf.length(),
									szAppleActionSignature,
									appleActionSignLen,
									TRUE,
									(LPVOID*)strXPEventData.c_str(),
									strXPEventData.length());
	::LeaveCriticalSection(&g_cs);

	strXAppleActionSignature = A2T(szAppleActionSignature);
	
	return strXAppleActionSignature;
}

CString& CxhNetwork::GetUserAgent()
{
	return m_userAgent;
}
CString& CxhNetwork::GetitunesstoredUserAgnet()
{
	return m_itunesstoredUserAgent;
}
string& CxhNetwork::GetVersion()
{
	return m_version;
}
int CxhNetwork::GetWidth()
{
	return m_width;
}
int CxhNetwork::GetHeight()
{
	return m_height;
}

void CxhNetwork::SetUserAgent(CString strUserAgent)
{
	m_userAgent = strUserAgent;
}

void CxhNetwork::SetMmeClientInfo(CString strMmeClientInfo)
{
	m_mmeClientInfo = strMmeClientInfo;
}
void CxhNetwork::SetItunesstoredUserAgent(CString strItunesstoredUserAgent)
{
	m_itunesstoredUserAgent = strItunesstoredUserAgent;
}
void CxhNetwork::SetStoreKitUIServiceAgent(CString strStoreKitUIServiceAgent)
{
	m_StoreKitUIServiceAgent = strStoreKitUIServiceAgent;
}
void CxhNetwork::SetApplePreferencesAgent(CString strApplePreferencesAgent)
{
	m_ApplePreferencesAgent = strApplePreferencesAgent;
}

void CxhNetwork::SetComAppleAppStoredAgent(CString strComAppleAppStoredAgent)
{
	m_comAppleAppStoredAgent = strComAppleAppStoredAgent;
}
void CxhNetwork::SetModel(CString strModel)
{
	m_strModel = strModel;
}
void CxhNetwork::SetVersion(string strVersion)
{
	m_version = strVersion;
}
void CxhNetwork::SetWidth(int width)
{
	m_width = width;
}
void CxhNetwork::SetHeight(int height)
{
	m_height = height;
}

void CxhNetwork::SetInnerWidth(int InnerWidth)
{
	m_dwSignInScreenWidth = InnerWidth;
}
void CxhNetwork::SetInnerHeight(int InnerHeight)
{
	m_dwSignInScreenHeight = InnerHeight;
}
string& CxhNetwork::getns_mzf_inst()
{
	return m_strns_mzf_inst;
}
string& CxhNetwork::getmzf_in()
{
	return m_strmzf_in;
}
string& CxhNetwork::getitspod()
{
	return m_stritspod;
}
string& CxhNetwork::getwosid()
{
	return m_strwosid;
}
string& CxhNetwork::getwosid_lite()
{
	return m_strwosid_lite;
}
string& CxhNetwork::getsession_store_id()
{
	return m_strsession_store_id;
}
string& CxhNetwork::getX_Dsid()
{
	return m_strX_Dsid;
}
string& CxhNetwork::gethsaccnt()
{
	return m_strhsaccnt;
}
string& CxhNetwork::getmt_tkn_()
{
	return m_strmt_tkn_;
}
string& CxhNetwork::getmz_at0_()
{
	return m_strmz_at0_;
}
string& CxhNetwork::getamp()
{
	return m_stramp;
}
string& CxhNetwork::getampt_()
{
	return m_strampt_;
}
string& CxhNetwork::getmz_at_ssl_()
{
	return m_strmz_at_ssl_;
}
string& CxhNetwork::getpldfltcid()
{
	return m_strpldfltcid;
}
string& CxhNetwork::getxt_b_ts_()
{
	return m_strxt_b_ts_;
}
string& CxhNetwork::getxp_ci()
{
	return m_strxp_ci;
}
string& CxhNetwork::getxp_ab()
{
	return m_strxp_ab;
}
string& CxhNetwork::getxt_src()
{
	return m_strxt_src;
}

string CxhNetwork::getTimeStamp()
{
	string strTimeStamp;
	char eventtime[128] = {0};
	SYSTEMTIME sysTime;
	time_t unixTime;
	GetLocalTime(&sysTime);
	time(&unixTime);
	__int64 timestamp = unixTime*1000 + sysTime.wMilliseconds;
	_i64toa(timestamp,eventtime,10);
	strTimeStamp = eventtime;
	return strTimeStamp;
}

void CxhNetwork::parseResponseCookie(HINTERNET hOpenReq)
{
	string strResHeaderData;
	string strSubData;
	int beginPos = 0;
	int endPos = 0;
	DWORD headSize = 0;
	char* pAnsiiBuf = NULL;

	HttpQueryInfo(hOpenReq,HTTP_QUERY_RAW_HEADERS_CRLF,NULL,&headSize,NULL);

	LPVOID pHeaderBuf = VirtualAlloc(NULL,headSize,MEM_COMMIT | MEM_RESERVE,PAGE_EXECUTE_READWRITE);
	if (!pHeaderBuf)
	{
		return;
	}
	memset(pHeaderBuf,0,headSize);
	HttpQueryInfo(hOpenReq,HTTP_QUERY_RAW_HEADERS_CRLF,pHeaderBuf,&headSize,NULL);
	if (((char*)pHeaderBuf)[1] == 0)//UNICODE
	{
		int actualSize = WideCharToMultiByte(CP_ACP,0,(LPCWSTR)pHeaderBuf,-1,NULL,0,NULL,NULL);
		pAnsiiBuf = (char*)VirtualAlloc(NULL,actualSize,MEM_COMMIT | MEM_RESERVE,PAGE_EXECUTE_READWRITE);
		if (!pAnsiiBuf)
		{
			if (pHeaderBuf)
			{
				VirtualFree(pHeaderBuf,0,MEM_RELEASE);
				pHeaderBuf = NULL;
			}
			return;
		}
		memset(pAnsiiBuf,0,actualSize);
		WideCharToMultiByte(CP_ACP,0,(LPCWSTR)pHeaderBuf,headSize,pAnsiiBuf,actualSize,NULL,NULL);
		strResHeaderData = pAnsiiBuf;
	}
	else//ANSII
	{
		strResHeaderData = (char*)pHeaderBuf;
	}

	
	beginPos = strResHeaderData.find("mt-asn-");
	if (beginPos >= 0)
	{
		strSubData = strResHeaderData.substr(beginPos);
		endPos = strSubData.find(";");
		m_strmt_asn = strSubData.substr(0,endPos);
	}

	beginPos = strResHeaderData.find("ns-mzf-inst=");
	if (beginPos >= 0)
	{
		strSubData = strResHeaderData.substr(beginPos);
		endPos = strSubData.find(";");
		m_strns_mzf_inst = strSubData.substr(0,endPos);
		
	}

	beginPos = strResHeaderData.find("mzf_in=");
	if (beginPos >= 0)
	{
		strSubData = strResHeaderData.substr(beginPos);
		endPos = strSubData.find(";");
		m_strmzf_in = strSubData.substr(0,endPos);
		
	}

	beginPos = strResHeaderData.find("itspod=");
	if (beginPos >= 0)
	{
		strSubData = strResHeaderData.substr(beginPos);
		endPos = strSubData.find(";");
		m_stritspod = strSubData.substr(0,endPos);
		
	}

	beginPos = strResHeaderData.find("woinst=");
	if (beginPos >= 0)
	{
		strSubData = strResHeaderData.substr(beginPos);
		endPos = strSubData.find(";");
		m_strwoinst = strSubData.substr(0,endPos);
		
	}

	beginPos = strResHeaderData.find("wosid=");
	if (beginPos >= 0)
	{
		strSubData = strResHeaderData.substr(beginPos);
		endPos = strSubData.find(";");
		m_strwosid = strSubData.substr(0,endPos);
		
	}


	beginPos = strResHeaderData.find("wosid-lite=");
	if (beginPos >= 0)
	{
		strSubData = strResHeaderData.substr(beginPos+11);
		endPos = strSubData.find(";");
		if (endPos == 0)
		{
			strSubData = strSubData.substr(endPos+11);
			while ((endPos = strSubData.find("wosid-lite=;")) >=0 )
			{
				strSubData = strSubData.substr(endPos + 12);
			}
			beginPos = strSubData.find("wosid-lite=");
			if (beginPos >= 0)
			{
				strSubData = strSubData.substr(beginPos);
				endPos = strSubData.find(";");
				m_strwosid_lite = strSubData.substr(0,endPos);

			}
		}
		else
		{
			m_strwosid_lite = "wosid-lite="; 
			m_strwosid_lite += strSubData.substr(0,endPos);
		}
		
	}

	beginPos = strResHeaderData.find("session-store-id=");
	if (beginPos >= 0)
	{
		strSubData = strResHeaderData.substr(beginPos);
		endPos = strSubData.find(";");
		m_strsession_store_id = strSubData.substr(0,endPos);
		
	}

	beginPos = strResHeaderData.find("X-Dsid=");
	if (beginPos >= 0)
	{
		strSubData = strResHeaderData.substr(beginPos);
		endPos = strSubData.find(";");
		m_strX_Dsid = strSubData.substr(0,endPos);
		
	}

	beginPos = strResHeaderData.find("hsaccnt=");
	if (beginPos >= 0)
	{
		strSubData = strResHeaderData.substr(beginPos);
		endPos = strSubData.find(";");
		m_strhsaccnt = strSubData.substr(0,endPos);
		
	}

	beginPos = strResHeaderData.find("mt-tkn-");
	if (beginPos >= 0)
	{
		strSubData = strResHeaderData.substr(beginPos);
		endPos = strSubData.find(";");
		m_strmt_tkn_ = strSubData.substr(0,endPos);
		
	}

	beginPos = strResHeaderData.find("mz_at0-");
	if (beginPos >= 0)
	{
		strSubData = strResHeaderData.substr(beginPos);
		endPos = strSubData.find(";");
		m_strmz_at0_ = strSubData.substr(0,endPos);
		
	}

	beginPos = strResHeaderData.find("amp=");
	if (beginPos >= 0)
	{
		strSubData = strResHeaderData.substr(beginPos);
		endPos = strSubData.find(";");
		m_stramp = strSubData.substr(0,endPos);
		
	}

	beginPos = strResHeaderData.find("ampt-");
	if (beginPos >= 0)
	{
		strSubData = strResHeaderData.substr(beginPos);
		endPos = strSubData.find(";");
		m_strampt_ = strSubData.substr(0,endPos);
		
	}

	beginPos = strResHeaderData.find("mz_at_ssl-");
	if (beginPos >= 0)
	{
		strSubData = strResHeaderData.substr(beginPos);
		endPos = strSubData.find(";");
		m_strmz_at_ssl_ = strSubData.substr(0,endPos);
		
	}

	beginPos = strResHeaderData.find("pldfltcid=");
	if (beginPos >= 0)
	{
		strSubData = strResHeaderData.substr(beginPos);
		endPos = strSubData.find(";");
		m_strpldfltcid = strSubData.substr(0,endPos);
		
	}

	beginPos = strResHeaderData.find("xt-b-ts-");
	if (beginPos >= 0)
	{
		strSubData = strResHeaderData.substr(beginPos);
		endPos = strSubData.find(";");
		m_strxt_b_ts_ = strSubData.substr(0,endPos);
		
	}

	beginPos = strResHeaderData.find("xp_ci=");
	if (beginPos >= 0)
	{
		strSubData = strResHeaderData.substr(beginPos);
		endPos = strSubData.find(";");
		m_strxp_ci = strSubData.substr(0,endPos);
		
	}

	beginPos = strResHeaderData.find("xp_ab=");
	if (beginPos >= 0)
	{
		strSubData = strResHeaderData.substr(beginPos);
		endPos = strSubData.find(";");
		m_strxp_ab = strSubData.substr(0,endPos);
		
	}

	beginPos = strResHeaderData.find("xp_abc=");
	if (beginPos >= 0)
	{
		strSubData = strResHeaderData.substr(beginPos);
		endPos = strSubData.find(";");
		m_strxp_abc = strSubData.substr(0,endPos);
	}

	beginPos = strResHeaderData.find("xt-src=");
	if (beginPos >= 0)
	{
		strSubData = strResHeaderData.substr(beginPos);
		endPos = strSubData.find(";");
		m_strxt_src = strSubData.substr(0,endPos);
		
	}

	if (pAnsiiBuf)
	{
		VirtualFree(pAnsiiBuf,0,MEM_RELEASE);
		pAnsiiBuf = NULL;
	}

	if (pHeaderBuf)
	{
		VirtualFree(pHeaderBuf,0,MEM_RELEASE);
		pHeaderBuf = NULL;
	}

	return;
}

CString CxhNetwork::getSendCookie()
{
	CString strCookie;
	string strCookieData;
	USES_CONVERSION;
	if (!m_strwosid.empty())
	{
		strCookieData = m_strwosid;
		strCookieData += "; ";
	}
	if (!m_strwoinst.empty())
	{
		strCookieData += m_strwoinst;
		strCookieData += "; ";
	}
	if (!m_strns_mzf_inst.empty())
	{
		strCookieData += m_strns_mzf_inst;
		strCookieData += "; ";
	}
	
	if (!m_strsession_store_id.empty())
	{
		strCookieData += m_strsession_store_id;
		strCookieData += "; ";
	}
	if (!m_stritspod.empty())
	{
		strCookieData += m_stritspod;
		strCookieData += "; ";
	}
	if (!m_strwosid_lite.empty())
	{
		strCookieData += m_strwosid_lite;
		strCookieData += "; ";
	}
	if (!m_strX_Dsid.empty())
	{
		strCookieData += m_strX_Dsid;
		strCookieData += "; ";
	}
	if (!m_strhsaccnt.empty())
	{
		strCookieData += m_strhsaccnt;
		strCookieData += "; ";
	}
	if (!m_strmt_tkn_.empty())
	{
		strCookieData += m_strmt_tkn_;
		strCookieData += "; ";
	}
	if (!m_strmz_at0_.empty())
	{
		strCookieData += m_strmz_at0_;
		strCookieData += "; ";
	}
	if (!m_stramp.empty())
	{
		strCookieData += m_stramp;
		strCookieData += "; ";
	}
	if (!m_strampt_.empty())
	{
		strCookieData += m_strampt_;
		strCookieData += "; ";
	}
	if (!m_strmz_at_ssl_.empty())
	{
		strCookieData += m_strmz_at_ssl_;
		strCookieData += "; ";
	}
	if (!m_strpldfltcid.empty())
	{
		strCookieData += m_strpldfltcid;
		strCookieData += "; ";
	}
	if (!m_strxt_b_ts_.empty())
	{
		strCookieData += m_strxt_b_ts_;
		strCookieData += "; ";
	}
	if (!m_strxp_ci.empty())
	{
		strCookieData += m_strxp_ci;
		strCookieData += "; ";
	}
	if (!m_strxp_ab.empty())
	{
		strCookieData += m_strxp_ab;
		strCookieData += "; ";
	}
	if (!m_strxp_abc.empty())
	{
		strCookieData += m_strxp_abc;
		strCookieData += "; ";
	}
	if (!m_strxt_src.empty())
	{
		strCookieData += m_strxt_src;
		strCookieData += "; ";
	}
	if (!m_strmt_asn.empty())
	{
		strCookieData += m_strmt_asn;
		strCookieData += "; ";
	}
	//结束的最有一个字段
	if (!m_strmzf_in.empty())
	{
		strCookieData += m_strmzf_in;
	}

	if (!strCookieData.empty())
	{
		strCookie = A2T(strCookieData.c_str());
	}

	return strCookie;
		

}

string& CxhNetwork::getUdid()
{
	return m_strUdid;
}
void CxhNetwork::setUdid(string strUdid)
{
	m_strUdid = strUdid;
}

string& CxhNetwork::getPasswordToken()
{
	return m_strPasswordToken;
}
void CxhNetwork::setPasswordToken(string strPasswordToken)
{
	m_strPasswordToken = strPasswordToken;
}
string& CxhNetwork::getDsid()
{
	return m_strDsid;
}
void CxhNetwork::setDsid(string strDsid)
{
	m_strDsid = strDsid;
}

string& CxhNetwork::getCreditDisplay()
{
	return m_strCreditDisplay;
}

void CxhNetwork::setCreditDisplay(string strCreditDisplay)
{
	m_strCreditDisplay = strCreditDisplay;
}

string& CxhNetwork::getAppleId()
{
	return m_strAppleId;
}
void CxhNetwork::setAppleId(string strAppleId)
{
	m_strAppleId = strAppleId;
	m_ViewPage.m_appleId = strAppleId;
}
string& CxhNetwork::getPassword()
{
	return m_strPassword;
}
void CxhNetwork::setPassword(string strPassword)
{
	m_strPassword = strPassword;
	m_ViewPage.m_password = strPassword;
}
string& CxhNetwork::getAdsid()
{
	return m_strAdsid;
}
void CxhNetwork::setAdsid(string strAdsid)
{
	m_strAdsid = strAdsid;
}

string& CxhNetwork::getClientId()
{
	return m_strClientId;
}
void CxhNetwork::setClientId(string strClientId,string strClientIdValue)
{
	m_strxp_ci = strClientId;
	m_strClientId = strClientIdValue;
}

void CxhNetwork::setxpab_and_xpabc(string strxpab,string strxpabc,string strxpab_value,string strxpabc_value)
{
	m_strxp_ab = strxpab;
	m_strxp_abc = strxpabc;
	m_strxp_ab_value = strxpab_value;
	m_strxp_abc_value = strxpabc_value;
}

string& CxhNetwork::getAppExtVrsId()
{
	return m_strAppExtVrsId;
}

void CxhNetwork::setAppExtVrsId(string strAppExtVrsId)
{
	m_strAppExtVrsId = strAppExtVrsId;
}

string& CxhNetwork::getMtRequestId()
{
	return m_strMtRequestId;
}
void CxhNetwork::setMtRequestId(string strMtRequestId)
{
	m_strMtRequestId = strMtRequestId;
}

string& CxhNetwork::getPliIds()
{
	return m_strPliIds;
}
void CxhNetwork::setPliIds(string strPliIds)
{
	m_strPliIds = strPliIds;
}

string& CxhNetwork::getMfz_inst()
{
	return m_strMfz_inst;
}
void CxhNetwork::setMfz_inst(string strMfz_inst)
{
	m_strMfz_inst = strMfz_inst;
}

string& CxhNetwork::getX_Apple_amd()
{
	return m_strX_Apple_amd ;
}
void CxhNetwork::setX_Apple_amd(string strX_Apple_amd)
{
	m_strX_Apple_amd = strX_Apple_amd;
}
string& CxhNetwork::getX_Apple_amd_m()
{
	return m_strX_Apple_amd_m;
}
void CxhNetwork::setX_Apple_amd_m(string strX_Apple_amd_m)
{
	m_strX_Apple_amd_m = strX_Apple_amd_m;
}
string& CxhNetwork::getX_Apple_I_md()
{
	return m_strX_Apple_I_md;
}
void CxhNetwork::setX_Apple_I_md(string strX_Apple_I_md)
{
	m_strX_Apple_I_md = strX_Apple_I_md;
}
string& CxhNetwork::getX_Apple_I_md_m()
{
	return m_strX_Apple_I_md_m;
}
void CxhNetwork::setX_Apple_I_md_m(string strX_Apple_I_md_m)
{
	m_strX_Apple_I_md_m = strX_Apple_I_md_m;
}

string& CxhNetwork::getServerSignBuf()
{
	return m_strServerSignBuf;
}
void CxhNetwork::setServerSignBuf(string strServerSignBuf)
{
	m_strServerSignBuf = strServerSignBuf;
}

string& CxhNetwork::getXAppleSignBuf()
{
	return m_strXAppleSignBuf;
}

void CxhNetwork::setXAppleSignBuf(string strXAppleSignBuf)
{
	m_strXAppleSignBuf = strXAppleSignBuf;
}