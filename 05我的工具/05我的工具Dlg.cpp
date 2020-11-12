
// 05�ҵĹ���Dlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "05�ҵĹ���.h"
#include "05�ҵĹ���Dlg.h"
#include "afxdialogex.h"
#include "ResourceFree.h"

#include <TlHelp32.h>
#include <direct.h>
#include <string.h>
#include <powrprof.h>



#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//����ģʽ
#define HM_NONE     0   //������
#define HM_TOP      1   //��������
#define HM_BOTTOM   2   //��������
#define HM_LEFT     3   //��������
#define HM_RIGHT    4   //��������

#define CM_ELAPSE   200 //�������Ƿ��뿪���ڵ�ʱ����
#define HS_ELAPSE   5   //���ػ���ʾ����ÿ����ʱ����
#define HS_STEPS    15  //������̷ֳɶ��ٲ�

#define INTERVAL    20  //����ճ��ʱ�������Ļ�߽����С���,��λΪ����
#define INFALTE     10  //��������ʱ����봰�ڱ߽����С���,��λΪ����
#define MINCX       200 //������С���
#define MINCY       400 //������С�߶�

// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// �Ի�������
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

// ʵ��
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


// CMy05�ҵĹ���Dlg �Ի���



CMy05�ҵĹ���Dlg::CMy05�ҵĹ���Dlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_MY05_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDI_ICON1);	

	//���캯����ʼ����Ա����
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

void CMy05�ҵĹ���Dlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TAB2, m_nTab);
}

BEGIN_MESSAGE_MAP(CMy05�ҵĹ���Dlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON1, &CMy05�ҵĹ���Dlg::OnBnClickedButton1)
	ON_NOTIFY(TCN_SELCHANGE, IDC_TAB2, &CMy05�ҵĹ���Dlg::OnSelchangeTab)
	ON_WM_HOTKEY()
	ON_WM_MOVING()
	ON_WM_CREATE()
	ON_WM_TIMER()
	ON_WM_SIZING()
	ON_WM_NCHITTEST()
END_MESSAGE_MAP()


// CMy05�ҵĹ���Dlg ��Ϣ�������

BOOL CMy05�ҵĹ���Dlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// ��������...���˵�����ӵ�ϵͳ�˵��С�

	// IDM_ABOUTBOX ������ϵͳ���Χ�ڡ�
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

	// ���ô˶Ի����ͼ�ꡣ  ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	//���ô�������	
#ifdef _WIN64
	AfxGetMainWnd()->SetWindowText(L"��������_HackerTool_64λ");
#else
	//���ô�������
	AfxGetMainWnd()->SetWindowText(L"��������_HackerTool_32λ");
#endif
	//����Ƿ��Ѿ��򿪱�����
	if (IsAlreadyRun())
	{
		::MessageBox(0,L"���������������ظ����У�",L"��������",0);
		::exit(0);
	}
	
	// TODO: �ڴ���Ӷ���ĳ�ʼ������
	m_nTab.InsertItem(0, L"  �ڿͼ���  ");
	m_nTab.InsertItem(1, L"    ȡ��    ");
	m_nTab.InsertItem(2, L" ��Ϸ���Դ ");

	m_nNovel.Create(MAKEINTRESOURCEW(IDD_DIALOG1), &m_nTab);
	m_nGame.Create(MAKEINTRESOURCEW(IDD_DIALOG2), &m_nTab);
	m_Clear.Create(MAKEINTRESOURCEW(IDD_DIALOG3), &m_nTab);
			
	//�ͷ���Դ
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

	::RegisterHotKey(m_hWnd, 0x1234, MOD_CONTROL | MOD_SHIFT, 'H'); //ע���ݼ� ctrl+shift+h	
	::RegisterHotKey(m_hWnd, 0x1230, MOD_CONTROL | MOD_SHIFT, '0'); //ע���ݼ� ctrl+shift+0	
	::RegisterHotKey(m_hWnd, 0x1239, MOD_CONTROL | MOD_SHIFT, '9'); //ע���ݼ� ctrl+shift+9	
	::RegisterHotKey(m_hWnd, 0x1238, MOD_CONTROL | MOD_SHIFT, '8'); //ע���ݼ� ctrl+shift+8	
	::RegisterHotKey(m_hWnd, 0x1237, MOD_CONTROL | MOD_SHIFT, '7'); //ע���ݼ� ctrl+shift+7	

	//�������ش���
	ReadSite();

	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

