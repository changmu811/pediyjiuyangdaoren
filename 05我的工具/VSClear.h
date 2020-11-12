#pragma once
#include <vector>
using namespace std;
// VSClear 对话框

class VSClear : public CDialogEx
{
	DECLARE_DYNAMIC(VSClear)

public:
	VSClear(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~VSClear();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG4 };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CListCtrl m_FileList;  // 拖拽文件列表
	CListCtrl m_ClearList; // 清理文件列表
	CString m_FileFormat;  // 要删除的文件格式
	CString m_HintA;	   // 提示1
	CString m_HintB;	   // 提示2

	vector<CString> vecPath;   	// 保存路径的容器
	vector<CString> vecDelete;	// 要删除文件的容器
	LARGE_INTEGER allSize;		// 总文件大小
	LARGE_INTEGER delSize;		// 要删除文件的大小	
	DWORD dwEnableCount;        // 可删除文件个数
	DWORD dwDeleteCount;        // 已删除文件个数

	void EnumFileInfo(CString Path); // 遍历文件信息

	afx_msg void OnGetInfo();
	afx_msg void OnBeginDelete();
	virtual BOOL OnInitDialog();
	afx_msg void OnDropFiles(HDROP hDropInfo);
	afx_msg void OnInitList();
};
