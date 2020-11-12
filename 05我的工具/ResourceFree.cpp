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
	// ��ȡָ��ģ�����ָ����Դ
	HRSRC hRsrc = ::FindResource(NULL, MAKEINTRESOURCE(uiResouceName), lpszResourceType);
	if (NULL == hRsrc)
	{
		FreeRes_ShowError(L"FindResource");
		return FALSE;
	}
	// ��ȡ��Դ�Ĵ�С
	DWORD dwSize = ::SizeofResource(NULL, hRsrc);
	if (0 >= dwSize)
	{
		FreeRes_ShowError(L"SizeofResource");
		return FALSE;
	}
	// ����Դ���ص��ڴ���
	HGLOBAL hGlobal = ::LoadResource(NULL, hRsrc);
	if (NULL == hGlobal)
	{
		FreeRes_ShowError(L"LoadResource");
		return FALSE;
	}
	// ������Դ
	LPVOID lpVoid = ::LockResource(hGlobal);
	if (NULL == lpVoid)
	{
		FreeRes_ShowError(L"LockResource");
		return FALSE;
	}

	// ������ԴΪ�ļ�
	FILE *fp = NULL;
	_wfopen_s(&fp, lpszSaveFileName, L"wb+");
	if (NULL == fp)
	{
		//FreeRes_ShowError(L"OpenResource");
		//::MessageBoxA(0, "��رճ����,�Ҽ��Թ���Ա������б�����", "ϣ����", 0);
		return FALSE;
	}
	fwrite(lpVoid, sizeof(wchar_t), dwSize, fp);
	fclose(fp);

	return TRUE;
}

