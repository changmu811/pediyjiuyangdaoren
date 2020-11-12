// KeyCord.cpp: 实现文件
//

#include "stdafx.h"
#include "KeyCord.h"
#include "afxdialogex.h"
#include "05我的工具.h"


// 虚拟键值与其对应的注释
VKeyInfo KeyCord::keyAddr[] = {
		group(VK_LBUTTON, L"Left mouse button"),
		group(VK_RBUTTON, L"Right mouse button"),
		group(VK_CANCEL, L"Control-break processing"),
		group(0x04, L"Middle mouse button (three-button mouse)"),
		group(0x05, L"Windows 2000/XP: X1 mouse button"),
		group(0x06, L"Windows 2000/XP: X2 mouse button"),
		group(0x07, L"Undefined"),
		group(VK_BACK, L"BACKSPACE key"),
		group(VK_TAB, L"TAB key"),
		group(0x0A, L"Reserved"),
		group(0x0B, L"Reserved"),
		group(VK_CLEAR, L"CLEAR key"),
		group(VK_RETURN, L"ENTER key"),
		group(0x0E, L"Undefined"),
		group(0x0F, L"Undefined"),
		group(VK_SHIFT, L"SHIFT key"),
		group(VK_CONTROL, L"CTRL key"),
		group(VK_MENU, L"ALT key"),
		group(VK_PAUSE, L"PAUSE key"),
		group(VK_CAPITAL, L"CAPS LOCK key"),
		group(VK_KANA, L"Input Method Editor (IME) Kana mode"),
		group(VK_HANGUL, L"IME Hangul mode"),
		group(0x16, L"Undefined"),
		group(VK_JUNJA, L"IME Junja mode"),
		group(VK_FINAL, L"IME final mode"),
		group(VK_HANJA, L"IME Hanja mode"),
		group(VK_KANJI, L"IME Kanji mode"),
		group(0x1A, L"Undefined"),
		group(VK_ESCAPE, L"ESC key"),
		group(VK_CONVERT, L"IME convert"),
		group(VK_NONCONVERT, L"IME nonconvert"),
		group(VK_ACCEPT, L"IME accept"),
		group(VK_MODECHANGE, L"IME mode change request"),
		group(VK_SPACE, L"SPACEBAR"),
		group(VK_PRIOR, L"PAGE UP key"),
		group(VK_NEXT, L"PAGE DOWN key"),
		group(VK_END, L"END key"),
		group(VK_HOME, L"HOME key"),
		group(VK_LEFT, L"LEFT ARROW key"),
		group(VK_UP, L"UP ARROW key"),
		group(VK_RIGHT, L"RIGHT ARROW key"),
		group(VK_DOWN, L"DOWN ARROW key"),
		group(VK_SELECT, L"SELECT key"),
		group(VK_PRINT, L"PRINT key"),
		group(VK_EXECUTE, L"EXECUTE key"),
		group(VK_SNAPSHOT, L"PRINT SCREEN key"),
		group(VK_INSERT, L"INSERT key"),
		group(VK_DELETE, L"DEL key"),
		group(VK_HELP, L"HELP key"),
		group(0x30, L"0"),
		group(0x31, L"1"),
		group(0x32, L"2"),
		group(0x33, L"3"),
		group(0x34, L"4"),
		group(0x35, L"5"),
		group(0x36, L"6"),
		group(0x37, L"7"),
		group(0x38, L"8"),
		group(0x39, L"9"),
		group(0x3A, L"Undefined"),
		group(0x3B, L"Undefined"),
		group(0x3C, L"Undefined"),
		group(0x3D, L"Undefined"),
		group(0x3E, L"Undefined"),
		group(0x3F, L"Undefined"),
		group(0x40, L"Undefined"),
		group(0x41, L"A"),
		group(0x42, L"B"),
		group(0x43, L"C"),
		group(0x44, L"D"),
		group(0x45, L"E"),
		group(0x46, L"F"),
		group(0x47, L"G"),
		group(0x48, L"H"),
		group(0x49, L"I"),
		group(0x4A, L"J"),
		group(0x4B, L"K"),
		group(0x4C, L"L"),
		group(0x4D, L"M"),
		group(0x4E, L"N"),
		group(0x4F, L"O"),
		group(0x50, L"P"),
		group(0x51, L"Q"),
		group(0x52, L"R"),
		group(0x53, L"S"),
		group(0x54, L"T"),
		group(0x55, L"U"),
		group(0x56, L"V"),
		group(0x57, L"W"),
		group(0x58, L"X"),
		group(0x59, L"Y"),
		group(0x5A, L"Z"),
		group(VK_LWIN, L"Left Windows key (Microsoft Natural keyboard)"),
		group(VK_RWIN, L"Right Windows key (Natural keyboard)"),
		group(VK_APPS, L"Applications key (Natural keyboard)"),
		group(0x5E, L"Reserved"),
		group(VK_SLEEP, L"Computer Sleep key"),
		group(VK_NUMPAD0, L"Numeric keypad 0 key"),
		group(VK_NUMPAD1, L"Numeric keypad 1 key"),
		group(VK_NUMPAD2, L"Numeric keypad 2 key"),
		group(VK_NUMPAD3, L"Numeric keypad 3 key"),
		group(VK_NUMPAD4, L"Numeric keypad 4 key"),
		group(VK_NUMPAD5, L"Numeric keypad 5 key"),
		group(VK_NUMPAD6, L"Numeric keypad 6 key"),
		group(VK_NUMPAD7, L"Numeric keypad 7 key"),
		group(VK_NUMPAD8, L"Numeric keypad 8 key"),
		group(VK_NUMPAD9, L"Numeric keypad 9 key"),
		group(VK_MULTIPLY, L"Multiply key"),
		group(VK_ADD, L"Add key"),
		group(VK_SEPARATOR, L"Separator key"),
		group(VK_SUBTRACT, L"Subtract key"),
		group(VK_DECIMAL, L"Decimal key"),
		group(VK_DIVIDE, L"Divide key"),
		group(VK_F1, L"F1 key"),
		group(VK_F2, L"F2 key"),
		group(VK_F3, L"F3 key"),
		group(VK_F4, L"F4 key"),
		group(VK_F5, L"F5 key"),
		group(VK_F6, L"F6 key"),
		group(VK_F7, L"F7 key"),
		group(VK_F8, L"F8 key"),
		group(VK_F9, L"F9 key"),
		group(VK_F10, L"F10 key"),
		group(VK_F11, L"F11 key"),
		group(VK_F12, L"F12 key"),
		group(VK_F13, L"F13 key"),
		group(VK_F14, L"F14 key"),
		group(VK_F15, L"F15 key"),
		group(VK_F16, L"F16 key"),
		group(VK_F17, L"F17 key"),
		group(VK_F18, L"F18 key"),
		group(VK_F19, L"F19 key"),
		group(VK_F20, L"F20 key"),
		group(VK_F21, L"F21 key"),
		group(VK_F22, L"F22 key"),
		group(VK_F23, L"F23 key"),
		group(VK_F24, L"F24 key"),
		group(0x88, L"Unassigned"),
		group(0x89, L"Unassigned"),
		group(0x8A, L"Unassigned"),
		group(0x8B, L"Unassigned"),
		group(0x8C, L"Unassigned"),
		group(0x8D, L"Unassigned"),
		group(0x8E, L"Unassigned"),
		group(0x8F, L"Unassigned"),
		group(VK_NUMLOCK, L"NUM LOCK key"),
		group(VK_SCROLL, L"SCROLL LOCK key"),
		group(0x92, L"OEM specific"),
		group(0x93, L"OEM specific"),
		group(0x94, L"OEM specific"),
		group(0x95, L"OEM specific"),
		group(0x96, L"OEM specific"),
		group(0x97, L"Unassigned"),
		group(0x98, L"Unassigned"),
		group(0x99, L"Unassigned"),
		group(0x9A, L"Unassigned"),
		group(0x9B, L"Unassigned"),
		group(0x9C, L"Unassigned"),
		group(0x9D, L"Unassigned"),
		group(0x9E, L"Unassigned"),
		group(0x9F, L"Unassigned"),
		group(VK_LSHIFT, L"Left SHIFT key"),
		group(VK_RSHIFT, L"Right SHIFT key"),
		group(VK_LCONTROL, L"Left CONTROL key"),
		group(VK_RCONTROL, L"Right CONTROL key"),
		group(VK_LMENU, L"Left MENU key"),
		group(VK_RMENU, L"Right MENU key"),
		group(0xA6, L"Windows 2000/XP: Browser Back key"),
		group(0xA7, L"Windows 2000/XP: Browser Forward key"),
		group(0xA8, L"Windows 2000/XP: Browser Refresh key"),
		group(0xA9, L"Windows 2000/XP: Browser Stop key"),
		group(0xAA, L"Windows 2000/XP: Browser Search key"),
		group(0xAB, L"Windows 2000/XP: Browser Favorites key"),
		group(0xAC, L"Windows 2000/XP: Browser Start and Home key"),
		group(0xAD, L"Windows 2000/XP: Volume Mute key"),
		group(0xAE, L"Windows 2000/XP: Volume Down key"),
		group(0xAF, L"Windows 2000/XP: Volume Up key"),
		group(0xB0, L"Windows 2000/XP: Next Track key"),
		group(0xB1, L"Windows 2000/XP: Previous Track key"),
		group(0xB2, L"Windows 2000/XP: Stop Media key"),
		group(0xB3, L"Windows 2000/XP: Play/Pause Media key"),
		group(0xB4, L"Windows 2000/XP: Start Mail key"),
		group(0xB5, L"Windows 2000/XP: Select Media key"),
		group(0xB6, L"Windows 2000/XP: Start Application 1 key"),
		group(0xB7, L"Windows 2000/XP: Start Application 2 key"),
		group(0xB8, L"Reserved"),
		group(0xB9, L"Reserved"),
		group(VK_OEM_1, L"Used for miscellaneous characters; it can vary by keyboard."
						  "Windows 2000/XP: For the US standard keyboard, the \';:\' key"),
		group(VK_OEM_PLUS, L"Windows 2000/XP: For any country/region, the \'+\' key"),
		group(VK_OEM_COMMA, L"Windows 2000/XP: For any country/region, the \',\' key"),
		group(VK_OEM_MINUS, L"Windows 2000/XP: For any country/region, the \'-\' key"),
		group(VK_OEM_PERIOD, L"Windows 2000/XP: For any country/region, the \'.\' key"),
		group(VK_OEM_2, L"Used for miscellaneous characters; it can vary by keyboard."
						  "Windows 2000/XP: For the US standard keyboard, the \'/?\' key"),
		group(VK_OEM_3, L"Used for miscellaneous characters; it can vary by keyboard."
						  "Windows 2000/XP: For the US standard keyboard, the \'`~\' key"),
		group(0xC1, L"Reserved"),
		group(0xC2, L"Reserved"),
		group(0xC3, L"Reserved"),
		group(0xC4, L"Reserved"),
		group(0xC5, L"Reserved"),
		group(0xC6, L"Reserved"),
		group(0xC7, L"Reserved"),
		group(0xC8, L"Reserved"),
		group(0xC9, L"Reserved"),
		group(0xCA, L"Reserved"),
		group(0xCB, L"Reserved"),
		group(0xCC, L"Reserved"),
		group(0xCD, L"Reserved"),
		group(0xCE, L"Reserved"),
		group(0xCF, L"Reserved"),
		group(0xD0, L"Reserved"),
		group(0xD1, L"Reserved"),
		group(0xD2, L"Reserved"),
		group(0xD3, L"Reserved"),
		group(0xD4, L"Reserved"),
		group(0xD5, L"Reserved"),
		group(0xD6, L"Reserved"),
		group(0xD7, L"Reserved"),
		group(0xD8, L"Unassigned"),
		group(0xD9, L"Unassigned"),
		group(0xDA, L"Unassigned"),
		group(VK_OEM_4, L"Used for miscellaneous characters; it can vary by keyboard."
						  "Windows 2000/XP: For the US standard keyboard, the \'[{\' key"),
		group(VK_OEM_5, L"Used for miscellaneous characters; it can vary by keyboard."
						  "Windows 2000/XP: For the US standard keyboard, the \'\\|\' key"),
		group(VK_OEM_6, L"Used for miscellaneous characters; it can vary by keyboard."
						  "Windows 2000/XP: For the US standard keyboard, the \']}\' key"),
		group(VK_OEM_7, L"Used for miscellaneous characters; it can vary by keyboard."
						  "Windows 2000/XP: For the US standard keyboard, the \'single-quote/double-quote\' key"),
		group(VK_OEM_8, L"Used for miscellaneous characters; it can vary by keyboard."),
		group(0xE0, L"Reserved"),
		group(0xE1, L"OEM specific"),
		group(VK_OEM_102, L"Windows 2000/XP: Either the angle bracket key or the backslash key on the RT 102-key keyboard"),
		group(0xE3, L"OEM specific"),
		group(0xE4, L"OEM specific"),
		group(VK_PROCESSKEY, L"Windows 95/98/Me, Windows NT 4.0, Windows 2000/XP: IME PROCESS key"),
		group(0xE6, L"OEM specific"),
		group(0xE7, L"Windows 2000/XP: Used to pass Unicode characters as if they were keystrokes. The VK_PACKET key is the low word of a 32-bit Virtual Key value used for non-keyboard input methods. For more information, see Remark in KEYBDINPUT, SendInput, WM_KEYDOWN, and WM_KEYUP"),
		group(0xE8, L"Unassigned"),
		group(0xE9, L"OEM specific"),
		group(0xEA, L"OEM specific"),
		group(0xEB, L"OEM specific"),
		group(0xEC, L"OEM specific"),
		group(0xED, L"OEM specific"),
		group(0xEF, L"OEM specific"),
		group(0xF0, L"OEM specific"),
		group(0xF1, L"OEM specific"),
		group(0xF2, L"OEM specific"),
		group(0xF3, L"OEM specific"),
		group(0xF4, L"OEM specific"),
		group(0xF5, L"OEM specific"),
		group(VK_ATTN, L"Attn key"),
		group(VK_CRSEL, L"CrSel key"),
		group(VK_EXSEL, L"ExSel key"),
		group(VK_EREOF, L"Erase EOF key"),
		group(VK_PLAY, L"Play key"),
		group(VK_ZOOM, L"Zoom key"),
		group(VK_NONAME, L"Reserved"),
		group(VK_PA1, L"PA1 key"),
		group(VK_OEM_CLEAR, L"Clear key"),
		group(0xFF, L"Unknown Virtual-Key Code")
};

