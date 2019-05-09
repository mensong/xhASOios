#pragma once
#include "afxwin.h"



// CAppleIdDlg dialog

class CAppleIdDlg : public CDialog
{
	DECLARE_DYNAMIC(CAppleIdDlg)

public:
	CAppleIdDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CAppleIdDlg();

// Dialog Data
	enum { IDD = IDD_LOAD_APPLEID_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedBtnBrowser();
	afx_msg void OnBnClickedBtnLoad();
	CEdit m_appleIdFile;
	CString m_strAppleIdFilePath;
	CString	m_strAppleIdFileName;
};
