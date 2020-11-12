// Socket_TCP_Client.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include "pch.h"
#include <iostream>

#define  _WINSOCK_DEPRECATED_NO_WARNINGS
#include <WinSock2.h>
#pragma comment(lib, "ws2_32.lib")

SOCKET comSocket;
// 线程回调
DWORD WINAPI ThreadProc(LPVOID lpThreadParameter);

int main()
{
	printf("***我是客户端***\n");
	// 1.初始化winsock库
	WSADATA wd = {};
	int nSucess = WSAStartup(MAKEWORD(2, 2), &wd);
	if (nSucess != 0)
	{
		printf("WSAStartup失败！");
		return 0;
	}

	// 2.创建一个流式通讯套接字
	comSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (INVALID_SOCKET == comSocket)
	{
		printf("socket失败！");
		WSACleanup();
		return 0;
	}

	// 3.开始连接服务器
	sockaddr_in addr = {};
	addr.sin_family = AF_INET;
	addr.sin_port = htons(1234);   // 端口
	addr.sin_addr.S_un.S_addr = inet_addr("192.168.1.105");  // 服务端IP
	//addr.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");  // 服务端IP
	nSucess = connect(comSocket, (sockaddr*)&addr, sizeof(addr));
	if(nSucess != 0)
	{
		printf("connect失败！");		
		closesocket(comSocket);
		WSACleanup();
		return 0;		
	}

	// 5.创建线程发送数据
	CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)ThreadProc, NULL, 0, NULL);

	// 6.接受数据
	while (true)
	{
		char szBuffer[1024] = {};
		int nRet = recv(comSocket, szBuffer, 1024, 0);
		if (nRet <= 0) 
			continue;
		printf("\n[other]>>> %s\n[me]: ", szBuffer);
	}

	// 7.关闭
	closesocket(comSocket);
	WSACleanup();		
	return 0;
}

// 线程回调
DWORD WINAPI ThreadProc(LPVOID lpThreadParameter)
{	
	while (true)
	{
		char szBuffer[1024] = {};
		printf("[me]: ");
		scanf_s("%s", szBuffer, 1024);		
		send(comSocket, szBuffer, 1024, 0);				
	}
	return 0;
}