#pragma once

#include <windows.h>
#include <winternl.h>





// �Լ�Ҫʵ�ֵĺ���
HANDLE WINAPI MyOpenProcess(DWORD dwDesiredAccess, BOOL bInheritHandle, DWORD dwProcessId);

BOOL WINAPI MyTerminateProcess(
	_In_ HANDLE hProcess,
	_In_ UINT uExitCode
);

// �ҹ�
void InHook();

// �ѹ�
void UnHook();

// �ҹ�
void TermInHook();

// �ѹ�
void TermUnHook();