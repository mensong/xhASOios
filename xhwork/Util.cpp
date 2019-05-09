#include "StdAfx.h"
#include "Util.h"
#include <atltime.h>

CString Util::GetCurrentDate()
{
	CTime time = CTime::GetCurrentTime();
	CString date = time.Format(TEXT("Date: %a, %d %b %Y %H:%M:%S GMT"));
	return date;
}

CString Util::ConvertUTF8toUnicode( LPCSTR pszStr )
{
	if (!pszStr)
		return _T("");

	//预转换，计算需要的空间
	int wcslen = ::MultiByteToWideChar(CP_UTF8, NULL, pszStr, strlen(pszStr), NULL, 0);
	WCHAR *wp = new WCHAR[wcslen + 1];

	//转换
	::MultiByteToWideChar(CP_UTF8, NULL, pszStr, strlen(pszStr), wp, wcslen);
	wp[wcslen] = '\0';

	//保存并返回
	CString s = wp ;
	delete[] wp ;
	return s ;
}

CStringA Util::ConvertUnicodeToUTF8( LPCWSTR pszStr )
{
	if (!pszStr)
		return "";

	int len = _tcslen(pszStr);
	CStringA sr;
	UINT uLimit = 0x7fffffff;
	if (uLimit <= 0 || len == 0)
	{
		return sr;
	}
	UINT utf8len = 0;
	for (int i=0; i<len; ++i)
	{
		int marki = i;
		unsigned short us = pszStr[i];
		if (us == 0)
		{
			utf8len += 2;
		}
		else if (us <= 0x7f)
		{
			utf8len += 1;
		}
		else if (us <= 0x7ff)
		{
			utf8len += 2;
		}
		else if (0xd800 <= us && us <= 0xdbff && i+1<len)
		{
			unsigned short ul = pszStr[i+1];
			if (0xdc00 <= ul && ul <= 0xdfff)
			{
				++i;
				utf8len += 4;
			}
			else
			{
				utf8len += 3;
			}
		}
		else
		{
			utf8len += 3;
		}
		if (utf8len > uLimit)
		{
			len = marki;
			break;
		}
	}

	PBYTE pc = (PBYTE)sr.GetBuffer(utf8len+1);
	for (int i=0; i<len; ++i)
	{
		unsigned short us = pszStr[i];
		if (us == 0)
		{
			*pc ++ = 0xc0;
			*pc ++ = 0x80;
		}
		else if (us <= 0x7f)
		{
			*pc ++ = (char)us;
		}
		else if (us <= 0x7ff)
		{
			*pc ++ = 0xc0 | (us >> 6);
			*pc ++ = 0x80 | (us & 0x3f);
		}
		else if(0xd800 <= us && us <= 0xdbff && i+1<len)
		{
			unsigned short ul = pszStr[i+1];
			if (0xdc00 <= ul && ul <= 0xdfff)
			{
				++i;
				UINT bc = (us-0xD800)*0x400 + (ul-0xDC00) + 0x10000;
				*pc ++ = (BYTE)(0xf0 | ((bc >> 18) & 0x07));
				*pc ++ = (BYTE)(0x80 | ((bc >> 12) & 0x3f));
				*pc ++ = (BYTE)(0x80 | ((bc >>  6) & 0x3f));
				*pc ++ = (BYTE)(0x80 | ((bc      ) & 0x3f));
			}
			else
			{
				*pc ++ = (BYTE) (0xe0 | ((us >> 12) & 0x0f));
				*pc ++ = (BYTE) (0x80 | ((us >>  6) & 0x3f));
				*pc ++ = (BYTE) (0x80 | ((us      ) & 0x3f));
			}
		}
		else
		{
			*pc ++ = (BYTE) (0xe0 | ((us >> 12) & 0x0f));
			*pc ++ = (BYTE) (0x80 | ((us >>  6) & 0x3f));
			*pc ++ = (BYTE) (0x80 | ((us      ) & 0x3f));
		}
	}
	* pc++ = 0;
	sr.ReleaseBuffer();
	return sr;
}

