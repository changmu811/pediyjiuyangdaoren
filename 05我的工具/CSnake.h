#pragma once
const int  ��  = 21;     // ����ĺ궨��
const int  ��  = 22;
const int  ��  = 23;
const int  ��  = 24;
const int �յ� =  0;     // ��ͼ�и��ֶ����ĺ궨��
const int ʳ�� =  7;
const int �Լ� =  8;

class CSnake
{
public:
	CSnake();
	~CSnake();
	// �Զ�����������
	void FindDeskHwnd();
	// ��������ͼ��(ͼ�����ʱִ��)
	void CreateLnk(int IconFlag);	
	// �ƶ�����ͼ��
	void MoveIcon(int n, int x, int y);
	// ��ʼ��ȫ��
	void InitAll();
	// �������ʳ��
	void MakeFood();	
	// �ߵ��ƶ�
	int MoveSnake();	
	// �����ƶ��ٶ�
	void Speed();

	void Keyboard();            //���ռ�������
	
	// ��Ϸ��ʼ
	void RunGame();

	static int g_Map[32][18]; // ��ͼ���飨�ѵ�����Ļ���ֳ�32x18������ӣ�
	COORD g_Snake[150]; // �ߵĽṹ������(�������100��ͼ��)
	int g_nLenth = 0;       // �ߵĳ���
	int g_nDir = 0;         // �ߵĳ�ʼ����
	int g_nIcoNum = 0;      // ����ͼ������

	int g_nFoodshow = 0; // ʳ�����
	int key = 1;         // �Ƿ������Ϸ

	int g_hpipe[2] = {};      // �ܵ�����

	HWND hProgMan = NULL; //explorer.exe���
	HWND g_hWin = NULL; //������
	WCHAR DeskPath[MAX_PATH]; //����·��

	
};