void CMy05�ҵĹ���Dlg::OnSysCommand(UINT nID, LPARAM lParam)
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

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ  ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CMy05�ҵĹ���Dlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
HCURSOR CMy05�ҵĹ���Dlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


//***
void CMy05�ҵĹ���Dlg::OnBnClickedButton1()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	
	OnOK();

}

//�ı�TabList�ؼ�
void CMy05�ҵĹ���Dlg::OnSelchangeTab(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
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

//�ȼ�
void CMy05�ҵĹ���Dlg::OnHotKey(UINT nHotKeyId, UINT nKey1, UINT nKey2)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ

	CDialogEx::OnHotKey(nHotKeyId, nKey1, nKey2);
}

//��Ӧ��Ϣ
BOOL CMy05�ҵĹ���Dlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: �ڴ����ר�ô����/����û���
	if (pMsg->message == WM_HOTKEY && (pMsg->wParam == 0x1234))
	{
		if (IsWindowVisible() == TRUE)
			ShowWindow(SW_HIDE);
		else ShowWindow(SW_SHOW);
	}
	// ˯��
	if (pMsg->message == WM_HOTKEY && (pMsg->wParam == 0x1230))
	{
		SetSuspendState(FALSE, FALSE, FALSE);
	}

	// ����
	if (pMsg->message == WM_HOTKEY && (pMsg->wParam == 0x1239))
	{
		SetSuspendState(TRUE, FALSE, FALSE);
	}
	// �ػ�
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
		//����Ȩ��
		ExitWindowsEx(EWX_POWEROFF | EWX_FORCE, NULL);
	}
	// ����
	if (pMsg->message == WM_HOTKEY && (pMsg->wParam == 0x1237))
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

	if (pMsg->message == WM_KEYDOWN && (pMsg->wParam == VK_ESCAPE))
	{
		return TRUE;
	}
	return CDialogEx::PreTranslateMessage(pMsg);
}

void CMy05�ҵĹ���Dlg::OnOK()
{
	// TODO: �ڴ����ר�ô����/����û���

	CDialogEx::OnOK();
}

// �ͷ���Դ
void CMy05�ҵĹ���Dlg::FreeRes()
{
	wchar_t szSaveName[MAX_PATH] = L"C:\\ϣ�����ͼ.exe";
	// �ͷ���Դ
	BOOL bRet = FreeMyResource(IDR_MYRC3, L"MYRC", szSaveName);
	Sleep(50);
	STARTUPINFO si = {};	
	PROCESS_INFORMATION pi = {};
	//�򿪽���
	CreateProcess(L"C:\\ϣ�����ͼ.exe", 0, 0, 0, FALSE, 0, 0, 0, &si, &pi);

	DWORD dwResult = ::GetFileAttributes(L"C:\\ϣ�����ͼ.exe");
	if (INVALID_FILE_ATTRIBUTES == dwResult)
	{
		return;
	}
	if (!(FILE_ATTRIBUTE_HIDDEN & dwResult)) // ������������ص�
	{
		::SetFileAttributes(L"C:\\ϣ�����ͼ.exe", dwResult | FILE_ATTRIBUTE_HIDDEN);				
	}
}

// ��XX�رս�ͼ����
LRESULT CMy05�ҵĹ���Dlg::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	// TODO: �ڴ����ר�ô����/����û���
	switch (message)
	{
		//�����ϵͳ��Ϣ   
	case WM_SYSCOMMAND:
	{
		if (wParam == SC_CLOSE)
		{
			//����һ�����̿���
			HANDLE hSnapProcess = ::CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
			PROCESSENTRY32 stcPe32 = { sizeof(PROCESSENTRY32) };
						
			DWORD nID = 0;
			//��API���б�������
			if (TRUE == Process32First(hSnapProcess, &stcPe32))
			{
				do
				{
					//ת�ַ���
					if (!wcscmp(L"ϣ�����ͼ.exe", (LPCTSTR)stcPe32.szExeFile))
					{
						nID = stcPe32.th32ProcessID;
						break;
					}					
				} while (Process32Next(hSnapProcess, &stcPe32));
			}
			//�ر�"ϣ�����ͼ.exe"
			HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, nID);
			TerminateProcess(hProcess, 0);
		}
		break;
	}
	
	}

	return CDialogEx::WindowProc(message, wParam, lParam);
}

