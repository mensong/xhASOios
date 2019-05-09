#pragma once
#include "afxwin.h"


// CLoadCommentDlg dialog

class CLoadCommentDlg : public CDialog
{
	DECLARE_DYNAMIC(CLoadCommentDlg)

public:
	CLoadCommentDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CLoadCommentDlg();

// Dialog Data
	enum { IDD = IDD_LOAD_COMMENT_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CString m_strCommentfilePath;
	CString m_strCommentfileName;
	CEdit m_commentEdit;
	afx_msg void OnBnClickedBtnLoadComment();
	afx_msg void OnBnClickedBtnBrowse();
};
