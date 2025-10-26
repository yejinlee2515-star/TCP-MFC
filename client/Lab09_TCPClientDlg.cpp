
// Lab09_TCPClientDlg.cpp : ���� ����
//

#include "stdafx.h"
#include "Lab09_TCPClient.h"
#include "Lab09_TCPClientDlg.h"
#include "afxdialogex.h"
#include "DataSocket.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CCriticalSection tx_cs;
CCriticalSection rx_cs;

// ���� ���α׷� ������ ���Ǵ� CAboutDlg ��ȭ �����Դϴ�.

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// ��ȭ ���� �������Դϴ�.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

// �����Դϴ�.
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CLab09_TCPClientDlg ��ȭ ����



CLab09_TCPClientDlg::CLab09_TCPClientDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_LAB09_TCPCLIENT_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CLab09_TCPClientDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT1, m_tx_edit_short);
	DDX_Control(pDX, IDC_EDIT2, m_rx_edit);
	DDX_Control(pDX, IDC_EDIT3, m_tx_edit);
	DDX_Control(pDX, IDC_IPADDRESS1, m_ipaddr);
}

BEGIN_MESSAGE_MAP(CLab09_TCPClientDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_CONNECT, &CLab09_TCPClientDlg::OnBnClickedConnect)
	ON_BN_CLICKED(IDC_DISCONNECT, &CLab09_TCPClientDlg::OnBnClickedDisconnect)
	ON_BN_CLICKED(IDC_SEND, &CLab09_TCPClientDlg::OnBnClickedSend)
END_MESSAGE_MAP()


// CLab09_TCPClientDlg �޽��� ó����
UINT RXThread(LPVOID arg)
{
	ThreadArg *pArg = (ThreadArg*)arg;
	CStringList *plist = pArg->pList;	// ���޹��� pLIst ����
	CLab09_TCPClientDlg *pDlg = (CLab09_TCPClientDlg *)pArg->pDlg;	// ��ȭ���ڸ� ����Ű�� ����
	while (pArg->Thread_run)	// Thread�� run�� ���� �����ϵ��� ��
	{
		POSITION pos = plist->GetHeadPosition();
		POSITION current_pos;
		while (pos != NULL)
		{
			current_pos = pos;	// pList�� ���� ��
			rx_cs.Lock();	// cs
			CString str = plist->GetNext(pos);	// ���� ��ġ�� �ٲ� �� �����Ƿ� cs�� �־� ��
			rx_cs.Unlock();	// cs ����
			CString message;
			pDlg->m_rx_edit.GetWindowText(message);
			message += str;
			pDlg->m_rx_edit.SetWindowTextW(message);	// ȭ�鿡 ��Ÿ������ ��
			pDlg->m_rx_edit.LineScroll(pDlg->m_rx_edit.GetLineCount());	// �Ҵ�� ĭ�� ��� ����ϸ� �ڵ����� ��ũ�� �ǵ��� ��

			plist->RemoveAt(current_pos);	// ó���� �Ϸ��Ͽ����Ƿ� ���� �޼��� ���� 
		}
		Sleep(10);
	}
	return 0;
}

UINT TXThread(LPVOID arg)
{
	ThreadArg *pArg = (ThreadArg*)arg;
	CStringList *plist = pArg->pList;	// �Է¹��� pLIst ����
	CLab09_TCPClientDlg *pDlg = (CLab09_TCPClientDlg *)pArg->pDlg;	// ��ȭ���ڸ� ����Ű�� ����
	while (pArg->Thread_run)	// Thread�� run�� ���� �����ϵ��� ��
	{
		POSITION pos = plist->GetHeadPosition();
		POSITION current_pos;
		while (pos != NULL)
		{
			current_pos = pos;	// pList�� ���� ��
			tx_cs.Lock();	// cs
			CString str = plist->GetNext(pos);	// ���� ��ġ�� �ٲ� �� �����Ƿ� cs�� �־� ��
			tx_cs.Unlock();	// cs ����
			pDlg->m_pDataSocket->Send((LPCTSTR)str, (str.GetLength() + 1) * sizeof(TCHAR));	// �޼��� ����
			pDlg->m_tx_edit.LineScroll(pDlg->m_tx_edit.GetLineCount());

			plist->RemoveAt(current_pos);	// ó���� �Ϸ��Ͽ����Ƿ� ���� �޼��� ���� 
		}
		Sleep(10);
	}
	return 0;
}

BOOL CLab09_TCPClientDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// �ý��� �޴��� "����..." �޴� �׸��� �߰��մϴ�.

	// IDM_ABOUTBOX�� �ý��� ��� ������ �־�� �մϴ�.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// �� ��ȭ ������ �������� �����մϴ�.  ���� ���α׷��� �� â�� ��ȭ ���ڰ� �ƴ� ��쿡��
	//  �����ӿ�ũ�� �� �۾��� �ڵ����� �����մϴ�.
	SetIcon(m_hIcon, TRUE);			// ū �������� �����մϴ�.
	SetIcon(m_hIcon, FALSE);		// ���� �������� �����մϴ�.

	// TODO: ���⿡ �߰� �ʱ�ȭ �۾��� �߰��մϴ�.
	CStringList* newlist = new CStringList;
	arg1.pList = newlist;
	arg1.Thread_run = 1;
	arg1.pDlg = this;

	CStringList* newlist2 = new CStringList;
	arg2.pList = newlist2;
	arg2.Thread_run = 1;
	arg2.pDlg = this;

	m_pDataSocket = NULL;
	m_ipaddr.SetWindowTextW(_T("127.0.0.1"));

	WSADATA wsa;
	int error_code;
	if ((error_code = WSAStartup(MAKEWORD(2, 2), &wsa)) != 0)
	{
		TCHAR buffer[256];
		FormatMessageW(FORMAT_MESSAGE_FROM_SYSTEM, NULL, error_code, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), buffer, 256, NULL);
		AfxMessageBox(buffer, MB_ICONERROR);
	}

	pThread1 = AfxBeginThread(TXThread, (LPVOID)&arg1);	// TXThread ����
	pThread2 = AfxBeginThread(RXThread, (LPVOID)&arg2);	// RXThread ����
	
	return TRUE;  // ��Ŀ���� ��Ʈ�ѿ� �������� ������ TRUE�� ��ȯ�մϴ�.
}

