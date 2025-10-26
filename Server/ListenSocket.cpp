// ListenSocket.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "Lab09_TCPServer.h"
#include "ListenSocket.h"
#include "Lab09_TCPServerDlg.h"


// CListenSocket

CListenSocket::CListenSocket(CLab09_TCPServerDlg *pDlg)
{
	m_pDlg = pDlg;
}

CListenSocket::~CListenSocket()
{
}


// CListenSocket 멤버 함수


void CListenSocket::OnAccept(int nErrorCode)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.

	CSocket::OnAccept(nErrorCode);
	m_pDlg->ProcessAccept(nErrorCode);
}
