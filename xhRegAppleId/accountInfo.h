#ifndef __ACCOUNT_H
#define __ACCOUNT_H
#include "stdafx.h"
#include <list>
#include <vector>
#define		LOADMOD_FAILED		-3
#define		NETCONNECT_FAILED   -2
#define		ALLOCATEMEM_FAILED	-1
#define		S_SUCCESS			0

#define		UA_8_4				TEXT("AppStore/2.0 iOS/8.4 model/iPhone5,3 build/12H143 (6; dt:97)")
#define		UA_ITUNES_12_1_1	TEXT("iTunes/12.1.1 (Windows; Microsoft Windows 7 Ultimate Edition Service Pack 1 (Build 7601))")

typedef struct _modelInfo
{
	char szModel[32];
	char szBuild[32];
	char szVersion[32];
	int  width;
	int  height;
}MODELINFO,*PMODELINFO;

typedef std::vector<MODELINFO> VECTORMODEL;


typedef struct _deviceInfo
{
	TCHAR strUdid[MAX_PATH];
	TCHAR strSerialNumber[64];
	TCHAR strIMEI[64];
	TCHAR strMEID[64];
}DEVICEINFO,*PDEVICEINFO;


typedef struct _accountInfo
{
	int nIndex;
	TCHAR strAppleId[128];
	TCHAR strPassword[128];
	DEVICEINFO deviceInfo;

}ACCOUNTINFO,*PACCOUNTINFO;


typedef std::list<ACCOUNTINFO> AccountInfo;

typedef struct _hwInfo
{
	CString strComputerName;
	CString strHwProfileGuid;
	CString strSystemBiosVersion;
	CString strProcessorNameString;
	CString strProductId;
}HARDWAREINFO,*PHARDWAREINFO;

typedef struct _sendInfo
{
	ACCOUNTINFO accountInfo;
	MODELINFO modelInfo;
	TCHAR strTargetUrl[MAX_PATH];
	TCHAR strEventName[MAX_PATH];
	TCHAR strRate[MAX_PATH];
	TCHAR strNickName[MAX_PATH];
	TCHAR strTitle[64];
	TCHAR strContext[512];
	TCHAR strHotKeyWord[MAX_PATH];
	TCHAR strSaleId[MAX_PATH];
	TCHAR strProxy[MAX_PATH];
	TCHAR strFatherAndMather[MAX_PATH];
	TCHAR strTeacher[MAX_PATH];
	TCHAR strBook[MAX_PATH];
	TCHAR strAppPrice[64];
	int netType;
	int type;
	BOOL bPaidApp;
}SENDINFO,*PSENDINFO;

typedef struct _tagSubmitInfo
{
	CString strLastFirstName;
	CString strFirstName;
	CString strStreet1;
	CString strStreet2;
	CString strStreet3;
	CString strCountry;
	CString strCity;
	CString strPostNum;
	CString strPhone;
	CString strCreateButton;
}TAGSUBMITINFO,*PTAGSUBMITINFO;

typedef struct _tagRegisterInfo
{
	AccountInfo accountInfo;
	CString	  strMachineGuid;
	CString   strZoneAddr;
	CString   strBuildAddr;
	CString   strStreetAddr;
	CString   strCityAddr;
	CString   strZipNumber;
	CString   strAnswer1;
	CString	  strAnswer2;
	CString   strAnswer3;
	int       nProvince;
	CString   strYear;
	int       nMonth;
	int       nDay;
	CString   strFisrtName;
	CString   strLastName;
	CString   strPhoneNum;
	CString   strRescueMail;
	TAGSUBMITINFO submitInfo;
}TAGREGISTERINFO,*PTAGREGISTERINFO;




#endif