#ifndef _HIDE_PROCESS_H_
#define _HIDE_PROCESS_H_



#include <Windows.h>
#include <Winternl.h>

extern DWORD g_ProcessId;

typedef NTSTATUS(*FnZwQuerySystemInformation)(
	SYSTEM_INFORMATION_CLASS SystemInformationClass,
	PVOID SystemInformation,
	ULONG SystemInformationLength,
	PULONG ReturnLength
	);


NTSTATUS MyZwQuerySystemInformation(
	SYSTEM_INFORMATION_CLASS SystemInformationClass,
	PVOID SystemInformation,
	ULONG SystemInformationLength,
	PULONG ReturnLength
);


void InHook();

void UnHook();

#endif