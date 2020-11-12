#include "stdafx.h"
#include "MySocket.h"
#include "RemoteCmd.h"
//#include <afxsock.h>


MySocket::MySocket()
{
	/*
	// ���ڳ�ʼ��ģ��Ľṹ��
	WSAData WsaData = { 0 };

	// ��ʼ��ģ�鲢�����ж�
	if (WSAStartup(MAKEWORD(2, 2), &WsaData))
	{
		MessageBox(NULL, L"��ʼ��ģ��ʧ��", L"������Ϣ", MB_OK | MB_ICONERROR);
		ExitProcess(-1);
	}*/

	// ��ʼ���׽���
	AfxSocketInit();

	// ʹ�� Create ����һ���׽���
	if (!this->Create())
	{
		int index = GetLastError();
		MessageBox(NULL, L"�����׽���ʧ��", L"������Ϣ", MB_OK | MB_ICONERROR);
		ExitProcess(-1);
	}
}


MySocket::~MySocket()
{
	// �����׽��ֺ�ģ��
	this->Close();
	AfxSocketTerm();
}

void MySocket::OnReceive(int nErrorCode)
{
	// �������鲢��������
	BYTE msg[2048] = {};
	int size = this->Receive(msg, 2048);

	// ��ȡԶ�̴��ڴ����ص��ַ���
	//RemoteCmd* dlg = (RemoteCmd*)AfxGetMainWnd();
	//dlg->ShowData(msg, size);
	RemoteCmd obj;
	obj.ShowData(msg, size);

	CSocket::OnReceive(nErrorCode);
}