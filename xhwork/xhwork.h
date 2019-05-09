#ifndef INTERNET_WORK_API
#define INTERNET_WORK_API extern "C" _declspec(dllimport)
#endif
#include "AccountInfo.h"
#include "secdata.h"
#include "statusData.h"
#include "viewpage.h"
#include "CaptchaPageInfo.h"
#include "CxhLog.h"
#include "mem_lock.h"
#include <iostream>
#include <string>
using namespace std;

INTERNET_WORK_API VOID xhInitCriticalSection();
INTERNET_WORK_API VOID xhDeleteCriticalSection();
INTERNET_WORK_API LPVOID xhCreateNetworkObject();
INTERNET_WORK_API VOID xhReleaseNewtworkObject(LPVOID lpNetwork);

INTERNET_WORK_API int xhGSA_Apple_Thread_NetworkVerifier(IN TCHAR* lpUdid,TCHAR* lpProxy,int netType);

INTERNET_WORK_API int xhSetUAInfo(IN LPVOID lpNetwork,PMODELINFO pModInfo);

//gsa.apple.com网络验证设备授权机制接口
INTERNET_WORK_API int xhGSA_Apple_NetworkVerifier(IN LPVOID lpNetwork,IN LPVOID lpUdid,PMODELINFO pModInfo,PDEVICEINFO pDeviceInfo,TCHAR* lpProxy,int netType);


//登录App Store接口
INTERNET_WORK_API int xhLoginAppStore(IN LPVOID lpNetwork,
									   IN LPVOID lpUserID,
									   IN LPVOID lpPassword,
									   IN LPTSTR lpwFatherAndMother,
									   IN LPTSTR lpwTeacher,
									   IN LPTSTR lpwBook,
									   IN PDEVICEINFO pDeviceInfo,
									   OUT LPVOID lpPasswordToken,
									   OUT LPVOID lpDsid,
									   TCHAR* lpProxy,
									   int netType);

//设备授权
INTERNET_WORK_API int xhMachineAuthorize(IN LPVOID lpNetwork,IN LPVOID lpPasswordToken,IN LPVOID lpDsid);

//取消设备授权
INTERNET_WORK_API int xhMachineDeAuthorize(IN LPVOID lpNetwork);

//下载应用接口
INTERNET_WORK_API int xhDownloadApp(IN LPVOID lpNetwork,
									 IN LPVOID lpPasswordToken,
									 IN LPVOID lpDsid,
									 IN LPVOID lpSaleId,
									 IN LPVOID lpAppPrice,
									 BOOL bSendReport,
									 BOOL bPaidApp);

//评论应用接口
INTERNET_WORK_API int xhCommentApp(IN LPVOID lpNetwork,
									IN LPVOID lpPasswordToken,
									IN LPVOID lpDsid,
									IN LPVOID lpSaleId,
									IN int rating,
									IN LPTSTR lpwNickname,
									IN LPTSTR lpwTitle,
									IN LPTSTR lpwBody);

INTERNET_WORK_API int xhGetAppExtVrsIdValue(IN LPVOID lpNetwork,
											 IN LPVOID lpPasswordToken,
											 IN LPVOID lpDsid,
											 IN LPVOID lpTargetUrl);


//搜索热门关键字接口
INTERNET_WORK_API int xhSearchKeyHotWord(IN LPVOID lpNetwork,
										  IN LPTSTR strwKeyword,
										  IN LPVOID lpSaleId,
										  IN LPVOID lpDsid,
										  IN LPVOID lpPasswordToken,
										  IN LPVOID lpTargetUrl);

//购买确认和app激活接口
INTERNET_WORK_API int xhBuyConfirmActiveApp(IN LPVOID lpNetwork,
											 IN LPVOID lpPasswordToken,
											 IN LPVOID lpSaleId,
											 IN LPVOID lpDsid,
											 IN LPVOID lpTargetUrl);

//只搜索关键词
INTERNET_WORK_API int xhOnlySearchKeyword(IN LPVOID lpNetwork,IN LPTSTR strwKeyword);


