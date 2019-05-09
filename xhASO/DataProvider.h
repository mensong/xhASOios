#pragma once
#include <string>

class CDataProvider
{
public:
	CDataProvider(void);
	~CDataProvider(void);
	void CreateUserData();
	std::string GetFirstName();
	std::string GetLastName();
	const char * GetEmailAddress() const;
	std::wstring GetEmailAddressW();
	std::string GetPhoneNumber();
	const char * GetPassword() const;
	std::wstring GetPasswordW();
	int GetSecurityQuestion();
	std::string GetSecurityAnswer();
	std::string GetYear();
	int GetMonth();
	int GetDay();
	std::string GetZoneAddress();
	std::string GetBuildNumber();
	std::string GetStreetAddress();
	std::string GetCityAddress();
	std::string GetZipNumber();
	int GetProvince();
private:
	std::string MakeRandString( int nMinLen,int nMaxLen );
	CString GetBufferMD5String(BYTE* pBuf, DWORD dwBufLen);
private:
	std::string m_strEmailAddress;
	std::string m_strPassword;

};
