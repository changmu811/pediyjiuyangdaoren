// InjectDll.cpp: 实现文件
//

#include "stdafx.h"
#include "InjectDll.h"
#include "afxdialogex.h"
#include "05我的工具.h"
#include "COptiClear.h"
#include "InjectCode.h"
#include "AdjustPrivileges.h"
#include "HidePretend.h"
#include <TlHelp32.h>
#include <locale.h>

// InjectDll 对话框

IMPLEMENT_DYNAMIC(InjectDll, CDialogEx)

InjectDll::InjectDll(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIGINJECT, pParent)
	, m_DirDll(_T("提示：请拖拽DLL文件或者打开DLL文件放入本对话框中！"))
{
	m_Icon = new CImageList();
}

InjectDll::~InjectDll()
{		
	delete[] m_Icon;
}

void InjectDll::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_ProcessList);
	DDX_Text(pDX, IDC_EDIT1, m_DirDll);
	DDX_Control(pDX, IDC_LIST2, m_ModuleList);
}


BEGIN_MESSAGE_MAP(InjectDll, CDialogEx)
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_BUTTON11, &InjectDll::OnRefreshProcess)
	ON_NOTIFY(NM_RCLICK, IDC_LIST1, &InjectDll::OnNMRClickList1)
	ON_COMMAND(ID__REMOTE, &InjectDll::OnRemoteInject)
	ON_WM_DROPFILES()
	ON_COMMAND(ID__STOPREMOTE, &InjectDll::OnSuspendedInject)
	ON_COMMAND(ID__SESSIONREMOTE, &InjectDll::OnAcrossSession0)
	ON_COMMAND(ID__APCINJECT, &InjectDll::OnAPCInject)
	ON_NOTIFY(NM_CLICK, IDC_LIST1, &InjectDll::OnNMClickList1)
	ON_BN_CLICKED(IDC_BUTTON1, &InjectDll::OnFindFileButton)
	ON_COMMAND(ID__32780, &InjectDll::OnUnloadDll)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_LIST2, &InjectDll::OnNMCustomdrawList)
	ON_COMMAND(ID__GLOBALINJECT, &InjectDll::OnGlobalInject)
	ON_COMMAND(ID__APC32773, &InjectDll::OnGlobalUnload)
	ON_COMMAND(ID__32784, &InjectDll::PretendProcess)
	ON_COMMAND(ID__32785, &InjectDll::OnPuppetCommand)
	ON_COMMAND(ID__32786, &InjectDll::CodeInject)
	ON_COMMAND(ID__32787, &InjectDll::OnHideProcess)
	ON_COMMAND(ID__32788, &InjectDll::OnCancelHide)
	ON_COMMAND(ID__32789, &InjectDll::OnProtectProcess)
END_MESSAGE_MAP()


// InjectDll 消息处理程序


BOOL InjectDll::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 在此添加专用代码和/或调用基类
	if (pMsg->message == WM_KEYDOWN && (pMsg->wParam == VK_ESCAPE))
	{
		return TRUE;
	}
	return CDialogEx::PreTranslateMessage(pMsg);
}

// 关闭窗口时执行的功能
void InjectDll::OnClose()
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	m_pOpti->m_IsOpenInject = FALSE;	
	CDialogEx::OnClose();
}

// 初始化窗口
BOOL InjectDll::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	// TODO:  在此添加额外的初始化
		//设置窗口名称	
#ifdef _WIN64
	SetWindowText(L"注入与隐藏功能_64位");
#else
	//设置窗口名称
	SetWindowText(L"注入与隐藏功能_32位");
