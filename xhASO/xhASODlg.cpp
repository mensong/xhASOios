
// xhASODlg.cpp : implementation file
//

#include "stdafx.h"
#include "xhASO.h"
#include "xhASODlg.h"
#include "AutoDial.h"
#include "vpndial.h"
#include "statusData.h"
#include <iostream>
#include <string>
#include <Winsock2.h>
#include <iphlpapi.h>
#include<locale.h> 
#include "requestInterface.h"
#include "json.h"
#include "memoryshared.h"
using namespace std;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#pragma warning(disable:4996)
#pragma warning(disable:4995)

#define     MAX_TITLE          64
#define     MAX_BODY           512
#define     MAX_KEY_WORD       64  
#define     COMMENTED          TEXT("已评论")
#define     COMMENTING         TEXT("正在评论")
#define		LOGINED			   TEXT("已登录")
#define     LOGIN_READY		   TEXT("已加载")
#define     ADSLCONNECT        TEXT("宽带连接")
#define     VPN_CONNECTION     TEXT("vpn connection")

#define     KEEPALIVE_HEARTBEAT_NOTIFY		2002
#define     TIME_SERVER_NOTIFY				2003
#define		SECOND_TIME_NOTIFY				2004

#define     OK_TASK							1
#define		NO_TASK							0
#define		COMMENT_TASK					1
#define		ASO_TASK						2
#define		RANK_TASK						3
#define		LOCAL_NETWORK                   1
#define		VPN_NETWORK						2
#define		DIALUP_NETWORK					3
#define		PROXY_NETWORK					4
#define		TASK_PROCESS_MAX_RUN_TIME		60*5 //5分钟,如果一个任务进程5分钟内没有执行完，就干预强制结束

// Link with Iphlpapi.lib
#pragma comment(lib, "IPHLPAPI.lib")

#define WORKING_BUFFER_SIZE 15000
#define MAX_TRIES 3

#define MALLOC(x) HeapAlloc(GetProcessHeap(), 0, (x))
#define FREE(x) HeapFree(GetProcessHeap(), 0, (x))

CRITICAL_SECTION g_cs;

typedef BOOL (* PINITIALIZEMODULE)(LPVOID strPath);
typedef void (* PUNINITIALIZEMODULE)();
PINITIALIZEMODULE pInitialModule = NULL;
PUNINITIALIZEMODULE pUninitialModule = NULL;

typedef VOID (*PXHINITCRITICALSECTION)();
typedef VOID (*PXHDELETECRITICALSECTION)();
typedef LPVOID (*PXHCREATENETWORKOBJECT)();
typedef VOID (*PXHRELEASENEWTWORKOBJECT)(LPVOID lpNetwork);
typedef int (*PXHGSA_APPLE_THREAD_NETWORKVERIFIER)(IN TCHAR* lpUdid,TCHAR* lpProxy,int netType);
typedef int (*PXHSETUAINFO)(IN LPVOID lpNetwork,PMODELINFO pModInfo);
typedef int (*PXHGSA_APPLE_NETWORKVERIFIER)(IN LPVOID lpNetwork,IN LPVOID lpUdid,PMODELINFO pModInfo,PDEVICEINFO pDeviceInfo,TCHAR* lpProxy,int netType);

typedef int (*PXHLOGINAPPSTORE)(IN LPVOID lpNetwork,
								 IN LPVOID lpUserID,
								 IN LPVOID lpPassword,
								 IN LPTSTR lpwFatherAndMother,
							     IN LPTSTR lpwTeacher,
							     IN LPTSTR lpwBook,
								 IN PDEVICEINFO pDeviceInfo,
								 OUT LPVOID lpPasswordToken,
								 OUT LPVOID lpDsid,
								 TCHAR* lpProxy,
								 int netType);

typedef int (*PXHDOWNLOADAPP)(IN LPVOID lpNetwork,
							   IN LPVOID lpPasswordToken,
							   IN LPVOID lpDsid,
							   IN LPVOID lpSaleId,
							   IN LPVOID lpAppPrice,
							   BOOL bSendReport,
							   BOOL bPaidApp);
typedef int (*PXHCOMMENTAPP)(IN LPVOID lpNetwork,
							  IN LPVOID lpPasswordToken,
							  IN LPVOID lpDsid,
							  IN LPVOID lpSaleId,
							  IN int rating,
							  IN LPTSTR lpwNickname,
							  IN LPTSTR lpwTitle,
							  IN LPTSTR lpwBody);
typedef int (*PXHSEARCHKEYHOTWORD)(IN LPVOID lpNetwork,
									IN LPTSTR strwKeyword,
									IN LPVOID lpSaleId,
									IN LPVOID lpDsid,
									IN LPVOID lpPasswordToken,
									IN LPVOID lpTargetUrl);

typedef int (*PXHGETAPPEXTVRSIDVALUE)(IN LPVOID lpNetwork,
									   IN LPVOID lpPasswordToken,
									   IN LPVOID lpDsid,
									   IN LPVOID lpTargetUrl);

typedef int (*PXHBUYCONFIRMACTIVEAPP)(IN LPVOID lpNetwork,
									   IN LPVOID lpPasswordToken,
									   IN LPVOID lpSaleId,
									   IN LPVOID lpDsid,
									   IN LPVOID lpTargetUrl);

typedef  int (*PXHONLYSEARCHKEYWORD)(IN LPVOID lpNetwork,IN LPTSTR strwKeyword);

typedef int (*PXHREGISTERAPPLEID)(IN LPVOID lpNetwork,TAGREGISTERINFO& tagRegisterInfo);

PXHINITCRITICALSECTION pInitCriticalSection = NULL;
PXHDELETECRITICALSECTION pDeleteCriticalSection = NULL;
PXHCREATENETWORKOBJECT pCreateNetworkObject = NULL;
PXHRELEASENEWTWORKOBJECT pReleaseNetworkObject = NULL;
PXHGSA_APPLE_THREAD_NETWORKVERIFIER pGSA_Apple_Thread_NetworkVerifier = NULL;
PXHSETUAINFO pSetUAInfo = NULL;
PXHGSA_APPLE_NETWORKVERIFIER pGSA_Apple_NetworkVerifier = NULL;
PXHLOGINAPPSTORE pLoginAppStore = NULL; 
PXHDOWNLOADAPP pDownloadApp = NULL;
PXHCOMMENTAPP pCommentApp = NULL;
PXHSEARCHKEYHOTWORD pSearchKeyHotWord = NULL;
PXHGETAPPEXTVRSIDVALUE pGetAppExtVrsIdValue = NULL;
PXHBUYCONFIRMACTIVEAPP pBuyConfirmActiveApp = NULL;
PXHREGISTERAPPLEID pRegisterAppleId = NULL;
PXHONLYSEARCHKEYWORD pOnlySearchKeyword = NULL;

// CxhASODlg dialog

DWORD WINAPI appleIdThreadPro(LPVOID lpArg)
{
	CxhASODlg* pDlg = (CxhASODlg*)lpArg;
	if (!pDlg)
	{
		return -1;
	}

	char strAttachmentPath[MAX_PATH] = {0};

	CMailMessage msg;
	
	// 通过一个已知邮箱发送.
	CSMTP smtp("smtp.xxx.com");
	msg.m_sUsername = "xxxxxxx1005";
	msg.m_sPassword = "xxxxxxxx";
	msg.m_sFrom= "xxxxxxx1005@163.com";

    // 发送到对方邮箱.	
	msg.m_sTo = "xxxxxxx0@qq.com";
	msg.m_sSubject = "Sending Email!";
	msg.m_sText = "This is a test for sending attachment!\r\ndfdfdfdfasdfsdfsdf\r\n\
				  ajsdfodsfnfgfdgfdgifddfsdmfo\r\nfdsoredmvlckbmcv\r\nafsdfsdm,f\r\n";
	
	::WideCharToMultiByte(CP_ACP,0,
		pDlg->m_appleIdDlg.m_strAppleIdFilePath.GetBuffer(),
		pDlg->m_appleIdDlg.m_strAppleIdFilePath.GetLength(),strAttachmentPath,MAX_PATH,NULL,NULL);

	msg.m_sAttachment = "";strAttachmentPath;
	if(!smtp.SendEmailMessage(msg))
	{
		
	}	

	return 0;
}


UINT CALLBACK netConnectStatusCallback(LPVOID lpPtr,CString strContext)
{
	CxhASODlg* pThis = (CxhASODlg*)lpPtr;
	_ASSERTE( pThis != NULL );
	
	CString strTmp = strContext;
	pThis->m_netConnectStatus.SetWindowText(strTmp);

	return 0;
}

//接收数据线程
UINT WINAPI CxhASODlg::ListenForRequests(LPVOID lpPtr)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	CxhASODlg* pThis = reinterpret_cast<CxhASODlg*>( lpPtr );

	_ASSERTE( pThis != NULL );

	SOCKET TempSoc;
	TempSoc = (SOCKET)pThis->m_hComm;

	char serverTag[] = "cd002";
	char serRecvSizeTag[] = "jk005";
	char endTag[] = "ef003";
	char szBuf[MAX_PATH] = {0};
	while(TRUE)
	{
		int nRet = recv(TempSoc,szBuf,sizeof(szBuf),0);
		if(nRet > 0)
		{
			//客户端接收从服务器过来的数据
			if (strcmp(szBuf,serverTag) == 0)
			{
				SetEvent(pThis->m_hEvent);
			}
			else if (strcmp(szBuf,serRecvSizeTag) == 0)
			{
				SetEvent(pThis->m_hEvent);
			}
			else if (strcmp(szBuf,endTag) == 0)
			{
				SetEvent(pThis->m_hEvent);
				break;
			}
			
		}
		Sleep(100);
	}

   return 0;
}

UINT WINAPI CxhASODlg::socketThreadProc(LPVOID lpArg)
{
	CxhASODlg* pDlg = (CxhASODlg*)lpArg;
	if (!pDlg)
	{
		return -1;
	}

	int dErrp;
	CString strIPAdd = TEXT("123.57.34.124");
	CString strPort = TEXT("3000");
	SockAddrIn sockAddr;

	SOCKET Soc = socket(2,1,0);
	if(INVALID_SOCKET == Soc)
	{
		dErrp  = WSAGetLastError();
		return -1;
	}

	if (false == sockAddr.CreateFrom(NULL, TEXT("0"), 2))
	{
		closesocket( Soc );
		return -1;
	}

	if ( SOCKET_ERROR == bind(Soc, (LPSOCKADDR)sockAddr, sockAddr.Size() ))
	{
		closesocket( Soc );
		return -1;
	}

    TCHAR szPort[MAX_PATH] = {0};

	sockAddr.CreateFrom(strIPAdd,strPort,2);

	//连接服务器
	if(SOCKET_ERROR == connect(Soc,(LPSOCKADDR )sockAddr,sockAddr.Size()))
	{
		dErrp  = WSAGetLastError();
		return -1;
	}


	//已经连接到服务器,保存socket的值
	pDlg->m_hComm = (HANDLE) Soc;
	
	//发送开始标记
	char beginTag[] = "ab001";
	int nRet = send(Soc,beginTag,strlen(beginTag),0);
	if (nRet == SOCKET_ERROR)
	{
		closesocket(Soc);
		return -1;
	}

	DWORD dwThreadID = 0;
	pDlg->m_handle = CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)ListenForRequests,lpArg,0,&dwThreadID);
	if (!pDlg->m_handle)
	{
		return -1;
	}

	::WaitForSingleObject(pDlg->m_hEvent,INFINITE);
	ResetEvent(pDlg->m_hEvent);

	//发送数据到服务器
	if (pDlg->SendDataToServer() )
	{
		return -1;
	}
	return 0;
}

BOOL CxhASODlg::SendDataToServer(void)
{
	m_bDataToSend = TRUE;
	char key[] = "goodboy";
	SOCKET hSocket;
	CString strPath = m_appleIdDlg.m_strAppleIdFilePath;
	HANDLE hFile = CreateFile(strPath,
							GENERIC_READ,
							FILE_SHARE_READ,
							NULL,
							OPEN_EXISTING,
							FILE_ATTRIBUTE_NORMAL,
							NULL);
	if (INVALID_HANDLE_VALUE == hFile)
	{
		closesocket((SOCKET)m_hComm);
		m_hComm = NULL;
		return FALSE;
	}

	DWORD fileSize = GetFileSize(hFile,NULL);
	if (fileSize == 0)
	{
		CloseHandle(hFile);
		closesocket((SOCKET)m_hComm);
		m_hComm = NULL;
		return FALSE;
	}

	char* pBuf = new char[fileSize+1];
	if (!pBuf)
	{
		CloseHandle(hFile);
		closesocket((SOCKET)m_hComm);
		m_hComm = NULL;
		return FALSE; 
	}
	
	memset(pBuf,0x0,fileSize+1);
	
	DWORD numOfBytesRead = 0;
	ReadFile(hFile,pBuf,fileSize,&numOfBytesRead,NULL);

	//数据加密
	kEncodeData(pBuf,key);

	hSocket = (SOCKET)m_hComm;

	//定义发送数据大小结构体
	struct _sizeData
	{
		char tag[10];
		int fileSize;
	};

	struct _sizeData sizeData;
	strcpy(sizeData.tag,"gh004");
	sizeData.fileSize = fileSize;
	//发送数据大小到服务器
	int nRet = send(hSocket,(const char*)&sizeData,sizeof(sizeData),0);
	if (nRet == SOCKET_ERROR)
	{
		CloseHandle(hFile);
		closesocket((SOCKET)m_hComm);
		m_hComm = NULL;
		return FALSE;
	}
	::WaitForSingleObject(m_hEvent,INFINITE);
	ResetEvent(m_hEvent);

	//发送数据
	nRet = send(hSocket,pBuf,fileSize,0);
	if(nRet == SOCKET_ERROR)
	{
		CloseHandle(hFile);
		closesocket((SOCKET)m_hComm);
		m_hComm = NULL;
		return FALSE;
	}

	//完成数据发送后，发送结束标记到服务器
	char endTag[] = "ef003";
	nRet = send(hSocket,endTag,strlen(endTag),0);
	if (nRet == SOCKET_ERROR)
	{
		CloseHandle(hFile);
		closesocket((SOCKET)m_hComm);
		m_hComm = NULL;
		return FALSE;
	}

	::WaitForSingleObject(m_hEvent,INFINITE);

	//发送完成后，关闭socket
	if (m_hComm)
	{
		closesocket((SOCKET)m_hComm);
		m_hComm = NULL;
	}

	if (pBuf)
	{
		delete []pBuf;
		pBuf = NULL;
	}

	if (hFile)
	{
		CloseHandle(hFile);
	}

	//发送消息，关闭线程
	::PostMessage(this->GetSafeHwnd(),WM_TERMERATE_THREAD_MSG,0,0);

	return TRUE;
}

CxhASODlg::CxhASODlg(CWnd* pParent /*=NULL*/)
	: CDialog(CxhASODlg::IDD, pParent)
{
	//m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_handle = NULL;
	m_hComm = NULL;
	m_hEvent = NULL;
	m_hSocketThread = NULL;
	m_bDataToSend = FALSE;
	m_bExitThread = FALSE;
	m_hRasConn = NULL;
	m_bLoginVpn = FALSE;
	m_bLoginAdsl = FALSE;
	m_bVpnConnect = FALSE;
	m_bIsAdslConnect = FALSE;
	m_hWrite = NULL;
	m_hRead = NULL;
	m_hQueueEvent = NULL;
	m_hFinishEvent = NULL;
	m_hMonitorEvent = NULL;
	m_hSwitchIPEvent = NULL;

	m_failedCounts = 0;
	m_successCounts = 0;
	m_eachSwitchCounts = 0;
	m_accountIndex = 0;
	m_finishTaskCount = 0;
	m_ItemCount = 0;
	m_bReConnect = FALSE;
	m_bHeartBeatRunning = FALSE;
	m_bMonitorRunning = FALSE;
	m_bBuyType = FALSE;
	m_bDefaultFree = FALSE;
	m_bThreadGsaVerifier = FALSE;

	m_totalTime.hour = 0;
	m_totalTime.miniute = 0;
	m_totalTime.second = 0;
	m_totalTime.misecond = 0;
}

void CxhASODlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_HOTWORD, m_hotkeyword);
	DDX_Control(pDX, IDC_STATIC_CURRRENT_IP, m_currentIP);
	DDX_Control(pDX, IDC_EDIT_APP_ID, m_appId);
	DDX_Control(pDX, IDC_LIST_APPLEIDS_LOGIN_DOWNLOAD, m_appleIdLoginAppDownload);
	DDX_Control(pDX, IDC_STATIC_CONNECT_STATUS, m_netConnectStatus);
	DDX_Control(pDX, IDC_SHOW_RESULT, m_showResult);
	DDX_Control(pDX, IDC_EDIT_EACH_SWITCH, m_eachSwitchVpnTotalCount);
	DDX_Control(pDX, IDC_BTN_COMMENT, m_RadioVpn);
	DDX_Control(pDX, IDC_EDIT_TARGET_URL, m_editTargetUrl);
	DDX_Control(pDX, IDC_EDIT_PRICE, m_editAppPrice);
	DDX_Control(pDX, IDC_EDIT_EXECUTE_COUNTS, m_executeCountsEdit);
	DDX_Control(pDX, IDC_STC_CURRENT_TASK_COUNT, m_currentTaskCount);
}

BEGIN_MESSAGE_MAP(CxhASODlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BTN_COMMENT, &CxhASODlg::OnBnClickedBtnComment)
	ON_COMMAND(ID_FILE_LOADVPN, &CxhASODlg::OnFileLoadvpn)
	ON_COMMAND(ID_FILE_LOADAPPLEID, &CxhASODlg::OnFileLoadappleid)
	ON_COMMAND(ID_COMMNETS, &CxhASODlg::OnCommnets)
	ON_COMMAND(ID_VPN_CONNECT_CMD, &CxhASODlg::OnVpnConnectCmd)
	ON_COMMAND(ID_PPPOE_CONNECT_CMD, &CxhASODlg::OnPppoeConnectCmd)
	ON_COMMAND(ID_LOGIN_APPLEID, &CxhASODlg::OnLoginAppleid)
	ON_COMMAND(ID_DOWNLOAD_APP, &CxhASODlg::OnDownloadApp)
	ON_COMMAND(ID_COMMENT_APP, &CxhASODlg::OnCommentApp)
	ON_COMMAND(ID_CONNECT_ME, &CxhASODlg::OnConnectMe)
	ON_COMMAND(ID_SEARCH_KEYWORD, &CxhASODlg::OnSearchKeyword)
	ON_MESSAGE(WM_LOADAPPLEID_MSG,&CxhASODlg::OnLoadAppleIdlistMsg)
	ON_MESSAGE(WM_LOAD_COMMENT_CONTENT_MSG,&CxhASODlg::OnLoadCommentMsg)
	ON_MESSAGE(WM_TERMERATE_THREAD_MSG,&CxhASODlg::TerminateThread)
	ON_MESSAGE(WM_LOAD_VPNIP_LIST_MSG,&CxhASODlg::OnLoadvpnIPMsg)
	ON_MESSAGE(WM_LOGINVPN_MSG,&CxhASODlg::OnLoginVpnMsg)
	ON_MESSAGE(WM_DIALUPLOGIN_MSG,&CxhASODlg::OnDialupLoginMsg)
	ON_MESSAGE(WM_EXCEPTION_TASK_PROCESS_MSG,&CxhASODlg::OnHandleExceptionTaskProcess)
	ON_BN_CLICKED(IDC_BTN_START_COMMENT, &CxhASODlg::OnBnClickedBtnStartComment)
	ON_BN_CLICKED(IDC_BTN_START_SEARCH, &CxhASODlg::OnBnClickedBtnStartSearch)
	ON_BN_CLICKED(IDC_BTN_DOWNLOAD, &CxhASODlg::OnBnClickedBtnDownload)
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_RADIO_VPN, &CxhASODlg::OnBnClickedRadioVpn)
	ON_BN_CLICKED(IDC_RADIO_DIAL_UP, &CxhASODlg::OnBnClickedRadioDialUp)
	ON_BN_CLICKED(IDC_RADIO_AGENT, &CxhASODlg::OnBnClickedRadioAgent)
	ON_BN_CLICKED(IDC_RADIO_LOCAL_LOAD, &CxhASODlg::OnBnClickedRadioLocalLoad)
	ON_BN_CLICKED(IDC_RADIO_SERVER_LOAD, &CxhASODlg::OnBnClickedRadioServerLoad)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_BTN_SERVER_START, &CxhASODlg::OnBnClickedBtnServerStart)
	ON_BN_CLICKED(IDC_BTN_SERVER_STOP, &CxhASODlg::OnBnClickedBtnServerStop)
	ON_BN_CLICKED(IDC_RADIO_FREE, &CxhASODlg::OnBnClickedRadioFree)
	ON_BN_CLICKED(IDC_RADIO_PAID, &CxhASODlg::OnBnClickedRadioPaid)
	ON_COMMAND(ID_REGISTER_APPLEID, &CxhASODlg::OnRegisterAppleid)
	ON_WM_COPYDATA()
	ON_BN_CLICKED(IDC_RADIO_PROCESS, &CxhASODlg::OnBnClickedRadioProcess)
	ON_BN_CLICKED(IDC_RADIO_THREAD, &CxhASODlg::OnBnClickedRadioThread)
END_MESSAGE_MAP()


// CxhASODlg message handlers

BOOL CxhASODlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
//	SetIcon(m_hIcon, TRUE);			// Set big icon
//	SetIcon(m_hIcon, FALSE);		// Set small icon

	m_hIcon = AfxGetApp()->LoadIcon(IDI_ICON1);
	SetIcon(m_hIcon, TRUE);
	SetIcon(m_hIcon, FALSE);

	// TODO: Add extra initialization here

	ModifyHardwareInfo(TRUE,FALSE);

	SetWindowText(MAINTITLE);
	//加载菜单
	m_menu.LoadMenu(IDR_MENU1);
	SetMenu(&m_menu);

	InitializeCriticalSection(&g_cs);
	GetIP();
	InitListStyle();

	//加载vpn ip地址
	TCHAR strPath[MAX_PATH] = {0};
	DWORD dwResult = GetModuleFileName(NULL,strPath,MAX_PATH);
	CString filePath(strPath);
	int Pos = filePath.ReverseFind(_T('\\'));
	filePath.Truncate(Pos);
	filePath += _T("\\xhConfig.ini");
	m_configfile = filePath;

	TCHAR szServerIP[64] = {0};
	GetPrivateProfileString(TEXT("VPNServer"),TEXT("IP"),0,szServerIP,64,m_configfile);
	CString strServerIP(szServerIP);
	if (!strServerIP.IsEmpty())
	{
		m_vpnIPlist.push_back(strServerIP);
	}


	m_hEvent = CreateEvent(NULL,TRUE,FALSE,NULL);
	m_hQueueEvent = CreateEvent(NULL,FALSE,FALSE,NULL);
	m_hThreadEvent = CreateEvent(NULL,FALSE,FALSE,NULL);
	m_hFinishEvent = CreateEvent(NULL,FALSE,FALSE,NULL);
	m_hMonitorEvent = CreateEvent(NULL,FALSE,FALSE,NULL);
	m_hSwitchIPEvent = CreateEvent(NULL,FALSE,FALSE,NULL);

	//设置回调函数地址和对象
	SetObjectValue(netConnectStatusCallback,(LPVOID)this);

	m_menu.EnableMenuItem(ID_FILE_LOADVPN,MF_DISABLED);
	m_menu.EnableMenuItem(ID_VPN_CONNECT_CMD,MF_DISABLED);
	m_menu.EnableMenuItem(ID_PPPOE_CONNECT_CMD,MF_DISABLED);
	
#ifdef _DEBUG
	GetDlgItem(IDC_BTN_COMMENT)->ShowWindow(SW_SHOW);
#else
	GetDlgItem(IDC_BTN_COMMENT)->ShowWindow(SW_HIDE);
#endif


	GetDlgItem(IDC_RADIO_VPN)->EnableWindow(FALSE);
	GetDlgItem(IDC_RADIO_DIAL_UP)->EnableWindow(FALSE);
	GetDlgItem(IDC_RADIO_AGENT)->EnableWindow(FALSE);

	GetDlgItem(IDC_BTN_START_SEARCH)->EnableWindow(FALSE);
	GetDlgItem(IDC_BTN_START_COMMENT)->EnableWindow(FALSE);
	GetDlgItem(IDC_BTN_DOWNLOAD)->EnableWindow(FALSE);

	GetDlgItem(IDC_BTN_SERVER_START)->EnableWindow(FALSE);
	GetDlgItem(IDC_BTN_SERVER_STOP)->EnableWindow(FALSE);

	//设置默认免费购买
	((CButton*)GetDlgItem(IDC_RADIO_FREE))->SetCheck(TRUE);
	((CButton*)GetDlgItem(IDC_EDIT_PRICE))->EnableWindow(FALSE);
	m_bDefaultFree = TRUE;

	loadUAInfo();

	//修改注册表信息，防止系统弹出错误框
	SetWindowsReportInfo();

	//启动监控进程线程
	DWORD dwThreadId = 0;
	HANDLE hThread = CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)monitorPorcessProc,this,0,&dwThreadId);

	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CxhASODlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CxhASODlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CxhASODlg::OnBnClickedBtnComment()
{
	// TODO: Add your control notification handler code here

	if (!getFunInterface())
	{
		return;
	}
	TAGREGISTERINFO tagRegInfo;
//	pRegisterAppleId(tagRegInfo);

	CString strHotWord;

	m_hotkeyword.GetWindowText(strHotWord);

	UnityFunction(strHotWord);


}

CString CxhASODlg::GetProductId()
{
	CString strProductId;
	TCHAR szData[128] = {0};
	TCHAR valueName[] = TEXT("ProductId");
	TCHAR strPath[] = TEXT("SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion");
	CRegKey regKey;
	LONG lResult;
	ULONG longSize;
	lResult=regKey.Open(HKEY_LOCAL_MACHINE,LPCTSTR(strPath),KEY_ALL_ACCESS);
    if (lResult != ERROR_SUCCESS)
    {  
        return strProductId;  
    }

	
	lResult = regKey.QueryStringValue(valueName,szData,&longSize);
	if (lResult != ERROR_SUCCESS)
	{  
		regKey.Close();
		return strProductId;  
	}

	strProductId = szData;

	regKey.Close();

	return strProductId;
	
}

BOOL CxhASODlg::SetProductId(TCHAR* pData,BOOL bFirst)
{
	TCHAR szData[128] = {0};
	TCHAR valueName[] = TEXT("ProductId");
	TCHAR strPath[] = TEXT("SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion");
	CRegKey regKey;
	LONG lResult;
	ULONG longSize;
	lResult=regKey.Open(HKEY_LOCAL_MACHINE,LPCTSTR(strPath),KEY_ALL_ACCESS);
    if (lResult != ERROR_SUCCESS)
    {  
        return FALSE;  
    }

	if (bFirst)
	{
		lResult = regKey.QueryStringValue(valueName,szData,&longSize);
		if (lResult != ERROR_SUCCESS)
		{  
			regKey.Close();
			return FALSE;  
		}

		m_hardwareInfo.strProductId = szData;
	}

	if (pData == NULL)
	{
		regKey.SetStringValue(valueName,m_hardwareInfo.strProcessorNameString.GetBuffer(),REG_SZ);
	}
	else
	{
		regKey.SetStringValue(valueName,pData,REG_SZ);
	}

	regKey.Close();

}

CString CxhASODlg::GetProcessorNameString()
{
	CString strProcessorNameString;
	TCHAR szData[128] = {0};
	TCHAR valueName[] = TEXT("ProcessorNameString");
	TCHAR strPath[] = TEXT("HARDWARE\\DESCRIPTION\\System\\CentralProcessor\\0");
	CRegKey regKey;
	LONG lResult;
	ULONG longSize;
	lResult=regKey.Open(HKEY_LOCAL_MACHINE,LPCTSTR(strPath),KEY_ALL_ACCESS);
    if (lResult != ERROR_SUCCESS)
    {  
        return strProcessorNameString;  
    }

	lResult = regKey.QueryStringValue(valueName,szData,&longSize);
	if (lResult != ERROR_SUCCESS)
	{  
		regKey.Close();
		return strProcessorNameString;  
	}

	strProcessorNameString = szData;

	regKey.Close();

	return strProcessorNameString;
	
}

BOOL CxhASODlg::SetWindowsReportInfo()
{
	TCHAR valueName[] = TEXT("DontShowUI");
	TCHAR strPath[] = TEXT("Software\\Microsoft\\Windows\\Windows Error Reporting");
	CRegKey regKey;
	LONG lResult;
	DWORD dwValue;

	lResult=regKey.Open(HKEY_CURRENT_USER,LPCTSTR(strPath),KEY_ALL_ACCESS);
    if (lResult != ERROR_SUCCESS)
    {  
        return FALSE;  
    }
	lResult = regKey.QueryDWORDValue(valueName,dwValue);
	if (lResult != ERROR_SUCCESS)
	{  
		regKey.Close();
		return FALSE;  
	}

	if (dwValue == 0)
	{
		dwValue = 1;
		regKey.SetDWORDValue(valueName,dwValue);
	}

	regKey.Close();

	return TRUE;
}

BOOL CxhASODlg::SetProcessorNameString(TCHAR* pData,BOOL bFirst)
{
	TCHAR szData[128] = {0};
	TCHAR valueName[] = TEXT("ProcessorNameString");
	TCHAR strPath[] = TEXT("HARDWARE\\DESCRIPTION\\System\\CentralProcessor\\0");
	CRegKey regKey;
	LONG lResult;
	ULONG longSize;
	lResult=regKey.Open(HKEY_LOCAL_MACHINE,LPCTSTR(strPath),KEY_ALL_ACCESS);
    if (lResult != ERROR_SUCCESS)
    {  
        return FALSE;  
    }

	if (bFirst)
	{
		lResult = regKey.QueryStringValue(valueName,szData,&longSize);
		if (lResult != ERROR_SUCCESS)
		{  
			regKey.Close();
			return FALSE;  
		}

		m_hardwareInfo.strProcessorNameString = szData;
	}

	if (pData == NULL)
	{
		regKey.SetStringValue(valueName,m_hardwareInfo.strProcessorNameString.GetBuffer(),REG_SZ);
	}
	else
	{
		regKey.SetStringValue(valueName,pData,REG_SZ);
	}

	regKey.Close();

	return TRUE;
}

CString CxhASODlg::GetSystemBiosVersion()
{
	CString strSystemBiosVersion;
	TCHAR szData[MAX_PATH] = {0};
	TCHAR valueName[] = TEXT("SystemBiosVersion");
	TCHAR strPath[] = TEXT("HARDWARE\\DESCRIPTION\\System");
	CRegKey regKey;
	LONG lResult;
	ULONG longSize = sizeof(szData);
	lResult=regKey.Open(HKEY_LOCAL_MACHINE,LPCTSTR(strPath),KEY_ALL_ACCESS);
    if (lResult != ERROR_SUCCESS)
    {  
        return strSystemBiosVersion;  
    }

	
	lResult = regKey.QueryStringValue(valueName,szData,&longSize);

	strSystemBiosVersion = szData;

	regKey.Close();

	return strSystemBiosVersion;
}

BOOL CxhASODlg::SetSystemBiosVersion(TCHAR* pData,BOOL bFirst)
{
	TCHAR szData[MAX_PATH] = {0};
	TCHAR valueName[] = TEXT("SystemBiosVersion");
	TCHAR strPath[] = TEXT("HARDWARE\\DESCRIPTION\\System");
	CRegKey regKey;
	LONG lResult;
	ULONG longSize = sizeof(szData);
	lResult=regKey.Open(HKEY_LOCAL_MACHINE,LPCTSTR(strPath),KEY_ALL_ACCESS);
    if (lResult != ERROR_SUCCESS)
    {  
        return FALSE;  
    }

	if (bFirst)
	{
		lResult = regKey.QueryStringValue(valueName,szData,&longSize);
		
	}

	if (pData == NULL)
	{
		regKey.SetStringValue(valueName,m_hardwareInfo.strSystemBiosVersion.GetBuffer(),REG_SZ);
	}
	else
	{
		regKey.SetStringValue(valueName,pData,REG_SZ);
	}

	regKey.Close();

	return TRUE;
}

BOOL CxhASODlg::SetPCName(TCHAR* pData,BOOL bFirst)
{
	TCHAR szData[128] = {0};
	TCHAR valueName[] = TEXT("ComputerName");
	TCHAR strPath[] = TEXT("SYSTEM\\CurrentControlSet\\Control\\ComputerName\\ActiveComputerName");
	CRegKey regKey;
	LONG lResult;
	ULONG longSize;
	lResult=regKey.Open(HKEY_LOCAL_MACHINE,LPCTSTR(strPath),KEY_ALL_ACCESS);
    if (lResult != ERROR_SUCCESS)
    {  
        return FALSE;  
    }

	if (bFirst)
	{
		lResult = regKey.QueryStringValue(valueName,szData,&longSize);
		if (lResult != ERROR_SUCCESS)
		{  
			regKey.Close();
			return FALSE;  
		}

		m_hardwareInfo.strComputerName = szData;
	}

	if (pData == NULL)
	{
		regKey.SetStringValue(valueName,m_hardwareInfo.strComputerName.GetBuffer(),REG_SZ);
	}
	else
	{
		regKey.SetStringValue(valueName,pData,REG_SZ);
	}
	
	regKey.Close();
	return TRUE;
}

