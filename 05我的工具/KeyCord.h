#pragma once


struct VKeyInfo
{
	USHORT uKey;
	LPWSTR keyName;
};

#define group(key, keyName)  {(key), (keyName)}

// KeyCord 对话框

class KeyCord : public CDialogEx
{
	DECLARE_DYNAMIC(KeyCord)

public:
	KeyCord(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~KeyCord();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_KEYRECORD };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CListCtrl m_DataList;
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	
	static VKeyInfo keyAddr[];            // 按键数据数组
	void RegDevice();                     // 注册原始输入设备
	void GetData(LPARAM lParam);          // 获得原始输入数据
	void SaveInfo(USHORT key, DWORD num); // 记录按键信息
	LPWSTR GetkeyName(USHORT key);        // 获取键名
	
	
	virtual BOOL OnInitDialog();
	virtual LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam);
};
