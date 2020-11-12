// HashAndCrypt.cpp: 实现文件
//

#include "stdafx.h"
#include "HashAndCrypt.h"
#include "afxdialogex.h"
#include "OtherCode.h"
#include "05我的工具.h"


// HashAndCrypt 对话框

IMPLEMENT_DYNAMIC(HashAndCrypt, CDialogEx)

HashAndCrypt::HashAndCrypt(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_HashOrCrypt, pParent)
	, m_FilePath(_T(""))
	, m_DataList(_T(""))
{

}

HashAndCrypt::~HashAndCrypt()
{
}

void HashAndCrypt::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, m_FilePath);
	DDX_Text(pDX, IDC_EDIT2, m_DataList);
}


BEGIN_MESSAGE_MAP(HashAndCrypt, CDialogEx)
	ON_WM_DROPFILES()
	ON_BN_CLICKED(IDC_BUTTON1, &HashAndCrypt::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON11, &HashAndCrypt::OnBnClickedButton11)
	ON_BN_CLICKED(IDC_BUTTON17, &HashAndCrypt::OnBnClickedButton17)
	ON_BN_CLICKED(IDC_BUTTON18, &HashAndCrypt::OnBnClickedButton18)
	ON_BN_CLICKED(IDC_BUTTON19, &HashAndCrypt::OnBnClickedButton19)
END_MESSAGE_MAP()


// HashAndCrypt 消息处理程序


void HashAndCrypt::OnDropFiles(HDROP hDropInfo)
{
	TCHAR szStr[MAX_PATH] = {};
	::DragQueryFile(hDropInfo, 0, szStr, MAX_PATH);
	m_FilePath = szStr;
	OnBnClickedButton11();
	UpdateData(FALSE);
	CDialogEx::OnDropFiles(hDropInfo);
}

// 打开
void HashAndCrypt::OnBnClickedButton1()
{
	// TODO: 在此添加控件通知处理程序代码
	CFileDialog dlg(TRUE, //TRUE为OPEN对话框，FALSE为SAVE AS对话框
		NULL,
		NULL,
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		(LPCTSTR)_TEXT("All Files (*.*)|*.*||"),
		NULL);
	if (dlg.DoModal() == IDOK)
	{
		// 打开成功，保存文件路径
		m_FilePath = dlg.GetPathName();
		OnBnClickedButton11();
	}
	else
	{
		m_FilePath = _T("打开文件失败！");
	}

	UpdateData(FALSE);
}

// 计算Hash
void HashAndCrypt::OnBnClickedButton11()
{
	// TODO: 在此添加控件通知处理程序代码
	m_DataList.Empty();

	WIN32_FIND_DATA wfd = { 0 };
	int nNum = 0;
	//找到第一个文件
	HANDLE hFindFile = FindFirstFile(m_FilePath, &wfd);
	if (hFindFile == INVALID_HANDLE_VALUE)
	{
		return;
	}
	m_DataList += L"名称： ";
	m_DataList += wfd.cFileName;
	m_DataList += L"\r\n";

	CString Str[8] = {};
	if (wfd.nFileSizeLow <= 1024)
	{
		Str[0].Format(L"%d", wfd.nFileSizeLow);
		Str[0] = Str[0] + L" B";
	}
	else if (wfd.nFileSizeLow <= 1024 * 1024)
	{
		Str[0].Format(L"%.1lf", wfd.nFileSizeLow / 1024.0);
		Str[0] = Str[0] + L" KB";
	}
	else if (wfd.nFileSizeLow <= 1024 * 1024 * 1024)
	{
		Str[0].Format(L"%.1lf", wfd.nFileSizeLow / 1024.0 / 1024);
		Str[0] = Str[0] + L" MB";
	}
	else
	{
		Str[0].Format(L"%.2lf", wfd.nFileSizeLow / 1024.0 / 1024 / 1024);
		Str[0] = Str[0] + L" GB";
	}
	m_DataList += L"大小： ";
	m_DataList += Str[0] + L"\r\n";
	
	//创建时间
	FILETIME  Time = { 0 };
	FileTimeToLocalFileTime(&wfd.ftCreationTime, &Time);
	SYSTEMTIME sysTime = { 0 };
	FileTimeToSystemTime(&Time, &sysTime);
	Str[1].Format(L"%d", sysTime.wYear);
	Str[2].Format(L"%02d", sysTime.wMonth);
	Str[3].Format(L"%02d", sysTime.wDay);
	Str[4].Format(L"%02d", sysTime.wHour);
	Str[5].Format(L"%02d", sysTime.wMinute);
	Str[6].Format(L"%02d", sysTime.wSecond);
	Str[7] = Str[1] + L"年" + Str[2] + L"月" + Str[3] + L"日  " +
		Str[4] + L":" + Str[5] + L":" + Str[6];
	m_DataList += L"创建复制时间： " + Str[7] + L"\r\n";
	//最后修改时间
	FileTimeToLocalFileTime(&wfd.ftLastWriteTime, &Time);
	sysTime = { 0 };
	FileTimeToSystemTime(&Time, &sysTime);
	Str[1].Format(L"%d", sysTime.wYear);
	Str[2].Format(L"%02d", sysTime.wMonth);
	Str[3].Format(L"%02d", sysTime.wDay);
	Str[4].Format(L"%02d", sysTime.wHour);
	Str[5].Format(L"%02d", sysTime.wMinute);
	Str[6].Format(L"%02d", sysTime.wSecond);
	Str[7] = Str[1] + L"年" + Str[2] + L"月" + Str[3] + L"日  " +
		Str[4] + L":" + Str[5] + L":" + Str[6];
	m_DataList += L"最后修改时间： " + Str[7] + L"\r\n";
	
	PBYTE pData = nullptr;
	DWORD dwSize = 0;
	ReadFileData(m_FilePath.GetBuffer(), &pData, &dwSize);

	PBYTE pHash = nullptr;
	DWORD dwHashSize = 0;

	// 计算MD5
	GetHash(CALG_MD5, pData, dwSize, &pHash, &dwHashSize);

	CStringA magic,temp;
	magic = "MD5：    ";
	for (DWORD i = 0; i < dwHashSize; i++)
	{
		temp.Format("%X", pHash[i]);
		magic += temp;
	}
	m_DataList += (CString)magic;
	m_DataList += "\r\n";
	delete[] pHash;
	pHash = nullptr;

	// 计算SHA1
	GetHash(CALG_SHA1, pData, dwSize, &pHash, &dwHashSize);
	magic.Empty();
	magic += "SHA1：   ";
	for (DWORD i = 0; i < dwHashSize; i++)
	{
		temp.Format("%X", pHash[i]);
		magic += temp;
	}
	m_DataList += (CString)magic;
	m_DataList += "\r\n";
	delete[] pHash;
	pHash = nullptr;
	// 计算SHA256
	GetHash(CALG_SHA_256, pData, dwSize, &pHash, &dwHashSize);
	magic.Empty();
	magic += "SHA256：";
	for (DWORD i = 0; i < dwHashSize; i++)
	{
		temp.Format("%X", pHash[i]);
		magic += temp;
	}
	m_DataList += (CString)magic;
	m_DataList += "\r\n";	  
	delete[] pHash;	
	delete[] pData;
}

