// ListenSocket.cpp : ���� �����Դϴ�.
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


// CListenSocket ��� �Լ�


void CListenSocket::OnAccept(int nErrorCode)
{
	// TODO: ���⿡ Ư��ȭ�� �ڵ带 �߰� ��/�Ǵ� �⺻ Ŭ������ ȣ���մϴ�.

	CSocket::OnAccept(nErrorCode);
	m_pDlg->ProcessAccept(nErrorCode);
}
