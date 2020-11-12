#include "stdafx.h"

extern HMODULE g_hModule;

#pragma data_seg(".Jydr")
HHOOK g_hHook = NULL;
#pragma data_seg()
#pragma comment(linker, "/SECTION:.Jydr,RWS")

// ��Ϣ���ӻص�����
LRESULT CALLBACK MyHookProc(int code, WPARAM wParam, LPARAM lParam)
{
	// ʲô��������Ŀ�����ڽ�DLLȫ�ֹҹ�
	return ::CallNextHookEx(g_hHook, code, wParam, lParam);
}

// ��װȫ�ֹ���
extern "C" __declspec(dllexport) HHOOK SetHook()
{
	// ����ȫ�ֹ���
	HHOOK hHook = ::SetWindowsHookEx(WH_GETMESSAGE, (HOOKPROC)MyHookProc, g_hModule, 0);
	if (!hHook)
		return NULL;
	g_hHook = hHook;
	return hHook;
}

// ж�ع���
extern "C" __declspec(dllexport) BOOL UnloadHook(HHOOK hHook)
{
	if (!UnhookWindowsHookEx(hHook))
		return FALSE;
	return TRUE;
}