#pragma once
#include <winternl.h>

class HidePretend
{
public:
	HidePretend();
	~HidePretend();
};

// ���庯������
typedef NTSTATUS(NTAPI* MyNtQueryInformationProcess)(
	IN HANDLE ProcessHandle,
	IN PROCESSINFOCLASS ProcessInformationClass,
	OUT PVOID ProcessInformation,
	IN ULONG ProcessInformationLength,
	OUT PULONG ReturnLength OPTIONAL
	);

// αװ����
void DisguiseProcess(DWORD Pid, LPWSTR szPath, LPWSTR szName);

// ���ܽ���
void Puppet();

// ����֮�ҹ�
void HideHook(BOOL IsWho, DWORD dwPid);

// ����֮�ѹ�
void HideUnHook();