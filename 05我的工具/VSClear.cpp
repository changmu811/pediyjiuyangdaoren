// VSClear.cpp: 实现文件
//

#include "stdafx.h"
#include "VSClear.h"
#include "afxdialogex.h"
#include "05我的工具.h"

// VSClear 对话框

IMPLEMENT_DYNAMIC(VSClear, CDialogEx)

VSClear::VSClear(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG4, pParent)
	, m_FileFormat(_T(".obj\\.tlog\\.lastbuildstate\\.idb\\.pdb\\.pch\\.res\\.ilk\\.suo\\.sdf\\\r\n.ipch\\.log\\.iobj\\.ipdb\\.db\\.dll\\.exe\\"))
	, m_HintA(_T(""))
	, m_HintB(_T(""))
{
	
}

VSClear::~VSClear()
{
}

void VSClear::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST2, m_FileList);
	DDX_Control(pDX, IDC_LIST3, m_ClearList);
	DDX_Text(pDX, IDC_EDIT1, m_FileFormat);
	DDX_Text(pDX, IDC_TISI0, m_HintA);
	DDX_Text(pDX, IDC_TISHI1, m_HintB);
}


BEGIN_MESSAGE_MAP(VSClear, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON2, &VSClear::OnGetInfo)
	ON_BN_CLICKED(IDC_BUTTON12, &VSClear::OnBeginDelete)
	ON_WM_DROPFILES()
	ON_BN_CLICKED(IDC_BUTTON13, &VSClear::OnInitList)
END_MESSAGE_MAP()


// VSClear 消息处理程序


BOOL VSClear::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	m_FileList.InsertColumn(0, L"拖拽的文件名称:", 0, 445);
	m_FileList.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);

	m_ClearList.InsertColumn(0, L"文件信息：", 0, 800);

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}

// 拖拽响应
void VSClear::OnDropFiles(HDROP hDropInfo)
{
	// 获取被拖拽文件的数量
	UINT dropCount = DragQueryFile(hDropInfo, -1, nullptr, 0);

	// 循环获取每一个文件
	for (UINT i = 0; i < dropCount; i++)
	{
		TCHAR szFileName[MAX_PATH] = {};
		// 获取拖拽的第i个文件的文件名
		DragQueryFile(hDropInfo, i, szFileName, MAX_PATH);
		// 如果这是一个文件夹 则加入到容器中
		if (GetFileAttributes(szFileName) & FILE_ATTRIBUTE_DIRECTORY)
		{
			// 如果容器中没有该路径
			if (find(vecPath.begin(), vecPath.end(), szFileName) == vecPath.end())
			{
				// 将该路径压入vector容器
				vecPath.push_back(szFileName);
				// 插入到列表控件
				m_FileList.InsertItem(0, szFileName);
			}
		}
	}
	// 释放拖拽文件的内存
	DragFinish(hDropInfo);

	CDialogEx::OnDropFiles(hDropInfo);
}

// 遍历文件信息
void VSClear::EnumFileInfo(CString Path)
{
	WIN32_FIND_DATA fileInfo = {};
	// 查找第一个文件
	HANDLE hFile = ::FindFirstFile(Path + L"\\*", &fileInfo);

	// 遍历文件夹内的所有文件
	if (hFile != INVALID_HANDLE_VALUE)
	{
		do 
		{
			// 若是文件夹则递归
			if (fileInfo.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			{
				// 排除当前目录和上层目录
				if (fileInfo.cFileName != CString(".") && fileInfo.cFileName != CString(".."))
				{
					EnumFileInfo(Path + L"\\" + fileInfo.cFileName);
				}
			}
			else // 是文件时
			{
				// 记录所有文件总大小				
				allSize.QuadPart += fileInfo.nFileSizeLow;
				// 匹配后缀名相同的文件 执行显示与储存
				if (m_FileFormat.Find(PathFindExtension(fileInfo.cFileName)) != -1)
				{					
					delSize.QuadPart += fileInfo.nFileSizeLow;
					m_ClearList.InsertItem(0, Path + L"\\" + fileInfo.cFileName);
					vecDelete.push_back(Path + L"\\" + fileInfo.cFileName);
				}
			}
		} while (FindNextFile(hFile, &fileInfo));		
	}
}

// 获取信息按钮
void VSClear::OnGetInfo()
{
	UpdateData(TRUE);
	// 清空数据，避免被干扰
	m_ClearList.DeleteAllItems();
	vecDelete.clear();
	allSize.QuadPart = 0;
	delSize.QuadPart = 0;
	
	// 遍历拖拽来的所有文件夹
	for (auto Path : vecPath)
		EnumFileInfo(Path);
	
	// 显示信息
	m_HintA.Format(L"可删除大小：%.2lf MB", delSize.QuadPart / 1024.0 / 1024);
	m_HintB.Format(L"总文件大小：%.2lf MB", allSize.QuadPart / 1024.0 / 1024);
	UpdateData(FALSE);
}

// 开始删除按钮
void VSClear::OnBeginDelete()
{	
	UpdateData(TRUE);
	// 清空数据，避免被干扰
	m_ClearList.DeleteAllItems();	
	dwDeleteCount = 0;
	dwEnableCount = 0;

	for (auto Path : vecDelete)
	{
		++dwEnableCount;
		// 如果删除成功
		if (DeleteFile(Path))
		{
			++dwDeleteCount;
			m_ClearList.InsertItem(0, Path);
		}
		else // 删除失败
			m_ClearList.InsertItem(0, Path + L"  ★★★此文件删除失败！"); 
	}

	// 显示信息
	m_HintA.Format(L"已删除文件数目：%d 个", dwDeleteCount);
	m_HintB.Format(L"可删除文件数目：%d 个", dwEnableCount);
	UpdateData(FALSE);
}

// 清空列表与容器按钮
void VSClear::OnInitList()
{
	m_ClearList.DeleteAllItems();
	m_FileList.DeleteAllItems();
	vecDelete.clear();
	vecPath.clear();
	allSize.QuadPart = 0;
	delSize.QuadPart = 0;
	dwDeleteCount = 0;
	dwEnableCount = 0;
	m_HintA = L"";
	m_HintB = L"";
	UpdateData(FALSE);

	// TODO: 在此添加控件通知处理程序代码
}
