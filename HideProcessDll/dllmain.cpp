// dllmain.cpp : 定义 DLL 应用程序的入口点。
#include "stdafx.h"
#include "HideProcessDll.h"

HMODULE g_hModule;
//extern DWORD g_ProcessId;

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:	
	{
		// 获取到名为"九阳道人"的信号量
		HANDLE hSemaphore = ::OpenSemaphore(
			SEMAPHORE_ALL_ACCESS, FALSE, L"九阳道人");
		// 把信号减1，锁定当前线程
		::WaitForSingleObject(hSemaphore, -1);
		LONG lSemaphore = 99;
		// 把信号加1	
		::ReleaseSemaphore(hSemaphore, 1, &lSemaphore);
		g_ProcessId = lSemaphore + 1;
		InHook();
		g_hModule = hModule;
		
		break;
	}
    case DLL_THREAD_ATTACH:
		break;
    case DLL_THREAD_DETACH:
		break;
    case DLL_PROCESS_DETACH:
	{
		UnHook();
		break;
	}
	default:break;
    }
    return TRUE;
}

