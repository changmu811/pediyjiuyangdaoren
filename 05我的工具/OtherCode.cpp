#include "stdafx.h"
#include "OtherCode.h"
#include "aplib.h"
#include <locale.h>

#define MAX_FILE_SIZE (100*1024*1024)	// �����ļ����ƴ�СΪ 100M 

#pragma comment(lib,"aplib.lib")


bool ErrorHint(char* szError)
{
	::MessageBoxA(0, szError, "������ʾ��", 0);		
	return true;
}

// ���ֽ�ת���ֽ�
void WtoA(PBYTE pBase, LPWSTR* ppBuffer)
{
	// ��ȡDLL·��
	TCHAR szBuffer[MAX_PATH] = {};

	// ����ԭ��������ϵ��
	char* old_locale = _strdup(setlocale(LC_CTYPE, NULL));
	// �����µ�����ϵ��
	setlocale(LC_CTYPE, setlocale(LC_ALL, ""));

	size_t siSize = 0;
	mbstowcs_s(&siSize, szBuffer, MAX_PATH, (const char*)pBase, _TRUNCATE);

	// ��ԭ����ϵ��  
	setlocale(LC_CTYPE, old_locale);
	free(old_locale);
	*ppBuffer = szBuffer;
}

// aPlibѹ��
void aPlibCompress(LPWSTR szFilePath)
{
#ifndef _WIN64
	   
	// �ļ���С
	DWORD dwSize = 0;
	// ���ݿռ�
	PBYTE pSrc = NULL;
	// ��ȡ�ļ�����
	ReadFileData(szFilePath, &pSrc, &dwSize);
			
	// ѹ������	
	PBYTE pDest = new BYTE[dwSize]{};
	PBYTE pWorkmen = new BYTE[dwSize]();
	DWORD dwDestSize = aPsafe_pack(pSrc, pDest, dwSize, pWorkmen, NULL, NULL);
	if (dwDestSize == APLIB_ERROR)
	{		
		ErrorHint("ѹ��ʧ�ܣ�");
		delete[] pSrc;
		delete[] pDest;
		delete[] pWorkmen;
		return;
	}
	
	SaveToFile(pDest, dwDestSize, L"aPlibѹ����.aP");

	delete[] pSrc;
	delete[] pDest;
	delete[] pWorkmen;
#endif
}

// aPlib��ѹ
void aPlibDeCompress(LPWSTR szFilePath)
{
#ifndef _WIN64
		
	DWORD dwSize = 0;	
	PBYTE pSrc = NULL;

	// ��ȡ�ļ�
	ReadFileData(szFilePath, &pSrc, &dwSize);
	
	// ����ԭ���ݵĴ�С
	DWORD dwSrcSize = aPsafe_get_orig_size(pSrc);
	PBYTE pDest = new BYTE[dwSrcSize]();

	// ��ѹ
	dwSize = aPsafe_depack(pSrc, dwSize, pDest, dwSrcSize);
	if (dwSize != dwSrcSize)
	{
		
		ErrorHint("��ѹ����");
		delete[] pSrc;
		delete[] pDest;
		return;
	}

	// ����
	SaveToFile(pDest, dwSrcSize, L"aPlib��ѹ��.exe");

	delete[] pSrc;
	delete[] pDest;
#endif

}

// ��ȡ�ļ����ݺʹ�С
void ReadFileData(LPWSTR szFilePath, PBYTE* ppFileBase, PDWORD pFileSize)
{
	// ���ļ���ȡ���
	HANDLE hFile = CreateFile(szFilePath, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE,
		NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		ErrorHint("��ԭ�ļ�ʧ�ܣ�");
		return;
	}

	// ��ȡ�ļ���С
	DWORD dwSize = GetFileSize(hFile, NULL);
	if (dwSize > MAX_FILE_SIZE)
	{
		CloseHandle(hFile);
		ErrorHint("ԭ�ļ�����");
		return;
	}

	// ����ѿռ�
	PBYTE pSrc = new BYTE[dwSize];

	// ��ȡ�ļ�
	DWORD dwDestSize = 0;
	ReadFile(hFile, pSrc, dwSize, &dwDestSize, NULL);
	if (dwSize != dwDestSize)
	{
		CloseHandle(hFile);
		ErrorHint("��ȡԭ�ļ�����");
		delete[] pSrc;
		return;
	}

	*ppFileBase = pSrc;
	*pFileSize = dwSize;

	CloseHandle(hFile);
}

