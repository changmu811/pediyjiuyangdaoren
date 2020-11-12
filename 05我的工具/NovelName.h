#pragma once
#include "afxwin.h"
#include "MakeName.h"

// CNovelName �Ի���

class CNovelName : public CDialogEx
{
	DECLARE_DYNAMIC(CNovelName)

public:
	CNovelName(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CNovelName();

// �Ի�������
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG1 };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButton1();
	CEdit m_objEdit;
	CString m_strEdit;
	CFont m_editFont; //������
	CMakeName m_MakeName;

	BOOL m_YesNoRun = 0;
	virtual BOOL OnInitDialog();

	int m_nRadio;

	//����ע���
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
