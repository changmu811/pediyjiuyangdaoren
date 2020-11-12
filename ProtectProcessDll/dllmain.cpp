﻿// dllmain.cpp : 定义 DLL 应用程序的入口点。
#include "stdafx.h"
#include "ProtectProcessDll.h"

HMODULE g_hModule;

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
		g_hModule = hModule;
		InHook();
		TermInHook();
		break;
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
		UnHook();
		TermUnHook();
        break;
    }
    return TRUE;
}

