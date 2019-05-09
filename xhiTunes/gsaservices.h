#ifndef __GSA_SERVICES_H
#define __GSA_SERVICES_H
#include <string>
using namespace std;

//itunes版本号：12.4.1.6
//需要调用的itunes中的接口函数
class CGsaservices
{
public:
	CGsaservices();
	~CGsaservices();

public://itunes中公共调用函数
	void SetAllocPtr(LPVOID lpAllocPtr);
	void SetSaveAllocPtr(LPVOID lpSaveAllocPtr);
	LPVOID GetAllocPtr();
	LPVOID GetSaveAllocPtr();

public://生成A2k值需要调用的itunes中的接口函数
	int getDo_Call_addr();
	int getCall_OnceEx_addr();
	int getArray1_addr();			//dword_11C529AC
	int getArray2_addr();			//off_117F481C
	int getArray3_addr();			//unk_11C529C0
	int getAllocatMemmory_addr();
	int getAlloca_probe_16_addr();
	int getEncodeData1_addr();		//sub_112E0E00
	int getEncodeData2_addr();		//sub_112E1680
	int getGenerateA2kData_addr();	//sub_112DD530
	int getArrayData1_addr();       //11694290
	int getArrayData2_addr();       //11694B60
	int getSetData_addr();			//sub_112D7B30
	int getInitSetData_addr();		//sub_112D7AE0
	int getImportKeyBagData_addr(); //sub_100BD210(importKeyBagFunction)
	int getSet_ic_Info_value_addr();//sub_10059E90
	int getSet_SC_Info_Path_addr(); //sub_103B3B90
	int getGenerate_ic_info_value_addr();//sub_103B40E0
	int getSet_ic_Info_Value_addr2(); //sub_1003FDF0
	int getSet_dsid_ic_Info_addr();   //sub_10037270



public://生成M1需要调用的itunes中的接口函数
	int getEncodeString1_addr();    //sub_112D7A10,对密码第一次加密的函数地址
	void setEncodeString1(string str);
	string& getEncodeString1();

	int getEncodeString2_addr();    //sub_112DE340
	void setEncodeString2(string str);
	string& getEncodeString2();

	int getGenerateM1Value_addr();  //sub_112DDE20


public://解析AKPassword需要调用的itunes中的接口函数
	int getCheckM2Value_addr();		//sub_112DE0C0
	int getArrayData3_addr();       //sub_112D9FF0
	int getGen32BytesData_addr();   //sub_112DE120
	int getArrayData4_addr();		//unk_11694B4C
	int getSetData2_addr();         //off_11694B54
	int getSetData3_addr();         //sub_112E4C70
	int getDecodeSpdData_addr();    //sub_112DE4B0
public:
	void parseDecryptData(char* lpDecryptData);
	string& getPetPasswordToken();
	string& getHBToken();
	string& getAdsid();

protected:
	string m_strEncodeString1;
	string m_strEncodeString2;
	string m_petPasswordToken;
	string m_hbToken;
	string m_Adsid;
	
private:
	HMODULE m_hModule;
	LPVOID m_lpAllocPtr;		//[edi+0x38]
	LPVOID m_lpSaveAllocPtr;	//[edi+0x48]	

};
#endif