void CLab09_TCPClientDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// ��ȭ ���ڿ� �ּ�ȭ ���߸� �߰��� ��� �������� �׸�����
//  �Ʒ� �ڵ尡 �ʿ��մϴ�.  ����/�� ���� ����ϴ� MFC ���� ���α׷��� ��쿡��
//  �����ӿ�ũ���� �� �۾��� �ڵ����� �����մϴ�.

void CLab09_TCPClientDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // �׸��⸦ ���� ����̽� ���ؽ�Ʈ�Դϴ�.

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Ŭ���̾�Ʈ �簢������ �������� ����� ����ϴ�.
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// �������� �׸��ϴ�.
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// ����ڰ� �ּ�ȭ�� â�� ���� ���ȿ� Ŀ���� ǥ�õǵ��� �ý��ۿ���
//  �� �Լ��� ȣ���մϴ�.
HCURSOR CLab09_TCPClientDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CLab09_TCPClientDlg::OnBnClickedConnect()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	if (m_pDataSocket == NULL)
	{
		m_pDataSocket = new CDataSocket(this);
		m_pDataSocket->Create();	// ���� ����
		CString addr;
		m_ipaddr.GetWindowText(addr);	// ip �ּҸ� ������
		if (m_pDataSocket->Connect(addr, 8000))	// addr�� ip�� 8000�� port�� ����
		{
			MessageBox(_T("������ ���� ����!"), _T("�˸�"), MB_ICONINFORMATION);
			m_tx_edit_short.SetFocus();
			CString str;
			str.Format(_T("### ������ ���� ����! ###\r\n"));
			m_tx_edit.SetWindowTextW(str);
		}
		else // ���� ���� �� ���
		{
			MessageBox(_T("������ ���� ����!"), _T("�˸�"), MB_ICONERROR);
			delete m_pDataSocket;
			m_pDataSocket = NULL;
		}
	}
	else // data������ null�� �ƴ� ��� �̹� ���ӵ� ���̹Ƿ�
	{
		MessageBox(_T("������ �̹� ���ӵ�!"), _T("�˸�"), MB_ICONINFORMATION);
		m_tx_edit_short.SetFocus();
	}
}


void CLab09_TCPClientDlg::OnBnClickedDisconnect()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	if (m_pDataSocket == NULL)
	{
		MessageBox(_T("������ ���� �� ��!"), _T("�˸�"), MB_ICONERROR);
	}
	else // ���ӵǾ� �ִ� ���
	{
		m_pDataSocket->Close();
		delete m_pDataSocket;
		m_pDataSocket = NULL; // data ���� ����
	}
}


void CLab09_TCPClientDlg::OnBnClickedSend()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	if (m_pDataSocket == NULL)
	{
		MessageBox(_T("������ ���� �� ��!"), _T("�˸�"), MB_ICONERROR);
	}
	else
	{
		CString message;
		m_tx_edit_short.GetWindowText(message);	// message�� �Է¹��� �޼��� ����
		message += _T("\r\n");
		tx_cs.Lock();
		arg1.pList->AddTail(message);	// pList ���κп� �Է¹��� �޼��� �߰�, �ٸ� �����尡 ����Ǹ� ������ �߻��� �� �����Ƿ� cs�� ����
		tx_cs.Unlock();
		m_tx_edit_short.SetWindowTextW(_T(""));	// �Է�ĭ���� �޼����� ������ �� ���
		m_tx_edit_short.SetFocus();

		int len = m_tx_edit.GetWindowTextLengthW();
		m_tx_edit.SetSel(len, len);
		m_tx_edit.ReplaceSel(message);
	}
}

void CLab09_TCPClientDlg::ProcessReceive(CDataSocket* pSocket, int nErrorCode)
{
	TCHAR pBuf[1024 + 1];
	CString strData;
	int nbytes;

	nbytes = pSocket->Receive(pBuf, 1024);	// pBuf�� �Է¹��� ������ ����
	pBuf[nbytes] = NULL;	// �������� NULL�� �־� ������ �˸�
	strData = (LPCTSTR)pBuf;	// strData�� pBuf�� ����� �����͸� LPCTSTR ���·� ����
	// AfxMessageBox(strData);

	rx_cs.Lock();
	arg2.pList->AddTail((LPCTSTR)strData);	// pList ���κп� ���۹��� �޼��� �߰�, �ٸ� �����尡 ����Ǹ� ������ �߻��� �� �����Ƿ� cs�� ����
	rx_cs.Unlock();
}