#endif
	
	m_Icon->Create(22, 22, ILC_COLOR32, 0, 4);

	m_ProcessList.InsertColumn(0, L"程序名称", 0, 170);
	m_ProcessList.InsertColumn(1, L"PID", 0, 70);
	m_ProcessList.InsertColumn(2, L"SESSION", 0, 65);
	m_ProcessList.InsertColumn(3, L"线程数", 0, 60);
	m_ProcessList.InsertColumn(4, L"路径", 0, 315);

	m_ModuleList.InsertColumn(0, L"模块名称", 0, 200);
	m_ModuleList.InsertColumn(1, L"大小", 0, 80);
	m_ModuleList.InsertColumn(2, L"路径", 0, 400);

	// 让List可以被选中 和 显示线条
	m_ProcessList.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
	m_ModuleList.SetExtendedStyle(LVS_EX_FULLROWSELECT);
	
	// 遍历进程
	FindAllProcess();

	// 提权
	EnbalePrivileges(::GetCurrentProcess(), "SeDebugPrivilege");

	//::SetWindowPos(GetSafeHwnd(), HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
	
	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}

// 遍历进程
void InjectDll::FindAllProcess() 
{
	//创建一个进程快照
	HANDLE hSnapProcess = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	PROCESSENTRY32 stcPe32 = { sizeof(PROCESSENTRY32) };

	TCHAR Buffer[3][MAX_PATH] = {};	
	int nNum = 0;//列表行数标记
	
	//用API进行遍历快照
	if (TRUE == Process32First(hSnapProcess, &stcPe32))
	{
		do
		{					   
			// 获取进程会话ID
			DWORD sessionId = 0;
			ProcessIdToSessionId(stcPe32.th32ProcessID, &sessionId);
			
			// 转字符串
			_stprintf_s(Buffer[0], MAX_PATH, _T("%d"), stcPe32.th32ProcessID);
			_stprintf_s(Buffer[1], MAX_PATH, _T("%d"), stcPe32.cntThreads);
			_stprintf_s(Buffer[2], MAX_PATH, _T("%d"), sessionId);

			// 获取进程路径
			TCHAR szBuf[MAX_PATH] = {};
			DWORD dwSize = MAX_PATH;
			HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, stcPe32.th32ProcessID);
			if (hProcess)
				QueryFullProcessImageName(hProcess, NULL, szBuf, &dwSize);
						
			CString strProcessName;
 			strProcessName = stcPe32.szExeFile;

			// 判断程序是64位还是32位
			if (!wcscmp(szBuf, L""))
			{
				strProcessName += L"*64";
			}
			else
			{
				if (Is64or32Process(hProcess))
					strProcessName += L"*64";
				else
					strProcessName += L"*32";
			}
			
			CloseHandle(hProcess);
								   		
			//获取图标信息
			SHFILEINFO sf;			
			SHGetFileInfo(szBuf, 0, &sf, sizeof(SHFILEINFO), SHGFI_ICON);
			//加载图标
			int index = m_Icon->Add(sf.hIcon);
			m_ProcessList.SetImageList(m_Icon, LVSIL_SMALL);
			// 插入首列元素 图标与进程名称
			m_ProcessList.InsertItem(nNum, strProcessName.GetBuffer(), index);			
			// 在首项后一列插入元素 PID
			m_ProcessList.SetItemText(nNum, 1, Buffer[0]);
			// 插入SESSION ID
			m_ProcessList.SetItemText(nNum, 2, Buffer[2]);
			// 插入线程数
			m_ProcessList.SetItemText(nNum, 3, Buffer[1]);			
			// 插入路径			
			m_ProcessList.SetItemText(nNum, 4, szBuf);
					   
			nNum++;
		} while (Process32Next(hSnapProcess, &stcPe32));
	}
}

// 判断是是否是x64进程
BOOL InjectDll::Is64or32Process(HANDLE hProcess)
{
 	// 判断ntdll中的导出函数,可知是否是64位OS
  	HMODULE hMod = GetModuleHandle(L"ntdll.dll");
//  	FARPROC x64fun = ::GetProcAddress(hMod, "ZwWow64ReadVirtualMemory64");
//  	if (!x64fun) return FALSE;

	// 利用IsWow64Process判断是否是x64进程
	typedef BOOL(WINAPI *pfnIsWow64Process)(HANDLE, PBOOL);
	pfnIsWow64Process fnIsWow64Process = NULL;

	hMod = GetModuleHandle(L"kernel32.dll");
	fnIsWow64Process = (pfnIsWow64Process)GetProcAddress(hMod, "IsWow64Process");
	if (!fnIsWow64Process) return FALSE;				//如果没有导出则判定为32位

	BOOL bX64;
	if (!fnIsWow64Process(hProcess, &bX64)) return FALSE;

	return !bX64;
}

