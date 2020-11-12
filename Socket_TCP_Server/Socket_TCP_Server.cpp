// Socket_TCP_Server.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include "pch.h"
#include <iostream>

#define _WINSOCK_DEPRECATED_NO_WARNINGS  // 需要定义，否则 3的sin_addr会错误

#include <WinSock2.h>
#pragma comment(lib, "Ws2_32.lib")


SOCKET g_acceptSocket;
// 线程回调
DWORD WINAPI ThreadProc(LPVOID lpThreadParameter);

int main()
{
	printf("***我是服务端***\n");
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
	addr.sin_addr.S_un.S_addr = inet_addr("192.168.211.128");  // 服务器的IP地址
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

	// 6.创建线程接受数据
	CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)ThreadProc, NULL, 0, NULL);

	// 7.发送数据
	while (true)
	{
		char szBuffer[1024] = {};
		printf("[me]: ");
		scanf_s("%s", szBuffer, 1024);
		send(g_acceptSocket, szBuffer, 1024, 0);
		Sleep(100);
	}
	   
	// 8.清理和关闭, 这里用不到，但其他地方...
	closesocket(g_acceptSocket);
	closesocket(listenSocket);
	WSACleanup();
}

// 线程回调
DWORD WINAPI ThreadProc(LPVOID lpThreadParameter)
{
	while (true)
	{
		char szBuffer[1024] = {};
		int nRet = recv(g_acceptSocket, szBuffer, 1024, 0);
		if(nRet <= 0) 
			continue;
		printf("\n[other]>>> %s\n[me]: ", szBuffer);	
		Sleep(100);
	}	

	return 0;
}
