// RemoteCmd.cpp: 实现文件
//

#include "stdafx.h"
#include "RemoteCmd.h"
#include "afxdialogex.h"
#include "05我的工具.h"

// RemoteCmd 对话框

BOOL m_bIsStart = FALSE;
CString g_strBuf;

IMPLEMENT_DYNAMIC(RemoteCmd, CDialogEx)

RemoteCmd::RemoteCmd(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_RemoteCMD, pParent)
	, m_IP(_T("输入正确IP地址"))
	, m_CmdCode(_T("输入正确CMD指令"))
	, m_Temp("")
	, m_Edit(_T(""))
{
	
}

RemoteCmd::~RemoteCmd()
{
}

void RemoteCmd::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, m_IP);
	DDX_Text(pDX, IDC_EDIT3, m_CmdCode);
	DDX_Text(pDX, IDC_EDIT2, m_Edit);
}


BEGIN_MESSAGE_MAP(RemoteCmd, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON11, &RemoteCmd::OnBnClickedButton11)
	ON_BN_CLICKED(IDC_BUTTON1, &RemoteCmd::OnBnClickedButton1)
	ON_WM_TIMER()
END_MESSAGE_MAP()


// RemoteCmd 消息处理程序

// ip 确定按钮
void RemoteCmd::OnBnClickedButton11()
{
	UpdateData(TRUE);
	if (!m_IP.GetLength())
		return;

	// 获取IP后  连接服务端
	
	m_Socket.Connect(m_IP.GetBuffer(), 1234);	
	Sleep(100);
	m_IP = L"已经连接服务器！";
	UpdateData(FALSE);

}

// cmd 命令确定发送按钮
void RemoteCmd::OnBnClickedButton1()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData(TRUE);
	m_Temp = (CStringA)m_CmdCode;
	char szBuffer[512] = {};
	strcpy_s(szBuffer, m_Temp.GetLength() + 1, m_Temp.GetBuffer());
	m_Socket.Send(szBuffer, 512);
}

// 处理接受数据
void RemoteCmd::ShowData(BYTE szBuf[], int nSize)
{	
	CStringA strTemp = (char*)szBuf;
	g_strBuf = (CString)strTemp;
	m_bIsStart = TRUE;	
}


void RemoteCmd::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	if (m_bIsStart)
	{
		m_Edit = g_strBuf;
		UpdateData(FALSE);
		m_bIsStart = FALSE;
	}

	CDialogEx::OnTimer(nIDEvent);
}


BOOL RemoteCmd::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化
	SetTimer(1, 100, NULL);
	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}
