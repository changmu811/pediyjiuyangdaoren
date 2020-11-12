#pragma once
#include <wincrypt.h>

// aPlibѹ��
void aPlibCompress(LPWSTR szFilePath);

// aPlib��ѹ
void aPlibDeCompress(LPWSTR szFilePath);

// ����Ϊ�ļ�
void SaveToFile(PBYTE pFileAddr, DWORD FileSize, LPWSTR SavePath);

// ��ȡ�ļ����ݺʹ�С
void ReadFileData(LPWSTR szFilePath, PBYTE* ppFileBase, PDWORD pFileSize);

// ����Hashֵ
void GetHash(ALG_ID nHashType, PBYTE pFileData, DWORD dwFileLength, PBYTE* pOutHash, PDWORD pOutHashLength);

// ���ֽ�ת���ֽ�
void WtoA(PBYTE pBase, LPWSTR* ppBuffer);

// AES����
void AesEncrypt(PBYTE pPassword, DWORD dwPasswordLength,
	PBYTE pData, DWORD & dwDataLength, DWORD dwBufferLength);

// AES����
void AesDecrypt(PBYTE pPassword, DWORD dwPasswordLength,	PBYTE pData, DWORD & dwDataLength);

// ������Կ��˽Կ
BOOL CreateSecretKey(PBYTE & pPublicKey, DWORD & dwPublicKeyLength,
					 PBYTE & pPrivateKey, DWORD & dwPrivateKeyLength);

// RSA��Կ����
void RSAEncrypt(PBYTE pPublicKey, DWORD dwPublicKeyLength,
	PBYTE & pData, DWORD & dwDataLength, DWORD dwBufferLength);

// RSA˽Կ����
void RSADecrypt(PBYTE pPrivateKey, DWORD dwPrivateKeyLength, PBYTE & pData, DWORD & dwDataLength);