//注册账号调用接口，得到需要的x-apple-signature的值
INTERNET_WORK_API int xhRegisterAppleId(IN LPVOID lpNetwork,TAGREGISTERINFO& tagRegisterInfo);

class CxhNetwork
{
public:
	CxhNetwork();
	~CxhNetwork();
public:
	BOOL CreateNetGSaServiceObject();
	VOID ReleaseNetGsaServiceObject();
	BOOL GSA_Apple_NetworkVerifier(IN LPVOID lpUdid,PMODELINFO pModInfo,PDEVICEINFO pDeviceInfo,TCHAR* lpProxy,int netType);
	int LoginAppStore(IN LPVOID lpUserID,
					   IN LPVOID lpPassword,
					   IN LPTSTR lpwFatherAndMother,
					   IN LPTSTR lpwTeacher,
					   IN LPTSTR lpwBook,
					   IN PDEVICEINFO pDeviceInfo,
					   OUT LPVOID lpPasswordToken,
					   OUT LPVOID lpDsid,
					   TCHAR* lpProxy,
					   int netType);

	int MachineAuthorize(IN LPVOID lpPasswordToken,IN LPVOID lpDsid);
	int MachineDeAuthorize();
	int DownloadApp(IN LPVOID lpPasswordToken,
					 IN LPVOID lpDsid,
					 IN LPVOID lpSaleId,
					 IN LPVOID lpAppPrice,
					 BOOL bSendReport,
					 BOOL bPaidApp);

	int CommentApp(IN LPVOID lpPasswordToken,
					IN LPVOID lpDsid,
					IN LPVOID lpSaleId,
					IN int rating,
					IN LPTSTR lpwNickname,
					IN LPTSTR lpwTitle,
					IN LPTSTR lpwBody);

	int GetAppExtVrsIdValue(IN LPVOID lpPasswordToken,
							 IN LPVOID lpDsid,
							 IN LPVOID lpTargetUrl);

	int SearchKeyHotWord(IN LPTSTR strwKeyword,
						  IN LPVOID lpSaleId,
						  IN LPVOID lpDsid,
						  IN LPVOID lpPasswordToken,
						  IN LPVOID lpTargetUrl);

	int OnlySearchKeyword(IN LPTSTR strwKeyword);

	int BuyConfirmActiveApp(IN LPVOID lpPasswordToken,
							 IN LPVOID lpSaleId,
							 IN LPVOID lpDsid,
							 IN LPVOID lpTargetUrl);