// 刷新进程
void InjectDll::OnRefreshProcess()
{
	// TODO: 在此添加控件通知处理程序代码
	m_ProcessList.DeleteAllItems();
	m_ModuleList.DeleteAllItems();
	FindAllProcess();
}

// 拖拽文件响应
void InjectDll::OnDropFiles(HDROP hDropInfo)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	TCHAR szStr[MAX_PATH] = {};
	::DragQueryFile(hDropInfo, 0, szStr, MAX_PATH);
	m_DirDll = szStr;
	UpdateData(FALSE);

	CDialogEx::OnDropFiles(hDropInfo);
}

// 第一个列表右键单击出现菜单
void InjectDll::OnNMRClickList1(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	CMenu Menu, *PMenu;
	Menu.LoadMenu(IDR_MENU1);
	PMenu = Menu.GetSubMenu(0);
	CPoint Pt;
	GetCursorPos(&Pt);
	TrackPopupMenu(PMenu->m_hMenu,TPM_LEFTALIGN,
		Pt.x, Pt.y,
		0, m_hWnd, 0);
	
	*pResult = 0;
}

// 远程线程注入
void InjectDll::OnRemoteInject()
{
	// 根据列表得到进程PID字符串
	int nIndex = m_ProcessList.GetSelectionMark();
	CString Str = m_ProcessList.GetItemText(nIndex, 1);
	// 将字符串转为数值PID
	DWORD dwPid = _wtoi(Str);
	
	// 获取DLL路径
	char DllPath[MAX_PATH] = {};
	// 保存原来的语言系列
	char* old_locale = _strdup(setlocale(LC_CTYPE, NULL));
	// 设置新的语言系列
	setlocale(LC_CTYPE, setlocale(LC_ALL, ""));

	size_t siSize = 0;
	::wcstombs_s(&siSize, DllPath, MAX_PATH, m_DirDll.GetBuffer(), _TRUNCATE);

	// 还原语言系列  
	setlocale(LC_CTYPE, old_locale);
	free(old_locale);

	size_t strLength = strlen(DllPath);
	if (DllPath[strLength - 1] == 'l')
	{
		if (DllPath[strLength - 2] == 'l')
			if (DllPath[strLength - 3] == 'd')
				if (DllPath[strLength - 4] == '.')
				{
					// 远程线程注入
					RemoteInjectDll(dwPid, DllPath);
					ShowModuleList(dwPid);
					return;
				}
	}
	AfxMessageBox(L"没有DLL文件，不能注入！");

}

