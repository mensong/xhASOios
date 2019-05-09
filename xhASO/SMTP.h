// SMTP.h: CSMTP 类定义.
//
#ifndef _SMTP_H__INCLUDED
#define _SMTP_H__INCLUDED

#include <afxsock.h>
#include "MailMessage.h"

#define SMTP_PORT 25		// SMTP服务器的标准端口.
#define RESPONSE_BUFFER_SIZE 1024

class CSMTP  
{
public:
	CSMTP( LPSTR szSMTPServerName, UINT nPort = SMTP_PORT );
	virtual ~CSMTP();

public:
	void SetServerProperties( LPCTSTR sServerHostName, UINT nPort = SMTP_PORT );
	UINT GetPort()              { return m_nPort; }
	CString GetLastError()      { return m_sError; }
	CString GetServerHostName() { return m_sSMTPServerHostName; }
	BOOL Disconnect();
	BOOL Connect();
	BOOL SendEmailMessage(CMailMessage msg);
	BOOL AuthCheck(CMailMessage msg);

	void Cancel();
	string cook_body(string sBody);

private:
	BOOL get_response(LPCTSTR sDes,UINT response_expected);

	CString m_sError;
	BOOL m_bConnected;
	UINT m_nPort;
	CString m_sSMTPServerHostName;
	CSocket m_wsSMTPServer;

protected:

	char response_buf[ RESPONSE_BUFFER_SIZE ];
};

#endif
