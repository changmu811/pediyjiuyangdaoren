#pragma once
#include "afxwin.h"
#include "MakeName.h"

// CNovelName 对话框

class CNovelName : public CDialogEx
{
	DECLARE_DYNAMIC(CNovelName)

public:
	CNovelName(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CNovelName();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG1 };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButton1();
	CEdit m_objEdit;
	CString m_strEdit;
	CFont m_editFont; //光标变量
	CMakeName m_MakeName;

	BOOL m_YesNoRun = 0;
	virtual BOOL OnInitDialog();

	int m_nRadio;

	//遍历注册表
	void EnumRegTable();

	afx_msg void OnBnClickedButton4();
	afx_msg void OnBnClickedButton8();
	afx_msg void OnBnClickedButton5();
	afx_msg void OnBnClickedButton9();
	afx_msg void OnBnClickedButton3();
	afx_msg void OnBnClickedButton7();
	afx_msg void OnBnClickedButton6();
	afx_msg void OnBnClickedButton10();
	CButton m_Check;
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};