// 挂起程序再远程线程注入
void InjectDll::OnSuspendedInject()
{
	// 根据列表得到进程PID字符串
	int nIndex = m_ProcessList.GetSelectionMark();
	CString Str = m_ProcessList.GetItemText(nIndex, 1);
	// 将字符串转为数值PID
	DWORD dwPid = _wtoi(Str);

	// 获取DLL路径
	char DllPath[MAX_PATH] = {};

	// 保存原来的语言系列
	char* old_locale = _strdup(setlocale(LC_CTYPE, NULL));
	// 设置新的语言系列
	setlocale(LC_CTYPE, setlocale(LC_ALL, ""));

	size_t siSize = 0;
	::wcstombs_s(&siSize, DllPath, MAX_PATH, m_DirDll.GetBuffer(), _TRUNCATE);

	// 还原语言系列  
	setlocale(LC_CTYPE, old_locale);
	free(old_locale);

	size_t strLength = strlen(DllPath);
	if (DllPath[strLength - 1] == 'l')
	{
		if (DllPath[strLength - 2] == 'l')
			if (DllPath[strLength - 3] == 'd')
				if (DllPath[strLength - 4] == '.')
				{
					BOOL bIsInject = FALSE;
					// 创建线程快照
					HANDLE hSnapshot = NULL;
					THREADENTRY32 te32 = { sizeof(THREADENTRY32) };
					hSnapshot = ::CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
					if (!hSnapshot)
						AfxMessageBox(L"创建线程快照失败！");

					DWORD dwThreadNum = 0;
					// 遍历线程快照
					if (::Thread32First(hSnapshot, &te32))
					{
						do
						{
							if (te32.th32OwnerProcessID == dwPid)
							{
								vecTid.push_back(te32.th32ThreadID);
								HANDLE hThread1 = OpenThread(THREAD_ALL_ACCESS, FALSE, te32.th32ThreadID);								
								// 挂起线程		
								SuspendThread(hThread1);
							}
						} while (::Thread32Next(hSnapshot, &te32));
					}
															
					// 远程线程注入
					RemoteInjectDll(dwPid, DllPath);
					ShowModuleList(dwPid);
					// 遍历vecTID
					for (auto Tid : vecTid)
					{
						HANDLE hThread2 = OpenThread(THREAD_ALL_ACCESS, FALSE, Tid);
						// 恢复线程		
						ResumeThread(hThread2);
					}	
					vecTid.clear();
					return;
				}
	}
	AfxMessageBox(L"没有加载DLL文件，不能注入！");

}

// 突破SESSION 0 隔离的远程注入
void InjectDll::OnAcrossSession0()
{
	// TODO: 在此添加命令处理程序代码
	// 根据列表得到进程PID字符串
	int nIndex = m_ProcessList.GetSelectionMark();
	CString Str = m_ProcessList.GetItemText(nIndex, 1);
	// 将字符串转为数值PID
	DWORD dwPid = _wtoi(Str);

	// 获取DLL路径
	char DllPath[MAX_PATH] = {};

	// 保存原来的语言系列
	char* old_locale = _strdup(setlocale(LC_CTYPE, NULL));
	// 设置新的语言系列
	setlocale(LC_CTYPE, setlocale(LC_ALL, ""));

	size_t siSize = 0;
	::wcstombs_s(&siSize, DllPath, MAX_PATH, m_DirDll.GetBuffer(), _TRUNCATE);

	// 还原语言系列  
	setlocale(LC_CTYPE, old_locale);
	free(old_locale);

	size_t strLength = strlen(DllPath);
	if (DllPath[strLength - 1] == 'l')
	{
		if (DllPath[strLength - 2] == 'l')
			if (DllPath[strLength - 3] == 'd')
				if (DllPath[strLength - 4] == '.')
				{
					// 突破SESSION0隔离注入
					AcrossSession0(dwPid, DllPath);
					ShowModuleList(dwPid);
					return;
				}
	}
	AfxMessageBox(L"没有加载DLL文件，不能注入！");
}

// APC注入
void InjectDll::OnAPCInject()
{
	int nIndex = m_ProcessList.GetSelectionMark();
	CString Str = m_ProcessList.GetItemText(nIndex, 1);
	// 将字符串转为数值PID
	DWORD dwPid = _wtoi(Str);

	// 获取DLL路径
	char DllPath[MAX_PATH] = {};

	// 保存原来的语言系列
	char* old_locale = _strdup(setlocale(LC_CTYPE, NULL));
	// 设置新的语言系列
	setlocale(LC_CTYPE, setlocale(LC_ALL, ""));

	size_t siSize = 0;
	::wcstombs_s(&siSize, DllPath, MAX_PATH, m_DirDll.GetBuffer(), _TRUNCATE);

	// 还原语言系列  
	setlocale(LC_CTYPE, old_locale);
	free(old_locale);

	size_t strLength = strlen(DllPath);
	if (DllPath[strLength - 1] == 'l')
	{
		if (DllPath[strLength - 2] == 'l')
			if (DllPath[strLength - 3] == 'd')
				if (DllPath[strLength - 4] == '.')
				{
					// APC注入
					APCInject(dwPid, DllPath);
					ShowModuleList(dwPid);
					return;
				}
	}
	AfxMessageBox(L"没有加载DLL文件，不能注入！");
}