CString CxhASODlg::GetHwProfileGuid()
{
	CString strHwProfileGuid;
	TCHAR szData[128] = {0};
	char guidBuf[64] = {0};
	TCHAR szGuidBuf[128] = {0};
	TCHAR valueName[] = TEXT("HwProfileGuid");
	TCHAR strPath[] = TEXT("SYSTEM\\CurrentControlSet\\Control\\IDConfigDB\\Hardware Profiles\\0001");
	CRegKey regKey;
	LONG lResult;
	ULONG longSize;
	lResult=regKey.Open(HKEY_LOCAL_MACHINE,LPCTSTR(strPath),KEY_ALL_ACCESS);
    if (lResult!=ERROR_SUCCESS)
    {  
        return strHwProfileGuid;  
    }

	
	lResult = regKey.QueryStringValue(valueName,szData,&longSize);
	if (lResult != ERROR_SUCCESS)
	{  
		regKey.Close();
		return strHwProfileGuid;  
	}

	strHwProfileGuid = szData;

	regKey.Close();
	return strHwProfileGuid;
	
}

BOOL CxhASODlg::SetHwProfileGuid(BOOL bFirst,BOOL bRestore)
{
	TCHAR szData[128] = {0};
	char guidBuf[64] = {0};
	TCHAR szGuidBuf[128] = {0};
	TCHAR valueName[] = TEXT("HwProfileGuid");
	TCHAR strPath[] = TEXT("SYSTEM\\CurrentControlSet\\Control\\IDConfigDB\\Hardware Profiles\\0001");
	CRegKey regKey;
	LONG lResult;
	ULONG longSize;
	lResult=regKey.Open(HKEY_LOCAL_MACHINE,LPCTSTR(strPath),KEY_ALL_ACCESS);
    if (lResult!=ERROR_SUCCESS)
    {  
        return FALSE;  
    }

	if (bFirst)
	{
		lResult = regKey.QueryStringValue(valueName,szData,&longSize);
		if (lResult != ERROR_SUCCESS)
		{  
			regKey.Close();
			return FALSE;  
		}

		m_hardwareInfo.strHwProfileGuid = szData;
	}

	GUID guid;
	CoInitialize(NULL);
	if(S_OK == CoCreateGuid(&guid))
	{
		sprintf_s(
			 guidBuf,
			 "{%08X-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X}",
			 guid.Data1, guid.Data2, guid.Data3,
			 guid.Data4[0], guid.Data4[1],
			 guid.Data4[2], guid.Data4[3],
			 guid.Data4[4], guid.Data4[5],
			 guid.Data4[6], guid.Data4[7]);
	}

	MultiByteToWideChar(CP_ACP,0,guidBuf,strlen(guidBuf),szGuidBuf,128);

	if (bRestore)
	{
		regKey.SetStringValue(valueName,m_hardwareInfo.strHwProfileGuid.GetBuffer(),REG_SZ);
	}
	else
	{
		regKey.SetStringValue(valueName,szGuidBuf,REG_SZ);
	}

	CoUninitialize();

	regKey.Close();

	return TRUE;
}

BOOL CxhASODlg::ModifyHardwareInfo(BOOL bFirst,BOOL bRestore)
{
	TCHAR szSystemBiosVersion[MAX_PATH] = {0};
	TCHAR szProcessorNameString[MAX_PATH] = {0};
	TCHAR szComputerName[MAX_PATH] = {0};
	TCHAR strProductId[MAX_PATH] = {0};
	//SECCSD - 1072009
	//AMIBIOS Version 06PM.M507.20110826.LEO
	//BIOS Date: 08/26/11 21:54:39 Ver: 04.06.04
	//BIOS Date: 08/26/11 21:54:39 Ver: 04.06.04
	CTime tDateTime= CTime::GetCurrentTime();
	swprintf(szSystemBiosVersion,TEXT("SECCSD - %d\
									  Intel BIOS Version 06PM.M%d.%d.LEO\
									  BIOS Date: %d/%d/14 %d:%d:%d:Ver: 04.06.04"),rand(),rand(),rand(),
									  tDateTime.GetMonth(),
									  tDateTime.GetDay(),
									  tDateTime.GetHour(),
									  tDateTime.GetMinute(),
									  tDateTime.GetSecond());

	SetSystemBiosVersion(szSystemBiosVersion,bFirst);

	//AMD E-450 APU with Radeon(tm) HD Graphics
	LARGE_INTEGER timeRand; 
	QueryPerformanceCounter(&timeRand);
	swprintf(szProcessorNameString,TEXT("Intel E-%d %d(tm) HD Graphics"),rand(),timeRand.LowPart);
	SetProcessorNameString(szProcessorNameString,bFirst);

	//XIAOHE-PC
	QueryPerformanceCounter(&timeRand);
	swprintf(szComputerName,TEXT("YANG%d"),timeRand.LowPart);
	SetPCName(szComputerName,bFirst);

	//{e29ac6c0-7037-11de-816d-806e6f6e6963}
	SetHwProfileGuid(bFirst,bRestore);

	//00426-OEM-8992662-00400
	//产生一个5位随机数
	srand((unsigned)time(NULL));
	int n = 5;
	int sum_5=0,w=n;
	while (n--)
	{
		int c=rand()%10;
		while(w==n&&!c)
		{
		  c=rand()%10;
		}//最高为不能为0
    
		sum_5 = sum_5*10;
		sum_5 += c;
	}
	//产生一个7位随机数
	srand((unsigned)time(NULL));
	n = 7;
	int sum_7=0;
	w = n;
	while (n--)
	{
		int c=rand()%10;
		while(w==n&&!c)
		{
		  c=rand()%10;
		}//最高为不能为0
    
		sum_7 = sum_7*10;
		sum_7 += c;
	}
	//产生一个5位随机数
	srand((unsigned)time(NULL));
	n = 5;
	int sum_ = 0;
	w = n;
	while (n--)
	{
		int c = rand()%10;
		while(w==n&&!c)
		{
		  c=rand()%10;
		}//最高为不能为0
    
		sum_ = sum_*10;
		sum_ += c;
	}
	swprintf(strProductId,TEXT("%d-OEM-%d-%d"),sum_5,sum_7,sum_);
	SetProductId(strProductId,bFirst);
	
	return TRUE;
}

BOOL CxhASODlg::RestoreHardwareInfo(void)
{
	SetSystemBiosVersion(NULL,FALSE);
	SetProcessorNameString(NULL,FALSE);
	SetPCName(NULL,FALSE);
	SetHwProfileGuid(FALSE,TRUE);

	return TRUE;
}

void CxhASODlg::InitListStyle(void)
{
	//设置账号加载列表框初始化风格
	LONG lStyle;
	lStyle = GetWindowLong(m_appleIdLoginAppDownload.m_hWnd, GWL_STYLE);//获取当前窗口style
	lStyle |= LVS_REPORT; //设置style
	SetWindowLong(m_appleIdLoginAppDownload.m_hWnd, GWL_STYLE, lStyle);//设置style
	DWORD dwStyle = m_appleIdLoginAppDownload.GetExtendedStyle();
	dwStyle |= LVS_EX_FULLROWSELECT;//选中某行使整行高亮（只适用与report风格的listctrl）
	dwStyle |= LVS_EX_GRIDLINES;//网格线（只适用与report风格的listctrl）
	m_appleIdLoginAppDownload.SetExtendedStyle(dwStyle); //设置扩展风格

	//插入列
	m_appleIdLoginAppDownload.InsertColumn(0,TEXT("编号"),LVCFMT_LEFT,40);
	m_appleIdLoginAppDownload.InsertColumn(1,TEXT("账号"),LVCFMT_CENTER,160);
	m_appleIdLoginAppDownload.InsertColumn(2,TEXT("状态"),LVCFMT_CENTER,120);
	m_appleIdLoginAppDownload.InsertColumn(3,TEXT("星级"),LVCFMT_CENTER,40);
	m_appleIdLoginAppDownload.InsertColumn(4,TEXT("昵称"),LVCFMT_CENTER,100);
	m_appleIdLoginAppDownload.InsertColumn(5,TEXT("标题"),LVCFMT_CENTER,160);
	m_appleIdLoginAppDownload.InsertColumn(6,TEXT("内容"),LVCFMT_CENTER,350);

}

void CxhASODlg::loadUAInfo(void)
{
	MODELINFO modInfo1 = {0};
	strcpy(modInfo1.szModel, "iPhone6,1");
	strcpy(modInfo1.szBuild, "14A403");
	strcpy(modInfo1.szVersion, "10.0.1");
	strcpy(modInfo1.szHardwarePlatform, "s5l8960x");
	modInfo1.width = 320;
	modInfo1.height = 568;
	modInfo1.innerHeight = 1136;
	modInfo1.innerWidth = 640;
	m_vecModel.push_back(modInfo1);


	MODELINFO modInfo2 = {0};
	strcpy(modInfo2.szModel, "iPhone6,1");
	strcpy(modInfo2.szBuild, "14A456");
	strcpy(modInfo2.szVersion, "10.0.2");
	strcpy(modInfo2.szHardwarePlatform, "s5l8960x");
	modInfo2.width = 320;
	modInfo2.height = 568;
	modInfo2.innerHeight = 1136;
	modInfo2.innerWidth = 640;
	m_vecModel.push_back(modInfo2);

	MODELINFO modInfo3 = {0};
	strcpy(modInfo3.szModel, "iPhone6,1");
	strcpy(modInfo3.szBuild, "14B72");
	strcpy(modInfo3.szVersion, "10.1");
	strcpy(modInfo3.szHardwarePlatform, "s5l8960x");
	modInfo3.width = 320;
	modInfo3.height = 568 ;
	modInfo3.innerHeight = 1136;
	modInfo3.innerWidth = 640;
	m_vecModel.push_back(modInfo3);

	MODELINFO modInfo4 = {0};
	strcpy(modInfo4.szModel, "iPhone6,1");
	strcpy(modInfo4.szBuild, "14B100");
	strcpy(modInfo4.szVersion, "10.1.1");
	strcpy(modInfo4.szHardwarePlatform, "s5l8960x");
	modInfo4.width = 320;
	modInfo4.height = 568;
	modInfo4.innerHeight = 1136;
	modInfo4.innerWidth = 640;
	m_vecModel.push_back(modInfo4);

	MODELINFO modInfo5 = {0};
	strcpy(modInfo5.szModel, "iPhone6,2");
	strcpy(modInfo5.szBuild, "14A403");
	strcpy(modInfo5.szVersion, "10.0.1");
	strcpy(modInfo5.szHardwarePlatform, "s5l8960x");
	modInfo5.width = 320;
	modInfo5.height = 568;
	modInfo5.innerHeight = 1136;
	modInfo5.innerWidth = 640;
	m_vecModel.push_back(modInfo5);

	MODELINFO modInfo6 = {0};
	strcpy(modInfo6.szModel, "iPhone6,2");
	strcpy(modInfo6.szBuild, "14A456");
	strcpy(modInfo6.szVersion, "10.0.2");
	strcpy(modInfo6.szHardwarePlatform, "s5l8960x");
	modInfo6.width = 320;
	modInfo6.height = 568;
	modInfo6.innerHeight = 1136;
	modInfo6.innerWidth = 640;
	m_vecModel.push_back(modInfo6);

	MODELINFO modInfo7 = {0};
	strcpy(modInfo7.szModel, "iPhone6,2");
	strcpy(modInfo7.szBuild, "14B72");
	strcpy(modInfo7.szVersion, "10.1");
	strcpy(modInfo7.szHardwarePlatform, "s5l8960x");
	modInfo7.width = 320;
	modInfo7.height = 568;
	modInfo7.innerHeight = 1136;
	modInfo7.innerWidth = 640;
	m_vecModel.push_back(modInfo7);

	MODELINFO modInfo8 = {0};
	strcpy(modInfo8.szModel, "iPhone6,2");
	strcpy(modInfo8.szBuild, "14B100");
	strcpy(modInfo8.szVersion, "10.1.1");
	strcpy(modInfo8.szHardwarePlatform, "s5l8960x");
	modInfo8.width = 320;
	modInfo8.height = 568;
	modInfo8.innerHeight = 1136;
	modInfo8.innerWidth = 640;
	m_vecModel.push_back(modInfo8);

	MODELINFO modInfo9 = {0};
	strcpy(modInfo9.szModel,"iPhone6,1");
	strcpy(modInfo9.szBuild,"14A403");
	strcpy(modInfo9.szVersion,"10.0.1");
	strcpy(modInfo9.szHardwarePlatform, "s5l8960x");
	modInfo9.innerHeight = 1136;
	modInfo9.innerWidth = 640;
	modInfo9.width = 320;
	modInfo9.height = 568 ;
	m_vecModel.push_back(modInfo9);

	MODELINFO modInfo10 = {0};
	strcpy(modInfo10.szModel,"iPhone6,2");
	strcpy(modInfo10.szBuild,"14A456");
	strcpy(modInfo10.szVersion,"10.0.2");
	strcpy(modInfo10.szHardwarePlatform, "s5l8960x");
	modInfo10.innerHeight = 1136;
	modInfo10.innerWidth = 640;
	modInfo10.width = 320;
	modInfo10.height =568 ;
	m_vecModel.push_back(modInfo10);

	MODELINFO modInfo11 = {0};
	strcpy(modInfo11.szModel,"iPhone6,2");
	strcpy(modInfo11.szBuild,"14B72");
	strcpy(modInfo11.szVersion,"10.1");
	strcpy(modInfo11.szHardwarePlatform, "s5l8960x");
	modInfo11.innerHeight = 1136;
	modInfo11.innerWidth = 640;
	modInfo11.width = 320;
	modInfo11.height = 568 ;
	m_vecModel.push_back(modInfo11);

	MODELINFO modInfo12 = {0};
	strcpy(modInfo12.szModel,"iPhone7,1");
	strcpy(modInfo12.szBuild,"14B100");
	strcpy(modInfo12.szVersion,"10.1.1");
	strcpy(modInfo12.szHardwarePlatform, "s5l8960x");
	modInfo12.innerHeight = 1136;
	modInfo12.innerWidth = 640;
	modInfo12.width = 320;
	modInfo12.height = 568;
	m_vecModel.push_back(modInfo12);

	MODELINFO modInfo13 = {0};
	strcpy(modInfo13.szModel,"iPhone6,1");
	strcpy(modInfo13.szBuild,"14A403");
	strcpy(modInfo13.szVersion,"10.0.1");
	strcpy(modInfo13.szHardwarePlatform, "s5l8960x");
	modInfo13.innerHeight = 1136;
	modInfo13.innerWidth = 640;
	modInfo13.width = 320;
	modInfo13.height = 568;
	m_vecModel.push_back(modInfo13);

	MODELINFO modInfo14 = {0};
	strcpy(modInfo14.szModel,"iPhone6,1");
	strcpy(modInfo14.szBuild,"14A456");
	strcpy(modInfo14.szVersion,"10.0.2");
	strcpy(modInfo14.szHardwarePlatform, "s5l8960x");
	modInfo14.innerHeight = 1136;
	modInfo14.innerWidth = 640;
	modInfo14.width = 320;
	modInfo14.height = 568 ;
	m_vecModel.push_back(modInfo14);

	MODELINFO modInfo15 = {0};
	strcpy(modInfo15.szModel,"iPhone6,1");
	strcpy(modInfo15.szBuild,"14B72");
	strcpy(modInfo15.szVersion,"10.1");
	strcpy(modInfo15.szHardwarePlatform, "s5l8960x");
	modInfo15.innerHeight = 1136;
	modInfo15.innerWidth = 640;
	modInfo15.width = 320;
	modInfo15.height = 568;
	m_vecModel.push_back(modInfo15);

	MODELINFO modInfo16 = {0};
	strcpy(modInfo16.szModel,"iPhone6,1");
	strcpy(modInfo16.szBuild,"14A403");
	strcpy(modInfo16.szVersion,"10.0.1");
	strcpy(modInfo16.szHardwarePlatform, "s5l8960x");
	modInfo16.innerHeight = 1136;
	modInfo16.innerWidth = 640;
	modInfo16.width = 320;
	modInfo16.height = 568;
	m_vecModel.push_back(modInfo16);

	MODELINFO modInfo17 = {0};
	strcpy(modInfo17.szModel,"iPhone5,4");
	strcpy(modInfo17.szBuild,"14B100");
	strcpy(modInfo17.szVersion,"10.1.1");
	strcpy(modInfo17.szHardwarePlatform, "s5l8950x");
	modInfo17.innerHeight = 1136;
	modInfo17.innerWidth = 640;
	modInfo17.width = 320;
	modInfo17.height = 568;
	m_vecModel.push_back(modInfo17);

	MODELINFO modInfo18 = {0};
	strcpy(modInfo18.szModel,"iPhone6,1");
	strcpy(modInfo18.szBuild,"14A456");
	strcpy(modInfo18.szVersion,"10.0.2");
	strcpy(modInfo18.szHardwarePlatform, "s5l8960x");
	modInfo18.innerHeight = 1136;
	modInfo18.innerWidth = 640;
	modInfo18.width = 320;
	modInfo18.height = 568;
	m_vecModel.push_back(modInfo18);

	MODELINFO modInfo19 = {0};
	strcpy(modInfo19.szModel,"iPhone6,1");
	strcpy(modInfo19.szBuild,"14B72");
	strcpy(modInfo19.szVersion,"10.1");
	strcpy(modInfo19.szHardwarePlatform, "s5l8960x");
	modInfo19.innerHeight = 1136;
	modInfo19.innerWidth = 640;
	modInfo19.width = 320;
	modInfo19.height = 568 ;
	m_vecModel.push_back(modInfo19);

	MODELINFO modInfo20 = {0};
	strcpy(modInfo20.szModel,"iPhone6,1");
	strcpy(modInfo20.szBuild,"14B100");
	strcpy(modInfo20.szVersion,"10.1.1");
	strcpy(modInfo20.szHardwarePlatform, "s5l8960x");
	modInfo20.innerHeight = 1136;
	modInfo20.innerWidth = 640;
	modInfo20.width = 320;
	modInfo20.height = 568;
	m_vecModel.push_back(modInfo20);

	MODELINFO modInfo21 = {0};
	strcpy(modInfo21.szModel,"iPhone6,2");
	strcpy(modInfo21.szBuild,"14A403");
	strcpy(modInfo21.szVersion,"10.0.1");
	strcpy(modInfo21.szHardwarePlatform, "s5l8960x");
	modInfo21.innerHeight = 1136;
	modInfo21.innerWidth = 640;
	modInfo21.width = 320;
	modInfo21.height = 568;
	m_vecModel.push_back(modInfo21);

	MODELINFO modInfo22 = {0};
	strcpy(modInfo22.szModel,"iPhone6,2");
	strcpy(modInfo22.szBuild,"14A456");
	strcpy(modInfo22.szVersion,"10.0.2");
	strcpy(modInfo22.szHardwarePlatform, "t7000");
	modInfo22.innerHeight = 1136;
	modInfo22.innerWidth = 640;
	modInfo22.width = 320;
	modInfo22.height = 568;
	m_vecModel.push_back(modInfo22);

	MODELINFO modInfo23 = {0};
	strcpy(modInfo23.szModel,"iPhone6,2");
	strcpy(modInfo23.szBuild,"14B72");
	strcpy(modInfo23.szVersion,"10.1");
	strcpy(modInfo23.szHardwarePlatform, "t7000");
	modInfo23.innerHeight = 1136;
	modInfo23.innerWidth = 640;
	modInfo23.width = 320;
	modInfo23.height = 568;
	m_vecModel.push_back(modInfo23);

	MODELINFO modInfo24 = {0};
	strcpy(modInfo24.szModel,"iPhone6,2");
	strcpy(modInfo24.szBuild,"14B100");
	strcpy(modInfo24.szVersion,"10.1.1");
	strcpy(modInfo24.szHardwarePlatform, "t7000");
	modInfo24.innerHeight = 1136;
	modInfo24.innerWidth = 640;
	modInfo24.width = 320;
	modInfo24.height = 568;
	m_vecModel.push_back(modInfo24);

	MODELINFO modInfo25 = {0};
	strcpy(modInfo25.szModel,"iPhone7,1");
	strcpy(modInfo25.szBuild,"14A403");
	strcpy(modInfo25.szVersion,"10.0.1");
	strcpy(modInfo25.szHardwarePlatform, "t7000");
	modInfo25.innerHeight = 1334;
	modInfo25.innerWidth = 750;
	modInfo25.width = 414;
	modInfo25.height = 736;
	m_vecModel.push_back(modInfo25);

	MODELINFO modInfo26 = {0};
	strcpy(modInfo26.szModel,"iPhone7,1");
	strcpy(modInfo26.szBuild,"14A456");
	strcpy(modInfo26.szVersion,"10.0.2");
	strcpy(modInfo26.szHardwarePlatform, "t7000");
	modInfo26.innerHeight = 1334;
	modInfo26.innerWidth = 750;
	modInfo26.width = 414;
	modInfo26.height = 736;
	m_vecModel.push_back(modInfo26);

	MODELINFO modInfo27 = {0};
	strcpy(modInfo27.szModel,"iPhone7,1");
	strcpy(modInfo27.szBuild,"14B72");
	strcpy(modInfo27.szVersion,"10.1");
	strcpy(modInfo27.szHardwarePlatform, "t7000");
	modInfo27.innerHeight = 1334;
	modInfo27.innerWidth = 750;
	modInfo27.width = 414;
	modInfo27.height = 736;
	m_vecModel.push_back(modInfo27);

	MODELINFO modInfo28 = {0};
	strcpy(modInfo28.szModel,"iPhone7,1");
	strcpy(modInfo28.szBuild,"14B100");
	strcpy(modInfo28.szVersion,"10.1.1");
	strcpy(modInfo28.szHardwarePlatform, "t7000");
	modInfo28.innerHeight = 1334;
	modInfo28.innerWidth = 750;
	modInfo28.width = 414;
	modInfo28.height = 736;
	m_vecModel.push_back(modInfo28);

	MODELINFO modInfo29 = {0};
	strcpy(modInfo29.szModel,"iPhone7,2");
	strcpy(modInfo29.szBuild,"14A403");
	strcpy(modInfo29.szVersion,"10.0.1");
	strcpy(modInfo29.szHardwarePlatform, "t7000");
	modInfo29.innerHeight = 1334;
	modInfo29.innerWidth = 750;
	modInfo29.width = 375;
	modInfo29.height = 667;
	m_vecModel.push_back(modInfo29);

	MODELINFO modInfo30 = {0};
	strcpy(modInfo30.szModel,"iPhone7,2");
	strcpy(modInfo30.szBuild,"14A456");
	strcpy(modInfo30.szVersion,"10.0.2");
	strcpy(modInfo30.szHardwarePlatform, "t7000");
	modInfo30.innerHeight = 1334;
	modInfo30.innerWidth = 750;
	modInfo30.width = 375;
	modInfo30.height = 667;
	m_vecModel.push_back(modInfo30);

	MODELINFO modInfo31 = {0};
	strcpy(modInfo31.szModel,"iPhone7,2");
	strcpy(modInfo31.szBuild,"14B72");
	strcpy(modInfo31.szVersion,"10.1");
	strcpy(modInfo31.szHardwarePlatform, "t7000");
	modInfo31.innerHeight = 1334;
	modInfo31.innerWidth = 750;
	modInfo31.width = 375;
	modInfo31.height = 667;
	m_vecModel.push_back(modInfo31);

	MODELINFO modInfo32 = {0};
	strcpy(modInfo32.szModel,"iPhone7,2");
	strcpy(modInfo32.szBuild,"14B100");
	strcpy(modInfo32.szVersion,"10.1.1");
	strcpy(modInfo32.szHardwarePlatform, "t7000");
	modInfo32.innerHeight = 1334;
	modInfo32.innerWidth = 750;
	modInfo32.width = 375;
	modInfo32.height = 667;
	m_vecModel.push_back(modInfo32);

	MODELINFO modInfo33 = {0};
	strcpy(modInfo33.szModel,"iPhone8,1");
	strcpy(modInfo33.szBuild,"14A403");
	strcpy(modInfo33.szVersion,"10.0.1");
	strcpy(modInfo33.szHardwarePlatform, "s8000");
	modInfo33.innerHeight = 1334;
	modInfo33.innerWidth = 750;
	modInfo33.width = 375;
	modInfo33.height = 667;
	m_vecModel.push_back(modInfo33);

	MODELINFO modInfo34 = {0};
	strcpy(modInfo34.szModel,"iPhone8,1");
	strcpy(modInfo34.szBuild,"14A456");
	strcpy(modInfo34.szVersion,"10.0.2");
	strcpy(modInfo34.szHardwarePlatform, "s8000");
	modInfo34.innerHeight = 1334;
	modInfo34.innerWidth = 750;
	modInfo34.width = 375;
	modInfo34.height = 667;
	m_vecModel.push_back(modInfo34);

	MODELINFO modInfo35 = {0};
	strcpy(modInfo35.szModel,"iPhone8,1");
	strcpy(modInfo35.szBuild,"14B72");
	strcpy(modInfo35.szVersion,"10.1");
	strcpy(modInfo35.szHardwarePlatform, "s8000");
	modInfo35.innerHeight = 1334;
	modInfo35.innerWidth = 750;
	modInfo35.width = 375;
	modInfo35.height = 667;
	m_vecModel.push_back(modInfo35);

	MODELINFO modInfo36 = {0};
	strcpy(modInfo36.szModel,"iPhone8,1");
	strcpy(modInfo36.szBuild,"14B100");
	strcpy(modInfo36.szVersion,"10.1.1");
	strcpy(modInfo36.szHardwarePlatform, "s8000");
	modInfo36.innerHeight = 1334;
	modInfo36.innerWidth = 750;
	modInfo36.width = 375;
	modInfo36.height = 667;
	m_vecModel.push_back(modInfo36);

	MODELINFO modInfo37 = {0};
	strcpy(modInfo37.szModel,"iPhone8,2");
	strcpy(modInfo37.szBuild,"14A403");
	strcpy(modInfo37.szVersion,"10.0.1");
	strcpy(modInfo37.szHardwarePlatform, "s8000");
	modInfo37.innerHeight = 1334;
	modInfo37.innerWidth = 750;
	modInfo37.width = 414;
	modInfo37.height = 736;
	m_vecModel.push_back(modInfo37);

	MODELINFO modInfo38 = {0};
	strcpy(modInfo38.szModel,"iPhone8,2");
	strcpy(modInfo38.szBuild,"14A456");
	strcpy(modInfo38.szVersion,"10.0.2");
	strcpy(modInfo38.szHardwarePlatform, "s8000");
	modInfo38.innerHeight = 1334;
	modInfo38.innerWidth = 750;
	modInfo38.width = 414;
	modInfo38.height = 736;
	m_vecModel.push_back(modInfo38);

	MODELINFO modInfo39 = {0};
	strcpy(modInfo39.szModel,"iPhone8,2");
	strcpy(modInfo39.szBuild,"14B72");
	strcpy(modInfo39.szVersion,"10.1");
	strcpy(modInfo39.szHardwarePlatform, "s8000");
	modInfo39.innerHeight = 1334;
	modInfo39.innerWidth = 750;
	modInfo39.width = 414;
	modInfo39.height = 736;
	m_vecModel.push_back(modInfo39);

	MODELINFO modInfo40 = {0};
	strcpy(modInfo40.szModel,"iPhone8,2");
	strcpy(modInfo40.szBuild,"14B100");
	strcpy(modInfo40.szVersion,"10.1.1");
	strcpy(modInfo40.szHardwarePlatform, "s8000");
	modInfo40.innerHeight = 1334;
	modInfo40.innerWidth = 750;
	modInfo40.width = 414;
	modInfo40.height = 736;
	m_vecModel.push_back(modInfo40);

	MODELINFO modInfo41 = {0};
	strcpy(modInfo41.szModel,"iPhone8,4");
	strcpy(modInfo41.szBuild,"14A403");
	strcpy(modInfo41.szVersion,"10.0.1");
	strcpy(modInfo41.szHardwarePlatform, "s8960");
	modInfo41.innerHeight = 1334;
	modInfo41.innerWidth = 750;
	modInfo41.width = 320;
	modInfo41.height = 568 ;
	m_vecModel.push_back(modInfo41);

	MODELINFO modInfo42 = {0};
	strcpy(modInfo42.szModel,"iPhone8,4");
	strcpy(modInfo42.szBuild,"14A456");
	strcpy(modInfo42.szVersion,"10.0.2");
	strcpy(modInfo42.szHardwarePlatform, "s8960");
	modInfo42.innerHeight = 1334;
	modInfo42.innerWidth = 750;
	modInfo42.width = 320;
	modInfo42.height = 568;
	m_vecModel.push_back(modInfo42);

	MODELINFO modInfo43 = {0};
	strcpy(modInfo43.szModel,"iPhone8,4");
	strcpy(modInfo43.szBuild,"14B72");
	strcpy(modInfo43.szVersion,"10.1");
	strcpy(modInfo43.szHardwarePlatform, "s8960");
	modInfo43.innerHeight = 1334;
	modInfo43.innerWidth = 750;
	modInfo43.width = 320;
	modInfo43.height = 568;
	m_vecModel.push_back(modInfo43);

	MODELINFO modInfo44 = {0};
	strcpy(modInfo44.szModel,"iPhone8,4");
	strcpy(modInfo44.szBuild,"14B100");
	strcpy(modInfo44.szVersion,"10.1.1");
	strcpy(modInfo44.szHardwarePlatform, "s8960");
	modInfo44.innerHeight = 1334;
	modInfo44.innerWidth = 750;
	modInfo44.width = 320;
	modInfo44.height = 568;
	m_vecModel.push_back(modInfo44);

	MODELINFO modInfo45 = {0};
	strcpy(modInfo45.szModel,"iPhone9,1");
	strcpy(modInfo45.szBuild,"14A403");
	strcpy(modInfo45.szVersion,"10.0.1");
	strcpy(modInfo45.szHardwarePlatform, "t8010");
	modInfo45.innerHeight = 1334;
	modInfo45.innerWidth = 750;
	modInfo45.width = 375;
	modInfo45.height =667;
	m_vecModel.push_back(modInfo45);

	MODELINFO modInfo46 = {0};
	strcpy(modInfo46.szModel,"iPhone9,1");
	strcpy(modInfo46.szBuild,"14A456");
	strcpy(modInfo46.szVersion,"10.0.2");
	strcpy(modInfo46.szHardwarePlatform, "t8010");
	modInfo46.innerHeight = 1334;
	modInfo46.innerWidth = 750;
	modInfo46.width = 375;
	modInfo46.height =667;
	m_vecModel.push_back(modInfo46);

	MODELINFO modInfo47 = {0};
	strcpy(modInfo47.szModel,"iPhone9,1");
	strcpy(modInfo47.szBuild,"14B72");
	strcpy(modInfo47.szVersion,"10.1");
	strcpy(modInfo47.szHardwarePlatform, "t8010");
	modInfo47.innerHeight = 1334;
	modInfo47.innerWidth = 750;
	modInfo47.width = 375;
	modInfo47.height =667;
	m_vecModel.push_back(modInfo47);

	MODELINFO modInfo48 = {0};
	strcpy(modInfo48.szModel,"iPhone9,1");
	strcpy(modInfo48.szBuild,"14B100");
	strcpy(modInfo48.szVersion,"10.1.1");
	modInfo48.width = 375;
	modInfo48.height =667;
	m_vecModel.push_back(modInfo48);

	MODELINFO modInfo49 = {0};
	strcpy(modInfo49.szModel,"iPhone9,2");
	strcpy(modInfo49.szBuild,"14A403");
	strcpy(modInfo49.szVersion,"10.0.1");
	strcpy(modInfo49.szHardwarePlatform, "t8010");
	modInfo49.innerHeight = 1334;
	modInfo49.innerWidth = 750;
	modInfo49.width = 414;
	modInfo49.height =736;
	m_vecModel.push_back(modInfo49);

	MODELINFO modInfo50 = {0};
	strcpy(modInfo50.szModel,"iPhone9,2");
	strcpy(modInfo50.szBuild,"14A456");
	strcpy(modInfo50.szVersion,"10.0.2");
	strcpy(modInfo50.szHardwarePlatform, "t8010");
	modInfo50.innerHeight = 1334;
	modInfo50.innerWidth = 750;
	modInfo50.width = 414;
	modInfo50.height =736;
	m_vecModel.push_back(modInfo50);

	MODELINFO modInfo51 = {0};
	strcpy(modInfo51.szModel,"iPhone9,2");
	strcpy(modInfo51.szBuild,"14B72");
	strcpy(modInfo51.szVersion,"10.1");
	strcpy(modInfo51.szHardwarePlatform, "t8010");
	modInfo51.innerHeight = 1334;
	modInfo51.innerWidth = 750;
	modInfo51.width = 414;
	modInfo51.height =736;
	m_vecModel.push_back(modInfo51);

	MODELINFO modInfo52 = {0};
	strcpy(modInfo52.szModel,"iPhone9,2");
	strcpy(modInfo52.szBuild,"14B100");
	strcpy(modInfo52.szVersion,"10.1.1");
	strcpy(modInfo52.szHardwarePlatform, "t8010");
	modInfo52.innerHeight = 1334;
	modInfo52.innerWidth = 750;
	modInfo52.width = 414;
	modInfo52.height =736;
	m_vecModel.push_back(modInfo52);

	MODELINFO modInfo53 = {0};
	strcpy(modInfo53.szModel,"iPhone9,3");
	strcpy(modInfo53.szBuild,"14A403");
	strcpy(modInfo53.szVersion,"10.0.1");
	strcpy(modInfo53.szHardwarePlatform, "t8010");
	modInfo53.innerHeight = 1334;
	modInfo53.innerWidth = 750;
	modInfo53.width = 375;
	modInfo53.height = 667;
	m_vecModel.push_back(modInfo53);

	MODELINFO modInfo54 = {0};
	strcpy(modInfo54.szModel,"iPhone9,3");
	strcpy(modInfo54.szBuild,"14A456");
	strcpy(modInfo54.szVersion,"10.0.2");
	strcpy(modInfo54.szHardwarePlatform, "t8010");
	modInfo54.innerHeight = 1334;
	modInfo54.innerWidth = 750;
	modInfo54.width = 375;
	modInfo54.height = 667;
	m_vecModel.push_back(modInfo54);

	MODELINFO modInfo55 = {0};
	strcpy(modInfo55.szModel,"iPhone9,3");
	strcpy(modInfo55.szBuild,"14B72");
	strcpy(modInfo55.szVersion,"10.1");
	strcpy(modInfo55.szHardwarePlatform, "t8010");
	modInfo55.innerHeight = 1334;
	modInfo55.innerWidth = 750;
	modInfo55.width = 375;
	modInfo55.height = 667;
	m_vecModel.push_back(modInfo55);

	MODELINFO modInfo56 = {0};
	strcpy(modInfo56.szModel,"iPhone9,3");
	strcpy(modInfo56.szBuild,"14B100");
	strcpy(modInfo56.szVersion,"10.1.1");
	strcpy(modInfo56.szHardwarePlatform, "t8010");
	modInfo56.innerHeight = 1334;
	modInfo56.innerWidth = 750;
	modInfo56.width = 375;
	modInfo56.height = 667;
	m_vecModel.push_back(modInfo56);

	MODELINFO modInfo57 = {0};
	strcpy(modInfo57.szModel,"iPhone9,4");
	strcpy(modInfo57.szBuild,"14A403");
	strcpy(modInfo57.szVersion,"10.0.1");
	strcpy(modInfo57.szHardwarePlatform, "t8010");
	modInfo57.innerHeight = 1334;
	modInfo57.innerWidth = 750;
	modInfo57.width = 414;
	modInfo57.height = 736;
	m_vecModel.push_back(modInfo57);

	MODELINFO modInfo58 = {0};
	strcpy(modInfo58.szModel,"iPhone9,4");
	strcpy(modInfo58.szBuild,"14A456");
	strcpy(modInfo58.szVersion,"10.0.2");
	strcpy(modInfo58.szHardwarePlatform, "t8010");
	modInfo58.innerHeight = 1334;
	modInfo58.innerWidth = 750;
	modInfo58.width = 414;
	modInfo58.height = 736;
	m_vecModel.push_back(modInfo58);

	MODELINFO modInfo59 = {0};
	strcpy(modInfo59.szModel,"iPhone9,4");
	strcpy(modInfo59.szBuild,"14B72");
	strcpy(modInfo59.szVersion,"10.1");
	strcpy(modInfo59.szHardwarePlatform, "t8010");
	modInfo59.innerHeight = 1334;
	modInfo59.innerWidth = 750;
	modInfo59.width = 414;
	modInfo59.height = 736;
	m_vecModel.push_back(modInfo59);

	MODELINFO modInfo60 = {0};
	strcpy(modInfo60.szModel,"iPhone9,4");
	strcpy(modInfo60.szBuild,"14B100");
	strcpy(modInfo60.szVersion,"10.1.1");
	strcpy(modInfo60.szHardwarePlatform, "t8010");
	modInfo60.innerHeight = 1334;
	modInfo60.innerWidth = 750;
	modInfo60.width = 414;
	modInfo60.height = 736;
	m_vecModel.push_back(modInfo60);


}

