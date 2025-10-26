
// Lab09_TCPServerDlg.cpp : 구현 파일
//

#include "stdafx.h"
#include "Lab09_TCPServer.h"
#include "Lab09_TCPServerDlg.h"
#include "afxdialogex.h"
#include "ListenSocket.h"
#include "DataSocket.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CCriticalSection tx_cs;
CCriticalSection rx_cs;

// 응용 프로그램 정보에 사용되는 CAboutDlg 대화 상자입니다.

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

// 구현입니다.
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


// CLab09_TCPServerDlg 대화 상자



CLab09_TCPServerDlg::CLab09_TCPServerDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_LAB09_TCPSERVER_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CLab09_TCPServerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT1, m_tx_edit_short);
	DDX_Control(pDX, IDC_EDIT2, m_rx_edit);
	DDX_Control(pDX, IDC_EDIT3, m_tx_edit);
}

BEGIN_MESSAGE_MAP(CLab09_TCPServerDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_SEND, &CLab09_TCPServerDlg::OnBnClickedSend)
	ON_BN_CLICKED(IDC_DISCONNECT, &CLab09_TCPServerDlg::OnBnClickedDisconnect)
END_MESSAGE_MAP()


// CLab09_TCPServerDlg 메시지 처리기
UINT RXThread(LPVOID arg)
{
	// 파라미터 초기화
	ThreadArg *pArg = (ThreadArg*)arg;
	CStringList *plist = pArg->pList;	// 전달받은 pLIst 저장
	CLab09_TCPServerDlg *pDlg = (CLab09_TCPServerDlg *)pArg->pDlg;	// 대화상자를 가리키는 변수

	while (pArg->Thread_run)	// Thread가 run인 동안 실행하도록 함
	{
		POSITION pos = plist->GetHeadPosition();
		POSITION current_pos;
		while (pos != NULL)
		{
			current_pos = pos;	// pList의 가장 앞
			rx_cs.Lock();	// cs
			CString str = plist->GetNext(pos);	// 현재 위치가 바뀔 수 있으므로 cs에 넣어 줌
			rx_cs.Unlock();	// cs 해제

			CString message;
			pDlg->m_rx_edit.GetWindowText(message);
			message += str;
			pDlg->m_rx_edit.SetWindowText(message);	// 화면에 출력
			pDlg->m_rx_edit.LineScroll(pDlg->m_rx_edit.GetLineCount());	// 할당된 칸을 모두 사용하면 자동으로 스크롤 되도록 함

			plist->RemoveAt(current_pos);	// 현재 메세지 삭제 
		}
		Sleep(10);
	}
	return 0;
}

UINT TXThread(LPVOID arg)
{
	ThreadArg *pArg = (ThreadArg*)arg;
	CStringList *plist = pArg->pList;	// 입력받은 pLIst 저장
	CLab09_TCPServerDlg *pDlg = (CLab09_TCPServerDlg *)pArg->pDlg;	// 대화상자를 가리키는 변수
	while (pArg->Thread_run)	// Thread가 run인 동안 실행하도록 함
	{
		POSITION pos = plist->GetHeadPosition();
		POSITION current_pos;
		while (pos != NULL)
		{
			current_pos = pos;	// pList의 가장 앞
			tx_cs.Lock();	// cs
			CString str = plist->GetNext(pos);	// 현재 위치가 바뀔 수 있으므로 cs에 넣어 줌
			tx_cs.Unlock();	// cs 해제
			pDlg->m_pDataSocket->Send((LPCTSTR)str, (str.GetLength() + 1) * sizeof(TCHAR));	// 메세지 전송
			pDlg->m_tx_edit.LineScroll(pDlg->m_tx_edit.GetLineCount());

			plist->RemoveAt(current_pos);	// 현재 메세지 삭제 


		}
		Sleep(10);
	}
	return 0;
}

