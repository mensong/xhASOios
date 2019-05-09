#include "StdAfx.h"
#include "EmailDomainMgr.h"


CEmailDomainMgr::CEmailDomainMgr(void)
{
}


CEmailDomainMgr::~CEmailDomainMgr(void)
{
}

void CEmailDomainMgr::Initialize()
{
  m_vecDomain.clear();

  USES_CONVERSION;
  TCHAR szFileName[_MAX_PATH] = {0};
  GetModuleFileName(NULL,szFileName,_MAX_PATH);
  PathRemoveFileSpec(szFileName);
  PathAppend(szFileName, _T("emailData\\email.txt"));
  if (!PathFileExists(szFileName))
  {
    ::MessageBox(NULL, TEXT("EmailDomain文件打开失败，请检查文件路径！"), TEXT("注册"), MB_OK);
    return;
  }

  CFile file;
  if (!file.Open(szFileName, CFile::modeReadWrite))
  {
    return;
  }

  int nLen = file.GetLength();
  BYTE *pData = new BYTE[nLen];
  UINT uRealLen = file.Read(pData, nLen);
  if (uRealLen != nLen)
  {
    return;
  }

  CStringA strTemp((LPCSTR)pData, nLen);
  CString strData = A2T(strTemp);

  int nIndex = strData.Find(TEXT("\r\n"));
  while(nIndex != -1)
  {
    CString strDomain = strData.Left(nIndex);
    m_vecDomain.push_back(strDomain);

    strData = strData.Right(strData.GetLength() - nIndex - sizeof(TEXT("\r\n"))/sizeof(TCHAR) + 1);
    nIndex = strData.Find(TEXT("\r\n")); 
  }
}

CString CEmailDomainMgr::GetRandomDomain()
{
  UINT uSize = m_vecDomain.size();
  if(uSize == 0)
    return TEXT("@xemesoft.com");

  srand((unsigned) time(NULL));
  int nIndex = rand()%uSize;

  if (nIndex >= 0 && nIndex < uSize)
  {
    return m_vecDomain[nIndex];
  }

  return TEXT("@xemesoft.com");
}

