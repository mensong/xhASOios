#pragma once

namespace Util
{
  CString GetCurrentDate();

  CString ConvertUTF8toUnicode (LPCSTR pszStr);
  CStringA ConvertUnicodeToUTF8( LPCWSTR pszStr );

  CString URLEncode(CString sIn);
  CString URLEncode2(CString sIn);

  BOOL ExtractDataByTag(CString strData, CString& strRet, CString strTag, CString strPreTag, CString strPostTag);

  BOOL VerifyXmlData(CStringA strXmlData);

  CString AppendIdToUrl(CString strUrl);

  BOOL GetModuleAddress(HMODULE hMod, LPVOID& pBeginAddr, LPVOID& pEndAddr);

  DWORD GetBaseAddress(PBYTE const pCharacter, DWORD dwSize, int iOffset = 0, HMODULE hModule = NULL) ;
}
