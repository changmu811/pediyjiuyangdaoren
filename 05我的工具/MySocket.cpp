#include "stdafx.h"
#include "MySocket.h"
#include "RemoteCmd.h"
//#include <afxsock.h>


MySocket::MySocket()
{
	/*
	// 用于初始化模块的结构体
	WSAData WsaData = { 0 };

	// 初始化模块并进行判断
	if (WSAStartup(MAKEWORD(2, 2), &WsaData))
	{
		MessageBox(NULL, L"初始化模块失败", L"错误信息", MB_OK | MB_ICONERROR);
		ExitProcess(-1);
	}*/

	// 初始化套接字
	AfxSocketInit();

	// 使用 Create 创建一个套接字
	if (!this->Create())
	{
		int index = GetLastError();
		MessageBox(NULL, L"创建套接字失败", L"错误信息", MB_OK | MB_ICONERROR);
		ExitProcess(-1);
	}
}


MySocket::~MySocket()
{
	// 清理套接字和模块
	this->Close();
	AfxSocketTerm();
}

void MySocket::OnReceive(int nErrorCode)
{
	// 定义数组并接收数据
	BYTE msg[2048] = {};
	int size = this->Receive(msg, 2048);

	// 获取远程窗口处理返回的字符串
	//RemoteCmd* dlg = (RemoteCmd*)AfxGetMainWnd();
	//dlg->ShowData(msg, size);
	RemoteCmd obj;
	obj.ShowData(msg, size);

	CSocket::OnReceive(nErrorCode);
}