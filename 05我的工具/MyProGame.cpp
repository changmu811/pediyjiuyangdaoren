// MyProGame.cpp : 实现文件
//

#include "stdafx.h"
#include "05我的工具.h"
#include "MyProGame.h"
#include "afxdialogex.h"
#include "CSnake.h"
#include <powrprof.h>
#pragma comment(lib, "PowrProf.lib")
// CMyProGame 对话框

IMPLEMENT_DYNAMIC(CMyProGame, CDialogEx)

CMyProGame::CMyProGame(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_DIALOG2, pParent)
{

}

CMyProGame::~CMyProGame()
{
}

void CMyProGame::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_BUTTON1, m_Butt1);
}


BEGIN_MESSAGE_MAP(CMyProGame, CDialogEx)
	ON_BN_CLICKED(IDCANCEL, &CMyProGame::OnBnClickedCancel)
	ON_BN_CLICKED(IDC_BUTTON1, &CMyProGame::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON11, &CMyProGame::OnBnClickedButton11)
	ON_BN_CLICKED(IDC_BUTTON2, &CMyProGame::OnBnClickedButton2)
	ON_BN_CLICKED(IDC_BUTTON12, &CMyProGame::OnBnClickedButton12)
	ON_BN_CLICKED(IDC_BUTTON14, &CMyProGame::OnBnClickedButton14)
	ON_BN_CLICKED(IDC_BUTTON13, &CMyProGame::OnBnClickedButton13)
	ON_BN_CLICKED(IDC_BUTTON15, &CMyProGame::OnBnClickedButton15)
	ON_BN_CLICKED(IDC_BUTTON16, &CMyProGame::OnBnClickedButton16)
END_MESSAGE_MAP()


// CMyProGame 消息处理程序

//按钮1
void CMyProGame::OnBnClickedCancel()
{
	// TODO: 在此添加控件通知处理程序代码
	//CDialogEx::OnCancel(); 关闭窗口
	AfxMessageBox(L"开启隐藏功能，恭喜你获得￥10,000,000,000.00");
}

//贪吃蛇线程
unsigned __stdcall ThreadProc()
{
	CSnake obj;
	obj.RunGame();
	return TRUE;
}

//贪吃蛇按钮
void CMyProGame::OnBnClickedButton1()
{	
	if (m_IsGame)
	{
		m_Butt1.SetWindowText(L"关闭贪吃蛇");
		m_ThreadHandle = (HANDLE)_beginthreadex(0, 0, (_beginthreadex_proc_type)ThreadProc, 0, 0, 0);
		m_IsGame = FALSE;
	}
	else
	{
		m_Butt1.SetWindowText(L"开启贪吃蛇");		
		TerminateThread(m_ThreadHandle, 0);		
		m_IsGame = TRUE;
	}
	

}

//坦克大战
void CMyProGame::OnBnClickedButton11()
{
	AfxMessageBox(L"敬请期待，游戏正在开发中……");
}

//关机
void CMyProGame::OnBnClickedButton2()
{
	// TODO: 在此添加控件通知处理程序代码
	HANDLE hToken = NULL;
	HANDLE hProcess = GetCurrentProcess();
	OpenProcessToken(hProcess, TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken);
	TOKEN_PRIVILEGES tp = { 0 };
	LookupPrivilegeValue(0, SE_SHUTDOWN_NAME, &tp.Privileges[0].Luid);
	tp.PrivilegeCount = 1;
	tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
	AdjustTokenPrivileges(hToken, FALSE, &tp, sizeof(tp), NULL, NULL);
	//提升权限
	ExitWindowsEx(EWX_POWEROFF | EWX_FORCE, NULL);		
}

//重启
void CMyProGame::OnBnClickedButton12()
{
	HANDLE hToken = NULL;
	HANDLE hProcess = GetCurrentProcess();
	OpenProcessToken(hProcess, TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken);
	TOKEN_PRIVILEGES tp = { 0 };
	LookupPrivilegeValue(0, SE_SHUTDOWN_NAME, &tp.Privileges[0].Luid);
	tp.PrivilegeCount = 1;
	tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
	//提升权限
	AdjustTokenPrivileges(hToken, FALSE, &tp, sizeof(tp), NULL, NULL);		
	ExitWindowsEx(EWX_REBOOT | EWX_FORCE, NULL);
}

//休眠
void CMyProGame::OnBnClickedButton14()
{
	// TODO: 在此添加控件通知处理程序代码
	SetSuspendState(TRUE, FALSE, FALSE);
}

//注销
void CMyProGame::OnBnClickedButton13()
{
	ExitWindowsEx(EWX_LOGOFF | EWX_FORCE, NULL);	
}

//睡眠
void CMyProGame::OnBnClickedButton15()
{
	// TODO: 在此添加控件通知处理程序代码
	SetSuspendState(FALSE, FALSE, FALSE);
}

//锁屏
void CMyProGame::OnBnClickedButton16()
{
	// TODO: 在此添加控件通知处理程序代码
	LockWorkStation();
}
