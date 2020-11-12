#include "stdafx.h"
#include "CSnake.h"
#include <commctrl.h>

CSnake::CSnake()
{
}

CSnake::~CSnake()
{
}
#define KEYDOWN(key)  ((GetAsyncKeyState(key)& 0x8000)?1:0)
#define KEYUP(key)  ((GetAsyncKeyState(key)& 0x8000)?0:1)

int CSnake::g_Map[32][18] = { 0 }; // ��ͼ���飨�ѵ�����Ļ���ֳ�32x18������ӣ�

// �Զ�����������
void CSnake::FindDeskHwnd()
{
	HWND hShellDefView = NULL;
	hProgMan = FindWindow(L"ProgMan", NULL);
	hShellDefView = ::FindWindowEx(hProgMan, NULL, L"SHELLDLL_DefView", NULL);
	if (hShellDefView)
		g_hWin = ::FindWindowEx(hShellDefView, NULL, L"SysListView32", NULL);
	if (NULL == g_hWin)
	{
		OutputDebugString(L"����win7���������������У�");
		hProgMan = ::FindWindowEx(NULL, NULL, L"WorkerW", L"");
		while ((!hShellDefView) && hProgMan)
		{
			hShellDefView = ::FindWindowEx(hProgMan, NULL, L"SHELLDLL_DefView", NULL);
			hProgMan = FindWindowEx(NULL, hProgMan, L"WorkerW", L"");
		}
		g_hWin = ::FindWindowEx(hShellDefView, 0, L"SysListView32", L"FolderView");
	}
	if (NULL == g_hWin)
	{
		OutputDebugString(L"û���ҵ�������������");
	}
}

// ��������ͼ��(ͼ�����ʱִ��)
void CSnake::CreateLnk(int IconFlag)
{
	HRESULT hr = CoInitialize(NULL);
	if (SUCCEEDED(hr))
	{
		IShellLink *pisl;
		hr = CoCreateInstance(CLSID_ShellLink, NULL,
			CLSCTX_INPROC_SERVER, IID_IShellLink, (void**)&pisl);
		if (SUCCEEDED(hr))
		{
			IPersistFile* pIPF = NULL;

			//����������Ҫ������ݷ�ʽ��ԭʼ�ļ���ַ
			CString strCurPath = {};
			GetModuleFileName(NULL, strCurPath.GetBuffer(MAX_PATH), MAX_PATH);
			int pos = strCurPath.ReverseFind(_T('\\'));
			strCurPath = strCurPath.Left(pos);

			pisl->SetPath(strCurPath.GetBuffer());
			hr = pisl->QueryInterface(IID_IPersistFile, (void**)&pIPF);
			if (SUCCEEDED(hr))
			{
				//������Ҫ������ݷ�ʽ��Ŀ���ַ
				WCHAR szDir[MAX_PATH] = {};
				wcscat_s(szDir, DeskPath);
				WCHAR NumStr[20] = { 0 };
				_itow_s(IconFlag, NumStr, 10); //_itoa_s
				wcscat_s(szDir, L"\\��������");
				wcscat_s(szDir, NumStr);
				wcscat_s(szDir, L".lnk");

				pIPF->Save(szDir, FALSE);
				pIPF->Release();
			}
			pisl->Release();
		}
		CoUninitialize();
	}
}

// �ƶ�����ͼ��
void CSnake::MoveIcon(int n, int x, int y)
{
	SendMessage(g_hWin, LVM_SETITEMPOSITION, n, MAKELPARAM(x, y));
}

// ��ʼ��ȫ��
void CSnake::InitAll()
{
	// ��ͼȫΪ�յ�	
	memset(g_Map, 0, sizeof(g_Map));

	// ��ȡ����ͼ������
	g_nIcoNum = ListView_GetItemCount(g_hWin);

	// ����ͼ���ݷ�ʽ20��
	if (g_nIcoNum <= 30)
	{
		for (int i = 0; i < 20; i++)
		{
			CreateLnk(i);
		}
		Sleep(3000);
	}
	// ���»�ȡ����ͼ������
	g_nIcoNum = ListView_GetItemCount(g_hWin);

	// ��ÿһ��ͼ�궼�ƶ���������Ļ֮�⣨����30000��30000��
	if (key == 2 || key != 0)
	{
		for (int i = 0; i < g_nIcoNum; i++)
		{
			MoveIcon(i, 30000, 30000);
		}
	}

	// �߳�����Ϊ3(3��ͼ��)
	g_nLenth = 3;
	// �ߵķ���Ϊ������
	g_nDir = 20;

	// ������ʼ��ͼ�꣬���Ϊ60�ֱ��ʣ�����60Ϊһ��ͼ����ӵĵ�λ��
	// ���Ե�1920x1080�ֱ��ʣ�����ͼ�������ֵ
	for (int i = 0; i < g_nLenth; i++)
	{
		// ��ʼͼ������Ļ�м�
		g_Snake[i].Y = 540;
		g_Snake[i].X = 840 - i * 60;
		if (key == 1 || key == 2)
			MoveIcon(i, g_Snake[i].X, g_Snake[i].Y);
	}
}

