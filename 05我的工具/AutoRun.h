#pragma once
#include "COptiClear.h"

// AutoRun 对话框

class AutoRun : public CDialogEx
{
	DECLARE_DYNAMIC(AutoRun)

public:
	AutoRun(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~AutoRun();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_AUTORUN };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()

public:	
	COptiClear* m_pFeather; // 父窗口指针
	CString serPath; // Service程序的路径

	CString m_ExePath; // 接受EXE路径的文本框变量
	afx_msg void OnDropFiles(HDROP hDropInfo);
	afx_msg void OnClose();
	afx_msg void OnOpenFileButton();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnRegAutoRun();
	CString m_Prompt;

	BOOL IsExeFile(); // 是否是EXE文件

	BOOL m_IsShutdown = FALSE; // 是否关机了
	void SaveAutoRunConfig();  // 保存自启动配置

	LPITEMIDLIST GetItemIDList(CString strPathName); // 获取一个指针

	afx_msg void OnStartupRun();
	afx_msg void OnTaskSchedule();
	afx_msg void OnOpenStartup();
	afx_msg void OnOpenRegedit();
	afx_msg void OnOpenMmc();
	afx_msg void OnSystemService();
	afx_msg void OnBnClickedButton2();
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
};
