
// 05我的工具Dlg.cpp : 实现文件
//

#include "stdafx.h"
#include "05我的工具.h"
#include "05我的工具Dlg.h"
#include "afxdialogex.h"
#include "ResourceFree.h"

#include <TlHelp32.h>
#include <direct.h>
#include <string.h>
#include <powrprof.h>



#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//收缩模式
#define HM_NONE     0   //不收缩
#define HM_TOP      1   //向上收缩
#define HM_BOTTOM   2   //向下收缩
#define HM_LEFT     3   //向左收缩
#define HM_RIGHT    4   //向右收缩

#define CM_ELAPSE   200 //检测鼠标是否离开窗口的时间间隔
#define HS_ELAPSE   5   //隐藏或显示过程每步的时间间隔
#define HS_STEPS    15  //抽屉过程分成多少步

#define INTERVAL    20  //触发粘附时鼠标与屏幕边界的最小间隔,单位为象素
#define INFALTE     10  //触发收缩时鼠标与窗口边界的最小间隔,单位为象素
#define MINCX       200 //窗口最小宽度
#define MINCY       400 //窗口最小高度

// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
	
END_MESSAGE_MAP()


// CMy05我的工具Dlg 对话框



CMy05我的工具Dlg::CMy05我的工具Dlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_MY05_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDI_ICON1);	

	//构造函数初始化成员变量
	m_isSizeChanged = FALSE;
	m_isSetTimer = FALSE;
	m_hsFinished = TRUE;
	m_hiding = FALSE;

	m_oldWndHeight = MINCY;
	m_taskBarHeight = 30;
	m_edgeHeight = 0;
	m_edgeWidth = 0;
	m_hideMode = HM_NONE;
}

void CMy05我的工具Dlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TAB2, m_nTab);
}

BEGIN_MESSAGE_MAP(CMy05我的工具Dlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON1, &CMy05我的工具Dlg::OnBnClickedButton1)
	ON_NOTIFY(TCN_SELCHANGE, IDC_TAB2, &CMy05我的工具Dlg::OnSelchangeTab)
	ON_WM_HOTKEY()
	ON_WM_MOVING()
	ON_WM_CREATE()
	ON_WM_TIMER()
	ON_WM_SIZING()
	ON_WM_NCHITTEST()
END_MESSAGE_MAP()


// CMy05我的工具Dlg 消息处理程序

BOOL CMy05我的工具Dlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	//设置窗口名称	
#ifdef _WIN64
	AfxGetMainWnd()->SetWindowText(L"九阳道人_HackerTool_64位");
#else
	//设置窗口名称
	AfxGetMainWnd()->SetWindowText(L"九阳道人_HackerTool_32位");
#endif
	//检测是否已经打开本程序
	if (IsAlreadyRun())
	{
		::MessageBox(0,L"已启动程序，请勿重复运行！",L"九阳道人",0);
		::exit(0);
	}
	
	// TODO: 在此添加额外的初始化代码
	m_nTab.InsertItem(0, L"  黑客技术  ");
	m_nTab.InsertItem(1, L"    取名    ");
	m_nTab.InsertItem(2, L" 游戏与电源 ");

	m_nNovel.Create(MAKEINTRESOURCEW(IDD_DIALOG1), &m_nTab);
	m_nGame.Create(MAKEINTRESOURCEW(IDD_DIALOG2), &m_nTab);
	m_Clear.Create(MAKEINTRESOURCEW(IDD_DIALOG3), &m_nTab);
			
	//释放资源
	FreeRes();

	RECT rect;
	m_nTab.GetClientRect(&rect);
	rect.top += 26;
	rect.left += 1;
	rect.bottom -= 2;
	rect.right -= 3;
	m_nNovel.MoveWindow(&rect);
	m_nGame.MoveWindow(&rect);
	m_Clear.MoveWindow(&rect);

	m_Clear.ShowWindow(SW_SHOW);

	::RegisterHotKey(m_hWnd, 0x1234, MOD_CONTROL | MOD_SHIFT, 'H'); //注册快捷键 ctrl+shift+h	
	::RegisterHotKey(m_hWnd, 0x1230, MOD_CONTROL | MOD_SHIFT, '0'); //注册快捷键 ctrl+shift+0	
	::RegisterHotKey(m_hWnd, 0x1239, MOD_CONTROL | MOD_SHIFT, '9'); //注册快捷键 ctrl+shift+9	
	::RegisterHotKey(m_hWnd, 0x1238, MOD_CONTROL | MOD_SHIFT, '8'); //注册快捷键 ctrl+shift+8	
	::RegisterHotKey(m_hWnd, 0x1237, MOD_CONTROL | MOD_SHIFT, '7'); //注册快捷键 ctrl+shift+7	

	//开机隐藏窗口
	ReadSite();

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CMy05我的工具Dlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CMy05我的工具Dlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CMy05我的工具Dlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