// ����Ϊ�ļ�
void SaveToFile(PBYTE pFileAddr, DWORD FileSize, LPWSTR SavePath)
{
	HANDLE hFile = CreateFile(SavePath, GENERIC_WRITE, FILE_SHARE_READ,
		NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

	if (hFile == INVALID_HANDLE_VALUE)
	{
		ErrorHint("�������ļ�ʧ�ܣ�");
		return;
	}

	DWORD dwWrite = 0;
	WriteFile(hFile, pFileAddr, FileSize, &dwWrite, NULL);

	CloseHandle(hFile);
}

// ����Hashֵ
void GetHash(ALG_ID nHashType, PBYTE pFileData, DWORD dwFileLength,
	PBYTE* pOutHash, PDWORD pOutHashLength)
{
	BOOL bRet = FALSE;

	HCRYPTPROV hCryptProv = NULL;	
	// ��ȡָ��CSP����Կ�������
	bRet = CryptAcquireContext(&hCryptProv, NULL, NULL, PROV_RSA_AES,
		CRYPT_VERIFYCONTEXT //ָ��Ӧ�ó�����Ҫʹ�ù�Կ/˽Կ�ԡ����磬����ִֻ��HASH�ͶԳƼ���
	);
	if(!bRet)
	{
		ErrorHint("CryptAcquireContextʧ�ܣ�");
		return;
	}

	HCRYPTHASH hCrypthash = NULL;
	// ����һ��Hash ����
	bRet = CryptCreateHash(hCryptProv, nHashType, NULL, NULL, &hCrypthash);
	if (!bRet)
	{
		ErrorHint("CryptCreateHashʧ�ܣ�");
		goto _END;
	}

	// ����Hashֵ
	bRet = CryptHashData(hCrypthash, pFileData, dwFileLength, 0);
	if (!bRet)
	{
		ErrorHint("CryptHashDataʧ�ܣ�");		
		goto _END;
	}
	
	// ��ȡHash�ĳ���
	DWORD dwHashLength = 0, dwTemp = sizeof(DWORD);
	bRet = CryptGetHashParam(hCrypthash, HP_HASHSIZE, (PBYTE)(&dwHashLength), &dwTemp, 0);
	if (!bRet)
	{
		ErrorHint("CryptGetHashParamʧ�ܣ�");
		goto _END;
	}

	PBYTE pHash = new BYTE[dwHashLength]();
	// ��ȡHash�������
	bRet = CryptGetHashParam(hCrypthash, HP_HASHVAL, pHash, &dwHashLength, 0);
	if (!bRet)
	{
		ErrorHint("CryptGetHashParam2ʧ�ܣ�");
		delete[] pHash;
		goto _END;
	}

	*pOutHash = pHash;
	*pOutHashLength = dwHashLength;

_END:
	if (hCrypthash)
	{
		::CryptDestroyHash(hCrypthash);
	}
	if (hCryptProv)
	{
		::CryptReleaseContext(hCryptProv, 0);
	}

}

// AES����
void AesEncrypt(PBYTE pPassword, DWORD dwPasswordLength,
	PBYTE pData, DWORD & dwDataLength, DWORD dwBufferLength)
{
	BOOL bRet = FALSE;

	HCRYPTPROV hCryptProv = NULL;
	// ��ȡָ��CSP����Կ�������
	bRet = CryptAcquireContext(&hCryptProv, NULL, NULL, PROV_RSA_AES,
		CRYPT_VERIFYCONTEXT //ָ��Ӧ�ó�����Ҫʹ�ù�Կ/˽Կ�ԡ����磬����ִֻ��HASH�ͶԳƼ���
	);
	if (!bRet)
	{
		ErrorHint("CryptAcquireContextʧ�ܣ�");
		goto _END;
	}

	HCRYPTHASH hCrypthash = NULL;
	// ����һ��Hash ����
	bRet = CryptCreateHash(hCryptProv, CALG_MD5, NULL, NULL, &hCrypthash);
	if (!bRet)
	{
		ErrorHint("CryptCreateHashʧ�ܣ�");
		goto _END;
	}

	// ����Hashֵ
	bRet = CryptHashData(hCrypthash, pPassword, dwPasswordLength, 0);
	if (!bRet)
	{
		ErrorHint("CryptHashDataʧ�ܣ�");
		goto _END;
	}

	// ʹ��Hash������������Կ
	HCRYPTKEY hCryptkey = NULL;
	bRet = ::CryptDeriveKey(hCryptProv, CALG_AES_128, hCrypthash, CRYPT_EXPORTABLE, &hCryptkey);
	if (!bRet)
	{
		ErrorHint("CryptDeriveKeyʧ�ܣ�");
		goto _END;
	}

	// ��������
	bRet = ::CryptEncrypt(hCryptkey, NULL, TRUE, 0, pData, &dwDataLength, dwBufferLength);
	if (!bRet)
	{
		ErrorHint("CryptEncryptʧ�ܣ�");	
		goto _END;
	}

	SaveToFile(pData, dwDataLength, L"AES����.aes");

_END:
	// �ͷž��
	if (hCryptkey) 
		::CryptDestroyKey(hCryptkey);
	if (hCrypthash) 
		::CryptDestroyHash(hCrypthash);
	if (hCryptProv) 
		::CryptReleaseContext(hCryptProv,0);

}

// AES����
void AesDecrypt(PBYTE pPassword, DWORD dwPasswordLength, PBYTE pData, DWORD & dwDataLength)
{
	BOOL bRet = FALSE;

	HCRYPTPROV hCryptProv = NULL;
	// ��ȡָ��CSP����Կ�������
	bRet = CryptAcquireContext(&hCryptProv, NULL, NULL, PROV_RSA_AES,
		CRYPT_VERIFYCONTEXT //ָ��Ӧ�ó�����Ҫʹ�ù�Կ/˽Կ�ԡ����磬����ִֻ��HASH�ͶԳƼ���
	);
	if (!bRet)
	{
		ErrorHint("CryptAcquireContextʧ�ܣ�");
		goto _END;
	}

	HCRYPTHASH hCrypthash = NULL;
	// ����һ��Hash ����
	bRet = CryptCreateHash(hCryptProv, CALG_MD5, NULL, NULL, &hCrypthash);
	if (!bRet)
	{
		ErrorHint("CryptCreateHashʧ�ܣ�");
		goto _END;
	}

	// ����Hashֵ
	bRet = CryptHashData(hCrypthash, pPassword, dwPasswordLength, 0);
	if (!bRet)
	{
		ErrorHint("CryptHashDataʧ�ܣ�");
		goto _END;
	}

	// ʹ��Hash������������Կ
	HCRYPTKEY hCryptkey = NULL;
	bRet = ::CryptDeriveKey(hCryptProv, CALG_AES_128, hCrypthash, CRYPT_EXPORTABLE, &hCryptkey);
	if (!bRet)
	{
		ErrorHint("CryptDeriveKeyʧ�ܣ�");
		goto _END;
	}

	// ��������
	bRet = ::CryptDecrypt(hCryptkey, NULL, TRUE, 0, pData, &dwDataLength);
	if (!bRet)
	{
		ErrorHint("CryptEncryptʧ�ܣ�");
		goto _END;
	}

	SaveToFile(pData, dwDataLength, L"AES����.exe");

_END:
	// �ͷž��
	if (hCryptkey)
		::CryptDestroyKey(hCryptkey);
	if (hCrypthash)
		::CryptDestroyHash(hCrypthash);
	if (hCryptProv)
		::CryptReleaseContext(hCryptProv, 0);
}

// ������Կ��˽Կ
BOOL CreateSecretKey(PBYTE & pPublicKey, DWORD & dwPublicKeyLength,
	PBYTE & pPrivateKey, DWORD & dwPrivateKeyLength)
{
	BOOL bRet = FALSE;

	// 1.��ȡCSP���
	HCRYPTPROV hCryptprov = NULL;
	bRet = CryptAcquireContext(&hCryptprov, NULL, NULL, PROV_RSA_AES, 0);
	if (!bRet)
	{
		ErrorHint("CryptAcquireContext!");
		goto _END;
	}
	
	// 2.���ɹ�˽Կ��Կ��
	HCRYPTKEY hCryptkey = NULL;
	bRet = CryptGenKey(hCryptprov, AT_KEYEXCHANGE, CRYPT_EXPORTABLE, &hCryptkey);
	if (!bRet)
	{
		ErrorHint("CryptGenKey!");
		goto _END;
	}

	// 3.1.��ȡ��Կ����
	bRet = CryptExportKey(hCryptkey, NULL, PUBLICKEYBLOB, 0, NULL, &dwPublicKeyLength);
	if (!bRet)
	{
		ErrorHint("CryptExportKey PublicKey!");
		goto _END;
	}

	// 3.2.��ȡ��Կ����
	PBYTE pPublicData = new BYTE[dwPublicKeyLength]();
	bRet = CryptExportKey(hCryptkey, NULL, PUBLICKEYBLOB, 0, pPublicData, &dwPublicKeyLength);
	if (!bRet)
	{
		ErrorHint("CryptExportKey PublicKey!");
		goto _END;
	}
	pPublicKey = pPublicData;

	// 4.1.��ȡ˽Կ��Ϣ
	bRet = CryptExportKey(hCryptkey, NULL, PRIVATEKEYBLOB, 0, NULL, &dwPrivateKeyLength);
	if (!bRet)
	{
		ErrorHint("CryptExportKey Private!");
		goto _END;
	}

	// 4.2.��ȡ˽Կ����
	PBYTE pPrivateData = new BYTE[dwPrivateKeyLength]();
	bRet = CryptExportKey(hCryptkey, NULL, PRIVATEKEYBLOB, 0, pPrivateData, &dwPrivateKeyLength);
	if (!bRet)
	{
		ErrorHint("CryptExportKey Private!");
		goto _END;
	}
	pPrivateKey = pPrivateData;
	bRet = TRUE;	

_END:
	// 5.�ͷž��
	if (hCryptprov)
		CryptReleaseContext(hCryptprov, 0);
	if (hCryptkey)
		CryptDestroyKey(hCryptkey);

	return bRet;	
}

// RSA��Կ����
void RSAEncrypt(PBYTE pPublicKey, DWORD dwPublicKeyLength,
	PBYTE & pData, DWORD & dwDataLength, DWORD dwBufferLength)
{
	BOOL bRet = FALSE;
	HCRYPTPROV hCryptprov = NULL;
	// 1.��ȡCSP���
	bRet = CryptAcquireContext(&hCryptprov, NULL, NULL, PROV_RSA_FULL, 0);
	if (!bRet)
	{
		ErrorHint("CryptAcquireContext!");
		goto _END;
	}
	
	// 2.���빫Կ
	HCRYPTKEY hCryptkey = NULL;
	bRet = CryptImportKey(hCryptprov, pPublicKey, dwPublicKeyLength, NULL, 0, &hCryptkey);
	if (!bRet)
	{
		ErrorHint("CryptImportKey!");
		goto _END;
	}
	
	// 3.��������
	bRet = ::CryptEncrypt(hCryptkey, NULL, TRUE, 0, pData, &dwDataLength, 4096);	
	if (!bRet)
	{
		ErrorHint("CryptEncrypt!");
		goto _END;
	}
	   
_END:
	if (hCryptkey)
		CryptDestroyKey(hCryptkey);
	if (hCryptprov)
		CryptReleaseContext(hCryptprov, 0);
	
}

// RSA˽Կ����
void RSADecrypt(PBYTE pPrivateKey, DWORD dwPrivateKeyLength, PBYTE & pData, DWORD & dwDataLength)
{
	BOOL bRet = FALSE;

	HCRYPTPROV hCryptprov = NULL;
	bRet = CryptAcquireContext(&hCryptprov, NULL, NULL, PROV_RSA_FULL, 0);
	if (!bRet)
	{
		ErrorHint("CryptAcquireContext!");
		goto _END;
	}

	// ����˽Կ
	HCRYPTKEY hCryptkey = NULL;
	bRet = CryptImportKey(hCryptprov, pPrivateKey, dwPrivateKeyLength, NULL, 0, &hCryptkey);
	if (!bRet)
	{
		ErrorHint("CryptImportKey!");
		goto _END;
	}

	// ��������
	bRet = CryptDecrypt(hCryptkey, NULL, TRUE, 0, pData, &dwDataLength);
	if (!bRet)
	{
		ErrorHint("CryptDecrypt!");
		goto _END;
	}

_END:
	if (hCryptkey)
		CryptDestroyKey(hCryptkey);
	if (hCryptprov)
		CryptReleaseContext(hCryptprov, 0);

}