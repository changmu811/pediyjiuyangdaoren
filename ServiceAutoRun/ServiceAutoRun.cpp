// ServiceAutoRun.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include "pch.h"
#include <Windows.h>
#include <WtsApi32.h>
#include <Userenv.h>
#include <stdio.h>
#include <locale.h>

#pragma comment(lib, "Wtsapi32.lib")
#pragma comment(lib, "Userenv.lib")

// 服务入口函数以及处理回调函数
void __stdcall ServiceMain(DWORD dwArgc, char *lpszArgv);
void __stdcall ServiceCtrlHandle(DWORD dwOperateCode);
BOOL TellSCM(DWORD dwState, DWORD dwExitCode, DWORD dwProgress);
void DoTask();
BOOL OpenUserProcedure(LPWSTR szExePath);

// 全局变量
TCHAR g_szServiceName[MAX_PATH] = L"ServiceJydr";    // 服务名称 
SERVICE_STATUS_HANDLE g_ServiceStatusHandle = { 0 };

TCHAR g_szName[MAX_PATH] = {}; // 要打开程序的路径

void ErrorHint(const char* szError)
{
	char szTitle[50] = "自启服务提示";
	char szText[50] = {};
	strcat_s(szText, 50, szError);

	DWORD dwOut = 0;
	WTSSendMessageA(WTS_CURRENT_SERVER_HANDLE, WTSGetActiveConsoleSessionId(), szTitle, strlen(szTitle) + 1,
		szText, strlen(szText) + 1, 0, 0, &dwOut, FALSE);
}

void SaveFile()
{
	char PathName[MAX_PATH] = {};
	char Path[MAX_PATH] = {};
	GetModuleFileNameA(NULL, PathName, MAX_PATH);
	for (int i = 0; i < (int)(strlen(PathName) + 1); i++)
	{
		if (!strcmp(PathName + i, "ServiceAuto.exe"))
			break;
		Path[i] = PathName[i];
	}
	strcat_s(Path, "AutoTest.jydr");

	//读取配置
	FILE * pFile;
	fopen_s(&pFile, Path, "wb+");
	if (NULL == pFile)
	{
		OutputDebugStringA("打开文件失败！");
		return;
	}
	char cPath[MAX_PATH] = "asdfasdfklj\\asdfjaslkdfjaskjdfhkajsdhfkjahsdkfjhaksjdfnzxcv,mnaqwieury??\\asdfasdf.frr";
	fwrite(&cPath, sizeof(cPath), 1, pFile);
	fclose(pFile);

	/*
	// 保存原来的语言系列
	char* old_locale = _strdup(setlocale(LC_CTYPE, NULL));
	// 设置新的语言系列
	setlocale(LC_CTYPE, setlocale(LC_ALL, ""));

	// 获取要自启动程序的路径
	size_t st = 0;
	mbstowcs_s(&st, g_szName, MAX_PATH, cPath, _TRUNCATE);

	// 还原语言系列  
	setlocale(LC_CTYPE, old_locale);
	free(old_locale);*/
}


int main(int argc, char* argv[])
{	
	char PathName[MAX_PATH] = {};
	char Path[MAX_PATH] = {};
	GetModuleFileNameA(NULL, PathName, MAX_PATH);	
	for (int i = 0; i < (int)(strlen(PathName) + 1); i++)
	{
		if (!strcmp(PathName + i, "ServiceAuto.exe"))
			break;
		Path[i] = PathName[i];
	}
	strcat_s(Path, "AutoConfig.jydr");
	
	//读取配置
	FILE * pFile;
	fopen_s(&pFile, Path, "rb");
	if (NULL == pFile)
	{
		OutputDebugStringA("打开文件失败！");
		return 0;
	}
	char cPath[MAX_PATH] = {};
	fread(&cPath, sizeof(cPath), 1, pFile);
	fclose(pFile);

	// 保存原来的语言系列
	char* old_locale = _strdup(setlocale(LC_CTYPE, NULL));
	// 设置新的语言系列
	setlocale(LC_CTYPE, setlocale(LC_ALL, ""));
	
	// 获取要自启动程序的路径
	size_t st = 0;
	mbstowcs_s(&st, g_szName, MAX_PATH, cPath, _TRUNCATE);

	// 还原语言系列  
	setlocale(LC_CTYPE, old_locale);
	free(old_locale);
	
	// 注册服务入口函数
	SERVICE_TABLE_ENTRY stDispatchTable[] = { { g_szServiceName, (LPSERVICE_MAIN_FUNCTION)ServiceMain }, { NULL, NULL } };
	::StartServiceCtrlDispatcher(stDispatchTable);

	return 0;
}

