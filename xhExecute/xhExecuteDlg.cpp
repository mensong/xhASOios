
// xhExecuteDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "xhExecute.h"
#include "xhExecuteDlg.h"
#include "cryptAlgorithm.h"
#include "statusData.h"
#include "memoryshared.h"
#include <iostream>
#include <string>
#include <Winsock2.h>
#include <iphlpapi.h>
#include<locale.h> 
using namespace std;
#ifdef _DEBUG
#define new DEBUG_NEW
#endif
#define     BUFFER_SIZE        7168
#define     MAX_TITLE          64
#define     MAX_BODY           512
#define     MAX_KEY_WORD       64  
#define		MAINTITLE			TEXT("苹果商店APP优化工具")
typedef BOOL (* PINITIALIZEMODULE)(LPVOID strPath);
typedef void (* PUNINITIALIZEMODULE)();
PINITIALIZEMODULE pInitialModule = NULL;
PUNINITIALIZEMODULE pUninitialModule = NULL;

typedef VOID (*PXHINITCRITICALSECTION)();
typedef VOID (*PXHDELETECRITICALSECTION)();
typedef LPVOID (*PXHCREATENETWORKOBJECT)();
typedef VOID (*PXHRELEASENEWTWORKOBJECT)(LPVOID lpNetwork);

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

PXHINITCRITICALSECTION pInitCriticalSection = NULL;
PXHDELETECRITICALSECTION pDeleteCriticalSection = NULL;
PXHCREATENETWORKOBJECT pCreateNetworkObject = NULL;
PXHRELEASENEWTWORKOBJECT pReleaseNetworkObject = NULL;
PXHGSA_APPLE_NETWORKVERIFIER pGSA_Apple_NetworkVerifier = NULL;
PXHLOGINAPPSTORE pLoginAppStore = NULL; 
PXHDOWNLOADAPP pDownloadApp = NULL;
PXHCOMMENTAPP pCommentApp = NULL;
PXHSEARCHKEYHOTWORD pSearchKeyHotWord = NULL;
PXHGETAPPEXTVRSIDVALUE pGetAppExtVrsIdValue = NULL;
PXHBUYCONFIRMACTIVEAPP pBuyConfirmActiveApp = NULL;
PXHONLYSEARCHKEYWORD pOnlySearchKeyword = NULL;

int    g_status = 0;

// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// CxhExecuteDlg 对话框




CxhExecuteDlg::CxhExecuteDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CxhExecuteDlg::IDD, pParent)
{
	m_hPipeRead = NULL;
	m_hPipeWrite = NULL;
	m_hEvent = NULL;
}

void CxhExecuteDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CxhExecuteDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


// CxhExecuteDlg 消息处理程序

