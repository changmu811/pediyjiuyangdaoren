// NovelName.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "05�ҵĹ���.h"
#include "NovelName.h"
#include "afxdialogex.h"


// CNovelName �Ի���

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


// CNovelName ��Ϣ�������


BOOL CNovelName::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	
	// TODO:  �ڴ���Ӷ���ĳ�ʼ��
	m_editFont.CreatePointFont(98, L"΢���ź�");
	m_objEdit.SetFont(&m_editFont); // ����������

	srand((unsigned)time(0));

	//��ʱ��
	SetTimer(1, 1000, NULL);
	//Ĭ��ѡ������
	
	EnumRegTable();

	return TRUE;  // return TRUE unless you set the focus to a control
				  // �쳣: OCX ����ҳӦ���� FALSE
}

//������ť
void CNovelName::OnBnClickedButton1()
{
	//m_strEdit.Empty();	
	//UpdateData(FALSE);
	// TODO: �ڴ���ӿؼ�֪ͨ����������

	//��ձ༭��
	m_objEdit.SetSel(0, -1);
	m_objEdit.Clear();
	UpdateData(TRUE);

	//��ӡ����
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

//ҩ����
void CNovelName::OnBnClickedButton4()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	//��ձ༭��
	m_objEdit.SetSel(0, -1);
	m_objEdit.Clear();
	UpdateData(TRUE);

	//��ӡ����
	for (int i = 0; i < 50; i++)
	{		
		m_strEdit += m_MakeName.CrudeDrugsName();
		m_strEdit += L"\r\n";
		m_objEdit.LineScroll(m_objEdit.GetLineCount() - 1, 1);
	}
	UpdateData(FALSE);
}

//��ҩ��
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

//��ĵر�
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

//��������
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

//����
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

//���һʳ�
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

//������
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

//��������
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

//�����Զ�����
void CNovelName::OnTimer(UINT_PTR nIDEvent)
{
	// ��ó���ȫ·��
	wchar_t PathName[MAX_PATH] = {};
	GetModuleFileName(NULL, PathName, MAX_PATH);

	// �޸�ע�������Զ�����
	if (m_Check.GetCheck() == 1 && m_YesNoRun == FALSE)
	{		
		// Ĭ��Ȩ��
		HKEY hKey;
		// ��ע����
		if (ERROR_SUCCESS != ::RegOpenKeyEx(HKEY_CURRENT_USER, L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run", 0, KEY_WRITE, &hKey))
		{			
			return;
		}
		// �޸�ע���ֵ��ʵ�ֿ�������
		if (ERROR_SUCCESS != ::RegSetValueExW(hKey, L"�������˺ڿ͹���", 0, REG_SZ, (BYTE *)PathName, (1 + ::lstrlen(PathName))*2))
		{
			::RegCloseKey(hKey);			
			return;
		}
		// �ر�ע����
		::RegCloseKey(hKey);
		m_YesNoRun = TRUE;
		m_Check.SetCheck(1);
	}
	else if (m_Check.GetCheck() == 0)//ɾ���Զ�����
	{
		//������´���  
		HKEY   hKey;		
		//�ҵ�ϵͳ��������   
		LPCTSTR lpRun = _T("Software\\Microsoft\\Windows\\CurrentVersion\\Run");
		//��������Key   
		long lRet = RegOpenKeyEx(HKEY_CURRENT_USER, lpRun, 0, KEY_WRITE, &hKey);
		if (lRet == ERROR_SUCCESS)
		{
			//ɾ��ע��  
			RegDeleteValue(hKey, L"�������˺ڿ͹���");
			RegCloseKey(hKey);
		}
		m_YesNoRun = FALSE;
	}
	CDialogEx::OnTimer(nIDEvent);
}

//ö��ע������������
void CNovelName::EnumRegTable()
{	
	CString szRegPath;
	szRegPath.Format(L"Software\\Microsoft\\Windows\\CurrentVersion\\Run");
	// ����ԱȨ��
	HKEY hKey;
	// ��ע����
	if (ERROR_SUCCESS != ::RegOpenKeyEx(HKEY_CURRENT_USER, szRegPath.GetBuffer(), 0, KEY_READ, &hKey))
	{
		return;
	}

	//��ȡע����Ӽ���Ϣ���ж��Ƿ����Ӽ�
	TCHAR szName[MAX_PATH] = {};	//�Ӽ�����
	DWORD dwSize = 512;	//���ƴ�С
	DWORD dwIndex = 0;		
	while (ERROR_NO_MORE_ITEMS != ::RegEnumValue(hKey, dwIndex++, szName, &dwSize, NULL,
			NULL, NULL, 0))
	{		
		if (!wcscmp(szName, L"�������˺ڿ͹���"))
		{
			m_YesNoRun = TRUE;
			m_Check.SetCheck(1);
			break;
		}		
		m_Check.SetCheck(0);
		//�������ø�ֵ������ֻ�ܱ��������Σ�
		dwSize = 512;		
	}
	// �ر�ע����
	::RegCloseKey(hKey);
	
}


BOOL CNovelName::PreTranslateMessage(MSG* pMsg)
{
	// TODO: �ڴ����ר�ô����/����û���
	if (pMsg->message == WM_KEYDOWN && (pMsg->wParam == VK_ESCAPE))
	{
		return TRUE;
	}
	return CDialogEx::PreTranslateMessage(pMsg);
}
