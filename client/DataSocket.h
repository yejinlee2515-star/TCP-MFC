#pragma once

// CDataSocket ��� ����Դϴ�.

class CLab09_TCPClientDlg;

class CDataSocket : public CSocket
{
public:
	CDataSocket(CLab09_TCPClientDlg *pDlg);
	virtual ~CDataSocket();
	CLab09_TCPClientDlg *m_pDlg;
	virtual void OnReceive(int nErrorCode);
};


