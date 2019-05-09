#include "stdafx.h"
#include "gsaservices.h"

//该搜索内存函数的方式将采用 函数地址=基地址+偏移量  这样的方式来寻找函数
CGsaservices::CGsaservices()
{
	m_hModule = ::GetModuleHandle(TEXT("iTunesCore.dll"));
	if (m_hModule)
	{
		m_hModule = (HMODULE)((int)m_hModule + 0x1000);
	}

}

CGsaservices::~CGsaservices()
{
	if (m_lpAllocPtr)
	{
		VirtualFree(m_lpAllocPtr,0,MEM_RELEASE);
		m_lpAllocPtr = NULL;
	}

	if (m_lpSaveAllocPtr)
	{
		VirtualFree(m_lpSaveAllocPtr,0,MEM_RELEASE);
		m_lpSaveAllocPtr = NULL;
	}
}

void CGsaservices::SetAllocPtr(LPVOID lpAllocPtr)
{
	m_lpAllocPtr = lpAllocPtr;
}

LPVOID CGsaservices::GetAllocPtr()
{
	return m_lpAllocPtr;
}

void CGsaservices::SetSaveAllocPtr(LPVOID lpSaveAllocPtr)
{
	m_lpSaveAllocPtr = lpSaveAllocPtr;
}

LPVOID CGsaservices::GetSaveAllocPtr()
{
	return m_lpSaveAllocPtr;
}

int CGsaservices::getDo_Call_addr()
{
	return ((int)m_hModule + 0x14087FA);
}

int CGsaservices::getCall_OnceEx_addr()
{
	return ((int)m_hModule + 0x140870D);
}

int CGsaservices::getArray1_addr()
{
	return ((int)m_hModule + 0x1C519AC);
}

int CGsaservices::getArray2_addr()
{
	return ((int)m_hModule + 0x17F381C);
}

int CGsaservices::getArray3_addr()
{
	return ((int)m_hModule + 0x1C519C0);
}

int CGsaservices::getAllocatMemmory_addr()
{
	return ((int)m_hModule + 0x141DF1A);
}

int CGsaservices::getAlloca_probe_16_addr()
{
	return ((int)m_hModule + 0x1428920);
}
int CGsaservices::getEncodeData1_addr()
{
	return ((int)m_hModule + 0x12DFE00);
}
int CGsaservices::getEncodeData2_addr()
{
	return ((int)m_hModule + 0x12E0680);
}

int CGsaservices::getGenerateA2kData_addr()
{
	return ((int)m_hModule + 0x12DC530);
}

int CGsaservices::getArrayData1_addr()
{
	return ((int)m_hModule + 0x1693290);
}
int CGsaservices::getArrayData2_addr()
{
	return ((int)m_hModule + 0x1693B60);
}

int CGsaservices::getSetData_addr()
{
	return ((int)m_hModule + 0x12D6B30);
}

int CGsaservices::getInitSetData_addr()
{
	return ((int)m_hModule + 0x12D6AE0);
}
int CGsaservices::getEncodeString1_addr()
{
	return ((int)m_hModule + 0x12D6A10);
}

int CGsaservices::getImportKeyBagData_addr()
{
	return ((int)m_hModule + 0xBC210);
}
int CGsaservices::getSet_ic_Info_value_addr()
{
	return ((int)m_hModule + 0x58E90);
}
int CGsaservices::getSet_SC_Info_Path_addr()
{
	return ((int)m_hModule + 0x3B2B90);
}
int CGsaservices::getGenerate_ic_info_value_addr()
{
	return ((int)m_hModule + 0x3B30E0);
}
int CGsaservices::getSet_ic_Info_Value_addr2()
{
	return ((int)m_hModule + 0x3EDF0);
}
int CGsaservices::getSet_dsid_ic_Info_addr()
{
	return ((int)m_hModule + 0x36270);
}
void CGsaservices::setEncodeString1(string str)
{
	m_strEncodeString1 = str;
}

string& CGsaservices::getEncodeString1()
{
	return m_strEncodeString1;
}

int CGsaservices::getEncodeString2_addr()
{
	return ((int)m_hModule + 0x12DD340);
}
void CGsaservices::setEncodeString2(string str)
{
	m_strEncodeString2 = str;
}
string& CGsaservices::getEncodeString2()
{
	return m_strEncodeString2;
}

int CGsaservices::getGenerateM1Value_addr()
{
	return ((int)m_hModule + 0x12DCE20);
}

int CGsaservices::getCheckM2Value_addr()
{
	return ((int)m_hModule + 0x12DD0C0);
}