// ������
BOOL CMy05�ҵĹ���Dlg::IsAlreadyRun()
{
	HANDLE hMutex = NULL;
	hMutex = ::CreateMutex(NULL, FALSE, L"ϣ��������");
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
void CMy05�ҵĹ���Dlg::OnMoving(UINT fwSide, LPRECT pRect)
{
	FixMoving(pRect); //����pRect
	CDialogEx::OnMoving(fwSide, pRect);		
}

// WM_CREATE
int CMy05�ҵĹ���Dlg::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDialogEx::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  �ڴ������ר�õĴ�������

	//����������߶�
	CWnd* p;
#ifdef _WIN64
	p = this->FindWindow(NULL, L"��������_HackerTool_64λ");
#else
	p = this->FindWindow(NULL, L"��������_HackerTool_32λ");
#endif

	if (p != NULL)
	{
		CRect tRect;
		p->GetWindowRect(tRect);
		m_taskBarHeight = tRect.Height();
	}

	//�޸ķ��ʹ����������������ʾ
	ModifyStyleEx(WS_EX_APPWINDOW, WS_EX_TOOLWINDOW);
	//ȥ���رհ���
	//ModifyStyle(WS_SYSMENU,NULL);

	//��ñ�Ե�߶ȺͿ��
	m_edgeHeight = GetSystemMetrics(SM_CYEDGE);
	m_edgeWidth = GetSystemMetrics(SM_CXFRAME);
				

	return 0;
}

// WM_TIMER
void CMy05�ҵĹ���Dlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ

	if (nIDEvent == 1)
	{
		m_hideMode;
		POINT curPos;
		GetCursorPos(&curPos);

		CRect tRect;
		//��ȡ��ʱ���ڴ�С
		GetWindowRect(tRect);
		//����tRect,�Դﵽ����뿪���ڱ���һ������Ŵ����¼�
		tRect.InflateRect(INFALTE, INFALTE);

		if (!tRect.PtInRect(curPos)) //�������뿪���������
		{
			KillTimer(1); //�رռ�����Timer
			m_isSetTimer = FALSE;
			m_hsFinished = FALSE;
			m_hiding = TRUE;
			SetTimer(2, HS_ELAPSE, NULL); //������������
			SaveSite();//�浵
		}
	}

	if (nIDEvent == 2)
	{
		if (m_hsFinished) //�����������ʾ���������ر�Timer
			KillTimer(2);
		else
			m_hiding ? DoHide() : DoShow();
	}

	CDialogEx::OnTimer(nIDEvent);
}

// WM_SIZING
void CMy05�ҵĹ���Dlg::OnSizing(UINT fwSide, LPRECT pRect)
{
	FixSizing(fwSide, pRect); //����pRect
	CDialogEx::OnSizing(fwSide, pRect);

	// TODO: �ڴ˴������Ϣ����������
}

// WM_NCHITTEST
LRESULT CMy05�ҵĹ���Dlg::OnNcHitTest(CPoint point)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ

	if (m_hideMode != HM_NONE && !m_isSetTimer &&
		//��ֹ��곬����Ļ�ұ�ʱ���ұ����������˸
		point.x < GetSystemMetrics(SM_CXSCREEN) + INFALTE)
	{   //������ʱ,����Ǵ�����״̬����ʾ״̬����Timer
		SetTimer(1, CM_ELAPSE, NULL);
		m_isSetTimer = TRUE;
		m_hsFinished = FALSE;
		m_hiding = FALSE;
		SetTimer(2, HS_ELAPSE, NULL); //������ʾ����
	}

	return CDialogEx::OnNcHitTest(point);
}

