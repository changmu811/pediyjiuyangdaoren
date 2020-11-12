// MyProGame.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "05�ҵĹ���.h"
#include "MyProGame.h"
#include "afxdialogex.h"
#include "CSnake.h"
#include <powrprof.h>
#pragma comment(lib, "PowrProf.lib")
// CMyProGame �Ի���

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


// CMyProGame ��Ϣ�������

//��ť1
void CMyProGame::OnBnClickedCancel()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	//CDialogEx::OnCancel(); �رմ���
	AfxMessageBox(L"�������ع��ܣ���ϲ���ã�10,000,000,000.00");
}

//̰�����߳�
unsigned __stdcall ThreadProc()
{
	CSnake obj;
	obj.RunGame();
	return TRUE;
}

//̰���߰�ť
void CMyProGame::OnBnClickedButton1()
{	
	if (m_IsGame)
	{
		m_Butt1.SetWindowText(L"�ر�̰����");
		m_ThreadHandle = (HANDLE)_beginthreadex(0, 0, (_beginthreadex_proc_type)ThreadProc, 0, 0, 0);
		m_IsGame = FALSE;
	}
	else
	{
		m_Butt1.SetWindowText(L"����̰����");		
		TerminateThread(m_ThreadHandle, 0);		
		m_IsGame = TRUE;
	}
	

}

//̹�˴�ս
void CMyProGame::OnBnClickedButton11()
{
	AfxMessageBox(L"�����ڴ�����Ϸ���ڿ����С���");
}

//�ػ�
void CMyProGame::OnBnClickedButton2()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	HANDLE hToken = NULL;
	HANDLE hProcess = GetCurrentProcess();
	OpenProcessToken(hProcess, TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken);
	TOKEN_PRIVILEGES tp = { 0 };
	LookupPrivilegeValue(0, SE_SHUTDOWN_NAME, &tp.Privileges[0].Luid);
	tp.PrivilegeCount = 1;
	tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
	AdjustTokenPrivileges(hToken, FALSE, &tp, sizeof(tp), NULL, NULL);
	//����Ȩ��
	ExitWindowsEx(EWX_POWEROFF | EWX_FORCE, NULL);		
}

//����
void CMyProGame::OnBnClickedButton12()
{
	HANDLE hToken = NULL;
	HANDLE hProcess = GetCurrentProcess();
	OpenProcessToken(hProcess, TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken);
	TOKEN_PRIVILEGES tp = { 0 };
	LookupPrivilegeValue(0, SE_SHUTDOWN_NAME, &tp.Privileges[0].Luid);
	tp.PrivilegeCount = 1;
	tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
	//����Ȩ��
	AdjustTokenPrivileges(hToken, FALSE, &tp, sizeof(tp), NULL, NULL);		
	ExitWindowsEx(EWX_REBOOT | EWX_FORCE, NULL);
}

//����
void CMyProGame::OnBnClickedButton14()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	SetSuspendState(TRUE, FALSE, FALSE);
}

//ע��
void CMyProGame::OnBnClickedButton13()
{
	ExitWindowsEx(EWX_LOGOFF | EWX_FORCE, NULL);	
}

//˯��
void CMyProGame::OnBnClickedButton15()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	SetSuspendState(FALSE, FALSE, FALSE);
}

//����
void CMyProGame::OnBnClickedButton16()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	LockWorkStation();
}