// KeyCord 对话框

IMPLEMENT_DYNAMIC(KeyCord, CDialogEx)

KeyCord::KeyCord(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_KEYRECORD, pParent)
{
	   	 
}

KeyCord::~KeyCord()
{
}

void KeyCord::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_DataList);
}


BEGIN_MESSAGE_MAP(KeyCord, CDialogEx)
END_MESSAGE_MAP()


// KeyCord 消息处理程序


BOOL KeyCord::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 在此添加专用代码和/或调用基类
	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_ESCAPE)
		return TRUE;
	
	return CDialogEx::PreTranslateMessage(pMsg);
}

// 获取键名
LPWSTR KeyCord::GetkeyName(USHORT key)
{
	size_t i = 0;
	for (i = 0; i < sizeof(keyAddr); i++)
	{
		if (key == keyAddr[i].uKey)
			return keyAddr[i].keyName;
	}
	return L"Not find keyName!";
}

// 注册原始输入设备
void KeyCord::RegDevice()
{
	// 设置结构体信息
	RAWINPUTDEVICE rawDevice = {};
	rawDevice.usUsagePage = 0x01;
	rawDevice.usUsage = 0x06;
	rawDevice.dwFlags = RIDEV_INPUTSINK;
	rawDevice.hwndTarget = this->m_hWnd;

	// 注册原始输入设备
	BOOL bRet = ::RegisterRawInputDevices(&rawDevice, 1, sizeof(rawDevice));
	if (!bRet)
	{
		AfxMessageBox(L"RegisterRawInputDevices");
	}
}

