#include "StdAfx.h"
#include "DataProvider.h"
#include "UserName.h"
#include <algorithm>

#include "md5.h"
#include "EmailDomainMgr.h"
#define MILLISEC_TO_SEC 1000
#define MICROSEC_TO_MINSEC 1000
 int g_randArr[] = 
 {
 	100, 
 	-1212,
	934342,
 	-2343,
 	1,
 	33,
 	-34324,
 	-258713,
 	1233,
 	98,
 	-525,
 	36,
	128986766554,
 	25,
	-83472374,
 	296,
 	147,
 	-587,
 	-654,
 	-55,
     45679255,
 	-99,
 	8118,
 	909,
 	707,
 	-569877545,
 	66,
 	321,
 	654,
 	987,
 	852,
 	-568794,
 	159,
 	1324645669,
 	357,
 	654,
	-83272,
 	852,
 	-258,
 	-4,
 	369,
 	-269,
 	-147,
 	-459,
 	123213231,
 	22223,
	34556,
	-76565
 };

static UINT64 PrecisionSystemTime()
{
	LARGE_INTEGER lvTick;

	// 获取每秒多少CPU Performance Tick
	QueryPerformanceFrequency( &lvTick );

	// 转换为每个Tick多少秒
	double dSecondsPerTick = 1.0 / lvTick.QuadPart;

	LARGE_INTEGER lvCounter;
	// 获取CPU运行到现在的Tick数
	QueryPerformanceCounter( &lvCounter );

	// 计算CPU运行到现在的时间
	// 比GetTickCount和timeGetTime更加精确 
	return (dSecondsPerTick * lvCounter.QuadPart * MILLISEC_TO_SEC * MICROSEC_TO_MINSEC);
}

int RealRand()
{
	int nLen = sizeof(g_randArr)/sizeof(int);
	srand(PrecisionSystemTime());
	int i = rand() % nLen;
    UINT64 nSeed = PrecisionSystemTime() + g_randArr[i];
    srand(nSeed);
	return rand();
}

CDataProvider::CDataProvider(void)
{
}

CDataProvider::~CDataProvider(void)
{
}
void CDataProvider::CreateUserData()
{
	int nFirstNameCount = sizeof(g_szFirstNameArr)/sizeof(char *);
	int nLastNameCount = sizeof( g_szLastNameArr )/sizeof(char *);
	int nFirstNameIndex =  RealRand()%nFirstNameCount;
	int nLastNameIndex = RealRand()%nLastNameCount;
	char szBuf[256] = {0};
	int nRand = 0;
	for( int i = 0; i< 3; i++ )
	{
		int nTemp = RealRand()%10;
		if ( 0 == nTemp )
		{
			nTemp = 1;
		}
		nRand = nRand*10 + nTemp;
	}
	std::string strFirstName = g_szFirstNameArr[nFirstNameIndex];
	std::string strLastName = g_szLastNameArr[nLastNameIndex];
	std::transform(strFirstName.begin(), strFirstName.end(), strFirstName.begin(), towlower);
	std::transform(strLastName.begin(), strLastName.end(), strLastName.begin(), towlower);

  USES_CONVERSION;
  CString strDomain = CEmailDomainMgr::GetInstance()->GetRandomDomain();
	sprintf( szBuf,"%s%s%d%s",strFirstName.c_str(),strLastName.c_str(),nRand, T2A(strDomain) );
	CString strMD5 = GetBufferMD5String( (BYTE*)szBuf,strlen( szBuf ) );
	char szPasswordBuf[32] = {0};
	int i = 0; 
	for( ;i< 6 & i < strMD5.GetLength(); i++ )
	{
		szPasswordBuf[i] = strMD5.GetAt( i );
	}
	char temp = strFirstName[0];
	szPasswordBuf[i++] = toupper( temp );
	szPasswordBuf[i++] = temp;
	szPasswordBuf[i] = '\0';
	sprintf( szPasswordBuf,"%s%d",szPasswordBuf,strlen( szBuf ) );
	m_strPassword = szPasswordBuf;
	m_strEmailAddress = szBuf;
}
std::string CDataProvider::GetFirstName()
{
	int nFirstNameCount = sizeof(g_szFirstNameArr)/sizeof(char *);
	int i =  RealRand()%nFirstNameCount;
	return g_szFirstNameArr[i];
}

std::string CDataProvider::GetLastName()
{
	int nLastNameCount = sizeof( g_szLastNameArr )/sizeof(char *);
	int i =  RealRand()%nLastNameCount;
	return g_szFirstNameArr[i];
}

const char * CDataProvider::GetEmailAddress() const
{
	
	return m_strEmailAddress.c_str();	
}

std::wstring CDataProvider::GetEmailAddressW()
{ 
	int nRetLen = MultiByteToWideChar(CP_ACP, 0, m_strEmailAddress.c_str(), -1, NULL, NULL); 
	WCHAR * lpUnicodeStr = new WCHAR[nRetLen + 1];  
	nRetLen = MultiByteToWideChar(CP_ACP, 0, m_strEmailAddress.c_str(), -1, lpUnicodeStr, nRetLen); 
	std::wstring strTemp = lpUnicodeStr;
	delete []lpUnicodeStr;
	return strTemp;
}

