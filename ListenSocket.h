#pragma once

// CListenSocket 명령 대상입니다.

class CLab09_TCPServerDlg;

class CListenSocket : public CSocket
{
public:
	CListenSocket(CLab09_TCPServerDlg *pDlg);
	virtual ~CListenSocket();
	CLab09_TCPServerDlg *m_pDlg;
	virtual void OnAccept(int nErrorCode);
};


