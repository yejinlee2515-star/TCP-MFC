
// Lab09_TCPClientDlg.h : ��� ����
//

#pragma once
#include "afxwin.h"
#include "afxcmn.h"

struct ThreadArg
{
	CStringList* pList;
	CDialogEx* pDlg;
	int Thread_run;
};

class CDataSocket;

// CLab09_TCPClientDlg ��ȭ ����
class CLab09_TCPClientDlg : public CDialogEx
{
// �����Դϴ�.
public:
	CLab09_TCPClientDlg(CWnd* pParent = NULL);	// ǥ�� �������Դϴ�.
	

// ��ȭ ���� �������Դϴ�.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_LAB09_TCPCLIENT_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV �����Դϴ�.


// �����Դϴ�.
protected:
	HICON m_hIcon;

	// ������ �޽��� �� �Լ�
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	CWinThread *pThread1, *pThread2;
	ThreadArg arg1, arg2;
	CDataSocket *m_pDataSocket;
	CEdit m_tx_edit_short;
	CEdit m_rx_edit;
	CEdit m_tx_edit;
	CIPAddressCtrl m_ipaddr;
	afx_msg void OnBnClickedConnect();
	afx_msg void OnBnClickedDisconnect();
	afx_msg void OnBnClickedSend();
	void ProcessReceive(CDataSocket* pSocket, int nErrorCode);
};
