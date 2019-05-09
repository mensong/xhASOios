// AppOctetStream.cpp: 实现CAppOctetStream类.
//
#include "stdafx.h"
#include "AppOctetStream.h"
#include "Base64.h"
#include "MailMessage.h"


#define BYTES_TO_READ 54 // 此数保证输出不超过行的最大长度.

//---------------------------------------------------------------------------
CAppOctetStream::CAppOctetStream()
{
}

//---------------------------------------------------------------------------
CAppOctetStream::~CAppOctetStream()
{
}

//---------------------------------------------------------------------------
BOOL CAppOctetStream::AppendPart(LPSTR szContent, 
								 LPSTR szParameters, 
								 int nEncoding, 
								 string & sDestination,BOOL bPath)
{
	CStdioFile fAttachment;
	TCHAR strContent[MAX_PATH] = {0};

	ASSERT( szContent != NULL );

	// 此类只处理附件,所以忽略bPath参数.
	if( szContent == NULL )
	{	
		return FALSE;
	}

	::MultiByteToWideChar(CP_ACP,0,szContent,strlen(szContent),strContent,MAX_PATH);

	if( !fAttachment.Open(strContent,(CFile::modeRead |
		                      CFile::shareDenyWrite | CFile::typeBinary) ) )
	{
		return FALSE;
	}
	
	sDestination += build_sub_header( szContent,szParameters,
									  nEncoding, bPath);
	attach_file( &fAttachment, CMailMessage::BASE64, sDestination );
	fAttachment.Close();
	return TRUE;
}

//---------------------------------------------------------------------------
string CAppOctetStream::build_sub_header(LPSTR szContent, 
										  LPSTR szParameters, 
										  int nEncoding,BOOL bPath)
{
	string sSubHeader;
	char sTemp[MAX_PATH] = {0};
	char szFName[ _MAX_FNAME ] = {0};
	char szExt[ _MAX_EXT ] = {0};

	_splitpath( szContent, NULL, NULL, szFName, szExt );

	// 此类忽略szParameters 和 nEncoding.
	// 它控制自己的参数和只处理 Base64 编码.
	if( bPath )
	{
		sprintf(sTemp,"; file=%s%s", szFName, szExt );
	}
	else
	{
		memset(sTemp,0x0,MAX_PATH);
	}
	
	sSubHeader = "Content-Type: ";
	sSubHeader += GetContentTypeString();
	sSubHeader += sTemp;
	sSubHeader += "\r\n";
	sSubHeader += "Content-Transfer-Encoding: base64\r\nContent-Disposition: attachment; filename=";
	sSubHeader += szFName;
	sSubHeader += szExt;
	sSubHeader += "\r\n";

	// 标示子标头结束.
	sSubHeader += "\r\n"; 
	
	return sSubHeader;
}

//---------------------------------------------------------------------------
string CAppOctetStream::GetContentTypeString()
{
	string s;
	s = "application/octet-stream";
	return s;
}

//---------------------------------------------------------------------------
// 增加邮件附件.
void CAppOctetStream::attach_file(CStdioFile* pFileAtt, 
								  int nEncoding, 
								  string & sDestination)
{
	CBase64* pEncoder;
	int nBytesRead;
	char szBuffer[ BYTES_TO_READ + 1 ];
 
	ASSERT( pFileAtt != NULL );
	if( pFileAtt == NULL )
		return;
	switch( nEncoding )
	{
		// 此类只处理 7bit 编码, 但其他的可以在此添加.
		default:
		// 处理7bit 编码...
		case CMailMessage::BASE64:
			try 
			{
				pEncoder = new CBase64;
			}
			catch( CMemoryException* e )
			{
				delete e;
				return;
			}
	}
	if( pEncoder == NULL )
		return;
	do
	{
		try
		{
			nBytesRead = pFileAtt->Read( szBuffer, BYTES_TO_READ );
		}
		catch( CFileException* e )
		{
			delete e;
			break;
		}
		szBuffer[ nBytesRead ] = 0;	// 字符串结尾.
		
		sDestination += pEncoder->Encode( szBuffer, nBytesRead );

		sDestination += "\r\n";
	} while( nBytesRead == BYTES_TO_READ );
	sDestination += "\r\n";
	delete pEncoder;
}
