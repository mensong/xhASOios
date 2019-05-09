// AppleIdDlg.cpp : implementation file
//

#include "stdafx.h"
#include "xhASO.h"
#include "AppleIdDlg.h"


// CAppleIdDlg dialog

IMPLEMENT_DYNAMIC(CAppleIdDlg, CDialog)

CAppleIdDlg::CAppleIdDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CAppleIdDlg::IDD, pParent)
{

}

CAppleIdDlg::~CAppleIdDlg()
{
}

void CAppleIdDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_APPLEIDFILE_PATH, m_appleIdFile);
}


BEGIN_MESSAGE_MAP(CAppleIdDlg, CDialog)
	ON_BN_CLICKED(IDC_BTN_BROWSER, &CAppleIdDlg::OnBnClickedBtnBrowser)
	ON_BN_CLICKED(IDC_BTN_LOAD, &CAppleIdDlg::OnBnClickedBtnLoad)
END_MESSAGE_MAP()


// CAppleIdDlg message handlers

void CAppleIdDlg::OnBnClickedBtnBrowser()
{
	// TODO: Add your control notification handler code here
	CString strFilter = _T("csv file (*.csv)|*csv|txt file (*.txt)|*.txt|ini file (*.ini)|*.ini|All file (*.*)|*.*||");
	CFileDialog openFileDlg(TRUE, NULL, NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, strFilter);

	if (openFileDlg.DoModal() == IDOK)
	{
		m_strAppleIdFilePath = openFileDlg.GetPathName();
		m_strAppleIdFileName = openFileDlg.GetFileName();
		m_appleIdFile.SetWindowText(m_strAppleIdFilePath);
	}
	UpdateData();
}

void CAppleIdDlg::OnBnClickedBtnLoad()
{
	// TODO: Add your control notification handler code here
	CString strPath;
	this->m_appleIdFile.GetWindowText(strPath);
	if (strPath.IsEmpty())
	{
		MessageBox(TEXT("请先加载文件，然后再导入"));
		return;
	}
	HWND hMainWnd = ::FindWindow(NULL,MAINTITLE);
	if (!hMainWnd)
	{
		return;
	}

	::PostMessage(hMainWnd,WM_LOADAPPLEID_MSG,0,0);

	SendMessage(WM_CLOSE,0,0);
}
