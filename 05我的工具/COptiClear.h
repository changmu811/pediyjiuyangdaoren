#pragma once
#include <vector>
using namespace std;


// COptiClear 对话框

class COptiClear : public CDialogEx
{
	DECLARE_DYNAMIC(COptiClear)

public:
	COptiClear(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~COptiClear();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG3 };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()


	

public:
	BOOL m_IsOpenInject;  // 注入窗口是否打开
	BOOL m_IsOpenAutoRun; // 自启窗口是否打开
	BOOL m_IsOpenPrivileges; // 提权窗口是否打开

	void GetMem(); //获取内存信息
	CString m_Mem;
	CString m_Occupy;
	CString m_All;
	CProgressCtrl m_MemPro;

	virtual BOOL OnInitDialog();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
		
	afx_msg void OnBnClickedButton2();// 优化按钮						   	

	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnInjectButton();
	afx_msg void OnAutoRunButton();
	afx_msg void OnVSClearButton();
	afx_msg void OnAdjustPrivileges();
	afx_msg void OnBnClickedButton20();
	afx_msg void OnBnClickedButton21();
};