// 全局注入
void InjectDll::OnGlobalInject()
{
	int strLength = lstrlen(m_DirDll);
	if (m_DirDll[strLength - 1] == 'l')
	{
		if (m_DirDll[strLength - 2] == 'l')
			if (m_DirDll[strLength - 3] == 'd')
				if (m_DirDll[strLength - 4] == '.')
				{
					// 全局注入
					GlobalInject(m_DirDll, TRUE);					
					return;
				}
	}
	AfxMessageBox(L"没有加载DLL文件，不能注入！");	
}

// 代码注入
void InjectDll::CodeInject()
{
	// 根据列表得到进程PID字符串
	int nIndex = m_ProcessList.GetSelectionMark();
	CString Str = m_ProcessList.GetItemText(nIndex, 1);
	// 将字符串转为数值PID
	DWORD dwPid = _wtoi(Str);
	InjectCode(dwPid);
}

// 单击进程列表显示模块信息
void InjectDll::OnNMClickList1(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码


	int nIndex = m_ProcessList.GetSelectionMark();
	CString Str = m_ProcessList.GetItemText(nIndex, 1);
	// 将字符串转为数值PID
	DWORD dwPid = _wtoi(Str);

	ShowModuleList(dwPid);
	
	*pResult = 0;
}

// 打开文件按钮
void InjectDll::OnFindFileButton()
{
	// TODO: 在此添加控件通知处理程序代码
	//如果为单个文件查杀，弹出文件选择框，路径保存在m_szSingleFilePath
	CFileDialog dlg(TRUE, //TRUE为OPEN对话框，FALSE为SAVE AS对话框
		NULL,
		NULL,
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		(LPCTSTR)_TEXT("DLL Files (*.dll)|*.dll||"),
		NULL);
	if (dlg.DoModal() == IDOK)
	{
		// 打开成功，保存文件路径
		m_DirDll = dlg.GetPathName();
	}
	else
	{
		m_DirDll = _T("打开文件失败！");		
	}
	UpdateData(FALSE);
}

// 卸载DLL
void InjectDll::OnUnloadDll()
{
	int nIndex = m_ProcessList.GetSelectionMark();
	CString Str0 = m_ProcessList.GetItemText(nIndex, 2);
	// 将字符串转为数值PID
	int dwSessionId = _wtoi(Str0);

	if (dwSessionId)
	{
		nIndex = m_ProcessList.GetSelectionMark();
		CString Str = m_ProcessList.GetItemText(nIndex, 1);
		// 将字符串转为数值PID
		DWORD dwPid = _wtoi(Str);

		int strLength = lstrlen(m_DirDll);
		if (m_DirDll[strLength - 1] == 'l')
		{
			if (m_DirDll[strLength - 2] == 'l')
				if (m_DirDll[strLength - 3] == 'd')
					if (m_DirDll[strLength - 4] == '.')
					{
						// 卸载DLL
						UnloadDll(dwPid, m_DirDll);
						ShowModuleList(dwPid);
						return;
					}
		}
		AfxMessageBox(L"没有目标DLL文件，不能卸载！");
	}
	else
	{
		AfxMessageBox(L"会话0层的程序，不能卸载！");
	}
		
}

