#pragma once
#include "COptiClear.h"
#include <vector>
using namespace std;

// InjectDll 对话框

class InjectDll : public CDialogEx
{
	DECLARE_DYNAMIC(InjectDll)

public:
	InjectDll(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~InjectDll();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIGINJECT };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()

	// 遍历进程
	void FindAllProcess();
	void ShowModuleList(DWORD dwPid);

	BOOL Is64or32Process(HANDLE hProcess); //判断是否是64位程序

public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);

	afx_msg void OnClose();
	virtual BOOL OnInitDialog();
	afx_msg void OnRefreshProcess();

	CString m_DirDll; // 文本框字符变量
	CListCtrl m_ProcessList; // 进程列表
	CListCtrl m_ModuleList;  // 模块列表
	CImageList *m_Icon; // 图标指针
	COptiClear* m_pOpti; // 父窗口指针
	vector<DWORD> vecTid; // 容器储存注入程序每个线程的TID	

	HANDLE threadHandle;

	afx_msg void OnNMRClickList1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnRemoteInject();
	afx_msg void OnDropFiles(HDROP hDropInfo);
	afx_msg void OnSuspendedInject();
	afx_msg void OnAcrossSession0();
	afx_msg void OnAPCInject();
	afx_msg void OnNMClickList1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnFindFileButton();
	afx_msg void OnUnloadDll();
	afx_msg void OnNMCustomdrawList(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnGlobalInject();
	afx_msg void OnGlobalUnload();
	afx_msg void PretendProcess();
	afx_msg void OnPuppetCommand();
	afx_msg void CodeInject();
	afx_msg void OnHideProcess();
	afx_msg void OnCancelHide();
	afx_msg void OnProtectProcess();
};
