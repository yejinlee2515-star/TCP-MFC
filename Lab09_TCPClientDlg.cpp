
// Lab09_TCPClientDlg.cpp : 구현 파일
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


// CLab09_TCPClientDlg 대화 상자



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


// CLab09_TCPClientDlg 메시지 처리기
UINT RXThread(LPVOID arg)
{
	ThreadArg *pArg = (ThreadArg*)arg;
	CStringList *plist = pArg->pList;	// 전달받은 pLIst 저장
	CLab09_TCPClientDlg *pDlg = (CLab09_TCPClientDlg *)pArg->pDlg;	// 대화상자를 가리키는 변수
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
			pDlg->m_rx_edit.SetWindowTextW(message);	// 화면에 나타나도록 함
			pDlg->m_rx_edit.LineScroll(pDlg->m_rx_edit.GetLineCount());	// 할당된 칸을 모두 사용하면 자동으로 스크롤 되도록 함

			plist->RemoveAt(current_pos);	// 처리를 완료하였으므로 현재 메세지 삭제 
		}
		Sleep(10);
	}
	return 0;
}

UINT TXThread(LPVOID arg)
{
	ThreadArg *pArg = (ThreadArg*)arg;
	CStringList *plist = pArg->pList;	// 입력받은 pLIst 저장
	CLab09_TCPClientDlg *pDlg = (CLab09_TCPClientDlg *)pArg->pDlg;	// 대화상자를 가리키는 변수
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

			plist->RemoveAt(current_pos);	// 처리를 완료하였으므로 현재 메세지 삭제 
		}
		Sleep(10);
	}
	return 0;
}

BOOL CLab09_TCPClientDlg::OnInitDialog()
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

	pThread1 = AfxBeginThread(TXThread, (LPVOID)&arg1);	// TXThread 시작
	pThread2 = AfxBeginThread(RXThread, (LPVOID)&arg2);	// RXThread 시작
	
	return TRUE;  // 포커스를 컨트롤에 설정하지 않으면 TRUE를 반환합니다.
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

// 대화 상자에 최소화 단추를 추가할 경우 아이콘을 그리려면
//  아래 코드가 필요합니다.  문서/뷰 모델을 사용하는 MFC 응용 프로그램의 경우에는
//  프레임워크에서 이 작업을 자동으로 수행합니다.

void CLab09_TCPClientDlg::OnPaint()
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
HCURSOR CLab09_TCPClientDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CLab09_TCPClientDlg::OnBnClickedConnect()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if (m_pDataSocket == NULL)
	{
		m_pDataSocket = new CDataSocket(this);
		m_pDataSocket->Create();	// 소켓 생성
		CString addr;
		m_ipaddr.GetWindowText(addr);	// ip 주소를 가져옴
		if (m_pDataSocket->Connect(addr, 8000))	// addr의 ip와 8000번 port에 연결
		{
			MessageBox(_T("서버에 접속 성공!"), _T("알림"), MB_ICONINFORMATION);
			m_tx_edit_short.SetFocus();
			CString str;
			str.Format(_T("### 서버에 접속 성공! ###\r\n"));
			m_tx_edit.SetWindowTextW(str);
		}
		else // 연결 실패 할 경우
		{
			MessageBox(_T("서버에 접속 실패!"), _T("알림"), MB_ICONERROR);
			delete m_pDataSocket;
			m_pDataSocket = NULL;
		}
	}
	else // data소켓이 null이 아닐 경우 이미 접속된 것이므로
	{
		MessageBox(_T("서버에 이미 접속됨!"), _T("알림"), MB_ICONINFORMATION);
		m_tx_edit_short.SetFocus();
	}
}


void CLab09_TCPClientDlg::OnBnClickedDisconnect()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if (m_pDataSocket == NULL)
	{
		MessageBox(_T("서버에 접속 안 함!"), _T("알림"), MB_ICONERROR);
	}
	else // 접속되어 있는 경우
	{
		m_pDataSocket->Close();
		delete m_pDataSocket;
		m_pDataSocket = NULL; // data 소켓 삭제
	}
}


void CLab09_TCPClientDlg::OnBnClickedSend()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if (m_pDataSocket == NULL)
	{
		MessageBox(_T("서버에 접속 안 함!"), _T("알림"), MB_ICONERROR);
	}
	else
	{
		CString message;
		m_tx_edit_short.GetWindowText(message);	// message에 입력받은 메세지 저장
		message += _T("\r\n");
		tx_cs.Lock();
		arg1.pList->AddTail(message);	// pList 끝부분에 입력받은 메세지 추가, 다른 쓰레드가 실행되면 오류가 발생할 수 있으므로 cs에 넣음
		tx_cs.Unlock();
		m_tx_edit_short.SetWindowTextW(_T(""));	// 입력칸에서 메세지를 가져온 후 비움
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

	nbytes = pSocket->Receive(pBuf, 1024);	// pBuf에 입력받은 데이터 저장
	pBuf[nbytes] = NULL;	// 마지막은 NULL을 넣어 끝임을 알림
	strData = (LPCTSTR)pBuf;	// strData에 pBuf에 저장된 데이터를 LPCTSTR 형태로 저장
	// AfxMessageBox(strData);

	rx_cs.Lock();
	arg2.pList->AddTail((LPCTSTR)strData);	// pList 끝부분에 전송받은 메세지 추가, 다른 쓰레드가 실행되면 오류가 발생할 수 있으므로 cs에 넣음
	rx_cs.Unlock();
}
