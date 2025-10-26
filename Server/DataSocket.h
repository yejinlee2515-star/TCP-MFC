#pragma once

// CDataSocket 명령 대상입니다.

class CLab09_TCPServerDlg;

class CDataSocket : public CSocket
{
public:
	CDataSocket(CLab09_TCPServerDlg *pDlg);
	virtual ~CDataSocket();
	CLab09_TCPServerDlg *m_pDlg;
	virtual void OnReceive(int nErrorCode);
	virtual void OnClose(int nErrorCode);
};


