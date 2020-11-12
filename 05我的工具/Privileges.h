#pragma once
#include "COptiClear.h"

// Privileges 对话框

class Privileges : public CDialogEx
{
	DECLARE_DYNAMIC(Privileges)

public:
	Privileges(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~Privileges();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_PRIVILEGES };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()

	BOOL IsExeFile(); // 是否是EXE文件


public:
	COptiClear* m_pFeather;
	afx_msg void OnClose();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	CString m_DirExe;
	afx_msg void OnBypassUAC();
	afx_msg void OnDropFiles(HDROP hDropInfo);
	afx_msg void OnBnClickedButton11();
	afx_msg void OnCOM();
	afx_msg void OnShell();
};