//***
void CMy05我的工具Dlg::OnBnClickedButton1()
{
	// TODO: 在此添加控件通知处理程序代码
	
	OnOK();

}

//改变TabList控件
void CMy05我的工具Dlg::OnSelchangeTab(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: 在此添加控件通知处理程序代码
	int num = m_nTab.GetCurSel();
	switch (num)
	{
	case 0:
		m_nNovel.ShowWindow(SW_HIDE);
		m_nGame.ShowWindow(SW_HIDE);
		m_Clear.ShowWindow(SW_SHOW);
		break;
	case 1:
		m_nNovel.ShowWindow(SW_SHOW);
		m_nGame.ShowWindow(SW_HIDE);
		m_Clear.ShowWindow(SW_HIDE);
		break;
	case 2:
		m_nNovel.ShowWindow(SW_HIDE);
		m_nGame.ShowWindow(SW_SHOW);
		m_Clear.ShowWindow(SW_HIDE);
		break;
	}
	*pResult = 0;
}

//热键
void CMy05我的工具Dlg::OnHotKey(UINT nHotKeyId, UINT nKey1, UINT nKey2)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	CDialogEx::OnHotKey(nHotKeyId, nKey1, nKey2);
}

//响应消息
BOOL CMy05我的工具Dlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 在此添加专用代码和/或调用基类
	if (pMsg->message == WM_HOTKEY && (pMsg->wParam == 0x1234))
	{
		if (IsWindowVisible() == TRUE)
			ShowWindow(SW_HIDE);
		else ShowWindow(SW_SHOW);
	}
	// 睡眠
	if (pMsg->message == WM_HOTKEY && (pMsg->wParam == 0x1230))
	{
		SetSuspendState(FALSE, FALSE, FALSE);
	}

	// 休眠
	if (pMsg->message == WM_HOTKEY && (pMsg->wParam == 0x1239))
	{
		SetSuspendState(TRUE, FALSE, FALSE);
	}
	// 关机
	if (pMsg->message == WM_HOTKEY && (pMsg->wParam == 0x1238))
	{
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
	// 重启
	if (pMsg->message == WM_HOTKEY && (pMsg->wParam == 0x1237))
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

	if (pMsg->message == WM_KEYDOWN && (pMsg->wParam == VK_ESCAPE))
	{
		return TRUE;
	}
	return CDialogEx::PreTranslateMessage(pMsg);
}

void CMy05我的工具Dlg::OnOK()
{
	// TODO: 在此添加专用代码和/或调用基类

	CDialogEx::OnOK();
}

// 释放资源
void CMy05我的工具Dlg::FreeRes()
{
	wchar_t szSaveName[MAX_PATH] = L"C:\\希望峰截图.exe";
	// 释放资源
	BOOL bRet = FreeMyResource(IDR_MYRC3, L"MYRC", szSaveName);
	Sleep(50);
	STARTUPINFO si = {};	
	PROCESS_INFORMATION pi = {};
	//打开进程
	CreateProcess(L"C:\\希望峰截图.exe", 0, 0, 0, FALSE, 0, 0, 0, &si, &pi);

	DWORD dwResult = ::GetFileAttributes(L"C:\\希望峰截图.exe");
	if (INVALID_FILE_ATTRIBUTES == dwResult)
	{
		return;
	}
	if (!(FILE_ATTRIBUTE_HIDDEN & dwResult)) // 如果它不是隐藏的
	{
		::SetFileAttributes(L"C:\\希望峰截图.exe", dwResult | FILE_ATTRIBUTE_HIDDEN);				
	}
}

