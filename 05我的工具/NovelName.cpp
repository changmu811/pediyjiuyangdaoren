// NovelName.cpp : 实现文件
//

#include "stdafx.h"
#include "05我的工具.h"
#include "NovelName.h"
#include "afxdialogex.h"


// CNovelName 对话框

IMPLEMENT_DYNAMIC(CNovelName, CDialogEx)

CNovelName::CNovelName(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_DIALOG1, pParent)
	, m_strEdit(_T(""))
	
	, m_nRadio(0)
{
	
}

CNovelName::~CNovelName()
{
}

void CNovelName::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT2, m_objEdit);
	DDX_Text(pDX, IDC_EDIT2, m_strEdit);
	DDX_Radio(pDX, IDC_RADIO1, m_nRadio);

	DDX_Control(pDX, IDC_CHECK1, m_Check);
}


BEGIN_MESSAGE_MAP(CNovelName, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON1, &CNovelName::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON4, &CNovelName::OnBnClickedButton4)
	ON_BN_CLICKED(IDC_BUTTON8, &CNovelName::OnBnClickedButton8)
	ON_BN_CLICKED(IDC_BUTTON5, &CNovelName::OnBnClickedButton5)
	ON_BN_CLICKED(IDC_BUTTON9, &CNovelName::OnBnClickedButton9)
	ON_BN_CLICKED(IDC_BUTTON3, &CNovelName::OnBnClickedButton3)
	ON_BN_CLICKED(IDC_BUTTON7, &CNovelName::OnBnClickedButton7)
	ON_BN_CLICKED(IDC_BUTTON6, &CNovelName::OnBnClickedButton6)
	ON_BN_CLICKED(IDC_BUTTON10, &CNovelName::OnBnClickedButton10)
	ON_WM_TIMER()
END_MESSAGE_MAP()


// CNovelName 消息处理程序


BOOL CNovelName::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	
	// TODO:  在此添加额外的初始化
	m_editFont.CreatePointFont(98, L"微软雅黑");
	m_objEdit.SetFont(&m_editFont); // 设置新字体

	srand((unsigned)time(0));

	//计时器
	SetTimer(1, 1000, NULL);
	//默认选中启动
	
	EnumRegTable();

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}

//人名按钮
void CNovelName::OnBnClickedButton1()
{
	//m_strEdit.Empty();	
	//UpdateData(FALSE);
	// TODO: 在此添加控件通知处理程序代码

	//清空编辑框
	m_objEdit.SetSel(0, -1);
	m_objEdit.Clear();
	UpdateData(TRUE);

	//打印名称
	for (int i = 0; i < 50; i++)
	{
		if (!m_nRadio)
			m_strEdit += m_MakeName.ManName();
		else
			m_strEdit += m_MakeName.WomanName();
		m_strEdit += L"\r\n";
		m_objEdit.LineScroll(m_objEdit.GetLineCount() - 1, 1);
	}
	UpdateData(FALSE);
}

//药材名
void CNovelName::OnBnClickedButton4()
{
	// TODO: 在此添加控件通知处理程序代码
	//清空编辑框
	m_objEdit.SetSel(0, -1);
	m_objEdit.Clear();
	UpdateData(TRUE);

	//打印名称
	for (int i = 0; i < 50; i++)
	{		
		m_strEdit += m_MakeName.CrudeDrugsName();
		m_strEdit += L"\r\n";
		m_objEdit.LineScroll(m_objEdit.GetLineCount() - 1, 1);
	}
	UpdateData(FALSE);
}

//丹药名
void CNovelName::OnBnClickedButton8()
{
	m_objEdit.SetSel(0, -1);
	m_objEdit.Clear();
	UpdateData(TRUE);
	
	for (int i = 0; i < 50; i++)
	{
		m_strEdit += m_MakeName.ElixirName();
		m_strEdit += L"\r\n";
		m_objEdit.LineScroll(m_objEdit.GetLineCount() - 1, 1);
	}
	UpdateData(FALSE);
}

//天材地宝
void CNovelName::OnBnClickedButton5()
{
	m_objEdit.SetSel(0, -1);
	m_objEdit.Clear();
	UpdateData(TRUE);

	for (int i = 0; i < 50; i++)
	{
		m_strEdit += m_MakeName.MineralName();
		m_strEdit += L"\r\n";
		m_objEdit.LineScroll(m_objEdit.GetLineCount() - 1, 1);
	}
	UpdateData(FALSE);
}

//武器法宝
void CNovelName::OnBnClickedButton9()
{
	m_objEdit.SetSel(0, -1);
	m_objEdit.Clear();
	UpdateData(TRUE);

	for (int i = 0; i < 50; i++)
	{
		m_strEdit += m_MakeName.WeaponName();
		m_strEdit += L"\r\n";
		m_objEdit.LineScroll(m_objEdit.GetLineCount() - 1, 1);
	}
	UpdateData(FALSE);
}

