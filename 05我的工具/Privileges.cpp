// Privileges.cpp: 实现文件
//

#include "stdafx.h"
#include "Privileges.h"
#include "afxdialogex.h"
#include "05我的工具.h"
#include "AdjustPrivileges.h"
#include "Share.h"
#include <locale.h>

// Privileges 对话框

IMPLEMENT_DYNAMIC(Privileges, CDialogEx)

Privileges::Privileges(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_PRIVILEGES, pParent)
	, m_DirExe(_T("提示：请将要提权运行的EXE程序拖到这里或者打开！"))
{
	// 提权
	//EnbalePrivileges(::GetCurrentProcess(), "SeSecurityPrivilege");
}

Privileges::~Privileges()
{
}

void Privileges::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, m_DirExe);
}


BEGIN_MESSAGE_MAP(Privileges, CDialogEx)
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_BUTTON1, &Privileges::OnBypassUAC)
	ON_WM_DROPFILES()
	ON_BN_CLICKED(IDC_BUTTON11, &Privileges::OnBnClickedButton11)
	ON_BN_CLICKED(IDC_BUTTON19, &Privileges::OnCOM)
	ON_BN_CLICKED(IDC_BUTTON20, &Privileges::OnShell)
END_MESSAGE_MAP()


// Privileges 消息处理程序


void Privileges::OnClose()
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	m_pFeather->m_IsOpenPrivileges = FALSE;
	CDialogEx::OnClose();
}


BOOL Privileges::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 在此添加专用代码和/或调用基类
	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_ESCAPE)
	{
		return TRUE;
	}
	return CDialogEx::PreTranslateMessage(pMsg);
}

// 白名单按钮
void Privileges::OnBypassUAC()
{
	if (IsExeFile())
	{
		HKEY hKey = NULL;
		// 创建注册表项
		::RegCreateKeyEx(HKEY_CURRENT_USER, L"SOFTWARE\\Classes\\mscfile\\Shell\\Open\\Command", 0, NULL, 0,
			KEY_ALL_ACCESS | KEY_WOW64_64KEY, NULL, &hKey, NULL);
		if (!hKey)
			return;
		// 设置键值
		::RegSetValueEx(hKey, NULL, 0, REG_SZ, (BYTE*)m_DirExe.GetBuffer(), (lstrlen(m_DirExe) + 1) * 2);
		// 关闭
		::RegCloseKey(hKey);
		
		CString szCompPath = L"C:\\Windows\\System32\\CompMgmtLauncher.exe";	
		// 以管理员的权限打开目标文件
		ShellExecute(0, 0, szCompPath, 0, 0, SW_SHOWNORMAL);

		// 必须停两秒，等待CompMgmtLauncher.exe查找路径配置完毕
		Sleep(2000); 
		//删除注册key，若不删除，此电脑之后会一直打不开PC管理
		RegDeleteKey(HKEY_CURRENT_USER, L"SOFTWARE\\Classes\\mscfile\\Shell\\Open\\Command");
		
		return;
	}
	AfxMessageBox(L"不是EXE文件，无法操作！");	
}

// 响应拖拽文件
void Privileges::OnDropFiles(HDROP hDropInfo)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	TCHAR szPath[MAX_PATH] = {};
	::DragQueryFile(hDropInfo, 0, szPath, MAX_PATH);
	m_DirExe = szPath;
	UpdateData(FALSE);

	CDialogEx::OnDropFiles(hDropInfo);
}

// 打开
void Privileges::OnBnClickedButton11()
{
	CFileDialog dlg(TRUE, //TRUE为OPEN对话框，FALSE为SAVE AS对话框
		NULL,
		NULL,
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		(LPCTSTR)_TEXT("EXE Files (*.exe)|*.exe||"),
		NULL);
	if (dlg.DoModal() == IDOK)
	{
		// 打开成功，保存文件路径
		m_DirExe = dlg.GetPathName();
	}
	else
	{
		m_DirExe = _T("打开文件失败！");
	}
	UpdateData(FALSE);
}

// COM组件按钮
void Privileges::OnCOM()
{
	
}

// 是否是EXE文件
BOOL Privileges::IsExeFile()
{
	int strLength = lstrlen(m_DirExe);
	if (m_DirExe[strLength - 1] == 'e')
	{
		if (m_DirExe[strLength - 2] == 'x')
			if (m_DirExe[strLength - 3] == 'e')
				if (m_DirExe[strLength - 4] == '.')
				{
					return TRUE;
				}
	}
	return FALSE;
}

// 加壳按钮
void Privileges::OnShell()
{
#ifdef _WIN64
	AfxMessageBox(L"64位不支持加壳！");
#else
	if (IsExeFile())
	{
		// 获取DLL路径
		char exePath[MAX_PATH] = {};
		// 保存原来的语言系列
		char* old_locale = _strdup(setlocale(LC_CTYPE, NULL));
		// 设置新的语言系列
		setlocale(LC_CTYPE, setlocale(LC_ALL, ""));

		size_t siSize = 0;
		::wcstombs_s(&siSize, exePath, MAX_PATH, m_DirExe.GetBuffer(), _TRUNCATE);

		// 还原语言系列  
		setlocale(LC_CTYPE, old_locale);
		free(old_locale);
		OpenShell(exePath);		
		return;
	}
	AfxMessageBox(L"不是EXE文件，无法操作！");
#endif
}
