// DialupLoginDlg.cpp : implementation file
//

#include "stdafx.h"
#include "xhASO.h"
#include "DialupLoginDlg.h"


// CDialupLoginDlg dialog

IMPLEMENT_DYNAMIC(CDialupLoginDlg, CDialog)

CDialupLoginDlg::CDialupLoginDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CDialupLoginDlg::IDD, pParent)
{

}

CDialupLoginDlg::~CDialupLoginDlg()
{
}

void CDialupLoginDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_DIALUP_ACCOUNT, m_dialupAccount);
	DDX_Control(pDX, IDC_EDIT_DIALUP_PWD, m_dialupPwd);
	DDX_Control(pDX, IDC_CHECK_REMEMBER, m_Remcheck);
}


BEGIN_MESSAGE_MAP(CDialupLoginDlg, CDialog)
	ON_BN_CLICKED(IDC_BTN_DIALUP_LOGIN, &CDialupLoginDlg::OnBnClickedBtnDialupLogin)
	ON_BN_CLICKED(IDC_CHECK_REMEMBER, &CDialupLoginDlg::OnBnClickedCheckRemember)
END_MESSAGE_MAP()


// CDialupLoginDlg message handlers

void CDialupLoginDlg::OnBnClickedBtnDialupLogin()
{
	// TODO: Add your control notification handler code here
	m_dialupAccount.GetWindowText(m_strDialupAccount);
	m_dialupPwd.GetWindowText(m_strDialupPassword);

	if (m_strDialupAccount.IsEmpty())
	{
		MessageBox(TEXT("账号不能为空"));
		return;
	}

	if (m_strDialupPassword.IsEmpty())
	{
		MessageBox(TEXT("密码不能为空"));
		return;
	}
	HWND hMainWnd = ::FindWindow(NULL,MAINTITLE);
	if (!hMainWnd)
	{
		return;
	}
	BOOL bAdslLogin = TRUE;
	::PostMessage(hMainWnd,WM_DIALUPLOGIN_MSG,(WPARAM)bAdslLogin,0);

	SendMessage(WM_CLOSE,0,0);

}


void CDialupLoginDlg::OnBnClickedCheckRemember()
{
	// TODO: Add your control notification handler code here
	CString strAccount;
	CString strPwd;
	int nState = m_Remcheck.GetCheck();
	if (nState == BST_CHECKED)
	{
		m_dialupAccount.GetWindowText(strAccount);
		if (strAccount.IsEmpty())
		{
			MessageBox(TEXT("账号不能为空"));
			m_Remcheck.SetCheck(BST_UNCHECKED);
			return;
		}
		m_dialupPwd.GetWindowText(strPwd);
		if (strPwd.IsEmpty())
		{
			MessageBox(TEXT("密码不能为空"));
			m_Remcheck.SetCheck(BST_UNCHECKED);
			return;
		}
		
		WritePrivateProfileString(TEXT("Dialup"),TEXT("account"),strAccount,m_dialupConfig);
		WritePrivateProfileString(TEXT("Dialup"),TEXT("password"),strPwd,m_dialupConfig);

	}
	else
	{
		
	}
}

BOOL CDialupLoginDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  Add extra initialization here
	TCHAR strPath[MAX_PATH] = {0};
	DWORD dwResult = GetModuleFileName(NULL,strPath,MAX_PATH);
	CString filePath(strPath);
	int Pos = filePath.ReverseFind(_T('\\'));
	filePath.Truncate(Pos);
	filePath += _T("\\xhdialup.ini");
	m_dialupConfig = filePath;

	TCHAR szAccount[MAX_PATH] = {0};
	TCHAR szPwd[MAX_PATH] = {0};
	GetPrivateProfileString(TEXT("Dialup"),TEXT("account"),0,szAccount,MAX_PATH,m_dialupConfig);
	GetPrivateProfileString(TEXT("Dialup"),TEXT("password"),0,szPwd,MAX_PATH,m_dialupConfig);

	CString strVpnAccount(szAccount);
	CString strVpnPwd(szPwd);
	
	if ( !strVpnAccount.IsEmpty())
	{
		m_dialupAccount.SetWindowText(strVpnAccount);
		m_dialupPwd.SetWindowText(strVpnPwd);

		m_Remcheck.SetCheck(BST_CHECKED);
	}
	else
	{
		m_Remcheck.SetCheck(BST_UNCHECKED);
	}


	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}