int CGsaservices::getArrayData3_addr()
{
	return ((int)m_hModule + 0x12D8FF0);
}
int CGsaservices::getGen32BytesData_addr()
{
	return ((int)m_hModule + 0x12DD120);
}

int CGsaservices::getArrayData4_addr()
{
	return ((int)m_hModule + 0x1693B4C);
}

int CGsaservices::getSetData2_addr()
{
	return ((int)m_hModule + 0x1693B54);
}

int CGsaservices::getSetData3_addr()
{
	return ((int)m_hModule + 0x12E3C70);
}

int CGsaservices::getDecodeSpdData_addr()
{
	return ((int)m_hModule + 0x12DD4B0);
}

void CGsaservices::parseDecryptData(char* lpDecryptData)
{
	CString strSrcData;
	CString strTmp;
	CString strTargetValue;
	CString strPetTokenKey = TEXT("<key>com.apple.gs.idms.pet</key>");
	CString strStringKey = TEXT("<string>");
	CString strIdms_hbKey = TEXT("<key>com.apple.gs.idms.hb</key>");
	CString strAdsdiKey = TEXT("<key>adsid</key>");

	char szPetToken[512] = {0};
	char szHBToken[512] = {0};
	char szAdsid[MAX_PATH] = {0};

	if (!lpDecryptData)
	{
		return;
	}
	int dataLen = strlen(lpDecryptData);
	TCHAR* lpwcData = new TCHAR[dataLen*2+2];
	if (!lpwcData)
	{
		return;
	}
	memset(lpwcData,0,dataLen*2+2);
	MultiByteToWideChar(CP_UTF8,0,lpDecryptData,dataLen,lpwcData,dataLen*2);

	strSrcData = lpwcData;

	int pos = strSrcData.Find(strPetTokenKey);
	if (pos < 0)
	{
		goto EXIT1;
	}
	strTmp = strSrcData.Right(strSrcData.GetLength()-pos-strPetTokenKey.GetLength());
	pos = strTmp.Find(strStringKey);
	if (pos < 0)
	{
		goto EXIT1;
	}
	strTmp = strTmp.Right(strTmp.GetLength()-pos-strStringKey.GetLength());
	pos = strTmp.Find(TEXT("</string>"));
	if (pos < 0)
	{
		goto EXIT1;
	}
	strTargetValue = strTmp.Left(pos);
	WideCharToMultiByte(CP_ACP,0,strTargetValue.GetBuffer(),strTargetValue.GetLength(),szPetToken,512,NULL,NULL);
	m_petPasswordToken = szPetToken;
	//获取HB token
	pos = strSrcData.Find(strIdms_hbKey);
	if (pos < 0)
	{
		goto EXIT1;
	}
	strTmp = strSrcData.Right(strSrcData.GetLength()-pos-strIdms_hbKey.GetLength());
	pos = strTmp.Find(strStringKey);
	if (pos < 0)
	{
		goto EXIT1;
	}
	strTmp = strTmp.Right(strTmp.GetLength()-pos-strStringKey.GetLength());
	pos = strTmp.Find(TEXT("</string>"));
	if (pos < 0)
	{
		goto EXIT1;
	}
	strTargetValue = strTmp.Left(pos);
	WideCharToMultiByte(CP_ACP,0,strTargetValue.GetBuffer(),strTargetValue.GetLength(),szHBToken,512,NULL,NULL);
	m_hbToken = szHBToken;

	//获取adsid
	pos = strSrcData.Find(strAdsdiKey);
	if (pos < 0)
	{
		goto EXIT1;
	}
	strTmp = strSrcData.Right(strSrcData.GetLength()-pos-strAdsdiKey.GetLength());
	pos = strTmp.Find(strStringKey);
	if (pos < 0)
	{
		goto EXIT1;
	}
	strTmp = strTmp.Right(strTmp.GetLength()-pos-strStringKey.GetLength());
	pos = strTmp.Find(TEXT("</"));
	if (pos < 0)
	{
		goto EXIT1;
	}
	strTargetValue = strTmp.Left(pos);
	WideCharToMultiByte(CP_ACP,0,strTargetValue.GetBuffer(),strTargetValue.GetLength(),szAdsid,MAX_PATH,NULL,NULL);
	m_Adsid = szAdsid;


EXIT1:
	if (lpwcData)
	{
		delete []lpwcData;
		lpwcData = NULL;
	}
	
	return;

}

string& CGsaservices::getPetPasswordToken()
{
	return m_petPasswordToken;
}
string& CGsaservices::getHBToken()
{
	return m_hbToken;
}
string& CGsaservices::getAdsid()
{
	return m_Adsid;
}
