// OtherFunction.cpp: 实现文件
//

#include "stdafx.h"
#include "OtherFunction.h"
#include "afxdialogex.h"
#include "05我的工具.h"
#include "OtherCode.h"
#include "KeyCord.h"
#include "FileMonitor.h"
#include "RemoteCmd.h"
#include <Dbt.h>
// OtherFunction 对话框

IMPLEMENT_DYNAMIC(OtherFunction, CDialogEx)

OtherFunction::OtherFunction(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG5, pParent)
	, m_FilePath(_T(""))
	, m_Remind(_T(""))
{

}

OtherFunction::~OtherFunction()
{
}

void OtherFunction::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, m_FilePath);
	DDX_Text(pDX, IDC_JJJ, m_Remind);
}


BEGIN_MESSAGE_MAP(OtherFunction, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON25, &OtherFunction::OnBnClickedButton25)
	ON_WM_DROPFILES()
	ON_BN_CLICKED(IDC_BUTTON23, &OtherFunction::OnAPIcompress)
	ON_BN_CLICKED(IDC_BUTTON24, &OtherFunction::OnAPIdecompress)
	ON_BN_CLICKED(IDC_BUTTON1, &OtherFunction::OnCompress)
	ON_BN_CLICKED(IDC_BUTTON11, &OtherFunction::OnDeompress)
	ON_BN_CLICKED(IDC_BUTTON17, &OtherFunction::OnDesktopScreenShot)
	ON_BN_CLICKED(IDC_BUTTON18, &OtherFunction::OnKeyRecordDlg)
	ON_BN_CLICKED(IDC_BUTTON19, &OtherFunction::OnUSBcontrol)
	ON_BN_CLICKED(IDC_BUTTON20, &OtherFunction::OnFileMonitor)
	ON_BN_CLICKED(IDC_BUTTON22, &OtherFunction::OnSuicide)
	ON_BN_CLICKED(IDC_BUTTON21, &OtherFunction::OnRemoteCMD)
END_MESSAGE_MAP()


// OtherFunction 消息处理程序

// 打开按钮
void OtherFunction::OnBnClickedButton25()
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
	}
	else
	{
		m_FilePath = _T("打开文件失败！");
	}
	UpdateData(FALSE);
}

// 拖拽响应
void OtherFunction::OnDropFiles(HDROP hDropInfo)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	TCHAR szStr[MAX_PATH] = {};
	::DragQueryFile(hDropInfo, 0, szStr, MAX_PATH);
	m_FilePath = szStr;
	UpdateData(FALSE);

	CDialogEx::OnDropFiles(hDropInfo);
}

void OtherFunction::OnAPIdecompress(){}

// aPlib压缩
void OtherFunction::OnCompress()
{
	m_Remind = L"压缩中，请稍后……";
	UpdateData(FALSE);

	// TODO: 在此添加控件通知处理程序代码
	aPlibCompress(m_FilePath.GetBuffer());
	m_Remind = L"压缩完毕";
	UpdateData(FALSE);
}

// aPlib解压
void OtherFunction::OnDeompress()
{
	// TODO: 在此添加控件通知处理程序代码
	aPlibDeCompress(m_FilePath.GetBuffer());
	m_Remind = L"解压完毕";
	UpdateData(FALSE);
}

// 桌面截屏
void OtherFunction::OnDesktopScreenShot()
{
	// 1.获取桌面窗口句柄
	HWND hWnd = ::GetDesktopWindow();

	// 2.获取桌面窗口 DC
	HDC hDc = ::GetDC(hWnd);

	// 3.创建兼容的 DC
	HDC hComDC = ::CreateCompatibleDC(hDc);

	// 4.获取PC屏幕的宽高
	DWORD dwWidth = ::GetSystemMetrics(SM_CXSCREEN);
	DWORD dwHeight = ::GetSystemMetrics(SM_CYSCREEN);

	// 5.创建兼容位图
	HBITMAP hBitMap = ::CreateCompatibleBitmap(hDc, dwWidth, dwHeight);

	// 6.选中位图
	HBITMAP hGripBmp = (HBITMAP)::SelectObject(hComDC, hBitMap);

	// 7.将窗口内容绘制到位图上
	::BitBlt(hComDC, 0, 0, dwWidth, dwHeight, hDc, 0, 0, SRCCOPY);

	// 8.在位图上绘制光标
	PaintMouse(hComDC);

	// 9.保存为图标
	CImage image;	
	image.Attach(hBitMap);	
	image.Save(L"九阳截图.jpg");

	// 10.释放资源
	::SelectObject(hComDC, hGripBmp);	
	::DeleteDC(hComDC);	
}

// 绘制光标
void OtherFunction::PaintMouse(HDC hDc)
{
	CURSORINFO ci = { sizeof(CURSORINFO) };
	ICONINFO ii = { sizeof(ICONINFO) };
	ci.cbSize = sizeof(ci);

	// 1.创建兼容的 DC
	HDC hNewDC = ::CreateCompatibleDC(hDc);

	// 2.获取光标信息
	::GetCursorInfo(&ci);

	// 3.获取光标的图标信息
	::GetIconInfo(ci.hCursor, &ii);

	// 4.绘制白底黑色鼠标(背景图)
	HBITMAP hOldMask = (HBITMAP)::SelectObject(hNewDC, ii.hbmMask);
	::BitBlt(hDc, ci.ptScreenPos.x, ci.ptScreenPos.y, 20, 20, hNewDC, 0, 0, SRCAND);

	// 5.绘制黑底彩色鼠标(前景图)
	::SelectObject(hNewDC, ii.hbmColor);
	::BitBlt(hDc, ci.ptScreenPos.x, ci.ptScreenPos.y, 20, 20, hNewDC, 0, 0, SRCPAINT);

	// 6.释放资源
	::SelectObject(hNewDC, hOldMask);
	::DeleteObject(ii.hbmColor);
	::DeleteObject(ii.hbmMask);
	::DeleteDC(hNewDC);
}