void CxhASODlg::GetIP(void)
{
	char szhostname[128] = {0};
    CString str;
 
	//获得主机名
	int res = gethostname(szhostname,128);
	if( res == 0 )
	{
		// 获得主机ip地址
		struct hostent * phost;
		int i;
		phost = gethostbyname(szhostname);
		i=0;
		int j;
		int h_length=4;
		for( j = 0; j<h_length; j++ )
		{
			CString addr;
			
			if( j > 0 )
				str += _T(".");
			
			addr.Format(_T("%u"), (unsigned int)((unsigned
				char*)phost->h_addr_list[i])[j]);
			str += addr;
		}
  
	}
	m_currentIP.SetWindowText(str);
	
}

void CxhASODlg::SetVPNIPDisplay(void)
{
    DWORD dwSize = 0;
    DWORD dwRetVal = 0;

    unsigned int i = 0;

    // Set the flags to pass to GetAdaptersAddresses
    ULONG flags = GAA_FLAG_INCLUDE_PREFIX;

    // default to unspecified address family (both)
    ULONG family = AF_UNSPEC;

    PIP_ADAPTER_ADDRESSES pAddresses = NULL;
    ULONG outBufLen = 0;
    ULONG Iterations = 0;

    PIP_ADAPTER_ADDRESSES pCurrAddresses = NULL;
    PIP_ADAPTER_UNICAST_ADDRESS pUnicast = NULL;

    // Allocate a 15 KB buffer to start with.
    outBufLen = WORKING_BUFFER_SIZE;
	TCHAR vpnIpAddress[MAX_PATH] = {0};

    do 
	{

        pAddresses = (IP_ADAPTER_ADDRESSES *) MALLOC(outBufLen);
        if (pAddresses == NULL)
		{
            exit(1);
        }

        dwRetVal = GetAdaptersAddresses(family, flags, NULL, pAddresses, &outBufLen);

        if (dwRetVal == ERROR_BUFFER_OVERFLOW) 
		{
            FREE(pAddresses);
            pAddresses = NULL;
        } 
		else
		{
            break;
        }

        Iterations++;

    } while ((dwRetVal == ERROR_BUFFER_OVERFLOW) && (Iterations < MAX_TRIES));

	if (dwRetVal == NO_ERROR)
	{
		 // If successful, output some information from the data we received
	    pCurrAddresses = pAddresses;
		while (pCurrAddresses)
		{
			if (wcscmp(pCurrAddresses->FriendlyName,m_vpnNameContext.GetBuffer()) == 0)
			{
				pUnicast = pCurrAddresses->FirstUnicastAddress;
				if (pUnicast != NULL)
				{
					//获取vpn的ip地址
					swprintf(vpnIpAddress,TEXT("%d.%d.%d.%d"),
						pUnicast->Address.lpSockaddr->sa_data[2],
						pUnicast->Address.lpSockaddr->sa_data[3],
						pUnicast->Address.lpSockaddr->sa_data[4],
						pUnicast->Address.lpSockaddr->sa_data[5]);
					
					m_currentIP.SetWindowText(vpnIpAddress);

					break;
				}
			}

			pCurrAddresses = pCurrAddresses->Next;
		}
	}

	if (pAddresses)
	{
		FREE(pAddresses);
		pAddresses = NULL;
	}

	return;
}

BOOL CxhASODlg::CheckInternetStatus(void)
{
	BOOL	bResult = FALSE;
	char	szIPAddress[128] = {0};
	TCHAR	strIPAddress[64] = {0};

	bResult = getDynamicIPAddress(strIPAddress);
	if (!bResult)
	{
		OutputDebugStringA("getDynamicIPAddress failed job...");
		return bResult;
	}

	m_currentIP.SetWindowText(strIPAddress);
	
	WideCharToMultiByte(CP_ACP,0,strIPAddress,wcslen(strIPAddress),szIPAddress,128,NULL,NULL);
	
	bResult = detectInternetConnect(szIPAddress);
	if (!bResult)
	{
		OutputDebugStringA("detectInternetConnect failed job....");
		return bResult;
	}


	return bResult;
}

void CxhASODlg::deleteCookieFile(void)
{
	CFileFind  fileFind;
	TCHAR szPath[MAX_PATH];
	if (SHGetSpecialFolderPath(NULL, szPath, CSIDL_COOKIES, FALSE))
	{
		CFileFind finder;
     
		// build a string with wildcards
		CString strWildcard(szPath);
		strWildcard += _T("\\*.*");
	     
		// start working for files
		BOOL bWorking = finder.FindFile(strWildcard);
	     
		while (bWorking)
		{
			bWorking = finder.FindNextFile();
	         
			// skip . and .. files; otherwise, we'd
			// recur infinitely!
	         
			if (finder.IsDots())
				continue;
	         
			// if it's a directory, recursively search it
	         
			if (finder.IsDirectory())
			{
				CString str = finder.GetFilePath();
				//TRACE(_T("%s\n"), (LPCTSTR)str);
				//Recurse(str);
			}
			else
			{
				CString strFilePath = finder.GetFilePath();
				int flag = strFilePath.ReverseFind(_T('.'));
				if(-1 != flag)
				{
					if(0 == strFilePath.Mid(flag).Compare(_T(".txt")))
					{
						DeleteFile(strFilePath);
					}
				}
			}
		}
	     
		finder.Close();
	}

	BOOL res = ::InternetSetOption(0,INTERNET_OPTION_END_BROWSER_SESSION,NULL,0);

	return;

}

BOOL CxhASODlg::UnityFunction(CString strHotKeyWord)
{
	char passwordToken[MAX_PATH] = {0};
	char strDsid[64] = {0};
	char saleId[64] = "1116587225";////app在商店的id号，就以这个id为例  吉祥坊
	char AppExtVrsId[64] = {0};
	char strRefferUrl[MAX_PATH] ={0};

	char szPageSearchUrl[MAX_PATH] = {0};
	char szPageDetails[512] = {0};
	char szClientId[MAX_PATH] = {0};
	DEVICEINFO deviceInfo = {0};
	wcscpy(deviceInfo.strUdid,TEXT("fc752fa69fff9479fcdc085704ea6280a12daae4"));
	wcscpy(deviceInfo.strSerialNumber,TEXT("DX6PQNKNFR9M"));
	wcscpy(deviceInfo.strXp_ci,TEXT("3z38vh7HzB9Uz4vhz9JbzTpha8ksu"));
	wcscpy(deviceInfo.strX_Apple_I_md,TEXT("AAAABQAAABA5I3Sg/h8LgjVaO1yoH/e3AAAAAw=="));
	wcscpy(deviceInfo.strX_Apple_I_md_m,TEXT("HCpCt1cNlYx5yYCeSL9GU0A0W5K2yhoVE0RiuEji8wvYGVrL5JoeNDfBfIrKiICX/I7LzDyFOcGu16QV"));

	MODELINFO modInfo = {0};
	strcpy(modInfo.szModel,"iPhone6,1");
	strcpy(modInfo.szBuild,"14A456");
	strcpy(modInfo.szVersion,"10.0.2");
	modInfo.width = 320;
	modInfo.height = 568;
	

	deleteCookieFile();
	BOOL Res = false;
/*************************下面注释掉这部分不需要*************************************/
	char szTargetUrl[] = "/cn/app/%E6%BF%80%E6%83%85%E5%AF%BB%E7%88%B1-%E5%BF%AB%E9%80%9F%E7%BA%A6%E9%99%84%E8%BF%91%E5%AF%82%E5%AF%9E%E7%9A%84%E4%BA%BA/id1116587225?mt=8";//"/cn/app/ji-xiang-fang-zu-qiu-ou-pei/id1176510531?mt=8";//"/cn/app/tong-hua-shun-zhi-zun-ban/id954724812?mt=8";//"/cn/app/pei-wo-lu-you-cheng-gong-nan/id1139405796?mt=8";//关键词：美丽约


	//创建网络工作对象
	LPVOID lpNetworkObj = pCreateNetworkObject();
	if (!lpNetworkObj)
	{
		return FALSE;
	}
	Res = pSetUAInfo(lpNetworkObj,&modInfo);
	//授权网路验证机制
	Res = pGSA_Apple_NetworkVerifier(lpNetworkObj,"698b783068bf19786e46502a4e0165dccd4dcc0c",&modInfo,&deviceInfo,NULL,0);
	if (!Res)
	{
		return FALSE;
	}

	//测试重搜索
//	Res = pOnlySearchKeyword(lpNetworkObj,strHotKeyWord.GetBuffer());

	//登录
	Res = pLoginAppStore(lpNetworkObj,"767949402@qq.com","Jw421022",
		TEXT("202"),TEXT("203"),TEXT("201"),&deviceInfo,passwordToken,strDsid,NULL,0);
	if (!Res)
	{
		pUninitialModule();
		return FALSE;
	}

	//搜索
	Res = pSearchKeyHotWord(lpNetworkObj,
							strHotKeyWord.GetBuffer(),
							saleId,
							strDsid,
							passwordToken,
							szTargetUrl);
	if (!Res)
	{
		pUninitialModule();
		return FALSE;
	}

	//获取AppExtVrsId的值
	Res = pGetAppExtVrsIdValue(lpNetworkObj,passwordToken,strDsid,szTargetUrl);
	if (!Res)
	{
		return FALSE;
	}

	//下载
	Res = pDownloadApp(lpNetworkObj,passwordToken,strDsid,saleId,"1000",FALSE,FALSE);
	if (!Res)
	{
		return FALSE;
	}

	Res = pBuyConfirmActiveApp(lpNetworkObj,passwordToken,saleId,strDsid,szTargetUrl);
	if (!Res)
	{
		return FALSE;
	}

	//评论
    Res = pCommentApp(lpNetworkObj,passwordToken,strDsid,saleId,1,_T("爱心浩浩"),_T("卸载了重新装还是闪退"),_T("怎么回事，已经相当无语了，卸载了重新装上还是闪退"));
	if (!Res)
	{
		pUninitialModule();
		return FALSE;
	}

	if (lpNetworkObj)
	{
		pReleaseNetworkObject(lpNetworkObj);
	}

	//卸载模块
	pUninitialModule();

	return TRUE;
}

int CxhASODlg::ExectueAProcess(PSENDINFO pSendInfo,int nIndex)
{
	int status = 0;
	CString strCurrentTaskCounts;
	MONPROCESSINFO monProcInfo = {0};
	TCHAR strPath[MAX_PATH] = {0};
	DWORD dwResult = GetModuleFileName(NULL,strPath,MAX_PATH);
	CString filePath(strPath);
	int Pos = filePath.ReverseFind(_T('\\'));
	filePath.Truncate(Pos);
	filePath += _T("\\xhExecute.exe");

	HANDLE hEvent = CreateEvent(NULL,FALSE,FALSE,pSendInfo->strEventName);
	if (!hEvent)
	{
		m_appleIdLoginAppDownload.SetItemText(nIndex,2,TEXT("创建事件失败"));
		return status;
	}


	m_appleIdLoginAppDownload.SetItemText(nIndex,2,TEXT("正在执行中……"));

	//写共享内存
	memoryshared ms;
	CString strMemSharedName;
	LARGE_INTEGER timeRand;
	QueryPerformanceCounter(&timeRand);
	strMemSharedName.Format(TEXT("__SHRMEM%d__"), timeRand.LowPart);
	ms.Create(strMemSharedName.GetBuffer(), sizeof(SENDINFO));
	ms.Write(1, (char*)pSendInfo, sizeof(SENDINFO));

	 //定义安全属性结构体  
    SECURITY_ATTRIBUTES sa;  
    //可以被继承  
    sa.bInheritHandle = TRUE;  
    //默认安全描述  
    sa.lpSecurityDescriptor = NULL;  
    sa.nLength = sizeof(SECURITY_ATTRIBUTES); 

	//新创建的进程的主窗口信息  
    STARTUPINFO sui;  
    ZeroMemory(&sui,sizeof(STARTUPINFO));  
    sui.cb = sizeof(STARTUPINFO);  
    sui.dwFlags = STARTF_USESTDHANDLES;  
    sui.hStdInput = m_hRead;  
    sui.hStdOutput = m_hWrite;  
    //获取标准输入句柄  
    sui.hStdError = GetStdHandle(STD_ERROR_HANDLE);  
      
    //新创建的进程和主线程信息，由函数填写  
    PROCESS_INFORMATION pi;  

	//创建子进程
	if(!CreateProcess(filePath,                                 //子进程名  
                     strMemSharedName.GetBuffer(),              //命令行参数为共享内存名
                     NULL,                                      //新创建的进程使用默认安全级别  
                     NULL,                                      //新创建的主线程使用默认安全级别  
                     TRUE,                                      //子进程可以继承父进程的句柄  
                     0,                                         //无特殊创建标记  
                     NULL,                                      //新进程使用调用进程的环境块  
                     NULL,                                      //子进程和父进程具有相同的当前路径  
                     &sui,                                      //启动信息  
                     &pi))                                      //新创建的进程和主线程信息  
    {  
		m_appleIdLoginAppDownload.SetItemText(nIndex,2,TEXT("创建进程失败"));
        return status;  
    }

	//记录索引值和任务子进程信息
	EnterCriticalSection(&g_cs);

	strCurrentTaskCounts.Format(TEXT("%d"),m_ItemCount);
	m_currentTaskCount.SetWindowText(strCurrentTaskCounts);

	monProcInfo.nIndex = nIndex;
	monProcInfo.processInfo = pi;
	m_monTaskList.push_back(monProcInfo);
	LeaveCriticalSection(&g_cs);
	
	//等待客户端信号
	DWORD dwRet = WaitForSingleObject(hEvent,120*1000);//最长等待2分钟
	if (dwRet == WAIT_TIMEOUT)
	{
		InterlockedDecrement(&m_ItemCount);
		m_appleIdLoginAppDownload.SetItemText(nIndex,2,TEXT("共享内存错误"));
		//关闭句柄
		if (pi.hProcess)
		{
			CloseHandle(pi.hProcess);  
		}
		if (pi.hThread)
		{
			CloseHandle(pi.hThread); 
		}
		return status;
	}
	::Sleep(1000);
	if (hEvent)
	{
		CloseHandle(hEvent);
	}

    return status;

}

BOOL CxhASODlg::getFunInterface()
{
	HMODULE hIntwork = NULL;
	HMODULE hxhiTunes = NULL;
	
	hIntwork = LoadLibrary(_T("xhwork.dll"));
	if (hIntwork == NULL)
	{
		return FALSE;
	}

	hxhiTunes = LoadLibrary(_T("xhiTunes.dll"));
	if (hxhiTunes == NULL)
	{
		return FALSE;
	}

	pInitialModule = (PINITIALIZEMODULE)GetProcAddress(hxhiTunes,"InitializeModule");
	if (!pInitialModule)
	{
		return FALSE;
	}

	pUninitialModule = (PUNINITIALIZEMODULE)GetProcAddress(hxhiTunes,"UnInitializeModule");
	if (!pUninitialModule)
	{
		return FALSE;
	}

	//加密字符串
	char srcData[] = "iTunesCore.dll";
	int dataLen = strlen(srcData);
	char key[] = "xhiTunes";
	kEncodeData(srcData,key);

	BOOL Result = pInitialModule((LPVOID)srcData);
	if (!Result)
	{
		return FALSE;
	}

	pInitCriticalSection = (PXHINITCRITICALSECTION)GetProcAddress(hIntwork,"xhInitCriticalSection");
	if (!pInitCriticalSection)
	{
		return FALSE;
	}
	pDeleteCriticalSection = (PXHDELETECRITICALSECTION)GetProcAddress(hIntwork,"xhDeleteCriticalSection");
	if (!pDeleteCriticalSection)
	{
		return FALSE;
	}

	pCreateNetworkObject = (PXHCREATENETWORKOBJECT)GetProcAddress(hIntwork,"xhCreateNetworkObject");
	if (!pCreateNetworkObject)
	{
		return FALSE;
	}
	pReleaseNetworkObject = (PXHRELEASENEWTWORKOBJECT)GetProcAddress(hIntwork,"xhReleaseNewtworkObject");
	if (!pReleaseNetworkObject)
	{
		return FALSE;
	}

	pGSA_Apple_Thread_NetworkVerifier = (PXHGSA_APPLE_THREAD_NETWORKVERIFIER)GetProcAddress(hIntwork,"xhGSA_Apple_Thread_NetworkVerifier");
	if (!pGSA_Apple_Thread_NetworkVerifier)
	{
		return FALSE;
	}
	pSetUAInfo = (PXHSETUAINFO)GetProcAddress(hIntwork,"xhSetUAInfo");
	if (!pSetUAInfo)
	{
		return FALSE;
	}
	pGSA_Apple_NetworkVerifier = (PXHGSA_APPLE_NETWORKVERIFIER)GetProcAddress(hIntwork,"xhGSA_Apple_NetworkVerifier");
	if (!pGSA_Apple_NetworkVerifier)
	{
		return FALSE;
	}

	pLoginAppStore = (PXHLOGINAPPSTORE)GetProcAddress(hIntwork,"xhLoginAppStore");
	if (!pLoginAppStore)
	{
		return FALSE;
	}
	

	pDownloadApp = (PXHDOWNLOADAPP)GetProcAddress(hIntwork,"xhDownloadApp");
	if (!pDownloadApp)
	{
		return FALSE;
	}

	pCommentApp = (PXHCOMMENTAPP)GetProcAddress(hIntwork,"xhCommentApp");
	if (!pCommentApp)
	{
		return FALSE;
	}

	pSearchKeyHotWord = (PXHSEARCHKEYHOTWORD)GetProcAddress(hIntwork,"xhSearchKeyHotWord");
	if (!pSearchKeyHotWord)
	{
		return FALSE;
	}
	pGetAppExtVrsIdValue = (PXHGETAPPEXTVRSIDVALUE)GetProcAddress(hIntwork,"xhGetAppExtVrsIdValue");
	if (!pGetAppExtVrsIdValue)
	{
		return FALSE;
	}

	pBuyConfirmActiveApp = (PXHBUYCONFIRMACTIVEAPP)GetProcAddress(hIntwork,"xhBuyConfirmActiveApp");
	if (!pBuyConfirmActiveApp)
	{
		return FALSE;
	}

	pRegisterAppleId = (PXHREGISTERAPPLEID)GetProcAddress(hIntwork,"xhRegisterAppleId");
	if (!pRegisterAppleId)
	{
		return FALSE;
	}

	pOnlySearchKeyword = (PXHONLYSEARCHKEYWORD)GetProcAddress(hIntwork,"xhOnlySearchKeyword");
	if (!pOnlySearchKeyword)
	{
		return FALSE;
	}

	pInitCriticalSection();

	return TRUE;
}

int CxhASODlg::InterfaceFunction(PSENDINFO pSendInfo)
{
	char szPasswordToken[MAX_PATH] = {0};

	char szDsid[64] = {0};
	char szRefferUrl[MAX_PATH] ={0};
	char szPageDetails[512] = {0};
	char szAppleId[MAX_PATH] = {0};
	char szPwd[MAX_PATH] = {0};
	char szAppSaleId[MAX_PATH] = {0};
	char szAppExtVrsId[64] = {0};
	char szRate[10] = {0};
	char szClientId[MAX_PATH] = {0};
	char szTargetUrl[MAX_PATH] = {0};
	char szAppPrice[32] = {0};
	char szUdid[MAX_PATH] = {0};
	BOOL Res = FALSE;
	int status = 0;

	InterlockedIncrement(&m_ItemCount);
	::WideCharToMultiByte(CP_ACP,0,pSendInfo->strTargetUrl,wcslen(pSendInfo->strTargetUrl),szTargetUrl,MAX_PATH,NULL,NULL);
	::WideCharToMultiByte(CP_ACP,0,pSendInfo->accountInfo.strAppleId,wcslen(pSendInfo->accountInfo.strAppleId),szAppleId,MAX_PATH,NULL,NULL);
	::WideCharToMultiByte(CP_ACP,0,pSendInfo->accountInfo.strPassword,wcslen(pSendInfo->accountInfo.strPassword),szPwd,MAX_PATH,NULL,NULL);
	::WideCharToMultiByte(CP_ACP,0,pSendInfo->strSaleId,wcslen(pSendInfo->strSaleId),szAppSaleId,MAX_PATH,NULL,NULL);
	::WideCharToMultiByte(CP_ACP,0,pSendInfo->strRate,wcslen(pSendInfo->strRate),szRate,10,NULL,NULL);
	::WideCharToMultiByte(CP_ACP,0,pSendInfo->strAppPrice,wcslen(pSendInfo->strAppPrice),szAppPrice,32,NULL,NULL);
	::WideCharToMultiByte(CP_ACP,0,pSendInfo->accountInfo.deviceInfo.strUdid,wcslen(pSendInfo->accountInfo.deviceInfo.strUdid),szUdid,MAX_PATH,NULL,NULL);

	LPVOID lpNetworkObj = pCreateNetworkObject();
	if (!lpNetworkObj)
	{
		return status;
	}

	switch (pSendInfo->type)
	{
		case ONLYLOGIN:
			{
				Res = pSetUAInfo(lpNetworkObj,&pSendInfo->modelInfo);
				Res = pLoginAppStore(lpNetworkObj,
									 szAppleId,
									 szPwd,
									 pSendInfo->strFatherAndMather,
									 pSendInfo->strTeacher,
									 pSendInfo->strBook,
									 &pSendInfo->accountInfo.deviceInfo,
									 szPasswordToken,
									 szDsid,
									 pSendInfo->strProxy,
									 pSendInfo->netType);
				if (!Res)
				{
					Res = pLoginAppStore(lpNetworkObj,
									 szAppleId,
									 szPwd,
									 pSendInfo->strFatherAndMather,
									 pSendInfo->strTeacher,
									 pSendInfo->strBook,
									 &pSendInfo->accountInfo.deviceInfo,
									 szPasswordToken,
									 szDsid,
									 pSendInfo->strProxy,
									 pSendInfo->netType);
					if (!Res)
					{
						status = LOGIN_FAILED;
					}
				}
				else
				{
					status = LOGIN_SUCCESS;
				}

			}break;
		case LOGINDOWNLOADAPP://登录下载
			{
				Res = pSetUAInfo(lpNetworkObj,&pSendInfo->modelInfo);
				Res = pLoginAppStore(lpNetworkObj,
									 szAppleId,
									 szPwd,
									 pSendInfo->strFatherAndMather,
									 pSendInfo->strTeacher,
									 pSendInfo->strBook,
									 &pSendInfo->accountInfo.deviceInfo,
									 szPasswordToken,
									 szDsid,
									 pSendInfo->strProxy,
									 pSendInfo->netType);
				if (!Res)
				{
					Res = pLoginAppStore(lpNetworkObj,
									 szAppleId,
									 szPwd,
									 pSendInfo->strFatherAndMather,
									 pSendInfo->strTeacher,
									 pSendInfo->strBook,
									 &pSendInfo->accountInfo.deviceInfo,
									 szPasswordToken,
									 szDsid,
									 pSendInfo->strProxy,
									 pSendInfo->netType);
					if (!Res)
					{
						status = LOGIN_FAILED;
					}
				}
				else
				{

					//获取AppExtVrsId的值
					Res = pGetAppExtVrsIdValue(lpNetworkObj,szPasswordToken,szDsid,szTargetUrl);
					if (!Res)
					{
						Res = pGetAppExtVrsIdValue(lpNetworkObj,szPasswordToken,szDsid,szTargetUrl);
						if (!Res)
						{
							status = COMMENTFAILED;

						}
					}
					else
					{
						Res = pDownloadApp(lpNetworkObj,szPasswordToken,szDsid,szAppSaleId,szAppPrice,FALSE,pSendInfo->bPaidApp);
						if (!Res)
						{
							Res = pDownloadApp(lpNetworkObj,szPasswordToken,szDsid,szAppSaleId,szAppPrice,FALSE,pSendInfo->bPaidApp);
							if (!Res)
							{
								status = DOWNLOADFAILED;
							}
						}
						else
						{
							status = DOWNLOADSUCCESS;
							
						}
					}
				}

			}break;
		case COMMENTAPP://登录下载评论
			{
				Res = pSetUAInfo(lpNetworkObj,&pSendInfo->modelInfo);
				Res = pLoginAppStore(lpNetworkObj,
									szAppleId,
									szPwd,
									pSendInfo->strFatherAndMather,
									 pSendInfo->strTeacher,
									 pSendInfo->strBook,
									 &pSendInfo->accountInfo.deviceInfo,
									 szPasswordToken,
									 szDsid,
									 pSendInfo->strProxy,
									 pSendInfo->netType);
				if (!Res)
				{
					Res = pLoginAppStore(lpNetworkObj,
									szAppleId,
									szPwd,
									pSendInfo->strFatherAndMather,
									 pSendInfo->strTeacher,
									 pSendInfo->strBook,
									 &pSendInfo->accountInfo.deviceInfo,
									 szPasswordToken,
									 szDsid,
									 pSendInfo->strProxy,
									 pSendInfo->netType);
					if (!Res)
					{
						status = LOGIN_FAILED;
					}
					
				}
				else
				{
					
					Res = pSearchKeyHotWord(lpNetworkObj,
											pSendInfo->strHotKeyWord,
											szAppSaleId,
											szDsid,
											szPasswordToken,
											szTargetUrl);
					if (!Res)
					{
						Res = pSearchKeyHotWord(lpNetworkObj,
											pSendInfo->strHotKeyWord,
											szAppSaleId,
											szDsid,
											szPasswordToken,
											szTargetUrl);
						if (!Res)
						{
							status = SEARCHFAILED;
						}
					}
					else
					{
						Res = pDownloadApp(lpNetworkObj,szPasswordToken,szDsid,szAppSaleId,szAppPrice,TRUE,pSendInfo->bPaidApp);
						if (!Res)
						{
							Res = pDownloadApp(lpNetworkObj,szPasswordToken,szDsid,szAppSaleId,szAppPrice,TRUE,pSendInfo->bPaidApp);
							if (!Res)
							{
								status = DOWNLOADFAILED;
							}
						}
						else
						{
							Res = pBuyConfirmActiveApp(lpNetworkObj,szPasswordToken,szAppSaleId,szDsid,szTargetUrl);
							if (!Res)
							{
								Res = pBuyConfirmActiveApp(lpNetworkObj,szPasswordToken,szAppSaleId,szDsid,szTargetUrl);
								if (!Res)
								{
									status = ACTIVEAPPFAILED;
								}
							}
							else
							{
								int rate = atoi(szRate);//星级
								Res = pCommentApp(lpNetworkObj,
													szPasswordToken,
													szDsid,
													szAppSaleId,
													rate,
													pSendInfo->strNickName,
													pSendInfo->strTitle,
													pSendInfo->strContext);
								if (!Res)
								{
									status = COMMENTFAILED;
								}
								else
								{
									status = COMMENTSUCCESS;
								}
							}
						}
					}
				}
			}break;
		case SEARCHAPP://登录搜索关键词下载,核心ASO部分
			{
				Res = pSetUAInfo(lpNetworkObj,&pSendInfo->modelInfo);
				Res = pLoginAppStore(lpNetworkObj,
									szAppleId,
									szPwd,
									pSendInfo->strFatherAndMather,
									 pSendInfo->strTeacher,
									 pSendInfo->strBook,
									 &pSendInfo->accountInfo.deviceInfo,
									 szPasswordToken,
									 szDsid,
									 pSendInfo->strProxy,
									 pSendInfo->netType);
				if (!Res)
				{
					Res = pLoginAppStore(lpNetworkObj,
									szAppleId,
									szPwd,
									pSendInfo->strFatherAndMather,
									 pSendInfo->strTeacher,
									 pSendInfo->strBook,
									 &pSendInfo->accountInfo.deviceInfo,
									 szPasswordToken,
									 szDsid,
									 pSendInfo->strProxy,
									 pSendInfo->netType);
					if (!Res)
					{
						status = LOGIN_FAILED;
					}
				}
				else
				{
					Res = pSearchKeyHotWord(lpNetworkObj,
											pSendInfo->strHotKeyWord,
											szAppSaleId,
											szDsid,
											szPasswordToken,
											szTargetUrl);
					if (!Res)
					{
						Res = pSearchKeyHotWord(lpNetworkObj,
											pSendInfo->strHotKeyWord,
											szAppSaleId,
											szDsid,
											szPasswordToken,
											szTargetUrl);
						if (!Res)
						{
							status = SEARCHFAILED;
						}
					}
					else
					{
						Res = pDownloadApp(lpNetworkObj,szPasswordToken,szDsid,szAppSaleId,szAppPrice,TRUE,pSendInfo->bPaidApp);
						if (!Res)
						{
							Res = pDownloadApp(lpNetworkObj,szPasswordToken,szDsid,szAppSaleId,szAppPrice,TRUE,pSendInfo->bPaidApp);
							if (!Res)
							{
								status = DOWNLOADFAILED;
							}
						}
						else
						{
							Res = pBuyConfirmActiveApp(lpNetworkObj,szPasswordToken,szAppSaleId,szDsid,szTargetUrl);
							if (!Res)
							{
								Res = pBuyConfirmActiveApp(lpNetworkObj,szPasswordToken,szAppSaleId,szDsid,szTargetUrl);
								if (!Res)
								{
									status = ACTIVEAPPFAILED;
								}
							}
							else
							{
								status = SEARCHSUCCESS;
							}
						}
					}
				}
			}break;
		default:
			break;
	}

	InterlockedDecrement(&m_ItemCount);

	if (lpNetworkObj)
	{
		pReleaseNetworkObject(lpNetworkObj);
	}

	SetEvent(m_hQueueEvent);

	return status;
}

