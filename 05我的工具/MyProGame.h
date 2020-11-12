#pragma once


// CMyProGame 对话框

class CMyProGame : public CDialogEx
{
	DECLARE_DYNAMIC(CMyProGame)

public:
	CMyProGame(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CMyProGame();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG2 };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedButton11();
	CButton m_Butt1;
	BOOL m_IsGame = TRUE;
	HANDLE m_ThreadHandle;
	unsigned threadID;
	afx_msg void OnBnClickedButton2();
	afx_msg void OnBnClickedButton12();
	afx_msg void OnBnClickedButton14();
	afx_msg void OnBnClickedButton13();
	afx_msg void OnBnClickedButton15();
	afx_msg void OnBnClickedButton16();
};