BOOL CLab09_TCPServerDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 시스템 메뉴에 "정보..." 메뉴 항목을 추가합니다.

	// IDM_ABOUTBOX는 시스템 명령 범위에 있어야 합니다.
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

	// 이 대화 상자의 아이콘을 설정합니다.  응용 프로그램의 주 창이 대화 상자가 아닐 경우에는
	//  프레임워크가 이 작업을 자동으로 수행합니다.
	SetIcon(m_hIcon, TRUE);			// 큰 아이콘을 설정합니다.
	SetIcon(m_hIcon, FALSE);		// 작은 아이콘을 설정합니다.

	// TODO: 여기에 추가 초기화 작업을 추가합니다.
	CStringList* newlist = new CStringList;
	arg1.pList = newlist;
	arg1.Thread_run = 1;
	arg1.pDlg = this;

	CStringList* newlist2 = new CStringList;
	arg2.pList = newlist2;
	arg2.Thread_run = 1;
	arg2.pDlg = this;

	WSADATA wsa;
	int error_code;
	if ((error_code = WSAStartup(MAKEWORD(2, 2), &wsa)) != 0)
	{
		TCHAR buffer[256];
		FormatMessageW(FORMAT_MESSAGE_FROM_SYSTEM, NULL, error_code, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), buffer, 256, NULL);
		AfxMessageBox(buffer, MB_ICONERROR);
	}
	m_pListenSocket = NULL;
	m_pDataSocket = NULL;
	ASSERT(m_pListenSocket == NULL);
	m_pListenSocket = new CListenSocket(this);
	if (m_pListenSocket->Create(8000))
	{
		if (m_pListenSocket->Listen())
		{
			AfxMessageBox(_T("서버를 시작합니다."), MB_ICONINFORMATION);
			pThread1 = AfxBeginThread(TXThread, (LPVOID)&arg1);	// TXThread 시작
			pThread2 = AfxBeginThread(RXThread, (LPVOID)&arg2);	// RXThread 시작
			return TRUE;
		}
	}
	else
	{
		int err = m_pListenSocket->GetLastError();
		TCHAR buffer[256];
		FormatMessageW(FORMAT_MESSAGE_FROM_SYSTEM, NULL, err, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), buffer, 256, NULL);
		AfxMessageBox(buffer, MB_ICONERROR);
	}

	AfxMessageBox(_T("이미 실행중인 서버가 있습니다.") _T("\n프로그램을 종료합니다."), MB_ICONERROR);

	return FALSE;  
}

void CLab09_TCPServerDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

// 대화 상자에 최소화 단추를 추가할 경우 아이콘을 그리려면
//  아래 코드가 필요합니다.  문서/뷰 모델을 사용하는 MFC 응용 프로그램의 경우에는
//  프레임워크에서 이 작업을 자동으로 수행합니다.

void CLab09_TCPServerDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 그리기를 위한 디바이스 컨텍스트입니다.

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 클라이언트 사각형에서 아이콘을 가운데에 맞춥니다.
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 아이콘을 그립니다.
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// 사용자가 최소화된 창을 끄는 동안에 커서가 표시되도록 시스템에서
//  이 함수를 호출합니다.
HCURSOR CLab09_TCPServerDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CLab09_TCPServerDlg::ProcessAccept(int nErrorCode)
{
	CString PeerAddr;
	UINT PeerPort;
	CString str;

	ASSERT(nErrorCode == 0);
	if (m_pDataSocket == NULL)
	{
		m_pDataSocket = new CDataSocket(this);
		if (m_pListenSocket->Accept(*m_pDataSocket))
		{
			m_pDataSocket->GetPeerName(PeerAddr, PeerPort);
			str.Format(_T("### IP주소: %s, 포트번호: %d ###\r\n"), PeerAddr, PeerPort);
			m_rx_edit.SetWindowTextW(str);
		}
		else
		{
			delete m_pDataSocket;
			m_pDataSocket = NULL;
		}
	}
}


void CLab09_TCPServerDlg::ProcessReceive(CDataSocket* pSocket, int nErrorCode)
{
	TCHAR pBuf[1024 + 1];
	CString strData;
	int nbytes;

	nbytes = pSocket->Receive(pBuf, 1024);
	pBuf[nbytes] = NULL;
	strData = (LPCTSTR)pBuf;
	// AfxMessageBox(strData);

	rx_cs.Lock();
	arg2.pList->AddTail((LPCTSTR)strData);
	rx_cs.Unlock();

	// rx_event.SetEvent();
}
void CLab09_TCPServerDlg::ProcessClose(CDataSocket* pSocket, int nErrorCode)
{
	pSocket->Close();
	delete m_pDataSocket;
	m_pDataSocket = NULL;

	int len = m_rx_edit.GetWindowTextLengthW();
	CString message = _T("### 접속 종료 ###\r\n\r\n");
	m_rx_edit.SetSel(len, len);
	m_rx_edit.ReplaceSel(message);
}


void CLab09_TCPServerDlg::OnBnClickedSend()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CString tx_message;
	m_tx_edit_short.GetWindowText(tx_message);
	tx_message += _T("\r\n");
	tx_cs.Lock();
	arg1.pList->AddTail(tx_message);
	tx_cs.Unlock();
	m_tx_edit_short.SetWindowTextW(_T(""));
	m_tx_edit_short.SetFocus();

	int len = m_tx_edit.GetWindowTextLengthW();
	m_tx_edit.SetSel(len, len);
	m_tx_edit.ReplaceSel(tx_message);
}


void CLab09_TCPServerDlg::OnBnClickedDisconnect()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if (m_pDataSocket == NULL)
	{
		AfxMessageBox(_T("이미 접속 종료"));
	}
	else
	{
		arg1.Thread_run = 0;
		arg2.Thread_run = 0;
		m_pDataSocket->Close();
		delete m_pDataSocket;
		m_pDataSocket = NULL;
		m_pListenSocket->Close();
		delete m_pListenSocket;
		m_pListenSocket = NULL;
	}
}
