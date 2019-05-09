
// xhASODlg.h : header file
//

#pragma once
#include "afxwin.h"
#include "VPNIPDlg.h"
#include "AppleIdDlg.h"
#include "VPNloginDlg.h"
#include "LoadCommentDlg.h"
#include "DialupLoginDlg.h"
#include "afxcmn.h"
#include "SMTP.h"
#include "SocketComm.h"
#include "cryptAlgorithm.h"
#include "accountInfo.h"
#include <vector>
#include <list>

#define MAX_SEND_SIZE 1024
#define THREAD_COUNTS 10

typedef struct _costTime
{
	int hour;
	int miniute;
	int second;
	int misecond;
}TOTALTIME;


// CxhASODlg dialog
class CxhASODlg : public CDialog
{
// Construction
public:
	CxhASODlg(CWnd* pParent = NULL);	// standard constructor
	void InitListStyle(void);
	void loadUAInfo(void);
	BOOL SetWindowsReportInfo();
	CString GetProcessorNameString();
	BOOL SetProcessorNameString(TCHAR* pData,BOOL bFirst);
	CString GetSystemBiosVersion();
	BOOL SetSystemBiosVersion(TCHAR* pData,BOOL bFirst);
	BOOL SetPCName(TCHAR* pData,BOOL bFirst);
	CString GetHwProfileGuid();
	BOOL SetHwProfileGuid(BOOL bFirst,BOOL bRestore);
	BOOL ModifyHardwareInfo(BOOL bFirst,BOOL bRestore);
	BOOL RestoreHardwareInfo(void);
	BOOL SetProductId(TCHAR* pData,BOOL bFirst);
	CString GetProductId();
	void FilterAppleId(void);
	void SendEmailEx(void);
	BOOL SendDataToServer();
	void readTxtFormatData(CString strPath);
	void readCSVFormatData(CString strPath);
	void parseLineData(CString pLineData,CString strFormat,int nIndex);
	void parsePaidLineData(CString pLineData,CString strFormat,int nIndex);
	void parseCommentLineData(CString pLineData,int nIndex);
	void deleteCookieFile(void);
	BOOL ReadStringToUnicode(CString &str);
	int CharToUnicode(char *pchIn, CString *pstrOut);
	BOOL getFunInterface();
	int InterfaceFunction(PSENDINFO pSendInfo);
	void SetDisplayResultInfo(int status,int nIndex);
	void unloadFunInterface();
	BOOL TrialVersionCheck(void);
	int ConvertUtf8ToGBK(char* pChangeStr,int nLength);
	void Convert(const char* strIn,char* strOut, int sourceCodepage, int targetCodepage);
	static UINT WINAPI socketThreadProc(LPVOID lpArg);
	static UINT WINAPI ListenForRequests(LPVOID lpPtr);
	static UINT WINAPI showAppleIdListThreadProc(LPVOID lpPtr);

	//线程池执行队列线程
	static DWORD WINAPI executeSearchWorkItem(LPVOID lpPtr);
	static DWORD WINAPI executeTaskThread(LPVOID lpPtr);
	static DWORD WINAPI executeLoginAppleIdItem(LPVOID lpPtr);
	static DWORD WINAPI executeDownloadWorkItem(LPVOID lpPtr);
	static DWORD WINAPI executeCommentWorkItem(LPVOID lpPtr);

	//正常网络执行线程
	static UINT WINAPI normalnetDownloadAppThreadProc(LPVOID lpPtr);
	static UINT WINAPI normalnetCommentThreadProc(LPVOID lpPtr);
	static UINT WINAPI normalnetSearchKeywordThreadProc(LPVOID lpPtr);

	//vpn执行线程
	static UINT WINAPI vpnDownloadAppThreadProc(LPVOID lpPtr);
	static UINT WINAPI vpnCommentThreadProc(LPVOID lpPtr);
	static UINT WINAPI vpnSearchKeywordThreadProc(LPVOID lpPtr);

	//拨号执行线程
	static UINT WINAPI dialupDownloadAppThreadProc(LPVOID lpPtr);
	static UINT WINAPI dialupCommentThreadProc(LPVOID lpPtr);
	static UINT WINAPI dialupSearchKeywordThreadProc(LPVOID lpPtr);

	//登录执行线程
	static UINT WINAPI loginAccountThreadProc(LPVOID lpPtr);

	static UINT WINAPI commentShowThreadProc(LPVOID lpPtr);
	static UINT WINAPI vpnIpThreadProc(LPVOID lpPtr);
	static UINT WINAPI loginVpnThreadProc(LPVOID lpPtr);
	static UINT WINAPI dialupLoginThreadProc(LPVOID lpPtr);
	static UINT WINAPI loginVpnUseRasDialCmdThreadProc(LPVOID lpPtr);

	static UINT WINAPI heartBeatThreadProc(LPVOID lpPtr);

