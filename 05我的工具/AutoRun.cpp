// AutoRun.cpp: 实现文件
//

#include "stdafx.h"
#include "AutoRun.h"
#include "afxdialogex.h"
#include "05我的工具.h"
#include "AutoRunCode.h"
#include <locale.h>

// AutoRun 对话框

IMPLEMENT_DYNAMIC(AutoRun, CDialogEx)

AutoRun::AutoRun(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_AUTORUN, pParent)
	, m_ExePath(_T("提示：请将要设置自启动的EXE程序拖到这里或者打开！"))
	, m_Prompt(_T("提示：计划任务与自启服务需要以管理员权限运行！"))
{	
	TCHAR PathName[MAX_PATH] = {};
	TCHAR Path[MAX_PATH] = {};
	// 必须用此函数获取路径，否则开机无法自动读取。
	GetModuleFileName(NULL, PathName, MAX_PATH);
	for (int i = 0; i < (int)(lstrlen(PathName) + 1)*2; i++)
	{
		if (!wcscmp(PathName + i, L"九阳道人.exe"))
			break;
		Path[i] = PathName[i];
	}
	wcscat_s(Path, MAX_PATH, L"ServiceAuto.exe");
	serPath = Path;
}

AutoRun::~AutoRun()
{
}

void AutoRun::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, m_ExePath);
	DDX_Text(pDX, IDC_PROMPT, m_Prompt);
}


BEGIN_MESSAGE_MAP(AutoRun, CDialogEx)
	ON_WM_DROPFILES()
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_BUTTON19, &AutoRun::OnOpenFileButton)
	ON_BN_CLICKED(IDC_BUTTON1, &AutoRun::OnRegAutoRun)
	ON_BN_CLICKED(IDC_BUTTON11, &AutoRun::OnStartupRun)
	ON_BN_CLICKED(IDC_BUTTON17, &AutoRun::OnTaskSchedule)
	ON_BN_CLICKED(IDC_BUTTON20, &AutoRun::OnOpenStartup)
	ON_BN_CLICKED(IDC_BUTTON21, &AutoRun::OnOpenRegedit)
	ON_BN_CLICKED(IDC_BUTTON22, &AutoRun::OnOpenMmc)
	ON_BN_CLICKED(IDC_BUTTON18, &AutoRun::OnSystemService)
	ON_BN_CLICKED(IDC_BUTTON2, &AutoRun::OnBnClickedButton2)
END_MESSAGE_MAP()


// AutoRun 消息处理程序

// 响应拖拽
void AutoRun::OnDropFiles(HDROP hDropInfo)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	TCHAR szStr[MAX_PATH] = {};
	::DragQueryFile(hDropInfo, 0, szStr, MAX_PATH);
	m_ExePath = szStr;
	UpdateData(FALSE);

	CDialogEx::OnDropFiles(hDropInfo);
}

void AutoRun::OnClose()
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	m_pFeather->m_IsOpenAutoRun = FALSE;
	CDialogEx::OnClose();
}

// 打开按钮
void AutoRun::OnOpenFileButton()
{
	
	// TODO: 在此添加控件通知处理程序代码
	CFileDialog dlg(TRUE, //TRUE为OPEN对话框，FALSE为SAVE AS对话框
		NULL,
		NULL,
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		(LPCTSTR)_TEXT("EXE Files (*.exe)|*.exe||"),
		NULL);
	if (dlg.DoModal() == IDOK)
	{
		// 打开成功，保存文件路径
		m_ExePath = dlg.GetPathName();
	}
	else
	{
		m_ExePath = _T("打开文件失败！");
	}
	UpdateData(FALSE);
}

BOOL AutoRun::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 在此添加专用代码和/或调用基类
	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_ESCAPE)
		return TRUE;
	return CDialogEx::PreTranslateMessage(pMsg);
}

// 是否是EXE文件
BOOL AutoRun::IsExeFile()
{
	int strLength = lstrlen(m_ExePath);
	if (m_ExePath[strLength - 1] == 'e')
	{
		if (m_ExePath[strLength - 2] == 'x')
			if (m_ExePath[strLength - 3] == 'e')
				if (m_ExePath[strLength - 4] == '.')
				{
					return TRUE;
				}
	}
	return FALSE;
}

