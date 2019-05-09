#ifndef __VIEWPAGE_H
#define __VIEWPAGE_H
#include <string>
#include <vector>
#include <time.h>
#include "secdata.h"
#include "statusData.h"
#include "mem_lock.h"

#include <shlwapi.h>
#pragma comment(lib,"Shlwapi.lib")
using namespace std;

typedef struct _a2kResponseData
{
	CString strI;
	CString strC;
	CString strS;
	CString strB;
}A2KRESPONSEPACKAGE,*PA2KRESPONSEPACKAGE;

typedef struct _m1ResponseData
{
	CString strCPD;
	CString strM2;
	CString strNP;
}M1RESPONSEPACKAGE,*PM1RESPONSEPACKAGE;


class CViewPage
{
	enum appSortData
	{
		FIRST_APP = 1,
		SECOND_APP,
		PREV_APP,
		NEXT_APP
	};
public:
	CViewPage();
	~CViewPage();
public:
	BOOL appBootStrap(CString strUserAgent,
					char* lpPasswordToken,
					char* lpDsid,
					CString strCookie,
					TCHAR* lpX_Apple_I_md_m,
					TCHAR* lpX_Apple_I_md,
					TCHAR* lpProxy,int netType);
	BOOL searchHotWordHint(CString strUserAgent,
							LPTSTR pSearchHotWord,
						   char* lpPasswordToken,
						   char* lpDsid,
						   CString strCookie,
						   TCHAR* lpX_Apple_I_md_m,
						   TCHAR* lpX_Apple_I_md,
						   TCHAR* lpProxy,
						   int netType);

	BOOL searchHotWordSubmit(CString strUserAgent,
							LPTSTR pSearchHotWord,
							 char* lpSaleId,
							 char* lpDsid,
							 char* lpPasswordToken,
							 char* lpPageSearchUrl,
							 CString strCookie,
							 TCHAR* lpX_Apple_I_md_m,
							 TCHAR* lpX_Apple_I_md,
							 TCHAR* lpProxy,
							 int netType);

	BOOL showInfoAccountSummary(CString strUserAgent,
								 char* lpDsid,
								 char* lpPasswordToken,
								 CString strCookie,
								 TCHAR* lpX_Apple_I_md_m,
								 TCHAR* lpX_Apple_I_md,
								 TCHAR* lpProxy,
								 int netType);

	BOOL showInfoAccountFieldsSrv(CString strUserAgent,
								 char* lpDsid,
								 char* lpPasswordToken,
								 CString strCookie,
								 TCHAR* lpX_Apple_I_md_m,
								 TCHAR* lpX_Apple_I_md,
								 TCHAR* lpProxy,
								 int netType);

	BOOL openViewAppMain(CString strUserAgent,
						char* strPasswordToken,
						char* lpDsid,
						CString strCookie,
						TCHAR* lpX_Apple_I_md_m,
						TCHAR* lpX_Apple_I_md,
						TCHAR* lpProxy,
						int netType);

	BOOL getTargetIdWebData(CString strUserAgent,
							LPTSTR pSearchHotWord,
							char* lpTargetUrl,
							char* lpSaleId,
							char* lpDsid,
							char* lpAppExtVrsId,
							char* lpPasswordToken,
							CString strCookie,
							TCHAR* lpX_Apple_I_md_m,
							TCHAR* lpX_Apple_I_md,
							TCHAR* lpProxy,
							int netType);
	
	BOOL reLoginAppstoreXpReport(CString strUserAgent,
								CString strXpReportUrl,
								CString strXpReportHostName,
								TCHAR* lpDsid,
								CString strCookie,
								TCHAR* lpX_Apple_I_md_m,
								TCHAR* lpX_Apple_I_md,
								TCHAR* lpProxy,
								int netType);


	BOOL itmsAppsReport(CString strUserAgent,
					   CString strBuyUrl,
					   CString strItmsAppsHostName,
					   TCHAR* lpDsid,					  
					   TCHAR* lpAgreeAction,
					   TCHAR* lpAgreeKey,
					   TCHAR* lpTermsId,
					   TCHAR* lpWosid_lite,
					   CString strCookie,
					   TCHAR* lpProxy,
					   int netType);

