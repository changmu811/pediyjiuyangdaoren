// COptiClear.cpp: 实现文件
//

#include "stdafx.h"
#include "COptiClear.h"
#include "afxdialogex.h"
#include "05我的工具.h"
#include "InjectDll.h"
#include "AutoRun.h"
#include "Privileges.h"
#include "VSClear.h"
#include "OtherFunction.h"
#include "HashAndCrypt.h"

#include <corecrt_wstdlib.h>
#include <Psapi.h>


// COptiClear 对话框

IMPLEMENT_DYNAMIC(COptiClear, CDialogEx)

COptiClear::COptiClear(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG3, pParent)	
	, m_Mem(_T(""))
	, m_Occupy(_T(""))
	, m_All(_T(""))
{
	m_IsOpenInject = FALSE;
	m_IsOpenAutoRun = FALSE;
	m_IsOpenPrivileges = FALSE;
}

COptiClear::~COptiClear()
{
}

void COptiClear::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_SMEM, m_Mem);
	DDX_Text(pDX, IDC_SOCCUPY, m_Occupy);
	DDX_Text(pDX, IDC_SALL, m_All);
	DDX_Control(pDX, IDC_PROGRESS1, m_MemPro);
}


BEGIN_MESSAGE_MAP(COptiClear, CDialogEx)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_BUTTON2, &COptiClear::OnBnClickedButton2)
	ON_BN_CLICKED(IDC_BUTTON1, &COptiClear::OnInjectButton)
	ON_BN_CLICKED(IDC_BUTTON17, &COptiClear::OnAutoRunButton)
	ON_BN_CLICKED(IDC_BUTTON18, &COptiClear::OnVSClearButton)
	ON_BN_CLICKED(IDC_BUTTON19, &COptiClear::OnAdjustPrivileges)
	ON_BN_CLICKED(IDC_BUTTON20, &COptiClear::OnBnClickedButton20)
	ON_BN_CLICKED(IDC_BUTTON21, &COptiClear::OnBnClickedButton21)
END_MESSAGE_MAP()


// COptiClear 消息处理程序

// 初始化函数
BOOL COptiClear::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化
	// 初始化内存
	GetMem();
	m_MemPro.SetRange(0, 100);

	// 设置计时器
	SetTimer(1, 1000, NULL);
	SetTimer(2, 1200000, NULL);
	
	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}

// 获取内存信息
void COptiClear::GetMem()
{
	MEMORYSTATUSEX memStatus = { sizeof(MEMORYSTATUSEX) };
	GlobalMemoryStatusEx(&memStatus);
	memStatus.dwMemoryLoad;
	CString Str[10] = {};
	Str[0].Format(L"%d", memStatus.dwMemoryLoad);
	Str[9] = Str[0] + L" %";
	Str[1].Format(L"%.2lf", memStatus.ullTotalPhys / 1024.0 / 1024 / 1024);
	Str[8] = Str[1] + L" GB";

	Str[2].Format(L"%.2lf", (memStatus.ullTotalPhys - memStatus.ullAvailPhys) / 1024.0 / 1024 / 1024);
	Str[7] = Str[2] + L" GB";

	// 设置文本
	m_Mem = Str[9];
	m_Occupy = Str[7];
	m_All = Str[8];
	// 设置滑条
	int num = _wtoi(Str[0].GetBuffer());
	m_MemPro.SetPos(num);

	UpdateData(FALSE);
}

// 计时器
void COptiClear::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if (nIDEvent == 1)
		GetMem();

	// 优化内存
	if (nIDEvent == 2)
	{
		MEMORYSTATUSEX memStatus = { sizeof(MEMORYSTATUSEX) };
		//清理内存
		DWORD dwPIDList[1000] = {};
		DWORD bufSize = sizeof(dwPIDList);
		DWORD dwNeedSize = 0;
		EnumProcesses(dwPIDList, bufSize, &dwNeedSize);
		for (DWORD i = 0; i < dwNeedSize / sizeof(DWORD); ++i)
		{
			HANDLE hProcess = OpenProcess(PROCESS_SET_QUOTA, false, dwPIDList[i]);
			SetProcessWorkingSetSize(hProcess, -1, -1);
		}
	}
	CDialogEx::OnTimer(nIDEvent);
}

// 内存优化按钮
void COptiClear::OnBnClickedButton2()
{
	MEMORYSTATUSEX memStatus = { sizeof(MEMORYSTATUSEX) };
	// 清理内存
	DWORD dwPIDList[1000] = {};
	DWORD bufSize = sizeof(dwPIDList);
	DWORD dwNeedSize = 0;
	EnumProcesses(dwPIDList, bufSize, &dwNeedSize);
	for (DWORD i = 0; i < dwNeedSize / sizeof(DWORD); ++i)
	{
		HANDLE hProcess = OpenProcess(PROCESS_SET_QUOTA, false, dwPIDList[i]);
		SetProcessWorkingSetSize(hProcess, -1, -1);
	}
}

// 解决按ESC键退出问题
BOOL COptiClear::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 在此添加专用代码和/或调用基类
	if (pMsg->message == WM_ISOPEN_INJECT)
	{
		m_IsOpenInject = FALSE;
	}

	if (pMsg->message == WM_KEYDOWN && (pMsg->wParam == VK_ESCAPE))
	{
		return TRUE;
	}
	return CDialogEx::PreTranslateMessage(pMsg);
}

// 注入按钮
void COptiClear::OnInjectButton()
{
	if (!m_IsOpenInject)
	{
		m_IsOpenInject = TRUE;
		InjectDll* obj = new InjectDll(this);	
		obj->m_pOpti = this;
		obj->Create(IDD_DIGINJECT, this);
		obj->ShowWindow(SW_SHOWNORMAL);
	}	   	
}

// 自启动按钮
void COptiClear::OnAutoRunButton()
{
	if (!m_IsOpenAutoRun)
	{
		m_IsOpenAutoRun = TRUE;
		AutoRun* obj = new AutoRun(this);
		obj->m_pFeather = this;
		obj->Create(IDD_AUTORUN, this);
		obj->ShowWindow(SW_SHOWNORMAL);
	}
}

// VS清理按钮
void COptiClear::OnVSClearButton()
{
	VSClear obj;
	obj.DoModal();
}

// 提权按钮
void COptiClear::OnAdjustPrivileges()
{
	if (!m_IsOpenPrivileges)
	{
		m_IsOpenPrivileges = TRUE;
		Privileges* obj = new Privileges(this);
		obj->m_pFeather = this;
		obj->Create(IDD_PRIVILEGES, this);
		obj->ShowWindow(SW_SHOWNORMAL);
	}
}

// 其他功能
void COptiClear::OnBnClickedButton20()
{
	OtherFunction obj;
	obj.DoModal();
}

// Hash 与 加密
void COptiClear::OnBnClickedButton21()
{
	// TODO: 在此添加控件通知处理程序代码
	HashAndCrypt obj;
	obj.DoModal();
}
