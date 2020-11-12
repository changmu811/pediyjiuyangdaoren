
// 05我的工具Dlg.h : 头文件
//

#pragma once
#include "MyProGame.h"
#include "NovelName.h"
#include "COptiClear.h"
#include "afxcmn.h"


// CMy05我的工具Dlg 对话框
class CMy05我的工具Dlg : public CDialogEx
{
// 构造
public:
	CMy05我的工具Dlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_MY05_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()


	//修正移动时窗口的大小
	void FixMoving(LPRECT pRect);
	//修正改变窗口大小时窗口的大小
	void FixSizing(UINT fwSide, LPRECT pRect);
	//从收缩状态显示窗口
	void DoShow();
	//从显示状态收缩窗口
	void DoHide();
	//重载函数,只是为了方便调用
	BOOL SetWindowPos(const CWnd* pWndInsertAfter,
		LPCRECT pCRect, UINT nFlags = SWP_SHOWWINDOW);
	//存档是否处于贴边隐藏状态设置
	void SaveSite();
	//读取存档设置
	void ReadSite();

private:
	BOOL m_isSizeChanged;   //窗口大小是否改变了 
	BOOL m_isSetTimer;      //是否设置了检测鼠标的Timer

	INT  m_oldWndHeight;    //旧的窗口宽度
	INT  m_taskBarHeight;   //任务栏高度
	INT  m_edgeHeight;      //边缘高度
	INT  m_edgeWidth;       //边缘宽度

	INT  m_hideMode;        //隐藏模式
	BOOL m_hsFinished;      //隐藏或显示过程是否完成
	BOOL m_hiding;          //该参数只有在!m_hsFinished才有效


public:
	afx_msg void OnBnClickedButton1();
	CTabCtrl m_nTab;
	CNovelName m_nNovel;
	CMyProGame m_nGame;
	COptiClear m_Clear;
	
	void FreeRes();//释放资源
	BOOL IsAlreadyRun();

	afx_msg void OnSelchangeTab(NMHDR *pNMHDR, LRESULT *pResult);

	afx_msg void OnHotKey(UINT nHotKeyId, UINT nKey1, UINT nKey2);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual void OnOK();
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	afx_msg void OnMoving(UINT fwSide, LPRECT pRect);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnSizing(UINT fwSide, LPRECT pRect);
	afx_msg LRESULT OnNcHitTest(CPoint point);
};