	BOOL getSecurityVerifyWebReport(CString strUserAgent,
									 CString strBuyUrl,
									 CString strItmsAppsHostName,
									 TCHAR* lpDsid,
									 TCHAR* lpSecurityAction,
									 TCHAR* lpSubmitKey,
									 SECQUESTIONTONAME& securityMap,
									 CString strCookie,
									 TCHAR* lpProxy,
									 int netType);

	BOOL securityAnswerReport(CString strUserAgent,
							CString strBuyUrl,
							CString strSecurityActinHostName,
							CString strRefererUrl,
							TCHAR* lpDsid,
							TCHAR* lpSubmitKey,
							NAMETOANSWER* pNameToAnswer,
							char* lpXAppleMdData,
							CString strCookie,
							TCHAR* lpProxy,
							int netType);

	BOOL ProvisioningReport(CString strUserAgent,
							CString strBuyUrl,
							CString strHostName,
							TCHAR* lpPasswordToken,
							TCHAR* lpDsid,
							CString strCookie,
							TCHAR* lpX_Apple_I_md_m,
							TCHAR* lpX_Apple_I_md,
							TCHAR* lpX_Apple_amd_m,
							TCHAR* lpX_Apple_amd,
							char* lpSendData,
							int sendLength,
							char* lpSettingInfo,
							char* lpTransportKey,
							TCHAR* lpProxy,
							int netType);
	static BOOL startMachineProvisionReport(TCHAR* lpUdid,
									char* lpSpimData,
									CString strCookie,
									CString strMME_Client_Info,
									CString strSerialNo,
									TCHAR* lpProxy,
									int netType);

	static BOOL finishMachineProvisionReport(TCHAR* lpUdid,
									char* lpCpimData,
									char* lpTKData,
									char* lpPtmData,
									CString strCookie,
									CString strMME_Client_Info,
									CString strSerialNo,
									TCHAR* lpProxy,
									int netType);
	BOOL getPreAndNextAppName(CString strUserAgent,char* lpAppId,int appLocation);
	int loginGsaServices2(char* lpSendData,int sendDataLen,CString strCookie,BOOL bA2kPackage,CString strMME_Client_Info,TCHAR* lpProxy,int netType);

	BOOL agreeDataReport(CString strUserAgent,
						CString strBuyUrl,
						CString strActionHostName,
						CString strRefererUrl,
						TCHAR* lpDsid,
						TCHAR* lpAgreeKey,
						TCHAR* lpTermsId,
					    TCHAR* lpWosid_lite,
						CString strCookie,
						TCHAR* lpProxy,
						int netType);

	BOOL secureTermsPageReport(CString strUserAgent,
									  CString strXpUrl,
									  CString strRefererUrl,
									  TCHAR* lpDsid,
									  CString strCookie,
									  TCHAR* lpProxy,
									  int netType);
	BOOL registerXpReport(CString strUserAgent,
								char* lpDsid,
								string &strClientId,
								string &strXpab,
								string &strXpabc,
								string &strClientId_value,
								string &strXpab_value,
								string &strXpabc_value,
								CString strCookie,
								BOOL bReadData,
								TCHAR* lpX_Apple_I_md_m,
								TCHAR* lpX_Apple_I_md,
								TCHAR* lpProxy,
								int netType);

	BOOL xpReportTargetAppMain(CString strUserAgent,
								char* lpSendData,
								char* lpPasswordToken,
								char* lpDsid,
								CString strCookie,
								CString strXAppleSignature,
								TCHAR* lpX_Apple_I_md_m,
								TCHAR* lpX_Apple_I_md,
								TCHAR* lpProxy,
								int netType);

	BOOL xpAmpClientPerf(CString strUserAgent,
							char* lpSendData,
							char* lpDsid,
							CString strCookie,
							CString strXAppleSignature,
							TCHAR* lpX_Apple_I_md_m,
							TCHAR* lpX_Apple_I_md,
							TCHAR* lpProxy,
							int netType);

	BOOL xpAppBuyReport(CString strUserAgent, char* lpSaleId,TCHAR* lpProxy,int netType);

	BOOL SendGsasServicesPostData(CString strUserAgent,
									CString strMmeClientInfo,
									char* lpSendData,
									CString strXAppleHBToken,
									CString strUdid,
									CString strSerialNo,
									CString strCookie,
									TCHAR* lpX_Apple_I_md_m,
									TCHAR* lpX_Apple_I_md,
									TCHAR* lpProxy,
									int netType);