// AES加密
void HashAndCrypt::OnBnClickedButton17()
{
	PBYTE pData = nullptr;
	DWORD dwSize = 0;
	ReadFileData(m_FilePath.GetBuffer(), &pData, &dwSize);	
	PBYTE pNowData = new BYTE[dwSize * 3]();
	memcpy(pNowData, pData, dwSize);
	AesEncrypt((PBYTE)"JiuYangDaoRen", 13, pNowData, dwSize, dwSize * 3);
	delete[] pData;
	delete[] pNowData;

}

// AES解密
void HashAndCrypt::OnBnClickedButton18()
{
	PBYTE pData = nullptr;
	DWORD dwSize = 0;
	ReadFileData(m_FilePath.GetBuffer(), &pData, &dwSize);	
	AesDecrypt((PBYTE)"JiuYangDaoRen", 13, pData, dwSize);
	delete[] pData;
}

// RSA加密
void HashAndCrypt::OnBnClickedButton19()
{
	PBYTE pPublicKey = NULL;  // 公钥
	DWORD dwPublicLength = 0; // 公钥长度
	// 1.生成密钥对
	if (!CreateSecretKey(pPublicKey, dwPublicLength, pPrivateKey, dwPrivateLength)) 
		return;

	CStringA strKey, strTemp;

	CStringA csBuffer = (CStringA)m_FilePath;
	DWORD dwSize = csBuffer.GetLength();
	PBYTE pNowData = new BYTE[4096]();
	memcpy(pNowData, csBuffer.GetBuffer(), dwSize);
	// 加密
	RSAEncrypt(pPublicKey, dwPublicLength, pNowData, dwSize, 4096);
	
	// 2.显示加密字符
	strKey += "\r\n";
	strTemp.Format("%d", dwSize);
	strKey += "加密后字节长度：";
	strKey += strTemp;
	strKey += "\r\n";
	strKey += "RSA加密后字符：\r\n";
	for (DWORD i = 0; i < dwSize; i++)
	{
		strTemp.Format("%c", pNowData[i]);
		strKey += strTemp;
		if (i >= 32 && !(i % 32))
			strKey += "\r\n";
	}	
	strKey += "\r\n";
	strKey += "\r\n";
	strKey += "RSA解密后的字符为：\r\n";

	// 3.显示解密字符
	RSADecrypt(pPrivateKey, dwPrivateLength, pNowData, dwSize);
	for (DWORD i = 0; i < dwSize; i++)
	{
		strTemp.Format("%c", pNowData[i]);
		strKey += strTemp;		
	}
	strKey += "\r\n";
	strKey += "\r\n";

	// 4.公钥内容
	strTemp.Format("%d", dwPublicLength);
	strKey += "公钥长度：";
	strKey += strTemp;
	strKey += "\r\n";
	strKey += "公钥：";
	for (DWORD i = 0; i < dwPublicLength; i++)
	{
		strTemp.Format("%X", pPublicKey[i]);
		strKey += strTemp;
		if (i >= 35 && !(i % 35))
			strKey += "\r\n";
	}
	strKey += "\r\n";
	strKey += "\r\n";

	// 5.私钥内容
	strTemp.Format("%d", dwPrivateLength);
	strKey += "私钥长度：";
	strKey += strTemp;
	strKey += "\r\n";
	strKey += "私钥：";
	for (DWORD i = 0; i < dwPrivateLength; i++)
	{
		strTemp.Format("%X", pPrivateKey[i]);
		strKey += strTemp;
		if (i >= 35 && !(i % 35))
			strKey += "\r\n";
	}
	strKey += "\r\n";
	m_DataList += (CString)strKey;

	UpdateData(FALSE);
	
	delete[] pPublicKey;
	delete[] pNowData;
	
	

}