char hexs[]={'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'};
CString Util::URLEncode( CString sIn )
{
	CString sOut;
	int nLen = sIn.GetLength();    
	PBYTE pInBuf = (PBYTE)sIn.GetBuffer(0);
	BYTE* pOutBuf=new BYTE[nLen*7];
	memset(pOutBuf,0,nLen*7);
	int n=0;
	BYTE A,B;
	for(int i=0;i<nLen;i++)
	{    BYTE a = pInBuf[i*2];
	BYTE b = pInBuf[i*2+1];
	if(b>0)
	{     

		pOutBuf[n++]='%';
		pOutBuf[n++]='u';

		A=b&0xf;
		B=(b&0xf0)>>4;
		pOutBuf[n++] = hexs[B];
		pOutBuf[n++] = hexs[A];

		A=a&0xf;
		B=(a&0xf0)>>4;
		pOutBuf[n++] = hexs[B];
		pOutBuf[n++] = hexs[A];
	}
	else if(a<'*'||a==','||(a>'9'&&a<'@')||(a>'Z'&&a<'_')||a=='`'||a>'z')
	{
		pOutBuf[n++]='%';

		A=a&0xf;
		B=(a&0xf0)>>4;

		pOutBuf[n++] = hexs[B];
		pOutBuf[n++] = hexs[A];
	}
	else
	{
		pOutBuf[n++]=a;
	}
	}

	sOut=CString(pOutBuf);
	sIn.ReleaseBuffer();
	delete[] pOutBuf;
	return sOut;
}

BOOL Util::ExtractDataByTag( CString strData, CString& strRet, CString strTag, CString strPreTag, CString strPostTag )
{
	//定位Tag
	int nIndex = strData.Find(strTag);
	if (nIndex == -1)
	{
		return FALSE;
	}
	strData = strData.Right(strData.GetLength() - nIndex - strTag.GetLength() + 1);

	//定位PreTag
	nIndex = strData.Find(strPreTag);
	if(nIndex == -1)
	{
		return FALSE;
	}
	strData = strData.Right(strData.GetLength() - nIndex - strPreTag.GetLength());

  if(!strPostTag.IsEmpty())
  {
	  nIndex = strData.Find(strPostTag);
	  if (nIndex == -1)
	  {
		  return FALSE;
	  }
	  strRet = strData.Left(nIndex);
  }
  else
  {
    strRet = strData;
  }

	return TRUE;
}

BOOL Util::VerifyXmlData( CStringA strXmlData )
{
	for (int i = 0; i < strXmlData.GetLength(); i++)
	{
		char aChar = strXmlData.GetAt(strXmlData.GetLength() - i - 1);
		if (isprint(aChar) && aChar != ' ')
		{
			if (aChar == '>')
			{
				return TRUE;
			}
			else
			{
				return FALSE;
			}
		}
	}

	return FALSE;
}

CString Util::AppendIdToUrl( CString strUrl )
{
	CString strRetUrl;

	if (strUrl.Find(L"?") != -1)
	{
		strRetUrl.Format(L"%s&t=%d_%d", strUrl, GetTickCount(), ::GetCurrentProcessId());
	}
	else
	{
		strRetUrl.Format(L"%s?t=%d_%d", strUrl, GetTickCount(), ::GetCurrentProcessId());
	}

	return strRetUrl;
}

BOOL Util::GetModuleAddress( HMODULE hMod, LPVOID& pBeginAddr, LPVOID& pEndAddr )
{
	if (!hMod)
	{
		return FALSE;
	}

	MEMORY_BASIC_INFORMATION mi;
	int Size = 0;
	HANDLE hProcess = GetCurrentProcess();
	VirtualQueryEx(hProcess, hMod, &mi, sizeof(mi));
	PVOID BaseAddress = mi.AllocationBase;

	while(mi.AllocationBase == BaseAddress)
	{
		Size += mi.RegionSize;
		VirtualQueryEx(hProcess,(PBYTE)hMod + Size,&mi,sizeof(mi));
	}
	
	pBeginAddr = (LPVOID)hMod;
	pEndAddr = LPVOID((DWORD)pBeginAddr + Size);
	return TRUE;
}

