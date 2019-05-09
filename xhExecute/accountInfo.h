#ifndef __ACCOUNT_H
#define __ACCOUNT_H
#include "stdafx.h"
#include <list>
#include <vector>
typedef struct _modelInfo
{
	char szModel[32];
	char szBuild[32];
	char szHardwarePlatform[32];
	char szVersion[32];
	int  width;
	int  height;
	int  innerWidth;
	int  innerHeight;
}MODELINFO,*PMODELINFO;

typedef std::vector<MODELINFO> VECTORMODEL;


typedef struct _deviceInfo
{
	TCHAR strUdid[MAX_PATH];
	TCHAR strSerialNumber[64];
	TCHAR strIMEI[64];
	TCHAR strMEID[64];
	TCHAR strXp_ci[64];
	TCHAR strX_Apple_I_md_m[MAX_PATH];
	TCHAR strX_Apple_I_md[128];
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
	TCHAR strClientId[64];
	int netType;
	int type;
	int orderId;
	BOOL bPaidApp;
}SENDINFO,*PSENDINFO;

typedef struct _logInfo
{
	int nIndex;
	TCHAR strItemLog[MAX_PATH];
	int result;
}LOGINFO,*PLOGINFO;

#endif