	BOOL sendBuyButtonMetaData(CString strUserAgent,
								char* lpSendContext,
								char* lpDsid,
								char* lpToken,
								CString strCookie,
								TCHAR* lpX_Apple_I_md_m,
								TCHAR* lpX_Apple_I_md,
								TCHAR* lpProxy,
								int netType);

	vector<string>& GetIdsList();

	int gzcompress22(unsigned char* data,unsigned long ndata,unsigned char* zdata,unsigned nzdata);

	BOOL parseViewAppMainJsonData(char* lpSrcData);

	BOOL parseJsonData(char* lpSrcData,char* lpSaleId,char* lpAppExtVrsId);

	BOOL parseRreAppJsonData(char* lpSrcData,char* lpSaleId,int appLocation);

	BOOL parseJsonForLookupCaller(char* lpSrcData,char* lpSaleId);

	BOOL parsejsonAppIdSortData(char* lpSrcData,char* lpSaleId);

	BOOL sendCheckAppIdPageDataToServer(CString strUserAgent,
												char* lpDsid,
												char* lpToken,
												char* lpMfz_inst,
												CString strCookie,
												TCHAR* lpX_Apple_I_md_m,
												TCHAR* lpX_Apple_I_md,
												TCHAR* lpProxy,int netType);

	BOOL getConnectCookie(char* lpDsid);

	BOOL parseCookieFile(TCHAR* strCookieFile,char* lpMz_at,char* lpMz_at_ssl,BOOL *lpbgetData);

	BOOL ReadStringToUnicode(CString &str,int type);

	int CharToUnicode(char *pchIn,int type,CString *pstrOut);

	static int ConvertUtf8ToGBK(char* pChangeStr,int nLength);

	static BOOL decodeServerData(HINTERNET hOpenReq,LPVOID lpBuffer,LPDWORD lpdwSize);

	int checkPasswordIsCorrect(char* lpSrcData);

	BOOL parseAgreeWebData(char* lpDecodeData,
									TCHAR* lpAgreeWebActionHostName,
									TCHAR* lpAgreeKey,
									TCHAR* lpTermsId,
									TCHAR* lpWosid_lite);

	BOOL parseSecurityQustionData(char* lpDecodeData,TCHAR* lpSecurityActionHostName,TCHAR* lpSubmitKey,SECQUESTIONTONAME& securityMap);

	void getXAppleAMdDataValue(HINTERNET hOpenReq,char* lpXAppleAMdData);

	void getXAppleMdDataValue(HINTERNET hOpenReq,char* lpXAppleMdData);

	void getxpClientIdValue(HINTERNET hOpenReq,
							string &strXpClientId,
							string &strXpab,
							string &strXpabc,
							string &strXpClientId_value,
							string &strXpab_value,
							string &strXpabc_value);

	static void getSpimValue(char* lpDecodeData,char* lpSpimData);

	static void getTKandPTMValue(char* lpDecodeData,char* lpTKData,char* lpPtmData);

	void getSettingInfoAndtransportKeyValue(char* lpDecodeData,char* lpSettingInfo,char* lpTransportKey);

	void parseA2kResponsePacakage(char* lpDecodeData);

	string UTF8ToGBK(string strUtf8);

	void WriteDataToFile(string strSrcData);

	void ParseWebData(string strWebData);

	void ParseAccountBlanceData(string strWebData);

	BOOL parseM1ResponsePackage(char* lpDecodeData);
public:
	DWORD m_dwSignInScreenHeight;
	DWORD m_dwSignInScreenWidth;
	string m_appleId;
	string m_password;
	string m_strUdid;
	string m_accountBalance;
	string m_viewMainEnvironmentDataCenter;
	string m_viewMainServerInstance;
	string m_viewMainRevNum;
	string m_viewMainStoreFrontHeader;
	string m_viewMainPlatformName;
	string m_viewMaintPlatformId;
public:
	string m_termEnvironmentDataCenter;
	string m_termServerInstance;
	string m_termRevNum;
	string m_termStoreFrontHeader;
	string m_termPlatformName;
	string m_termPlatformId;
	string m_termPage;
	string m_termPageId;
public:
	string m_targetAppEnvironmentDataCenter;
	string m_targetAppServerInstance;
	string m_targetAppRevNum;
	string m_targetAppStoreFrontHeader;
	string m_targetAppPlatformName;
	string m_targetAppPlatformId;
	string m_targetAppAppExtVrsId;
	string m_targetAppPageDetails;
	string m_targetAppName;
	string m_targetAppBunldeId;
	string m_strAppleIDClientIdentifier;
	string m_strSerialNumber;
	string m_strTimeStamp;
public:
	VECEVENTTIME m_vecEventtime;
	A2KRESPONSEPACKAGE m_a2kResponseData;
	M1RESPONSEPACKAGE m_m1ResponseData;
	int s_FirstlocationPosition;
	int s_SecondlocationPosition;
	int s_NextlocationPosition;
	int s_PrelocationPosition;
	int s_locationPosition;
	int m_currentPos;
	int m_asn;
	vector <string> m_strVect;