// �������ʳ��
void CSnake::MakeFood()
{
	if (g_nFoodshow < 1 && g_nLenth <= g_nIcoNum)
	{
		while (1)
		{
			int Foodx = (rand() % 31 + 1) * 60;  //�����������
			int Foody = (rand() % 17 + 1) * 60;
			if (g_Map[Foodx / 60][Foody / 60] == �յ�)     //�ж��Ƿ�����ʳ��
			{
				g_Map[Foodx / 60][Foody / 60] = ʳ��;
				MoveIcon(g_nLenth, Foodx, Foody);  //����ʳ������
				g_nFoodshow++;
				if (g_nFoodshow == 1)            //��ʳ���ӡ��n��ʱ����ѭ��
				{
					break;
				}
			}
		}
	}
}

// �ߵ��ƶ�
int CSnake::MoveSnake()
{
	int head_x = g_Snake[0].X; //��ͷ������x
	int head_y = g_Snake[0].Y; //��ͷ������y

	switch (g_nDir)            //���ݷ��������ͷ����
	{
	case ��: if (head_y > 0)
		head_y -= 60;
			else return 1; break;
	case ��: if (head_y < 1020)
		head_y += 60;
			else return 1; break;
	case ��: if (head_x > 0)
		head_x -= 60;
			else return 1; break;
	case ��: if (head_x < 1860)
		head_x += 60;
			else return 1; break;
	default:break;
	}

	if (g_Map[head_x / 60][head_y / 60] == ʳ��)  //��ͷ����ʳ��ʱ�޸ĵ�����
	{
		g_Map[head_x / 60][head_y / 60] = �յ�;
		++g_nLenth;
		g_nFoodshow--;		
	}

	if (g_Map[head_x / 60][head_y / 60] == �Լ�)
	{		
		AfxMessageBox(L"��������!");
		key = 2;
		return 0;
	}

	for (int i = g_nLenth - 1; i >= 0; i--)  //�����������ǰһ�ڵ����������һ��
	{
		g_Snake[i + 1].X = g_Snake[i].X;
		g_Snake[i + 1].Y = g_Snake[i].Y;
	}

	g_Snake[0].X = head_x;      //��x�������ͷ
	g_Snake[0].Y = head_y;	    //��y�������ͷ

	for (int i = 0; i < g_nLenth; i++)       //�����ߵ������ڵ�ͼ������Ϊ�Լ�
	{
		g_Map[g_Snake[i].X / 60][g_Snake[i].Y / 60] = �Լ�;
	}

	// ��β�ͺ���һ��ת��Ϊ�յ�
	g_Map[g_Snake[g_nLenth - 1].X / 60][g_Snake[g_nLenth - 1].Y / 60] = �յ�;

	return 1;
}

// �����ƶ��ٶ�
void CSnake::Speed()
{
	//�������峤�ȣ��ж�һ�δ�ӡ˯�ߵ�ʱ��	
	if (g_nLenth <= 5)
	{
		Sleep(150);
	}
	else if (g_nLenth > 5 && g_nLenth <= 10)
	{
		Sleep(100);
	}
	else if (g_nLenth > 10 && g_nLenth <= 20)
	{
		Sleep(70);
	}
	else if (g_nLenth > 20 && g_nLenth <= 30)
	{
		Sleep(50);
	}
	else if (g_nLenth > 30 && g_nLenth <= 40)
	{
		Sleep(30);
	}
	else
	{
		Sleep(10);
	}
}

// ���ռ�������
void CSnake::Keyboard()
{
	if (KEYDOWN('W'))
	{
		
	}
	else if (KEYDOWN('S'))
	{
		
	}
	else if (KEYDOWN('A'))
	{
		
	}
	else if (KEYDOWN('D'))
	{
		
	}
}

// ��Ϸ��ʼ
void CSnake::RunGame()
{
	// ȡ������ͼ���������	
	srand((unsigned)time(0));
	FindDeskHwnd();

	LPITEMIDLIST pidl = {};
	LPMALLOC pShellMalloc = {};

	if (SUCCEEDED(SHGetMalloc(&pShellMalloc)))
	{
		if (SUCCEEDED(SHGetSpecialFolderLocation(NULL, CSIDL_DESKTOP, &pidl))) {
			// ����ɹ�����true  
			SHGetPathFromIDList(pidl, DeskPath);
			pShellMalloc->Free(pidl);
		}
		pShellMalloc->Release();
	}
	while (true)
	{		
		// ��ʼ��ȫ��
		InitAll();
		key = 0;
		g_nFoodshow = 0;
		char cDir = 0;
		while (1)
		{
			Speed();     // �ٶȿ���
			if (KEYDOWN('W'))
			{
				if (g_nDir == ��) continue;
				g_nDir = ��;
				key = 3;
			}
			else if (KEYDOWN('S'))
			{
				if (g_nDir == ��) continue;
				g_nDir = ��;
				key = 3;
			}
			else if (KEYDOWN('A'))
			{
				if (g_nDir == ��) continue;
				g_nDir = ��;
				key = 3;
			}
			else if (KEYDOWN('D'))
			{
				if (g_nDir == ��) continue;
				g_nDir = ��;
				key = 3;
			}
								
			// ��һ��û���������˶�
			if (key == 0)
			{
				break;
			}		

			// �ߵ��ƶ�
			if (!MoveSnake())
			{
				break;
			}			
			MakeFood();  // ����ʳ��

			for (int i = 0; i < g_nLenth; i++)
			{
				MoveIcon(i, g_Snake[i].X, g_Snake[i].Y);
			}
		}
		Sleep(100);//��ֹCPUռ�ù���
	}
}