void CxhASODlg::SetDisplayResultInfo(int status,int nIndex)
{
	LOGINFO logInfo = {0};
	CString strShow;
	int appleIdIndex = 0;
	switch(status)
	{
	case LOGIN_SUCCESS:
		{
			logInfo.nIndex = nIndex;
			appleIdIndex = nIndex + 1;
			m_vecLoginSucessAppleIdIndex.push_back(appleIdIndex);
			wcscpy(logInfo.strItemLog,TEXT("登录成功"));
			logInfo.result = LOGIN_FAILED;
			m_successCounts ++;
		}break;
	case LOGIN_FAILED:
		{
			logInfo.nIndex = nIndex;
			appleIdIndex = nIndex + 1;
			m_vecLoginFailedAppleIdIndex.push_back(appleIdIndex);
			wcscpy(logInfo.strItemLog,TEXT("登录失败"));
			logInfo.result = LOGIN_FAILED;
			m_failedCounts ++;
		}break;
	case DOWNLOADFAILED:
		{
			logInfo.nIndex = nIndex;
			appleIdIndex = nIndex + 1;
			m_vecExecuteFailedAppleIdIndex.push_back(appleIdIndex);
			wcscpy(logInfo.strItemLog,TEXT("下载失败"));
			logInfo.result = DOWNLOADFAILED;
			m_failedCounts ++;
		}break;
	case SEARCHFAILED:
		{
			logInfo.nIndex = nIndex;
			appleIdIndex = nIndex + 1;
			m_vecExecuteFailedAppleIdIndex.push_back(appleIdIndex);
			wcscpy(logInfo.strItemLog,TEXT("搜索失败"));
			logInfo.result = SEARCHFAILED;
			m_failedCounts ++;
		}break;
	case COMMENTFAILED:
		{
			logInfo.nIndex = nIndex;
			appleIdIndex = nIndex + 1;
			m_vecExecuteFailedAppleIdIndex.push_back(appleIdIndex);
			wcscpy(logInfo.strItemLog,TEXT("评论失败"));
			logInfo.result = COMMENTFAILED;
			m_failedCounts ++;
		}
	case DOWNLOADSUCCESS:
		{
			logInfo.nIndex = nIndex;
			appleIdIndex = nIndex + 1;
			wcscpy(logInfo.strItemLog,TEXT("下载完成"));
			logInfo.result = DOWNLOADSUCCESS;
			m_successCounts ++;
		}break;
	case SEARCHSUCCESS:
		{
			logInfo.nIndex = nIndex;
			appleIdIndex = nIndex + 1;
			wcscpy(logInfo.strItemLog,TEXT("搜索完成"));
			logInfo.result = SEARCHSUCCESS;
			m_successCounts ++;
		}break;
	case COMMENTSUCCESS:
		{
			logInfo.nIndex = nIndex;
			appleIdIndex = nIndex + 1;
			wcscpy(logInfo.strItemLog,TEXT("评论完成"));
			logInfo.result = COMMENTSUCCESS;
			m_successCounts ++;
		}break;
	case ACTIVEAPPFAILED:
		{
			logInfo.nIndex = nIndex;
			appleIdIndex = nIndex + 1;
			wcscpy(logInfo.strItemLog,TEXT("激活失败"));
			logInfo.result = ACTIVEAPPFAILED;
			m_failedCounts ++;
		}break;
	case GSA_FAILED:
		{
			logInfo.nIndex = nIndex;
			appleIdIndex = nIndex + 1;
			m_vecExecuteFailedAppleIdIndex.push_back(appleIdIndex);
			wcscpy(logInfo.strItemLog,TEXT("gsa失败"));
			logInfo.result = GSA_FAILED;
			m_failedCounts ++;
		}break;
	default:
		{
			logInfo.nIndex = nIndex;
			appleIdIndex = nIndex + 1;
			m_vecExecuteFailedAppleIdIndex.push_back(appleIdIndex);
			wcscpy(logInfo.strItemLog,TEXT("执行失败"));
			logInfo.result = EXECTUEFAILED;
			m_failedCounts ++;
		}
		break;
	}

	//显示log数据
	m_appleIdLoginAppDownload.SetItemText(nIndex,2,logInfo.strItemLog);

	strShow.Format(TEXT("成功:%d个,失败:%d"),m_successCounts,m_failedCounts);

	m_showResult.SetWindowText(strShow);

	return;

}

void CxhASODlg::unloadFunInterface()
{
	pDeleteCriticalSection();
	pUninitialModule();
}

BOOL CxhASODlg::TrialVersionCheck(void)
{
	return TRUE;
}

void CxhASODlg::OnBnClickedBtnConnect()
{
	// TODO: Add your control notification handler code here
	CString strEntryName;
	CString strUserName;
	CString strUserPassword;

	m_strAdsl_EntryName.GetWindowText(strEntryName);
	m_strAdsl_userName.GetWindowText(strUserName);
	m_strAdsl_userPwd.GetWindowText(strUserPassword);

	CAutoDial::GetInstance()->Connect(strEntryName,strUserName,strUserPassword);
	bIsAdslConnect = TRUE;
}

void CxhASODlg::OnBnClickedBtnReconnect()
{
	// TODO: Add your control notification handler code here
	if (bIsAdslConnect)
	{
		 CAutoDial::GetInstance()->Disconnect();
		 Sleep(1000*3);
       //  CAutoDial::GetInstance()->Connect
	}

}

void CxhASODlg::OnFileLoadvpn()
{
	// TODO: Add your command handler code here
	INT_PTR res = m_vpnIpDlg.DoModal();
}

void CxhASODlg::OnCommnets()
{
	// TODO: Add your command handler code here
	INT_PTR res = m_commentDlg.DoModal();
}

void CxhASODlg::OnVpnConnectCmd()
{
	// TODO: Add your command handler code here
	INT_PTR res = m_vpnLoginDlg.DoModal();
}

void CxhASODlg::OnPppoeConnectCmd()
{
	// TODO: Add your command handler code here
	INT_PTR res = m_dialupLoginDlg.DoModal();
}


void CxhASODlg::OnLoginAppleid()
{
	// TODO: Add your command handler code here
	int processCheck = ((CButton*)GetDlgItem(IDC_RADIO_PROCESS))->GetCheck();
	int threadCheck = ((CButton*)GetDlgItem(IDC_RADIO_THREAD))->GetCheck();
	DWORD dwThreadId = 0;
	HANDLE hThread = NULL;
	if (processCheck == BST_CHECKED)
	{
		hThread = CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)loginAccountThreadProc,this,0,&dwThreadId);
		if (!hThread)
		{
			return;
		}
	}
	else if (threadCheck == BST_CHECKED)
	{
		if (!getFunInterface())
		{
			MessageBox(TEXT("模块加载失败"));
			return;
		}
		hThread = CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)executeLoginAppleIdItem,this,0,&dwThreadId);
	}
	else
	{
		MessageBox(TEXT("请选择执行类型"));
		return;
	}
}

void CxhASODlg::OnDownloadApp()
{
	// TODO: Add your command handler code here
	int processCheck = ((CButton*)GetDlgItem(IDC_RADIO_PROCESS))->GetCheck();
	int threadCheck = ((CButton*)GetDlgItem(IDC_RADIO_THREAD))->GetCheck();
	DWORD dwThreadId = 0;
	HANDLE hThread = CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)normalnetDownloadAppThreadProc,this,0,&dwThreadId);
	if (!hThread)
	{
		return;
	}
}

void CxhASODlg::OnCommentApp()
{
	// TODO: Add your command handler code here
	int processCheck = ((CButton*)GetDlgItem(IDC_RADIO_PROCESS))->GetCheck();
	int threadCheck = ((CButton*)GetDlgItem(IDC_RADIO_THREAD))->GetCheck();
	DWORD dwThreadId = 0;
	HANDLE hThread = CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)normalnetCommentThreadProc,this,0,&dwThreadId);
	if (!hThread)
	{
		return;
	}
}

void CxhASODlg::OnConnectMe()
{
	// TODO: Add your command handler code here
	CDialog* pDlg = new CDialog(IDD_ABOUT_DIALOG);
	pDlg->DoModal();
}

void CxhASODlg::OnSearchKeyword()
{
	// TODO: Add your command handler code here
	int processCheck = ((CButton*)GetDlgItem(IDC_RADIO_PROCESS))->GetCheck();
	int threadCheck = ((CButton*)GetDlgItem(IDC_RADIO_THREAD))->GetCheck();
	DWORD dwThreadId = 0;
	HANDLE hThread = CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)normalnetSearchKeywordThreadProc,this,0,&dwThreadId);
	if (!hThread)
	{
		return;
	}
}

void CxhASODlg::OnFileLoadappleid()
{
	// TODO: Add your command handler code here
	INT_PTR res = m_appleIdDlg.DoModal();
}



LRESULT CxhASODlg::OnLoadAppleIdlistMsg(WPARAM wParam, LPARAM lParam)
{
	if (!m_accountInfoList.empty())
	{
		m_accountInfoList.clear();
	}

	if (m_appleIdLoginAppDownload.GetItemCount() > 0)
	{
		m_appleIdLoginAppDownload.DeleteAllItems();
	}

	m_failedCounts = 0;
	m_successCounts = 0;
	m_showResult.SetWindowText(TEXT(""));

	//加载appleId线程
	DWORD dwThreadId1;
	HANDLE hThread1 = CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)showAppleIdListThreadProc,this,0,&dwThreadId1);
	if (!hThread1)
	{
		return -1;
	}
	
	return 0;
}

LRESULT CxhASODlg::OnLoadCommentMsg(WPARAM wParam, LPARAM lParam)
{
	DWORD dwThreadId;
	HANDLE hThread = CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)commentShowThreadProc,this,0,&dwThreadId);
	if (!hThread)
	{
		return -1;
	}
	return 0;
}

LRESULT CxhASODlg::OnLoadvpnIPMsg(WPARAM wParam, LPARAM lParam)
{
	DWORD dwThreadId;
	HANDLE hThread = CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)vpnIpThreadProc,this,0,&dwThreadId);
	if (!hThread)
	{
		return -1;
	}

	WaitForSingleObject(hThread,INFINITE);

	WritePrivateProfileString(TEXT("VPNServer"),TEXT("IP"),m_ServerVPNIP,m_configfile);

	m_showResult.SetWindowText(TEXT("vpn IP已加载"));

	return 0;
}

LRESULT CxhASODlg::OnLoginVpnMsg(WPARAM wParam, LPARAM lParam)
{
	m_bLoginVpn = (BOOL)wParam;
	DWORD dwThreadId;
	HANDLE hThread = CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)loginVpnThreadProc,this,0,&dwThreadId);
	if (!hThread)
	{
		return -1;
	}

	return 0;
}

LRESULT CxhASODlg::OnDialupLoginMsg(WPARAM wParam, LPARAM lParam)
{
	m_bLoginAdsl = (BOOL)wParam;
	DWORD dwThreadId;
	HANDLE hThread = CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)dialupLoginThreadProc,this,0,&dwThreadId);
	if (!hThread)
	{
		return -1;
	}
	return 0;
}

LRESULT CxhASODlg::OnHandleExceptionTaskProcess(WPARAM wParam, LPARAM lParam)
{
	int nIndex = (int)wParam;
	CString strShow;
	//界面上显示信息
	m_appleIdLoginAppDownload.SetItemText(nIndex,2,TEXT("进程异常"));

	EnterCriticalSection(&g_cs);
	m_failedCounts ++;
	LeaveCriticalSection(&g_cs);

	strShow.Format(TEXT("成功:%d,失败:%d"),m_successCounts,m_failedCounts);

	m_showResult.SetWindowText(strShow);

	return 0;
}

UINT WINAPI CxhASODlg::loginVpnThreadProc(LPVOID lpPtr)
{
	CxhASODlg* pThis = (CxhASODlg*)lpPtr;
	if (!pThis)
	{
		return -1;
	}

	//获取vpn的IP,账号和密码
	CString strIpAddress = pThis->m_vpnLoginDlg.m_strIPAddress;
	CString strLoginName = pThis->m_vpnLoginDlg.m_strVpnAccount;
	CString strPwd = pThis->m_vpnLoginDlg.m_strPassword;

	//创建随机的vpn连接名称
	TCHAR szVpnContext[MAX_PATH] = {0};
	LARGE_INTEGER timeRand; 
	QueryPerformanceCounter(&timeRand);
	swprintf(szVpnContext,sizeof(szVpnContext),TEXT("vpn%d"),timeRand.LowPart);
	pThis->m_vpnNameContext = szVpnContext;

	//创建连接
	BOOL bRet = CreateConnection(VPN_CONNECTION,
									strLoginName.GetBuffer(),
									strPwd.GetBuffer(),
									2,
									strIpAddress.GetBuffer(),
									TRUE,
									TRUE);

	if (!bRet)
	{
		return -1;
	}

	//开始拨号
	while( !connectDialVPN(pThis->m_hRasConn,VPN_CONNECTION,strLoginName.GetBuffer(),strPwd.GetBuffer(),strIpAddress.GetBuffer()) )
	{
		::Sleep(100);
	}
	pThis->m_netConnectStatus.SetWindowText(TEXT("VPN已连接"));

	pThis->m_bVpnConnect = TRUE;

	pThis->SetVPNIPDisplay();

	::Sleep(100);

	return 0;
}


UINT WINAPI CxhASODlg::dialupLoginThreadProc(LPVOID lpPtr)
{
	CxhASODlg* pThis = (CxhASODlg*)lpPtr;
	if (!pThis)
	{
		return -1;
	}
	
	//获取宽带账号和密码
	if (pThis->m_strSaveDialupAccount.IsEmpty())
	{
		pThis->m_strSaveDialupAccount = pThis->m_dialupLoginDlg.m_strDialupAccount;
	}
	if (pThis->m_strSaveDialupPassword.IsEmpty())
	{
		pThis->m_strSaveDialupPassword = pThis->m_dialupLoginDlg.m_strDialupPassword;
	}
	
	CAutoDial::GetInstance()->Connect(ADSLCONNECT,pThis->m_strSaveDialupAccount,pThis->m_strSaveDialupPassword);

	pThis->m_netConnectStatus.SetWindowText(TEXT("宽带已连接"));

	pThis->m_bIsAdslConnect = TRUE;

	return 0;
}

UINT WINAPI CxhASODlg::loginVpnUseRasDialCmdThreadProc(LPVOID lpPtr)
{
	CxhASODlg* pThis = (CxhASODlg*)lpPtr;
	if (!pThis)
	{
		return -1;
	}

	//获取vpn账号和密码
	CString strIpAddress = pThis->m_vpnLoginDlg.m_strIPAddress;
	CString strLoginName = pThis->m_vpnLoginDlg.m_strVpnAccount;
	CString strPwd = pThis->m_vpnLoginDlg.m_strPassword;
	CString strIPAddress;

	//开始拨号
	while( !connectDialVPN(pThis->m_hRasConn,VPN_CONNECTION,strLoginName.GetBuffer(),strPwd.GetBuffer(),strIpAddress.GetBuffer()) )
	{
		::Sleep(100);
	}

	pThis->m_netConnectStatus.SetWindowText(TEXT("VPN已连接"));

	pThis->m_bVpnConnect = TRUE;

	return 0;
}

int CxhASODlg::ConvertUtf8ToGBK(char* pChangeStr,int nLength)
{
	int nLen = nLength*2;
 
    LPWSTR lpwsz = new WCHAR[nLen+2]; 
    if( lpwsz == NULL)
    {
        return 0;
    }
    MultiByteToWideChar( CP_UTF8, 0, pChangeStr, -1, lpwsz, nLen );//转换的结果是UCS2格式
    int nLen1 = WideCharToMultiByte( CP_ACP, 0, lpwsz, -1, pChangeStr, nLen, NULL, NULL );//转换完毕
 
    delete [] lpwsz; 
    *(pChangeStr + nLen1) = '\0';
 
    return nLen1;
}

void CxhASODlg::Convert(const char* strIn,char* strOut, int sourceCodepage, int targetCodepage)
{
   int len = strlen(strIn);
   int unicodeLen = MultiByteToWideChar(sourceCodepage,0,strIn,-1,NULL,0);

   wchar_t* pUnicode;
   pUnicode = new wchar_t[unicodeLen+1];

   memset(pUnicode,0,(unicodeLen+1)*sizeof(wchar_t));

   MultiByteToWideChar(sourceCodepage,0,strIn,-1,(LPWSTR)pUnicode,unicodeLen);

   BYTE * pTargetData = NULL;
   int targetLen = WideCharToMultiByte(targetCodepage,0,(LPWSTR)pUnicode,-1,(char *)pTargetData,0,NULL,NULL);

   pTargetData = new BYTE[targetLen+1];
   memset(pTargetData,0,targetLen+1);

   WideCharToMultiByte(targetCodepage,0,(LPWSTR)pUnicode,-1,(char *)pTargetData,targetLen,NULL,NULL);

   strcpy(strOut,(char*)pTargetData);
   if (pUnicode)
   {
	   delete []pUnicode;
   }
   if (pTargetData)
   {
	   delete []pTargetData;
   }
}

BOOL CxhASODlg::InternetIPDetect(CString strDialupAccount,CString strPassword,char* lpIPAddr,int networkType)
{
	BOOL	bRet = FALSE;

	bRet = xhNetworkCheck(lpIPAddr);
	if (!bRet)
	{
		//检测联网失败，则进行判断
		switch(networkType)
		{
		case LOCAL_NETWORK:
			{
				//本地网络连接
			}break;
		case VPN_NETWORK:
			{
				//vpn连接
			}break;
		case DIALUP_NETWORK:
			{
				//拨号连接
				CAutoDial::GetInstance()->Connect(ADSLCONNECT,strDialupAccount,strPassword);
				m_netConnectStatus.SetWindowText(TEXT("宽带已连接"));
				m_bIsAdslConnect = TRUE;
				bRet = TRUE;
			}break;
		case PROXY_NETWORK:
			{
				//代理连接
			}break;
		default:
			{
				//第一次客户端本地连接networkType为0，需要判断本地连接类型
			}
			break;
		}
	}
	return bRet;
}

UINT WINAPI CxhASODlg::heartBeatThreadProc(LPVOID lpPtr)
{
	CxhASODlg* pThis = (CxhASODlg*)lpPtr;
	_ASSERTE( pThis != NULL );

	BOOL bRet = FALSE;
	int status = 0;
	char	szIPAddr[64] = {0};
	char szServerData[MAX_BODY] = {0};
	char szReportData[MAX_BODY] = {0};
	char szClientId[MAX_PATH] = {0};
	TCHAR strClientId[MAX_PATH] = {0};
	TCHAR strPath[MAX_PATH] = {0};
	TCHAR strAccount[MAX_PATH] = {0};
	TCHAR strPwd[MAX_PATH] = {0};
	
	TCHAR swNickname[MAX_PATH] ={0};
	TCHAR swTitle[MAX_PATH] = {0};
	TCHAR swContext[MAX_BODY] = {0};
	TCHAR swKeyword[MAX_PATH] = {0};

	CString strTargetUrl;
	CString strAppleId;
	CString strPassword;
	CString strRate;
	CString strNickName;
	CString strTitle;
	CString strContext;
	CString strKeyword;
	CString strSaleId;
	CString strProxy;

	std::string appleId;
	std::string password;
	std::string appId;
	std::string keyword;
	std::string utf8Keyword;
	std::string proxy;
	std::string nickname;
	std::string utf8Nickname;
	std::string title;
	std::string utf8Title;
	std::string content;
	std::string utf8Content;
	std::string itunesUrl;
	int taskType = 0;
	int networkType = 0;
	int orderId = 0;
	int reportLen = 0;
	int ResDialupIndex = 0;
	SENDINFO sendInfo = {0};

	CString strDevicePath;
	CString strDialupPath;
	DWORD dwResult = GetModuleFileName(NULL,strPath,MAX_PATH);
	CString filePath(strPath);
	int Pos = filePath.ReverseFind(TEXT('\\'));
	filePath.Truncate(Pos);
	
	strDevicePath = filePath;
	strDialupPath = filePath;
	strDevicePath += TEXT("\\xhDevice.ini");
	strDialupPath += TEXT("\\xhdialup.ini");

	GetPrivateProfileString(TEXT("machineNumber"),TEXT("clientId"),0,strClientId,MAX_PATH,strDevicePath);
	GetPrivateProfileString(TEXT("Dialup"),TEXT("account"),0,strAccount,MAX_PATH,strDialupPath);
	GetPrivateProfileString(TEXT("Dialup"),TEXT("password"),0,strPwd,MAX_PATH,strDialupPath);

	::WideCharToMultiByte(CP_ACP,0,strClientId,wcslen(strClientId),szClientId,MAX_PATH,NULL,NULL);
	szClientId[strlen(szClientId)] = '\0';

	while (pThis->m_bHeartBeatRunning)
	{
		//检测网络是否连通
		if ( !pThis->InternetIPDetect(strAccount,strPwd,szIPAddr,networkType) )
		{
			break;
		}
		
		//请求数据包
		if ( !xhRequestDataFromServer(szServerData,strClientId) )
		{
			Sleep(5000);
			continue;
		}

		//解析json数据
		Json::Reader reader;  
		Json::Value root; 

		do
		{
			////字符串转为JSON格式存于root
			if ( !reader.parse(szServerData,root) )
			{		
				Sleep(5000);
				break;	
			}
			
			int result = root["result"].asInt();
			if (result == NO_TASK)
			{
				//无任务心跳
				Sleep(5000);
				break;
			}
			else if (result == OK_TASK)
			{
				//有任务
				Json::Value dataValue = root["data"];
				appleId = dataValue["appleid"].asString();
				password = dataValue["password"].asString();
			    appId = dataValue["appid"].asString();

				keyword = dataValue["keyword"].asString();
				utf8Keyword = keyword;

				proxy = dataValue["proxy"].asString();

				nickname = dataValue["nickname"].asString();
				utf8Nickname = nickname;

				title = dataValue["title"].asString();
				utf8Title = title;

				content = dataValue["content"].asString();
				utf8Content = content;

				itunesUrl = dataValue["iTunesURL"].asString();
				taskType = dataValue["tasktype"].asInt();
				networkType = dataValue["networktype"].asInt();
				orderId = dataValue["oid"].asInt();

				strTargetUrl = itunesUrl.c_str();
				strAppleId = appleId.c_str();
				strPassword = password.c_str();
				strRate = TEXT("5");

				pThis->ConvertUtf8ToGBK((char*)nickname.c_str(),nickname.length());
				MultiByteToWideChar(CP_ACP,0,nickname.c_str(),nickname.length(),swNickname,MAX_PATH);
				strNickName = swNickname;

				
				pThis->ConvertUtf8ToGBK((char*)title.c_str(),title.length());
				MultiByteToWideChar(CP_ACP,0,title.c_str(),title.length(),swTitle,MAX_PATH);
				strTitle = swTitle;

				
				pThis->ConvertUtf8ToGBK((char*)content.c_str(),content.length());
				MultiByteToWideChar(CP_ACP,0,content.c_str(),content.length(),swContext,MAX_BODY);
				strContext = swContext;

				pThis->ConvertUtf8ToGBK((char*)keyword.c_str(),keyword.length());
				MultiByteToWideChar(CP_ACP,0,keyword.c_str(),keyword.length(),swKeyword,MAX_PATH);
				strKeyword = swKeyword;

				strSaleId = appId.c_str();
				strProxy = proxy.c_str();


				//列表显示
				if (pThis->m_appleIdLoginAppDownload.GetItemCount() > 0)
				{
					pThis->m_appleIdLoginAppDownload.DeleteAllItems();
				}
				pThis->m_hotkeyword.SetWindowText(strKeyword);
				pThis->m_appId.SetWindowText(strSaleId);
				pThis->m_editTargetUrl.SetWindowText(strTargetUrl);
				int i = 0;
				CString strIndex;
				strIndex.Format(TEXT("%d"),i+1);
				pThis->m_appleIdLoginAppDownload.InsertItem(i,strIndex);
				pThis->m_appleIdLoginAppDownload.SetItemText(i,1,strAppleId);
				pThis->m_appleIdLoginAppDownload.SetItemText(i,2,LOGIN_READY);
				pThis->m_appleIdLoginAppDownload.SetItemText(i,3,strRate);
				pThis->m_appleIdLoginAppDownload.SetItemText(i,4,strNickName);
				pThis->m_appleIdLoginAppDownload.SetItemText(i,5,strTitle);
				pThis->m_appleIdLoginAppDownload.SetItemText(i,6,strContext);


				CString strTmp = _T("https://itunes.apple.com");
				strTargetUrl = strTargetUrl.Right(strTargetUrl.GetLength()-strTmp.GetLength());

				//判断联网类型
				switch(networkType)
				{
				case LOCAL_NETWORK:
					{
					}break;
				case VPN_NETWORK:
					{
					}break;
				case DIALUP_NETWORK:
					{
						if (pThis->m_bIsAdslConnect)
						{
							//断开重新连接
							CAutoDial::GetInstance()->Disconnect();
							pThis->m_netConnectStatus.SetWindowText(TEXT("宽带已断开"));
							pThis->m_bIsAdslConnect = FALSE;
							Sleep(1000*3);
						}
						
						CAutoDial::GetInstance()->Connect(ADSLCONNECT,strAccount,strPwd);
						pThis->m_netConnectStatus.SetWindowText(TEXT("宽带已连接"));
						pThis->m_bIsAdslConnect = TRUE;

					}break;
				case PROXY_NETWORK:
					{
					}break;
				default:
					break;
				}

				//清除cookie
				pThis->deleteCookieFile();
				//修改注册表硬件信息
				pThis->ModifyHardwareInfo(FALSE,FALSE);

				//创建事件名
				CString strEventName;
				LARGE_INTEGER timeRand; 
				QueryPerformanceCounter(&timeRand);
				strEventName.Format(TEXT("EXEPROC%d"),timeRand.LowPart);

				wcscpy(sendInfo.strTargetUrl,strTargetUrl.GetBuffer());
				wcscpy(sendInfo.strEventName,strEventName.GetBuffer());
				//wcscpy(sendInfo.strAppleId,strAppleId.GetBuffer());
				//wcscpy(sendInfo.strPwd,strPassword.GetBuffer());
				wcscpy(sendInfo.strRate,strRate.GetBuffer());
				wcscpy(sendInfo.strNickName,strNickName.GetBuffer());
				wcscpy(sendInfo.strTitle,strNickName.GetBuffer());
				wcscpy(sendInfo.strContext,strContext.GetBuffer());
				wcscpy(sendInfo.strHotKeyWord,strKeyword.GetBuffer());
				wcscpy(sendInfo.strSaleId,strSaleId.GetBuffer());
				wcscpy(sendInfo.strProxy,strProxy.GetBuffer());
				sendInfo.netType = networkType;
				sendInfo.type = taskType;
				sendInfo.bPaidApp = pThis->m_bBuyType;

				//根据任务类型做任务
				status = pThis->ExectueAProcess(&sendInfo,0);
				if (status == 0)
				{
					pThis->ExectueAProcess(&sendInfo,0);
				}

			}

			//做完任务，上报结果
			ResDialupIndex = 0;
			do
			{
				ResDialupIndex ++;
				//汇报包时再次检测网络是否连通
				if (pThis->InternetIPDetect(strAccount,strPwd,szIPAddr,networkType))
				{
					sprintf(szReportData,"appid=%s&oid=%d&itunesUrl=%s&appleid=%s&password=%s&keyword=%s&nickname=%s&title=%s&content=%s&tasktype=%d&proxy=%s&status=%d&resbody=%s&ip=%s&clientid=%s",
								appId.c_str(),
								orderId,
								itunesUrl.c_str(),
								appleId.c_str(),
								password.c_str(),
								utf8Keyword.c_str(),
								utf8Nickname.c_str(),
								utf8Title.c_str(),
								utf8Content.c_str(),
								taskType,
								proxy.c_str(),
								status,
								NULL,
								szIPAddr,
								szClientId);

					reportLen = strlen(szReportData);
					szReportData[reportLen] = '\0';

					bRet = xhReportDataToServer(szReportData,szServerData);
				}

				if (ResDialupIndex > 5)
				{
					break;
				}

			}while (!bRet);
				
		}while ( pThis->m_bHeartBeatRunning);

		
	}//end while

	return 0;
}

UINT WINAPI CxhASODlg::monitorPorcessProc(LPVOID lpPtr)
{
	CxhASODlg* pThis = (CxhASODlg*)lpPtr;
	pThis->m_bMonitorRunning = TRUE;
	while (pThis->m_bMonitorRunning)
	{
		//等待事件，4分钟内如果没有执行完40个任务，就进入任务进程检测
		WaitForSingleObject(pThis->m_hMonitorEvent,240*1000);

		EnterCriticalSection(&g_cs);

		pThis->monitorTaskProcess();

		LeaveCriticalSection(&g_cs);

	}//end while
	return 0;
}

void CxhASODlg::monitorTaskProcess()
{
	HANDLE		hProcess = NULL;
	FILETIME	createTime = {0};
	FILETIME	exitTime = {0};
	FILETIME	kernelTime = {0};
	FILETIME	userTime = {0};
	SYSTEMTIME  procCreatTime = {0};
	SYSTEMTIME  localTime = {0};
	time_t		createProcessTimeStamp = 0;
	time_t		currentTimeStamp = 0;
	DWORD		timeSpan = 0;
	LOGINFO		logInfo = {0};
	int executeCounts = GetDlgItemInt(IDC_EDIT_EXECUTE_COUNTS);
	int size = m_monTaskList.size();
	if (size == 0)
	{
		return;
	}
	//检查任务进程是否超时，如果超过规定的时间，要结束任务进程
	for (MONTASKLIST::iterator ptr = m_monTaskList.begin(); ptr != m_monTaskList.end();)
	{
		if (ptr->finishTag == 1)
		{
			//移除已经正常完成的进程
			m_monTaskList.erase(ptr++);
			continue;
		}
		//获取程序创建时间
		GetProcessTimes(ptr->processInfo.hProcess,&createTime,&exitTime,&kernelTime,&userTime);

		//将FILETIME转换成SYSTEMTIME
		FileTimeToSystemTime(&createTime,&procCreatTime);

		//获取系统时间
		GetSystemTime(&localTime);

		//SYSTEMTIME转time_t时间戳
		struct tm gm1 = {procCreatTime.wSecond,
						procCreatTime.wMinute,
						procCreatTime.wHour,
						procCreatTime.wDay,
						procCreatTime.wMonth-1,
						procCreatTime.wYear-1900,
						procCreatTime.wDayOfWeek,
						0,
						0};
		createProcessTimeStamp = mktime(&gm1);

		struct tm gm2 = {localTime.wSecond,
						localTime.wMinute,
						localTime.wHour,
						localTime.wDay,
						localTime.wMonth-1,
						localTime.wYear-1900,
						localTime.wDayOfWeek,
						0,
						0};
		currentTimeStamp = mktime(&gm2); 

		//计算时间间隔，也就是任务进程运行时间
		timeSpan = currentTimeStamp - createProcessTimeStamp;
		if (timeSpan >= TASK_PROCESS_MAX_RUN_TIME)
		{
			HWND hWnd = getWindowHandleByProcessId(ptr->processInfo.dwProcessId);
			if (hWnd)
			{
				//发送超时关闭消息
				DWORD dwResult = 0;
				SendMessageTimeoutW(hWnd, WM_SYSCOMMAND, SC_CLOSE, 0, SMTO_BLOCK|SMTO_ABORTIFHUNG, 250, &dwResult);

			}

			hProcess = OpenProcess(PROCESS_ALL_ACCESS,TRUE,ptr->processInfo.dwProcessId);
			if (hProcess)
			{
				//结束进程，并通知本次任务失败的消息给窗口
				TerminateProcess(hProcess,0);
			}

			//发送消息，通知应用程序处理异常任务进程
			::PostMessage(this->GetSafeHwnd(),WM_EXCEPTION_TASK_PROCESS_MSG,(WPARAM)(ptr->nIndex),NULL);

			//移除异常进程
			m_monTaskList.erase(ptr++);

			//保证原子操作执行完一个后减一
			if (m_ItemCount > 0)
			{
				InterlockedDecrement(&m_ItemCount);
			}
			
			if (hProcess)
			{
				CloseHandle(hProcess);
			}
		

		}
		else
		{
			ptr ++;
		}

	}//end for

	return;
}