//地名
void CNovelName::OnBnClickedButton3()
{
	m_objEdit.SetSel(0, -1);
	m_objEdit.Clear();
	UpdateData(TRUE);

	for (int i = 0; i < 50; i++)
	{
		m_strEdit += m_MakeName.PlaceName();
		m_strEdit += L"\r\n";
		m_objEdit.LineScroll(m_objEdit.GetLineCount() - 1, 1);
	}
	UpdateData(FALSE);
}

//国家皇朝
void CNovelName::OnBnClickedButton7()
{
	m_objEdit.SetSel(0, -1);
	m_objEdit.Clear();
	UpdateData(TRUE);

	for (int i = 0; i < 50; i++)
	{
		m_strEdit += m_MakeName.CountryName();
		m_strEdit += L"\r\n";
		m_objEdit.LineScroll(m_objEdit.GetLineCount() - 1, 1);
	}
	UpdateData(FALSE);
}

//宗派名
void CNovelName::OnBnClickedButton6()
{
	m_objEdit.SetSel(0, -1);
	m_objEdit.Clear();
	UpdateData(TRUE);

	for (int i = 0; i < 50; i++)
	{
		m_strEdit += m_MakeName.SectName();
		m_strEdit += L"\r\n";
		m_objEdit.LineScroll(m_objEdit.GetLineCount() - 1, 1);
	}
	UpdateData(FALSE);
}

//仙禽异兽
void CNovelName::OnBnClickedButton10()
{
	m_objEdit.SetSel(0, -1);
	m_objEdit.Clear();
	UpdateData(TRUE);

	for (int i = 0; i < 50; i++)
	{
		m_strEdit += m_MakeName.MonsterName();
		m_strEdit += L"\r\n";
		m_objEdit.LineScroll(m_objEdit.GetLineCount() - 1, 1);
	}
	UpdateData(FALSE);
}

//设置自动启动
void CNovelName::OnTimer(UINT_PTR nIDEvent)
{
	// 获得程序全路径
	wchar_t PathName[MAX_PATH] = {};
	GetModuleFileName(NULL, PathName, MAX_PATH);

	// 修改注册表，添加自动启动
	if (m_Check.GetCheck() == 1 && m_YesNoRun == FALSE)
	{		
		// 默认权限
		HKEY hKey;
		// 打开注册表键
		if (ERROR_SUCCESS != ::RegOpenKeyEx(HKEY_CURRENT_USER, L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run", 0, KEY_WRITE, &hKey))
		{			
			return;
		}
		// 修改注册表值，实现开机自启
		if (ERROR_SUCCESS != ::RegSetValueExW(hKey, L"九阳道人黑客工具", 0, REG_SZ, (BYTE *)PathName, (1 + ::lstrlen(PathName))*2))
		{
			::RegCloseKey(hKey);			
			return;
		}
		// 关闭注册表键
		::RegCloseKey(hKey);
		m_YesNoRun = TRUE;
		m_Check.SetCheck(1);
	}
	else if (m_Check.GetCheck() == 0)//删除自动启动
	{
		//添加以下代码  
		HKEY   hKey;		
		//找到系统的启动项   
		LPCTSTR lpRun = _T("Software\\Microsoft\\Windows\\CurrentVersion\\Run");
		//打开启动项Key   
		long lRet = RegOpenKeyEx(HKEY_CURRENT_USER, lpRun, 0, KEY_WRITE, &hKey);
		if (lRet == ERROR_SUCCESS)
		{
			//删除注册  
			RegDeleteValue(hKey, L"九阳道人黑客工具");
			RegCloseKey(hKey);
		}
		m_YesNoRun = FALSE;
	}
	CDialogEx::OnTimer(nIDEvent);
}

//枚举注册表的自启动项
void CNovelName::EnumRegTable()
{	
	CString szRegPath;
	szRegPath.Format(L"Software\\Microsoft\\Windows\\CurrentVersion\\Run");
	// 管理员权限
	HKEY hKey;
	// 打开注册表键
	if (ERROR_SUCCESS != ::RegOpenKeyEx(HKEY_CURRENT_USER, szRegPath.GetBuffer(), 0, KEY_READ, &hKey))
	{
		return;
	}

	//获取注册表子键信息，判断是否有子键
	TCHAR szName[MAX_PATH] = {};	//子键名称
	DWORD dwSize = 512;	//名称大小
	DWORD dwIndex = 0;		
	while (ERROR_NO_MORE_ITEMS != ::RegEnumValue(hKey, dwIndex++, szName, &dwSize, NULL,
			NULL, NULL, 0))
	{		
		if (!wcscmp(szName, L"九阳道人黑客工具"))
		{
			m_YesNoRun = TRUE;
			m_Check.SetCheck(1);
			break;
		}		
		m_Check.SetCheck(0);
		//必须重置该值，否则只能遍历两三次；
		dwSize = 512;		
	}
	// 关闭注册表键
	::RegCloseKey(hKey);
	
}


BOOL CNovelName::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 在此添加专用代码和/或调用基类
	if (pMsg->message == WM_KEYDOWN && (pMsg->wParam == VK_ESCAPE))
	{
		return TRUE;
	}
	return CDialogEx::PreTranslateMessage(pMsg);
}
