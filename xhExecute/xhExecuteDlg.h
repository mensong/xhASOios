
// xhExecuteDlg.h : 头文件
//
#include "accountInfo.h"
#pragma once


// CxhExecuteDlg 对话框
class CxhExecuteDlg : public CDialog
{
// 构造
public:
	CxhExecuteDlg(CWnd* pParent = NULL);	// 标准构造函数

	enum
	{
		LOGINDOWNLOADAPP = 1,
		COMMENTAPP,
		SEARCHAPP,
		ONLYLOGIN,
		LOGINSEARCH,
		ONLYSEARCH
	};
// 对话框数据
	enum { IDD = IDD_XHEXECUTE_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持
public:
	HANDLE m_hPipeRead;
	HANDLE m_hPipeWrite;
	HANDLE m_hEvent;

// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	BOOL getFunInterface();
	static UINT WINAPI InterfaceFunction(PSENDINFO pSendInfo);
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
};
