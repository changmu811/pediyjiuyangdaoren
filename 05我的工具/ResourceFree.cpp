#include "stdafx.h"
#include "ResourceFree.h"

void FreeRes_ShowError(wchar_t *pszText)
{
	wchar_t szErr[MAX_PATH] = { 0 };
	::wsprintf(szErr, L"%ls Error[%d]\n", pszText, ::GetLastError());
#ifdef _DEBUG
	::MessageBox(NULL, szErr, L"ERROR", MB_OK | MB_ICONERROR);
#endif
}

BOOL FreeMyResource(UINT uiResouceName, wchar_t *lpszResourceType, wchar_t *lpszSaveFileName)
{
	// 获取指定模块里的指定资源
	HRSRC hRsrc = ::FindResource(NULL, MAKEINTRESOURCE(uiResouceName), lpszResourceType);
	if (NULL == hRsrc)
	{
		FreeRes_ShowError(L"FindResource");
		return FALSE;
	}
	// 获取资源的大小
	DWORD dwSize = ::SizeofResource(NULL, hRsrc);
	if (0 >= dwSize)
	{
		FreeRes_ShowError(L"SizeofResource");
		return FALSE;
	}
	// 将资源加载到内存里
	HGLOBAL hGlobal = ::LoadResource(NULL, hRsrc);
	if (NULL == hGlobal)
	{
		FreeRes_ShowError(L"LoadResource");
		return FALSE;
	}
	// 锁定资源
	LPVOID lpVoid = ::LockResource(hGlobal);
	if (NULL == lpVoid)
	{
		FreeRes_ShowError(L"LockResource");
		return FALSE;
	}

	// 保存资源为文件
	FILE *fp = NULL;
	_wfopen_s(&fp, lpszSaveFileName, L"wb+");
	if (NULL == fp)
	{
		//FreeRes_ShowError(L"OpenResource");
		//::MessageBoxA(0, "请关闭程序后,右键以管理员身份运行本程序！", "希望峰", 0);
		return FALSE;
	}
	fwrite(lpVoid, sizeof(wchar_t), dwSize, fp);
	fclose(fp);

	return TRUE;
}

