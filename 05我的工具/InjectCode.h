#pragma once

// ������ʾ
void ShowError(const wchar_t* szBuf);

// Զ���߳�ע��
BOOL RemoteInjectDll(DWORD dwPid, const char* DllPath);

// ͻ��SESSION0�����Զ���߳�ע��
void AcrossSession0(DWORD dwPid, const char* DllPath);

// APCע��
void APCInject(DWORD dwPid, const char* DllPath);

// ����PID��ȡ�ý�������TID   
BOOL GetAllTidByPid(DWORD dwPid, DWORD** ppTid, DWORD* pTidNum);

// ж��DLL
BOOL UnloadDll(DWORD dwPid, LPCTSTR szDllName);

// ȫ��ע��
void GlobalInject(const wchar_t* DllPath, BOOL InjectOrUnLoad);

// ����ע��
void InjectCode(DWORD dwPid);