/************************************************************************/
/* 函数说明：根据特征码扫描基址
/* 参数一：process 要查找的进程
/* 参数二：markCode 特征码字符串,不能有空格
/* 参数三：特征码离基址的距离，默认距离：1
/* 参数四：findMode 扫描方式，找到特征码后，默认为：1
/*                  0：往上找基址（特征码在基址下面）
/*                  1：往下找基址（特征码在基址上面）
/* 参数五：offset 保存基址距离进程的偏移，默认为：不保存
/************************************************************************/
DWORD ScanAddress(HANDLE process, char *markCode, DWORD distinct = 1, DWORD findMode = 1, LPDWORD offset = NULL)
{
	union Base
	{
		DWORD	address;
		BYTE	data[4];
	};
	 
	//起始地址
	const DWORD beginAddr = 0x00400000;
	//结束地址
	const DWORD endAddr = 0x7FFFFFFF;
	//每次读取游戏内存数目的大小
	const DWORD pageSize = 4096;

	////////////////////////处理特征码/////////////////////
	//特征码长度不能为单数
	if (strlen(markCode) % 2 != 0) 
		return 0;
	//特征码长度
	int len = strlen(markCode) / 2;
	//将特征码转换成byte型
	BYTE *m_code = new BYTE[len];
	for (int i = 0; i < len; i++)
	{
		char c[] = {markCode[i*2], markCode[i*2+1], '\0'};
		m_code[i] = (BYTE)::strtol(c, NULL, 16);
	}

	/////////////////////////查找特征码/////////////////////
	BOOL _break = FALSE;
	//用来保存在第几页中的第几个找到的特征码
	int curPage = 0;
	int curIndex = 0;
	Base base;
	//每页读取4096个字节
	BYTE page[pageSize];
	DWORD tmpAddr = beginAddr;
	while (tmpAddr <= endAddr - len)
	{
		::ReadProcessMemory(process, (LPCVOID)tmpAddr, &page, pageSize, 0);
		//在该页中查找特征码
		for (int i = 0; i < pageSize; i++)
		{
			for (int j = 0; j < len; j++)
			{
				//只要有一个与特征码对应不上则退出循环
				if (m_code[j] != page[i + j])
					break;
				//找到退出所有循环
				if (j == len - 1)
				{
					_break = TRUE;
					if (!findMode)
					{
						curIndex = i;
						base.data[0] = page[curIndex-distinct-4];
						base.data[1] = page[curIndex-distinct-3];
						base.data[2] = page[curIndex-distinct-2];
						base.data[3] = page[curIndex-distinct-1];
					}
					else
					{
						curIndex = i + j;
						base.data[0] = page[curIndex+distinct+1];
						base.data[1] = page[curIndex+distinct+2];
						base.data[2] = page[curIndex+distinct+3];
						base.data[3] = page[curIndex+distinct+4];
					}
					break;
				}
			}
			if (_break) break;
		}
		if (_break) break;
		curPage++;
		tmpAddr += pageSize;
	}
	if(offset != NULL)
	{
		*offset = curPage * pageSize + curIndex + beginAddr;
	}
	return base.address;
}