	string m_strFirstAppId;
	string m_strSecondAppId;
	string m_strNextAppId;
	string m_strPreAppId;
	string m_strNextAppName;
	string m_strpreAppName;
	string m_strFirstAppName;
	string m_strSecondAppName;
	string m_strClientCorrelationKey;
	string m_strPageRequestedTime;
public:
	void SetSerialNumber(string strSerialNo);
	
private:
	int    m_timeSpan;
protected:
	vector<string> m_vecIdsList;

public:
	void setXpEventTime();

public:
	static void getFormatTime(char* lpFormatTime);

	CString GetClientTime(string& strTimeStamp);
	CString GetClientTime();

	void getEventTime(char* lpTimeData,BOOL IsSecond,BOOL IsMiSecond);//输出参数

	string getLaunchCorrelationKey();

	string getTimeStamp();

	void getMtRequestId(char* lpRequestId,char* lpClientId,char* lpTimeStamp);//第一个输出参数，第二个输入参数

	void xpAppLaunchEvent(string strUserAgent,
		                  char* lpVersion,
						  int width,
						  int height,
						  char* lpSrcData,
						  char* lpClientId,
						  char* lpDsid,
						  char* lpXpab);
	
	void xpSearchSumbitEvent(string strUserAgent,
							char* lpVersion,
							int width,
							int height,
							char* lpSrcData,
							char* lpSearchword,
							char* lpActionUrl,
							char* lpClientId,
							char* lpSaleId,
							char* lpDsid,
							char* lpXpab,
							char* lpXpabc);

	void xpImpressionsEvent(string strUserAgent,
							char* lpVersion,
							int width,
							int height,
						   char* lpSrcData,
						   char* lpSearchword,
						   char* lpActionUrl,
						   char* lpClientId,
						   char* lpSaleId,
						   char* lpDsid,
						   char* lpXpab,
						   char* lpXpabc);

	void xpFinishImpressionEvent(CString strUserAgent,
								char* lpVersion,
								int width,
								int height,
							   char* lpSrcData,
							   char* lpSearchword,
							   char* lpClientId,
							   char* lpSaleId,
							   char* lpDsid,
							   char* lpXpab,
							   char* lpXpabc);

	void xpPageSearchEvent(string strUserAgent,
							char* lpVersion,
							int width,
							int height,
						  char* lpSrcData,
						  char* lpSearchTerm,
						  char* lpClientId,
						  char* lpDsid,
						  char* lpXpab,
						  char* lpXpabc);

	void xpTargetAppPageDetailEvent(string strUserAgent,
									char* lpVersion,
									int width,
									int height,
                                   char* lpSrcData,
								   char* lpPageUrl,
								   char* lpClientId,
								   char* lpSaleId,
								   char* lpDsid,
								   char* lpSearchTerm,
								   char* lpPageloadTime,
								   char* lpXpab,
								   char* lpXpabc);


	void xpBuyEvent(CString strUserAgent,
					char* lpVersion,
					int width,
					int height,
					char* lpSrcData,
				   char* lpSearchword,
				   char* lpClientId,
				   char* lpSaleId,
				   char* lpDsid,
				   char* lpMtRequestId,
				   char* lpPliIds,
				   char* lpXpab,
				   char* lpXpabc);

	void xpBuyConfirmEvent(string strUserAgent,
						   char* lpVersion,
						   int width,
						   int height,
						  char* lpSrcData,
						  char* lpPageUrl,
						  char* lpClientId,
						  char* lpSaleId,
						  char* lpDsid,
						  char* lpMtRequestId,
						  char* lpPliIds,
						  char* lpXpab,
						  char* lpXpabc);

