#include "stdafx.h"
#include "HttpSocket.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#define MAXHEADERSIZE 1024
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CHttpSocket::CHttpSocket()
{
	m_s=NULL;
	m_phostent=NULL;
	m_port=80;

	m_bConnected=FALSE;

	for(int i=0;i<256;i++)
		m_ipaddr[i]='\0';
	memset(m_requestheader,0,MAXHEADERSIZE);
	memset(m_ResponseHeader,0,MAXHEADERSIZE);

	m_nCurIndex = 0;		//
	m_bResponsed = FALSE;
	m_nResponseHeaderSize = -1;
}

CHttpSocket::~CHttpSocket()
{
	CloseSocket();
}

BOOL CHttpSocket::Socket()
{
	if(m_bConnected)
		return FALSE;

	struct protoent *ppe; 

	WORD wVersionRequested;
	WSADATA wsaData;
	int err;

	wVersionRequested = MAKEWORD( 2, 2 );

	err = WSAStartup( wVersionRequested, &wsaData );

	ppe=getprotobyname("tcp"); 
	int nErr = WSAGetLastError();
	
	//创建SOCKET对象
	m_s=socket(PF_INET,SOCK_STREAM,ppe->p_proto);
	if(m_s==INVALID_SOCKET)
	{
		MessageBox(NULL,"socket()函数执行失败!","错误",MB_OK);
		return FALSE;
	}

	return TRUE;

}

BOOL CHttpSocket::Connect(char *szHostName,int nPort)
{
	if(szHostName==NULL)
		return FALSE;

	///若已经连接,则先关闭
	if(m_bConnected)
	{
		CloseSocket();
	}

	///保存端口号
	m_port=nPort;

	///根据主机名获得IP地址
	m_phostent=gethostbyname(szHostName);
	if(m_phostent==NULL)
	{
		MessageBox(NULL,"gethostbyname()函数执行失败!","错误",MB_OK);
		return FALSE;
	}
	
	///连接
	struct in_addr ip_addr;
	memcpy(&ip_addr,m_phostent->h_addr_list[0],4);		///h_addr_list[0]里4个字节,每个字节8位
	
	struct sockaddr_in destaddr;
	memset((void *)&destaddr,0,sizeof(destaddr)); 
	destaddr.sin_family=AF_INET;
	destaddr.sin_port=htons(80);
	destaddr.sin_addr=ip_addr;

	///保存主机的IP地址字符串
	sprintf(m_ipaddr,"%d.%d.%d.%d",
		destaddr.sin_addr.S_un.S_un_b.s_b1,
		destaddr.sin_addr.S_un.S_un_b.s_b2,
		destaddr.sin_addr.S_un.S_un_b.s_b3,
		destaddr.sin_addr.S_un.S_un_b.s_b4);
	/*inet_addr();把带点的IP地址字符串转化为in_addr格式;
	  inet_ntoa();作用相反*/
	
	/*注意理解sturct in_addr 的结构:一个32位的数;一共同体的形式使用
	(1)每8位一个即s_b1~s_b4;
	(2)每16位一个即s_w1~s_w2;
	(3)32位s_addr
	struct   in_addr {
    union   {
			  struct{
				unsigned  char   s_b1,
								 s_b2,
								 s_b3,
								 s_b4;
					} S_un_b;
              struct{
				unsigned  short  s_w1,
                                 s_w2
			        }S_un_w;      
               unsigned long  S_addr;
			} S_un;
		};
	*/

	if(connect(m_s,(struct sockaddr*)&destaddr,sizeof(destaddr))!=0)
	{
		//CloseSocket();
		//m_s=NULL;
		MessageBox(NULL,"connect()函数执行失败!","错误",MB_OK);
		return FALSE;
	}

	///设置已经连接的标志
	m_bConnected=TRUE;
	return TRUE;
}

///根据请求的相对URL输出HTTP请求头
const char *CHttpSocket::FormatRequestHeader(char *pServer,char *pObject, long &Length,
									  char *pCookie,char *pReferer,long nFrom,
									  long nTo,int nServerType)
{
	char szPort[10];
	char szTemp[20];
	sprintf(szPort,"%d",m_port);
	memset(m_requestheader,'\0',1024);

	///第1行:方法,请求的路径,版本
	strcat(m_requestheader,"GET ");
	strcat(m_requestheader,pObject);
	strcat(m_requestheader," HTTP/1.1");
    strcat(m_requestheader,"\r\n");

	///第2行:主机
    strcat(m_requestheader,"Host:");
	strcat(m_requestheader,pServer);
    strcat(m_requestheader,"\r\n");

	///第3行:
	if(pReferer != NULL)
	{
		strcat(m_requestheader,"Referer:");
		strcat(m_requestheader,pReferer);
		strcat(m_requestheader,"\r\n");		
	}

	///第4行:接收的数据类型
    strcat(m_requestheader,"Accept:*/*");
    strcat(m_requestheader,"\r\n");

	///第5行:浏览器类型
    strcat(m_requestheader,"User-Agent:Mozilla/4.0 (compatible; MSIE 5.00; Windows 98)");
    strcat(m_requestheader,"\r\n");

	///第6行:连接设置,保持
	strcat(m_requestheader,"Connection:Keep-Alive");
	strcat(m_requestheader,"\r\n");

	///第7行:Cookie.
	if(pCookie != NULL)
	{
		strcat(m_requestheader,"Set Cookie:0");
		strcat(m_requestheader,pCookie);
		strcat(m_requestheader,"\r\n");
	}

	///第8行:请求的数据起始字节位置(断点续传的关键)
	if(nFrom > 0)
	{
		strcat(m_requestheader,"Range: bytes=");
		_ltoa(nFrom,szTemp,10);
		strcat(m_requestheader,szTemp);
		strcat(m_requestheader,"-");
		if(nTo > nFrom)
		{
			_ltoa(nTo,szTemp,10);
			strcat(m_requestheader,szTemp);
		}
		strcat(m_requestheader,"\r\n");
	}
	
	///最后一行:空行
	strcat(m_requestheader,"\r\n");

	///返回结果
	Length=strlen(m_requestheader);
	return m_requestheader;
}

