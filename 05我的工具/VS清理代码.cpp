// COptiClear.cpp: 实现文件
//

#include "stdafx.h"
#include "COptiClear.h"
#include "afxdialogex.h"
#include "05我的工具.h"
#include <corecrt_wstdlib.h>
#include <Psapi.h>


// COptiClear 对话框

IMPLEMENT_DYNAMIC(COptiClear, CDialogEx)

COptiClear::COptiClear(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG3, pParent)	
	, m_Mem(_T(""))
	, m_Occupy(_T(""))
	, m_All(_T(""))
	, m_ClearSize(_T(""))
	, m_AllSize(_T(""))
	, m_flagName(_T(""))
	, m_FileProName(_T(".obj\\.tlog\\.lastbuildstate\\.idb\\.pdb\\.pch\\.res\\.ilk\\.suo\\.sdf\\.ipch\\\r\n.log\\.iobj\\.ipdb\\.db\\.dll\\.exe\\"))
	, m_(_T(""))
{

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
	DDX_Text(pDX, IDC_SFILE1, m_ClearSize);
	DDX_Text(pDX, IDC_SFILE2, m_AllSize);
	DDX_Text(pDX, IDC_SFILENAME, m_flagName);
	DDX_Control(pDX, IDC_LIST1, m_FileList);
	DDX_Control(pDX, IDC_LIST2, m_ShowDeletList);
	DDX_Text(pDX, IDC_EDIT1, m_FileProName);
	DDX_Text(pDX, IDC_SCLIENT, m_);
}


BEGIN_MESSAGE_MAP(COptiClear, CDialogEx)
	ON_BN_CLICKED(IDOK, &COptiClear::OnBnClickedOk)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDOK2, &COptiClear::OnBnClickedOk2)
	ON_WM_DROPFILES()
	ON_BN_CLICKED(IDC_BUTTON2, &COptiClear::OnBnClickedButton2)
END_MESSAGE_MAP()


// COptiClear 消息处理程序

//初始化函数
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

	//第一个列表设置可选和名称
	m_FileList.InsertColumn(0, L"拖拽文件名称：", 0, 700);
	m_FileList.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);

	m_ShowDeletList.InsertColumn(0, L"待删除文件信息：", 0, 700);
	//UpdateData(FALSE);


	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}


//VS垃圾检测
void COptiClear::OnBnClickedOk()
{
	//先清空所有数据，防止干扰
	m_ShowDeletList.DeleteAllItems();
	DelString.clear();
	FileCount = 0;
	DestCount = 0;
	CountSize.QuadPart = 0;
	DestSize.QuadPart = 0;

	//遍历所有文件
	for (auto Path : PathString)
	{
		FindFile(Path);
	}

	//显示文件信息
	m_ = L"可删除大小：";
	m_flagName = L"总文件大小：";

	CString Str[2];
	Str[0].Format(L"%.2lfMB", CountSize.QuadPart / 1024.0 / 1024.0);
	m_AllSize = Str[0].GetBuffer();	

	Str[1].Format(L"%.2lfMB", DestSize.QuadPart / 1024.0 / 1024.0);
	m_ClearSize = Str[1].GetBuffer();

	UpdateData(FALSE);		
}

//VS垃圾清理
void COptiClear::OnBnClickedOk2()
{
	DeleteCount = 0;
	//遍历所有要删除的文件的路径
	for (auto File : DelString)
	{
		//判断是否删除成功
		if (DeleteFile(File))
		{
			++DeleteCount;
			m_ShowDeletList.InsertItem(0, File);
		}
	}
	
	//显示文件信息
	m_ = L"已删除个数：";
	m_flagName = L"可删除个数：";

	CString Str[2];
	Str[0].Format(L"%d", DestCount);
	m_AllSize = Str[0].GetBuffer();

	Str[1].Format(L"%d", DeleteCount);
	m_ClearSize = Str[1].GetBuffer();

	UpdateData(FALSE);

}

// 响应拖拽文件
void COptiClear::OnDropFiles(HDROP hDropInfo)
{	
	//获得被拖动文件的数目
	int DropCount = DragQueryFile(hDropInfo, -1, NULL, 0);
	//循环获取每一个文件
	for (int i = 0; i < DropCount; ++i)
	{
		WCHAR wcStr[MAX_PATH] = {};
		//获得拖拽的第i个文件的文件名
		DragQueryFile(hDropInfo, i, wcStr, MAX_PATH);
		//如果这是一个文件夹，则加入到列表中
		if (GetFileAttributes(wcStr) & FILE_ATTRIBUTE_DIRECTORY)
		{
			//如果路径不存在与列表中
			if (find(PathString.begin(), PathString.end(), wcStr) == PathString.end())
			{
				PathString.push_back(wcStr);//则添加到保存的容器中
				m_FileList.InsertItem(0, wcStr);//添加在列表控件上
			}
		}
	}
	DragFinish(hDropInfo); //结束后释放内存
	CDialogEx::OnDropFiles(hDropInfo);
}

//遍历文件信息
void COptiClear::FindFile(CString Path) 
{
	//查找第一个文件
	WIN32_FIND_DATA FileInfo = { 0 };
	HANDLE File = FindFirstFile(Path + "\\*", &FileInfo);
	//遍历文件夹的所有文件
	int nNum = 0;
	if (File != INVALID_HANDLE_VALUE)
	{
		do
		{
			//如果是文件夹就递归
			if (FileInfo.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			{
				//排除两个文件夹
				if (CString(".") != FileInfo.cFileName && CString("..") != FileInfo.cFileName)
				{
					FindFile(Path + CString("\\") + FileInfo.cFileName);
				}
			}
			else //是文件夹时
			{
				++FileCount;//数量加1
				CountSize.QuadPart += FileInfo.nFileSizeLow;
				//判断后缀名
				if (m_FileProName.Find(PathFindExtension(FileInfo.cFileName)) != -1)
				{
					++DestCount;
					DestSize.QuadPart += FileInfo.nFileSizeLow;
					m_ShowDeletList.InsertItem(0, Path + CString("\\") + FileInfo.cFileName);
					//WaitDelete.SetItemText(nNum,0,Path + CString("\\") + FileInfo.cFileName);
					DelString.push_back(Path + CString("\\") + FileInfo.cFileName);
					nNum++;
				}
			}
		} while (FindNextFile(File, &FileInfo));
	}
}

//获取内存信息
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

	//设置文本
	m_Mem = Str[9];
	m_Occupy = Str[7];
	m_All = Str[8];
	//设置滑条
	int num = _wtoi(Str[0].GetBuffer());
	m_MemPro.SetPos(num);

	UpdateData(FALSE);
}

//计时器
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

//内存优化
void COptiClear::OnBnClickedButton2()
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

//解决按ESC键退出问题
BOOL COptiClear::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 在此添加专用代码和/或调用基类
	if (pMsg->message == WM_KEYDOWN && (pMsg->wParam == VK_ESCAPE))
	{
		return TRUE;
	}
	return CDialogEx::PreTranslateMessage(pMsg);
}