void __stdcall ServiceMain(DWORD dwArgc, char *lpszArgv)
{
	g_ServiceStatusHandle = ::RegisterServiceCtrlHandler(g_szServiceName, ServiceCtrlHandle);

	TellSCM(SERVICE_START_PENDING, 0, 1);
	TellSCM(SERVICE_RUNNING, 0, 0);

	// 自己程序实现部分代码放在这里
	// 此处一定要为死循环, 否则在关机再开机的情况, 不能打开用户进程
	while (TRUE)
	{
		DoTask();
		Sleep(20000);
	}
}

void __stdcall ServiceCtrlHandle(DWORD dwOperateCode)
{
	switch (dwOperateCode)
	{
	case SERVICE_CONTROL_PAUSE:
	{
		// 暂停
		TellSCM(SERVICE_PAUSE_PENDING, 0, 1);
		TellSCM(SERVICE_PAUSED, 0, 0);
		break;
	}
	case SERVICE_CONTROL_CONTINUE:
	{
		// 继续
		TellSCM(SERVICE_CONTINUE_PENDING, 0, 1);
		TellSCM(SERVICE_RUNNING, 0, 0);
		break;
	}
	case SERVICE_CONTROL_STOP:
	{
		// 停止
		TellSCM(SERVICE_STOP_PENDING, 0, 1);
		TellSCM(SERVICE_STOPPED, 0, 0);
		break;
	}
	case SERVICE_CONTROL_INTERROGATE:
	{
		// 询问
		break;
	}
	default:
		break;
	}
}

BOOL TellSCM(DWORD dwState, DWORD dwExitCode, DWORD dwProgress)
{
	SERVICE_STATUS serviceStatus = {};
	BOOL bRet = FALSE;

	::RtlZeroMemory(&serviceStatus, sizeof(serviceStatus));
	serviceStatus.dwServiceType = SERVICE_WIN32_OWN_PROCESS;
	serviceStatus.dwCurrentState = dwState;
	serviceStatus.dwControlsAccepted = SERVICE_ACCEPT_STOP | SERVICE_ACCEPT_PAUSE_CONTINUE | SERVICE_ACCEPT_SHUTDOWN;
	serviceStatus.dwWin32ExitCode = dwExitCode;
	serviceStatus.dwWaitHint = 3000;

	bRet = ::SetServiceStatus(g_ServiceStatusHandle, &serviceStatus);
	return bRet;
}

void DoTask()
{	
	ErrorHint("打开了我的服务进程！");
	OpenUserProcedure(g_szName);
	/*
	//PBT_APMSUSPEND
	static BOOL bStart = FALSE;
	if (!bStart)
	{		
		ErrorHint("打开了我的服务进程！");	
		OpenUserProcedure(g_szName);
		bStart = true;
	}	*/
}

// 打开用户层程序
BOOL OpenUserProcedure(LPWSTR szExePath)
{
	BOOL bRet = FALSE;
	HANDLE hToken = NULL;
	HANDLE hDuplicateToken = NULL;
	LPVOID lpEnviroment = NULL;

	do 
	{
		// 获得当前的程序会话ID
		DWORD dwSessionID = WTSGetActiveConsoleSessionId();

		// 获取当前Session的用户令牌
		if (!WTSQueryUserToken(dwSessionID, &hToken))
		{
			ErrorHint("WTSQueryUserToken错误！");
			break;
		}

		// 复制令牌
		if (!DuplicateTokenEx(hToken, MAXIMUM_ALLOWED, NULL,
			SecurityIdentification, TokenPrimary, &hDuplicateToken))
		{
			ErrorHint("DuplicateTokenEx错误！");
			break;
		}

		// 创建用户的Session环境
		if (!CreateEnvironmentBlock(&lpEnviroment, hDuplicateToken, FALSE))
		{
			ErrorHint("CreateEnvironmentBlock错误！");
			break;
		}

		STARTUPINFO si = { 0 };
		PROCESS_INFORMATION pi = { 0 };
		// 在复制的用户会话下执行应用程序，打开进程
		if (!CreateProcessAsUser(hDuplicateToken, szExePath, NULL, NULL, NULL, FALSE,
			NORMAL_PRIORITY_CLASS | CREATE_NEW_CONSOLE | CREATE_UNICODE_ENVIRONMENT,
			lpEnviroment, NULL, &si, &pi))
		{
			ErrorHint("CreateProcessAsUser失败！");
			break;
		}
		
		bRet = TRUE;

	} while (FALSE);

	if (hToken)
		CloseHandle(hToken);
	if (hDuplicateToken)
		CloseHandle(hDuplicateToken);
	if (lpEnviroment)
		DestroyEnvironmentBlock(lpEnviroment);

	return bRet;
}