// 点XX关闭截图程序
LRESULT CMy05我的工具Dlg::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	// TODO: 在此添加专用代码和/或调用基类
	switch (message)
	{
		//如果是系统消息   
	case WM_SYSCOMMAND:
	{
		if (wParam == SC_CLOSE)
		{
			//创建一个进程快照
			HANDLE hSnapProcess = ::CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
			PROCESSENTRY32 stcPe32 = { sizeof(PROCESSENTRY32) };
						
			DWORD nID = 0;
			//用API进行遍历快照
			if (TRUE == Process32First(hSnapProcess, &stcPe32))
			{
				do
				{
					//转字符串
					if (!wcscmp(L"希望峰截图.exe", (LPCTSTR)stcPe32.szExeFile))
					{
						nID = stcPe32.th32ProcessID;
						break;
					}					
				} while (Process32Next(hSnapProcess, &stcPe32));
			}
			//关闭"希望峰截图.exe"
			HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, nID);
			TerminateProcess(hProcess, 0);
		}
		break;
	}
	
	}

	return CDialogEx::WindowProc(message, wParam, lParam);
}

// 互斥体
BOOL CMy05我的工具Dlg::IsAlreadyRun()
{
	HANDLE hMutex = NULL;
	hMutex = ::CreateMutex(NULL, FALSE, L"希望峰制造");
	if (hMutex)
	{
		if (ERROR_ALREADY_EXISTS == ::GetLastError())
		{
			return TRUE;
		}
	}
	return FALSE;
}

// WM_MOVING
void CMy05我的工具Dlg::OnMoving(UINT fwSide, LPRECT pRect)
{
	FixMoving(pRect); //修正pRect
	CDialogEx::OnMoving(fwSide, pRect);		
}

// WM_CREATE
int CMy05我的工具Dlg::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDialogEx::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  在此添加您专用的创建代码

	//获得任务栏高度
	CWnd* p;
#ifdef _WIN64
	p = this->FindWindow(NULL, L"九阳道人_HackerTool_64位");
#else
	p = this->FindWindow(NULL, L"九阳道人_HackerTool_32位");
#endif

	if (p != NULL)
	{
		CRect tRect;
		p->GetWindowRect(tRect);
		m_taskBarHeight = tRect.Height();
	}

	//修改风格使得他不在任务栏显示
	ModifyStyleEx(WS_EX_APPWINDOW, WS_EX_TOOLWINDOW);
	//去掉关闭按键
	//ModifyStyle(WS_SYSMENU,NULL);

	//获得边缘高度和宽度
	m_edgeHeight = GetSystemMetrics(SM_CYEDGE);
	m_edgeWidth = GetSystemMetrics(SM_CXFRAME);
				

	return 0;
}

// WM_TIMER
void CMy05我的工具Dlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	if (nIDEvent == 1)
	{
		m_hideMode;
		POINT curPos;
		GetCursorPos(&curPos);

		CRect tRect;
		//获取此时窗口大小
		GetWindowRect(tRect);
		//膨胀tRect,以达到鼠标离开窗口边沿一定距离才触发事件
		tRect.InflateRect(INFALTE, INFALTE);

		if (!tRect.PtInRect(curPos)) //如果鼠标离开了这个区域
		{
			KillTimer(1); //关闭检测鼠标Timer
			m_isSetTimer = FALSE;
			m_hsFinished = FALSE;
			m_hiding = TRUE;
			SetTimer(2, HS_ELAPSE, NULL); //开启收缩过程
			SaveSite();//存档
		}
	}

	if (nIDEvent == 2)
	{
		if (m_hsFinished) //如果收缩或显示过程完毕则关闭Timer
			KillTimer(2);
		else
			m_hiding ? DoHide() : DoShow();
	}

	CDialogEx::OnTimer(nIDEvent);
}

