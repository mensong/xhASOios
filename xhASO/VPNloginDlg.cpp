// VPNloginDlg.cpp : implementation file
//

#include "stdafx.h"
#include "xhASO.h"
#include "VPNloginDlg.h"


// CVPNloginDlg dialog

IMPLEMENT_DYNAMIC(CVPNloginDlg, CDialog)

CVPNloginDlg::CVPNloginDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CVPNloginDlg::IDD, pParent)
{

}

CVPNloginDlg::~CVPNloginDlg()
{
}

void CVPNloginDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_ACCOUNT, m_vpnAccount);
	DDX_Control(pDX, IDC_EDIT_PASSWORD, m_password);
	DDX_Control(pDX, IDC_CHECK_SAVE, m_SelectCheck);
	DDX_Control(pDX, IDC_EDIT_IP_ADDRESS, m_IpAddress);
}


BEGIN_MESSAGE_MAP(CVPNloginDlg, CDialog)
	ON_BN_CLICKED(IDC_BTN_LOGIN, &CVPNloginDlg::OnBnClickedBtnLogin)
	ON_BN_CLICKED(IDC_CHECK_SAVE, &CVPNloginDlg::OnBnClickedCheckSave)
END_MESSAGE_MAP()


// CVPNloginDlg message handlers

void CVPNloginDlg::OnBnClickedBtnLogin()
{
	// TODO: Add your control notification handler code here
	m_vpnAccount.GetWindowText(m_strVpnAccount);
	m_password.GetWindowText(m_strPassword);
	m_IpAddress.GetWindowText(m_strIPAddress);

	if (m_strVpnAccount.IsEmpty())
	{
		MessageBox(TEXT("账号不能为空"));
		return;
	}

	if (m_strPassword.IsEmpty())
	{
		MessageBox(TEXT("密码不能为空"));
		return;
	}
	if (m_strIPAddress.IsEmpty())
	{
		MessageBox(TEXT("IP不能为空"));
		return;
	}
	HWND hMainWnd = ::FindWindow(NULL,MAINTITLE);
	if (!hMainWnd)
	{
		return;
	}

	BOOL bLoginVpn = TRUE;

	::PostMessage(hMainWnd,WM_LOGINVPN_MSG,(WPARAM)bLoginVpn,0);

	SendMessage(WM_CLOSE,0,0);
}

BOOL CVPNloginDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  Add extra initialization here
	TCHAR strPath[MAX_PATH] = {0};
	DWORD dwResult = GetModuleFileName(NULL,strPath,MAX_PATH);
	CString filePath(strPath);
	int Pos = filePath.ReverseFind(_T('\\'));
	filePath.Truncate(Pos);
	filePath += _T("\\xhConfig.ini");
	m_configfile = filePath;


	TCHAR szIpAddr[MAX_PATH] = {0};
	TCHAR szAccount[MAX_PATH] = {0};
	TCHAR szPwd[MAX_PATH] = {0};

	GetPrivateProfileString(TEXT("VPN"),TEXT("IP"),0,szIpAddr,MAX_PATH,m_configfile);
	GetPrivateProfileString(TEXT("VPN"),TEXT("account"),0,szAccount,MAX_PATH,m_configfile);
	GetPrivateProfileString(TEXT("VPN"),TEXT("password"),0,szPwd,MAX_PATH,m_configfile);

	CString strIpAddress(szIpAddr);
	CString strVpnAccount(szAccount);
	CString strVpnPwd(szPwd);
	
	if ( !strVpnAccount.IsEmpty())
	{
		m_IpAddress.SetWindowText(strIpAddress);
		m_vpnAccount.SetWindowText(strVpnAccount);
		m_password.SetWindowText(strVpnPwd);

		m_SelectCheck.SetCheck(BST_CHECKED);
	}
	else
	{
		m_SelectCheck.SetCheck(BST_UNCHECKED);
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CVPNloginDlg::OnBnClickedCheckSave()
{
	// TODO: Add your control notification handler code here
	CString strIPAddr;
	CString strAccount;
	CString strPwd;
	int nState = m_SelectCheck.GetCheck();
	if (nState == BST_CHECKED)
	{
		m_IpAddress.GetWindowText(strIPAddr);
		if (strIPAddr.IsEmpty())
		{
			MessageBox(TEXT("IP 不能为空"));
			m_SelectCheck.SetCheck(BST_UNCHECKED);
			return;
		}
		m_vpnAccount.GetWindowText(strAccount);
		if (strAccount.IsEmpty())
		{
			MessageBox(TEXT("账号不能为空"));
			m_SelectCheck.SetCheck(BST_UNCHECKED);
			return;
		}
		m_password.GetWindowText(strPwd);
		if (strPwd.IsEmpty())
		{
			MessageBox(TEXT("密码不能为空"));
			m_SelectCheck.SetCheck(BST_UNCHECKED);
			return;
		}

		WritePrivateProfileString(TEXT("VPN"),TEXT("IP"),strIPAddr,m_configfile);
		WritePrivateProfileString(TEXT("VPN"),TEXT("account"),strAccount,m_configfile);
		WritePrivateProfileString(TEXT("VPN"),TEXT("password"),strPwd,m_configfile);

	}
	else
	{
		
	}
}
