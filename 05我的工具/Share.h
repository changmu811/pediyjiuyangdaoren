#pragma once

#ifdef _WIN64
#else

struct  StubConf
{
	DWORD dwIndex;     //tls����
	DWORD dwStartData; //tls��ʼ
	DWORD dwEndData;   //tlsĩβ	
	DWORD dwTlsCallRva;//tls�ص���ַ��Rva

	DWORD dwTlsRva;  //tls��Rva
	DWORD dwTlsSize; //tls��Size

	DWORD dwOep;          //ԭʼOeP
	DWORD dwImportRVA;    //������Rva
	DWORD dwOldRelocSize; //Ŀ���ļ�ԭʼ���ض�λSize
	DWORD dwOldRelocRva;  //Ŀ���ļ�ԭʼ���ض�λRva
	DWORD dwImageBase;    //Ŀ���ļ�ԭʼ��Ĭ�ϼ��ػ�ַ
	
	DWORD dwPackSecRva;                //ѹ���ε�Rva
	DWORD dwPackSecSize;               //ѹ���εĴ�С
	DWORD dwPackSecNum;                //Ҫѹ�����εĸ���
	DWORD PackSecInfo[20][2] = { 0 };  //ѹ�����ε���Ϣ(�����ʹ�С)

	DWORD dwTextRva;
	DWORD dwTextSize;

	unsigned char key[20];//��Կ
};

//��PE�ļ�
HANDLE OpenPeFile(const char* pPath);

//����PE�ļ�
void SavePeFile(const char* pFile, int nSize, const char* pPath);

char* GetFileData(const char* pPath, int* nSize);

//��ȡDOSͷ
PIMAGE_DOS_HEADER GetDosHeader(char* pBase);

//��ȡNTͷ
PIMAGE_NT_HEADERS GetNtHeader(char* pBase);

//��ȡ�ļ�ͷ
PIMAGE_FILE_HEADER GetFileHeader(char* pBase);

//��ȡ��չͷ
PIMAGE_OPTIONAL_HEADER GetOptHeader(char* pBase);

//��ȡ����ͷ
PIMAGE_SECTION_HEADER GetSectionHeader(char* pBase);

//��ȡָ�������ֵ�����ͷ
PIMAGE_SECTION_HEADER GetSecHeader(char* pFile, const char* pName);

//��ȡ���һ������ͷ
PIMAGE_SECTION_HEADER GetLastSecHeader(char* pFile);

//��������
DWORD Aligment(DWORD dwSize, DWORD dwAlig);

// 01����stub
void LoadStub();

// 02���һ��������
void AddSection(char*& pTarBuff, int& nTarSize,	const char* pSecName, int nSecSize);

// 03�޸�stub���ض�λ
void FixStubReloc(char* pTarBuff, char*& hModule, DWORD dwNewBase, DWORD dwNewSecRva, char* pSubName);

// 04ֲ��stub
void ImplantStub(char*& pTarBuff, int& nTarSize);

// 05ѹ���ļ�
void PressFile(char*& pTarBuff, int& nTarSize);

// 00��ʼ�ӿ�
void OpenShell(char* pExePath);

#endif