// WM_SIZING
void CMy05我的工具Dlg::OnSizing(UINT fwSide, LPRECT pRect)
{
	FixSizing(fwSide, pRect); //修正pRect
	CDialogEx::OnSizing(fwSide, pRect);

	// TODO: 在此处添加消息处理程序代码
}

// WM_NCHITTEST
LRESULT CMy05我的工具Dlg::OnNcHitTest(CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	if (m_hideMode != HM_NONE && !m_isSetTimer &&
		//防止鼠标超出屏幕右边时向右边收缩造成闪烁
		point.x < GetSystemMetrics(SM_CXSCREEN) + INFALTE)
	{   //鼠标进入时,如果是从收缩状态到显示状态则开启Timer
		SetTimer(1, CM_ELAPSE, NULL);
		m_isSetTimer = TRUE;
		m_hsFinished = FALSE;
		m_hiding = FALSE;
		SetTimer(2, HS_ELAPSE, NULL); //开启显示过程
	}

	return CDialogEx::OnNcHitTest(point);
}

// 修正移动时窗口大小
void CMy05我的工具Dlg::FixMoving(LPRECT pRect)
{
	POINT curPos;
	GetCursorPos(&curPos);
	INT screenHeight = GetSystemMetrics(SM_CYSCREEN); //屏幕高度
	INT screenWidth = GetSystemMetrics(SM_CXSCREEN);  //屏幕宽度
	INT height = pRect->bottom - pRect->top;
	INT width = pRect->right - pRect->left;

	if (curPos.y <= INTERVAL)
	{ //粘附在上边
		pRect->bottom = height - m_edgeHeight;
		pRect->top = -m_edgeHeight;
		m_hideMode = HM_TOP;
	}
	else if (curPos.y >= (screenHeight - INTERVAL - m_taskBarHeight))
	{ //粘附在下边
		pRect->top = screenHeight - height;
		pRect->bottom = screenHeight;
		m_hideMode = HM_BOTTOM;
	}
	else if (curPos.x < INTERVAL)
	{ //粘附在左边
		if (!m_isSizeChanged)
		{
			CRect tRect;
			GetWindowRect(tRect);
			m_oldWndHeight = tRect.Height();
		}
		pRect->right = width;
		pRect->left = 0;
		m_isSizeChanged = TRUE;
		m_hideMode = HM_LEFT;
	}
	else if (curPos.x >= (screenWidth - INTERVAL))
	{ //粘附在右边
		if (!m_isSizeChanged)
		{
			CRect tRect;
			GetWindowRect(tRect);
			m_oldWndHeight = tRect.Height();
		}
		pRect->left = screenWidth - width;
		pRect->right = screenWidth;
		m_isSizeChanged = TRUE;
		m_hideMode = HM_RIGHT;
	}
	else
	{ //不粘附
		if (m_isSizeChanged)
		{ //如果收缩到两边,则拖出来后会变回原来大小
			//在"拖动不显示窗口内容下"只有光栅变回原来大小
			pRect->bottom = pRect->top + m_oldWndHeight;
			m_isSizeChanged = FALSE;
		}
		if (m_isSetTimer)
		{ //如果Timer开启了,则关闭之
			if (KillTimer(1) == 1)
				m_isSetTimer = FALSE;
		}
		m_hideMode = HM_NONE;
		SaveSite();
	}
}

