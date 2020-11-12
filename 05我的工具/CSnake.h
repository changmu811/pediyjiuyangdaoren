#pragma once
const int  上  = 21;     // 方向的宏定义
const int  下  = 22;
const int  左  = 23;
const int  右  = 24;
const int 空地 =  0;     // 地图中各种东西的宏定义
const int 食物 =  7;
const int 自己 =  8;

class CSnake
{
public:
	CSnake();
	~CSnake();
	// 自动查找桌面句柄
	void FindDeskHwnd();
	// 创建桌面图标(图标过少时执行)
	void CreateLnk(int IconFlag);	
	// 移动桌面图标
	void MoveIcon(int n, int x, int y);
	// 初始化全部
	void InitAll();
	// 随机生成食物
	void MakeFood();	
	// 蛇的移动
	int MoveSnake();	
	// 控制移动速度
	void Speed();

	void Keyboard();            //接收键盘输入
	
	// 游戏开始
	void RunGame();

	static int g_Map[32][18]; // 地图数组（把电脑屏幕划分成32x18数组格子）
	COORD g_Snake[150]; // 蛇的结构体数组(最大容纳100个图标)
	int g_nLenth = 0;       // 蛇的长度
	int g_nDir = 0;         // 蛇的初始方向
	int g_nIcoNum = 0;      // 桌面图标数量

	int g_nFoodshow = 0; // 食物个数
	int key = 1;         // 是否进行游戏

	int g_hpipe[2] = {};      // 管道数据

	HWND hProgMan = NULL; //explorer.exe句柄
	HWND g_hWin = NULL; //桌面句柄
	WCHAR DeskPath[MAX_PATH]; //桌面路径

	
};