// 添加到注册表按钮
void AutoRun::OnRegAutoRun()
{
	// TODO: 在此添加控件通知处理程序代码
	if (IsExeFile())
	{
		RegeditRun(m_ExePath.GetBuffer());
		m_Prompt = L"提示：该程序已经加入到注册表自启动项";
		UpdateData(FALSE);
		return;
	}
	AfxMessageBox(L"没有exe文件，不能设置自启动！");	
}

// 快速启动按钮
void AutoRun::OnStartupRun()
{
	if (IsExeFile())
	{
		StartupRun(m_ExePath.GetBuffer());
		m_Prompt = L"提示：该程序已经加入到快速启动目录";
		UpdateData(FALSE);
		return;
	}
	AfxMessageBox(L"没有exe文件，不能设置自启动！");
}

// 计划任务按钮
void AutoRun::OnTaskSchedule()
{
	if (IsExeFile())
	{
		CString KeyName = {};
		KeyName += L"九阳道人Maker_";
		int strLength = lstrlen(m_ExePath.GetBuffer());
		for (int i = 9; i >= 5; i--)
		{
			KeyName += m_ExePath.GetBuffer()[strLength - i];
		}	

		MyTaskSchedule obj;
		obj.CreateTask(KeyName.GetBuffer(), m_ExePath.GetBuffer());
		m_Prompt = L"提示：该程序已经加入到计划任务";
		UpdateData(FALSE);
		return;
	}
	AfxMessageBox(L"没有exe文件，不能设置自启动！");
}

// 打开启动目录按钮
void AutoRun::OnOpenStartup()
{		   
	TCHAR szStartupPath[MAX_PATH] = {};	
	// 获取启动目录
	BOOL bRet = ::SHGetSpecialFolderPath(NULL, szStartupPath, CSIDL_STARTUP, TRUE);
	if (!bRet) return;	
	::ShellExecute(NULL, NULL, szStartupPath, NULL, NULL, SW_SHOWNORMAL);
}

// 获取一个指针
LPITEMIDLIST AutoRun::GetItemIDList(CString strPathName)
{
	if (TRUE == strPathName.IsEmpty())
		return NULL;

	LPSHELLFOLDER pDesktopFolder = NULL;
	HRESULT hr = SHGetDesktopFolder(&pDesktopFolder);
	if (FAILED(hr))
		return NULL;

	// 得到文件路径对应的ItemIDList
	LPITEMIDLIST pItemIDList = {};
	CString pPathName = strPathName;
	hr = pDesktopFolder->ParseDisplayName(NULL, NULL, pPathName.GetBuffer(), NULL, &pItemIDList, NULL);
	pDesktopFolder->Release();
	if (hr != S_OK)
		return NULL;

	return pItemIDList;

}

// 打开注册表
void AutoRun::OnOpenRegedit()
{
	CString szRegPath;
	szRegPath.Format(L"Software\\Microsoft\\Windows\\CurrentVersion\\Applets\\Regedit");
	
	CString szPath = {};
	szPath = L"HKEY_CURRENT_USER\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run";

	// 用户权限
	HKEY hKey;
	// 打开注册表键
	if (ERROR_SUCCESS != ::RegOpenKeyEx(HKEY_CURRENT_USER, szRegPath.GetBuffer(), 0, KEY_ALL_ACCESS, &hKey))
	{
		return;
	}

	//获取注册表子键信息，判断是否有子键
	TCHAR szName[MAX_PATH] = {};	//子键名称r
	DWORD dwSize = 512;	//名称大小
	DWORD dwIndex = 0;
	while (ERROR_NO_MORE_ITEMS != ::RegEnumValue(hKey, dwIndex++, szName, &dwSize, NULL,
		NULL, NULL, 0))
	{
		if (!wcscmp(szName, L"LastKey"))
		{
			// 修改注册表值，实现开机自启
			if (ERROR_SUCCESS != ::RegSetValueExW(hKey, L"LastKey",
				0, REG_SZ, (BYTE *)szPath.GetBuffer(), (1 + ::lstrlen(szPath) * 2)))
			{
				::RegCloseKey(hKey);
				return;
			}
			break;
		}
		//必须重置该值，否则只能遍历两三次；
		dwSize = 512;
	}
	// 关闭注册表键
	::RegCloseKey(hKey);

	CString szRegegitPath = {};
	szRegegitPath = L"C:\\Windows\\regedit.exe";

 	::ShellExecute(NULL, NULL, szRegegitPath, NULL, NULL, SW_SHOWNORMAL);
}

