#pragma once
#include <wincrypt.h>

// aPlib压缩
void aPlibCompress(LPWSTR szFilePath);

// aPlib解压
void aPlibDeCompress(LPWSTR szFilePath);

// 保存为文件
void SaveToFile(PBYTE pFileAddr, DWORD FileSize, LPWSTR SavePath);

// 读取文件数据和大小
void ReadFileData(LPWSTR szFilePath, PBYTE* ppFileBase, PDWORD pFileSize);

// 计算Hash值
void GetHash(ALG_ID nHashType, PBYTE pFileData, DWORD dwFileLength, PBYTE* pOutHash, PDWORD pOutHashLength);

// 多字节转宽字节
void WtoA(PBYTE pBase, LPWSTR* ppBuffer);

// AES加密
void AesEncrypt(PBYTE pPassword, DWORD dwPasswordLength,
	PBYTE pData, DWORD & dwDataLength, DWORD dwBufferLength);

// AES解密
void AesDecrypt(PBYTE pPassword, DWORD dwPasswordLength,	PBYTE pData, DWORD & dwDataLength);

// 创建公钥和私钥
BOOL CreateSecretKey(PBYTE & pPublicKey, DWORD & dwPublicKeyLength,
					 PBYTE & pPrivateKey, DWORD & dwPrivateKeyLength);

// RSA公钥加密
void RSAEncrypt(PBYTE pPublicKey, DWORD dwPublicKeyLength,
	PBYTE & pData, DWORD & dwDataLength, DWORD dwBufferLength);

// RSA私钥解密
void RSADecrypt(PBYTE pPrivateKey, DWORD dwPrivateKeyLength, PBYTE & pData, DWORD & dwDataLength);

