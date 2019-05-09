#ifndef __REGISTER_APPLEID_H
#define __REGISTER_APPLEID_H
#include <string>
#include <map>
#include "accountInfo.h"
using namespace std;

typedef map<CString,CString> NAMETOVALUE;

typedef struct _regRichWebInfo
{
	CString strNextStepName;
	CString strPageUUIDValue;
	CString strBirthMonthName;
	CString strRescueEmailName;
	CString strBirthYearName;
	CString strBirthDayName;
	CString strAnswer1Name;
	CString strAnswer2Name;
	CString strAnswer3Name;
	CString strQuestion1Name;
	CString strQuestion2Name;
	CString strQuestion3Name;
	CString	strPasswordName;
	CString strVerifyPwdName;
	CString strAppleIdName;

}REGRICHWEBINFO,*PREGRICHWEBINFO;

typedef struct _regCrditWebInfo
{
	CString strCoderedemptName;
	CString strPostalCodeName;
	CString strNextStepName;
	CString strCityName;
	CString strFirstName;
	CString strLastName;
	CString strFullName;
	CString strStreet1Name;
	CString strStreet2Name;
	CString strStreet3Name;
	CString strNdpd_vk_Value;
	CString strNdpd_w_Value;
	CString strNdpd_s_Value;
	CString strNdpd_f_Value;
	CString strNdpd_fm_Value;
	CString strPhoneNumberName;
	CString strCCExprYearValue;
	CString strPageUUIDValue;

}REGCREDITWEBINFO,*PREGCREDITWEBINFO;

class ManagerRegister
{
public:
	ManagerRegister(TAGREGISTERINFO& tagRegisterInfo);
	~ManagerRegister(void);
public:
	BOOL sendSignSapSetupCert(CString strUrl,CString strRefererUrl,CString strVerb,CString strUserAgent,CString strHostName);
	BOOL sendSignSapSetup(CString strUrl,CString strRefererUrl,CString strVerb,CString strUserAgent,CString strHostName,char* lpSapBuf);
	BOOL sendSignupWizard(CString strUrl,CString strVerb,CString strUserAgent,CString strHostName,char* pXAppleSignature,int SignaureLength);
	BOOL sendGetAgreeWebPage(CString strUrl,CString strRefererUrl,CString strVerb,CString strUserAgent,CString strAgreeWebActionName,CString strNextStepName);
	BOOL sendGetRichInfoWebPage(CString strUrl,CString strRefererUrl,CString strVerb,CString strUserAgent,CString strRichInfoWebActionName,char* lpSendData,int dataLen);
	BOOL sendGetCreditInfoWebPage(CString strUrl,CString strRefererUrl,CString strVerb,CString strUserAgent,CString strCreditWebActionName,char* lpSendData,int dataLen);
	BOOL sendSumbitCreditInfo(CString strUrl,CString strRefererUrl,CString strVerb,CString strUserAgent,CString strSubmitCreditInfoWebActionName,char* lpSendData,int dataLen);
	BOOL sendXpReport();

public:
	char* getSignSapSetupData(void);
	char* getXAppleActionsignatureData(void);
	CString& getAgreeRegWebActionName(void);
	CString& getRichInfoWebActionName(void);
	CString& getCreditWebActionName(void);
	CString& getSubmitCreditInfoWebActionName(void);
	CString& getNextStepName(void);
	CString& getMachineGUID(void);
	CString& getRefererUrl(void);
	CString& getAgreeWebPageUUID(void);
	CString& getRichInfoWebPageUUID(void);
	CString& getCreditInfoWebPageUUID(void);
	CString& getRegisterAppleId(void);

public:
	CStringA& generateBasePostData(char* lpXAppleActionSignature);
	CStringA& generateRichInfoPostData(char* lpXAppleActionSignature);
	CStringA& generateCreditPostData(char* lpXAppleActionSignature);

protected:
	void extractSignSapSetup(HINTERNET hOpenReq);
	void extractResponseXAppleActionsignature(HINTERNET hOpenReq);
	void decodeResponseServerData(HINTERNET hOpenReq,char* lpBuffer,LPDWORD lpdwSize);
	void parseNextStepWebData(char* lpSrcWebData);
	void parseRegAgreeWebData(char* lpSrcWebData);
	void parseRichInfoWebData(char* lpSrcWebData);
	void parseCreditWebData(char* lpSrcWebData);

private:
	TAGREGISTERINFO		m_tagRegisterInfo;
	REGRICHWEBINFO		m_regRichWebInfo;
	REGCREDITWEBINFO	m_regCreditWebInfo;
	NAMETOVALUE			m_mapNameValue;
	char*			m_lpSignSapSetupBuffer;
	char*			m_lpXAppleActionSignature;

	CString			m_strRefererUrl;

	CString			m_strAgreeRegWebActionName;
	CString         m_strNextStepName;

	CString			m_strRichInfoWebActionName;
	CString			m_strAgreeName;

	CString			m_strSrcAgreeWebPageUUID;
	CString			m_strAgreeWebPageUUID;

	CString			m_richWebPageUUID;
	CString			m_creditWebPageUUID;

	CString			m_strCreditWebActionName;

	CString			m_strSubmitCreditInfoWebActionName;
	

	CString			m_strProxy;
	int				m_netType;

};

#endif