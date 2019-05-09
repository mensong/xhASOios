// TextPlain.h: CTextPlain ¿‡∂®“Â.
//
#include <iostream>
#include <string>
using namespace std;

#ifndef _TEXTPLAIN_H__INCLUDED
#define _TEXTPLAIN_H__INCLUDED

class CTextPlain
{
public:
	CTextPlain(UINT nWrapPos = 72 );
	virtual ~CTextPlain();

	virtual BOOL AppendPart( LPSTR szContent,
		       LPSTR szParameters, int nEncoding, string& sDestination );
	virtual string GetContentTypeString();

protected:
	UINT m_nWrapPos;

	string wrap_text( LPSTR szText );
	virtual string build_sub_header( LPSTR szContent, 
		      LPSTR szParameters, int nEncoding);
};

#endif 
