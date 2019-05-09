// MailMessage.h: CMailMessage类定义.
//
#include <iostream>
#include <string>
using namespace std;
#ifndef _MAILMESSAGE_H__INCLUDED
#define _MAILMESSAGE_H__INCLUDED

class CMailMessage  
{
public:
	CMailMessage();
	virtual ~CMailMessage();

private:
	void FormatMessage();  // 格式化发送消息.
	void prepare_header();
	void prepare_body();
	void insert_boundary( string& sText );
	void insert_message_end(string& sText );

public:
	string m_sFrom;
	string m_sTo;
	string m_sUsername;
	string m_sPassword;
	string m_sSubject;
	string m_sText;
	string m_sAttachment;
	BOOL    m_bCheck;

private:
	string m_sHeader;
	string m_sBody;
	string m_sNoMIMEText;
	string m_sPartBoundary;
	string m_sMIMEContentType;
	string m_sMailerName;

public:
	enum EMIMEEncodingCode
	{
		_7BIT = 0,
		_8BIT,
		BINARY,
		QUOTED_PRINTABLE,
		BASE64,
		NEXT_FREE_ENCODING_CODE
	};
	friend class CSMTP;
};

#endif 