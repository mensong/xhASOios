#pragma once
#include "stdafx.h"
#include "statusData.h"
#include <string>
using namespace std;
class CCaptchaPageInfo
{
public:
	CCaptchaPageInfo(void);
	virtual ~CCaptchaPageInfo(void);
private:
	int rand_range(int low, int hi);
	CStringA _getDI(CStringA ua);
	CStringA _quickHash(char* pStrData);
	CStringA _get_wg1();
	CStringA _getScreenFingerprint();
	CStringA _getScreenInfo();
	CStringA _getDeviceTimezone();
	CStringA _getDeviceLanguage();
	CStringA _getPlugins();
	CStringA _get_bp1();
	CStringA _getBI(CStringA ua, CStringA refer_url);
	CStringA _isFlashInstalled();
	CStringA _getWebGLInfo();
public:
    void ParseCaptchaPageData(char* lpSrcData);

	BOOL SendNuDataValidateCaptchaSrv(CString strUserAgent,CString strRefererUrl,CString strCookie,CString strToken,CString strDsid,CString strGuid,CString strSaleId,CString strX_Apple_I_md_m,CString strX_Apple_I_md,CString strProxy,int netType);

	BOOL SendFinanceAppChallengeReport(CString strUserAgent,CString strXpReportUrl,CString strXpReportHostName,CString strDsid,CString strCookie,CString strX_Apple_I_md_m,CString strX_Apple_I_md,CString strProxy,int netType);

	BOOL SendAppleLoc(CString strUserAgent,CString strRefererUrl,CString strDsid,CString strCookie,CString strX_Apple_I_md_m,CString strX_Apple_I_md,CString strProxy,int netType);

	BOOL SendNuDataSecurityGetWk(CString strUserAgent,CString strRefererUrl,CString strDsid,CString strCookie,CString strX_Apple_I_md_m,CString strX_Apple_I_md,CString strProxy,int netType);

	BOOL SendGetCaptchaGifData(CString strUserAgent,CString strRefererUrl,CString strDsid,CString strCookie,CString strX_Apple_I_md_m,CString strX_Apple_I_md,CString strProxy,int netType);

	void DecodeResponseCaptchaInfoData(HINTERNET hOpenReq,DWORD dwSrcSize,string& strDecodeData);

	void ReadNdpd_wkFromServer(HINTERNET hOpenReq,DWORD dwSrcSize);

	void ReadCaptchaGifData(HINTERNET hOpenReq,DWORD dwSrcSize);

	void GetBase64FromStream(CStringA& base64, IStream* stream);

	void GetStringFromStream(CStringA& str, IStream* stream);

	CStringA Base64Encode(const unsigned char* Data, int DataByte);



	CString GetClientTime();
private:
	CString m_strNdpd_s;
	CString m_strNdpd_f;
	CString m_strNdpd_fm;
	CString m_strNdpd_w;
	CString m_strNdpd_ipr;
	CString m_strNdpd_di;
	CString m_strNdpd_bi;
	CString m_strNdpd_probe;
	CString m_strNdpd_af;
	CString m_strNdpd_fv;
	CString m_strNdpd_fa;
	CString m_strNdpd_bp;
	CString m_strNdpd_wk;
	CString m_strNdpd_vk;
	CString m_strNdpd_wkr;
	CString m_strNuCaptcha_answer;
	CString m_strNuCaptcha_answer_enter;
	CString m_strNuCaptcha_answer_exit;
	CString m_strNuCaptcha_token;
	CString m_strGetnudatasecurityUrl;
	CString m_strGetCaptchaSecurityUrl;
	CString m_strGetWkHostName;
	CString m_strGetCaptchaHostName;
	CString m_strGetWk_wt_Value;
	CString m_str_r_Value;
private:
	string m_strDecodeResponseCaptchaData;
private:
	IStream* m_StreamCaptcha;

};
