#pragma once
#include "afxwin.h"


// CVPNIPDlg dialog

class CVPNIPDlg : public CDialog
{
	DECLARE_DYNAMIC(CVPNIPDlg)

public:
	CVPNIPDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CVPNIPDlg();

// Dialog Data
	enum { IDD = IDD_VPNIP_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedBtnVpnipBrowser();
	afx_msg void OnBnClickedBtnLoadIp();
	CString m_strVpnIPFilePath;
	CString m_strVpnIPFileName;
	CEdit m_vpnIPPath;
};
