#pragma once
#include <winternl.h>

class HidePretend
{
public:
	HidePretend();
	~HidePretend();
};

// 定义函数类型
typedef NTSTATUS(NTAPI* MyNtQueryInformationProcess)(
	IN HANDLE ProcessHandle,
	IN PROCESSINFOCLASS ProcessInformationClass,
	OUT PVOID ProcessInformation,
	IN ULONG ProcessInformationLength,
	OUT PULONG ReturnLength OPTIONAL
	);

// 伪装进程
void DisguiseProcess(DWORD Pid, LPWSTR szPath, LPWSTR szName);

// 傀儡进程
void Puppet();

// 隐藏之挂钩
void HideHook(BOOL IsWho, DWORD dwPid);

// 隐藏之脱钩
void HideUnHook();