// 获得原始输入数据
void KeyCord::GetData(LPARAM lParam)
{
	RAWINPUT rawData = {};
	UINT uSize = sizeof(rawData);

	// 获取输入数据的大小
	::GetRawInputData((HRAWINPUT)lParam, RID_INPUT, &rawData, &uSize, sizeof(RAWINPUTHEADER));
	if (RIM_TYPEKEYBOARD == rawData.header.dwType)
	{
		if ((WM_KEYDOWN == rawData.data.keyboard.Message) ||
			(WM_SYSKEYDOWN == rawData.data.keyboard.Message))
		{
			static DWORD num = 0;
			// 记录按键信息
			SaveInfo(rawData.data.keyboard.VKey, num);
			++num;
		}
	}
}

// 记录按键信息
void KeyCord::SaveInfo(USHORT key,DWORD num)
{
	TCHAR szKey[MAX_PATH] = {};
	TCHAR szTitle[MAX_PATH] = {};	

	// 获取顶层窗口
	HWND hWnd = ::GetForegroundWindow();
	// 获取顶层窗口的标题
	::GetWindowText(hWnd, szTitle, MAX_PATH);
	// 将虚拟键码转换为对应的名称
	wcscpy_s(szKey, MAX_PATH, GetkeyName(key));

	CString strTemp = L"[";
	strTemp += szTitle;
	strTemp += L"]  ";
	strTemp += szKey;

	// 将数据插入列表控件
	m_DataList.InsertItem(num, strTemp);
	DWORD nCount = m_DataList.GetItemCount();
	if (nCount > 0)
		m_DataList.EnsureVisible(nCount - 1, FALSE);
}


BOOL KeyCord::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化	
	m_DataList.InsertColumn(0, L"窗口名、按键 如下：", 0, 500);

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}


LRESULT KeyCord::DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	// TODO: 在此添加专用代码和/或调用基类
	if (message == WM_INITDIALOG)
		RegDevice();
	else if (message == WM_INPUT)
	{
		GetData(lParam);
	}

	if(WM_DEVICECHANGE)

	return CDialogEx::DefWindowProc(message, wParam, lParam);
}
