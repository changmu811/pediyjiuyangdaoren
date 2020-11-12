#pragma once

// 错误提示
void ShowError(const wchar_t* szBuf);

// 远程线程注入
BOOL RemoteInjectDll(DWORD dwPid, const char* DllPath);

// 突破SESSION0隔离的远程线程注入
void AcrossSession0(DWORD dwPid, const char* DllPath);

// APC注入
void APCInject(DWORD dwPid, const char* DllPath);

// 根据PID获取该进程所有TID   
BOOL GetAllTidByPid(DWORD dwPid, DWORD** ppTid, DWORD* pTidNum);

// 卸载DLL
BOOL UnloadDll(DWORD dwPid, LPCTSTR szDllName);

// 全局注入
void GlobalInject(const wchar_t* DllPath, BOOL InjectOrUnLoad);

// 代码注入
void InjectCode(DWORD dwPid);