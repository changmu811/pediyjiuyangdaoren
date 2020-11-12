#pragma once


// OtherFunction 对话框

class OtherFunction : public CDialogEx
{
	DECLARE_DYNAMIC(OtherFunction)

public:
	OtherFunction(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~OtherFunction();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG5 };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CString m_FilePath;
	afx_msg void OnBnClickedButton25();
	afx_msg void OnDropFiles(HDROP hDropInfo);
	afx_msg void OnAPIcompress();
	afx_msg void OnAPIdecompress();
	afx_msg void OnCompress();
	afx_msg void OnDeompress();
	CString m_Remind;
	afx_msg void OnDesktopScreenShot();

protected:
	void PaintMouse(HDC hDc);

public:
	afx_msg void OnKeyRecordDlg();
	afx_msg void OnUSBcontrol();
	virtual LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	afx_msg void OnFileMonitor();
	afx_msg void OnSuicide();
	afx_msg void OnRemoteCMD();
};