HWND CxhASODlg::getWindowHandleByProcessId(DWORD dwProcessId)
{
	HWND hWnd = ::GetTopWindow(NULL);
	while (hWnd)
	{
		DWORD pid = 0;
		DWORD dwThreadId = GetWindowThreadProcessId(hWnd,&pid);
		if (dwThreadId != 0)
		{
			if (pid == dwProcessId)
			{
				return hWnd;
			}
		}
		hWnd = ::GetNextWindow(hWnd,GW_HWNDNEXT);
	}

	return NULL;
}

UINT WINAPI CxhASODlg::showAppleIdListThreadProc(LPVOID lpPtr)
{
	CxhASODlg* pThis = (CxhASODlg*)lpPtr;

	CString strPath = pThis->m_appleIdDlg.m_strAppleIdFilePath;
	if (strPath.IsEmpty())
	{
		return 0;
	}

	pThis->m_showResult.SetWindowText(TEXT("正在显示账号信息……"));
	
	if (strPath.Find(TEXT(".csv")) > 0)
	{
		pThis->readCSVFormatData(strPath);
	}
	else
	{
		pThis->readTxtFormatData(strPath);
	}

	pThis->m_showResult.SetWindowText(TEXT("全部显示完毕"));
	
	return 0;
}

void CxhASODlg::readTxtFormatData(CString strPath)
{
	DWORD		dwThreadId = 0;
	int			nIndex = 0;
	CString		strLineData;
	HANDLE		hThread = NULL;
	CStdioFile	file;

	if (!file.Open(strPath.GetBuffer(),CFile::modeRead))
	{
		return;
	}
	try
	{
		while (file.ReadString(strLineData) != FALSE)
		{
			//解析行数据
			++nIndex;
			if (m_bDefaultFree)
			{
				//免费账号格式
				parseLineData(strLineData,TEXT("----"),nIndex);
			}
			else
			{
				//付费账号格式
				ReadStringToUnicode(strLineData);
				parsePaidLineData(strLineData,TEXT("----"),nIndex);
			}
		}

	}
	catch(int err)
	{
		file.Close();
		throw err;
	}
	
	m_nIndex = nIndex;

	file.Close();

	//创建socket线程
	hThread = ::CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)socketThreadProc,this,0,&dwThreadId);
	if (!hThread)
	{
		return;
	}
	return;
}
void CxhASODlg::readCSVFormatData(CString strPath)
{
	DWORD		dwThreadId = 0;
	int			nIndex = 0;
	CString		strLineData;
	HANDLE		hThread = NULL;
	CStdioFile	file;

	if (!file.Open(strPath.GetBuffer(),CFile::modeRead))
	{
		return;
	}
	try
	{
		while (file.ReadString(strLineData) != FALSE)
		{
			//解析行数据
			++nIndex;
			if (m_bDefaultFree)
			{
				//免费账号格式
				parseLineData(strLineData,TEXT(","),nIndex);
			}
			else
			{
				//付费账号格式
				ReadStringToUnicode(strLineData);
				parsePaidLineData(strLineData,TEXT(","),nIndex);
			}
		}

	}
	catch(int err)
	{
		file.Close();
		throw err;
	}
	
	m_nIndex = nIndex;

	file.Close();

	//创建socket线程
	/*hThread = ::CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)socketThreadProc,this,0,&dwThreadId);
	if (!hThread)
	{
		return;
	}*/
	return;
}

void CxhASODlg::parseLineData(CString pLineData,CString strFormat,int nIndex)
{
	ACCOUNTINFO tmpAccountInfo = {0};
	CString strTmp;
	CString strName;
	CString strPwd;

	CString strUdid;
	CString strMEID;
	CString strIMEI;
	CString strSerialNumber;

	int  pos = pLineData.Find(strFormat);
	strName = pLineData.Left(pos);

	strTmp = pLineData.Right(pLineData.GetLength()-pos-wcslen(strFormat));
	pos = strTmp.Find(strFormat);
	if (pos < 0)
	{
		strPwd = strTmp;
	}
	else
	{
		strPwd = strTmp.Left(pos);
	}

	strTmp = strTmp.Right(strTmp.GetLength()-pos-wcslen(strFormat));
	pos = strTmp.Find(strFormat);
	if (pos < 0)
	{
		strUdid = strTmp;
	}
	else
	{
		strUdid = strTmp.Left(pos);
	}

	strTmp = strTmp.Right(strTmp.GetLength()-pos-wcslen(strFormat));
	pos = strTmp.Find(strFormat);
	if (pos < 0)
	{
		strSerialNumber = strTmp;
	}
	else
	{
		strSerialNumber = strTmp.Left(pos);
	}

	strTmp = strTmp.Right(strTmp.GetLength()-pos-wcslen(strFormat));
	pos = strTmp.Find(strFormat);
	if (pos < 0)
	{
		strMEID = strTmp;
	}
	else
	{
		strMEID = strTmp.Left(pos);
	}

	strTmp = strTmp.Right(strTmp.GetLength()-pos-wcslen(strFormat));
	pos = strTmp.Find(strFormat);
	if (pos < 0)
	{
		strIMEI = strTmp;
	}
	else
	{
		strIMEI = strTmp.Left(pos);
	}
	

	tmpAccountInfo.nIndex = nIndex;
	wcscpy(tmpAccountInfo.strAppleId,strName.GetBuffer());
	wcscpy(tmpAccountInfo.strPassword,strPwd.GetBuffer());
	wcscpy(tmpAccountInfo.deviceInfo.strUdid,strUdid.GetBuffer());
	wcscpy(tmpAccountInfo.deviceInfo.strMEID,strMEID.GetBuffer());
	wcscpy(tmpAccountInfo.deviceInfo.strIMEI,strIMEI.GetBuffer());
	wcscpy(tmpAccountInfo.deviceInfo.strSerialNumber,strSerialNumber.GetBuffer());

	//保存进链表
	m_accountInfoList.push_back(tmpAccountInfo);

	//列表显示
	int i = nIndex - 1;
	CString strIndex;
	strIndex.Format(TEXT("%d"),nIndex);
	m_appleIdLoginAppDownload.InsertItem(i,strIndex);
	m_appleIdLoginAppDownload.SetItemText(i,1,strName);
	m_appleIdLoginAppDownload.SetItemText(i,2,LOGIN_READY);
	m_appleIdLoginAppDownload.SetItemText(i,3,TEXT("5"));
	return;

}

void CxhASODlg::parsePaidLineData(CString pLineData,CString strFormat,int nIndex)
{
	ACCOUNTINFO tmpAccountInfo = {0};
	CString strTmp;
	CString strName;
	CString strPwd;

	CString strUdid;
	CString strMEID;
	CString strIMEI;
	CString strSerialNumber;

	CString strFatherAndMather;
	CString strTeacher;
	CString strBook;

	int  pos = pLineData.Find(strFormat);
	strName = pLineData.Left(pos);

	strTmp = pLineData.Right(pLineData.GetLength()-pos-wcslen(strFormat));
	pos = strTmp.Find(strFormat);
	strPwd = strTmp.Left(pos);
	
	strTmp = strTmp.Right(strTmp.GetLength()-pos-wcslen(strFormat));
	pos = strTmp.Find(strFormat);
	strUdid = strTmp.Left(pos);

	strTmp = strTmp.Right(strTmp.GetLength()-pos-wcslen(strFormat));
	pos = strTmp.Find(strFormat);
	if (pos < 0)
	{
		strSerialNumber = strTmp;
	}
	else
	{
		strSerialNumber = strTmp.Left(pos);
	}

	strTmp = strTmp.Right(strTmp.GetLength()-pos-wcslen(strFormat));
	pos = strTmp.Find(strFormat);
	strMEID = strTmp.Left(pos);

	strTmp = strTmp.Right(strTmp.GetLength()-pos-wcslen(strFormat));
	pos = strTmp.Find(strFormat);
	if (pos < 0)
	{
		strIMEI = strTmp;
	}
	else
	{
		strIMEI = strTmp.Left(pos);
	}

	//解析安全问题
	strTmp = strTmp.Right(strTmp.GetLength()-pos-wcslen(strFormat));
	pos = strTmp.Find(strFormat);
	strFatherAndMather = strTmp.Left(pos);

	strTmp = strTmp.Right(strTmp.GetLength()-pos-wcslen(strFormat));
	pos = strTmp.Find(strFormat);
	strTeacher = strTmp.Left(pos);

	strTmp = strTmp.Right(strTmp.GetLength()-pos-wcslen(strFormat));
	strBook = strTmp;

	tmpAccountInfo.nIndex = nIndex;
	wcscpy(tmpAccountInfo.strAppleId,strName.GetBuffer());
	wcscpy(tmpAccountInfo.strPassword,strPwd.GetBuffer());
	wcscpy(tmpAccountInfo.deviceInfo.strUdid,strUdid.GetBuffer());
	wcscpy(tmpAccountInfo.deviceInfo.strMEID,strMEID.GetBuffer());
	wcscpy(tmpAccountInfo.deviceInfo.strIMEI,strIMEI.GetBuffer());
	wcscpy(tmpAccountInfo.deviceInfo.strSerialNumber,strSerialNumber.GetBuffer());

	//保存进链表
	m_accountInfoList.push_back(tmpAccountInfo);

	//列表显示
	int i = nIndex - 1;
	CString strIndex;
	strIndex.Format(TEXT("%d"),nIndex);
	m_appleIdLoginAppDownload.InsertItem(i,strIndex);
	m_appleIdLoginAppDownload.SetItemText(i,1,strName);
	m_appleIdLoginAppDownload.SetItemText(i,2,LOGIN_READY);
	m_appleIdLoginAppDownload.SetItemText(i,3,strFatherAndMather);
	m_appleIdLoginAppDownload.SetItemText(i,4,strTeacher);
	m_appleIdLoginAppDownload.SetItemText(i,5,strBook);

	return;
}

void CxhASODlg::FilterAppleId(void)
{
	CStdioFile loginSuccessFile;
	CStdioFile loginFailedFile;
	CStdioFile executeFailedFile;
	CStdioFile successFile;
	CStdioFile accountStopFile;
	CStdioFile accountLockedFile;
	CStdioFile passwordErrorFile;
	CStdioFile accountFaultFile;

	TCHAR strPath[MAX_PATH] = {0};
	TCHAR strWriteData[MAX_PATH] = {0};

	CTime tDateTime= CTime::GetCurrentTime();
	CString strDate =tDateTime.Format(TEXT("%a%d%b%y%H%M%S"));
	CString fileName = strDate + TEXT(".csv");

	DWORD dwResult = GetModuleFileName(NULL,strPath,MAX_PATH);
	CString filePath(strPath);
	int Pos = filePath.ReverseFind(_T('\\'));
	filePath.Truncate(Pos);
	filePath += TEXT("\\");

	CString loginSuccessPath = filePath;
	CString loginFailedPath = filePath;
	CString executeFailedPath = filePath;
	CString successPath = filePath;
	CString accountStopPath = filePath;
	CString accountLockedPath = filePath;
	CString passwordErrorPath = filePath;
	CString accountFaultPath = filePath;
	
	int loginSuccessCounts = m_vecLoginSucessAppleIdIndex.size();
	int executeFaliedCounts = m_vecExecuteFailedAppleIdIndex.size();
	int loginFailedCounts = m_vecLoginFailedAppleIdIndex.size();
	int successCounts = m_vecSuccessAppleIdIndex.size();
	int accountStopCounts = m_vecAccountStopAppleIdIndex.size();
	int accountLockedCounts = m_vecAccountLockedAppleIdIndex.size();
	int passwordErrorCounts = m_vecPasswordErrorAppleIdIndex.size();
	int accountFaultCounts = m_vecAccountFaultAppleIdIndex.size();

	if (loginSuccessCounts > 0)
	{
		loginSuccessPath += TEXT("LoginSuccessLog\\loginSuccessAppleId_");
		loginSuccessPath += fileName;
		if (!loginSuccessFile.Open(loginSuccessPath.GetBuffer(),CFile::modeCreate|CFile::modeWrite))
		{
			MessageBox(TEXT("创建文件失败:1"));
			return;
		}
		for (AccountInfo::iterator ptr = m_accountInfoList.begin(); ptr!= m_accountInfoList.end(); ptr++)
		{
			for (int i=0; i<loginSuccessCounts; i++)
			{
				if ( ptr->nIndex == m_vecLoginSucessAppleIdIndex.at(i) )
				{
					//把检测登录成功的账号信息记录到新的文件中
					memset(strWriteData,0,sizeof(TCHAR)*MAX_PATH);
					wcscpy(strWriteData,ptr->strAppleId);
					wcscat(strWriteData,TEXT(","));
					wcscat(strWriteData,ptr->strPassword);
					if (ptr->deviceInfo.strUdid[0] != TEXT('\0'))
					{
						wcscat(strWriteData,TEXT(","));
						wcscat(strWriteData,ptr->deviceInfo.strUdid);
					}
					if (ptr->deviceInfo.strSerialNumber[0] != TEXT('\0'))
					{
						wcscat(strWriteData,TEXT(","));
						wcscat(strWriteData,ptr->deviceInfo.strSerialNumber);
					}
					if (ptr->deviceInfo.strMEID[0] != TEXT('\0'))
					{
						wcscat(strWriteData,TEXT(","));
						wcscat(strWriteData,ptr->deviceInfo.strMEID);
					}
					if (ptr->deviceInfo.strIMEI[0] != TEXT('\0'))
					{
						wcscat(strWriteData,TEXT(","));
						wcscat(strWriteData,ptr->deviceInfo.strIMEI);
					}
					wcscat(strWriteData,TEXT("\n"));

					loginSuccessFile.Seek(0,CFile::end);
					loginSuccessFile.WriteString(strWriteData);
				
				}
			}
		}
		loginSuccessFile.Flush();
		loginSuccessFile.Close();
	}
	

	if (loginFailedCounts > 0)
	{
		loginFailedPath += TEXT("LoginFailedLog\\exectueFailedAppleId_");
		loginFailedPath += fileName;
		if (!loginFailedFile.Open(loginFailedPath.GetBuffer(),CFile::modeCreate|CFile::modeWrite))
		{
			MessageBox(TEXT("创建文件失败:2"));
			return;
		}
		for (AccountInfo::iterator ptr = m_accountInfoList.begin(); ptr!= m_accountInfoList.end(); ptr++)	
		{
			for (int i=0; i<loginFailedCounts; i++)
			{
				if ( ptr->nIndex == m_vecLoginFailedAppleIdIndex.at(i) )
				{
					//把登录失败的账号记录到新的文件中
					memset(strWriteData,0,sizeof(TCHAR)*MAX_PATH);
					wcscpy(strWriteData,ptr->strAppleId);
					wcscat(strWriteData,TEXT(","));
					wcscat(strWriteData,ptr->strPassword);
					if (ptr->deviceInfo.strUdid[0] != TEXT('\0'))
					{
						wcscat(strWriteData,TEXT(","));
						wcscat(strWriteData,ptr->deviceInfo.strUdid);
					}
					if (ptr->deviceInfo.strSerialNumber[0] != TEXT('\0'))
					{
						wcscat(strWriteData,TEXT(","));
						wcscat(strWriteData,ptr->deviceInfo.strSerialNumber);
					}
					if (ptr->deviceInfo.strMEID[0] != TEXT('\0'))
					{
						wcscat(strWriteData,TEXT(","));
						wcscat(strWriteData,ptr->deviceInfo.strMEID);
					}
					if (ptr->deviceInfo.strIMEI[0] != TEXT('\0'))
					{
						wcscat(strWriteData,TEXT(","));
						wcscat(strWriteData,ptr->deviceInfo.strIMEI);
					}
					wcscat(strWriteData,TEXT("\n"));

					loginFailedFile.Seek(0,CFile::end);
					loginFailedFile.WriteString(strWriteData);
				}
			}
		}
		loginFailedFile.Flush();
		loginFailedFile.Close();

	}


	if (executeFaliedCounts > 0)
	{
		executeFailedPath += TEXT("ExecuteFailedLog\\exectueFailedAppleId_");
		executeFailedPath += fileName;
		if (!executeFailedFile.Open(executeFailedPath.GetBuffer(),CFile::modeCreate|CFile::modeWrite))
		{
			MessageBox(TEXT("创建文件失败:3"));
			return;
		}
		for (AccountInfo::iterator ptr = m_accountInfoList.begin(); ptr!= m_accountInfoList.end(); ptr++)	
		{
			for (int i=0; i<executeFaliedCounts; i++)
			{
				if ( ptr->nIndex == m_vecExecuteFailedAppleIdIndex.at(i) )
				{
					//把任务失败的账号记录到新的文件中
					memset(strWriteData,0,sizeof(TCHAR)*MAX_PATH);
					wcscpy(strWriteData,ptr->strAppleId);
					wcscat(strWriteData,TEXT(","));
					wcscat(strWriteData,ptr->strPassword);
					if (ptr->deviceInfo.strUdid[0] != TEXT('\0'))
					{
						wcscat(strWriteData,TEXT(","));
						wcscat(strWriteData,ptr->deviceInfo.strUdid);
					}
					if (ptr->deviceInfo.strSerialNumber[0] != TEXT('\0'))
					{
						wcscat(strWriteData,TEXT(","));
						wcscat(strWriteData,ptr->deviceInfo.strSerialNumber);
					}
					if (ptr->deviceInfo.strMEID[0] != TEXT('\0'))
					{
						wcscat(strWriteData,TEXT(","));
						wcscat(strWriteData,ptr->deviceInfo.strMEID);
					}
					if (ptr->deviceInfo.strIMEI[0] != TEXT('\0'))
					{
						wcscat(strWriteData,TEXT(","));
						wcscat(strWriteData,ptr->deviceInfo.strIMEI);
					}
					wcscat(strWriteData,TEXT("\n"));

					executeFailedFile.Seek(0,CFile::end);
					executeFailedFile.WriteString(strWriteData);
				}
			}
		}
		executeFailedFile.Flush();
		executeFailedFile.Close();
	}


	if (successCounts > 0)
	{
		successPath += TEXT("SuccessLog\\successAppleId_");
		successPath += fileName;
		if (!successFile.Open(successPath.GetBuffer(),CFile::modeCreate|CFile::modeWrite))
		{
			MessageBox(TEXT("创建文件失败:4"));
			return;
		}
		for (AccountInfo::iterator ptr = m_accountInfoList.begin(); ptr!= m_accountInfoList.end(); ptr++)	
		{
			for (int i=0; i<successCounts; i++)
			{
				if ( ptr->nIndex == m_vecSuccessAppleIdIndex.at(i) )
				{
					//把任务失败的账号记录到新的文件中
					memset(strWriteData,0,sizeof(TCHAR)*MAX_PATH);
					wcscpy(strWriteData,ptr->strAppleId);
					wcscat(strWriteData,TEXT(","));
					wcscat(strWriteData,ptr->strPassword);
					if (ptr->deviceInfo.strUdid[0] != TEXT('\0'))
					{
						wcscat(strWriteData,TEXT(","));
						wcscat(strWriteData,ptr->deviceInfo.strUdid);
					}
					if (ptr->deviceInfo.strSerialNumber[0] != TEXT('\0'))
					{
						wcscat(strWriteData,TEXT(","));
						wcscat(strWriteData,ptr->deviceInfo.strSerialNumber);
					}
					if (ptr->deviceInfo.strMEID[0] != TEXT('\0'))
					{
						wcscat(strWriteData,TEXT(","));
						wcscat(strWriteData,ptr->deviceInfo.strMEID);
					}
					if (ptr->deviceInfo.strIMEI[0] != TEXT('\0'))
					{
						wcscat(strWriteData,TEXT(","));
						wcscat(strWriteData,ptr->deviceInfo.strIMEI);
					}
					wcscat(strWriteData,TEXT("\n"));

					successFile.Seek(0,CFile::end);
					successFile.WriteString(strWriteData);
				}
			}
		}
		successFile.Flush();
		successFile.Close();
	}

	if (accountStopCounts > 0)
	{
		accountStopPath += TEXT("AccountStopLog\\AccontStopAppleId_");
		accountStopPath += fileName;
		if (!accountStopFile.Open(accountStopPath.GetBuffer(),CFile::modeCreate|CFile::modeWrite))
		{
			MessageBox(TEXT("创建文件失败:5"));
			return;
		}
		for (AccountInfo::iterator ptr = m_accountInfoList.begin(); ptr!= m_accountInfoList.end(); ptr++)	
		{
			for (int i=0; i<accountStopCounts; i++)
			{
				if ( ptr->nIndex == m_vecAccountStopAppleIdIndex.at(i) )
				{
					//把任务失败的账号记录到新的文件中
					memset(strWriteData,0,sizeof(TCHAR)*MAX_PATH);
					wcscpy(strWriteData,ptr->strAppleId);
					wcscat(strWriteData,TEXT(","));
					wcscat(strWriteData,ptr->strPassword);
					if (ptr->deviceInfo.strUdid[0] != TEXT('\0'))
					{
						wcscat(strWriteData,TEXT(","));
						wcscat(strWriteData,ptr->deviceInfo.strUdid);
					}
					if (ptr->deviceInfo.strSerialNumber[0] != TEXT('\0'))
					{
						wcscat(strWriteData,TEXT(","));
						wcscat(strWriteData,ptr->deviceInfo.strSerialNumber);
					}
					if (ptr->deviceInfo.strMEID[0] != TEXT('\0'))
					{
						wcscat(strWriteData,TEXT(","));
						wcscat(strWriteData,ptr->deviceInfo.strMEID);
					}
					if (ptr->deviceInfo.strIMEI[0] != TEXT('\0'))
					{
						wcscat(strWriteData,TEXT(","));
						wcscat(strWriteData,ptr->deviceInfo.strIMEI);
					}
					wcscat(strWriteData,TEXT("\n"));

					accountStopFile.Seek(0,CFile::end);
					accountStopFile.WriteString(strWriteData);
				}
			}
		}
		accountStopFile.Flush();
		accountStopFile.Close();
	}

	if (accountLockedCounts > 0)
	{
		accountLockedPath += TEXT("AccountLockedLog\\AccontLockedAppleId_");
		accountLockedPath += fileName;
		if (!accountLockedFile.Open(accountLockedPath.GetBuffer(),CFile::modeCreate|CFile::modeWrite))
		{
			MessageBox(TEXT("创建文件失败:6"));
			return;
		}
		for (AccountInfo::iterator ptr = m_accountInfoList.begin(); ptr!= m_accountInfoList.end(); ptr++)	
		{
			for (int i=0; i<accountLockedCounts; i++)
			{
				if ( ptr->nIndex == m_vecAccountLockedAppleIdIndex.at(i) )
				{
					//把任务失败的账号记录到新的文件中
					memset(strWriteData,0,sizeof(TCHAR)*MAX_PATH);
					wcscpy(strWriteData,ptr->strAppleId);
					wcscat(strWriteData,TEXT(","));
					wcscat(strWriteData,ptr->strPassword);
					if (ptr->deviceInfo.strUdid[0] != TEXT('\0'))
					{
						wcscat(strWriteData,TEXT(","));
						wcscat(strWriteData,ptr->deviceInfo.strUdid);
					}
					if (ptr->deviceInfo.strSerialNumber[0] != TEXT('\0'))
					{
						wcscat(strWriteData,TEXT(","));
						wcscat(strWriteData,ptr->deviceInfo.strSerialNumber);
					}
					if (ptr->deviceInfo.strMEID[0] != TEXT('\0'))
					{
						wcscat(strWriteData,TEXT(","));
						wcscat(strWriteData,ptr->deviceInfo.strMEID);
					}
					if (ptr->deviceInfo.strIMEI[0] != TEXT('\0'))
					{
						wcscat(strWriteData,TEXT(","));
						wcscat(strWriteData,ptr->deviceInfo.strIMEI);
					}
					wcscat(strWriteData,TEXT("\n"));

					accountLockedFile.Seek(0,CFile::end);
					accountLockedFile.WriteString(strWriteData);
				}
			}
		}
		accountLockedFile.Flush();
		accountLockedFile.Close();
	}

	if (passwordErrorCounts > 0)
	{
		passwordErrorPath += TEXT("passwordErrorLog\\passwordErrorAppleId_");
		passwordErrorPath += fileName;
		if (!passwordErrorFile.Open(passwordErrorPath.GetBuffer(),CFile::modeCreate|CFile::modeWrite))
		{
			MessageBox(TEXT("创建文件失败:7"));
			return;
		}
		for (AccountInfo::iterator ptr = m_accountInfoList.begin(); ptr!= m_accountInfoList.end(); ptr++)	
		{
			for (int i=0; i<passwordErrorCounts; i++)
			{
				if ( ptr->nIndex == m_vecPasswordErrorAppleIdIndex.at(i) )
				{
					//把任务失败的账号记录到新的文件中
					memset(strWriteData,0,sizeof(TCHAR)*MAX_PATH);
					wcscpy(strWriteData,ptr->strAppleId);
					wcscat(strWriteData,TEXT(","));
					wcscat(strWriteData,ptr->strPassword);
					if (ptr->deviceInfo.strUdid[0] != TEXT('\0'))
					{
						wcscat(strWriteData,TEXT(","));
						wcscat(strWriteData,ptr->deviceInfo.strUdid);
					}
					if (ptr->deviceInfo.strSerialNumber[0] != TEXT('\0'))
					{
						wcscat(strWriteData,TEXT(","));
						wcscat(strWriteData,ptr->deviceInfo.strSerialNumber);
					}
					if (ptr->deviceInfo.strMEID[0] != TEXT('\0'))
					{
						wcscat(strWriteData,TEXT(","));
						wcscat(strWriteData,ptr->deviceInfo.strMEID);
					}
					if (ptr->deviceInfo.strIMEI[0] != TEXT('\0'))
					{
						wcscat(strWriteData,TEXT(","));
						wcscat(strWriteData,ptr->deviceInfo.strIMEI);
					}
					wcscat(strWriteData,TEXT("\n"));

					passwordErrorFile.Seek(0,CFile::end);
					passwordErrorFile.WriteString(strWriteData);
				}
			}
		}
		passwordErrorFile.Flush();
		passwordErrorFile.Close();
	}

	if (accountFaultCounts > 0)
	{
		accountFaultPath += TEXT("accountFaultLog\\accountFaultAppleId_");
		accountFaultPath += fileName;
		if (!accountFaultFile.Open(accountFaultPath.GetBuffer(),CFile::modeCreate|CFile::modeWrite))
		{
			MessageBox(TEXT("创建文件失败:8"));
			return;
		}
		for (AccountInfo::iterator ptr = m_accountInfoList.begin(); ptr!= m_accountInfoList.end(); ptr++)	
		{
			for (int i=0; i<accountFaultCounts; i++)
			{
				if ( ptr->nIndex == m_vecAccountFaultAppleIdIndex.at(i) )
				{
					//把任务失败的账号记录到新的文件中
					memset(strWriteData,0,sizeof(TCHAR)*MAX_PATH);
					wcscpy(strWriteData,ptr->strAppleId);
					wcscat(strWriteData,TEXT(","));
					wcscat(strWriteData,ptr->strPassword);
					if (ptr->deviceInfo.strUdid[0] != TEXT('\0'))
					{
						wcscat(strWriteData,TEXT(","));
						wcscat(strWriteData,ptr->deviceInfo.strUdid);
					}
					if (ptr->deviceInfo.strSerialNumber[0] != TEXT('\0'))
					{
						wcscat(strWriteData,TEXT(","));
						wcscat(strWriteData,ptr->deviceInfo.strSerialNumber);
					}
					if (ptr->deviceInfo.strMEID[0] != TEXT('\0'))
					{
						wcscat(strWriteData,TEXT(","));
						wcscat(strWriteData,ptr->deviceInfo.strMEID);
					}
					if (ptr->deviceInfo.strIMEI[0] != TEXT('\0'))
					{
						wcscat(strWriteData,TEXT(","));
						wcscat(strWriteData,ptr->deviceInfo.strIMEI);
					}
					wcscat(strWriteData,TEXT("\n"));

					accountFaultFile.Seek(0,CFile::end);
					accountFaultFile.WriteString(strWriteData);
				}
			}
		}
		accountFaultFile.Flush();
		accountFaultFile.Close();
	}
	return;

}


UINT WINAPI CxhASODlg::commentShowThreadProc(LPVOID lpPtr)
{
	CxhASODlg* pThis = (CxhASODlg*)lpPtr;
	CString strPath = pThis->m_commentDlg.m_strCommentfilePath;
	if (strPath.IsEmpty())
	{
		return 0;
	}

	CStdioFile file;
	if (!file.Open(strPath.GetBuffer(),CFile::modeRead))
	{
		return -1;
	}

	int nIndex = 0;
	CString strLineData;
	try
	{
		while (file.ReadString(strLineData) != FALSE)
		{
			//解析行数据
			++nIndex;
			pThis->ReadStringToUnicode(strLineData);
			pThis->parseCommentLineData(strLineData,nIndex);
		}

	}
	catch(int err)
	{
		file.Close();
		throw err;
	}

	file.Close();

	return 0;
}

void CxhASODlg::parseCommentLineData(CString pLineData,int nIndex)
{
	CString strNickName;
	CString strTitle;
	CString strContent;
	CString strTmp;
	TCHAR strFormat[] = TEXT(",");

	int  pos = pLineData.Find(strFormat);
	strNickName = pLineData.Left(pos);

	strTmp = pLineData.Right(pLineData.GetLength()-pos-wcslen(strFormat));
	pos = strTmp.Find(strFormat);

	strTitle = strTmp.Left(pos);
	strTmp = strTmp.Right(strTmp.GetLength()-pos-wcslen(strFormat));

	strContent = strTmp;

	int i = nIndex - 1;
	m_appleIdLoginAppDownload.SetItemText(i,4,strNickName);
	m_appleIdLoginAppDownload.SetItemText(i,5,strTitle);
	m_appleIdLoginAppDownload.SetItemText(i,6,strContent);

	return;

}

BOOL CxhASODlg::ReadStringToUnicode(CString &str)
{
	char *szBuf = new char[ str.GetLength()+1]; //数量要加1
	
	for (int i = 0; i<str.GetLength(); i++)
	{
		szBuf[i] = (CHAR)str.GetAt(i);
	}  
	szBuf[str.GetLength()]='\0';   //这里，必须要加上，否则会在结尾片显示乱码

	BOOL bok= CharToUnicode(szBuf , &str);
	if (szBuf)
	{
		delete []szBuf;
		szBuf = NULL;
	}

	return bok;
}

// 将Char型字符转换为Unicode字符
int CxhASODlg::CharToUnicode(char *pchIn, CString *pstrOut)
{
	int nLen;
	WCHAR *ptch;

	if(pchIn == NULL)
	{
		return 0;
	}

	nLen = MultiByteToWideChar(CP_ACP, 0, pchIn, -1, NULL, 0);//取得所需缓存的多少
	//申请缓存空间
	ptch = new WCHAR[nLen];
	MultiByteToWideChar(CP_ACP,0,pchIn,-1,ptch,nLen);//转码
	pstrOut->Format(_T("%s"), ptch);
	if (ptch)
	{
		delete [] ptch;

		ptch = NULL;
	}

	return nLen;
}


UINT WINAPI CxhASODlg::vpnIpThreadProc(LPVOID lpPtr)
{
	CxhASODlg* pThis = (CxhASODlg*)lpPtr;
	if (!pThis)
	{
		return -1;
	}

	CString strPath = pThis->m_vpnIpDlg.m_strVpnIPFilePath;
	if (strPath.IsEmpty())
	{
		return 0;
	}


	CStdioFile file;
	if (!file.Open(strPath.GetBuffer(),CFile::modeRead))
	{
		return -1;
	}

	CString strLineData;
	try
	{
		while (file.ReadString(strLineData) != FALSE)
		{
			pThis->m_vpnIPlist.push_back(strLineData);
			pThis->m_ServerVPNIP = strLineData;
		}

	}
	catch(int err)
	{
		file.Close();
		throw err;
	}

	file.Close();


	return 0;
}

LRESULT CxhASODlg::TerminateThread(WPARAM wParam, LPARAM lParam)
{
	DWORD dwExitCode = 0;
	if(m_handle)
	{
	   	::TerminateThread(m_handle,dwExitCode);
	}

	return 0;

}

void CxhASODlg::SendEmailEx(void)
{
	//获取文件路径
	CString strPath = m_appleIdDlg.m_strAppleIdFilePath;
	DWORD dwThreadId;
	HANDLE hThread = ::CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)appleIdThreadPro,this,0,&dwThreadId);
	if (!hThread)
	{
		return;
	}
}

