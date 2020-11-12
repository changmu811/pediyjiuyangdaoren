#pragma once


// HashAndCrypt 对话框

class HashAndCrypt : public CDialogEx
{
	DECLARE_DYNAMIC(HashAndCrypt)

public:
	HashAndCrypt(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~HashAndCrypt();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_HashOrCrypt };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnDropFiles(HDROP hDropInfo);
	CString m_FilePath;
	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedButton11();
	CString m_DataList;
	afx_msg void OnBnClickedButton17();
	afx_msg void OnBnClickedButton18();
	afx_msg void OnBnClickedButton19();

private:
	PBYTE pPrivateKey = NULL;  // 私钥
	DWORD dwPrivateLength = 0; // 私钥长度
};
