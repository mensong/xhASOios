// MailMessage.cpp: 实现CMailMessage类.
//
#include "stdafx.h"
#include "MailMessage.h"
#include "TextPlain.h"
#include "AppOctetStream.h"

//---------------------------------------------------------------------------
CMailMessage::CMailMessage()
{
	m_sMIMEContentType = "multipart/mixed";
	m_sMailerName = "Test Mail";
	m_sPartBoundary = "Test_Mail_Part_Boundary_09182005";
	m_sNoMIMEText = "This is a multipart message in MIME format.";

	m_sSubject= "Test Email";
	m_sUsername = "";
	m_sPassword = "";
	m_sFrom = "";
	m_sTo = "";
	m_sText = "";
	m_sAttachment = "";
	m_sHeader = "";
	m_sBody = "";
	m_bCheck = TRUE;
}

//---------------------------------------------------------------------------
CMailMessage::~CMailMessage()
{
}

//---------------------------------------------------------------------------
void CMailMessage::FormatMessage()
{
	prepare_header();
	prepare_body();
}

//---------------------------------------------------------------------------
void CMailMessage::prepare_header()
{
	CTime tDateTime= CTime::GetCurrentTime();

	// Format: Mon, day Jun Year H:M:S +hhmm (国际时差)
	char strDate[MAX_PATH] = {0};
	CString strdwDate = tDateTime.Format( TEXT("%a, %d %b %y %H:%M:%S +0800") );

	WideCharToMultiByte(CP_ACP,0,strdwDate.GetBuffer(),strdwDate.GetLength(),strDate,MAX_PATH,NULL,NULL);

	char sTemp1[MAX_PATH] = {0};
	char sTemp2[512] = {0};

	sprintf(sTemp2,"Date: %s\r\nFrom: %s\r\nTo: %s\r\nSubject: %s\r\nX-Mailer: %s\r\n\r\n",
				   // 根据需要包括其它扩展行.
				   strDate,
				   m_sFrom.c_str(),
				   m_sTo.c_str(),
				   m_sSubject.c_str(),
				   m_sMailerName.c_str());


	sprintf(sTemp2,"MIME-Version: 1.0\r\nContent-Type: %s; boundary=%s\r\nDate: %s\r\nFrom: %s\r\nTo: %s\r\nSubject: %s\r\nX-Mailer: %s\r\n\r\n",
					// 根据需要包括其它扩展行.
					m_sMIMEContentType.c_str(),
				    m_sPartBoundary.c_str(),
					strDate,
					m_sFrom.c_str(),
					m_sTo.c_str(),
					m_sSubject.c_str(),
					m_sMailerName.c_str());

	if(m_sAttachment.empty())
		m_sHeader = sTemp1;
     else  
	    m_sHeader = sTemp2;
}

//---------------------------------------------------------------------------
void CMailMessage::prepare_body()
{
	if(m_sAttachment.empty())
	{
		m_sBody=m_sText;
	     return;
	}

	// 初始化邮件体(取代当前内容).
	m_sBody = m_sNoMIMEText;
	m_sBody += "\r\n\r\n";

	CTextPlain* pType;
	insert_boundary( m_sBody );
	pType = new CTextPlain(76);

	pType->AppendPart((LPSTR)m_sText.c_str(),"", _7BIT,m_sBody );
    delete[] pType;

	insert_boundary( m_sBody );

	CAppOctetStream* pMIMEType;
	pMIMEType = new CAppOctetStream();
	pMIMEType->AppendPart((LPSTR)m_sAttachment.c_str(),"", BASE64,m_sBody,FALSE);
	delete[] pMIMEType;

	insert_message_end( m_sBody );

	// 根据需要给邮件体附加一个 CR/LF.
	if( m_sBody.at(m_sBody.length()-2) != '\r' && m_sBody.at(m_sBody.length()-1) != '\n')	
	{
		m_sBody += "\r\n";
	}
}

//---------------------------------------------------------------------------
void CMailMessage::insert_boundary(string& sText )
{
	char sTemp[MAX_PATH] = {0};
	if( sText.at(sText.length()-2) != '\r' && sText.at(sText.length()-1) != '\n')
	{
		sText += "\r\n";
	}
	sprintf(sTemp,"--%s\r\n",m_sPartBoundary.c_str() );
	sText += sTemp;
}

//---------------------------------------------------------------------------
void CMailMessage::insert_message_end( string& sText )
{
	char sTemp[MAX_PATH] = {0};
	if( sText.at(sText.length()-2) != '\r' && sText.at(sText.length()-1) != '\n')
	{
		sText += "\r\n";
	}
	sprintf(sTemp,"--%s--\r\n",m_sPartBoundary.c_str() );
	sText += sTemp;
}