DWORD WINAPI CxhASODlg::executeSearchWorkItem(LPVOID lpPtr)
{
	CxhASODlg* pThis = (CxhASODlg*)lpPtr;
	_ASSERTE( pThis != NULL );

	int ItemCount = pThis->m_appleIdLoginAppDownload.GetItemCount();
	int nSize = pThis->m_accountInfoList.size();
	int nModSize = pThis->m_vecModel.size();
	int randIndex = 0;
	int executeCounts = pThis->GetDlgItemInt(IDC_EDIT_EXECUTE_COUNTS);
	
	CString strAccountId;
	CString strPassword;
	CString strUdid;
	CString strMEID;
	CString strIMEI;
	CString strSerialNumber;

	HANDLE hThread = NULL;
	DWORD dwThreadId = 0;
	SENDINFO sendInfo = {0};
	ITEMINFO itemInfo = {0};

	//批量循环登录搜索下载
	for (AccountInfo::iterator ptr = pThis->m_accountInfoList.begin(); ptr!=pThis->m_accountInfoList.end(); ptr++)
	{
		strAccountId = ptr->strAppleId;
		strPassword = ptr->strPassword;
		strUdid = ptr->deviceInfo.strUdid;
		strMEID = ptr->deviceInfo.strMEID;
		strIMEI = ptr->deviceInfo.strIMEI;
		strSerialNumber = ptr->deviceInfo.strSerialNumber;
		int i = ptr->nIndex - 1;

		//清除cookie
		pThis->deleteCookieFile();

		//获取产品id和关键词
		CString strSaleId;
		pThis->m_appId.GetWindowText(strSaleId);

		CString strKeyword;
		pThis->m_hotkeyword.GetWindowText(strKeyword);

		CString strTargetUrl;
		pThis->m_editTargetUrl.GetWindowText(strTargetUrl);
		CString strTmp = _T("https://itunes.apple.com");
		strTargetUrl = strTargetUrl.Right(strTargetUrl.GetLength()-strTmp.GetLength());

		//创建事件名
		CString strEventName;
		LARGE_INTEGER timeRand; 
		QueryPerformanceCounter(&timeRand);
		strEventName.Format(TEXT("EXEPROC%d"),timeRand.LowPart);
		
		sendInfo.accountInfo.nIndex = i;
		wcscpy(sendInfo.accountInfo.strAppleId,strAccountId.GetBuffer());
		wcscpy(sendInfo.accountInfo.strPassword,strPassword.GetBuffer());
		wcscpy(sendInfo.accountInfo.deviceInfo.strUdid,strUdid.GetBuffer());
		wcscpy(sendInfo.accountInfo.deviceInfo.strMEID,strMEID.GetBuffer());
		wcscpy(sendInfo.accountInfo.deviceInfo.strIMEI,strIMEI.GetBuffer());
		wcscpy(sendInfo.accountInfo.deviceInfo.strSerialNumber,strSerialNumber.GetBuffer());
		wcscpy(sendInfo.strTargetUrl,strTargetUrl.GetBuffer());

		randIndex = rand()%nModSize;
		for (int k=0; k<nModSize; k++)
		{
			if (k == randIndex)
			{
				strcpy(sendInfo.modelInfo.szModel,pThis->m_vecModel.at(k).szModel);
				strcpy(sendInfo.modelInfo.szBuild,pThis->m_vecModel.at(k).szBuild);
				strcpy(sendInfo.modelInfo.szVersion,pThis->m_vecModel.at(k).szVersion);
				strcpy(sendInfo.modelInfo.szHardwarePlatform,pThis->m_vecModel.at(k).szHardwarePlatform);
				sendInfo.modelInfo.innerWidth = pThis->m_vecModel.at(k).innerWidth;
				sendInfo.modelInfo.innerHeight = pThis->m_vecModel.at(k).innerHeight;
				sendInfo.modelInfo.width = pThis->m_vecModel.at(k).width;
				sendInfo.modelInfo.height = pThis->m_vecModel.at(k).height;
			}
		}

		wcscpy(sendInfo.strEventName,strEventName.GetBuffer());
		wcscpy(sendInfo.strHotKeyWord,strKeyword.GetBuffer());
		wcscpy(sendInfo.strSaleId,strSaleId.GetBuffer());
		sendInfo.netType = 0;
		sendInfo.type = pThis->SEARCHAPP;
		sendInfo.bPaidApp = pThis->m_bBuyType;

		itemInfo.lpThis = pThis;
		itemInfo.sendInfo = sendInfo;

		//判断GSA网络验证是否执行过
		if (!pThis->m_bThreadGsaVerifier)
		{
			pThis->m_bThreadGsaVerifier = pGSA_Apple_Thread_NetworkVerifier(strUdid.GetBuffer(),NULL,0);
		}

		//判断线程池中排队的个数是否达到上限
		if (pThis->m_ItemCount >= executeCounts)
		{
			//队列已经满了，等待完成线程出队列
			WaitForSingleObject(pThis->m_hQueueEvent,INFINITE);
		
		}

		pThis->m_appleIdLoginAppDownload.SetItemText(i,2,TEXT("任务执行中"));

		QueueUserWorkItem(executeTaskThread,(PVOID)&itemInfo,WT_EXECUTEDEFAULT);

		::Sleep(100);

		if (ptr->nIndex == nSize)
		{
			::SetEvent(pThis->m_hThreadEvent);
		}

	}

	//等待任务线程全部完成
	::WaitForSingleObject(pThis->m_hThreadEvent,INFINITE);
	//线程完成后卸载模块
	pThis->unloadFunInterface();
	return 0;
}

DWORD WINAPI CxhASODlg::executeTaskThread(LPVOID lpPtr)
{
	PITEMINFO pItemInfo = (PITEMINFO)lpPtr;
	CxhASODlg* pThis = (CxhASODlg*)pItemInfo->lpThis;

	int nIndex = pItemInfo->sendInfo.accountInfo.nIndex;

	int status = pThis->InterfaceFunction(&pItemInfo->sendInfo);

	pThis->SetDisplayResultInfo(status,nIndex);

	return 0;
}

DWORD WINAPI CxhASODlg::executeLoginAppleIdItem(LPVOID lpPtr)
{
	CxhASODlg* pThis = (CxhASODlg*)lpPtr;
	_ASSERTE( pThis != NULL );

	int ItemCount = pThis->m_appleIdLoginAppDownload.GetItemCount();
	int nSize = pThis->m_accountInfoList.size();
	int nModSize = pThis->m_vecModel.size();
	int randIndex = 0;
	int executeCounts = pThis->GetDlgItemInt(IDC_EDIT_EXECUTE_COUNTS);
	
	CString strAccountId;
	CString strPassword;
	CString strUdid;
	CString strMEID;
	CString strIMEI;
	CString strSerialNumber;

	HANDLE hThread = NULL;
	DWORD dwThreadId = 0;
	SENDINFO sendInfo = {0};
	ITEMINFO itemInfo = {0};

	//批量循环登录搜索下载
	for (AccountInfo::iterator ptr = pThis->m_accountInfoList.begin(); ptr!=pThis->m_accountInfoList.end(); ptr++)
	{
		strAccountId = ptr->strAppleId;
		strPassword = ptr->strPassword;
		strUdid = ptr->deviceInfo.strUdid;
		strMEID = ptr->deviceInfo.strMEID;
		strIMEI = ptr->deviceInfo.strIMEI;
		strSerialNumber = ptr->deviceInfo.strSerialNumber;
		int i = ptr->nIndex - 1;

		//清除cookie
		pThis->deleteCookieFile();

		//获取产品id和关键词
		CString strSaleId;
		pThis->m_appId.GetWindowText(strSaleId);

		CString strKeyword;
		pThis->m_hotkeyword.GetWindowText(strKeyword);

		CString strTargetUrl;
		pThis->m_editTargetUrl.GetWindowText(strTargetUrl);
		CString strTmp = _T("https://itunes.apple.com");
		strTargetUrl = strTargetUrl.Right(strTargetUrl.GetLength()-strTmp.GetLength());

		//创建事件名
		CString strEventName;
		LARGE_INTEGER timeRand; 
		QueryPerformanceCounter(&timeRand);
		strEventName.Format(TEXT("EXEPROC%d"),timeRand.LowPart);
		
		sendInfo.accountInfo.nIndex = i;
		wcscpy(sendInfo.accountInfo.strAppleId,strAccountId.GetBuffer());
		wcscpy(sendInfo.accountInfo.strPassword,strPassword.GetBuffer());
		wcscpy(sendInfo.accountInfo.deviceInfo.strUdid,strUdid.GetBuffer());
		wcscpy(sendInfo.accountInfo.deviceInfo.strMEID,strMEID.GetBuffer());
		wcscpy(sendInfo.accountInfo.deviceInfo.strIMEI,strIMEI.GetBuffer());
		wcscpy(sendInfo.accountInfo.deviceInfo.strSerialNumber,strSerialNumber.GetBuffer());
		wcscpy(sendInfo.strTargetUrl,strTargetUrl.GetBuffer());

		randIndex = rand()%nModSize;
		for (int k=0; k<nModSize; k++)
		{
			if (k == randIndex)
			{
				strcpy(sendInfo.modelInfo.szModel,pThis->m_vecModel.at(k).szModel);
				strcpy(sendInfo.modelInfo.szBuild,pThis->m_vecModel.at(k).szBuild);
				strcpy(sendInfo.modelInfo.szVersion,pThis->m_vecModel.at(k).szVersion);
				strcpy(sendInfo.modelInfo.szHardwarePlatform,pThis->m_vecModel.at(k).szHardwarePlatform);
				sendInfo.modelInfo.innerWidth = pThis->m_vecModel.at(k).innerWidth;
				sendInfo.modelInfo.innerHeight = pThis->m_vecModel.at(k).innerHeight;
				sendInfo.modelInfo.width = pThis->m_vecModel.at(k).width;
				sendInfo.modelInfo.height = pThis->m_vecModel.at(k).height;
			}
		}

		wcscpy(sendInfo.strEventName,strEventName.GetBuffer());
		wcscpy(sendInfo.strHotKeyWord,strKeyword.GetBuffer());
		wcscpy(sendInfo.strSaleId,strSaleId.GetBuffer());
		sendInfo.netType = 0;
		sendInfo.type = pThis->ONLYLOGIN;
		sendInfo.bPaidApp = pThis->m_bBuyType;

		itemInfo.lpThis = pThis;
		itemInfo.sendInfo = sendInfo;

		//判断GSA网络验证是否执行过
		if (!pThis->m_bThreadGsaVerifier)
		{
			pThis->m_bThreadGsaVerifier = pGSA_Apple_Thread_NetworkVerifier(strUdid.GetBuffer(),NULL,0);
		}

		//判断线程池中排队的个数是否达到上限
		if (pThis->m_ItemCount >= executeCounts)
		{
			//队列已经满了，等待完成线程出队列
			WaitForSingleObject(pThis->m_hQueueEvent,INFINITE);
		
		}

		pThis->m_appleIdLoginAppDownload.SetItemText(i,2,TEXT("任务执行中"));

		QueueUserWorkItem(executeTaskThread,(PVOID)&itemInfo,WT_EXECUTEDEFAULT);

		::Sleep(100);

		if (ptr->nIndex == nSize)
		{
			::SetEvent(pThis->m_hThreadEvent);
		}

	}

	//等待任务线程全部完成
	::WaitForSingleObject(pThis->m_hThreadEvent,INFINITE);
	//线程完成后卸载模块
	pThis->unloadFunInterface();
	return 0;
}

DWORD WINAPI CxhASODlg::executeDownloadWorkItem(LPVOID lpPtr)
{
	CxhASODlg* pThis = (CxhASODlg*)lpPtr;
	_ASSERTE( pThis != NULL );

	int ItemCount = pThis->m_appleIdLoginAppDownload.GetItemCount();
	int nSize = pThis->m_accountInfoList.size();
	int nModSize = pThis->m_vecModel.size();
	int randIndex = 0;
	int executeCounts = pThis->GetDlgItemInt(IDC_EDIT_EXECUTE_COUNTS);
	
	CString strAccountId;
	CString strPassword;
	CString strUdid;
	CString strMEID;
	CString strIMEI;
	CString strSerialNumber;

	HANDLE hThread = NULL;
	DWORD dwThreadId = 0;
	SENDINFO sendInfo = {0};
	ITEMINFO itemInfo = {0};

	//批量循环登录搜索下载
	for (AccountInfo::iterator ptr = pThis->m_accountInfoList.begin(); ptr!=pThis->m_accountInfoList.end(); ptr++)
	{
		strAccountId = ptr->strAppleId;
		strPassword = ptr->strPassword;
		strUdid = ptr->deviceInfo.strUdid;
		strMEID = ptr->deviceInfo.strMEID;
		strIMEI = ptr->deviceInfo.strIMEI;
		strSerialNumber = ptr->deviceInfo.strSerialNumber;
		int i = ptr->nIndex - 1;

		//清除cookie
		pThis->deleteCookieFile();

		//获取产品id和关键词
		CString strSaleId;
		pThis->m_appId.GetWindowText(strSaleId);

		CString strKeyword;
		pThis->m_hotkeyword.GetWindowText(strKeyword);

		CString strTargetUrl;
		pThis->m_editTargetUrl.GetWindowText(strTargetUrl);
		CString strTmp = _T("https://itunes.apple.com");
		strTargetUrl = strTargetUrl.Right(strTargetUrl.GetLength()-strTmp.GetLength());

		//创建事件名
		CString strEventName;
		LARGE_INTEGER timeRand; 
		QueryPerformanceCounter(&timeRand);
		strEventName.Format(TEXT("EXEPROC%d"),timeRand.LowPart);
		
		sendInfo.accountInfo.nIndex = i;
		wcscpy(sendInfo.accountInfo.strAppleId,strAccountId.GetBuffer());
		wcscpy(sendInfo.accountInfo.strPassword,strPassword.GetBuffer());
		wcscpy(sendInfo.accountInfo.deviceInfo.strUdid,strUdid.GetBuffer());
		wcscpy(sendInfo.accountInfo.deviceInfo.strMEID,strMEID.GetBuffer());
		wcscpy(sendInfo.accountInfo.deviceInfo.strIMEI,strIMEI.GetBuffer());
		wcscpy(sendInfo.accountInfo.deviceInfo.strSerialNumber,strSerialNumber.GetBuffer());
		wcscpy(sendInfo.strTargetUrl,strTargetUrl.GetBuffer());
		
		randIndex = rand()%nModSize;
		for (int k=0; k<nModSize; k++)
		{
			if (k == randIndex)
			{
				strcpy(sendInfo.modelInfo.szModel,pThis->m_vecModel.at(k).szModel);
				strcpy(sendInfo.modelInfo.szBuild,pThis->m_vecModel.at(k).szBuild);
				strcpy(sendInfo.modelInfo.szVersion,pThis->m_vecModel.at(k).szVersion);
				strcpy(sendInfo.modelInfo.szHardwarePlatform,pThis->m_vecModel.at(k).szHardwarePlatform);
				sendInfo.modelInfo.innerWidth = pThis->m_vecModel.at(k).innerWidth;
				sendInfo.modelInfo.innerHeight = pThis->m_vecModel.at(k).innerHeight;
				sendInfo.modelInfo.width = pThis->m_vecModel.at(k).width;
				sendInfo.modelInfo.height = pThis->m_vecModel.at(k).height;
			}
		}

		wcscpy(sendInfo.strEventName,strEventName.GetBuffer());
		wcscpy(sendInfo.strHotKeyWord,strKeyword.GetBuffer());
		wcscpy(sendInfo.strSaleId,strSaleId.GetBuffer());
		sendInfo.netType = 0;
		sendInfo.type = pThis->LOGINDOWNLOADAPP;
		sendInfo.bPaidApp = pThis->m_bBuyType;

		itemInfo.lpThis = pThis;
		itemInfo.sendInfo = sendInfo;

		//判断GSA网络验证是否执行过
		if (!pThis->m_bThreadGsaVerifier)
		{
			pThis->m_bThreadGsaVerifier = pGSA_Apple_Thread_NetworkVerifier(strUdid.GetBuffer(),NULL,0);
		}

		//判断线程池中排队的个数是否达到上限
		if (pThis->m_ItemCount >= executeCounts)
		{
			//队列已经满了，等待完成线程出队列
			WaitForSingleObject(pThis->m_hQueueEvent,INFINITE);
		
		}

		pThis->m_appleIdLoginAppDownload.SetItemText(i,2,TEXT("任务执行中"));

		QueueUserWorkItem(executeTaskThread,(PVOID)&itemInfo,WT_EXECUTEDEFAULT);

		::Sleep(100);

		if (ptr->nIndex == nSize)
		{
			::SetEvent(pThis->m_hThreadEvent);
		}

	}

	//等待任务线程全部完成
	::WaitForSingleObject(pThis->m_hThreadEvent,INFINITE);
	//线程完成后卸载模块
	pThis->unloadFunInterface();
	return 0;
}
DWORD WINAPI CxhASODlg::executeCommentWorkItem(LPVOID lpPtr)
{
	CxhASODlg* pThis = (CxhASODlg*)lpPtr;
	_ASSERTE( pThis != NULL );

	int nModSize = pThis->m_vecModel.size();
	int randIndex = 0;
	return 0;
}

UINT WINAPI CxhASODlg::loginAccountThreadProc(LPVOID lpPtr)
{
	CxhASODlg* pThis = (CxhASODlg*)lpPtr;
	_ASSERTE( pThis != NULL );

	int ItemCount = pThis->m_appleIdLoginAppDownload.GetItemCount();
	int nSize = pThis->m_accountInfoList.size();
	int nModSize = pThis->m_vecModel.size();
	int randIndex = 0;
	int executeCounts = pThis->GetDlgItemInt(IDC_EDIT_EXECUTE_COUNTS);

	CString strAppPrice;

	CString strFatherAndMather;
	CString strTeacher;
	CString strBook;

	CString strAccountId;
	CString strPassword;
	CString strUdid;
	CString strMEID;
	CString strIMEI;
	CString strSerialNumber;
	HANDLE hThread = NULL;
	DWORD dwThreadId = 0;
	SENDINFO sendInfo = {0};
	ITEMINFO itemInfo = {0};

	//批量循环登录下载
	for (AccountInfo::iterator ptr = pThis->m_accountInfoList.begin(); ptr!= pThis->m_accountInfoList.end(); ptr++)
	{
		strAccountId = ptr->strAppleId;
		strPassword = ptr->strPassword;
		strUdid = ptr->deviceInfo.strUdid;
		strMEID = ptr->deviceInfo.strMEID;
		strIMEI = ptr->deviceInfo.strIMEI;
		strSerialNumber = ptr->deviceInfo.strSerialNumber;
		int i = ptr->nIndex - 1;

		if (pThis->m_bBuyType)
		{
			strFatherAndMather = pThis->m_appleIdLoginAppDownload.GetItemText(i,3);
			strTeacher = pThis->m_appleIdLoginAppDownload.GetItemText(i,4);
			strBook = pThis->m_appleIdLoginAppDownload.GetItemText(i,5);
		}
		
		//清除cookie
		pThis->deleteCookieFile();

		//创建事件名
		CString strEventName;
		LARGE_INTEGER timeRand; 
		QueryPerformanceCounter(&timeRand);
		strEventName.Format(TEXT("EXEPROC%d"),timeRand.LowPart);

		sendInfo.accountInfo.nIndex = i;
		wcscpy(sendInfo.accountInfo.strAppleId,strAccountId.GetBuffer());
		wcscpy(sendInfo.accountInfo.strPassword,strPassword.GetBuffer());
		wcscpy(sendInfo.accountInfo.deviceInfo.strUdid,strUdid.GetBuffer());
		wcscpy(sendInfo.accountInfo.deviceInfo.strMEID,strMEID.GetBuffer());
		wcscpy(sendInfo.accountInfo.deviceInfo.strIMEI,strIMEI.GetBuffer());
		wcscpy(sendInfo.accountInfo.deviceInfo.strSerialNumber,strSerialNumber.GetBuffer());

		randIndex = rand()%nModSize;
		for (int k=0; k<nModSize; k++)
		{
			if (k == randIndex)
			{
				strcpy(sendInfo.modelInfo.szModel,pThis->m_vecModel.at(k).szModel);
				strcpy(sendInfo.modelInfo.szBuild,pThis->m_vecModel.at(k).szBuild);
				strcpy(sendInfo.modelInfo.szVersion,pThis->m_vecModel.at(k).szVersion);
				strcpy(sendInfo.modelInfo.szHardwarePlatform,pThis->m_vecModel.at(k).szHardwarePlatform);
				sendInfo.modelInfo.innerWidth = pThis->m_vecModel.at(k).innerWidth;
				sendInfo.modelInfo.innerHeight = pThis->m_vecModel.at(k).innerHeight;
				sendInfo.modelInfo.width = pThis->m_vecModel.at(k).width;
				sendInfo.modelInfo.height = pThis->m_vecModel.at(k).height;
			}
		}

		wcscpy(sendInfo.strEventName,strEventName.GetBuffer());
		sendInfo.netType = 0;
		sendInfo.type = pThis->ONLYLOGIN;
		sendInfo.bPaidApp = pThis->m_bBuyType;

		itemInfo.lpThis = pThis;
		itemInfo.sendInfo = sendInfo;
	
		//判断线程池中排队的个数是否达到上限
		InterlockedIncrement(&pThis->m_ItemCount);
		if (pThis->m_ItemCount > executeCounts)
		{
			//队列已经满了，等待完成线程出队列
			WaitForSingleObject(pThis->m_hQueueEvent,INFINITE);
			
		}
		//开始登录
		pThis->ExectueAProcess(&sendInfo,i);

	}
	//等待完成剩下的任务
	WaitForSingleObject(pThis->m_hFinishEvent,1000*120);
	//过滤成功和失败的appleId
	pThis->FilterAppleId();

	pThis->m_bMonitorRunning = FALSE;

	return 0;
}

UINT WINAPI CxhASODlg::normalnetDownloadAppThreadProc(LPVOID lpPtr)
{
	CxhASODlg* pThis = (CxhASODlg*)lpPtr;
	_ASSERTE( pThis != NULL );

	int ItemCount = pThis->m_appleIdLoginAppDownload.GetItemCount();
	int nSize = pThis->m_accountInfoList.size();
	int nModSize = pThis->m_vecModel.size();
	int randIndex = 0;
	int executeCounts = pThis->GetDlgItemInt(IDC_EDIT_EXECUTE_COUNTS);

	CString strAppPrice;

	CString strFatherAndMather;
	CString strTeacher;
	CString strBook;

	CString strAccountId;
	CString strPassword;
	CString strUdid;
	CString strMEID;
	CString strIMEI;
	CString strSerialNumber;
	HANDLE hThread = NULL;
	DWORD dwThreadId = 0;
	SENDINFO sendInfo = {0};
	ITEMINFO itemInfo = {0};

	//批量循环登录下载
	for (AccountInfo::iterator ptr = pThis->m_accountInfoList.begin(); ptr!= pThis->m_accountInfoList.end(); ptr++)
	{
		strAccountId = ptr->strAppleId;
		strPassword = ptr->strPassword;
		strUdid = ptr->deviceInfo.strUdid;
		strMEID = ptr->deviceInfo.strMEID;
		strIMEI = ptr->deviceInfo.strIMEI;
		strSerialNumber = ptr->deviceInfo.strSerialNumber;
		int i = ptr->nIndex - 1;

		if (pThis->m_bBuyType)
		{
			strFatherAndMather = pThis->m_appleIdLoginAppDownload.GetItemText(i,3);
			strTeacher = pThis->m_appleIdLoginAppDownload.GetItemText(i,4);
			strBook = pThis->m_appleIdLoginAppDownload.GetItemText(i,5);
		}
		
		//清除cookie
		pThis->deleteCookieFile();

		//修改注册表硬件信息
		pThis->ModifyHardwareInfo(FALSE,FALSE);

		
		//获取显示当前ip地址
		pThis->SetVPNIPDisplay();

		//获取产品id和关键词
		CString strSaleId;
		pThis->m_appId.GetWindowText(strSaleId);

		CString strTargetUrl;
		pThis->m_editTargetUrl.GetWindowText(strTargetUrl);
		CString strTmp = _T("https://itunes.apple.com");
		strTargetUrl = strTargetUrl.Right(strTargetUrl.GetLength()-strTmp.GetLength());

		pThis->m_editAppPrice.GetWindowText(strAppPrice);


		//创建事件名
		CString strEventName;
		LARGE_INTEGER timeRand; 
		QueryPerformanceCounter(&timeRand);
		strEventName.Format(TEXT("EXEPROC%d"),timeRand.LowPart);

		sendInfo.accountInfo.nIndex = i;
		wcscpy(sendInfo.accountInfo.strAppleId,strAccountId.GetBuffer());
		wcscpy(sendInfo.accountInfo.strPassword,strPassword.GetBuffer());
		wcscpy(sendInfo.accountInfo.deviceInfo.strUdid,strUdid.GetBuffer());
		wcscpy(sendInfo.accountInfo.deviceInfo.strMEID,strMEID.GetBuffer());
		wcscpy(sendInfo.accountInfo.deviceInfo.strIMEI,strIMEI.GetBuffer());
		wcscpy(sendInfo.accountInfo.deviceInfo.strSerialNumber,strSerialNumber.GetBuffer());

		randIndex = rand()%nModSize;
		for (int k=0; k<nModSize; k++)
		{
			if (k == randIndex)
			{
				strcpy(sendInfo.modelInfo.szModel,pThis->m_vecModel.at(k).szModel);
				strcpy(sendInfo.modelInfo.szBuild,pThis->m_vecModel.at(k).szBuild);
				strcpy(sendInfo.modelInfo.szVersion,pThis->m_vecModel.at(k).szVersion);
				strcpy(sendInfo.modelInfo.szHardwarePlatform,pThis->m_vecModel.at(k).szHardwarePlatform);
				sendInfo.modelInfo.innerWidth = pThis->m_vecModel.at(k).innerWidth;
				sendInfo.modelInfo.innerHeight = pThis->m_vecModel.at(k).innerHeight;
				sendInfo.modelInfo.width = pThis->m_vecModel.at(k).width;
				sendInfo.modelInfo.height = pThis->m_vecModel.at(k).height;
			}
		}

		wcscpy(sendInfo.strTargetUrl,strTargetUrl.GetBuffer());
		wcscpy(sendInfo.strEventName,strEventName.GetBuffer());
		wcscpy(sendInfo.strSaleId,strSaleId.GetBuffer());
		wcscpy(sendInfo.strFatherAndMather,strFatherAndMather.GetBuffer());
		wcscpy(sendInfo.strTeacher,strTeacher.GetBuffer());
		wcscpy(sendInfo.strBook,strBook.GetBuffer());
		wcscpy(sendInfo.strAppPrice,strAppPrice.GetBuffer());
		sendInfo.netType = 0;
		sendInfo.type = pThis->LOGINDOWNLOADAPP;
		sendInfo.bPaidApp = pThis->m_bBuyType;

		itemInfo.lpThis = pThis;
		itemInfo.sendInfo = sendInfo;
	
		//判断线程池中排队的个数是否达到上限
		InterlockedIncrement(&pThis->m_ItemCount);
		if (pThis->m_ItemCount > executeCounts)
		{
			//队列已经满了，等待完成线程出队列
			WaitForSingleObject(pThis->m_hQueueEvent,INFINITE);
			
		}
		//开始登录下载
		pThis->ExectueAProcess(&sendInfo,i);

	}

	//等待完成剩下的任务
	WaitForSingleObject(pThis->m_hFinishEvent,1000*120);
	//过滤成功和失败的appleId
	pThis->FilterAppleId();

	pThis->m_bMonitorRunning = FALSE;
	return 0;
}

UINT WINAPI CxhASODlg::normalnetCommentThreadProc(LPVOID lpPtr)
{
	CxhASODlg* pThis = (CxhASODlg*)lpPtr;
	_ASSERTE( pThis != NULL );

	int ItemCount = pThis->m_appleIdLoginAppDownload.GetItemCount();
	int nSize = pThis->m_accountInfoList.size();
	int nModSize = pThis->m_vecModel.size();
	int randIndex = 0;
	int executeCounts = pThis->GetDlgItemInt(IDC_EDIT_EXECUTE_COUNTS);

	CString strNickName;
	CString strTitle;
	CString strContext;
	CString strAccountId;
	CString strPassword;

	CString strUdid;
	CString strMEID;
	CString strIMEI;
	CString strSerialNumber;

	CString strRate;
	SENDINFO sendInfo = {0};
	ITEMINFO itemInfo = {0};

	HANDLE hThread = NULL;
	DWORD dwThreadId = 0;

	//批量循环登录下载评论或付费购买
	for (AccountInfo::iterator ptr = pThis->m_accountInfoList.begin(); ptr!= pThis->m_accountInfoList.end(); ptr++)
	{
		strAccountId = ptr->strAppleId;
		strPassword = ptr->strPassword;
		strUdid = ptr->deviceInfo.strUdid;
		strMEID = ptr->deviceInfo.strMEID;
		strIMEI = ptr->deviceInfo.strIMEI;
		strSerialNumber = ptr->deviceInfo.strSerialNumber;
		int i = ptr->nIndex - 1;
		strRate = pThis->m_appleIdLoginAppDownload.GetItemText(i,3);
		strNickName = pThis->m_appleIdLoginAppDownload.GetItemText(i,4);
		strTitle = pThis->m_appleIdLoginAppDownload.GetItemText(i,5);
		strContext = pThis->m_appleIdLoginAppDownload.GetItemText(i,6);

		if (strTitle.IsEmpty() || strContext.IsEmpty())
		{
			pThis->m_appleIdLoginAppDownload.SetItemText(i,2,TEXT("无内容"));
			continue;
		}

		//清除cookie
		pThis->deleteCookieFile();
		//修改注册表硬件信息
		pThis->ModifyHardwareInfo(FALSE,FALSE);

		//获取显示当前ip地址
		pThis->SetVPNIPDisplay();
		
		//获取产品id和关键词
		CString strSaleId;
		pThis->m_appId.GetWindowText(strSaleId);

		CString strKeyword;
		pThis->m_hotkeyword.GetWindowText(strKeyword);

		CString strTargetUrl;
		pThis->m_editTargetUrl.GetWindowText(strTargetUrl);
		CString strTmp = _T("https://itunes.apple.com");
		strTargetUrl = strTargetUrl.Right(strTargetUrl.GetLength()-strTmp.GetLength());

		//创建事件名
		CString strEventName;
		LARGE_INTEGER timeRand; 
		QueryPerformanceCounter(&timeRand);
		strEventName.Format(TEXT("EXEPROC%d"),timeRand.LowPart);
		
		sendInfo.accountInfo.nIndex = i;
		wcscpy(sendInfo.accountInfo.strAppleId,strAccountId.GetBuffer());
		wcscpy(sendInfo.accountInfo.strPassword,strPassword.GetBuffer());
		wcscpy(sendInfo.accountInfo.deviceInfo.strUdid,strUdid.GetBuffer());
		wcscpy(sendInfo.accountInfo.deviceInfo.strMEID,strMEID.GetBuffer());
		wcscpy(sendInfo.accountInfo.deviceInfo.strIMEI,strIMEI.GetBuffer());
		wcscpy(sendInfo.accountInfo.deviceInfo.strSerialNumber,strSerialNumber.GetBuffer());
		wcscpy(sendInfo.strTargetUrl,strTargetUrl.GetBuffer());

		randIndex = rand()%nModSize;
		for (int k=0; k<nModSize; k++)
		{
			if (k == randIndex)
			{
				strcpy(sendInfo.modelInfo.szModel,pThis->m_vecModel.at(k).szModel);
				strcpy(sendInfo.modelInfo.szBuild,pThis->m_vecModel.at(k).szBuild);
				strcpy(sendInfo.modelInfo.szVersion,pThis->m_vecModel.at(k).szVersion);
				strcpy(sendInfo.modelInfo.szHardwarePlatform,pThis->m_vecModel.at(k).szHardwarePlatform);
				sendInfo.modelInfo.innerWidth = pThis->m_vecModel.at(k).innerWidth;
				sendInfo.modelInfo.innerHeight = pThis->m_vecModel.at(k).innerHeight;
				sendInfo.modelInfo.width = pThis->m_vecModel.at(k).width;
				sendInfo.modelInfo.height = pThis->m_vecModel.at(k).height;
			}
		}

		wcscpy(sendInfo.strEventName,strEventName.GetBuffer());
		wcscpy(sendInfo.strRate,strRate.GetBuffer());
		wcscpy(sendInfo.strNickName,strNickName.GetBuffer());
		wcscpy(sendInfo.strTitle,strTitle.GetBuffer());
		wcscpy(sendInfo.strContext,strContext.GetBuffer());
		wcscpy(sendInfo.strHotKeyWord,strKeyword.GetBuffer());
		wcscpy(sendInfo.strSaleId,strSaleId.GetBuffer());
		sendInfo.netType = 0;
		sendInfo.type = pThis->COMMENTAPP;
		sendInfo.bPaidApp = pThis->m_bBuyType;

		itemInfo.lpThis = pThis;
		itemInfo.sendInfo = sendInfo;

		//判断线程池中排队的个数是否达到上限
		InterlockedIncrement(&pThis->m_ItemCount);
		if (pThis->m_ItemCount > executeCounts)
		{
			//队列已经满了，等待完成线程出队列
			WaitForSingleObject(pThis->m_hQueueEvent,INFINITE);
			
		}
		//开始登录下载评论
		pThis->ExectueAProcess(&sendInfo,i);

	}//end for

	//等待完成剩下的任务
	WaitForSingleObject(pThis->m_hFinishEvent,1000*120);
	//过滤成功和失败的appleId
	pThis->FilterAppleId();
	
	pThis->m_bMonitorRunning = FALSE;
	return 0;
}

