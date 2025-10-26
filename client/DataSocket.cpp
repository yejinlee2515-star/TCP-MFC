// DataSocket.cpp : 구현 파일입니다.
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


// CDataSocket 멤버 함수


void CDataSocket::OnReceive(int nErrorCode)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.

	CSocket::OnReceive(nErrorCode);	// 메세지를 계속 받을 수 있도록 OnReceive함수 사용
	m_pDlg->ProcessReceive(this, nErrorCode);
}
