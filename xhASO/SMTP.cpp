// SMTP.cpp: 实现CSMTP类.
//
#include "stdafx.h"
#include "SMTP.h"
#include "base64.h"

//---------------------------------------------------------------------------
CSMTP::CSMTP( LPSTR szSMTPServerName, UINT nPort )
{
	ASSERT( szSMTPServerName != NULL );
	AfxSocketInit();
	m_sSMTPServerHostName = szSMTPServerName;
	m_nPort = nPort;
	m_bConnected = FALSE;
	m_sError = _T( "OK" );
}

//---------------------------------------------------------------------------
CSMTP::~CSMTP()
{
	if( m_bConnected )
		Disconnect();
}

//---------------------------------------------------------------------------
BOOL CSMTP::Connect()
{
	char sHello[MAX_PATH] = {0};
	TCHAR local_host[80] = {0};
	char szLocal_host[80] = {0};

	if( m_bConnected )
	{
		return TRUE;
	}

    // 创建Socket.
	if( !m_wsSMTPServer.Create() )
	{
		m_sError = _T( "Unable to create the socket." );
		return FALSE;
	}

    // 开始连接.
	if( !m_wsSMTPServer.Connect( GetServerHostName(), GetPort() ) )
	{
		m_sError = _T( "Unable to connect to server" );
		m_wsSMTPServer.Close();
		return FALSE;
	}

    // 检验应答码是否为220.
	if(!get_response(_T("Connect"),220)){
		m_wsSMTPServer.Close();
		return FALSE;
	}

    // 发送HELO标识.
	gethostname( szLocal_host, 80 );
	sprintf(sHello,"HELO %s\r\n", szLocal_host );

	int totalSend = m_wsSMTPServer.Send((void*)sHello, strlen(sHello) );
	if (SOCKET_ERROR == totalSend)
	{
		return FALSE;
	}

    // 检验应答码是否为250.
	if(!get_response(_T("HELO"),250))
	{
		m_wsSMTPServer.Close();
		return FALSE;
	}

	m_bConnected = TRUE;
	return TRUE;
}

//---------------------------------------------------------------------------
BOOL CSMTP::Disconnect()
{
	BOOL ret;
	if( !m_bConnected )
	{
		return TRUE;
	}

    // 向服务器发送退出标识和关闭Socket.
	char sQuit[] = "QUIT\r\n";
	m_wsSMTPServer.Send( (void*)sQuit,strlen(sQuit));

    // 验证QUIT,不需要返回.	
	ret =get_response(_T("QUIT"),221);

	m_wsSMTPServer.Close();
	m_bConnected = FALSE;

	return ret;
}

//---------------------------------------------------------------------------
void CSMTP::SetServerProperties( LPCTSTR sServerHostName, UINT nPort)
{
	ASSERT( sServerHostName != NULL );

	// 没有服务器名返回.
	if( sServerHostName == NULL )
		return;
	
	m_sSMTPServerHostName = sServerHostName;
	m_nPort = nPort;
}

//---------------------------------------------------------------------------
// 输入参数为希望的应答码.
BOOL CSMTP::get_response(LPCTSTR sDes,UINT response_expected )
{
	char retValue[4] = {0};
	UINT response = 0;

	// m_wsSMTPServer为CSocket的类对象，调用Receive()将应答码接收到缓存
	// response_buf中.
	memset(response_buf,0x0,RESPONSE_BUFFER_SIZE);
	if( m_wsSMTPServer.Receive( response_buf, RESPONSE_BUFFER_SIZE ) == SOCKET_ERROR )
	{
		m_sError = _T( "Socket Error" );
		return FALSE;
	}

	memcpy(retValue,response_buf,3);
	response = atoi(retValue);
 
	// 检验收到的应答码是否是所希望得到的.
	if( response != response_expected )
	{
		return FALSE;
	}

	return TRUE;
}

