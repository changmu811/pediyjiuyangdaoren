
// 05�ҵĹ���Dlg.h : ͷ�ļ�
//

#pragma once
#include "MyProGame.h"
#include "NovelName.h"
#include "COptiClear.h"
#include "afxcmn.h"


// CMy05�ҵĹ���Dlg �Ի���
class CMy05�ҵĹ���Dlg : public CDialogEx
{
// ����
public:
	CMy05�ҵĹ���Dlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_MY05_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()


	//�����ƶ�ʱ���ڵĴ�С
	void FixMoving(LPRECT pRect);
	//�����ı䴰�ڴ�Сʱ���ڵĴ�С
	void FixSizing(UINT fwSide, LPRECT pRect);
	//������״̬��ʾ����
	void DoShow();
	//����ʾ״̬��������
	void DoHide();
	//���غ���,ֻ��Ϊ�˷������
	BOOL SetWindowPos(const CWnd* pWndInsertAfter,
		LPCRECT pCRect, UINT nFlags = SWP_SHOWWINDOW);
	//�浵�Ƿ�����������״̬����
	void SaveSite();
	//��ȡ�浵����
	void ReadSite();

private:
	BOOL m_isSizeChanged;   //���ڴ�С�Ƿ�ı��� 
	BOOL m_isSetTimer;      //�Ƿ������˼������Timer

	INT  m_oldWndHeight;    //�ɵĴ��ڿ��
	INT  m_taskBarHeight;   //�������߶�
	INT  m_edgeHeight;      //��Ե�߶�
	INT  m_edgeWidth;       //��Ե���

	INT  m_hideMode;        //����ģʽ
	BOOL m_hsFinished;      //���ػ���ʾ�����Ƿ����
	BOOL m_hiding;          //�ò���ֻ����!m_hsFinished����Ч


public:
	afx_msg void OnBnClickedButton1();
	CTabCtrl m_nTab;
	CNovelName m_nNovel;
	CMyProGame m_nGame;
	COptiClear m_Clear;
	
	void FreeRes();//�ͷ���Դ
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
