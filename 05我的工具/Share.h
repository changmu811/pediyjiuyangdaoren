#pragma once

#ifdef _WIN64
#else

struct  StubConf
{
	DWORD dwIndex;     //tls索引
	DWORD dwStartData; //tls起始
	DWORD dwEndData;   //tls末尾	
	DWORD dwTlsCallRva;//tls回调地址表Rva

	DWORD dwTlsRva;  //tls的Rva
	DWORD dwTlsSize; //tls的Size

	DWORD dwOep;          //原始OeP
	DWORD dwImportRVA;    //导入表的Rva
	DWORD dwOldRelocSize; //目标文件原始的重定位Size
	DWORD dwOldRelocRva;  //目标文件原始的重定位Rva
	DWORD dwImageBase;    //目标文件原始的默认加载基址
	
	DWORD dwPackSecRva;                //压缩段的Rva
	DWORD dwPackSecSize;               //压缩段的大小
	DWORD dwPackSecNum;                //要压缩区段的个数
	DWORD PackSecInfo[20][2] = { 0 };  //压缩区段的信息(索引和大小)

	DWORD dwTextRva;
	DWORD dwTextSize;

	unsigned char key[20];//密钥
};

//打开PE文件
HANDLE OpenPeFile(const char* pPath);

//保存PE文件
void SavePeFile(const char* pFile, int nSize, const char* pPath);

char* GetFileData(const char* pPath, int* nSize);

//获取DOS头
PIMAGE_DOS_HEADER GetDosHeader(char* pBase);

//获取NT头
PIMAGE_NT_HEADERS GetNtHeader(char* pBase);

//获取文件头
PIMAGE_FILE_HEADER GetFileHeader(char* pBase);

//获取扩展头
PIMAGE_OPTIONAL_HEADER GetOptHeader(char* pBase);

//获取区段头
PIMAGE_SECTION_HEADER GetSectionHeader(char* pBase);

//获取指定的名字的区段头
PIMAGE_SECTION_HEADER GetSecHeader(char* pFile, const char* pName);

//获取最后一个区段头
PIMAGE_SECTION_HEADER GetLastSecHeader(char* pFile);

//对齐粒度
DWORD Aligment(DWORD dwSize, DWORD dwAlig);

// 01加载stub
void LoadStub();

// 02添加一个新区段
void AddSection(char*& pTarBuff, int& nTarSize,	const char* pSecName, int nSecSize);

// 03修复stub的重定位
void FixStubReloc(char* pTarBuff, char*& hModule, DWORD dwNewBase, DWORD dwNewSecRva, char* pSubName);

// 04植入stub
void ImplantStub(char*& pTarBuff, int& nTarSize);

// 05压缩文件
void PressFile(char*& pTarBuff, int& nTarSize);

// 00开始加壳
void OpenShell(char* pExePath);

#endif