//---------------------------------------------------------------------------
BOOL CSMTP::AuthCheck(CMailMessage msg)
{
	// 发送验证标识.	
	char sTemp[] = "AUTH LOGIN\r\n";
	m_wsSMTPServer.Send( (char*)sTemp,strlen(sTemp));

    // 检验应答码是否为334.
	if(!get_response(_T("AUTH LOGIN"),334)) 
	{
		return FALSE;
	}

    // 验证用户名.
	CBase64 code;
	string sUsername=msg.m_sUsername;
	string strEncode = code.Encode((LPSTR)sUsername.c_str(),sUsername.length());
	strEncode += "\r\n";
	m_wsSMTPServer.Send((void*)strEncode.c_str(),strEncode.length() );

    // 检验应答码是否为334.
	if(!get_response(_T("Username"),334)) 
	{
		return FALSE;
	}

    // 验证密码.     
	string sPassword=msg.m_sPassword;
	strEncode = code.Encode((LPSTR)sPassword.c_str(),sPassword.length());
	strEncode += "\r\n";
	m_wsSMTPServer.Send((void*)strEncode.c_str(),strEncode.length() );
    
	// 检验应答码是否为235.
	if(!get_response(_T("Password"),235)) 
	{
		return FALSE;
	}
    
	return TRUE;
}

//---------------------------------------------------------------------------
BOOL CSMTP::SendEmailMessage(CMailMessage msg)
{
	char sFrom[MAX_PATH] = {0};
	char sTo[MAX_PATH] = {0};
	char sTemp[MAX_PATH] = {0};

    // 连接服务器.
    if(!Connect()) 
	{
		return FALSE;
	}

	msg.FormatMessage();

    // 身份验证.
	if(msg.m_bCheck)
	{
	    if(!AuthCheck(msg)) 
		{
			return FALSE;
		}
	}

    // 格式化并发送MAIL命令，并接收、验证服务器应答码.
	sprintf(sFrom,"MAIL FROM: <%s>\r\n",msg.m_sFrom.c_str());
	m_wsSMTPServer.Send((void*)sFrom, strlen(sFrom) );

    // 检验应答码是否为250.
	if(!get_response(_T("MAIL FROM:"),250)) 
	{
		return FALSE;
	}

    // 格式化并发送RCPT命令,并接收、验证服务器应答码.
	sprintf(sTo,"RCPT TO: <%s>\r\n",msg.m_sTo.c_str());
	m_wsSMTPServer.Send( (void*)sTo, strlen(sTo) );

    // 检验应答码是否为250.
	if(!get_response(_T("RCPT TO:"),250))
	{
		return FALSE;
	}
	
    // 格式化并发送DATA命令，并接收、验证服务器应答码.
	char strData[] = "DATA\r\n";
	m_wsSMTPServer.Send((void*)strData, strlen(strData) );

	// 检验应答码是否为354.
	if(!get_response(_T("DATA"),354)) 
	{
		return FALSE;
	}

	// 发送根据RFC 822文档规定格式化过的邮件头.
	strcpy(sTemp,msg.m_sHeader.c_str());
	m_wsSMTPServer.Send( (void*)sTemp, strlen(sTemp) );

    // 发送邮件体.
	string strTmp = cook_body( msg.m_sBody );
	m_wsSMTPServer.Send((void*)strTmp.c_str(),strTmp.length());

    // 发送内容数据结束标志"<CRLF>.<CRLF>".
	char strEndTag[] = "\r\n.\r\n";
	m_wsSMTPServer.Send( (void*)strEndTag,strlen(strEndTag));
    
	// 检验应答码是否为250.
	if(!get_response(_T("."),250)) 
	{
		return FALSE;
	}

    // 断开服务器.
	if(!Disconnect())
	{
		return FALSE;
	}

	return TRUE;
}

//---------------------------------------------------------------------------
string CSMTP::cook_body(string sBody)
{
	string sTemp;
	string sCooked = "";
	LPSTR szBad   = "\r\n.\r\n";
	LPSTR szGood  = "\r\n..\r\n";
	int nPos;
	int nStart = 0;
	int nBadLength = strlen( szBad );
	sTemp = sBody;

	if( sTemp.substr(0,3) == ".\r\n" )
		sTemp = "." + sTemp;

	while( (nPos = sTemp.find( szBad )) > -1 )
	{
		sCooked = sTemp.substr( nStart, nPos );
		sCooked += szGood;
		sTemp = sCooked + sTemp.substr(nPos + nBadLength, sTemp.length() - (nPos + nBadLength) );
	}

	return sTemp;
}

//---------------------------------------------------------------------------
void CSMTP::Cancel()
{
   // 取消块调用.
   m_wsSMTPServer.CancelBlockingCall();

   // 立即关闭SOCKET.
   m_wsSMTPServer.Close();
}
