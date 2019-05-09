// VPNIPDlg.cpp : implementation file
//

#include "stdafx.h"
#include "xhASO.h"
#include "VPNIPDlg.h"


// CVPNIPDlg dialog

IMPLEMENT_DYNAMIC(CVPNIPDlg, CDialog)

CVPNIPDlg::CVPNIPDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CVPNIPDlg::IDD, pParent)
{

}

CVPNIPDlg::~CVPNIPDlg()
{
}

void CVPNIPDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT1_VPN_IP_PATH, m_vpnIPPath);
}


BEGIN_MESSAGE_MAP(CVPNIPDlg, CDialog)
	ON_BN_CLICKED(IDC_BTN_VPNIP_BROWSER, &CVPNIPDlg::OnBnClickedBtnVpnipBrowser)
	ON_BN_CLICKED(IDC_BTN_LOAD_IP, &CVPNIPDlg::OnBnClickedBtnLoadIp)
END_MESSAGE_MAP()


// CVPNIPDlg message handlers

void CVPNIPDlg::OnBnClickedBtnVpnipBrowser()
{
	// TODO: Add your control notification handler code here
	CString strFilter = _T("txt file (*.txt)|*.txt|ini file (*.ini)|*.ini|All file (*.*)|*.*||");
	CFileDialog openFileDlg(TRUE, NULL, NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, strFilter);

	if (openFileDlg.DoModal() == IDOK)
	{
		m_strVpnIPFilePath = openFileDlg.GetPathName();
		m_strVpnIPFileName = openFileDlg.GetFileName();
		m_vpnIPPath.SetWindowText(m_strVpnIPFilePath);
	}
}

void CVPNIPDlg::OnBnClickedBtnLoadIp()
{
	// TODO: Add your control notification handler code here
	CString strPath;
	this->m_vpnIPPath.GetWindowText(strPath);
	if (strPath.IsEmpty())
	{
		MessageBox(TEXT("请先加载vpn IP文件，然后导入"));
		return;
	}
	HWND hMainWnd = ::FindWindow(NULL,MAINTITLE);
	if (!hMainWnd)
	{
		return;
	}

	BOOL bLoadIPList = TRUE;
	::PostMessage(hMainWnd,WM_LOAD_VPNIP_LIST_MSG,(WPARAM)bLoadIPList,0);

	SendMessage(WM_CLOSE,0,0);
}