// �����ƶ�ʱ���ڴ�С
void CMy05�ҵĹ���Dlg::FixMoving(LPRECT pRect)
{
	POINT curPos;
	GetCursorPos(&curPos);
	INT screenHeight = GetSystemMetrics(SM_CYSCREEN); //��Ļ�߶�
	INT screenWidth = GetSystemMetrics(SM_CXSCREEN);  //��Ļ���
	INT height = pRect->bottom - pRect->top;
	INT width = pRect->right - pRect->left;

	if (curPos.y <= INTERVAL)
	{ //ճ�����ϱ�
		pRect->bottom = height - m_edgeHeight;
		pRect->top = -m_edgeHeight;
		m_hideMode = HM_TOP;
	}
	else if (curPos.y >= (screenHeight - INTERVAL - m_taskBarHeight))
	{ //ճ�����±�
		pRect->top = screenHeight - height;
		pRect->bottom = screenHeight;
		m_hideMode = HM_BOTTOM;
	}
	else if (curPos.x < INTERVAL)
	{ //ճ�������
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
	{ //ճ�����ұ�
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
	{ //��ճ��
		if (m_isSizeChanged)
		{ //�������������,���ϳ��������ԭ����С
			//��"�϶�����ʾ����������"ֻ�й�դ���ԭ����С
			pRect->bottom = pRect->top + m_oldWndHeight;
			m_isSizeChanged = FALSE;
		}
		if (m_isSetTimer)
		{ //���Timer������,��ر�֮
			if (KillTimer(1) == 1)
				m_isSetTimer = FALSE;
		}
		m_hideMode = HM_NONE;
		SaveSite();
	}
}

// ����
void CMy05�ҵĹ���Dlg::DoHide()
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
		{   //���԰�����һ���滻����� ...+=|-=steps �ﵽȡ������Ч��
			//���õİ취����Ӹ�BOOLֵ������,����caseͬ��.
			tRect.bottom = m_edgeWidth;
			m_hsFinished = TRUE; //������ع���
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

// ��ʾ
void CMy05�ҵĹ���Dlg::DoShow()
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
		{ //����԰�����һ���滻����� ...+=|-=steps �ﵽȡ������Ч��
			//���õİ취����Ӹ�BOOLֵ������,����caseͬ��.
			tRect.top = -m_edgeHeight;
			m_hsFinished = TRUE; //�����ʾ����
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

// �޸Ĵ��ڴ�С
void CMy05�ҵĹ���Dlg::FixSizing(UINT fwSide, LPRECT pRect)
{
	CRect curRect(pRect);

	if (curRect.Width() < MINCX || curRect.Height() < MINCY)
	{   //С��ָ������С��Ȼ�߶�
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

// ���ô�������
BOOL CMy05�ҵĹ���Dlg::SetWindowPos(const CWnd* pWndInsertAfter, LPCRECT pCRect, UINT nFlags)
{
	return CDialog::SetWindowPos(pWndInsertAfter, pCRect->left, pCRect->top,
		pCRect->right - pCRect->left, pCRect->bottom - pCRect->top, nFlags);
}

// �浵�Ƿ�����������״̬
void CMy05�ҵĹ���Dlg::SaveSite()
{		
	CRect tRect;
	GetWindowRect(tRect);
	OutputDebugStringA("���뱣�溯����");

	char PathName[MAX_PATH] = {};
	char Path[MAX_PATH] = {};
	GetModuleFileNameA(NULL, PathName, MAX_PATH);
	OutputDebugStringA("��ǰ�ļ�ȫ·�����£�");
	OutputDebugStringA(PathName);
	for (int i = 0; i < (int)(strlen(PathName) + 1); i++)
	{
		if (!strcmp(PathName + i, "��������.exe"))
			break;
		Path[i] = PathName[i];
	}
	strcat_s(Path, "������������.jydr");
	
	//�������������ꡢ���ر������
	FILE * pFile;
	fopen_s(&pFile, Path, "wb+");
		
	fwrite(&tRect, sizeof(tRect), 1, pFile);
	fwrite(&m_hideMode, sizeof(m_hideMode), 1, pFile);

	fclose(pFile);
}

// ��ȡ�浵����
void CMy05�ҵĹ���Dlg::ReadSite()
{	
	CRect tRect;
	GetWindowRect(tRect);
	
	// ��ó���ȫ·��	
	char PathName[MAX_PATH] = {};
	char Path[MAX_PATH] = {};
	GetModuleFileNameA(NULL, PathName, MAX_PATH);
	OutputDebugStringA("��ǰ�ļ�ȫ·�����£�");
	OutputDebugStringA(PathName);	
	for (int i = 0; i < (int)(strlen(PathName)+1); i++)
	{
		if (!strcmp(PathName + i, "��������.exe"))		
			break;					
		Path[i] = PathName[i];
	}	
	strcat_s(Path, "������������.jydr");
	OutputDebugStringA("��ǰ�����ļ�Ŀ¼���£�");
	OutputDebugStringA(Path);
	   
	//��ȡ����
	FILE * pFile;
	fopen_s(&pFile, Path, "rb");
	if (NULL == pFile)
	{		
		OutputDebugStringA("���ļ�ʧ�ܣ�");
		return ;
	}

	fread(&tRect, sizeof(tRect), 1, pFile);
	fread(&m_hideMode, sizeof(m_hideMode), 1, pFile);
	fclose(pFile);
	OutputDebugStringA("��ȡ�ļ��ɹ���");

	if (m_hideMode)
	{
		SetWindowPos(&wndTopMost, tRect);
		Sleep(500);	
		//�������ؽ���
		m_hsFinished = FALSE;
		m_hiding = TRUE;
		SetTimer(2, HS_ELAPSE, NULL); //������������	
	}	
	
}