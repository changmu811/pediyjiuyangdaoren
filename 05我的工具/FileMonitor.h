#pragma once


// FileMonitor 对话框

class FileMonitor : public CDialogEx
{
	DECLARE_DYNAMIC(FileMonitor)

public:
	FileMonitor(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~FileMonitor();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_FileMonitor };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	
	virtual BOOL OnInitDialog();
	CListCtrl m_DataList;

	static UINT ThreadProc(LPVOID);
};