/************************************************************************ 
功能：    根据特征码及位置搜索基址 
参数：    pCharacter:    特征码BYTE数组 
        dwSize:        特征码数组大小 
        iOffset:    默认值0，可不填，为正时，向下扫描(地址上升)；为负时，向上扫描（地址减少） 
返回：    DWORD：        基地址 
************************************************************************/ 
DWORD Util::GetBaseAddress(PBYTE const pCharacter, DWORD dwSize, int iOffset/* = 0*/, HMODULE hModule/* = NULL*/) 
{ 
    SYSTEM_INFO si; 
    HANDLE hProcess; 
    PBYTE pCurPos, pTemp; 
    MEMORY_BASIC_INFORMATION mbi; 
 
    //如果dwSize为0，IsBadReadPtr()返回0，即FALSE，但!dwSize为TRUE 
    //如果dwSize不为0，而且pCharacter可读，返回FLASE，!dwSize为FALSE 
    //如果dwSize不为0，但pCharacter不可读，IsBadReadPtr返回TRUE，!dwSize是FLASE 
    if (IsBadReadPtr(pCharacter, dwSize) || !dwSize) 
    { 
        MessageBoxW(NULL, L"Sorry, pCharacter is invalid or dwSize is zero....", L"Warning", MB_OK | MB_ICONWARNING); 
        SetLastError(ERROR_INVALID_ADDRESS); 
        return 0; 
    } 
 
    //初始化操作 
    GetSystemInfo(&si); 
    hProcess = GetCurrentProcess(); 
	LPVOID lpBeginAddr = si.lpMinimumApplicationAddress;
	LPVOID lpEndAddr = si.lpMaximumApplicationAddress;
	if (hModule)
	{
		GetModuleAddress(hModule, lpBeginAddr, lpEndAddr);
	}

	int nHitCnt = 0;
    //开始扫描可执行内存 
    for (pCurPos = (LPBYTE)lpBeginAddr; pCurPos < (LPBYTE)lpEndAddr - dwSize; pCurPos = (PBYTE)mbi.BaseAddress + mbi.RegionSize) 
    { 
        //查询页面属性 
        VirtualQueryEx(hProcess, pCurPos, &mbi, sizeof(MEMORY_BASIC_INFORMATION)); 
        if (/*mbi.State == MEM_COMMIT &&*/ 
			(mbi.Protect == PAGE_EXECUTE_READ || mbi.Protect == PAGE_EXECUTE_READWRITE || mbi.Protect == PAGE_EXECUTE_WRITECOPY ||
			mbi.Protect == PAGE_READONLY || mbi.Protect == PAGE_READWRITE || mbi.Protect == PAGE_WRITECOPY) && 
			dwSize <= mbi.RegionSize) 
        { 
            for (pTemp = (PBYTE)mbi.BaseAddress; pTemp < (PBYTE)mbi.BaseAddress + mbi.RegionSize; pTemp ++) 
            { 
                //直接比较，因为是注入DLL，当然，EXE来说，你可以用ReadProcessMemory 
                if (!memcmp(pCharacter, pTemp, dwSize)) 
                { 
					/* MessageBoxW(NULL, L"Have fun...", L"Tips", MB_OK | MB_ICONINFORMATION); 
					SetLastError(0); */
					nHitCnt++;
                    //return *(DWORD *)(pTemp + dwSize + iOffset); 
                } 
            } 
        } 
    } 
 
    MessageBoxW(NULL, L"Sorry, could not find the characteristic...", L"Warning", MB_OK | MB_ICONWARNING); 
    SetLastError(ERROR_NOT_FOUND); 
    return 0; 
} 

CString Util::URLEncode2( CString sIn )
{
  CString sOut;
	int nLen = sIn.GetLength();    
	PBYTE pInBuf = (PBYTE)sIn.GetBuffer(0);
	BYTE* pOutBuf=new BYTE[nLen*7];
	memset(pOutBuf,0,nLen*7);
	int n=0;
	BYTE A,B;
	for(int i=0;i<nLen;i++)
	{    BYTE a = pInBuf[i*2];
	BYTE b = pInBuf[i*2+1];
	if(b>0)
	{     

		pOutBuf[n++]='%';
		pOutBuf[n++]='u';

		A=b&0xf;
		B=(b&0xf0)>>4;
		pOutBuf[n++] = hexs[B];
		pOutBuf[n++] = hexs[A];

		A=a&0xf;
		B=(a&0xf0)>>4;
		pOutBuf[n++] = hexs[B];
		pOutBuf[n++] = hexs[A];
	}
	else if(a<'*'||a==','||(a>'9'&&a<'@')||(a>'Z'&&a<'_')||a=='`'||a>'z' || a=='+' || a=='=' || a=='/')
	{
		pOutBuf[n++]='%';

		A=a&0xf;
		B=(a&0xf0)>>4;

		pOutBuf[n++] = hexs[B];
		pOutBuf[n++] = hexs[A];
	}
	else
	{
		pOutBuf[n++]=a;
	}
	}

	sOut=CString(pOutBuf);
	sIn.ReleaseBuffer();
	delete[] pOutBuf;
	return sOut;
}