	void xpActiveAppEvent(string strUserAgent,
						 char* lpVersion,
					     int width,
					     int height,
						 char* lpSrcData,
						 char* lpClientId,
						 char* lpSaleId,
						 char* lpDsid,
						 char* lpImpressionTime,
						 char* lpXpab,
						 char* lpXpabc);

	void xpOpenAppEvent(string strUserAgent,
		                char* lpVersion,
						int width,
						int height,
						char* lpSrcData,
						char* lpClientId,
						char* lpSaleId,
						char* lpDsid,
						char* lpXpab,
						char* lpXpabc);

	void xpBuyInitiateEvent(string strUserAgent,
							char* lpVersion,
							int width,
							int height,
							char* lpSearchTerm,
							char* lpSrcData,
							char* lpClientId,
							char* lpSaleId,
							char* lpDsid,
							char* lpXpab,
							char* lpXpabc);

	void xpBuyAuthSuccessEvent(string strUserAgent,
		                       char* lpVersion,
							   int width,
							   int height,
							   char* lpSrcData,
							   char* lpClientId,
							   char* lpSaleId,
							   char* lpDsid,
							   char* lpXpab,
							   char* lpXpabc,
							   char* lpMtRequestId,
							   char* lpSearchTerm,
							   char* lpAppExtVrsId,
							   char* lpEventtime);

	void xpCommentAppEvent(CString strUserAgent,
						 char* lpVersion,
					     int width,
					     int height,
						 char* lpSrcData,
						 char* lpClientId,
						 char* lpSaleId,
						 char* lpDsid,
						 char* lpImpressionTime,
						 char* lpXpab,
						 char* lpXpabc);

	void xpPageRanderAmpClientPerfEvent(string strUserAgent,
										char* lpVersion,
										int width,
										int height,
										char* lpSrcData,
										char* lpSearchword,
										char* lpActionUrl,
										char* lpClientId,
										char* lpSaleId,
										char* lpDsid,
										char* lpXpab,
										char* lpXpabc);

	void loginXmlData(char* lpSrcData,
					char* lpAppleId,
					char* lpPassword,
					char* lpUdid,
					char* lpKbsync,
					BOOL bReLogin);

	void buyProductPostData(char* lpSrcData,
							char* lpAgreeKey, 
							char* lpAgreeValue,
							char* lpSubmitKey,
							char* lpSubmitValue,
							char* lpSecName1,
							char* lpSecAnswer1,
							char* lpSecName2,
							char* lpSecAnswer2,
							char* lpPasswordToken,
							char* lpAppExtVrsId,
							char* lpCreditDisplay,
							char* lpPrice,
							char* lpUdid,
							char* lpKbsyncValue,
							char* lpClientId,
							char* lpEventtime,
							char* lpSaleId,
							char* lpRequestId,
							char* lpSearchTerm,
							char* lpDsid,
							char* lpTermsId,
							char* lpSbsync,
							char* lpVid,
							char* lpWosid_lite,
							BOOL bPaid,
							int buyEventType);

	void getRegisterData(char* lpSrcData,
						char* lpSerialNumber,
						char* lpRegisterToken,
						char* lpUdid,
						char* lpDeviceNameData);

	string getShowPageIdsData();

	
	void viewAccountXmlData(char* lpSrcData,
							char* lpAppleId,
							char* lpPassword,
							char* lpUdid,
							char* lpKbsync);

	void getFinishMachineProvisioningData(char* lpSrcData,
									     char* lpClientData,
										 char* lpDsid,
										 char* lpUdid,
										 char* lpKbsync); 

	void getA2kPostData(char* lpSrcData,
					   char* lpA2k,
					   char* lpAppleIDClientIdentifier,
					   char* lpX_Apple_I_Client_Time,
					   char* lpX_Apple_I_md,
					   char* lpX_Apple_I_md_m,
					   char* lpUdid,
					   char* lpAppleId);

	void getM1PostData(char* lpSrcData,
					  char* lpC,
					  char* lpM1,
					  char* lpAppleIDClientIdentifier,
					  char* lpX_Apple_I_Client_Time,
					  char* lpX_Apple_I_md,
					  char* lpX_Apple_I_md_m,
					  char* lpUdid,
					  char* lpAppleId);

	void getGsasPostData(char* lpSrcData, 
						 char* lpIMEI, 
						 char* lpMEID, 
						 char* lpSerialNo);
};

#endif