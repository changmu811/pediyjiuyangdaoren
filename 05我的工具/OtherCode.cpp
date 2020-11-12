#include "stdafx.h"
#include "OtherCode.h"
#include "aplib.h"
#include <locale.h>

#define MAX_FILE_SIZE (100*1024*1024)	// 单个文件限制大小为 100M 

#pragma comment(lib,"aplib.lib")


bool ErrorHint(char* szError)
{
	::MessageBoxA(0, szError, "错误提示！", 0);		
	return true;
}

// 多字节转宽字节
void WtoA(PBYTE pBase, LPWSTR* ppBuffer)
{
	// 获取DLL路径
	TCHAR szBuffer[MAX_PATH] = {};

	// 保存原来的语言系列
	char* old_locale = _strdup(setlocale(LC_CTYPE, NULL));
	// 设置新的语言系列
	setlocale(LC_CTYPE, setlocale(LC_ALL, ""));

	size_t siSize = 0;
	mbstowcs_s(&siSize, szBuffer, MAX_PATH, (const char*)pBase, _TRUNCATE);

	// 还原语言系列  
	setlocale(LC_CTYPE, old_locale);
	free(old_locale);
	*ppBuffer = szBuffer;
}

// aPlib压缩
void aPlibCompress(LPWSTR szFilePath)
{
#ifndef _WIN64
	   
	// 文件大小
	DWORD dwSize = 0;
	// 数据空间
	PBYTE pSrc = NULL;
	// 读取文件数据
	ReadFileData(szFilePath, &pSrc, &dwSize);
			
	// 压缩数据	
	PBYTE pDest = new BYTE[dwSize]{};
	PBYTE pWorkmen = new BYTE[dwSize]();
	DWORD dwDestSize = aPsafe_pack(pSrc, pDest, dwSize, pWorkmen, NULL, NULL);
	if (dwDestSize == APLIB_ERROR)
	{		
		ErrorHint("压缩失败！");
		delete[] pSrc;
		delete[] pDest;
		delete[] pWorkmen;
		return;
	}
	
	SaveToFile(pDest, dwDestSize, L"aPlib压缩后.aP");

	delete[] pSrc;
	delete[] pDest;
	delete[] pWorkmen;
#endif
}

// aPlib解压
void aPlibDeCompress(LPWSTR szFilePath)
{
#ifndef _WIN64
		
	DWORD dwSize = 0;	
	PBYTE pSrc = NULL;

	// 读取文件
	ReadFileData(szFilePath, &pSrc, &dwSize);
	
	// 计算原数据的大小
	DWORD dwSrcSize = aPsafe_get_orig_size(pSrc);
	PBYTE pDest = new BYTE[dwSrcSize]();

	// 解压
	dwSize = aPsafe_depack(pSrc, dwSize, pDest, dwSrcSize);
	if (dwSize != dwSrcSize)
	{
		
		ErrorHint("解压错误！");
		delete[] pSrc;
		delete[] pDest;
		return;
	}

	// 保存
	SaveToFile(pDest, dwSrcSize, L"aPlib解压后.exe");

	delete[] pSrc;
	delete[] pDest;
#endif

}

// 读取文件数据和大小
void ReadFileData(LPWSTR szFilePath, PBYTE* ppFileBase, PDWORD pFileSize)
{
	// 打开文件获取句柄
	HANDLE hFile = CreateFile(szFilePath, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE,
		NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		ErrorHint("打开原文件失败！");
		return;
	}

	// 获取文件大小
	DWORD dwSize = GetFileSize(hFile, NULL);
	if (dwSize > MAX_FILE_SIZE)
	{
		CloseHandle(hFile);
		ErrorHint("原文件过大！");
		return;
	}

	// 申请堆空间
	PBYTE pSrc = new BYTE[dwSize];

	// 读取文件
	DWORD dwDestSize = 0;
	ReadFile(hFile, pSrc, dwSize, &dwDestSize, NULL);
	if (dwSize != dwDestSize)
	{
		CloseHandle(hFile);
		ErrorHint("读取原文件错误！");
		delete[] pSrc;
		return;
	}

	*ppFileBase = pSrc;
	*pFileSize = dwSize;

	CloseHandle(hFile);
}

