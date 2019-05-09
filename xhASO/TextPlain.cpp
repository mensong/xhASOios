// TextPlain.cpp: 实现CTextPlain类.
//
#include "stdafx.h"
#include "TextPlain.h"
#include "MailMessage.h"

//---------------------------------------------------------------------------
CTextPlain::CTextPlain(UINT nWrapPos )
{
	m_nWrapPos = nWrapPos;	
}

//---------------------------------------------------------------------------
CTextPlain::~CTextPlain()
{
}

//---------------------------------------------------------------------------
string CTextPlain::GetContentTypeString()
{
	string s;
	s = "text/plain";
	return s;
}

//---------------------------------------------------------------------------
BOOL CTextPlain::AppendPart(LPSTR szContent, 
							LPSTR szParameters, 
							int nEncoding, 
							string & sDestination)
{
	string sSubHeader;
	string sWrapped;
	sSubHeader = build_sub_header( szContent,
								   szParameters,
								   nEncoding);
	sWrapped = wrap_text( szContent );
	sDestination += (sSubHeader + sWrapped);
	return TRUE;
}

//---------------------------------------------------------------------------
string CTextPlain::build_sub_header(LPSTR szContent, 
									 LPSTR szParameters, 
									 int nEncoding)
{
	string sSubHeader;
	sSubHeader = "Content-Type: ";
	sSubHeader += GetContentTypeString().c_str();
	sSubHeader += szParameters;
	sSubHeader += "\r\n";				   
	sSubHeader += "Content-Transfer-Encoding: ";
	switch( nEncoding )
	{
		// 此类只处理 7bit 编码, 但其他的可以在此添加.
		default:
		// 处理7bit 编码...
		case CMailMessage::_7BIT:
			sSubHeader += "7Bit";
	}

	sSubHeader += "\r\n\r\n";
	return sSubHeader;
}

//---------------------------------------------------------------------------
string CTextPlain::wrap_text(LPSTR szText)
{
	string sTemp;
	string sLeft;
	string sRight;
	int lp = 0;
	UINT nCount = 0;
	int nSpacePos = 0;

	ASSERT( szText != NULL );
	if( szText == NULL )
	{
		return sTemp;
	}
	sTemp = szText;
	while( lp < sTemp.length() )
	{
		if( sTemp[ lp ] == ' ' )
		{
			nSpacePos = lp;
		}

		// 重置新行计数.
		if( sTemp.substr( lp, 2 ) == "\r\n" )
		{
			nCount = 0;
		}
		// 在最后找到的空格出包裹文本.
		if( nCount > m_nWrapPos )
		{
			sLeft = sTemp.substr(0, nSpacePos );
			sRight = sTemp.substr(nSpacePos, sTemp.length()-nSpacePos );

			//sLeft.TrimRight();
			//sRight.TrimLeft();
			sLeft += "\r\n";
			sTemp = sLeft + sRight;
			nCount = 0;
		}
		else
		{
			nCount++;
		}
		lp++;
	}
	return sTemp;
}
