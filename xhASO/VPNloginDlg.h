#pragma once
#include "afxwin.h"


// CVPNloginDlg dialog

class CVPNloginDlg : public CDialog
{
	DECLARE_DYNAMIC(CVPNloginDlg)

public:
	CVPNloginDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CVPNloginDlg();

// Dialog Data
	enum { IDD = IDD_VPN_CONNECT_DIALOG };
	CString m_configfile;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CString m_strIPAddress;
	CString m_strVpnAccount;
	CString m_strPassword;
	CButton m_SelectCheck;
	CEdit m_IpAddress;
	CEdit m_vpnAccount;
	CEdit m_password;
	afx_msg void OnBnClickedBtnLogin();
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedCheckSave();
	
};