	static UINT WINAPI monitorPorcessProc(LPVOID lpPtr);
	void monitorTaskProcess();
	HWND getWindowHandleByProcessId(DWORD dwProcessId);
	BOOL m_bHeartBeatRunning;
	BOOL m_bMonitorRunning;
	
	
	enum
	{
		LOGINDOWNLOADAPP=1,
		COMMENTAPP,
		SEARCHAPP,
		ONLYLOGIN,
		LOGINSEARCH,
		ONLYSEARCH,
	};

// Dialog Data
	enum { IDD = IDD_XHASO_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	TOTALTIME m_totalTime;
	CEdit m_appleId;
	CEdit m_pwd;
	afx_msg void OnBnClickedBtnComment();
	CEdit m_saleId;
	CEdit m_title;
	CEdit m_body;
	CMenu m_menu;
	HANDLE m_hComm;
	HANDLE m_handle;
	HANDLE m_hEvent;
	HANDLE m_hQueueEvent;
	HANDLE m_hThreadEvent;
	HANDLE m_hFinishEvent;
	HANDLE m_hMonitorEvent;
	HANDLE m_hSwitchIPEvent;
	HANDLE m_hSocketThread;
	BOOL m_bDataToSend;
	BOOL m_bExitThread;
	BOOL m_bLoginVpn;
	BOOL m_bLoginAdsl;
	BOOL m_bVpnConnect;
	BOOL m_bBuyType;
	BOOL m_bIsAdslConnect;
	BOOL m_bDefaultFree;
	BOOL m_bThreadGsaVerifier;
	int  m_nIndex;
	int	 m_finishTaskCount;
	LONG m_ItemCount;
	HARDWAREINFO m_hardwareInfo;
	CString m_vpnNameContext;
	CString m_strSaveDialupAccount;
	CString m_strSaveDialupPassword;
	HANDLE m_hWrite;  
    HANDLE m_hRead; 
	VECTORMODEL m_vecModel;
	MONTASKLIST m_monTaskList;
	std::vector<int> m_vecExceptionId;
	
public:
	void ConnectVPN(void);
	void DisconnectVPN(void);
	void ConnectVPS(void);
	void DisconnectVPS(void);

	void GetIP(void);
	void SetVPNIPDisplay(void);
	BOOL CheckInternetStatus(void);
	BOOL InternetIPDetect(CString strDialupAccount,CString strPassword,char* lpIPAddr,int networkType);
	BOOL UnityFunction(CString strHotKeyWord);
	int ExectueAProcess(PSENDINFO pSendInfo,int nIndex);
	CEdit m_hotkeyword;
	CEdit m_searchCount;
	afx_msg void OnBnClickedBtnConnect();
	CEdit m_strAdsl_EntryName;
	CEdit m_strAdsl_userName;
	CEdit m_strAdsl_userPwd;
	CEdit m_successCount;
	afx_msg void OnBnClickedBtnReconnect();

	CString m_ServerVPNIP;
	CString m_configfile;
	BOOL bIsAdslConnect;
	CEdit m_reConnCount;
	CStatic m_currentIP;
	CEdit m_appId;
	CVPNIPDlg m_vpnIpDlg;
	CAppleIdDlg m_appleIdDlg;
	CVPNloginDlg m_vpnLoginDlg;
	CDialupLoginDlg m_dialupLoginDlg;
	CLoadCommentDlg m_commentDlg;
	CListCtrl m_appleIdLoginAppDownload;
	AccountInfo m_accountInfoList;
	std::vector<CString> m_vpnIPlist;
	std::vector<int> m_vecLoginSucessAppleIdIndex;
	std::vector<int> m_vecLoginFailedAppleIdIndex;
	std::vector<int> m_vecExecuteFailedAppleIdIndex;
	std::vector<int> m_vecSuccessAppleIdIndex;
	std::vector<int> m_vecAccountStopAppleIdIndex;
	std::vector<int> m_vecAccountLockedAppleIdIndex;
	std::vector<int> m_vecPasswordErrorAppleIdIndex;
	std::vector<int> m_vecAccountFaultAppleIdIndex;
	HRASCONN m_hRasConn;
	CStatic m_netConnectStatus;
	CStatic m_showResult;
	CEdit m_executeCountsEdit;
	int     m_failedCounts;
	int     m_successCounts;
	int     m_accountIndex;

	afx_msg void OnFileLoadvpn();
	afx_msg void OnFileLoadappleid();
	afx_msg void OnCommnets();
	afx_msg void OnVpnConnectCmd();
	afx_msg void OnPppoeConnectCmd();
	afx_msg void OnLoginAppleid();
	afx_msg void OnDownloadApp();
	afx_msg void OnCommentApp();
	afx_msg void OnConnectMe();
	
	afx_msg void OnSearchKeyword();
	afx_msg LRESULT OnLoadAppleIdlistMsg(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnLoadCommentMsg(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT TerminateThread(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnLoadvpnIPMsg(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnLoginVpnMsg(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnDialupLoginMsg(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnHandleExceptionTaskProcess(WPARAM wParam, LPARAM lParam);
	afx_msg void OnBnClickedBtnStartComment();
	afx_msg void OnBnClickedBtnStartSearch();	
	afx_msg void OnBnClickedBtnDownload();	
	afx_msg void OnClose();
	CEdit m_eachSwitchVpnTotalCount;
	int m_eachSwitchCounts;
	BOOL m_bReConnect;
	CButton m_RadioVpn;
	afx_msg void OnBnClickedRadioVpn();
	afx_msg void OnBnClickedRadioDialUp();
	afx_msg void OnBnClickedRadioAgent();
	afx_msg void OnBnClickedRadioLocalLoad();
	afx_msg void OnBnClickedRadioServerLoad();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	CEdit m_editTargetUrl;
	afx_msg void OnBnClickedBtnServerStart();
	afx_msg void OnBnClickedBtnServerStop();
	afx_msg void OnBnClickedRadioFree();
	afx_msg void OnBnClickedRadioPaid();
	afx_msg void OnRegisterAppleid();
	CEdit m_editAppPrice;
	afx_msg BOOL OnCopyData(CWnd* pWnd, COPYDATASTRUCT* pCopyDataStruct);
	afx_msg void OnBnClickedRadioProcess();
	afx_msg void OnBnClickedRadioThread();
	
	CStatic m_currentTaskCount;
};