BOOL CxhExecuteDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	//SetIcon(m_hIcon, TRUE);			// 设置大图标
	//SetIcon(m_hIcon, FALSE);		// 设置小图标

	m_hIcon = AfxGetApp()->LoadIcon(IDI_ICON1);
	SetIcon(m_hIcon, TRUE);
	SetIcon(m_hIcon, FALSE);

	// TODO: 在此添加额外的初始化代码
	int ret = 0;
	SENDINFO recvInfo = {0};
	LOGINFO logInfo = {0};
	try
	{
		if (__argc != 1)
		{
			::ExitProcess(0);
		}

		AfxMessageBox(_T("123"));

		memoryshared ms;
		ms.Open(__wargv[0]);
		if ( 1 == ms.Read((char*)&recvInfo, sizeof(SENDINFO)) )
		{
			//打开事件,通知管理程序执行进程已经读取到数据
			m_hEvent = OpenEvent(EVENT_MODIFY_STATE,FALSE,recvInfo.strEventName);
			//设置事件有信号
			SetEvent(m_hEvent);

			if (m_hEvent)
			{
				CloseHandle(m_hEvent);
			}
			//加载模块获取接口
			if (!getFunInterface())
			{
				::ExitProcess(0);
			}
			
			//开始执行核心程序
			DWORD dwThreadId = 0;
			HANDLE hThread = CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)InterfaceFunction,(LPVOID)&recvInfo,0,&dwThreadId);
			DWORD result = ::WaitForSingleObject(hThread,1000*300);
			if (result == WAIT_TIMEOUT)
			{
				g_status = EXECUTE_TIME_OUT;
			}
			switch(g_status)
			{
			case LOGIN_SUCCESS:
				{
					logInfo.nIndex = recvInfo.accountInfo.nIndex;
					wcscpy(logInfo.strItemLog,TEXT("登录成功"));
					logInfo.result = LOGIN_SUCCESS;
				}break;
			case LOGIN_FAILED:
				{
					logInfo.nIndex = recvInfo.accountInfo.nIndex;
					wcscpy(logInfo.strItemLog,TEXT("登录失败"));
					logInfo.result = LOGIN_FAILED;
				}break;
			case DOWNLOADFAILED:
				{
					logInfo.nIndex = recvInfo.accountInfo.nIndex;
					wcscpy(logInfo.strItemLog,TEXT("下载失败"));
					logInfo.result = DOWNLOADFAILED;
				}break;
			case SEARCHFAILED:
				{
					logInfo.nIndex = recvInfo.accountInfo.nIndex;
					wcscpy(logInfo.strItemLog,TEXT("搜索失败"));
					logInfo.result = SEARCHFAILED;
				}break;
			case COMMENTFAILED:
				{
					logInfo.nIndex = recvInfo.accountInfo.nIndex;
					wcscpy(logInfo.strItemLog,TEXT("评论失败"));
					logInfo.result = COMMENTFAILED;
				}
			case DOWNLOADSUCCESS:
				{
					logInfo.nIndex = recvInfo.accountInfo.nIndex;
					wcscpy(logInfo.strItemLog,TEXT("下载完成"));
					logInfo.result = DOWNLOADSUCCESS;
				}break;
			case SEARCHSUCCESS:
				{
					logInfo.nIndex = recvInfo.accountInfo.nIndex;
					wcscpy(logInfo.strItemLog,TEXT("搜索完成"));
					logInfo.result = SEARCHSUCCESS;
				}break;
			case COMMENTSUCCESS:
				{
					logInfo.nIndex = recvInfo.accountInfo.nIndex;
					wcscpy(logInfo.strItemLog,TEXT("评论完成"));
					logInfo.result = COMMENTSUCCESS;
				}break;
			case ACTIVEAPPFAILED:
				{
					logInfo.nIndex = recvInfo.accountInfo.nIndex;
					wcscpy(logInfo.strItemLog,TEXT("激活失败"));
					logInfo.result = ACTIVEAPPFAILED;
				}break;
			case GSA_FAILED:
				{
					logInfo.nIndex = recvInfo.accountInfo.nIndex;
					wcscpy(logInfo.strItemLog,TEXT("gsa失败"));
					logInfo.result = GSA_FAILED;
				}break;
			case EXECUTE_TIME_OUT:
				{
					logInfo.nIndex = recvInfo.accountInfo.nIndex;
					wcscpy(logInfo.strItemLog,TEXT("执行超时"));
					logInfo.result = EXECUTE_TIME_OUT;

				}break;
			case PASSWORD_ERROR:
				{
					logInfo.nIndex = recvInfo.accountInfo.nIndex;
					wcscpy(logInfo.strItemLog,TEXT("密码错误"));
					logInfo.result = PASSWORD_ERROR;
				}break;
			case ACCOUNT_LOCKED:
				{
					logInfo.nIndex = recvInfo.accountInfo.nIndex;
					wcscpy(logInfo.strItemLog,TEXT("账号被锁"));
					logInfo.result = ACCOUNT_LOCKED;
				}break;
			case ACCOUNT_DISABLED:
				{
					logInfo.nIndex = recvInfo.accountInfo.nIndex;
					wcscpy(logInfo.strItemLog,TEXT("账号禁用"));
					logInfo.result = ACCOUNT_DISABLED;
				}break;
			case ACCOUNT_FAULT:
				{
					logInfo.nIndex = recvInfo.accountInfo.nIndex;
					wcscpy(logInfo.strItemLog,TEXT("账号信息不全"));
					logInfo.result = ACCOUNT_FAULT;
				}break;
			case ACCOUNT_STOP:
				{
					logInfo.nIndex = recvInfo.accountInfo.nIndex;
					wcscpy(logInfo.strItemLog,TEXT("账号停用"));
					logInfo.result = ACCOUNT_STOP;
				}break;
			case PRICE_MISMATCH:
				{
					logInfo.nIndex = recvInfo.accountInfo.nIndex;
					wcscpy(logInfo.strItemLog,TEXT("价格不匹配"));
					logInfo.result = PRICE_MISMATCH;
				}break;
			case UNKNOWN_ERROR:
				{
					logInfo.nIndex = recvInfo.accountInfo.nIndex;
					wcscpy(logInfo.strItemLog,TEXT("未知错误"));
					logInfo.result = UNKNOWN_ERROR;
				}break;
			case UNKNOWN_DEVICE:
				{
					logInfo.nIndex = recvInfo.accountInfo.nIndex;
					wcscpy(logInfo.strItemLog,TEXT("未知设备"));
					logInfo.result = UNKNOWN_DEVICE;
				}break;
			case BUY_FAILED:
				{
					logInfo.nIndex = recvInfo.accountInfo.nIndex;
					wcscpy(logInfo.strItemLog,TEXT("购买失败"));
					logInfo.result = BUY_FAILED;
				}break;
			case STATUS_FAILED:
				{
					logInfo.nIndex = recvInfo.accountInfo.nIndex;
					wcscpy(logInfo.strItemLog,TEXT("执行失败"));
					logInfo.result = STATUS_FAILED;
				}break;
			default:
				{
					logInfo.nIndex = recvInfo.accountInfo.nIndex;
					wcscpy(logInfo.strItemLog,TEXT("执行异常"));
					logInfo.result = EXECTUEFAILED;
				}
				break;
			}

			//发送WM_COPYDATA消息给管理程序，通知任务完成
			COPYDATASTRUCT cpd;
			HWND hWnd = ::FindWindow(NULL,MAINTITLE);
			if (!hWnd)
			{
				pUninitialModule();
				ExitProcess(0);
			}

			cpd.dwData = 0;
			cpd.cbData = sizeof(logInfo);
			cpd.lpData = &logInfo;
			::SendMessage(hWnd,WM_COPYDATA,(WPARAM)this->GetSafeHwnd(),(LPARAM)&cpd);
			pUninitialModule();
			ExitProcess(0);
		}

		ExitProcess(0);
	}
	catch(exception& e)
	{
		ExitProcess(0);
		OutputDebugStringA(e.what());
	}

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CxhExecuteDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CxhExecuteDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CxhExecuteDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