// 隐藏
void CMy05我的工具Dlg::DoHide()
{
	if (m_hideMode == HM_NONE)
		return;

	CRect tRect;
	GetWindowRect(tRect);

	INT height = tRect.Height();
	INT width = tRect.Width();

	INT steps = 0;

	switch (m_hideMode)
	{
	case HM_TOP:
		steps = height / HS_STEPS;
		tRect.bottom -= steps;
		if (tRect.bottom <= m_edgeWidth)
		{   //可以把下面一句替换上面的 ...+=|-=steps 达到取消抽屉效果
			//更好的办法是添加个BOOL值来控制,其他case同样.
			tRect.bottom = m_edgeWidth;
			m_hsFinished = TRUE; //完成隐藏过程
		}
		tRect.top = tRect.bottom - height;
		break;
	case HM_BOTTOM:
		steps = height / HS_STEPS;
		tRect.top += steps;
		if (tRect.top >= (GetSystemMetrics(SM_CYSCREEN) - m_edgeWidth))
		{
			tRect.top = GetSystemMetrics(SM_CYSCREEN) - m_edgeWidth;
			m_hsFinished = TRUE;
		}
		tRect.bottom = tRect.top + height;
		break;
	case HM_LEFT:
		steps = width / HS_STEPS;
		tRect.right -= steps;
		if (tRect.right <= m_edgeWidth)
		{
			tRect.right = m_edgeWidth;
			m_hsFinished = TRUE;
		}
		tRect.left = tRect.right - width;	
		break;
	case HM_RIGHT:
		steps = width / HS_STEPS;
		tRect.left += steps;
		if (tRect.left >= (GetSystemMetrics(SM_CXSCREEN) - m_edgeWidth))
		{
			tRect.left = GetSystemMetrics(SM_CXSCREEN) - m_edgeWidth;
			m_hsFinished = TRUE;
		}
		tRect.right = tRect.left + width;		
		break;
	default:
		break;
	}
	SetWindowPos(&wndTopMost, tRect);
}

// 显示
void CMy05我的工具Dlg::DoShow()
{
	if (m_hideMode == HM_NONE)
		return;

	CRect tRect;
	GetWindowRect(tRect);
	INT height = tRect.Height();
	INT width = tRect.Width();

	INT steps = 0;

	switch (m_hideMode)
	{
	case HM_TOP:
		steps = height / HS_STEPS;
		tRect.top += steps;
		if (tRect.top >= -m_edgeHeight)
		{ //你可以把下面一句替换上面的 ...+=|-=steps 达到取消抽屉效果
			//更好的办法是添加个BOOL值来控制,其他case同样.
			tRect.top = -m_edgeHeight;
			m_hsFinished = TRUE; //完成显示过程
		}
		tRect.bottom = tRect.top + height;
		break;
	case HM_BOTTOM:
		steps = height / HS_STEPS;
		tRect.top -= steps;
		if (tRect.top <= (GetSystemMetrics(SM_CYSCREEN) - height))
		{
			tRect.top = GetSystemMetrics(SM_CYSCREEN) - height;
			m_hsFinished = TRUE;
		}
		tRect.bottom = tRect.top + height;
		break;
	case HM_LEFT:
		steps = width / HS_STEPS;
		tRect.right += steps;
		if (tRect.right >= width)
		{
			tRect.right = width;
			m_hsFinished = TRUE;
		}
		tRect.left = tRect.right - width;		
		break;
	case HM_RIGHT:
		steps = width / HS_STEPS;
		tRect.left -= steps;
		if (tRect.left <= (GetSystemMetrics(SM_CXSCREEN) - width))
		{
			tRect.left = GetSystemMetrics(SM_CXSCREEN) - width;
			m_hsFinished = TRUE;
		}
		tRect.right = tRect.left + width;		
		break;
	default:
		break;
	}

	SetWindowPos(&wndTopMost, tRect);
}

