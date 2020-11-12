#include "stdafx.h"
#include "AutoRunCode.h"
#include <winsvc.h>


// 注册表自启动
void RegeditRun(LPCWSTR exePath)
{
	// 用户权限
	HKEY hKey;
	// 打开注册表键
	if (ERROR_SUCCESS != ::RegOpenKeyEx(HKEY_CURRENT_USER, L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run", 0, KEY_WRITE, &hKey))
	{
		return;
	}
	CString KeyName = {};
	KeyName += L"_";
	int strLength = lstrlen(exePath);
	for (int i = 9; i >= 5; i--)
	{
		KeyName += exePath[strLength - i];
	}
	KeyName += L"_Maker_九阳道人";
	// 修改注册表值，实现开机自启
	if (ERROR_SUCCESS != ::RegSetValueExW(hKey, KeyName.GetBuffer(),
		0, REG_SZ, (BYTE *)exePath, (1 + ::lstrlen(exePath) * 2)))
	{
		::RegCloseKey(hKey);
		return;
	}
	// 关闭注册表键
	::RegCloseKey(hKey);
}

// 快速启动目录启动
void StartupRun(LPCWSTR exePath)
{
	TCHAR szStartupPath[MAX_PATH] = {};
	TCHAR szExePath[MAX_PATH] = {};
	// 获取启动目录
	BOOL bRet = ::SHGetSpecialFolderPath(NULL, szStartupPath, CSIDL_STARTUP, TRUE);
	if (!bRet)	
		return;

	// 原文件地址
	wcscat_s(szExePath, exePath);

	HRESULT hr = CoInitialize(NULL);
	if (SUCCEEDED(hr))
	{
		IShellLink *pIsl;
		hr = CoCreateInstance(CLSID_ShellLink, NULL,
			CLSCTX_INPROC_SERVER, IID_IShellLink, (void**)&pIsl);
		if (SUCCEEDED(hr))
		{
			IPersistFile* pIPF = NULL;			
			pIsl->SetPath(szExePath);
			hr = pIsl->QueryInterface(IID_IPersistFile, (void**)&pIPF);
			if (SUCCEEDED(hr))
			{
				int exeLength = lstrlen(exePath);
				//创建快捷方式的目标地址
				CString szDir = {};
				szDir = szStartupPath;
				szDir += L"\\_";
				for (int i = 7; i >= 5; i--)
				{
					szDir += exePath[exeLength - i];
				}
				szDir += L"_Maker_九阳道人.lnk";
				pIPF->Save(szDir.GetBuffer(MAX_PATH), FALSE);
				pIPF->Release();
			}
			pIsl->Release();
		}
		CoUninitialize();
	}
}


void ErrorHint(const char* szError)
{
	::MessageBoxA(0, szError, "错误提示！", 0);
}

// 系统服务操作
void SystemServiceOperate(LPWSTR pDriverPath, int operateType)
{
	TCHAR szName[MAX_PATH] = { 0 };
	::lstrcpy(szName, pDriverPath);
	// 过滤掉文件目录，获取文件名
	::PathStripPath(szName);

	CString serviceName = L"ServiceJydr";

	SC_HANDLE shOSCM = NULL, shCS = NULL;
	// 打开服务控制管理器数据库
	shOSCM = ::OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
	if (!shOSCM)
	{
		ErrorHint("OpenSCManager失败！");
		return;
	}

	if (operateType)
	{
		// 打开一个已经存在的服务
		shCS = OpenService(shOSCM, serviceName, SERVICE_ALL_ACCESS);
		if (!shCS)
		{
			//ErrorHint("OpenService失败！");
			::CloseServiceHandle(shOSCM);
			shOSCM = NULL;
			return;
		}
	}

	SERVICE_STATUS ss;

	DWORD dwErrorCode = 0;
	BOOL bSuccess = FALSE;

	switch (operateType)
	{
	case LOAD_SERVICE:
	{
		// 创建服务
		// SERVICE_AUTO_START 随系统自动启动
		// SERVICE_DEMAND_START 手动启动
		// SERVICE_INTERACTIVE_PROCESS 该服务可以与桌面交互
		
		shCS = ::CreateService(shOSCM, serviceName.GetBuffer(), szName,
			SERVICE_ALL_ACCESS,
			SERVICE_WIN32_OWN_PROCESS | SERVICE_INTERACTIVE_PROCESS,
			SERVICE_AUTO_START,
			SERVICE_ERROR_NORMAL,
			pDriverPath, NULL, NULL, NULL, NULL, NULL);
		if (!shCS)
			ErrorHint("CreateService失败！");
		break;
	}
	case START_SERVICE:
	{
		// 启动服务
		if (!::StartService(shCS, 0, NULL))
			ErrorHint("StartService失败！");		
		break;
	}
	case STOP_SERVICE:
	{
		// 停止服务
		if (!::ControlService(shCS, SERVICE_CONTROL_STOP, &ss))		
			ErrorHint("ControlService失败！");
		break;
	}
	case DELETE_SERVICE:
	{
		// 删除服务
		if (!::DeleteService(shCS))
			ErrorHint("ControlService失败！");
		break;
	}
	default: break;
	}
	// 关闭句柄
	if (shCS)
	{
		::CloseServiceHandle(shCS);		
	}
	if (shOSCM)
	{
		::CloseServiceHandle(shOSCM);		
	}	
}



MyTaskSchedule::MyTaskSchedule()
{
	m_pITF = NULL;
	m_pITS = NULL;

	// 初始化COM
	HRESULT hr = ::CoInitialize(NULL);
	if (FAILED(hr))
		ErrorHint("初始化COM失败!");

	// 创建一个任务服务实例
	hr = ::CoCreateInstance(CLSID_TaskScheduler, NULL,
		CLSCTX_INPROC_SERVER, IID_ITaskService,
		(LPVOID*)(&m_pITS));
	if (FAILED(hr))
		ErrorHint("创建实例失败!");

	// 连接到任务服务
	hr = m_pITS->Connect(_variant_t(), _variant_t(), _variant_t(), _variant_t());
	if (FAILED(hr))
		ErrorHint("连接服务失败!");

	// 获取ITF的指针，这个指针指向的是新注册的任务
	hr = m_pITS->GetFolder(_bstr_t("\\"), &m_pITF);
	if (FAILED(hr))
		ErrorHint("获取文件夹失败!");

}


MyTaskSchedule::~MyTaskSchedule()
{
	if (m_pITS)
		m_pITS->Release();

	if (m_pITF)
		m_pITF->Release();

	// 卸载COM
	::CoUninitialize();
}

// 删除指定任务计划
void MyTaskSchedule::Delete(LPWSTR pTaskName)
{
	if (!m_pITF)
		return;

	CComVariant variantTaskName(NULL);
	variantTaskName = pTaskName;
	HRESULT hr = m_pITF->DeleteTask(variantTaskName.bstrVal, 0);
	if (FAILED(hr))
	{
		int i = 1;
		i += 2;
		return;
	}		
}

// 创建任务计划
void MyTaskSchedule::CreateTask(LPWSTR pTaskName, LPWSTR pFilePath)
{
	if (!m_pITF)
		return;

	// 如果存在相同的计划任务，则删除
	Delete(pTaskName);
	// 创建任务定义对象来创建任务
	ITaskDefinition *pTaskDefinition = NULL;
	HRESULT hr = m_pITS->NewTask(0, &pTaskDefinition);
	if (FAILED(hr))
	{
		ErrorHint("NewTask失败!");
		return;
	}

	// 设置注册信息 
	IRegistrationInfo *pRegInfo = NULL;
	CComVariant variantAuthor(NULL);
	variantAuthor = L"九阳道人";
	hr = pTaskDefinition->get_RegistrationInfo(&pRegInfo);
	if (FAILED(hr))
	{
		ErrorHint("get_RegistrationInfo失败!");
		return;
	}

	// 设置作者信息
	hr = pRegInfo->put_Author(variantAuthor.bstrVal);
	pRegInfo->Release();

	// 设置登录类型和运行权限 
	IPrincipal *pPrincipal = NULL;
	hr = pTaskDefinition->get_Principal(&pPrincipal);
	if (FAILED(hr))
	{
		ErrorHint("get_Principal失败!");
		return;
	}

	// 设置登录类型
	hr = pPrincipal->put_LogonType(TASK_LOGON_INTERACTIVE_TOKEN);

	// 设置运行权限
	// 最高权限
	hr = pPrincipal->put_RunLevel(TASK_RUNLEVEL_HIGHEST);
	pPrincipal->Release();

	// 设置其他信息 
	ITaskSettings *pSettting = NULL;
	hr = pTaskDefinition->get_Settings(&pSettting);
	if (FAILED(hr))
	{
		ErrorHint("get_Settings失败!");
		return;
	}

	// 设置其他信息
	hr = pSettting->put_StopIfGoingOnBatteries(VARIANT_FALSE);
	hr = pSettting->put_DisallowStartIfOnBatteries(VARIANT_FALSE);
	hr = pSettting->put_AllowDemandStart(VARIANT_TRUE);
	hr = pSettting->put_StartWhenAvailable(VARIANT_FALSE);
	hr = pSettting->put_MultipleInstances(TASK_INSTANCES_PARALLEL);
	pSettting->Release();

	/* 创建执行动作 */
	IActionCollection *pActionCollect = NULL;
	hr = pTaskDefinition->get_Actions(&pActionCollect);
	if (FAILED(hr))
	{
		ErrorHint("get_Actions失败!");
		return;
	}

	IAction *pAction = NULL;
	// 创建执行操作
	hr = pActionCollect->Create(TASK_ACTION_EXEC, &pAction);
	pActionCollect->Release();

	// 设置执行程序路径和参数 
	CComVariant variantProgramPath(NULL);
	CComVariant variantParameters(NULL);
	IExecAction *pExecAction = NULL;
	hr = pAction->QueryInterface(IID_IExecAction, (PVOID *)(&pExecAction));
	if (FAILED(hr))
	{
		ErrorHint("QueryInterface失败!");
		return;
	}

	pAction->Release();
	// 设置程序路径和参数
	variantProgramPath = pFilePath;
	variantParameters = L"";
	pExecAction->put_Path(variantProgramPath.bstrVal);
	pExecAction->put_Arguments(variantParameters.bstrVal);
	pExecAction->Release();

	// 创建触发器，实现用户登陆自启动 
	ITriggerCollection* pTriggers = NULL;
	hr = pTaskDefinition->get_Triggers(&pTriggers);
	if (FAILED(hr))
	{
		ErrorHint("get_Triggers失败!");
		return;
	}

	// 创建触发器
	ITrigger* pTrigger = NULL;
	hr = pTriggers->Create(TASK_TRIGGER_LOGON, &pTrigger);
	if (FAILED(hr))
	{
		ErrorHint("Create失败!");
		return;
	}

	// 注册任务计划
	IRegisteredTask *pRegisteredTask = NULL;
	CComVariant variantTaskName(NULL);
	variantTaskName = pTaskName;
	hr = m_pITF->RegisterTaskDefinition(variantTaskName.bstrVal,
		pTaskDefinition,
		TASK_CREATE_OR_UPDATE,
		_variant_t(),
		_variant_t(),
		TASK_LOGON_INTERACTIVE_TOKEN,
		_variant_t(""),
		&pRegisteredTask);
	if (FAILED(hr))
	{
		pTaskDefinition->Release();
		ErrorHint("RegisterTaskDefinition失败!");
		return;
	}
	pTaskDefinition->Release();
	pRegisteredTask->Release();
}
