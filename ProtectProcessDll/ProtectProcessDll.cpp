// ProtectProcessDll.cpp : 定义 DLL 应用程序的导出函数。
//

#include "stdafx.h"
#include "ProtectProcessDll.h"
#include <corecrt_wstdlib.h>

BYTE g_OldData64[12] = { 0 };
BYTE g_OldTerm64[12] = { 0 };
extern "C" __declspec(dllexport) DWORD g_dwPid = 0;

DWORD g_ProcessPid = 999;
// 挂钩
void InHook()
{	
	HANDLE hJuBing = NULL;
	HWND hWin = ::FindWindow(L"ConsoleWindowClass",NULL);	
	::GetWindowThreadProcessId(hWin, &g_ProcessPid);
#ifdef _WIN64
	// 64位下保存原函数地址的前12个字节
	memcpy(g_OldData64, OpenProcess, 12);

	// 64位函数调用汇编跳转如下
	// mov rax, 0x0011223344556677
	// jmp rax
	// 机器码
	// 48 b8 7766554433221100
	// ff e0
	BYTE newData[12] = { 0x48, 0xB8, 0, 0, 0, 0, 0, 0, 0, 0, 0xFF, 0xE0 };

	// 获取自己的MyOpenProcess函数地址
	ULONGLONG ullOffset = (ULONGLONG)MyOpenProcess;
	memcpy(&newData[2], &ullOffset, sizeof(ullOffset));

	// 设置内存页保护属性为 可读可写可执行
	DWORD dwProtect = 0;
	VirtualProtect(OpenProcess, 12, PAGE_EXECUTE_READWRITE, &dwProtect);
	// 将原函数修改为自己的函数
	memcpy(OpenProcess, newData, 12);
	// 将内存页属性改为它原来的
	VirtualProtect(OpenProcess, 12, dwProtect, &dwProtect);
#endif
}

// 脱钩
void UnHook()
{
#ifdef _WIN64	
	DWORD dwProtect = 0;
	VirtualProtect(OpenProcess, 12, PAGE_EXECUTE_READWRITE, &dwProtect);
	// 将原函数修改为自己的函数
	memcpy(OpenProcess, g_OldData64, 12);
	// 将内存页属性改为它原来的
	VirtualProtect(OpenProcess, 12, dwProtect, &dwProtect);
#endif
}

// Term挂钩
void TermInHook()
{
#ifdef _WIN64	
	// 64位下保存原函数地址的前12个字节
	memcpy(g_OldTerm64, TerminateProcess, 12);

	// 64位函数调用汇编跳转如下
	// mov rax, 0x0011223344556677
	// jmp rax
	// 机器码
	// 48 b8 7766554433221100
	// ff e0
	BYTE newData[12] = { 0x48, 0xB8, 0, 0, 0, 0, 0, 0, 0, 0, 0xFF, 0xE0 };

	// 获取自己的MyOpenProcess函数地址
	ULONGLONG ullOffset = (ULONGLONG)MyTerminateProcess;
	memcpy(&newData[2], &ullOffset, sizeof(ullOffset));

	// 设置内存页保护属性为 可读可写可执行
	DWORD dwProtect = 0;
	VirtualProtect(TerminateProcess, 12, PAGE_EXECUTE_READWRITE, &dwProtect);
	// 将原函数修改为自己的函数
	memcpy(TerminateProcess, newData, 12);
	// 将内存页属性改为它原来的
	VirtualProtect(TerminateProcess, 12, dwProtect, &dwProtect);
#endif
}

// Term脱钩
void TermUnHook()
{
#ifdef _WIN64	
	DWORD dwProtect = 0;
	VirtualProtect(TerminateProcess, 12, PAGE_EXECUTE_READWRITE, &dwProtect);
	// 将原函数修改为自己的函数
	memcpy(TerminateProcess, g_OldTerm64, 12);
	// 将内存页属性改为它原来的
	VirtualProtect(TerminateProcess, 12, dwProtect, &dwProtect);
#endif
}

// 自己的函数
HANDLE WINAPI MyOpenProcess(DWORD dwDesiredAccess, BOOL bInheritHandle, DWORD dwProcessId)
{
	// 用于返回的句柄
	HANDLE Handle = NULL;
	
	// 1. 一种是传入了需要保护的PID
	if (dwProcessId == g_ProcessPid && dwDesiredAccess == PROCESS_TERMINATE)
	{		
		TCHAR str[100] = {};
		_itow_s(g_ProcessPid, str, 10);
		OutputDebugString(L"进入OpenProcess了。");
		OutputDebugString(str);
		Handle = NULL;
		dwDesiredAccess |= PROCESS_QUERY_INFORMATION;
		MessageBox(NULL, L"OpenProcess", L"进程保护", NULL);
		return NULL;
	}
	// 2. 传入的是不需要保护的PID
	else
	{
		// 如果不进行还原，调用OpenProcess会导致无限递归
		UnHook();
		// 恢复之后重新调用原有函数
		Handle = OpenProcess(dwDesiredAccess, bInheritHandle, dwProcessId);
		// 恢复Hook，为了让下次的执行流程继续到我们的函数中
		InHook();
	}

	// 返回句柄
	return Handle;
}

BOOL WINAPI MyTerminateProcess(
	_In_ HANDLE hProcess,
	_In_ UINT uExitCode
)
{
	BOOL bRet = FALSE;
	// 因为只有自己的进程的句柄用PROCESS_QUERY_INFORMATION权限，才不会返回因为拒绝访问返回NULL
	DWORD dwProcessId = GetProcessId(hProcess);
	if (dwProcessId == g_ProcessPid)
	{
		TCHAR str[100] = {};
		_itow_s(g_ProcessPid, str, 10);
		OutputDebugString(L"进入TerminateProcess了。");
		OutputDebugString(str);
		// 发现是自己的ID就弹出对话框返回NULL 其他的正常结束
		::MessageBoxA(0,"TerminateProcess！","保护进程", 0);
		return bRet;
	}
	else
	{
		TermUnHook();
		bRet = TerminateProcess(hProcess, uExitCode);
		TermInHook();
	}
	return bRet;
}