	int RegisterAppleId(TAGREGISTERINFO& tagRegisterInfo);

public:
	string MakeRandomString( int nMinNum, int nMaxNum );
	int GetRandomLength( int nMinLen, int nMaxLen );
	string GenerateUdid();
	string FormatString(string strData);
	BOOL SendfuseNoCarrierBundleSubscription();
	BOOL SendSubscriptionStatusSrv();
	int loginConnection(char* lpKbsyncValue);
	BOOL InitSignSapSetupCert(CString strUrl,
							  CString strVerb,
							  CString strHostName,
							  TCHAR* lpX_Apple_I_md_m,
							  TCHAR* lpX_Apple_I_md); 
	BOOL ResponseServer(CString strUrl,
						CString strVerb,
						CString strHostName,
						TCHAR* lpX_Apple_I_md_m,
						TCHAR* lpX_Apple_I_md,
						char* lpSignSapsetupBuffer,
						char* lpOutSignSapSetupBuffer,
						int& outSignDataLen);
	BOOL GetPasswordTokenDsid(char* lpBuffer);
	BOOL GetSongIdUrlPathdownloadId(char* lpBuffer,char* lpSongId,CString& lpDownloadUrlPath,CString& lpDownloadKey,char* lpDownloadId,char* lpPliIds);
	BOOL ReadDataFromServer(HINTERNET hOpenReq,LPVOID lpBuffer,LPDWORD lpdwSize);
	int ResponseServerWithSignature(CString strUrl,
		                             CString strAgent,
									 CString strVerb,
									 CString strHostName,
									 char* lpDsid,
									 char* strUserXmlData,
									 int dataLength,
									 char* lpSignBuf,
									 char* lpXAppleAMdData);
	BOOL SendDownloadDoneSuccessNotify(CString strUrl,
									   CString strArg,
									   char* pSongId,
									   char* pDownloadId,
									   CString strPwdToken,
									   CString strdsid,
									   CString strSaleId,
									   CString strRefUrl,
									   TCHAR* lpX_Apple_I_md_m,
									   TCHAR* lpX_Apple_I_md);
	BOOL downloadAppFile(CString strDownloadUrlPath,CString strDownloadKey,CString strUrl);
	BOOL getpliIdsValue(char* lpBuffer,char* lpPiIds);
	int onlyBuyProduct(CString strBuyUrl,
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
					   char* lpXAppleAmdData);
	BOOL xpReportAppBuy(char* lpSaleId,char* lpDsid);
	BOOL reLoginAppStoreForDownload(char* lpAppleId,char* lpPassword,char* lpDsid,TCHAR* lpProxy,int netType);
	int NeedVerifyData(char* lpSrcData);
	BOOL RegisterSuccess(char* lpDsid,char* lpSignature,char* lpPostData,int dataLen);
	void generateSerialNumber(char* lpSerialNumber);
	int ExtractErrorInfo(char* lpSrcData);
	void ExtractKeybagData(char* lpSrcData);
	void getEventTime(char* lpEventtime);
	void generateMzRequestId(char* lpEventtime,char* lpMzRequestId);
	void generateVid(char* lpVid);
	void generateUdid(char* lpUdid);
	int preLoginGsaServicesVerifier(char* lpUserId,char* lpUserPassword,char* lpUdid,char* lpXApple_I_md,char* lpXApple_I_md_m);

	void Sendamp_clientPerfData();
	void SendBuyAndConfirmData();
	void SendOpenAppData();
	void GenerateXApple_I_MDandMDMValue();
	string GetStringSha1(string str);

protected:
	string m_strSHA1;
	string m_strAppUrl;
	string m_strUdid;
	string m_strPasswordToken;
	string m_strDsid;
	string m_strSaleId;
	string m_strPageSearchUrl;
	string m_strCreditDisplay;
	string m_strAppleId;
	string m_strPassword;
	string m_strAdsid;
	string m_strClientId;
	string m_strAppExtVrsId;
	string m_strMtRequestId;
	string m_strPliIds;
	string m_strMfz_inst;
	string m_strX_Apple_amd;
	string m_strX_Apple_amd_m;
	string m_strX_Apple_I_md;
	string m_strX_Apple_I_md_m;

	string m_strSearchKeyword;
	string m_strServerSignBuf;
	string m_strXAppleSignBuf;
	string m_strSignsapsetup;
	char*  m_pSignsapsetupbuffer;

protected:
	CString m_strXPVerifyHostName;
	CString m_strItmsAppsHostName;
	CString m_strSerialNo;
	CString m_strMEID;
	CString m_strIMEI;
	CString m_strCommentCookie;
	CString m_strAuthUrl;
	CString m_strAuthHostName;
	VECMATCHSECINFO m_vecMatchSecInfo;


protected:
	string m_strns_mzf_inst;
	string m_strmzf_in;
	string m_stritspod;
	string m_strwoinst;
	string m_strwosid;
	string m_strwosid_lite;
	string m_strsession_store_id;
	string m_strX_Dsid;
	string m_strhsaccnt;
	string m_strmt_tkn_;
	string m_strmz_at0_;
	string m_stramp;
	string m_strampt_;
	string m_strmz_at_ssl_;
	string m_strpldfltcid;
	string m_strxt_b_ts_;
	string m_strxp_ci;
	string m_strxp_ab;
	string m_strxt_src;
	string m_strxp_abc;
	string m_strxp_ab_value;
	string m_strxp_abc_value;
	string m_strmt_asn;
	string m_FormatUserAgent;

protected:
	CString m_userAgent;
	CString m_mmeClientInfo;
    CString m_itunesstoredUserAgent;
	CString m_StoreKitUIServiceAgent;
	CString m_ApplePreferencesAgent;
	CString m_comAppleAppStoredAgent;
	CString m_strModel;
	string m_version;
	int m_width;
	int m_height;
	DWORD m_dwSignInScreenHeight;
	DWORD m_dwSignInScreenWidth;
	CStringA m_strKeybag;

protected:
	LPVOID             m_pGsaService;
	CViewPage          m_ViewPage;
	CCaptchaPageInfo   m_CaptchaPage;
	CxhLog             m_log;

public:
	BOOL SendxpRegister(char* lpDsid,CString strUserAgent);
	CString GetClientAddress();
	CString GetxpMainXAppleActionSignature(string strXPEventData);
	CString& GetUserAgent();
	CString& GetitunesstoredUserAgnet();
	string& GetVersion();
	int GetWidth();
	int GetHeight();

