#include "stdafx.h"
#include "AutoRunCode.h"
#include <winsvc.h>


// ע���������
void RegeditRun(LPCWSTR exePath)
{
	// �û�Ȩ��
	HKEY hKey;
	// ��ע����
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
	KeyName += L"_Maker_��������";
	// �޸�ע���ֵ��ʵ�ֿ�������
	if (ERROR_SUCCESS != ::RegSetValueExW(hKey, KeyName.GetBuffer(),
		0, REG_SZ, (BYTE *)exePath, (1 + ::lstrlen(exePath) * 2)))
	{
		::RegCloseKey(hKey);
		return;
	}
	// �ر�ע����
	::RegCloseKey(hKey);
}

// ��������Ŀ¼����
void StartupRun(LPCWSTR exePath)
{
	TCHAR szStartupPath[MAX_PATH] = {};
	TCHAR szExePath[MAX_PATH] = {};
	// ��ȡ����Ŀ¼
	BOOL bRet = ::SHGetSpecialFolderPath(NULL, szStartupPath, CSIDL_STARTUP, TRUE);
	if (!bRet)	
		return;

	// ԭ�ļ���ַ
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
				//������ݷ�ʽ��Ŀ���ַ
				CString szDir = {};
				szDir = szStartupPath;
				szDir += L"\\_";
				for (int i = 7; i >= 5; i--)
				{
					szDir += exePath[exeLength - i];
				}
				szDir += L"_Maker_��������.lnk";
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
	::MessageBoxA(0, szError, "������ʾ��", 0);
}

// ϵͳ�������
void SystemServiceOperate(LPWSTR pDriverPath, int operateType)
{
	TCHAR szName[MAX_PATH] = { 0 };
	::lstrcpy(szName, pDriverPath);
	// ���˵��ļ�Ŀ¼����ȡ�ļ���
	::PathStripPath(szName);

	CString serviceName = L"ServiceJydr";

	SC_HANDLE shOSCM = NULL, shCS = NULL;
	// �򿪷�����ƹ��������ݿ�
	shOSCM = ::OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
	if (!shOSCM)
	{
		ErrorHint("OpenSCManagerʧ�ܣ�");
		return;
	}

	if (operateType)
	{
		// ��һ���Ѿ����ڵķ���
		shCS = OpenService(shOSCM, serviceName, SERVICE_ALL_ACCESS);
		if (!shCS)
		{
			//ErrorHint("OpenServiceʧ�ܣ�");
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
		// ��������
		// SERVICE_AUTO_START ��ϵͳ�Զ�����
		// SERVICE_DEMAND_START �ֶ�����
		// SERVICE_INTERACTIVE_PROCESS �÷�����������潻��
		
		shCS = ::CreateService(shOSCM, serviceName.GetBuffer(), szName,
			SERVICE_ALL_ACCESS,
			SERVICE_WIN32_OWN_PROCESS | SERVICE_INTERACTIVE_PROCESS,
			SERVICE_AUTO_START,
			SERVICE_ERROR_NORMAL,
			pDriverPath, NULL, NULL, NULL, NULL, NULL);
		if (!shCS)
			ErrorHint("CreateServiceʧ�ܣ�");
		break;
	}
	case START_SERVICE:
	{
		// ��������
		if (!::StartService(shCS, 0, NULL))
			ErrorHint("StartServiceʧ�ܣ�");		
		break;
	}
	case STOP_SERVICE:
	{
		// ֹͣ����
		if (!::ControlService(shCS, SERVICE_CONTROL_STOP, &ss))		
			ErrorHint("ControlServiceʧ�ܣ�");
		break;
	}
	case DELETE_SERVICE:
	{
		// ɾ������
		if (!::DeleteService(shCS))
			ErrorHint("ControlServiceʧ�ܣ�");
		break;
	}
	default: break;
	}
	// �رվ��
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

	// ��ʼ��COM
	HRESULT hr = ::CoInitialize(NULL);
	if (FAILED(hr))
		ErrorHint("��ʼ��COMʧ��!");

	// ����һ���������ʵ��
	hr = ::CoCreateInstance(CLSID_TaskScheduler, NULL,
		CLSCTX_INPROC_SERVER, IID_ITaskService,
		(LPVOID*)(&m_pITS));
	if (FAILED(hr))
		ErrorHint("����ʵ��ʧ��!");

	// ���ӵ��������
	hr = m_pITS->Connect(_variant_t(), _variant_t(), _variant_t(), _variant_t());
	if (FAILED(hr))
		ErrorHint("���ӷ���ʧ��!");

	// ��ȡITF��ָ�룬���ָ��ָ�������ע�������
	hr = m_pITS->GetFolder(_bstr_t("\\"), &m_pITF);
	if (FAILED(hr))
		ErrorHint("��ȡ�ļ���ʧ��!");

}


