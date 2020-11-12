#include "stdafx.h"

extern HMODULE g_hModule;

#pragma data_seg("mydata")
	HHOOK g_hHook = NULL;
	DWORD g_ProcessId = 999;
#pragma data_seg()
#pragma comment(linker, "/SECTION:mydata,RWS")

// ��Ϣ���ӻص�����
LRESULT CALLBACK MyHookProc(int code, WPARAM wParam, LPARAM lParam)
{
	// ʲô��������Ŀ�����ڽ�DLLȫ�ֹҹ�
	return ::CallNextHookEx(g_hHook, code, wParam, lParam);
}

// ��װȫ�ֹ���
HHOOK SetHook()
{	
	// ����ȫ�ֹ���
	HHOOK hHook = ::SetWindowsHookEx(WH_GETMESSAGE, (HOOKPROC)MyHookProc, g_hModule, 0);
	if (hHook == NULL)
		return NULL;
	g_hHook = hHook;
	return hHook;
}

// ж�ع���
BOOL UnloadHook(HHOOK hHook)
{
	if (UnhookWindowsHookEx(hHook) == FALSE)
		return FALSE;
	return TRUE;
}