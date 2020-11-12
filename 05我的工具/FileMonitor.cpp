// FileMonitor.cpp: 实现文件
//

#include "stdafx.h"
#include "FileMonitor.h"
#include "afxdialogex.h"
#include "05我的工具.h"

// FileMonitor 对话框

CString g_Str = {};
DWORD dwCount = 0;

IMPLEMENT_DYNAMIC(FileMonitor, CDialogEx)

FileMonitor::FileMonitor(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_FileMonitor, pParent)
{
		
}

FileMonitor::~FileMonitor()
{		
}

void FileMonitor::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_DataList);
}


BEGIN_MESSAGE_MAP(FileMonitor, CDialogEx)
	ON_WM_TIMER()
END_MESSAGE_MAP()


// FileMonitor 消息处理程序


void FileMonitor::OnTimer(UINT_PTR nIDEvent)
{
	static int nCount = 0;
	if (nCount != dwCount)
	{
		m_DataList.InsertItem(dwCount, g_Str);
		UpdateData(FALSE);
		nCount = dwCount;
	}
	
	CDialogEx::OnTimer(nIDEvent);
}


BOOL FileMonitor::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化
	// 设置计时器
	SetTimer(1, 100, NULL);

	m_DataList.InsertColumn(0, L"正在监控目录 D:\\图片\\", 0, 500);

	// 创建线程
	AfxBeginThread(ThreadProc, 0, THREAD_PRIORITY_NORMAL, 0, 0, NULL);
	
	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}

// 线程回调
UINT FileMonitor::ThreadProc(LPVOID)
{
	// 循环
	while (true)
	{
		// 获取监控目录句柄
		HANDLE m_hHandle = CreateFileW(L"D:\\图片\\",
			FILE_LIST_DIRECTORY,        // 要监视目录必须为这个宏
			FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING,
			FILE_FLAG_BACKUP_SEMANTICS, // 获取目录句柄需要这个宏
			NULL);
		if (INVALID_HANDLE_VALUE == m_hHandle)
		{
			AfxMessageBox(L"CreateFile!");
			return 0;
		}

		// 申请一个足够大的堆空间
		DWORD m_dwBufferSize = 2048;				
		PBYTE pSpace = new BYTE[m_dwBufferSize]();
		PFILE_NOTIFY_INFORMATION pni = (PFILE_NOTIFY_INFORMATION)pSpace;

		// 设置监控目录
		DWORD dwReturn = 0;
		BOOL bRet = ReadDirectoryChangesW(
			m_hHandle, pni, m_dwBufferSize, TRUE,
			FILE_NOTIFY_CHANGE_ATTRIBUTES | // 修改文件属性
			FILE_NOTIFY_CHANGE_FILE_NAME |  // 修改文件名
			FILE_NOTIFY_CHANGE_CREATION |   // 创建文件
			FILE_NOTIFY_CHANGE_LAST_ACCESS |// 最后的时间的任何修改
			FILE_NOTIFY_CHANGE_LAST_WRITE,  // 最后一次写入文件
			&dwReturn, NULL, NULL);
		if (!bRet)
		{
			bRet = TRUE;
			return 0;
		}

		switch (pni->Action)
		{
		case FILE_ACTION_ADDED:
		{
			CString str = L"[新增文件]：";
			str += pni->FileName;
			g_Str = str;
		}
		break;
		case FILE_ACTION_REMOVED:
		{
			CString str = L"[删除文件]：";
			str += pni->FileName;
			g_Str = str;

			break;
		}
		case FILE_ACTION_MODIFIED:
		{
			CString str = L"[文件属性或时间戳被修改]：";
			str += pni->FileName;
			g_Str = str;
			break;
		}
		case FILE_ACTION_RENAMED_OLD_NAME:
		{
			CString str = L"[文件名被修改]：";
			str += pni->FileName;
			g_Str = str;
			break;
		}

		default:
			break;
		}
		dwCount++;
		delete[] pSpace;
	}		
	return 0;
}