BOOL CxhExecuteDlg::getFunInterface()
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

	pOnlySearchKeyword = (PXHONLYSEARCHKEYWORD)GetProcAddress(hIntwork,"xhOnlySearchKeyword");
	if (!pOnlySearchKeyword)
	{
		return FALSE;
	}
	return TRUE;
}

UINT WINAPI CxhExecuteDlg::InterfaceFunction(PSENDINFO pSendInfo)
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
	int status = 0;

	::WideCharToMultiByte(CP_ACP,0,pSendInfo->strTargetUrl,wcslen(pSendInfo->strTargetUrl),szTargetUrl,MAX_PATH,NULL,NULL);
	::WideCharToMultiByte(CP_ACP,0,pSendInfo->accountInfo.strAppleId,wcslen(pSendInfo->accountInfo.strAppleId),szAppleId,MAX_PATH,NULL,NULL);
	::WideCharToMultiByte(CP_ACP,0,pSendInfo->accountInfo.strPassword,wcslen(pSendInfo->accountInfo.strPassword),szPwd,MAX_PATH,NULL,NULL);
	::WideCharToMultiByte(CP_ACP,0,pSendInfo->strSaleId,wcslen(pSendInfo->strSaleId),szAppSaleId,MAX_PATH,NULL,NULL);
	::WideCharToMultiByte(CP_ACP,0,pSendInfo->strRate,wcslen(pSendInfo->strRate),szRate,10,NULL,NULL);
	::WideCharToMultiByte(CP_ACP,0,pSendInfo->strAppPrice,wcslen(pSendInfo->strAppPrice),szAppPrice,32,NULL,NULL);
	::WideCharToMultiByte(CP_ACP,0,pSendInfo->accountInfo.deviceInfo.strUdid,wcslen(pSendInfo->accountInfo.deviceInfo.strUdid),szUdid,MAX_PATH,NULL,NULL);

	pInitCriticalSection();
	LPVOID lpNetworkObj = pCreateNetworkObject();
	if (!lpNetworkObj)
	{
		return status;
	}

	switch (pSendInfo->type)
	{
		case ONLYLOGIN:
			{
				status = pGSA_Apple_NetworkVerifier(lpNetworkObj,
													szUdid,
													&pSendInfo->modelInfo,
													&pSendInfo->accountInfo.deviceInfo,
													pSendInfo->strProxy,
													pSendInfo->netType);
				if (status != STATUS_SUCCESS)
				{
					return status;
				}
				status = pLoginAppStore(lpNetworkObj,
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
				if (status == STATUS_FAILED || status == LOGIN_FAILED)//在这两种情况下需要重新登录一次
				{
					status = pLoginAppStore(lpNetworkObj,
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
					
				}
			}break;
		case LOGINDOWNLOADAPP://登录下载
			{
				status = pGSA_Apple_NetworkVerifier(lpNetworkObj,
													szUdid,
													&pSendInfo->modelInfo,
													&pSendInfo->accountInfo.deviceInfo,
													pSendInfo->strProxy,
													pSendInfo->netType);
				if (status != STATUS_SUCCESS)
				{	
					return status;			
				}
				status = pLoginAppStore(lpNetworkObj,
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
				if (status == STATUS_FAILED || status == LOGIN_FAILED)//在这两种情况下需要重新登录一次
				{
					status = pLoginAppStore(lpNetworkObj,
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
				}
				if (status == LOGIN_SUCCESS)
				{

					//获取AppExtVrsId的值
					status = pGetAppExtVrsIdValue(lpNetworkObj,szPasswordToken,szDsid,szTargetUrl);
					if (status == STATUS_FAILED)
					{
						status = pGetAppExtVrsIdValue(lpNetworkObj,szPasswordToken,szDsid,szTargetUrl);
					}

					if (status == STATUS_SUCCESS)
					{
						status = pDownloadApp(lpNetworkObj,szPasswordToken,szDsid,szAppSaleId,szAppPrice,FALSE,pSendInfo->bPaidApp);
						if (status == STATUS_FAILED)
						{
							status = pDownloadApp(lpNetworkObj,szPasswordToken,szDsid,szAppSaleId,szAppPrice,FALSE,pSendInfo->bPaidApp);
						}
						if (status == BUY_SUCCESS)
						{
							status = DOWNLOADSUCCESS;
						}
					}
				}
			}break;
		case COMMENTAPP://登录下载评论
			{
				status = pGSA_Apple_NetworkVerifier(lpNetworkObj,
													szUdid,
													&pSendInfo->modelInfo,
													&pSendInfo->accountInfo.deviceInfo,
													pSendInfo->strProxy,
													pSendInfo->netType);
				if (status != STATUS_SUCCESS)
				{
					status = pGSA_Apple_NetworkVerifier(lpNetworkObj,
														szUdid,
														&pSendInfo->modelInfo,
														&pSendInfo->accountInfo.deviceInfo,
														pSendInfo->strProxy,
														pSendInfo->netType);
					if (status != STATUS_SUCCESS)
					{
						return status;
					}
				}
				status = pLoginAppStore(lpNetworkObj,
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
				if (status == STATUS_FAILED || status == LOGIN_FAILED)//在这两种情况下需要重新登录一次
				{
					status = pLoginAppStore(lpNetworkObj,
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
					
				}
				if (status == LOGIN_SUCCESS)
				{
					
					status = pSearchKeyHotWord(lpNetworkObj,
											pSendInfo->strHotKeyWord,
											szAppSaleId,
											szDsid,
											szPasswordToken,
											szTargetUrl);
					if (status == STATUS_FAILED)
					{
						status = pSearchKeyHotWord(lpNetworkObj,
											pSendInfo->strHotKeyWord,
											szAppSaleId,
											szDsid,
											szPasswordToken,
											szTargetUrl);
					}
					if (status == STATUS_SUCCESS)
					{
						status = pDownloadApp(lpNetworkObj,szPasswordToken,szDsid,szAppSaleId,szAppPrice,TRUE,pSendInfo->bPaidApp);
						if (status == STATUS_FAILED)
						{
							status = pDownloadApp(lpNetworkObj,szPasswordToken,szDsid,szAppSaleId,szAppPrice,TRUE,pSendInfo->bPaidApp);
							
						}

						if (status == BUY_SUCCESS)
						{
								if (status == BUY_SUCCESS)
								{
									status = pBuyConfirmActiveApp(lpNetworkObj,szPasswordToken,szAppSaleId,szDsid,szTargetUrl);
									if (status == STATUS_FAILED)
									{
										status = pBuyConfirmActiveApp(lpNetworkObj,szPasswordToken,szAppSaleId,szDsid,szTargetUrl);
										
									}
									if (status == STATUS_SUCCESS)
									{
										status = SEARCHSUCCESS;
									}
								}
								int rate = atoi(szRate);//星级
								status = pCommentApp(lpNetworkObj,
													szPasswordToken,
													szDsid,
													szAppSaleId,
													rate,
													pSendInfo->strNickName,
													pSendInfo->strTitle,
													pSendInfo->strContext);
								if (status == STATUS_FAILED)
								{
									status = pCommentApp(lpNetworkObj,
													szPasswordToken,
													szDsid,
													szAppSaleId,
													rate,
													pSendInfo->strNickName,
													pSendInfo->strTitle,
													pSendInfo->strContext);
								}
								if (status == STATUS_SUCCESS)
								{
									status = COMMENTSUCCESS;
								}
							
						}
					}
				}
			}break;
		case SEARCHAPP://登录搜索关键词下载,核心ASO部分
			{
				status = pGSA_Apple_NetworkVerifier(lpNetworkObj,
													szUdid,
													&pSendInfo->modelInfo,
													&pSendInfo->accountInfo.deviceInfo,
													pSendInfo->strProxy,
													pSendInfo->netType);
				if (status != STATUS_SUCCESS)
				{
					return status;
				}
				status = pLoginAppStore(lpNetworkObj,
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
				if (status == STATUS_FAILED || status == LOGIN_FAILED)//在这两种情况下需要重新登录一次
				{
					status = pLoginAppStore(lpNetworkObj,
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
				}

				if (status == LOGIN_SUCCESS)
				{
					status = pSearchKeyHotWord(lpNetworkObj,
											pSendInfo->strHotKeyWord,
											szAppSaleId,
											szDsid,
											szPasswordToken,
											szTargetUrl);
					if (status == STATUS_FAILED)
					{
						status = pSearchKeyHotWord(lpNetworkObj,
												pSendInfo->strHotKeyWord,
												szAppSaleId,
												szDsid,
												szPasswordToken,
												szTargetUrl);
					}
					
					if (status == STATUS_SUCCESS)
					{
						status = pDownloadApp(lpNetworkObj,szPasswordToken,szDsid,szAppSaleId,szAppPrice,TRUE,pSendInfo->bPaidApp);
						if (status == STATUS_FAILED)
						{
							status = pDownloadApp(lpNetworkObj,szPasswordToken,szDsid,szAppSaleId,szAppPrice,TRUE,pSendInfo->bPaidApp);
							
						}
						if (status == BUY_SUCCESS)
						{
							status = pBuyConfirmActiveApp(lpNetworkObj,szPasswordToken,szAppSaleId,szDsid,szTargetUrl);
							if (status == STATUS_FAILED)
							{
								status = pBuyConfirmActiveApp(lpNetworkObj,szPasswordToken,szAppSaleId,szDsid,szTargetUrl);
								
							}
							if (status == STATUS_SUCCESS)
							{
								status = SEARCHSUCCESS;
							}
						}
					}
				}
			}break;
		case LOGINSEARCH:
			{
				status = pGSA_Apple_NetworkVerifier(lpNetworkObj,
													szUdid,
													&pSendInfo->modelInfo,
													&pSendInfo->accountInfo.deviceInfo,
													pSendInfo->strProxy,
													pSendInfo->netType);
				if (status != STATUS_SUCCESS)
				{
					return status;
				}
				status = pLoginAppStore(lpNetworkObj,
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
				if (status == STATUS_FAILED || status == LOGIN_FAILED)//在这两种情况下需要重新登录一次
				{
					status = pLoginAppStore(lpNetworkObj,
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
				}

				if (status == LOGIN_SUCCESS)
				{
					status = pSearchKeyHotWord(lpNetworkObj,
											pSendInfo->strHotKeyWord,
											szAppSaleId,
											szDsid,
											szPasswordToken,
											szTargetUrl);
					if (status == STATUS_FAILED)
					{
						status = pSearchKeyHotWord(lpNetworkObj,
												pSendInfo->strHotKeyWord,
												szAppSaleId,
												szDsid,
												szPasswordToken,
												szTargetUrl);
					}
					if (status == STATUS_SUCCESS)
					{
						status = SEARCHSUCCESS;
					}
				}
			}break;
		case ONLYSEARCH:
			{
				status = pGSA_Apple_NetworkVerifier(lpNetworkObj,
													szUdid,
													&pSendInfo->modelInfo,
													&pSendInfo->accountInfo.deviceInfo,
													pSendInfo->strProxy,
													pSendInfo->netType);
				if (status != STATUS_SUCCESS)
				{
					return status;
				}
				//不登录搜索
				status = pOnlySearchKeyword(lpNetworkObj,pSendInfo->strHotKeyWord);

			}break;
		default:
			break;
	}

	if (lpNetworkObj)
	{
		pReleaseNetworkObject(lpNetworkObj);
	}

	pDeleteCriticalSection();

	g_status = status;

	return 0;

}