///发送请求头
BOOL CHttpSocket::SendRequest(const char *pRequestHeader, long Length)
{
	if(!m_bConnected)
		return FALSE;

	if(pRequestHeader==NULL)
		pRequestHeader=m_requestheader;
	if(Length==0)
		Length=strlen(m_requestheader);

	if(send(m_s,pRequestHeader,Length,0)==SOCKET_ERROR)
	{
		MessageBox(NULL,"send()函数执行失败!","错误",MB_OK);
		return FALSE;
	}
	int nLength;
	GetResponseHeader(nLength);
	return TRUE;
}

long CHttpSocket::Receive(char* pBuffer,long nMaxLength)
{
	if(!m_bConnected)return NULL;

	///接收数据
	long nLength;
	nLength=recv(m_s,pBuffer,nMaxLength,0);
	
	if(nLength <= 0)
	{
		//MessageBox(NULL,"recv()函数执行失败!","错误",MB_OK);
		CloseSocket();
	}
	return nLength;
}

///关闭套接字
BOOL CHttpSocket::CloseSocket()
{
	if(m_s != NULL)
	{
		if(closesocket(m_s)==SOCKET_ERROR)
		{
			MessageBox(NULL,"closesocket()函数执行失败!","错误",MB_OK);
			return FALSE;
		}
	}
	m_s = NULL;
	m_bConnected=FALSE;
	return TRUE;
}

int CHttpSocket::GetRequestHeader(char *pHeader, int nMaxLength) const
{
	int nLength;
	if(int(strlen(m_requestheader))>nMaxLength)
	{
		nLength=nMaxLength;
	}
	else
	{
		nLength=strlen(m_requestheader);
	}
	memcpy(pHeader,m_requestheader,nLength);
	return nLength;
}

//设置接收或者发送的最长时间
BOOL CHttpSocket::SetTimeout(int nTime, int nType)
{
	if(nType == 0)
	{
		nType = SO_RCVTIMEO;
	}
	else
	{
		nType = SO_SNDTIMEO;
	}

	DWORD dwErr;
    dwErr=setsockopt(m_s,SOL_SOCKET,nType,(char*)&nTime,sizeof(nTime)); 
	if(dwErr)
	{
		MessageBox(NULL,"setsockopt()函数执行失败!","错误",MB_OK);
		return FALSE;
	}
	return TRUE;
}

//获取HTTP请求的返回头
const char* CHttpSocket::GetResponseHeader(int &nLength)
{
	if(!m_bResponsed)
	{
		char c = 0;
		int nIndex = 0;
		BOOL bEndResponse = FALSE;
		while(!bEndResponse && nIndex < MAXHEADERSIZE)
		{
			recv(m_s,&c,1,0);
			m_ResponseHeader[nIndex++] = c;
			if(nIndex >= 4)
			{
				if(m_ResponseHeader[nIndex - 4] == '\r' && m_ResponseHeader[nIndex - 3] == '\n'
					&& m_ResponseHeader[nIndex - 2] == '\r' && m_ResponseHeader[nIndex - 1] == '\n')
				bEndResponse = TRUE;
			}
		}
		m_nResponseHeaderSize = nIndex;
		m_bResponsed = TRUE;
	}
	
	nLength = m_nResponseHeaderSize;
	return m_ResponseHeader;
}

//返回HTTP响应头中的一行.
int CHttpSocket::GetResponseLine(char *pLine, int nMaxLength)
{
	if(m_nCurIndex >= m_nResponseHeaderSize)
	{
		m_nCurIndex = 0;
		return -1;
	}

	int nIndex = 0;
	char c = 0;
	do 
	{
		c = m_ResponseHeader[m_nCurIndex++];
		pLine[nIndex++] = c;
	} while(c != '\n' && m_nCurIndex < m_nResponseHeaderSize && nIndex < nMaxLength);
	
	return nIndex;
}

int CHttpSocket::GetField(const char *szSession, char *szValue, int nMaxLength)
{
	//取得某个域值
	if(!m_bResponsed) return -1;
	
	CString strRespons;
	strRespons = m_ResponseHeader;
	int nPos = -1;
	nPos = strRespons.Find(szSession,0);
	if(nPos != -1)
	{
		nPos += strlen(szSession);
		nPos += 2;
		int nCr = strRespons.Find("\r\n",nPos);
		CString strValue = strRespons.Mid(nPos,nCr - nPos);
		strcpy(szValue,strValue);
		return (nCr - nPos);
	}
	else
	{
		return -1;
	}
}

int CHttpSocket::GetServerState()
{
	//若没有取得响应头,返回失败
	if(!m_bResponsed) return -1;
	
	char szState[3];
	szState[0] = m_ResponseHeader[9];
	szState[1] = m_ResponseHeader[10];
	szState[2] = m_ResponseHeader[11];

	return atoi(szState);
}