// 保存为文件
void SaveToFile(PBYTE pFileAddr, DWORD FileSize, LPWSTR SavePath)
{
	HANDLE hFile = CreateFile(SavePath, GENERIC_WRITE, FILE_SHARE_READ,
		NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

	if (hFile == INVALID_HANDLE_VALUE)
	{
		ErrorHint("创建新文件失败！");
		return;
	}

	DWORD dwWrite = 0;
	WriteFile(hFile, pFileAddr, FileSize, &dwWrite, NULL);

	CloseHandle(hFile);
}

// 计算Hash值
void GetHash(ALG_ID nHashType, PBYTE pFileData, DWORD dwFileLength,
	PBYTE* pOutHash, PDWORD pOutHashLength)
{
	BOOL bRet = FALSE;

	HCRYPTPROV hCryptProv = NULL;	
	// 获取指定CSP的密钥容器句柄
	bRet = CryptAcquireContext(&hCryptProv, NULL, NULL, PROV_RSA_AES,
		CRYPT_VERIFYCONTEXT //指出应用程序不需要使用公钥/私钥对。例如，程序只执行HASH和对称加密
	);
	if(!bRet)
	{
		ErrorHint("CryptAcquireContext失败！");
		return;
	}

	HCRYPTHASH hCrypthash = NULL;
	// 创建一个Hash 对象
	bRet = CryptCreateHash(hCryptProv, nHashType, NULL, NULL, &hCrypthash);
	if (!bRet)
	{
		ErrorHint("CryptCreateHash失败！");
		goto _END;
	}

	// 计算Hash值
	bRet = CryptHashData(hCrypthash, pFileData, dwFileLength, 0);
	if (!bRet)
	{
		ErrorHint("CryptHashData失败！");		
		goto _END;
	}
	
	// 获取Hash的长度
	DWORD dwHashLength = 0, dwTemp = sizeof(DWORD);
	bRet = CryptGetHashParam(hCrypthash, HP_HASHSIZE, (PBYTE)(&dwHashLength), &dwTemp, 0);
	if (!bRet)
	{
		ErrorHint("CryptGetHashParam失败！");
		goto _END;
	}

	PBYTE pHash = new BYTE[dwHashLength]();
	// 获取Hash结果数据
	bRet = CryptGetHashParam(hCrypthash, HP_HASHVAL, pHash, &dwHashLength, 0);
	if (!bRet)
	{
		ErrorHint("CryptGetHashParam2失败！");
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

// AES加密
void AesEncrypt(PBYTE pPassword, DWORD dwPasswordLength,
	PBYTE pData, DWORD & dwDataLength, DWORD dwBufferLength)
{
	BOOL bRet = FALSE;

	HCRYPTPROV hCryptProv = NULL;
	// 获取指定CSP的密钥容器句柄
	bRet = CryptAcquireContext(&hCryptProv, NULL, NULL, PROV_RSA_AES,
		CRYPT_VERIFYCONTEXT //指出应用程序不需要使用公钥/私钥对。例如，程序只执行HASH和对称加密
	);
	if (!bRet)
	{
		ErrorHint("CryptAcquireContext失败！");
		goto _END;
	}

	HCRYPTHASH hCrypthash = NULL;
	// 创建一个Hash 对象
	bRet = CryptCreateHash(hCryptProv, CALG_MD5, NULL, NULL, &hCrypthash);
	if (!bRet)
	{
		ErrorHint("CryptCreateHash失败！");
		goto _END;
	}

	// 计算Hash值
	bRet = CryptHashData(hCrypthash, pPassword, dwPasswordLength, 0);
	if (!bRet)
	{
		ErrorHint("CryptHashData失败！");
		goto _END;
	}

	// 使用Hash来生成派生密钥
	HCRYPTKEY hCryptkey = NULL;
	bRet = ::CryptDeriveKey(hCryptProv, CALG_AES_128, hCrypthash, CRYPT_EXPORTABLE, &hCryptkey);
	if (!bRet)
	{
		ErrorHint("CryptDeriveKey失败！");
		goto _END;
	}

	// 加密数据
	bRet = ::CryptEncrypt(hCryptkey, NULL, TRUE, 0, pData, &dwDataLength, dwBufferLength);
	if (!bRet)
	{
		ErrorHint("CryptEncrypt失败！");	
		goto _END;
	}

	SaveToFile(pData, dwDataLength, L"AES加密.aes");

_END:
	// 释放句柄
	if (hCryptkey) 
		::CryptDestroyKey(hCryptkey);
	if (hCrypthash) 
		::CryptDestroyHash(hCrypthash);
	if (hCryptProv) 
		::CryptReleaseContext(hCryptProv,0);

}

// AES解密
void AesDecrypt(PBYTE pPassword, DWORD dwPasswordLength, PBYTE pData, DWORD & dwDataLength)
{
	BOOL bRet = FALSE;

	HCRYPTPROV hCryptProv = NULL;
	// 获取指定CSP的密钥容器句柄
	bRet = CryptAcquireContext(&hCryptProv, NULL, NULL, PROV_RSA_AES,
		CRYPT_VERIFYCONTEXT //指出应用程序不需要使用公钥/私钥对。例如，程序只执行HASH和对称加密
	);
	if (!bRet)
	{
		ErrorHint("CryptAcquireContext失败！");
		goto _END;
	}

	HCRYPTHASH hCrypthash = NULL;
	// 创建一个Hash 对象
	bRet = CryptCreateHash(hCryptProv, CALG_MD5, NULL, NULL, &hCrypthash);
	if (!bRet)
	{
		ErrorHint("CryptCreateHash失败！");
		goto _END;
	}

	// 计算Hash值
	bRet = CryptHashData(hCrypthash, pPassword, dwPasswordLength, 0);
	if (!bRet)
	{
		ErrorHint("CryptHashData失败！");
		goto _END;
	}

	// 使用Hash来生成派生密钥
	HCRYPTKEY hCryptkey = NULL;
	bRet = ::CryptDeriveKey(hCryptProv, CALG_AES_128, hCrypthash, CRYPT_EXPORTABLE, &hCryptkey);
	if (!bRet)
	{
		ErrorHint("CryptDeriveKey失败！");
		goto _END;
	}

	// 加密数据
	bRet = ::CryptDecrypt(hCryptkey, NULL, TRUE, 0, pData, &dwDataLength);
	if (!bRet)
	{
		ErrorHint("CryptEncrypt失败！");
		goto _END;
	}

	SaveToFile(pData, dwDataLength, L"AES解密.exe");

_END:
	// 释放句柄
	if (hCryptkey)
		::CryptDestroyKey(hCryptkey);
	if (hCrypthash)
		::CryptDestroyHash(hCrypthash);
	if (hCryptProv)
		::CryptReleaseContext(hCryptProv, 0);
}

// 创建公钥和私钥
BOOL CreateSecretKey(PBYTE & pPublicKey, DWORD & dwPublicKeyLength,
	PBYTE & pPrivateKey, DWORD & dwPrivateKeyLength)
{
	BOOL bRet = FALSE;

	// 1.获取CSP句柄
	HCRYPTPROV hCryptprov = NULL;
	bRet = CryptAcquireContext(&hCryptprov, NULL, NULL, PROV_RSA_AES, 0);
	if (!bRet)
	{
		ErrorHint("CryptAcquireContext!");
		goto _END;
	}
	
	// 2.生成公私钥密钥对
	HCRYPTKEY hCryptkey = NULL;
	bRet = CryptGenKey(hCryptprov, AT_KEYEXCHANGE, CRYPT_EXPORTABLE, &hCryptkey);
	if (!bRet)
	{
		ErrorHint("CryptGenKey!");
		goto _END;
	}

	// 3.1.获取公钥长度
	bRet = CryptExportKey(hCryptkey, NULL, PUBLICKEYBLOB, 0, NULL, &dwPublicKeyLength);
	if (!bRet)
	{
		ErrorHint("CryptExportKey PublicKey!");
		goto _END;
	}

	// 3.2.获取公钥数据
	PBYTE pPublicData = new BYTE[dwPublicKeyLength]();
	bRet = CryptExportKey(hCryptkey, NULL, PUBLICKEYBLOB, 0, pPublicData, &dwPublicKeyLength);
	if (!bRet)
	{
		ErrorHint("CryptExportKey PublicKey!");
		goto _END;
	}
	pPublicKey = pPublicData;

	// 4.1.获取私钥信息
	bRet = CryptExportKey(hCryptkey, NULL, PRIVATEKEYBLOB, 0, NULL, &dwPrivateKeyLength);
	if (!bRet)
	{
		ErrorHint("CryptExportKey Private!");
		goto _END;
	}

	// 4.2.获取私钥数据
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
	// 5.释放句柄
	if (hCryptprov)
		CryptReleaseContext(hCryptprov, 0);
	if (hCryptkey)
		CryptDestroyKey(hCryptkey);

	return bRet;	
}

// RSA公钥加密
void RSAEncrypt(PBYTE pPublicKey, DWORD dwPublicKeyLength,
	PBYTE & pData, DWORD & dwDataLength, DWORD dwBufferLength)
{
	BOOL bRet = FALSE;
	HCRYPTPROV hCryptprov = NULL;
	// 1.获取CSP句柄
	bRet = CryptAcquireContext(&hCryptprov, NULL, NULL, PROV_RSA_FULL, 0);
	if (!bRet)
	{
		ErrorHint("CryptAcquireContext!");
		goto _END;
	}
	
	// 2.导入公钥
	HCRYPTKEY hCryptkey = NULL;
	bRet = CryptImportKey(hCryptprov, pPublicKey, dwPublicKeyLength, NULL, 0, &hCryptkey);
	if (!bRet)
	{
		ErrorHint("CryptImportKey!");
		goto _END;
	}
	
	// 3.加密数据
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

// RSA私钥解密
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

	// 导入私钥
	HCRYPTKEY hCryptkey = NULL;
	bRet = CryptImportKey(hCryptprov, pPrivateKey, dwPrivateKeyLength, NULL, 0, &hCryptkey);
	if (!bRet)
	{
		ErrorHint("CryptImportKey!");
		goto _END;
	}

	// 解密数据
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