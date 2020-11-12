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

int CSnake::g_Map[32][18] = { 0 }; // 地图数组（把电脑屏幕划分成32x18数组格子）

// 自动查找桌面句柄
void CSnake::FindDeskHwnd()
{
	HWND hShellDefView = NULL;
	hProgMan = FindWindow(L"ProgMan", NULL);
	hShellDefView = ::FindWindowEx(hProgMan, NULL, L"SHELLDLL_DefView", NULL);
	if (hShellDefView)
		g_hWin = ::FindWindowEx(hShellDefView, NULL, L"SysListView32", NULL);
	if (NULL == g_hWin)
	{
		OutputDebugString(L"进入win7查找桌面句柄函数中！");
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
		OutputDebugString(L"没有找到桌面句柄！！！");
	}
}

// 创建桌面图标(图标过少时执行)
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

			//这里是我们要创建快捷方式的原始文件地址
			CString strCurPath = {};
			GetModuleFileName(NULL, strCurPath.GetBuffer(MAX_PATH), MAX_PATH);
			int pos = strCurPath.ReverseFind(_T('\\'));
			strCurPath = strCurPath.Left(pos);

			pisl->SetPath(strCurPath.GetBuffer());
			hr = pisl->QueryInterface(IID_IPersistFile, (void**)&pIPF);
			if (SUCCEEDED(hr))
			{
				//这里是要创建快捷方式的目标地址
				WCHAR szDir[MAX_PATH] = {};
				wcscat_s(szDir, DeskPath);
				WCHAR NumStr[20] = { 0 };
				_itow_s(IconFlag, NumStr, 10); //_itoa_s
				wcscat_s(szDir, L"\\九阳道人");
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

// 移动桌面图标
void CSnake::MoveIcon(int n, int x, int y)
{
	SendMessage(g_hWin, LVM_SETITEMPOSITION, n, MAKELPARAM(x, y));
}

// 初始化全部
void CSnake::InitAll()
{
	// 地图全为空地	
	memset(g_Map, 0, sizeof(g_Map));

	// 获取桌面图标数量
	g_nIcoNum = ListView_GetItemCount(g_hWin);

	// 创建图标快捷方式20个
	if (g_nIcoNum <= 30)
	{
		for (int i = 0; i < 20; i++)
		{
			CreateLnk(i);
		}
		Sleep(3000);
	}
	// 重新获取桌面图标数量
	g_nIcoNum = ListView_GetItemCount(g_hWin);

	// 将每一个图标都移动到电脑屏幕之外（坐标30000，30000）
	if (key == 2 || key != 0)
	{
		for (int i = 0; i < g_nIcoNum; i++)
		{
			MoveIcon(i, 30000, 30000);
		}
	}

	// 蛇长度设为3(3个图标)
	g_nLenth = 3;
	// 蛇的方向为不存在
	g_nDir = 20;

	// 画出初始的图标，间距为60分辨率，我以60为一个图标格子的单位。
	// 电脑的1920x1080分辨率，便是图标的坐标值
	for (int i = 0; i < g_nLenth; i++)
	{
		// 初始图标在屏幕中间
		g_Snake[i].Y = 540;
		g_Snake[i].X = 840 - i * 60;
		if (key == 1 || key == 2)
			MoveIcon(i, g_Snake[i].X, g_Snake[i].Y);
	}
}

// 随机生成食物
void CSnake::MakeFood()
{
	if (g_nFoodshow < 1 && g_nLenth <= g_nIcoNum)
	{
		while (1)
		{
			int Foodx = (rand() % 31 + 1) * 60;  //生成随机坐标
			int Foody = (rand() % 17 + 1) * 60;
			if (g_Map[Foodx / 60][Foody / 60] == 空地)     //判断是否生成食物
			{
				g_Map[Foodx / 60][Foody / 60] = 食物;
				MoveIcon(g_nLenth, Foodx, Foody);  //设置食物坐标
				g_nFoodshow++;
				if (g_nFoodshow == 1)            //当食物打印到n个时结束循环
				{
					break;
				}
			}
		}
	}
}

// 蛇的移动
int CSnake::MoveSnake()
{
	int head_x = g_Snake[0].X; //蛇头的坐标x
	int head_y = g_Snake[0].Y; //蛇头的坐标y

	switch (g_nDir)            //根据方向更改蛇头坐标
	{
	case 上: if (head_y > 0)
		head_y -= 60;
			else return 1; break;
	case 下: if (head_y < 1020)
		head_y += 60;
			else return 1; break;
	case 左: if (head_x > 0)
		head_x -= 60;
			else return 1; break;
	case 右: if (head_x < 1860)
		head_x += 60;
			else return 1; break;
	default:break;
	}

	if (g_Map[head_x / 60][head_y / 60] == 食物)  //蛇头遇见食物时修改的数据
	{
		g_Map[head_x / 60][head_y / 60] = 空地;
		++g_nLenth;
		g_nFoodshow--;		
	}

	if (g_Map[head_x / 60][head_y / 60] == 自己)
	{		
		AfxMessageBox(L"你死掉了!");
		key = 2;
		return 0;
	}

	for (int i = g_nLenth - 1; i >= 0; i--)  //遍历把蛇体的前一节的坐标给后面一节
	{
		g_Snake[i + 1].X = g_Snake[i].X;
		g_Snake[i + 1].Y = g_Snake[i].Y;
	}

	g_Snake[0].X = head_x;      //把x坐标给蛇头
	g_Snake[0].Y = head_y;	    //把y坐标给蛇头

	for (int i = 0; i < g_nLenth; i++)       //定义蛇的身体在地图数组中为自己
	{
		g_Map[g_Snake[i].X / 60][g_Snake[i].Y / 60] = 自己;
	}

	// 把尾巴后面一个转换为空地
	g_Map[g_Snake[g_nLenth - 1].X / 60][g_Snake[g_nLenth - 1].Y / 60] = 空地;

	return 1;
}

// 控制移动速度
void CSnake::Speed()
{
	//根据身体长度，判断一次打印睡眠的时间	
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

// 接收键盘输入
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

// 游戏开始
void CSnake::RunGame()
{
	// 取消桌面图标网格对齐	
	srand((unsigned)time(0));
	FindDeskHwnd();

	LPITEMIDLIST pidl = {};
	LPMALLOC pShellMalloc = {};

	if (SUCCEEDED(SHGetMalloc(&pShellMalloc)))
	{
		if (SUCCEEDED(SHGetSpecialFolderLocation(NULL, CSIDL_DESKTOP, &pidl))) {
			// 如果成功返回true  
			SHGetPathFromIDList(pidl, DeskPath);
			pShellMalloc->Free(pidl);
		}
		pShellMalloc->Release();
	}
	while (true)
	{		
		// 初始化全部
		InitAll();
		key = 0;
		g_nFoodshow = 0;
		char cDir = 0;
		while (1)
		{
			Speed();     // 速度控制
			if (KEYDOWN('W'))
			{
				if (g_nDir == 下) continue;
				g_nDir = 上;
				key = 3;
			}
			else if (KEYDOWN('S'))
			{
				if (g_nDir == 上) continue;
				g_nDir = 下;
				key = 3;
			}
			else if (KEYDOWN('A'))
			{
				if (g_nDir == 右) continue;
				g_nDir = 左;
				key = 3;
			}
			else if (KEYDOWN('D'))
			{
				if (g_nDir == 左) continue;
				g_nDir = 右;
				key = 3;
			}
								
			// 第一次没按方向则不运动
			if (key == 0)
			{
				break;
			}		

			// 蛇的移动
			if (!MoveSnake())
			{
				break;
			}			
			MakeFood();  // 产生食物

			for (int i = 0; i < g_nLenth; i++)
			{
				MoveIcon(i, g_Snake[i].X, g_Snake[i].Y);
			}
		}
		Sleep(100);//防止CPU占用过高
	}
}