// 刷新并显示模块列表
void InjectDll::ShowModuleList(DWORD dwPid)
{
	m_ModuleList.DeleteAllItems();
	UpdateData(TRUE);
	// 创建快照
	HANDLE hSnapModule = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, dwPid);
	MODULEENTRY32 Module32 = { sizeof(MODULEENTRY32) };

	CString Str1 = {};
	CString Str2 = {};
	int nNum = 0;//列表行数标记
	// 遍历模块
	Module32First(hSnapModule, &Module32);
	do
	{
		if (Module32.modBaseSize / 1024 > 1024)
		{
			//转字符串		
			Str1.Format(L"%.2lf", Module32.modBaseSize / 1024.0 / 1024);
			Str2 = Str1 + L" MB";
		}
		else
		{
			//转字符串		
			Str1.Format(L"%d", Module32.modBaseSize / 1024);
			Str2 = Str1 + L" KB";
		}
		m_ModuleList.InsertItem(nNum, Module32.szModule);		
		m_ModuleList.SetItemText(nNum, 1, Str2);
		m_ModuleList.SetItemText(nNum, 2, Module32.szExePath);

		// 高亮显示注入的DLL
		if (!_tcsicmp((LPCTSTR)Module32.szModule, m_DirDll) ||
			!_tcsicmp((LPCTSTR)Module32.szExePath, m_DirDll))
		{
			m_ModuleList.SetItemData(nNum, 1);
		}
		nNum++;
	} while (Module32Next(hSnapModule, &Module32));
	CloseHandle(hSnapModule);
}

// LIST风格处理
void InjectDll::OnNMCustomdrawList(NMHDR *pNMHDR, LRESULT *pResult)
{	
	LPNMTVCUSTOMDRAW pNMCD = reinterpret_cast<LPNMTVCUSTOMDRAW>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码

	NMCUSTOMDRAW ns = pNMCD->nmcd;
	if (ns.dwDrawStage == CDDS_ITEMPREPAINT)
	{
		if (ns.lItemlParam == 1)
		{
			pNMCD->clrTextBk = RGB(255, 0, 0);		//背景红色
			pNMCD->clrText = RGB(255, 255, 255);	//文字白色
		}		
	}
	
	*pResult = 0;
	*pResult |= CDRF_NOTIFYPOSTPAINT;	
	*pResult |= CDRF_NOTIFYITEMDRAW;	

}

// 全局卸载DLL
void InjectDll::OnGlobalUnload()
{
	// TODO: 在此添加命令处理程序代码
	int strLength = lstrlen(m_DirDll);
	if (m_DirDll[strLength - 1] == 'l')
	{
		if (m_DirDll[strLength - 2] == 'l')
			if (m_DirDll[strLength - 3] == 'd')
				if (m_DirDll[strLength - 4] == '.')
				{
					// 全局注入
					GlobalInject(m_DirDll, FALSE);
					return;
				}
	}
	AfxMessageBox(L"没有加载DLL文件，不能卸载！");
}

/*************************************隐藏进程***************************************/
// 伪装进程
void InjectDll::PretendProcess()
{
	int nIndex = m_ProcessList.GetSelectionMark();
	CString Str = m_ProcessList.GetItemText(nIndex, 1);
	// 将字符串转为数值PID
	DWORD dwPid = _wtoi(Str);
	DisguiseProcess(dwPid, L"C:\\Windows\\explorer.exe", L"explorer.exe");
}

// 傀儡进程选项
void InjectDll::OnPuppetCommand()
{			
#ifdef _WIN64
	AfxMessageBox(L"不支持64位！");
#else
	Puppet();
#endif
		
}

// 隐藏进程
void InjectDll::OnHideProcess()
{
	int nIndex = m_ProcessList.GetSelectionMark();
	CString Str = m_ProcessList.GetItemText(nIndex, 1);
	int nPid = _ttoi(Str);

	/*使用信号量Hook时才用到*/
	CreateSemaphore(NULL, nPid, nPid + 1, L"九阳道人");

	// 安装消息钩子
	HideHook(TRUE, 0);

}

// 取消隐藏(卸载全局钩子)
void InjectDll::OnCancelHide()
{
	// 卸载消息钩子
	HideUnHook();
}

// 保护进程按钮
void InjectDll::OnProtectProcess()
{
	int nIndex = m_ProcessList.GetSelectionMark();
	CString Str = m_ProcessList.GetItemText(nIndex, 1);
	int nPid = _ttoi(Str);
	
	// 安装消息钩子
	HideHook(FALSE, nPid);
}