	void SetUserAgent(CString strUserAgent);
	void SetMmeClientInfo(CString strMmeClientInfo);
	void SetItunesstoredUserAgent(CString strItunesstoredUserAgent);
	void SetStoreKitUIServiceAgent(CString strStoreKitUIServiceAgent);
	void SetApplePreferencesAgent(CString strApplePreferencesAgent);
	void SetComAppleAppStoredAgent(CString strComAppleAppStoredAgent);
	void SetModel(CString strModel);
	void SetVersion(string strVersion);
	void SetWidth(int width);
	void SetHeight(int height);
	void SetInnerWidth(int InnerWidth);
	void SetInnerHeight(int InnerHeight);



public:
	string& getns_mzf_inst();
	string& getmzf_in();
	string& getitspod();
	string& getwosid();
	string& getwosid_lite();
	string& getsession_store_id();
	string& getX_Dsid();
	string& gethsaccnt();
	string& getmt_tkn_();
	string& getmz_at0_();
	string& getamp();
	string& getampt_();
	string& getmz_at_ssl_();
	string& getpldfltcid();
	string& getxt_b_ts_();
	string& getxp_ci();
	string& getxp_ab();
	string& getxt_src();
	string  getLaunchCorrelationKey();
	string  getTimeStamp();
public:
	void parseResponseCookie(HINTERNET hOpenReq);
	CString getSendCookie();
public:
	QUESTIONTOANSWER m_secQuesAnswerMap;
public:
	string& getUdid();
	void setUdid(string strUdid);
	string& getPasswordToken();
	void setPasswordToken(string strPasswordToken);
	string& getDsid();
	void setDsid(string strDsid);
	string& getCreditDisplay();
	void setCreditDisplay(string strCreditDisplay);
	string& getAppleId();
	void setAppleId(string strAppleId);
	string& getPassword();
	void setPassword(string strPassword);
	string& getAdsid();
	void setAdsid(string strAdsid);
	string& getClientId();
	void setClientId(string strClientId,string strClientIdValue);
	void setxpab_and_xpabc(string strxpab,string strxpabc,string strxpab_value,string strxpabc_value);
	string& getAppExtVrsId();
	void setAppExtVrsId(string strAppExtVrsId);
	string& getMtRequestId();
	void setMtRequestId(string strMtRequestId);
	string& getPliIds();
	void setPliIds(string strPliIds);
	string& getMfz_inst();
	void setMfz_inst(string strMfz_inst);
	string& getX_Apple_amd();
	void setX_Apple_amd(string strX_Apple_amd);
	string& getX_Apple_amd_m();
	void setX_Apple_amd_m(string strX_Apple_amd_m);
	string& getX_Apple_I_md();
	void setX_Apple_I_md(string strX_Apple_I_md);
	string& getX_Apple_I_md_m();
	void setX_Apple_I_md_m(string strX_Apple_I_md_m);

	string& getServerSignBuf();
	void setServerSignBuf(string strServerSignBuf);
	string& getXAppleSignBuf();
	void setXAppleSignBuf(string strXAppleSignBuf);
};