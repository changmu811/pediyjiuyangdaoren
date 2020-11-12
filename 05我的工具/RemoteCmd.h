#pragma once
#include "MySocket.h"

// RemoteCmd 对话框

class RemoteCmd : public CDialogEx
{
	DECLARE_DYNAMIC(RemoteCmd)

public:
	RemoteCmd(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~RemoteCmd();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_RemoteCMD };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CString m_IP;
	CString m_CmdCode;
	CString m_Text;
	CStringA m_Temp;	
	MySocket m_Socket;
	afx_msg void OnBnClickedButton11();
	afx_msg void OnBnClickedButton1();

	void ShowData(BYTE szBuf[], int nSize); // 处理接受数据

	CString m_Edit;

	afx_msg void OnTimer(UINT_PTR nIDEvent);
	virtual BOOL OnInitDialog();
};
