#include "stdafx.h"

extern HMODULE g_hModule;

#pragma data_seg(".Jydr")
HHOOK g_hHook = NULL;
#pragma data_seg()
#pragma comment(linker, "/SECTION:.Jydr,RWS")

// 消息钩子回调函数
LRESULT CALLBACK MyHookProc(int code, WPARAM wParam, LPARAM lParam)
{
	// 什么都不做，目的在于将DLL全局挂钩
	return ::CallNextHookEx(g_hHook, code, wParam, lParam);
}

// 安装全局钩子
extern "C" __declspec(dllexport) HHOOK SetHook()
{
	// 设置全局钩子
	HHOOK hHook = ::SetWindowsHookEx(WH_GETMESSAGE, (HOOKPROC)MyHookProc, g_hModule, 0);
	if (!hHook)
		return NULL;
	g_hHook = hHook;
	return hHook;
}

// 卸载钩子
extern "C" __declspec(dllexport) BOOL UnloadHook(HHOOK hHook)
{
	if (!UnhookWindowsHookEx(hHook))
		return FALSE;
	return TRUE;
}