UINT WINAPI CxhASODlg::normalnetSearchKeywordThreadProc(LPVOID lpPtr)
{
	CxhASODlg* pThis = (CxhASODlg*)lpPtr;
	_ASSERTE( pThis != NULL );

	int ItemCount = pThis->m_appleIdLoginAppDownload.GetItemCount();
	int nSize = pThis->m_accountInfoList.size();
	int nModSize = pThis->m_vecModel.size();
	int randIndex = 0;
	int executeCounts = pThis->GetDlgItemInt(IDC_EDIT_EXECUTE_COUNTS);
	
	CString strAccountId;
	CString strPassword;
	CString strUdid;
	CString strMEID;
	CString strIMEI;
	CString strSerialNumber;

	HANDLE hThread = NULL;
	DWORD dwThreadId = 0;
	SENDINFO sendInfo = {0};
	ITEMINFO itemInfo = {0};

	//批量循环登录搜索下载
	for (AccountInfo::iterator ptr = pThis->m_accountInfoList.begin(); ptr!=pThis->m_accountInfoList.end(); ptr++)
	{
		strAccountId = ptr->strAppleId;
		strPassword = ptr->strPassword;
		strUdid = ptr->deviceInfo.strUdid;
		strMEID = ptr->deviceInfo.strMEID;
		strIMEI = ptr->deviceInfo.strIMEI;
		strSerialNumber = ptr->deviceInfo.strSerialNumber;
		int i = ptr->nIndex - 1;

		//清除cookie
		pThis->deleteCookieFile();
		//修改注册表硬件信息
		pThis->ModifyHardwareInfo(FALSE,FALSE);

		//获取显示当前ip地址
		pThis->SetVPNIPDisplay();

		//获取产品id和关键词
		CString strSaleId;
		pThis->m_appId.GetWindowText(strSaleId);

		CString strKeyword;
		pThis->m_hotkeyword.GetWindowText(strKeyword);

		CString strTargetUrl;
		pThis->m_editTargetUrl.GetWindowText(strTargetUrl);
		CString strTmp = _T("https://itunes.apple.com");
		strTargetUrl = strTargetUrl.Right(strTargetUrl.GetLength()-strTmp.GetLength());

		//创建事件名
		CString strEventName;
		LARGE_INTEGER timeRand; 
		QueryPerformanceCounter(&timeRand);
		strEventName.Format(TEXT("EXEPROC%d"),timeRand.LowPart);
		
		sendInfo.accountInfo.nIndex = i;
		wcscpy(sendInfo.accountInfo.strAppleId,strAccountId.GetBuffer());
		wcscpy(sendInfo.accountInfo.strPassword,strPassword.GetBuffer());
		wcscpy(sendInfo.accountInfo.deviceInfo.strUdid,strUdid.GetBuffer());
		wcscpy(sendInfo.accountInfo.deviceInfo.strMEID,strMEID.GetBuffer());
		wcscpy(sendInfo.accountInfo.deviceInfo.strIMEI,strIMEI.GetBuffer());
		wcscpy(sendInfo.accountInfo.deviceInfo.strSerialNumber,strSerialNumber.GetBuffer());

		randIndex = rand()%nModSize;
		for (int k=0; k<nModSize; k++)
		{
			if (k == randIndex)
			{
				strcpy(sendInfo.modelInfo.szModel,pThis->m_vecModel.at(k).szModel);
				strcpy(sendInfo.modelInfo.szBuild,pThis->m_vecModel.at(k).szBuild);
				strcpy(sendInfo.modelInfo.szVersion,pThis->m_vecModel.at(k).szVersion);
				strcpy(sendInfo.modelInfo.szHardwarePlatform,pThis->m_vecModel.at(k).szHardwarePlatform);
				sendInfo.modelInfo.innerWidth = pThis->m_vecModel.at(k).innerWidth;
				sendInfo.modelInfo.innerHeight = pThis->m_vecModel.at(k).innerHeight;
				sendInfo.modelInfo.width = pThis->m_vecModel.at(k).width;
				sendInfo.modelInfo.height = pThis->m_vecModel.at(k).height;
			}
		}
		

		wcscpy(sendInfo.strTargetUrl,strTargetUrl.GetBuffer());
		wcscpy(sendInfo.strEventName,strEventName.GetBuffer());
		wcscpy(sendInfo.strHotKeyWord,strKeyword.GetBuffer());
		wcscpy(sendInfo.strSaleId,strSaleId.GetBuffer());
		sendInfo.netType = 0;
		sendInfo.type = pThis->SEARCHAPP;
		sendInfo.bPaidApp = pThis->m_bBuyType;

		itemInfo.lpThis = pThis;
		itemInfo.sendInfo = sendInfo;

		//判断线程池中排队的个数是否达到上限
		InterlockedIncrement(&pThis->m_ItemCount);
		if (pThis->m_ItemCount > executeCounts)
		{
			//队列已经满了，等待完成线程出队列
			WaitForSingleObject(pThis->m_hQueueEvent,INFINITE);
			
		}
		//开始搜索登录下载
		pThis->ExectueAProcess(&sendInfo,i);

	}//end for

	//等待完成剩下的任务
	WaitForSingleObject(pThis->m_hFinishEvent,1000*120);
	//过滤成功和失败的appleId
	pThis->FilterAppleId();

	pThis->m_bMonitorRunning = FALSE;

	return 0;
}

//下载app核心线程
UINT WINAPI CxhASODlg::vpnDownloadAppThreadProc(LPVOID lpPtr)
{
	CxhASODlg* pThis = (CxhASODlg*)lpPtr;
	_ASSERTE( pThis != NULL );
	//nSize == ItemCount
	int ItemCount = pThis->m_appleIdLoginAppDownload.GetItemCount();
	int nSize = pThis->m_accountInfoList.size();
	int nModSize = pThis->m_vecModel.size();
	int randIndex = 0;
	int executeCounts = pThis->GetDlgItemInt(IDC_EDIT_EXECUTE_COUNTS);

	CString strAppPrice;

	CString strFatherAndMather;
	CString strTeacher;
	CString strBook;

	CString strAccountId;
	CString strPassword;
	CString strUdid;
	CString strMEID;
	CString strIMEI;
	CString strSerialNumber;
	HANDLE hThread = NULL;
	DWORD dwThreadId;
	SENDINFO sendInfo = {0};

	//批量循环登录下载
	for (AccountInfo::iterator ptr = pThis->m_accountInfoList.begin(); ptr!= pThis->m_accountInfoList.end(); ptr++)
	{
		strAccountId = ptr->strAppleId;
		strPassword = ptr->strPassword;
		strUdid = ptr->deviceInfo.strUdid;
		strMEID = ptr->deviceInfo.strMEID;
		strIMEI = ptr->deviceInfo.strIMEI;
		strSerialNumber = ptr->deviceInfo.strSerialNumber;
		int i = ptr->nIndex - 1;

		if (pThis->m_bBuyType)
		{
			strFatherAndMather = pThis->m_appleIdLoginAppDownload.GetItemText(i,3);
			strTeacher = pThis->m_appleIdLoginAppDownload.GetItemText(i,4);
			strBook = pThis->m_appleIdLoginAppDownload.GetItemText(i,5);
		}
		
		//清除cookie
		pThis->deleteCookieFile();

		//修改注册表硬件信息
		pThis->ModifyHardwareInfo(FALSE,FALSE);

		//获取产品id和关键词
		CString strSaleId;
		pThis->m_appId.GetWindowText(strSaleId);

		CString strTargetUrl;
		pThis->m_editTargetUrl.GetWindowText(strTargetUrl);
		CString strTmp = _T("https://itunes.apple.com");
		strTargetUrl = strTargetUrl.Right(strTargetUrl.GetLength()-strTmp.GetLength());

		pThis->m_editAppPrice.GetWindowText(strAppPrice);

		//创建事件名
		CString strEventName;
		LARGE_INTEGER timeRand; 
		QueryPerformanceCounter(&timeRand);
		strEventName.Format(TEXT("EXEPROC%d"),timeRand.LowPart);

		sendInfo.accountInfo.nIndex = i;
		wcscpy(sendInfo.accountInfo.strAppleId,strAccountId.GetBuffer());
		wcscpy(sendInfo.accountInfo.strPassword,strPassword.GetBuffer());
		wcscpy(sendInfo.accountInfo.deviceInfo.strUdid,strUdid.GetBuffer());
		wcscpy(sendInfo.accountInfo.deviceInfo.strMEID,strMEID.GetBuffer());
		wcscpy(sendInfo.accountInfo.deviceInfo.strIMEI,strIMEI.GetBuffer());
		wcscpy(sendInfo.accountInfo.deviceInfo.strSerialNumber,strSerialNumber.GetBuffer());
		wcscpy(sendInfo.strTargetUrl,strTargetUrl.GetBuffer());

		randIndex = rand()%nModSize;
		for (int k=0; k<nModSize; k++)
		{
			if (k == randIndex)
			{
				strcpy(sendInfo.modelInfo.szModel,pThis->m_vecModel.at(k).szModel);
				strcpy(sendInfo.modelInfo.szBuild,pThis->m_vecModel.at(k).szBuild);
				strcpy(sendInfo.modelInfo.szVersion,pThis->m_vecModel.at(k).szVersion);
				strcpy(sendInfo.modelInfo.szHardwarePlatform,pThis->m_vecModel.at(k).szHardwarePlatform);
				sendInfo.modelInfo.innerWidth = pThis->m_vecModel.at(k).innerWidth;
				sendInfo.modelInfo.innerHeight = pThis->m_vecModel.at(k).innerHeight;
				sendInfo.modelInfo.width = pThis->m_vecModel.at(k).width;
				sendInfo.modelInfo.height = pThis->m_vecModel.at(k).height;
			}
		}

		wcscpy(sendInfo.strEventName,strEventName.GetBuffer());
		wcscpy(sendInfo.strSaleId,strSaleId.GetBuffer());
		wcscpy(sendInfo.strFatherAndMather,strFatherAndMather.GetBuffer());
		wcscpy(sendInfo.strTeacher,strTeacher.GetBuffer());
		wcscpy(sendInfo.strBook,strBook.GetBuffer());
		wcscpy(sendInfo.strAppPrice,strAppPrice.GetBuffer());
		sendInfo.netType = 0;
		sendInfo.type = pThis->LOGINDOWNLOADAPP;
		sendInfo.bPaidApp = pThis->m_bBuyType;

		//判断线程池中排队的个数是否达到上限
		InterlockedIncrement(&pThis->m_ItemCount);
		
		pThis->ConnectVPN();

		//开始登录下载
		pThis->ExectueAProcess(&sendInfo,i);

		//记录个数增加
		++pThis->m_eachSwitchCounts;
		if (pThis->m_eachSwitchCounts == executeCounts)
		{
			WaitForSingleObject(pThis->m_hSwitchIPEvent,INFINITE);
			pThis->DisconnectVPN();
		}

	}//end for 

	//等待完成剩下的任务
	WaitForSingleObject(pThis->m_hFinishEvent,1000*120);
	//过滤成功和失败的appleId
	pThis->FilterAppleId();

	return 0;
}

//登录下载评论核心线程
UINT WINAPI CxhASODlg::vpnCommentThreadProc(LPVOID lpPtr)
{
	CxhASODlg* pThis = (CxhASODlg*)lpPtr;
	_ASSERTE( pThis != NULL );
	
	//nSize == ItemCount
	int ItemCount = pThis->m_appleIdLoginAppDownload.GetItemCount();
	int nSize = pThis->m_accountInfoList.size();
	int nModSize = pThis->m_vecModel.size();
	int randIndex = 0;
	int executeCounts = pThis->GetDlgItemInt(IDC_EDIT_EXECUTE_COUNTS);
	
	CString strNickName;
	CString strTitle;
	CString strContext;
	CString strAccountId;
	CString strPassword;
	CString strRate;

	CString strUdid;
	CString strMEID;
	CString strIMEI;
	CString strSerialNumber;

	HANDLE hThread = NULL;
	DWORD dwThreadId;
	SENDINFO sendInfo = {0};

	//批量循环登录下载评论
	for (AccountInfo::iterator ptr = pThis->m_accountInfoList.begin(); ptr!= pThis->m_accountInfoList.end(); ptr++)
	{
		strAccountId = ptr->strAppleId;
		strPassword = ptr->strPassword;
		strUdid = ptr->deviceInfo.strUdid;
		strMEID = ptr->deviceInfo.strMEID;
		strIMEI = ptr->deviceInfo.strIMEI;
		strSerialNumber = ptr->deviceInfo.strSerialNumber;
		int i = ptr->nIndex - 1;
		strRate = pThis->m_appleIdLoginAppDownload.GetItemText(i,3);
		strNickName = pThis->m_appleIdLoginAppDownload.GetItemText(i,4);
		strTitle = pThis->m_appleIdLoginAppDownload.GetItemText(i,5);
		strContext = pThis->m_appleIdLoginAppDownload.GetItemText(i,6);

		if (strTitle.IsEmpty() || strContext.IsEmpty())
		{
			pThis->m_appleIdLoginAppDownload.SetItemText(i,2,TEXT("无内容"));
			continue;
		}

		//清除cookie
		pThis->deleteCookieFile();
		//修改注册表硬件信息
		pThis->ModifyHardwareInfo(FALSE,FALSE);
		
		//获取产品id和关键词
		CString strSaleId;
		pThis->m_appId.GetWindowText(strSaleId);

		CString strKeyword;
		pThis->m_hotkeyword.GetWindowText(strKeyword);

		CString strTargetUrl;
		pThis->m_editTargetUrl.GetWindowText(strTargetUrl);
		CString strTmp = _T("https://itunes.apple.com");
		strTargetUrl = strTargetUrl.Right(strTargetUrl.GetLength()-strTmp.GetLength());

		//创建事件名
		CString strEventName;
		LARGE_INTEGER timeRand; 
		QueryPerformanceCounter(&timeRand);
		strEventName.Format(TEXT("EXEPROC%d"),timeRand.LowPart);

		sendInfo.accountInfo.nIndex = i;
		wcscpy(sendInfo.accountInfo.strAppleId,strAccountId.GetBuffer());
		wcscpy(sendInfo.accountInfo.strPassword,strPassword.GetBuffer());
		wcscpy(sendInfo.accountInfo.deviceInfo.strUdid,strUdid.GetBuffer());
		wcscpy(sendInfo.accountInfo.deviceInfo.strMEID,strMEID.GetBuffer());
		wcscpy(sendInfo.accountInfo.deviceInfo.strIMEI,strIMEI.GetBuffer());
		wcscpy(sendInfo.accountInfo.deviceInfo.strSerialNumber,strSerialNumber.GetBuffer());
		wcscpy(sendInfo.strTargetUrl,strTargetUrl.GetBuffer());

		randIndex = rand()%nModSize;
		for (int k=0; k<nModSize; k++)
		{
			if (k == randIndex)
			{
				strcpy(sendInfo.modelInfo.szModel,pThis->m_vecModel.at(k).szModel);
				strcpy(sendInfo.modelInfo.szBuild,pThis->m_vecModel.at(k).szBuild);
				strcpy(sendInfo.modelInfo.szVersion,pThis->m_vecModel.at(k).szVersion);
				strcpy(sendInfo.modelInfo.szHardwarePlatform,pThis->m_vecModel.at(k).szHardwarePlatform);
				sendInfo.modelInfo.innerWidth = pThis->m_vecModel.at(k).innerWidth;
				sendInfo.modelInfo.innerHeight = pThis->m_vecModel.at(k).innerHeight;
				sendInfo.modelInfo.width = pThis->m_vecModel.at(k).width;
				sendInfo.modelInfo.height = pThis->m_vecModel.at(k).height;
			}
		}

		wcscpy(sendInfo.strEventName,strEventName.GetBuffer());
		wcscpy(sendInfo.strRate,strRate.GetBuffer());
		wcscpy(sendInfo.strNickName,strNickName.GetBuffer());
		wcscpy(sendInfo.strTitle,strTitle.GetBuffer());
		wcscpy(sendInfo.strContext,strContext.GetBuffer());
		wcscpy(sendInfo.strHotKeyWord,strKeyword.GetBuffer());
		wcscpy(sendInfo.strSaleId,strSaleId.GetBuffer());
		sendInfo.netType = 0;
		sendInfo.type = pThis->COMMENTAPP;
		sendInfo.bPaidApp = pThis->m_bBuyType;

		//判断线程池中排队的个数是否达到上限
		InterlockedIncrement(&pThis->m_ItemCount);
		
		pThis->ConnectVPN();

		//开始评论
		pThis->ExectueAProcess(&sendInfo,i);

		//记录个数增加1
		++pThis->m_eachSwitchCounts;
		if (pThis->m_eachSwitchCounts == executeCounts)
		{
			WaitForSingleObject(pThis->m_hSwitchIPEvent,INFINITE);
			pThis->DisconnectVPN();
		}

	}//end for

	//等待完成剩下的任务
	WaitForSingleObject(pThis->m_hFinishEvent,1000*120);
	//过滤成功和失败的appleId
	pThis->FilterAppleId();

	return 0;
}

//ASO核心线程
UINT WINAPI CxhASODlg::vpnSearchKeywordThreadProc(LPVOID lpPtr)
{
	CxhASODlg* pThis = (CxhASODlg*)lpPtr;
	_ASSERTE( pThis != NULL );
	//nSize == ItemCount
	int ItemCount = pThis->m_appleIdLoginAppDownload.GetItemCount();
	int nSize = pThis->m_accountInfoList.size();
	int nModSize = pThis->m_vecModel.size();
	int randIndex = 0;
	int executeCounts = pThis->GetDlgItemInt(IDC_EDIT_EXECUTE_COUNTS);
	
	CString strAccountId;
	CString strPassword;
	CString strUdid;
	CString strMEID;
	CString strIMEI;
	CString strSerialNumber;

	HANDLE hThread = NULL;
	DWORD dwThreadId;
	SENDINFO sendInfo = {0};

	//批量循环登录搜索下载
	for (AccountInfo::iterator ptr = pThis->m_accountInfoList.begin(); ptr!=pThis->m_accountInfoList.end(); ptr++)
	{
		strAccountId = ptr->strAppleId;
		strPassword = ptr->strPassword;
		strUdid = ptr->deviceInfo.strUdid;
		strMEID = ptr->deviceInfo.strMEID;
		strIMEI = ptr->deviceInfo.strIMEI;
		strSerialNumber = ptr->deviceInfo.strSerialNumber;
		int i = ptr->nIndex - 1;

		//清除cookie
		pThis->deleteCookieFile();
		//修改注册表硬件信息
		pThis->ModifyHardwareInfo(FALSE,FALSE);	

		//获取产品id和关键词
		CString strSaleId;
		pThis->m_appId.GetWindowText(strSaleId);

		CString strKeyword;
		pThis->m_hotkeyword.GetWindowText(strKeyword);

		CString strTargetUrl;
		pThis->m_editTargetUrl.GetWindowText(strTargetUrl);
		CString strTmp = _T("https://itunes.apple.com");
		strTargetUrl = strTargetUrl.Right(strTargetUrl.GetLength()-strTmp.GetLength());

		//创建事件名
		CString strEventName;
		LARGE_INTEGER timeRand; 
		QueryPerformanceCounter(&timeRand);
		strEventName.Format(TEXT("EXEPROC%d"),timeRand.LowPart);

		sendInfo.accountInfo.nIndex = i;
		wcscpy(sendInfo.accountInfo.strAppleId,strAccountId.GetBuffer());
		wcscpy(sendInfo.accountInfo.strPassword,strPassword.GetBuffer());
		wcscpy(sendInfo.accountInfo.deviceInfo.strUdid,strUdid.GetBuffer());
		wcscpy(sendInfo.accountInfo.deviceInfo.strMEID,strMEID.GetBuffer());
		wcscpy(sendInfo.accountInfo.deviceInfo.strIMEI,strIMEI.GetBuffer());
		wcscpy(sendInfo.accountInfo.deviceInfo.strSerialNumber,strSerialNumber.GetBuffer());
		wcscpy(sendInfo.strTargetUrl,strTargetUrl.GetBuffer());

		randIndex = rand()%nModSize;
		for (int k=0; k<nModSize; k++)
		{
			if (k == randIndex)
			{
				strcpy(sendInfo.modelInfo.szModel,pThis->m_vecModel.at(k).szModel);
				strcpy(sendInfo.modelInfo.szBuild,pThis->m_vecModel.at(k).szBuild);
				strcpy(sendInfo.modelInfo.szVersion,pThis->m_vecModel.at(k).szVersion);
				strcpy(sendInfo.modelInfo.szHardwarePlatform,pThis->m_vecModel.at(k).szHardwarePlatform);
				sendInfo.modelInfo.innerWidth = pThis->m_vecModel.at(k).innerWidth;
				sendInfo.modelInfo.innerHeight = pThis->m_vecModel.at(k).innerHeight;
				sendInfo.modelInfo.width = pThis->m_vecModel.at(k).width;
				sendInfo.modelInfo.height = pThis->m_vecModel.at(k).height;
			}
		}

		wcscpy(sendInfo.strEventName,strEventName.GetBuffer());
		wcscpy(sendInfo.strHotKeyWord,strKeyword.GetBuffer());
		wcscpy(sendInfo.strSaleId,strSaleId.GetBuffer());
		sendInfo.netType = 0;
		sendInfo.type = pThis->SEARCHAPP;
		sendInfo.bPaidApp = pThis->m_bBuyType;

		//判断线程池中排队的个数是否达到上限
		InterlockedIncrement(&pThis->m_ItemCount);

		pThis->ConnectVPN();

		//开始搜索
		pThis->ExectueAProcess(&sendInfo,i);

		//记录个数增加
		++pThis->m_eachSwitchCounts;
		if (pThis->m_eachSwitchCounts == executeCounts)
		{
			WaitForSingleObject(pThis->m_hSwitchIPEvent,INFINITE);
			pThis->DisconnectVPN();
		}

	}// end for

	//等待完成剩下的任务
	WaitForSingleObject(pThis->m_hFinishEvent,1000*120);
	//过滤成功和失败的appleId
	pThis->FilterAppleId();

	return 0;
}

UINT WINAPI CxhASODlg::dialupDownloadAppThreadProc(LPVOID lpPtr)
{
	CxhASODlg* pThis = (CxhASODlg*)lpPtr;
	_ASSERTE( pThis != NULL );

	int ItemCount = pThis->m_appleIdLoginAppDownload.GetItemCount();
	int nSize = pThis->m_accountInfoList.size();
	int nModSize = pThis->m_vecModel.size();
	int randIndex = 0;
	int executeCounts = pThis->GetDlgItemInt(IDC_EDIT_EXECUTE_COUNTS);

	CString strAppPrice;

	CString strFatherAndMather;
	CString strTeacher;
	CString strBook;
	
	CString strAccountId;
	CString strPassword;
	CString strUdid;
	CString strMEID;
	CString strIMEI;
	CString strSerialNumber;

	HANDLE hThread = NULL;
	DWORD dwThreadId;
	SENDINFO sendInfo = {0};

	//批量循环登录下载
	for (AccountInfo::iterator ptr = pThis->m_accountInfoList.begin(); ptr!=pThis->m_accountInfoList.end(); ptr++)
	{
		strAccountId = ptr->strAppleId;
		strPassword = ptr->strPassword;
		strUdid = ptr->deviceInfo.strUdid;
		strMEID = ptr->deviceInfo.strMEID;
		strIMEI = ptr->deviceInfo.strIMEI;
		strSerialNumber = ptr->deviceInfo.strSerialNumber;
		int i = ptr->nIndex - 1;

		if (pThis->m_bBuyType)
		{
			strFatherAndMather = pThis->m_appleIdLoginAppDownload.GetItemText(i,3);
			strTeacher = pThis->m_appleIdLoginAppDownload.GetItemText(i,4);
			strBook = pThis->m_appleIdLoginAppDownload.GetItemText(i,5);
		}

		//清除cookie
		pThis->deleteCookieFile();
		//修改注册表硬件信息
		pThis->ModifyHardwareInfo(FALSE,FALSE);
		
		//连接vps
		pThis->ConnectVPS();

		//获取产品id和关键词
		CString strSaleId;
		pThis->m_appId.GetWindowText(strSaleId);

		CString strTargetUrl;
		pThis->m_editTargetUrl.GetWindowText(strTargetUrl);
		CString strTmp = _T("https://itunes.apple.com");
		strTargetUrl = strTargetUrl.Right(strTargetUrl.GetLength()-strTmp.GetLength());

		pThis->m_editAppPrice.GetWindowText(strAppPrice);

		//创建事件名
		CString strEventName;
		LARGE_INTEGER timeRand; 
		QueryPerformanceCounter(&timeRand);
		strEventName.Format(TEXT("EXEPROC%d"),timeRand.LowPart);

		sendInfo.accountInfo.nIndex = i;
		wcscpy(sendInfo.accountInfo.strAppleId,strAccountId.GetBuffer());
		wcscpy(sendInfo.accountInfo.strPassword,strPassword.GetBuffer());
		wcscpy(sendInfo.accountInfo.deviceInfo.strUdid,strUdid.GetBuffer());
		wcscpy(sendInfo.accountInfo.deviceInfo.strMEID,strMEID.GetBuffer());
		wcscpy(sendInfo.accountInfo.deviceInfo.strIMEI,strIMEI.GetBuffer());
		wcscpy(sendInfo.accountInfo.deviceInfo.strSerialNumber,strSerialNumber.GetBuffer());
		wcscpy(sendInfo.strTargetUrl,strTargetUrl.GetBuffer());

		randIndex = rand()%nModSize;
		for (int k=0; k<nModSize; k++)
		{
			if (k == randIndex)
			{
				strcpy(sendInfo.modelInfo.szModel,pThis->m_vecModel.at(k).szModel);
				strcpy(sendInfo.modelInfo.szBuild,pThis->m_vecModel.at(k).szBuild);
				strcpy(sendInfo.modelInfo.szVersion,pThis->m_vecModel.at(k).szVersion);
				strcpy(sendInfo.modelInfo.szHardwarePlatform,pThis->m_vecModel.at(k).szHardwarePlatform);
				sendInfo.modelInfo.innerWidth = pThis->m_vecModel.at(k).innerWidth;
				sendInfo.modelInfo.innerHeight = pThis->m_vecModel.at(k).innerHeight;
				sendInfo.modelInfo.width = pThis->m_vecModel.at(k).width;
				sendInfo.modelInfo.height = pThis->m_vecModel.at(k).height;
			}
		}

		wcscpy(sendInfo.strEventName,strEventName.GetBuffer());
		wcscpy(sendInfo.strSaleId,strSaleId.GetBuffer());
		wcscpy(sendInfo.strFatherAndMather,strFatherAndMather.GetBuffer());
		wcscpy(sendInfo.strTeacher,strTeacher.GetBuffer());
		wcscpy(sendInfo.strBook,strBook.GetBuffer());
		wcscpy(sendInfo.strAppPrice,strAppPrice.GetBuffer());
		sendInfo.netType = 0;
		sendInfo.type = pThis->LOGINDOWNLOADAPP;
		sendInfo.bPaidApp = pThis->m_bBuyType;

		//判断线程池中排队的个数是否达到上限
		InterlockedIncrement(&pThis->m_ItemCount);
		if (pThis->m_ItemCount > executeCounts)
		{
			//队列已经满了，等待完成线程出队列
			WaitForSingleObject(pThis->m_hQueueEvent,INFINITE);
			
		}

		//开始登录下载
		pThis->ExectueAProcess(&sendInfo,i);
		
		//切换次数加1
		++pThis->m_eachSwitchCounts;
		//启动任务数已经是最大任务数，等待全部执行完毕，切换ip
		if (pThis->m_eachSwitchCounts == executeCounts)
		{
			WaitForSingleObject(pThis->m_hSwitchIPEvent,INFINITE);

			//断开连接
			pThis->DisconnectVPS();
		}

	}//end for

	//等待完成剩下的任务
	WaitForSingleObject(pThis->m_hFinishEvent,1000*120);
	//过滤成功和失败的appleId
	pThis->FilterAppleId();
	return 0;
}

UINT WINAPI CxhASODlg::dialupCommentThreadProc(LPVOID lpPtr)
{
	CxhASODlg* pThis = (CxhASODlg*)lpPtr;
	_ASSERTE( pThis != NULL );

	int ItemCount = pThis->m_appleIdLoginAppDownload.GetItemCount();
	int nSize = pThis->m_accountInfoList.size();
	int nModSize = pThis->m_vecModel.size();
	int randIndex = 0;
	int executeCounts = pThis->GetDlgItemInt(IDC_EDIT_EXECUTE_COUNTS);

	CString strNickName;
	CString strTitle;
	CString strContext;
	CString strAccountId;
	CString strPassword;
	CString strRate;

	CString strUdid;
	CString strMEID;
	CString strIMEI;
	CString strSerialNumber;

	HANDLE hThread = NULL;
	DWORD dwThreadId;
	SENDINFO sendInfo = {0};

	//批量登录下载评论
	for (AccountInfo::iterator ptr = pThis->m_accountInfoList.begin(); ptr!=pThis->m_accountInfoList.end(); ptr++)
	{
		strAccountId = ptr->strAppleId;
		strPassword = ptr->strPassword;
		strUdid = ptr->deviceInfo.strUdid;
		strMEID = ptr->deviceInfo.strMEID;
		strIMEI = ptr->deviceInfo.strIMEI;
		strSerialNumber = ptr->deviceInfo.strSerialNumber;
		int i = ptr->nIndex - 1;
		strRate = pThis->m_appleIdLoginAppDownload.GetItemText(i,3);
		strNickName = pThis->m_appleIdLoginAppDownload.GetItemText(i,4);
		strTitle = pThis->m_appleIdLoginAppDownload.GetItemText(i,5);
		strContext = pThis->m_appleIdLoginAppDownload.GetItemText(i,6);

		if (strTitle.IsEmpty() || strContext.IsEmpty())
		{
			pThis->m_appleIdLoginAppDownload.SetItemText(i,2,TEXT("无内容"));
			continue;
		}

		//清除cookie
		pThis->deleteCookieFile();
		//修改注册表硬件信息
		pThis->ModifyHardwareInfo(FALSE,FALSE);

		//获取产品id和关键词
		CString strSaleId;
		pThis->m_appId.GetWindowText(strSaleId);

		CString strKeyword;
		pThis->m_hotkeyword.GetWindowText(strKeyword);

		CString strTargetUrl;
		pThis->m_editTargetUrl.GetWindowText(strTargetUrl);
		CString strTmp = _T("https://itunes.apple.com");
		strTargetUrl = strTargetUrl.Right(strTargetUrl.GetLength()-strTmp.GetLength());

		//创建事件名
		CString strEventName;
		LARGE_INTEGER timeRand; 
		QueryPerformanceCounter(&timeRand);
		strEventName.Format(TEXT("EXEPROC%d"),timeRand.LowPart);

		sendInfo.accountInfo.nIndex = i;
		wcscpy(sendInfo.accountInfo.strAppleId,strAccountId.GetBuffer());
		wcscpy(sendInfo.accountInfo.strPassword,strPassword.GetBuffer());
		wcscpy(sendInfo.accountInfo.deviceInfo.strUdid,strUdid.GetBuffer());
		wcscpy(sendInfo.accountInfo.deviceInfo.strMEID,strMEID.GetBuffer());
		wcscpy(sendInfo.accountInfo.deviceInfo.strIMEI,strIMEI.GetBuffer());
		wcscpy(sendInfo.accountInfo.deviceInfo.strSerialNumber,strSerialNumber.GetBuffer());
		wcscpy(sendInfo.strTargetUrl,strTargetUrl.GetBuffer());

		randIndex = rand()%nModSize;
		for (int k=0; k<nModSize; k++)
		{
			if (k == randIndex)
			{
				strcpy(sendInfo.modelInfo.szModel,pThis->m_vecModel.at(k).szModel);
				strcpy(sendInfo.modelInfo.szBuild,pThis->m_vecModel.at(k).szBuild);
				strcpy(sendInfo.modelInfo.szVersion,pThis->m_vecModel.at(k).szVersion);
				strcpy(sendInfo.modelInfo.szHardwarePlatform,pThis->m_vecModel.at(k).szHardwarePlatform);
				sendInfo.modelInfo.innerWidth = pThis->m_vecModel.at(k).innerWidth;
				sendInfo.modelInfo.innerHeight = pThis->m_vecModel.at(k).innerHeight;
				sendInfo.modelInfo.width = pThis->m_vecModel.at(k).width;
				sendInfo.modelInfo.height = pThis->m_vecModel.at(k).height;
			}
		}

		wcscpy(sendInfo.strEventName,strEventName.GetBuffer());
		wcscpy(sendInfo.strRate,strRate.GetBuffer());
		wcscpy(sendInfo.strNickName,strNickName.GetBuffer());
		wcscpy(sendInfo.strTitle,strTitle.GetBuffer());
		wcscpy(sendInfo.strContext,strContext.GetBuffer());
		wcscpy(sendInfo.strHotKeyWord,strKeyword.GetBuffer());
		wcscpy(sendInfo.strSaleId,strSaleId.GetBuffer());
		sendInfo.netType = 0;
		sendInfo.type = pThis->COMMENTAPP;
		sendInfo.bPaidApp = pThis->m_bBuyType;

		//判断线程池中排队的个数是否达到上限
		InterlockedIncrement(&pThis->m_ItemCount);
		//连接vps
		pThis->ConnectVPS();

		//开始评论
		pThis->ExectueAProcess(&sendInfo,i);

		//切换次数加1
		++pThis->m_eachSwitchCounts;
		//启动任务数已经是最大任务数，等待全部执行完毕，切换ip
		if (pThis->m_eachSwitchCounts == executeCounts)
		{
			WaitForSingleObject(pThis->m_hSwitchIPEvent,INFINITE);

			//断开连接
			pThis->DisconnectVPS();
		}
	}//end for

	//等待完成剩下的任务
	WaitForSingleObject(pThis->m_hFinishEvent,1000*120);
	//过滤成功和失败的appleId
	pThis->FilterAppleId();

	return 0;
}