// 修改窗口大小
void CMy05我的工具Dlg::FixSizing(UINT fwSide, LPRECT pRect)
{
	CRect curRect(pRect);

	if (curRect.Width() < MINCX || curRect.Height() < MINCY)
	{   //小于指定的最小宽度或高度
		switch (fwSide)
		{
		case WMSZ_BOTTOM:
			pRect->bottom = pRect->top + MINCY;
			break;
		case WMSZ_BOTTOMLEFT:
			if (curRect.Width() <= MINCX)
				pRect->left = pRect->right - MINCX;
			if (curRect.Height() <= MINCY)
				pRect->bottom = pRect->top + MINCY;
			break;
		case WMSZ_BOTTOMRIGHT:
			if (curRect.Width() < MINCX)
				pRect->right = pRect->left + MINCX;
			if (curRect.Height() < MINCY)
				pRect->bottom = pRect->top + MINCY;
			break;
		case WMSZ_LEFT:
			pRect->left = pRect->right - MINCX;
			break;
		case WMSZ_RIGHT:
			pRect->right = pRect->left + MINCX;
			break;
		case WMSZ_TOP:
			pRect->top = pRect->bottom - MINCY;
			break;
		case WMSZ_TOPLEFT:
			if (curRect.Width() <= MINCX)
				pRect->left = pRect->right - MINCX;
			if (curRect.Height() <= MINCY)
				pRect->top = pRect->bottom - MINCY;
			break;
		case WMSZ_TOPRIGHT:
			if (curRect.Width() < MINCX)
				pRect->right = pRect->left + MINCX;
			if (curRect.Height() < MINCY)
				pRect->top = pRect->bottom - MINCY;
			break;
		}
	}
}

// 设置窗口坐标
BOOL CMy05我的工具Dlg::SetWindowPos(const CWnd* pWndInsertAfter, LPCRECT pCRect, UINT nFlags)
{
	return CDialog::SetWindowPos(pWndInsertAfter, pCRect->left, pCRect->top,
		pCRect->right - pCRect->left, pCRect->bottom - pCRect->top, nFlags);
}

// 存档是否处于贴边隐藏状态
void CMy05我的工具Dlg::SaveSite()
{		
	CRect tRect;
	GetWindowRect(tRect);
	OutputDebugStringA("进入保存函数！");

	char PathName[MAX_PATH] = {};
	char Path[MAX_PATH] = {};
	GetModuleFileNameA(NULL, PathName, MAX_PATH);
	OutputDebugStringA("当前文件全路径如下：");
	OutputDebugStringA(PathName);
	for (int i = 0; i < (int)(strlen(PathName) + 1); i++)
	{
		if (!strcmp(PathName + i, "九阳道人.exe"))
			break;
		Path[i] = PathName[i];
	}
	strcat_s(Path, "九阳道人配置.jydr");
	
	//创建并储存坐标、隐藏标记设置
	FILE * pFile;
	fopen_s(&pFile, Path, "wb+");
		
	fwrite(&tRect, sizeof(tRect), 1, pFile);
	fwrite(&m_hideMode, sizeof(m_hideMode), 1, pFile);

	fclose(pFile);
}

// 读取存档设置
void CMy05我的工具Dlg::ReadSite()
{	
	CRect tRect;
	GetWindowRect(tRect);
	
	// 获得程序全路径	
	char PathName[MAX_PATH] = {};
	char Path[MAX_PATH] = {};
	GetModuleFileNameA(NULL, PathName, MAX_PATH);
	OutputDebugStringA("当前文件全路径如下：");
	OutputDebugStringA(PathName);	
	for (int i = 0; i < (int)(strlen(PathName)+1); i++)
	{
		if (!strcmp(PathName + i, "九阳道人.exe"))		
			break;					
		Path[i] = PathName[i];
	}	
	strcat_s(Path, "九阳道人配置.jydr");
	OutputDebugStringA("当前配置文件目录如下：");
	OutputDebugStringA(Path);
	   
	//读取配置
	FILE * pFile;
	fopen_s(&pFile, Path, "rb");
	if (NULL == pFile)
	{		
		OutputDebugStringA("打开文件失败！");
		return ;
	}

	fread(&tRect, sizeof(tRect), 1, pFile);
	fread(&m_hideMode, sizeof(m_hideMode), 1, pFile);
	fclose(pFile);
	OutputDebugStringA("读取文件成功！");

	if (m_hideMode)
	{
		SetWindowPos(&wndTopMost, tRect);
		Sleep(500);	
		//贴边隐藏界面
		m_hsFinished = FALSE;
		m_hiding = TRUE;
		SetTimer(2, HS_ELAPSE, NULL); //开启收缩过程	
	}	
	
}