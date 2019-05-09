#ifndef _SEC_DATA_H
#define _SEC_DATA_H
#include <map>
#include <vector>
#include <string>
#define    MME_CLIENT_INFO         TEXT("<iPhone7,1> <iPhone OS;10.0.2;13B143> <com.apple.akd/1.0 (com.apple.akd/1.0)>")//TEXT("<iPhone7,1> <iPhone OS;9.1;13B143> <com.apple.akd/1.0 (com.apple.akd/1.0)>")  
#define	   GSA_USER_AGENT		   TEXT("akd/1.0 CFNetwork/808.1.4 Darwin/16.1.0")//akd/1.0 CFNetwork/758.1.6 Darwin/15.0.0

#define		NEXTSTEP_TAG	TEXT("id=\"hiddenBottomRightButtonId\" type=\"submit\"")
#define		MID_TAG			TEXT("name=\"")
#define		PAGEUUID_TAG	TEXT("name=\"mzPageUUID\" type=\"hidden\" value='")
#define		BIRTHMONTH_TAG  TEXT("\"birthMonthPopup\" name=\"")
#define		RESCUEMAIL_TAG	TEXT("title=\"rescue@example.com\" type=\"email\" name=\"")
#define		BIRTHYEAR_TAG	TEXT("id=\"birthYear\" title=\"")
#define		BIRTHDAY_TAG	TEXT("id=\"birthDayPopup\" name=\"")
#define		ANSWER3_TAG		TEXT("id=\"answerField3\" title=\"")
#define		ANSWER2_TAG		TEXT("id=\"answerField2\" title=\"")
#define		ANSWER1_TAG		TEXT("id=\"answerField1\" title=\"")
#define		QUESTION3_TAG	TEXT("id=\"questionField3Input\" type=\"hidden\" name=\"")
#define		QUESTION2_TAG	TEXT("id=\"questionField2Input\" type=\"hidden\" name=\"")
#define		QUESTION1_TAG	TEXT("id=\"questionField1Input\" type=\"hidden\" name=\"")
#define		PASSWORD_TAG	TEXT("id=\"passwordField\" title=\"")
#define		VERIFYPWD_TAG	TEXT("id=\"passwordVerificationField\" title=\"")
#define		APPLEID_TAG		TEXT("id=\"accountNameField\" title=\"")
#define		CODEREDEMPT_TAG TEXT("id=\"codeRedemptionField\" title=\"")
#define		POSTALCODE_TAG	TEXT("id=\"postalCodeField\" title=\"")
#define		CITY_TAG		TEXT("id=\"cityField\" title=\"")
#define		FIRSTNAME_TAG	TEXT("id=\"firstNameField\" title=\"")
#define		LASTNAME_TAG	TEXT("id=\"lastNameField\" title=\"")
#define		STREET1_TAG		TEXT("id=\"street1Field\" title=\"")
#define		STREET2_TAG		TEXT("id=\"street2Field\" title=\"")
#define		STREET3_TAG		TEXT("id=\"street3Field\" title=\"")
#define		NDPD_VK_TAG		TEXT("id=\"ndpd-vk\" name=\"ndpd-vk\" value=\"")
#define		NDPD_W_TAG		TEXT("id=\"ndpd-w\" name=\"ndpd-w\" value=\"")
#define		NDPD_S_TAG		TEXT("id=\"ndpd-s\" name=\"ndpd-s\" value=\"")
#define		NDPD_F_TAG		TEXT("id=\"ndpd-f\" name=\"ndpd-f\" value=\"")
#define		NDPD_FM_TAG		TEXT("id=\"ndpd-fm\" name=\"ndpd-fm\" value=\"")
#define		PHONENUMBER_TAG TEXT("id=\"phoneNumberField\" title=\"")
#define		EXPRYEAR_TAG	TEXT("id=\"expirationYearField\" name=\"cc-expr-year\"><option value=\"")


#define		GENERATE_AMD_FAILED					"generate AMD value failed"
#define		LOGINGSASERVER_FAILED				"preLoginGsaServicesVerifier failed"
#define		GENERATESRPADSID_FAILED				"pGenerateSrpAdsid failed"
#define		REGISTERXPREPORT_FAILED				"registerXpReport failed"
#define		SPIM_VALUE_NULL						"szSpim value is null"
#define		STARTMACHINEPROVISIONREPORT_FAILED	"startMachineProvisionReport failed"
#define		FINISHMACHINEPROVISIONREPORT_FAILED "finishMachineProvisionReport failed"
#define     TK_PTM_VALUE_NULL					"tk or ptm value is null"
#define		EXECUTE_EXCEPTION					"execute exception"
#define		INITSIGNSAPSETUPCERT_FAILED			"InitSignSapSetupCert failed"
#define		ALLOCATE_MEM_FAILED					"allocate memory failed"
#define		RESPONSESERVER_FAILED				"ResponseServer failed"
#define		PROVISIONINGREPORT_FAILED			"ProvisioningReport failed"
#define		RELOGINAPPSTOREFORDOWNLOAD_FAILED	"reLoginAppStoreForDownload failed"
#define		GETSIGNSAPSETUP_FAILED				"getSignSapSetup failed"
#define		RESPONSESERVERWITHSIGNATURE_FAILED  "ResponseServerWithSignature failed"
#define		SEARCHHOTWORDHINT_FAILED			"searchHotWordHint failed"
#define		SEARCHHOTWORDSUBMIT_FAILED			"searchHotWordSubmit failed"
#define		XPREPORTTARGETAPPMAIN_FAILED		"xpReportTargetAppMain failed"
#define		GETTARGETIDWEBDATA_FAILED			"getTargetIdWebData failed"
#define		SENDSPLOOKUPOFFER_FAILED			"sendSpLookupOffer failed"
#define		AGREEDATAREPORT_FAILED				"agreeDataReport failed"
#define		ITMSAPPSREPORT_FAILED				"itmsAppsReport failed"
#define		SECURITYANSWERREPORT_FAILED			"securityAnswerReport failed"
#define		GETSECURITYVERIFYWEBREPORT_FAILED	"getSecurityVerifyWebReport failed"
#define		SETTINGINFOVALUEIS_NULL				"SettingInfo value is null"
#define		GETSONGIDURLPATHDOWNLOADID_FAILED	"GetSongIdUrlPathdownloadId failed"

typedef std::map<CString,CString> SECQUESTIONTONAME;

typedef std::map<CString,CString> QUESTIONTOANSWER;

typedef std::map<CString,CString> NAMETOANSWER;

typedef std::vector<std::string> VECEVENTTIME;

typedef struct _matchSecInfo
{
	TCHAR strSecName[128];
	TCHAR strSecAnswer[128];

}MATCHSECINFO,*PMATCHSECINFO;

typedef std::vector<MATCHSECINFO> VECMATCHSECINFO;

#endif