UINT WINAPI CxhASODlg::dialupSearchKeywordThreadProc(LPVOID lpPtr)
{
	CxhASODlg* pThis = (CxhASODlg*)lpPtr;
	_ASSERTE( pThis != NULL );

	int ItemCount = pThis->m_appleIdLoginAppDownload.GetItemCount();
	int nSize = pThis->m_accountInfoList.size();
	int nModSize = pThis->m_vecModel.size();
	int randIndex = 0;
	int executeCounts = pThis->GetDlgItemInt(IDC_EDIT_EXECUTE_COUNTS);
	
	CString strAccountId;
	CString strPassword;
	CString strUdid;
	CString strMEID;
	CString strIMEI;
	CString strSerialNumber;

	HANDLE hThread = NULL;
	DWORD dwThreadId;
	SENDINFO sendInfo = {0};

	//批量循环登录搜索下载
	for (AccountInfo::iterator ptr = pThis->m_accountInfoList.begin(); ptr!=pThis->m_accountInfoList.end(); ptr++)
	{
		strAccountId = ptr->strAppleId;
		strPassword = ptr->strPassword;
		strUdid = ptr->deviceInfo.strUdid;
		strMEID = ptr->deviceInfo.strMEID;
		strIMEI = ptr->deviceInfo.strIMEI;
		strSerialNumber = ptr->deviceInfo.strSerialNumber;
		int i = ptr->nIndex - 1;

		//清除cookie
		pThis->deleteCookieFile();
		//修改注册表硬件信息
		pThis->ModifyHardwareInfo(FALSE,FALSE);


		//获取产品id和关键词
		CString strSaleId;
		pThis->m_appId.GetWindowText(strSaleId);

		CString strKeyword;
		pThis->m_hotkeyword.GetWindowText(strKeyword);

		CString strTargetUrl;
		pThis->m_editTargetUrl.GetWindowText(strTargetUrl);
		CString strTmp = _T("https://itunes.apple.com");
		strTargetUrl = strTargetUrl.Right(strTargetUrl.GetLength()-strTmp.GetLength());

		//创建事件名
		CString strEventName;
		LARGE_INTEGER timeRand; 
		QueryPerformanceCounter(&timeRand);
		strEventName.Format(TEXT("EXEPROC%d"),timeRand.LowPart);

		sendInfo.accountInfo.nIndex = i;
		wcscpy(sendInfo.accountInfo.strAppleId,strAccountId.GetBuffer());
		wcscpy(sendInfo.accountInfo.strPassword,strPassword.GetBuffer());
		wcscpy(sendInfo.accountInfo.deviceInfo.strUdid,strUdid.GetBuffer());
		wcscpy(sendInfo.accountInfo.deviceInfo.strMEID,strMEID.GetBuffer());
		wcscpy(sendInfo.accountInfo.deviceInfo.strIMEI,strIMEI.GetBuffer());
		wcscpy(sendInfo.accountInfo.deviceInfo.strSerialNumber,strSerialNumber.GetBuffer());
		wcscpy(sendInfo.strTargetUrl,strTargetUrl.GetBuffer());

		randIndex = rand()%nModSize;
		for (int k=0; k<nModSize; k++)
		{
			if (k == randIndex)
			{
				strcpy(sendInfo.modelInfo.szModel,pThis->m_vecModel.at(k).szModel);
				strcpy(sendInfo.modelInfo.szBuild,pThis->m_vecModel.at(k).szBuild);
				strcpy(sendInfo.modelInfo.szVersion,pThis->m_vecModel.at(k).szVersion);
				strcpy(sendInfo.modelInfo.szHardwarePlatform,pThis->m_vecModel.at(k).szHardwarePlatform);
				sendInfo.modelInfo.innerWidth = pThis->m_vecModel.at(k).innerWidth;
				sendInfo.modelInfo.innerHeight = pThis->m_vecModel.at(k).innerHeight;
				sendInfo.modelInfo.width = pThis->m_vecModel.at(k).width;
				sendInfo.modelInfo.height = pThis->m_vecModel.at(k).height;
			}
		}

		wcscpy(sendInfo.strEventName,strEventName.GetBuffer());
		wcscpy(sendInfo.strHotKeyWord,strKeyword.GetBuffer());
		wcscpy(sendInfo.strSaleId,strSaleId.GetBuffer());
		sendInfo.netType = 0;
		sendInfo.type = pThis->SEARCHAPP;
		sendInfo.bPaidApp = pThis->m_bBuyType;

		//判断线程池中排队的个数是否达到上限
		InterlockedIncrement(&pThis->m_ItemCount);

		//连接vps
		pThis->ConnectVPS();

		//开始搜索
		pThis->ExectueAProcess(&sendInfo,i);

		//切换次数加1
		++pThis->m_eachSwitchCounts;
		//启动任务数已经是最大任务数，等待全部执行完毕，切换ip
		if (pThis->m_eachSwitchCounts == executeCounts)
		{
			WaitForSingleObject(pThis->m_hSwitchIPEvent,INFINITE);

			//断开连接
			pThis->DisconnectVPS();
		}


	}//end for

	//等待完成剩下的任务
	WaitForSingleObject(pThis->m_hFinishEvent,1000*120);
	//过滤成功和失败的appleId
	pThis->FilterAppleId();
	return 0;
}

void CxhASODlg::ConnectVPN(void)
{
	HANDLE hThread = NULL;
	DWORD dwThreadId = 0;

	//vpn链接
	if (!m_bVpnConnect)
	{
		hThread = CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)loginVpnUseRasDialCmdThreadProc,this,0,&dwThreadId);
		if (!hThread)
		{
			m_netConnectStatus.SetWindowText(TEXT("链接失败"));
		}
		else
		{
			WaitForSingleObject(hThread,1000*10);
		}
	}
	
	return;
}
void CxhASODlg::DisconnectVPN(void)
{
	//vpn断开
	if (m_bVpnConnect)
	{
		//检查链接状态
		if (checkConnectStatus(m_hRasConn))
		{
			DisconnecDialtVPN(m_hRasConn);
			m_hRasConn = NULL;
			m_netConnectStatus.SetWindowText(TEXT("vpn已断开"));
			m_bVpnConnect = FALSE;
				
			 m_eachSwitchCounts = 0;

			::Sleep(8000);
		}
		else
		{
			m_hRasConn = NULL;
			m_netConnectStatus.SetWindowText(TEXT("vpn未连接"));
			m_bLoginVpn = TRUE;
			m_bVpnConnect = FALSE;
		}
	}
		
	return;
}
void CxhASODlg::ConnectVPS(void)
{
	HANDLE hThread = NULL;
	DWORD dwThreadId = 0;

	//宽带连接
	if (!m_bIsAdslConnect)
	{
		hThread = CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)dialupLoginThreadProc,this,0,&dwThreadId);
		if (!hThread)
		{
			m_netConnectStatus.SetWindowText(TEXT("链接失败"));
		}
		else
		{
			WaitForSingleObject(hThread,INFINITE);
		}
	}
	return;

}
void CxhASODlg::DisconnectVPS(void)
{
	//断开宽带连接
	if (m_bIsAdslConnect)
	{
		 CAutoDial::GetInstance()->Disconnect();
		 m_bIsAdslConnect = FALSE;
		 m_eachSwitchCounts = 0;

		 Sleep(1000*3);
       
	}
	
	return;
}

void CxhASODlg::OnBnClickedBtnStartComment()
{
	// TODO: Add your control notification handler code here
	CString strAppId;
	m_appId.GetWindowText(strAppId);
	if (strAppId.IsEmpty())
	{
		MessageBox(TEXT("app产品Id不能为空"));
		return;
	}

	int processCheck = ((CButton*)GetDlgItem(IDC_RADIO_PROCESS))->GetCheck();
	int threadCheck = ((CButton*)GetDlgItem(IDC_RADIO_THREAD))->GetCheck();


	int vpnCheck = ((CButton*)GetDlgItem(IDC_RADIO_VPN))->GetCheck();
	int adslCheck = ((CButton*)GetDlgItem(IDC_RADIO_DIAL_UP))->GetCheck();
	int agentCheck = ((CButton*)GetDlgItem(IDC_RADIO_AGENT))->GetCheck();

	int localCheck = ((CButton*)GetDlgItem(IDC_RADIO_LOCAL_LOAD))->GetCheck();
	int serverCheck = ((CButton*)GetDlgItem(IDC_RADIO_SERVER_LOAD))->GetCheck();

	
	DWORD dwThreadId = 0;
	HANDLE hThread = NULL;
	
	if (processCheck == BST_CHECKED)
	{
		if (localCheck == BST_CHECKED)//本地
		{
			if (vpnCheck == BST_CHECKED)//vpn 链接选中
			{
				if (!m_bLoginVpn)
				{
					MessageBox(TEXT("请先登录VPN"));
					return;
				}
				hThread = CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)vpnCommentThreadProc,this,0,&dwThreadId);
				if (!hThread)
				{
					return;
				}
			}
			else if (adslCheck == BST_CHECKED)//宽带连接选中
			{
				if (!m_bLoginAdsl)
				{
					MessageBox(TEXT("请先登录宽带"));
					return;
				}
				hThread = CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)dialupCommentThreadProc,this,0,&dwThreadId);
				if (!hThread)
				{
					return;
				}
			}
			else//本地正常链接
			{
				hThread = CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)normalnetCommentThreadProc,this,0,&dwThreadId);
				if (!hThread)
				{
					return;
				}
			}
		}
		else if (serverCheck == BST_CHECKED)
		{
			SetTimer(TIME_SERVER_NOTIFY,1000,NULL);
		}
		else
		{
		}
	}
	else if (threadCheck == BST_CHECKED)
	{
		if (!getFunInterface())
		{
			MessageBox(TEXT("模块加载失败"));
			return;
		}
		hThread = CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)executeCommentWorkItem,this,0,&dwThreadId);
	}
	else
	{
		MessageBox(TEXT("请选择执行类型"));
		return;
	}

}

void CxhASODlg::OnBnClickedBtnStartSearch()
{
	// TODO: Add your control notification handler code here
	CString strKeyword;
	m_hotkeyword.GetWindowText(strKeyword);
	if (strKeyword.IsEmpty())
	{
		MessageBox(TEXT("请输入和app相关的关键词"));
		return;
	}

	int processCheck = ((CButton*)GetDlgItem(IDC_RADIO_PROCESS))->GetCheck();//执行类型为进程
	int threadCheck = ((CButton*)GetDlgItem(IDC_RADIO_THREAD))->GetCheck();  //执行类型为线程

	int vpnCheck = ((CButton*)GetDlgItem(IDC_RADIO_VPN))->GetCheck();
	int adslCheck = ((CButton*)GetDlgItem(IDC_RADIO_DIAL_UP))->GetCheck();
	int agentCheck = ((CButton*)GetDlgItem(IDC_RADIO_AGENT))->GetCheck();

	int localCheck = ((CButton*)GetDlgItem(IDC_RADIO_LOCAL_LOAD))->GetCheck();
	int serverCheck = ((CButton*)GetDlgItem(IDC_RADIO_SERVER_LOAD))->GetCheck();

	DWORD dwThreadId = 0;
	HANDLE hThread = NULL;
	
	if (processCheck == BST_CHECKED)
	{
		if (localCheck == BST_CHECKED)//本地
		{
			if (vpnCheck == BST_CHECKED)//vpn链接选中
			{
				if (!m_bLoginVpn)
				{
					MessageBox(TEXT("请先登录VPN"));
					return;
				}
				hThread = CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)vpnSearchKeywordThreadProc,this,0,&dwThreadId);
				if (!hThread)
				{
					return;
				}
			}
			else if (adslCheck == BST_CHECKED)// 宽带连接选中
			{
				if (!m_bLoginAdsl)
				{
					MessageBox(TEXT("请先登录宽带"));
					return;
				}
				hThread = CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)dialupSearchKeywordThreadProc,this,0,&dwThreadId);
				if (!hThread)
				{
					return;
				}
			}
			else//本地正常流程
			{
				hThread = CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)normalnetSearchKeywordThreadProc,this,0,&dwThreadId);
				if (!hThread)
				{
					return;
				}
			}
		}
		else if (serverCheck == BST_CHECKED)
		{
			SetTimer(TIME_SERVER_NOTIFY,1000,NULL);
		}
		else
		{
		}
	}
	else if (threadCheck == BST_CHECKED)
	{
		if (!getFunInterface())
		{
			MessageBox(TEXT("模块加载失败"));
			return;
		}
		hThread = CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)executeSearchWorkItem,this,0,&dwThreadId);
	}
	else
	{
		MessageBox(TEXT("请选择执行类型"));
		return;
	}

}

void CxhASODlg::OnBnClickedBtnDownload()
{
	// TODO: Add your control notification handler code here
	CString strAppId;
	m_appId.GetWindowText(strAppId);
	if (strAppId.IsEmpty())
	{
		MessageBox(TEXT("app产品Id不能为空"));
		return;
	}

	int processCheck = ((CButton*)GetDlgItem(IDC_RADIO_PROCESS))->GetCheck();
	int threadCheck = ((CButton*)GetDlgItem(IDC_RADIO_THREAD))->GetCheck();


	int vpnCheck = ((CButton*)GetDlgItem(IDC_RADIO_VPN))->GetCheck();
	int adslCheck = ((CButton*)GetDlgItem(IDC_RADIO_DIAL_UP))->GetCheck();
	int agentCheck = ((CButton*)GetDlgItem(IDC_RADIO_AGENT))->GetCheck();

	int localCheck = ((CButton*)GetDlgItem(IDC_RADIO_LOCAL_LOAD))->GetCheck();
	int serverCheck = ((CButton*)GetDlgItem(IDC_RADIO_SERVER_LOAD))->GetCheck();


	DWORD dwThreadId = 0;
	HANDLE hThread = NULL;
	
	if (processCheck == BST_CHECKED)
	{
		if (localCheck == BST_CHECKED)//本地
		{
			if (vpnCheck == BST_CHECKED)//vpn 链接
			{
				if (!m_bLoginVpn)
				{
					MessageBox(TEXT("请先登录VPN"));
					return;
				}
				hThread = CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)vpnDownloadAppThreadProc,this,0,&dwThreadId);
				if (!hThread)
				{
					return;
				}
			}
			else if (adslCheck == BST_CHECKED)//宽带连接
			{
				if (!m_bLoginAdsl)
				{
					MessageBox(TEXT("请先登录宽带"));
					return;
				}
				hThread = CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)dialupDownloadAppThreadProc,this,0,&dwThreadId);
				if (!hThread)
				{
					return;
				}
			}
			else//正常流程
			{
				hThread = CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)normalnetDownloadAppThreadProc,this,0,&dwThreadId);
				if (!hThread)
				{
					return;
				}
			}
		}
		else if (serverCheck == BST_CHECKED)
		{
			SetTimer(TIME_SERVER_NOTIFY,1000,NULL);
		}
		else
		{
		}
	}
	else if (threadCheck == BST_CHECKED)
	{
		if (!getFunInterface())
		{
			MessageBox(TEXT("模块加载失败"));
			return;
		}
		hThread = CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)executeDownloadWorkItem,this,0,&dwThreadId);
	}
	else
	{
		MessageBox(TEXT("请选择执行类型"));
		return;
	}

}

void CxhASODlg::OnClose()
{
	// TODO: Add your message handler code here and/or call default
	RestoreHardwareInfo();
	CDialog::OnClose();
}

void CxhASODlg::OnBnClickedRadioVpn()
{
	// TODO: Add your control notification handler code here
	m_menu.EnableMenuItem(ID_FILE_LOADVPN,MF_ENABLED);
	m_menu.EnableMenuItem(ID_VPN_CONNECT_CMD,MF_ENABLED);
	m_menu.EnableMenuItem(ID_PPPOE_CONNECT_CMD,MF_DISABLED);
	

    ((CButton*)GetDlgItem(IDC_RADIO_VPN))->SetCheck(TRUE);
	((CButton*)GetDlgItem(IDC_RADIO_DIAL_UP))->SetCheck(FALSE);
	((CButton*)GetDlgItem(IDC_RADIO_AGENT))->SetCheck(FALSE);
}

void CxhASODlg::OnBnClickedRadioDialUp()
{
	// TODO: Add your control notification handler code here
	m_menu.EnableMenuItem(ID_FILE_LOADVPN,MF_DISABLED);
	m_menu.EnableMenuItem(ID_VPN_CONNECT_CMD,MF_DISABLED);
	m_menu.EnableMenuItem(ID_PPPOE_CONNECT_CMD,MF_ENABLED);
	

    ((CButton*)GetDlgItem(IDC_RADIO_VPN))->SetCheck(FALSE);
	((CButton*)GetDlgItem(IDC_RADIO_DIAL_UP))->SetCheck(TRUE);
	((CButton*)GetDlgItem(IDC_RADIO_AGENT))->SetCheck(FALSE);
}

void CxhASODlg::OnBnClickedRadioAgent()
{
	// TODO: Add your control notification handler code here
	m_menu.EnableMenuItem(ID_FILE_LOADVPN,MF_DISABLED);
	m_menu.EnableMenuItem(ID_VPN_CONNECT_CMD,MF_DISABLED);
	m_menu.EnableMenuItem(ID_PPPOE_CONNECT_CMD,MF_DISABLED);
	

    ((CButton*)GetDlgItem(IDC_RADIO_VPN))->SetCheck(FALSE);
	((CButton*)GetDlgItem(IDC_RADIO_DIAL_UP))->SetCheck(FALSE);
	((CButton*)GetDlgItem(IDC_RADIO_AGENT))->SetCheck(TRUE);
}

void CxhASODlg::OnBnClickedRadioLocalLoad()
{
	// TODO: Add your control notification handler code here
	m_menu.EnableMenuItem(ID_FILE_LOADAPPLEID,MF_ENABLED);
	m_menu.EnableMenuItem(ID_COMMNETS,MF_ENABLED);

	((CButton*)GetDlgItem(IDC_RADIO_LOCAL_LOAD))->SetCheck(TRUE);
	((CButton*)GetDlgItem(IDC_RADIO_SERVER_LOAD))->SetCheck(FALSE);

	GetDlgItem(IDC_RADIO_VPN)->EnableWindow(TRUE);
	GetDlgItem(IDC_RADIO_DIAL_UP)->EnableWindow(TRUE);
	GetDlgItem(IDC_RADIO_AGENT)->EnableWindow(TRUE);

	GetDlgItem(IDC_BTN_START_SEARCH)->EnableWindow(TRUE);
	GetDlgItem(IDC_BTN_START_COMMENT)->EnableWindow(TRUE);
	GetDlgItem(IDC_BTN_DOWNLOAD)->EnableWindow(TRUE);

	GetDlgItem(IDC_BTN_SERVER_START)->EnableWindow(FALSE);
	GetDlgItem(IDC_BTN_SERVER_STOP)->EnableWindow(FALSE);
}

void CxhASODlg::OnBnClickedRadioServerLoad()
{
	// TODO: Add your control notification handler code here
	m_menu.EnableMenuItem(ID_FILE_LOADAPPLEID,MF_DISABLED);
	m_menu.EnableMenuItem(ID_COMMNETS,MF_DISABLED);

	((CButton*)GetDlgItem(IDC_RADIO_LOCAL_LOAD))->SetCheck(FALSE);
	((CButton*)GetDlgItem(IDC_RADIO_SERVER_LOAD))->SetCheck(TRUE);

	GetDlgItem(IDC_RADIO_VPN)->EnableWindow(FALSE);
	GetDlgItem(IDC_RADIO_DIAL_UP)->EnableWindow(FALSE);
	GetDlgItem(IDC_RADIO_AGENT)->EnableWindow(FALSE);


	GetDlgItem(IDC_BTN_START_SEARCH)->EnableWindow(FALSE);
	GetDlgItem(IDC_BTN_START_COMMENT)->EnableWindow(FALSE);
	GetDlgItem(IDC_BTN_DOWNLOAD)->EnableWindow(FALSE);

	GetDlgItem(IDC_BTN_SERVER_START)->EnableWindow(TRUE);
	GetDlgItem(IDC_BTN_SERVER_STOP)->EnableWindow(FALSE);

}

void CxhASODlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: Add your message handler code here and/or call default
	CString strShowTime;
	switch(nIDEvent)
	{
	case SECOND_TIME_NOTIFY:
		{
			m_totalTime.misecond = m_totalTime.misecond + 1;
			if (m_totalTime.misecond == 100)
			{
				m_totalTime.misecond = 0;
				m_totalTime.second = m_totalTime.second + 1;
			}
			if (m_totalTime.second == 60)
			{
				m_totalTime.second = 0;
				m_totalTime.miniute = m_totalTime.miniute + 1;
			}
			if (m_totalTime.miniute == 60)
			{
				m_totalTime.miniute = 0;
				m_totalTime.hour = m_totalTime.hour + 1;
			}
			if (m_totalTime.second<10 && m_totalTime.miniute<10 && m_totalTime.hour < 10)
			{
				strShowTime.Format(TEXT("0%d:0%d:0%d:%d"),m_totalTime.hour,m_totalTime.miniute,m_totalTime.second,m_totalTime.misecond);
			}
			else if (m_totalTime.second>=10 && m_totalTime.miniute<10 && m_totalTime.hour < 10)
			{
				strShowTime.Format(TEXT("0%d:0%d:%d:%d"),m_totalTime.hour,m_totalTime.miniute,m_totalTime.second,m_totalTime.misecond);
			}
			else if (m_totalTime.second>=10 && m_totalTime.miniute>=10 && m_totalTime.hour < 10)
			{
				strShowTime.Format(TEXT("0%d:%d:%d:%d"),m_totalTime.hour,m_totalTime.miniute,m_totalTime.second,m_totalTime.misecond);
			}
			else if (m_totalTime.second>=10 && m_totalTime.miniute>=10 && m_totalTime.hour >= 10)
			{
				strShowTime.Format(TEXT("%d:%d:%d:%d"),m_totalTime.hour,m_totalTime.miniute,m_totalTime.second,m_totalTime.misecond);
			}
			else if (m_totalTime.second<10 && m_totalTime.miniute>=10 && m_totalTime.hour >= 10)
			{
				strShowTime.Format(TEXT("%d:%d:0%d:%d"),m_totalTime.hour,m_totalTime.miniute,m_totalTime.second,m_totalTime.misecond);
			}
		}break;
	case KEEPALIVE_HEARTBEAT_NOTIFY:
		{
		}break;
	case TIME_SERVER_NOTIFY:
		{
			m_accountIndex = m_accountIndex + 1;
		}break;
	default:
		break;
	}

	CDialog::OnTimer(nIDEvent);
}

void CxhASODlg::OnBnClickedBtnServerStart()
{
	// TODO: Add your control notification handler code here
	/*m_bHeartBeatRunning = TRUE;
	DWORD dwThreadId;
	HANDLE hThread = ::CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)heartBeatThreadProc,this,0,&dwThreadId);
	if (!hThread)
	{
		return;
	}*/
	GetDlgItem(IDC_BTN_SERVER_START)->EnableWindow(FALSE);
	GetDlgItem(IDC_BTN_SERVER_STOP)->EnableWindow(TRUE);
}

void CxhASODlg::OnBnClickedBtnServerStop()
{
	// TODO: Add your control notification handler code here
	m_bHeartBeatRunning = FALSE;

	GetDlgItem(IDC_BTN_SERVER_START)->EnableWindow(TRUE);
	GetDlgItem(IDC_BTN_SERVER_STOP)->EnableWindow(FALSE);

}

void CxhASODlg::OnBnClickedRadioFree()
{
	// TODO: Add your control notification handler code here
	m_bBuyType = FALSE;
	((CButton*)GetDlgItem(IDC_EDIT_PRICE))->EnableWindow(FALSE);

	//删除列
	if (!m_bDefaultFree)
	{
		m_appleIdLoginAppDownload.DeleteColumn(5);
		m_appleIdLoginAppDownload.DeleteColumn(4);
		m_appleIdLoginAppDownload.DeleteColumn(3);

		m_appleIdLoginAppDownload.InsertColumn(3,TEXT("星级"),LVCFMT_CENTER,40);
		m_appleIdLoginAppDownload.InsertColumn(4,TEXT("昵称"),LVCFMT_CENTER,100);
		m_appleIdLoginAppDownload.InsertColumn(5,TEXT("标题"),LVCFMT_CENTER,160);
		m_appleIdLoginAppDownload.InsertColumn(6,TEXT("内容"),LVCFMT_CENTER,350);
	}

	m_bDefaultFree = TRUE;
	
}

void CxhASODlg::OnBnClickedRadioPaid()
{
	// TODO: Add your control notification handler code here
	m_bBuyType = TRUE;

	((CButton*)GetDlgItem(IDC_EDIT_PRICE))->EnableWindow(TRUE);

	if (m_bDefaultFree)
	{
		//删除入列
		m_appleIdLoginAppDownload.DeleteColumn(6);
		m_appleIdLoginAppDownload.DeleteColumn(5);
		m_appleIdLoginAppDownload.DeleteColumn(4);
		m_appleIdLoginAppDownload.DeleteColumn(3);

		//插入新列
		m_appleIdLoginAppDownload.InsertColumn(3,TEXT("父母"),LVCFMT_CENTER,220);
		m_appleIdLoginAppDownload.InsertColumn(4,TEXT("老师"),LVCFMT_CENTER,220);
		m_appleIdLoginAppDownload.InsertColumn(5,TEXT("一本书"),LVCFMT_CENTER,220);
	}
	m_bDefaultFree = FALSE;
}

void CxhASODlg::OnRegisterAppleid()
{
	// TODO: Add your command handler code here
	MessageBox(TEXT("注册apple Id"));
}

BOOL CxhASODlg::OnCopyData(CWnd* pWnd, COPYDATASTRUCT* pCopyDataStruct)
{
	// TODO: Add your message handler code here and/or call default
	int appleIdIndex = 0;
	int ItemCount = 0;
	CString strShow;
	
	int vpnCheck = ((CButton*)GetDlgItem(IDC_RADIO_VPN))->GetCheck();
	int adslCheck = ((CButton*)GetDlgItem(IDC_RADIO_DIAL_UP))->GetCheck();
	int agentCheck = ((CButton*)GetDlgItem(IDC_RADIO_AGENT))->GetCheck();

	int executeCounts = GetDlgItemInt(IDC_EDIT_EXECUTE_COUNTS);
	PLOGINFO pLogInfo = (PLOGINFO)(pCopyDataStruct->lpData);

	//标记完成的任务进程
	for (MONTASKLIST::iterator ptr = m_monTaskList.begin(); ptr != m_monTaskList.end();ptr++)
	{
		if (ptr->nIndex == pLogInfo->nIndex)
		{
			ptr->finishTag = 1;
			break;
		}
	}
	//完成任务计数器加1
	m_finishTaskCount ++;
	//分类信息
	switch(pLogInfo->result)
	{
	case LOGIN_SUCCESS:
		{
			appleIdIndex = pLogInfo->nIndex + 1;
			m_vecLoginSucessAppleIdIndex.push_back(appleIdIndex);
			m_successCounts ++;
		}break;
	case LOGIN_FAILED:
		{
			appleIdIndex = pLogInfo->nIndex + 1;
			m_vecLoginFailedAppleIdIndex.push_back(appleIdIndex);
			m_failedCounts ++;
		}break;
	case DOWNLOADFAILED:
		{
			appleIdIndex = pLogInfo->nIndex + 1;
			m_vecExecuteFailedAppleIdIndex.push_back(appleIdIndex);
			m_failedCounts ++;
		}break;
	case SEARCHFAILED:
		{
			appleIdIndex = pLogInfo->nIndex + 1;
			m_vecExecuteFailedAppleIdIndex.push_back(appleIdIndex);
			m_failedCounts ++;
		}break;
	case COMMENTFAILED:
		{
			appleIdIndex = pLogInfo->nIndex + 1;
			m_vecExecuteFailedAppleIdIndex.push_back(appleIdIndex);
			m_failedCounts ++;
		}
	case DOWNLOADSUCCESS:
		{
			appleIdIndex = pLogInfo->nIndex + 1;
			m_vecSuccessAppleIdIndex.push_back(appleIdIndex);
			m_successCounts ++;

		}break;
	case SEARCHSUCCESS:
		{
			appleIdIndex = pLogInfo->nIndex + 1;
			m_vecSuccessAppleIdIndex.push_back(appleIdIndex);
			m_successCounts ++;
		}break;
	case COMMENTSUCCESS:
		{
			appleIdIndex = pLogInfo->nIndex + 1;
			m_vecSuccessAppleIdIndex.push_back(appleIdIndex);
			m_successCounts ++;
		}break;
	case ACTIVEAPPFAILED:
		{
			appleIdIndex = pLogInfo->nIndex + 1;
			m_vecExecuteFailedAppleIdIndex.push_back(appleIdIndex);
			m_failedCounts ++;
		}break;
	case GSA_FAILED:
		{
			appleIdIndex = pLogInfo->nIndex + 1;
			m_vecExecuteFailedAppleIdIndex.push_back(appleIdIndex);
			m_failedCounts ++;
		}break;
	case EXECUTE_TIME_OUT:
		{
			appleIdIndex = pLogInfo->nIndex + 1;
			m_vecExecuteFailedAppleIdIndex.push_back(appleIdIndex);
			m_failedCounts ++;
		}break;
	case ACCOUNT_LOCKED:
		{
			appleIdIndex = pLogInfo->nIndex + 1;
			m_vecAccountLockedAppleIdIndex.push_back(appleIdIndex);
			m_failedCounts ++;
		}break;
	case PASSWORD_ERROR:
		{
			appleIdIndex = pLogInfo->nIndex + 1;
			m_vecPasswordErrorAppleIdIndex.push_back(appleIdIndex);
			m_failedCounts ++;
		}break;
	case ACCOUNT_FAULT:
		{
			appleIdIndex = pLogInfo->nIndex + 1;
			m_vecAccountFaultAppleIdIndex.push_back(appleIdIndex);
			m_failedCounts ++;
		}break;
	case ACCOUNT_STOP:
		{
			appleIdIndex = pLogInfo->nIndex + 1;
			m_vecAccountStopAppleIdIndex.push_back(appleIdIndex);
			m_failedCounts ++;

		}break;
	default:
		{
			appleIdIndex = pLogInfo->nIndex + 1;
			m_vecExecuteFailedAppleIdIndex.push_back(appleIdIndex);
			m_failedCounts ++;
		}
		break;
	}
	
	//显示log数据
	m_appleIdLoginAppDownload.SetItemText(pLogInfo->nIndex,2,pLogInfo->strItemLog);

	strShow.Format(TEXT("成功:%d,失败:%d"),m_successCounts,m_failedCounts);

	m_showResult.SetWindowText(strShow);

	//保证原子操作执行完一个后减一
	InterlockedDecrement(&m_ItemCount);
	if (m_ItemCount < executeCounts)
	{
		if ( ( vpnCheck || adslCheck || agentCheck) != BST_CHECKED)
		{
			SetEvent(m_hQueueEvent);
		}
	}

	//最大任务数全部执行完毕,通知切换IP
	if (m_ItemCount <= 0)
	{
		SetEvent(m_hSwitchIPEvent);
	}

	//到完成数达到最大进程数时，标记完成信息，通知检测线程完成从链表中移除完成任务进程
	if (m_finishTaskCount == executeCounts)
	{
		//通知监控其它任务进程
		SetEvent(m_hMonitorEvent);

		//计数器重新归0
		m_finishTaskCount = 0;
	}

	//全部完成操作
	ItemCount = m_appleIdLoginAppDownload.GetItemCount();
	if (ItemCount == pLogInfo->nIndex+1)
	{
		SetEvent(m_hFinishEvent);

		//通知监控线程，完成最后的任务进程移除工作
		SetEvent(m_hMonitorEvent);
	}

	return CDialog::OnCopyData(pWnd, pCopyDataStruct);
}

void CxhASODlg::OnBnClickedRadioProcess()
{
	// TODO: Add your control notification handler code here
	((CStatic*)GetDlgItem(IDC_STATIC_EXECUTE_COUNTS))->SetWindowText(TEXT("进程数:"));
}

void CxhASODlg::OnBnClickedRadioThread()
{
	// TODO: Add your control notification handler code here
	((CStatic*)GetDlgItem(IDC_STATIC_EXECUTE_COUNTS))->SetWindowText(TEXT("线程数:"));
}
