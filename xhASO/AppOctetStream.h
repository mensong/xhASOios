// AppOctetStream.h: CAppOctetStream ¿‡∂®“Â.
//
#include <iostream>
#include <string>
using namespace std;

#ifndef _APPOCTETSTREAM_H__INCLUDED
#define _APPOCTETSTREAM_H__INCLUDED

class CAppOctetStream   
{
public:
	virtual string GetContentTypeString();
	CAppOctetStream();
	virtual ~CAppOctetStream();

	virtual BOOL AppendPart( LPSTR szContent, 
			 LPSTR szParameters,int nEncoding, 
			 string& sDestination,BOOL bPath);

protected:
	virtual void attach_file( CStdioFile* pFileAtt,
		     int nEncoding, string& sDestination );
	
	virtual string build_sub_header( LPSTR szContent,
			LPSTR szParameters,int nEncoding,BOOL bPath);
};

#endif
