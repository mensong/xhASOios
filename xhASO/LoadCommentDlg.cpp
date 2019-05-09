// LoadCommentDlg.cpp : implementation file
//

#include "stdafx.h"
#include "xhASO.h"
#include "LoadCommentDlg.h"


// CLoadCommentDlg dialog

IMPLEMENT_DYNAMIC(CLoadCommentDlg, CDialog)

CLoadCommentDlg::CLoadCommentDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CLoadCommentDlg::IDD, pParent)
{

}

CLoadCommentDlg::~CLoadCommentDlg()
{
}

void CLoadCommentDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_COMMENT, m_commentEdit);
}


BEGIN_MESSAGE_MAP(CLoadCommentDlg, CDialog)
	ON_BN_CLICKED(IDC_BTN_LOAD_COMMENT, &CLoadCommentDlg::OnBnClickedBtnLoadComment)
	ON_BN_CLICKED(IDC_BTN_BROWSE, &CLoadCommentDlg::OnBnClickedBtnBrowse)
END_MESSAGE_MAP()


// CLoadCommentDlg message handlers

void CLoadCommentDlg::OnBnClickedBtnLoadComment()
{
	// TODO: Add your control notification handler code here
	CString strPath;
	m_commentEdit.GetWindowText(strPath);
	if (strPath.IsEmpty())
	{
		MessageBox(TEXT("请先加载评论内容文件，然后再导入"));
		return;
	}
	HWND hMainWnd = ::FindWindow(NULL,MAINTITLE);
	if (!hMainWnd)
	{
		return;
	}
	::PostMessage(hMainWnd,WM_LOAD_COMMENT_CONTENT_MSG,0,0);

	SendMessage(WM_CLOSE,0,0);
}

void CLoadCommentDlg::OnBnClickedBtnBrowse()
{
	// TODO: Add your control notification handler code here
	CString strFilter = _T("csv file (*.csv)|*.csv|txt file (*.txt)|*.txt|ini file (*.ini)|*.ini|All file (*.*)|*.*||");
	CFileDialog openFileDlg(TRUE, NULL, NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, strFilter);

	if (openFileDlg.DoModal() == IDOK)
	{
		m_strCommentfilePath = openFileDlg.GetPathName();
		m_strCommentfileName = openFileDlg.GetFileName();
		m_commentEdit.SetWindowText(m_strCommentfilePath);
	}
	
}
