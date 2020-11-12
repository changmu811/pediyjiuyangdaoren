// Socket_TCP_Server.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include "pch.h"
#include <iostream>

#define _WINSOCK_DEPRECATED_NO_WARNINGS  // 需要定义，否则 3的sin_addr会错误

#include <WinSock2.h>
#pragma comment(lib, "Ws2_32.lib")


SOCKET g_acceptSocket; // 全局套接字

// 收发数据的缓冲区
char g_szSendBuffer[2048] = {}; 
char g_szRecvBuffer[512] = {};


// 重定向输出CMD数据
void ControlCmd();

int main()
{	
	printf("***我是服务端***\n请输入本机的IP地址：");
	char szServerIP[20] = {};
	scanf_s("%s", szServerIP, 20);
	// 1.初始化 Winsock 库
	WSADATA wd = {};
	int nSucess = WSAStartup(MAKEWORD(2, 2), &wd);
	if (nSucess != 0)
	{
		printf("WSAStartup失败！");
		return 0;
	}

	// 2.创建一个流式监听套接字
	SOCKET listenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (INVALID_SOCKET == listenSocket)
	{
		printf("socket失败！");
		WSACleanup();
		return 0;
	}

	// 3.绑定IP和端口
	sockaddr_in addr = {};
	addr.sin_family = AF_INET;
	addr.sin_port = htons(1234);   // 端口
	addr.sin_addr.S_un.S_addr = inet_addr(szServerIP);  // 服务器的IP地址
	nSucess = bind(listenSocket, (sockaddr*)&addr, sizeof(addr));
	if (nSucess == SOCKET_ERROR)
	{
		printf("bind失败！");
		closesocket(listenSocket);
		WSACleanup();
		return 0;
	}

	// 4.监听 1234 端口
	listen(listenSocket, 8); // 第二个参数是同时连接的最大数

	// 5.接受客户端连接
	sockaddr_in clientAddr = {};
	int nSize = sizeof(clientAddr);
	g_acceptSocket = accept(listenSocket, (sockaddr*)&addr, &nSize);

	// 6.接收指令后返回数据
	while (true)
	{
		Sleep(100);
		RtlZeroMemory(g_szRecvBuffer, sizeof(g_szRecvBuffer));
		int nRet = recv(g_acceptSocket, g_szRecvBuffer, 512, 0);
		if (nRet <= 0)
			continue;
		printf("已经接收到指令：%s\n", g_szRecvBuffer);
		// 操作CMD
		ControlCmd();
	}

	// 7.清理和关闭, 这里用不到，但其他地方...
	closesocket(g_acceptSocket);
	closesocket(listenSocket);
	WSACleanup();
}

// 重定向输出CMD数据
void ControlCmd()
{
	// 1.cmd 指令
	char szCmd[MAX_PATH] = {};
	strcpy_s(szCmd, strlen(g_szRecvBuffer) + 1, g_szRecvBuffer);

	// 2.设置管道的安全属性
	SECURITY_ATTRIBUTES sa = {};
	sa.bInheritHandle = TRUE;
	sa.nLength = sizeof(sa);
	sa.lpSecurityDescriptor = NULL;

	// 3.创建匿名管道
	HANDLE hReadPipe = NULL, hWritePipe = NULL;
	if (!CreatePipe(&hReadPipe, &hWritePipe, &sa, 0))
	{
		printf("创建管道失败！");
		return;
	}

	// 4.设置新进程的参数
	STARTUPINFOA si = {};
	si.cb = sizeof(si);
	si.dwFlags = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;
	si.wShowWindow = SW_HIDE;
	si.hStdError = hWritePipe;
	si.hStdOutput = hWritePipe;

	// 5.创建新进程执行cmd命令，执行的标准错误与标准输入会写入匿名管道内
	PROCESS_INFORMATION pi = {};
	BOOL bRet = CreateProcessA(NULL, szCmd, NULL, NULL, TRUE, 0, NULL, NULL, &si, &pi);
	if(!bRet)
	{
		printf("CreateProcessA失败！");
		return;
	}

	// 6.等待cmd命令执行结束
	WaitForSingleObject(pi.hThread, INFINITE);
	WaitForSingleObject(pi.hProcess, INFINITE);

	// 7.从匿名管道中读取结果输出到缓冲区
	RtlZeroMemory(g_szSendBuffer, sizeof(g_szSendBuffer));
	ReadFile(hReadPipe, g_szSendBuffer, sizeof(g_szSendBuffer), NULL, NULL);
	send(g_acceptSocket, g_szSendBuffer, sizeof(g_szSendBuffer), 0);
	printf("已发送数据如下：\n%s\n", g_szSendBuffer);
	memset(g_szRecvBuffer, 0, sizeof(g_szRecvBuffer));
	
	// 8.关闭句柄
	CloseHandle(pi.hThread);
	CloseHandle(pi.hProcess);
	CloseHandle(hWritePipe);
	CloseHandle(hReadPipe);
}