MyTaskSchedule::~MyTaskSchedule()
{
	if (m_pITS)
		m_pITS->Release();

	if (m_pITF)
		m_pITF->Release();

	// ж��COM
	::CoUninitialize();
}

// ɾ��ָ������ƻ�
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

// ��������ƻ�
void MyTaskSchedule::CreateTask(LPWSTR pTaskName, LPWSTR pFilePath)
{
	if (!m_pITF)
		return;

	// ���������ͬ�ļƻ�������ɾ��
	Delete(pTaskName);
	// �����������������������
	ITaskDefinition *pTaskDefinition = NULL;
	HRESULT hr = m_pITS->NewTask(0, &pTaskDefinition);
	if (FAILED(hr))
	{
		ErrorHint("NewTaskʧ��!");
		return;
	}

	// ����ע����Ϣ 
	IRegistrationInfo *pRegInfo = NULL;
	CComVariant variantAuthor(NULL);
	variantAuthor = L"��������";
	hr = pTaskDefinition->get_RegistrationInfo(&pRegInfo);
	if (FAILED(hr))
	{
		ErrorHint("get_RegistrationInfoʧ��!");
		return;
	}

	// ����������Ϣ
	hr = pRegInfo->put_Author(variantAuthor.bstrVal);
	pRegInfo->Release();

	// ���õ�¼���ͺ�����Ȩ�� 
	IPrincipal *pPrincipal = NULL;
	hr = pTaskDefinition->get_Principal(&pPrincipal);
	if (FAILED(hr))
	{
		ErrorHint("get_Principalʧ��!");
		return;
	}

	// ���õ�¼����
	hr = pPrincipal->put_LogonType(TASK_LOGON_INTERACTIVE_TOKEN);

	// ��������Ȩ��
	// ���Ȩ��
	hr = pPrincipal->put_RunLevel(TASK_RUNLEVEL_HIGHEST);
	pPrincipal->Release();

	// ����������Ϣ 
	ITaskSettings *pSettting = NULL;
	hr = pTaskDefinition->get_Settings(&pSettting);
	if (FAILED(hr))
	{
		ErrorHint("get_Settingsʧ��!");
		return;
	}

	// ����������Ϣ
	hr = pSettting->put_StopIfGoingOnBatteries(VARIANT_FALSE);
	hr = pSettting->put_DisallowStartIfOnBatteries(VARIANT_FALSE);
	hr = pSettting->put_AllowDemandStart(VARIANT_TRUE);
	hr = pSettting->put_StartWhenAvailable(VARIANT_FALSE);
	hr = pSettting->put_MultipleInstances(TASK_INSTANCES_PARALLEL);
	pSettting->Release();

	/* ����ִ�ж��� */
	IActionCollection *pActionCollect = NULL;
	hr = pTaskDefinition->get_Actions(&pActionCollect);
	if (FAILED(hr))
	{
		ErrorHint("get_Actionsʧ��!");
		return;
	}

	IAction *pAction = NULL;
	// ����ִ�в���
	hr = pActionCollect->Create(TASK_ACTION_EXEC, &pAction);
	pActionCollect->Release();

	// ����ִ�г���·���Ͳ��� 
	CComVariant variantProgramPath(NULL);
	CComVariant variantParameters(NULL);
	IExecAction *pExecAction = NULL;
	hr = pAction->QueryInterface(IID_IExecAction, (PVOID *)(&pExecAction));
	if (FAILED(hr))
	{
		ErrorHint("QueryInterfaceʧ��!");
		return;
	}

	pAction->Release();
	// ���ó���·���Ͳ���
	variantProgramPath = pFilePath;
	variantParameters = L"";
	pExecAction->put_Path(variantProgramPath.bstrVal);
	pExecAction->put_Arguments(variantParameters.bstrVal);
	pExecAction->Release();

	// ������������ʵ���û���½������ 
	ITriggerCollection* pTriggers = NULL;
	hr = pTaskDefinition->get_Triggers(&pTriggers);
	if (FAILED(hr))
	{
		ErrorHint("get_Triggersʧ��!");
		return;
	}

	// ����������
	ITrigger* pTrigger = NULL;
	hr = pTriggers->Create(TASK_TRIGGER_LOGON, &pTrigger);
	if (FAILED(hr))
	{
		ErrorHint("Createʧ��!");
		return;
	}

	// ע������ƻ�
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
		ErrorHint("RegisterTaskDefinitionʧ��!");
		return;
	}
	pTaskDefinition->Release();
	pRegisteredTask->Release();
}
