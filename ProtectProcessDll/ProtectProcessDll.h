#pragma once

#include <windows.h>
#include <winternl.h>





// 自己要实现的函数
HANDLE WINAPI MyOpenProcess(DWORD dwDesiredAccess, BOOL bInheritHandle, DWORD dwProcessId);

BOOL WINAPI MyTerminateProcess(
	_In_ HANDLE hProcess,
	_In_ UINT uExitCode
);

// 挂钩
void InHook();

// 脱钩
void UnHook();

// 挂钩
void TermInHook();

// 脱钩
void TermUnHook();