// 打开mmc
void AutoRun::OnOpenMmc()
{
	CString szPath = {};
	szPath = L"C:\\Windows\\System32\\compmgmt.msc"; 
	::ShellExecute(NULL, NULL, szPath, NULL, NULL, SW_SHOWNORMAL);
	
	Sleep(300);
//	HWND hWnd = ::FindWindow(L"MMCMainFrame", NULL);
// 	hWnd = ::FindWindowEx(hWnd, NULL, L"MDIClient", NULL);
// 	hWnd = ::FindWindowEx(hWnd, NULL, L"MMCChildFrm", NULL);
// 	hWnd = ::FindWindowEx(hWnd, NULL, L"MMCViewWindow", NULL);
// 	hWnd = ::FindWindowEx(hWnd, NULL, L"SysTreeView32", NULL);


	m_Prompt = L"计划任务：展开任务计划程序，再点击任务计划程序库\r\n查看服务：展开服务与应用程序,在点击服务";
	UpdateData(FALSE);
}

// 系统服务
void AutoRun::OnSystemService()
{
	if (IsExeFile())
	{
		char PathName[MAX_PATH] = {};
		char Path[MAX_PATH] = {};
		// 必须用此函数获取路径，否则开机无法自动读取。
		GetModuleFileNameA(NULL, PathName, MAX_PATH);
		for (int i = 0; i < (int)(strlen(PathName) + 1); i++)
		{
			if (!strcmp(PathName + i, "九阳道人.exe"))
				break;
			Path[i] = PathName[i];
		}
		strcat_s(Path, "AutoConfig.jydr");

		// 获取exe路径
		char exePath[MAX_PATH] = {};
		// 保存原来的语言系列
		char* old_locale = _strdup(setlocale(LC_CTYPE, NULL));
		// 设置新的语言系列
		setlocale(LC_CTYPE, setlocale(LC_ALL, ""));
		size_t siSize = 0;
		::wcstombs_s(&siSize, exePath, MAX_PATH, m_ExePath.GetBuffer(), _TRUNCATE);
		// 还原语言系列  
		setlocale(LC_CTYPE, old_locale);
		free(old_locale);
		// 保存exe路径
		FILE* pFile = NULL;
		fopen_s(&pFile, Path, "wb+");
		fwrite(&exePath, sizeof(exePath), 1, pFile);
		fclose(pFile);
		
		// 停止服务
		SystemServiceOperate(serPath.GetBuffer(), 2);
		// 删除服务
		SystemServiceOperate(serPath.GetBuffer(), 3);

		// 加载服务
		SystemServiceOperate(serPath.GetBuffer(), 0);
		// 启动服务
		SystemServiceOperate(serPath.GetBuffer(), 1);

		m_Prompt = L"提示：启动自启动服务成功！";
		UpdateData(FALSE);
		return;
	}
	AfxMessageBox(L"没有exe文件，不能设置自启动！");
	

}

// 删除服务
void AutoRun::OnBnClickedButton2()
{	
	// 停止服务
	SystemServiceOperate(serPath.GetBuffer(), 2);
	// 删除服务
	SystemServiceOperate(serPath.GetBuffer(), 3);
	m_Prompt = L"提示：删除自启动服务成功！";
	UpdateData(FALSE);
}


LRESULT AutoRun::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	// TODO: 在此添加专用代码和/或调用基类
	// 关机时要处理的代码
	if (message == WM_ENDSESSION)
	{

	}

	return CDialogEx::WindowProc(message, wParam, lParam);
}

// 保存自启动配置
void AutoRun::SaveAutoRunConfig()
{

}
