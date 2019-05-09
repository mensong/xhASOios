
// xhRegAppleIdDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "xhRegAppleId.h"
#include "xhRegAppleIdDlg.h"
#include "accountInfo.h"
#include "DataProvider.h"
#include "cryptAlgorithm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


typedef BOOL (* PINITIALIZEMODULE)(LPVOID strPath);
typedef void (* PUNINITIALIZEMODULE)();
typedef LPVOID (*PXHCREATENETWORKOBJECT)();
typedef VOID (*PXHRELEASENEWTWORKOBJECT)(LPVOID lpNetwork);
typedef int (* PXHREGISTERAPPLEID)(IN LPVOID lpNetwork,TAGREGISTERINFO& tagRegisterInfo);

PINITIALIZEMODULE				pInitialModule = NULL;
PUNINITIALIZEMODULE				pUninitialModule = NULL;
PXHCREATENETWORKOBJECT			pCreateNetworkObject = NULL;
PXHRELEASENEWTWORKOBJECT		pReleaseNetworkObject = NULL;
PXHREGISTERAPPLEID				pRegisterAppleId = NULL; 

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


// CxhRegAppleIdDlg 对话框




CxhRegAppleIdDlg::CxhRegAppleIdDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CxhRegAppleIdDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CxhRegAppleIdDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CxhRegAppleIdDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BTN_REGISTER_START, &CxhRegAppleIdDlg::OnBnClickedBtnRegisterStart)
END_MESSAGE_MAP()


// CxhRegAppleIdDlg 消息处理程序

BOOL CxhRegAppleIdDlg::OnInitDialog()
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
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CxhRegAppleIdDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CxhRegAppleIdDlg::OnPaint()
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
HCURSOR CxhRegAppleIdDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CxhRegAppleIdDlg::InitAndLoadModule()
{
	HMODULE hIntwork = NULL;
	HMODULE hxhiTunes = NULL;

	//加密字符串
	char srcData[] = "iTunesCore.dll";
	int dataLen = strlen(srcData);
	char key[] = "xhiTunes";
	kEncodeData(srcData,key);
	
	hIntwork = LoadLibrary(_T("xhwork.dll"));
	if (hIntwork == NULL)
	{
		return;
	}

	hxhiTunes = LoadLibrary(_T("xhiTunes.dll"));
	if (hxhiTunes == NULL)
	{
		return;
	}

	pInitialModule = (PINITIALIZEMODULE)GetProcAddress(hxhiTunes,"InitializeModule");
	if (!pInitialModule)
	{
		return;
	}

	pUninitialModule = (PUNINITIALIZEMODULE)GetProcAddress(hxhiTunes,"UnInitializeModule");
	if (!pUninitialModule)
	{
		return;
	}
	pCreateNetworkObject = (PXHCREATENETWORKOBJECT)GetProcAddress(hIntwork,"xhCreateNetworkObject");
	if (!pCreateNetworkObject)
	{
		return;
	}
	pReleaseNetworkObject = (PXHRELEASENEWTWORKOBJECT)GetProcAddress(hIntwork,"xhReleaseNewtworkObject");
	if (!pReleaseNetworkObject)
	{
		return;
	}
	pRegisterAppleId = (PXHREGISTERAPPLEID)GetProcAddress(hIntwork,"xhRegisterAppleId");
	if (!pRegisterAppleId)
	{
		return;
	}

	BOOL Result = pInitialModule((LPVOID)srcData);
	if (!Result)
	{
		return;
	}

}

void CxhRegAppleIdDlg::UnInitAndUnLoadModule()
{
	pUninitialModule();
}

UINT WINAPI  CxhRegAppleIdDlg::registerIdThread(LPVOID lPtr)
{
	CxhRegAppleIdDlg* pThis = (CxhRegAppleIdDlg*)lPtr;
	_ASSERTE( pThis != NULL );

	int					status = 0;
	TAGREGISTERINFO		tagRegInfo;
	ACCOUNTINFO			appleIdInfo = {0};
	CDataProvider		registerData;
	
	//初始化加载模块
	pThis->InitAndLoadModule();

	//创建注册信息
	registerData.CreateUserData();

	wcscpy(appleIdInfo.strAppleId,registerData.GetEmailAddressW().c_str());
	wcscpy(appleIdInfo.strPassword,registerData.GetPasswordW().c_str());
	tagRegInfo.accountInfo.push_back(appleIdInfo);

	//卸载模块
	pThis->UnInitAndUnLoadModule();
	return 0;
}



void CxhRegAppleIdDlg::OnBnClickedBtnRegisterStart()
{
	// TODO: Add your control notification handler code here
	//创建线程
	DWORD dwThreadId = 0;
	HANDLE hThread = CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)registerIdThread,this,0,&dwThreadId);

}