// 弹出按键记录窗口
void OtherFunction::OnKeyRecordDlg()
{
	KeyCord obj;
	obj.DoModal();
}

// 优盘监控
void OtherFunction::OnUSBcontrol()
{
	
}

// 窗口回调
LRESULT OtherFunction::DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{	
	// 更改设备 消息
	if (message == WM_DEVICECHANGE)
	{
		// 若是插入了新设备
		if (wParam == DBT_DEVICEARRIVAL)
		{
			PDEV_BROADCAST_HDR pbh = (PDEV_BROADCAST_HDR)lParam;
			// 设备类型是逻辑卷时
			if (DBT_DEVTYP_VOLUME == pbh->dbch_devicetype)
			{
				// 根据 unitmask 计算出设备盘符
				PDEV_BROADCAST_VOLUME pbv = (PDEV_BROADCAST_VOLUME)pbh;
				DWORD dwMask = pbv->dbcv_unitmask;
				DWORD dwTemp = 1;
				char szDriver[4] = "A:\\";
				for (szDriver[0] = 'A'; szDriver[0] <= 'Z'; szDriver[0]++)
				{
					if ((dwTemp & dwMask) > 0)
					{
						CStringA str = "已*插入*移动设备： ";
						str += szDriver;
						m_Remind = (CString)str;
						UpdateData(FALSE);
					}
					// 乘2，判断下一个盘符
					dwTemp = (dwTemp << 1);
				}

			}
		}
		// 若是完全移除了设备
		else if (wParam == DBT_DEVICEREMOVECOMPLETE)
		{
			PDEV_BROADCAST_HDR pbh = (PDEV_BROADCAST_HDR)lParam;
			// 设备类型是逻辑卷时
			if (DBT_DEVTYP_VOLUME == pbh->dbch_devicetype)
			{
				// 根据 unitmask 计算出设备盘符
				PDEV_BROADCAST_VOLUME pbv = (PDEV_BROADCAST_VOLUME)pbh;
				DWORD dwMask = pbv->dbcv_unitmask;
				DWORD dwTemp = 1;
				char szDriver[4] = "A:\\";
				for (szDriver[0] = 'A'; szDriver[0] <= 'Z'; szDriver[0]++)
				{
					if ((dwTemp & dwMask) > 0)
					{
						CStringA str = "已-移除-移动设备： ";
						str += szDriver;
						m_Remind = (CString)str;
						UpdateData(FALSE);
					}
					// 乘2，判断下一个盘符
					dwTemp = (dwTemp << 1);
				}

			}
		}
	}

	return CDialogEx::DefWindowProc(message, wParam, lParam);
}

// 文件监控
void OtherFunction::OnFileMonitor()
{
	// TODO: 在此添加控件通知处理程序代码
	FileMonitor obj;
	obj.DoModal();
}

// 自删除，需要管理员权限，并且需要重启电脑后才删除
void OtherFunction::OnSuicide()
{
	CString strPath = L"\\\\?\\";
	TCHAR szPath[MAX_PATH] = {};
	GetModuleFileName(NULL, szPath, MAX_PATH);
	strPath += szPath;
	MoveFileEx(strPath, NULL, MOVEFILE_DELAY_UNTIL_REBOOT);
}

// 批处理自删除
void OtherFunction::OnAPIcompress()
{	
	// 1.获取本程序的路径
	char szMyselfPath[MAX_PATH] = {};
	GetModuleFileNameA(NULL, szMyselfPath, MAX_PATH);

	// 2.获取该路径最后一个斜杠的指针
	char* p = strrchr(szMyselfPath, '\\');
	p[0] = '\0';
		
	// 3.构造批处理内容并生成.bat文件
	/*  下面 5 代表5秒
		@echo off
		choice /t 5 /d y /n >nul
		del *.exe
		del %0
	*/
	CStringA strCmd = szMyselfPath;
	strCmd += "\\temp.bat";
	CStringA strTemp = "@echo off\nchoice /t 5 /d y /n >nul\ndel *.exe\ndel %0\n"; //%
	FILE* fp = NULL;
	fopen_s(&fp, strCmd, "w+");
	if (!fp) 
		return;
	fwrite(strTemp.GetBuffer(), strTemp.GetLength(), 1, fp);
	fclose(fp);
	
	// 4.构造批处理文件路径
	strCmd = "cmd /c call \"";
	strCmd += szMyselfPath;
	strCmd += "\\temp.bat\"";

	// 5.设置进程结构体
	STARTUPINFOA si = {};
	PROCESS_INFORMATION pi = {};
	si.cb = sizeof(si);
	// 5.1.指定wShowWindow有效
	si.dwFlags = STARTF_USESHOWWINDOW;
	// 5.2.设置为隐藏窗口方式创建程序
	si.wShowWindow = FALSE;

	// 6.创建隐藏控制台 执行批处理
	BOOL bRet = ::CreateProcessA(NULL, strCmd.GetBuffer(), NULL, NULL, FALSE,
		CREATE_NEW_CONSOLE,  // 为新进程创建一个控制台窗口
		NULL, NULL, &si, &pi);

	// 7.关闭释放
	if (bRet)
	{
		CloseHandle(pi.hThread);
		CloseHandle(pi.hProcess);
		exit(0);
		ExitProcess(NULL);
	}	
}

// 远控cmd
void OtherFunction::OnRemoteCMD()
{
	RemoteCmd obj;
	obj.DoModal();
}
