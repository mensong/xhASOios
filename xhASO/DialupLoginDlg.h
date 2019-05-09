#pragma once
#include "afxwin.h"


// CDialupLoginDlg dialog

class CDialupLoginDlg : public CDialog
{
	DECLARE_DYNAMIC(CDialupLoginDlg)

public:
	CDialupLoginDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDialupLoginDlg();

// Dialog Data
	enum { IDD = IDD_DLG_DIALUP };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CEdit m_dialupAccount;
	CEdit m_dialupPwd;
	CButton m_Remcheck;
	CString m_strDialupAccount;
	CString m_strDialupPassword;
	CString m_dialupConfig;
	afx_msg void OnBnClickedBtnDialupLogin();
	afx_msg void OnBnClickedCheckRemember();
	virtual BOOL OnInitDialog();
};