std::string CDataProvider::GetPhoneNumber()
{
	char szPhoneNumber[12] = {"13"};
	char szTemp[10] = {0};
	for ( int i = 2; i< 11;i++ )
	{
		int nTemp = RealRand()%10;
		itoa( nTemp,szTemp,10 );
		szPhoneNumber[i] = szTemp[0];
	}
	szPhoneNumber[11] = '\0';
	return szPhoneNumber;
}

const char * CDataProvider::GetPassword() const
{
    return m_strPassword.c_str();
}

std::wstring CDataProvider::GetPasswordW()
{
	int nRetLen = MultiByteToWideChar(CP_ACP, 0, m_strPassword.c_str(), -1, NULL, NULL); 
	WCHAR * lpUnicodeStr = new WCHAR[nRetLen + 1];  
	nRetLen = MultiByteToWideChar(CP_ACP, 0, m_strPassword.c_str(), -1, lpUnicodeStr, nRetLen); 
	std::wstring strTemp = lpUnicodeStr;
	delete []lpUnicodeStr;
	return strTemp;
}

int CDataProvider::GetSecurityQuestion()
{
    int  i = RealRand()%6;
	if ( i == 0 )
	{
		i = 1;
	}
	return i;
}

std::string CDataProvider::GetSecurityAnswer()
{
	std::string strTemp = MakeRandString( 4,8 );
	strTemp += " ";
	strTemp += MakeRandString( 5,9 );
	strTemp += " ";
	strTemp += MakeRandString( 4,8 );
	return strTemp;
}

std::string CDataProvider::GetYear()
{
    int nYear = RealRand()%( 1990 - 1976 )  +  1976;
	char szTemp[32] = {0};
	itoa( nYear,szTemp,10 );
	return szTemp;
}

int CDataProvider::GetMonth()
{
	int  i = RealRand()%12;
	if ( i == 0 )
	{
		i = 1;
	}
	return i;
}

int CDataProvider::GetDay()
{
	int  i = RealRand()%31;
	if ( i == 0 )
	{
		i = 1;
	}
	return i;
}

std::string CDataProvider::GetZoneAddress()
{
    return MakeRandString( 8,16 );
}

std::string CDataProvider::GetBuildNumber()
{
    return MakeRandString( 6,10 );
}

std::string CDataProvider::GetStreetAddress()
{
	std::string strTemp = MakeRandString( 4,8 );
	strTemp += " ";
	strTemp += MakeRandString( 5,9 );
	return strTemp;
}

std::string CDataProvider::GetCityAddress()
{
	return MakeRandString( 6,10 );
}

std::string CDataProvider::GetZipNumber()
{
    int nTemp = RealRand()&100000;
	nTemp+=100000;
	char szTemp[32] = {0};
	itoa( nTemp,szTemp,10 );
	return szTemp;
}

int CDataProvider::GetProvince()
{
	int  i = RealRand()%23;
	if ( i == 0 )
	{
		i = 1;
	}
	return i;
}

std::string CDataProvider::MakeRandString( int nMinLen,int nMaxLen )
{
	if ( nMaxLen < nMinLen )
	{
		return "";
	}
	int nAscLen =  strlen(ASCIITABLE);
	int nNumLen = strlen(NUMBERTABLE);
	int nLen =(RealRand()%( nMaxLen - nMinLen ))  +  nMinLen;
	char * pTemp = new char[nLen + 1];
	for ( int i = 0; i<nLen; i++ )
	{
		int t = RealRand()%2;
		if( 0 == i 
			|| 0 == t )
		{
			int j = RealRand()%nAscLen;
			pTemp[i] = ASCIITABLE[j];
		}
		else
		{
			int j = RealRand()%nNumLen;
			pTemp[i] = NUMBERTABLE[j];
		}
	}
	pTemp[nLen] = '\0';
	std::string strTemp = pTemp;
	delete []pTemp;
	pTemp = NULL;
	return strTemp;
}
// CString Common::Util::GetStringMD5(CString strSrc)
// {
// 	CStringA strUrlA = strSrc;
// 	int nLength = (int)strUrlA.GetLength();
// 
// 	CString strMd5;
// 	return GetBufferMD5String((BYTE*)(LPCSTR)strUrlA, nLength);
// }
// 
CString CDataProvider::GetBufferMD5String(BYTE* pBuf, DWORD dwBufLen)
{
	if ( pBuf && dwBufLen )
	{
		MD5_CTX md5;
		MD5Init(&md5, 0);
		MD5Update(&md5, pBuf, dwBufLen);
		MD5Final(&md5);

		BYTE hash[16] = { 0 };
		memcpy(hash, md5.digest, 16);
		CString strMD5Make;
		for( int n = 0 ; n < 16; n++)
		{
			CString strTmp;
			strTmp.Format(_T("%02x"), hash[n]);
			strMD5Make += strTmp;
		}
		return strMD5Make;
	}
	return L"";
}
