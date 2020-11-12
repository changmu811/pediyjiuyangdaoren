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
	afx_msg void OnBnClickedOk();

	void GetMem(); //获取内存信息

	CString m_Mem;
	CString m_Occupy;
	CString m_All;
	virtual BOOL OnInitDialog();
	CProgressCtrl m_MemPro;
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnBnClickedOk2();

	CString m_ClearSize; //需删除的大小
	CString m_AllSize; //总文件的大小
	CString m_flagName;//显示总大小或者已删除大小
	   
	CListCtrl m_FileList;//拖拽的文件列表
	CListCtrl m_ShowDeletList;//显示删除列表
	CString m_FileProName; //文件扩展名

	vector<CString> PathString;//保存路径的容器
	vector<CString> DelString; //删除文件的容器
	LARGE_INTEGER CountSize;   //文件总大小
	LARGE_INTEGER DestSize;    //可删除文件的大小	
	INT FileCount;	 //总文件数量
	INT DestCount;	 //可删文件数量
	INT DeleteCount; //已删除的文件个数

	void FindFile(CString Path); //遍历文件信息
	   
	afx_msg void OnDropFiles(HDROP hDropInfo);
	CString m_;
	afx_msg void OnBnClickedButton2();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};
