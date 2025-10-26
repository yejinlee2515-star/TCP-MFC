
// Lab09_TCPServerDlg.h : 헤더 파일
//

#pragma once
#include "afxwin.h"
#include "afxcoll.h"

struct ThreadArg
{
	CStringList* pList;
	CDialogEx* pDlg;
	int Thread_run;
};

class CListenSocket;
class CDataSocket;


// CLab09_TCPServerDlg 대화 상자
class CLab09_TCPServerDlg : public CDialogEx
{
// 생성입니다.
public:
	CLab09_TCPServerDlg(CWnd* pParent = NULL);	// 표준 생성자입니다.

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_LAB09_TCPSERVER_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 지원입니다.


// 구현입니다.
protected:
	HICON m_hIcon;

	// 생성된 메시지 맵 함수
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	CWinThread *pThread1, *pThread2;
	ThreadArg arg1, arg2;
	CListenSocket *m_pListenSocket;
	CDataSocket *m_pDataSocket;

	CEdit m_tx_edit_short;
	CEdit m_rx_edit;
	CEdit m_tx_edit;
	void ProcessAccept(int nErrorCode);
	void ProcessReceive(CDataSocket* pSocket, int nErrorCode);
	void ProcessClose(CDataSocket* pSocket, int nErrorCode);
	afx_msg void OnBnClickedSend();
	afx_msg void OnBnClickedDisconnect();
};
