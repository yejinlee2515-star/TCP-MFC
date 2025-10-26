// DataSocket.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "Lab09_TCPClient.h"
#include "DataSocket.h"
#include "Lab09_TCPClientDlg.h"


// CDataSocket

CDataSocket::CDataSocket(CLab09_TCPClientDlg *pDlg)
{
	m_pDlg = pDlg;
}

CDataSocket::~CDataSocket()
{
}


// CDataSocket ��� �Լ�


void CDataSocket::OnReceive(int nErrorCode)
{
	// TODO: ���⿡ Ư��ȭ�� �ڵ带 �߰� ��/�Ǵ� �⺻ Ŭ������ ȣ���մϴ�.

	CSocket::OnReceive(nErrorCode);	// �޼����� ��� ���� �� �ֵ��� OnReceive�Լ� ���
	m_pDlg->ProcessReceive(this, nErrorCode);
}
