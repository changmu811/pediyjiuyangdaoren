// 07_加壳项目.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include "stdafx.h"

#ifndef _WIN64

#include <iostream>
#include "Share.h"
#include "aplib.h"

#pragma comment(lib,"aPlib.lib")
//stub信息结构体
struct StubInfo
{
	HMODULE hModule;//
	DWORD dwTextRva; //代码段RVA
	DWORD dwTextSize;//代码段大小
	DWORD dwFunAddr;//导出函数地址
	StubConf* sc;
};
StubInfo si = { 0 };

//RVA to FOA
DWORD RVAtoFOA(char* g_lpBase, DWORD dwRVA)
{
	//落在哪个区段内
	PIMAGE_DOS_HEADER pDos = (PIMAGE_DOS_HEADER)g_lpBase;
	PIMAGE_NT_HEADERS pNt =
		(PIMAGE_NT_HEADERS)
		(pDos->e_lfanew + g_lpBase);

	//找到区段表头地址
	PIMAGE_SECTION_HEADER pSection = IMAGE_FIRST_SECTION(pNt);
	//获取区段个数
	DWORD dwCount = pNt->FileHeader.NumberOfSections;
	for (DWORD i = 0; i < dwCount; i++)
	{
		if (dwRVA >= pSection->VirtualAddress &&
			dwRVA < pSection->VirtualAddress + pSection->SizeOfRawData)
		{
			return dwRVA -
				pSection->VirtualAddress +
				pSection->PointerToRawData;
		}
		pSection++;
	}
	return 0;
}

// 00开始加壳
void OpenShell(char* pExePath)
{	
	char szPath[MAX_PATH] = { 0 };	
	strcat_s(szPath, pExePath);

	char* pTarBuff = NULL;
	int nPeSize = 0;
	pTarBuff = GetFileData(szPath, &nPeSize);
	
	//加载stub
	LoadStub();	
		   	  
	auto pData = GetOptHeader(pTarBuff)->DataDirectory;

	pData[9].Size = 0;
	pData[9].VirtualAddress = 0;
	   
	char* pTarText = GetSecHeader(pTarBuff, ".text")->PointerToRawData + pTarBuff;
	int nSize = GetSecHeader(pTarBuff, ".text")->Misc.VirtualSize;
	for (int i = 0; i < nSize; ++i)
	{
		pTarText[i] ^= 0x15;
	}

	//压缩文件
	PressFile(pTarBuff, nPeSize);
	
	//植入stub
	ImplantStub(pTarBuff, nPeSize);

	//保存文件
	SavePeFile(pTarBuff,nPeSize,"九阳神功.exe");	
}

//01加载stub
void LoadStub()
{	
	//将DLL以不会执行代码的标志加载到进程中.
	HMODULE hStubDll = LoadLibraryExA("Stub.dll", 0,
		DONT_RESOLVE_DLL_REFERENCES);
	si.hModule = hStubDll;
	si.dwTextRva = (DWORD)GetSecHeader((char*)hStubDll, ".text")->VirtualAddress;
	si.dwTextSize = (DWORD)GetSecHeader((char*)hStubDll, ".text")->SizeOfRawData;
	si.dwFunAddr = (DWORD)GetProcAddress(hStubDll, "Start");
	si.sc = (StubConf*)GetProcAddress(hStubDll, "g_Sc");	
}

//02添加一个新区段
void AddSection(char*& pTarBuff, int& nTarSize,
	const char* pSecName, int nSecSize)
{
	//区段数目加1
	GetFileHeader(pTarBuff)->NumberOfSections++;
	PIMAGE_SECTION_HEADER pSec = GetLastSecHeader(pTarBuff);

	DWORD dwFileAlig = GetOptHeader(pTarBuff)->FileAlignment;
	DWORD dwMemAlig = GetOptHeader(pTarBuff)->SectionAlignment;
	//设置新区段信息
	memcpy(pSec->Name, pSecName, 8);
	pSec->Misc.VirtualSize = nSecSize;
	pSec->SizeOfRawData = Aligment(nSecSize, dwFileAlig);
	//内存的RVA
	pSec->VirtualAddress = (pSec - 1)->VirtualAddress +
		Aligment((pSec - 1)->Misc.VirtualSize, dwMemAlig);
	//文件偏移要注意，把之前的文件文件对齐；
	pSec->PointerToRawData = Aligment(nTarSize, dwFileAlig);
	pSec->Characteristics = 0xE00000E0;
	//映像大小,注意要进行内存对齐
	GetOptHeader(pTarBuff)->SizeOfImage = 
		Aligment(pSec->VirtualAddress + pSec->SizeOfRawData, dwMemAlig);
	
	//增加文件大小
	int nNewSize = pSec->SizeOfRawData + pSec->PointerToRawData;
	char* pNewFile = new char[nNewSize] {};	
	memcpy(pNewFile, pTarBuff, nTarSize);
	delete[] pTarBuff;
	pTarBuff = pNewFile;
	nTarSize = nNewSize;
}

//03修复stub的重定位
void FixStubReloc(char* pTarBuff, char*& hModule,DWORD dwNewBase,DWORD dwNewSecRva,char* pSubName)
{
	//获取sutb重定位va
	auto pReloc = (PIMAGE_BASE_RELOCATION)
		(GetOptHeader(hModule)->DataDirectory[5].VirtualAddress
			+ hModule);

	//获取stub的.text区段的Rva
	DWORD dwTextRva = (DWORD)GetSecHeader(hModule, ".text")->VirtualAddress;
	//获取壳区段的Rva
	DWORD dwShellRav = GetSecHeader(pTarBuff, pSubName)->VirtualAddress;

	//修复重定位
	while (pReloc->SizeOfBlock)
	{
		struct TypeOffset 
		{
			WORD offset : 12;
			WORD type : 4;
		};
		TypeOffset* pTyOf = (TypeOffset*)(pReloc + 1);
		DWORD dwCount = (pReloc->SizeOfBlock - 8) / 2;
		for (size_t i = 0; i < dwCount; i++)
		{
			if(pTyOf[i].type != 3)
				continue;
			//要修复的Rva
			DWORD dwFixRva = pTyOf[i].offset + pReloc->VirtualAddress;				
			//要修复的地址
			DWORD* pFixAddr = (DWORD*)(dwFixRva + (DWORD)hModule);	
			DWORD dwOldProc;
			VirtualProtect(pFixAddr, 4, PAGE_READWRITE, &dwOldProc);
			*pFixAddr -= (DWORD)hModule; //减去原始基址
			*pFixAddr -= dwTextRva;      //减去原始代码段Rva
			*pFixAddr += dwNewBase;      //加上新基址
			*pFixAddr += dwNewSecRva;    //加上新Rva		
			VirtualProtect(pFixAddr, 4, dwOldProc, &dwOldProc);
		}
		DWORD dwOld;
		VirtualProtect(pReloc, 4, PAGE_READWRITE, &dwOld);
		pReloc->VirtualAddress -= dwTextRva;	
		pReloc->VirtualAddress += dwShellRav;
		VirtualProtect(pReloc, 4, dwOld, &dwOld);
		//指向下一个重定位块
		pReloc = (PIMAGE_BASE_RELOCATION)
			((DWORD)pReloc + pReloc->SizeOfBlock);		
	}		
}

//04植入stub
void ImplantStub(char*& pTarBuff, int& nTarSize)
{	
	//新区段名字
	char NewSecName[] = "JydrStub";

	//添加新区段
	AddSection(pTarBuff, nTarSize, NewSecName, si.dwTextSize);

	//保存原始的OEP
	si.sc->dwOep = GetOptHeader(pTarBuff)->AddressOfEntryPoint;
	//保存导入表Rva
	si.sc->dwImportRVA = GetOptHeader(pTarBuff)->DataDirectory[1].VirtualAddress;
	//保存原始的ImageBase
	si.sc->dwImageBase = GetOptHeader(pTarBuff)->ImageBase;
	//保存原始的Reloc的Rav
	si.sc->dwOldRelocRva = GetOptHeader(pTarBuff)->DataDirectory[5].VirtualAddress;
	//保存原始的Reloc的Size
	si.sc->dwOldRelocSize = GetOptHeader(pTarBuff)->DataDirectory[5].Size;
	   	  
	//修复stub的重定位
	DWORD dwBase = GetOptHeader(pTarBuff)->ImageBase;
	DWORD dwNewSecRva = GetSecHeader(pTarBuff, NewSecName)->VirtualAddress;
	FixStubReloc(pTarBuff,(char*&)si.hModule, dwBase, dwNewSecRva, NewSecName);
	
	//把stub的代码段移植到目标文件的新区段中
	char* pNewSecAddr = GetSecHeader(pTarBuff, NewSecName)->PointerToRawData
		+ pTarBuff;
	char* pStubTextAddr = si.dwTextRva + (char*)si.hModule;	
	memcpy(pNewSecAddr, pStubTextAddr, si.dwTextSize);
		
	/*********************************************************************************/
	//添加新重定位的区段NewReloc
	char Reloc[] = "NewReloc";
	AddSection(pTarBuff, nTarSize, Reloc,
		GetSecHeader((char*)si.hModule, ".reloc")->Misc.VirtualSize-0xc);

	//将stub.dll.reloc 写入到新区段
	memcpy(GetSecHeader(pTarBuff, Reloc)->PointerToRawData + pTarBuff,
		GetSecHeader((char*)si.hModule, ".reloc")->VirtualAddress + (char*)si.hModule,
		GetSecHeader((char*)si.hModule, ".reloc")->Misc.VirtualSize-0xc);

	//修改目标文件重定位表的Rva和大小		
	GetOptHeader(pTarBuff)->DataDirectory[5].VirtualAddress =
		GetSecHeader(pTarBuff, Reloc)->VirtualAddress;
	GetOptHeader(pTarBuff)->DataDirectory[5].Size = 
		GetSecHeader(pTarBuff, Reloc)->Misc.VirtualSize;
	
	//设置新的OEP
	GetOptHeader(pTarBuff)->AddressOfEntryPoint = si.dwFunAddr - 
		(DWORD)si.hModule -	si.dwTextRva + 
		GetSecHeader(pTarBuff, NewSecName)->VirtualAddress;

}

//05压缩文件
void PressFile(char*& pTarBuff, int& nTarSize)
{
	//定义资源、TLS的RVA和大小
	DWORD dwTlsSize = 0;
	DWORD dwTlsRva = 0;
	DWORD dwRsrcSize = 0;
	DWORD dwRsrcRva = 0;
	//获取文件Nt头
	auto pNt = GetNtHeader(pTarBuff);
	//获取区段头
	auto pSec = IMAGE_FIRST_SECTION(pNt);
	//获取文件Tls段地址
	auto pTls = GetSecHeader(pTarBuff, ".tls");
	//获取文件资源段地址
	auto pRsrc = GetSecHeader(pTarBuff, ".rsrc");		
	//资源段的Rva和大小
	dwRsrcRva = pRsrc->VirtualAddress;
	dwRsrcSize = pRsrc->SizeOfRawData;	
	//存储.tls和.rsrc的缓冲区
	char* pTlsBuff = nullptr;
	char* pRsrcBuff = new char[dwRsrcSize] {};
	//若有Tls则存储信息
	if (pTls)
	{
		dwTlsRva = pTls->VirtualAddress;
		dwTlsSize = pTls->SizeOfRawData;
		pTlsBuff = new char[dwTlsSize] {};
		//保存tls内容到缓冲区
		memcpy(pTlsBuff, pTls->PointerToRawData + pTarBuff, dwTlsSize);
	}	
	//保存rsrc内容到缓冲区
	memcpy(pRsrcBuff, pRsrc->PointerToRawData + pTarBuff, dwRsrcSize);

	//临时区段
	auto pTempSec = pSec;
	//第一个非零区段的标志
	bool IsFirstNonzeroSec = true;
	//要压缩的大小
	DWORD dwPackSize = 0;
	//区段总数目
	DWORD dwSecNum = pNt->FileHeader.NumberOfSections;
	//文件头的大小
	DWORD dwHeaderSize = 0;
	//要压缩区段的数量
	si.sc->dwPackSecNum = 0;
	//计算压缩区段的数目与大小
	for (size_t i = 0; i < dwSecNum; i++)
	{		
		if (IsFirstNonzeroSec && pTempSec->SizeOfRawData)
		{
			//获取文件头的大小
			dwHeaderSize = pTempSec->PointerToRawData;
			IsFirstNonzeroSec = false;
		}
		if (pTempSec->VirtualAddress != dwTlsRva &&
			pTempSec->VirtualAddress != dwRsrcRva)
		{
			//计算压缩的大小
			dwPackSize += pTempSec->SizeOfRawData;
			//记录该区段的索引
			si.sc->PackSecInfo[si.sc->dwPackSecNum][0] = i;
			//记录该区段的大小
			si.sc->PackSecInfo[si.sc->dwPackSecNum][1] = pTempSec->SizeOfRawData;
			//压缩区段数目+1
			si.sc->dwPackSecNum++;
		}
		//下一个区段
		pTempSec += 1;
	}

	//时光倒流
	pTempSec = pSec;
	//申请压缩总数据的堆内存
	char* pPackBuff = new char[dwPackSize] {};	
	//已拷贝数据的大小
	DWORD dwAlreadyCopySize = 0;
	for (size_t i = 0; i < dwSecNum; i++)
	{
		if (pTempSec->VirtualAddress != dwTlsRva &&
			pTempSec->VirtualAddress != dwRsrcRva)
		{
			//循环拷贝要压缩的区段数据到缓冲区
			memcpy(pPackBuff + dwAlreadyCopySize,
				pTempSec->PointerToRawData + pTarBuff, 
				pTempSec->SizeOfRawData);
			dwAlreadyCopySize += pTempSec->SizeOfRawData;
			//拷贝完将该区段的文件偏移和大小置为0
			pTempSec->PointerToRawData = 0;
			pTempSec->SizeOfRawData = 0;
		}
		//下一个区段
		pTempSec += 1;
	}

	//***开始压缩***
	char* pWorkmem = (char*)malloc(aP_workmem_size(dwPackSize));
	//接收压缩后数据的缓冲区
	char* pCompressBuf = (char*)malloc(aP_max_packed_size(dwPackSize));
	size_t PackSize = aPsafe_pack(
		pPackBuff,    //要被压缩的数据
		pCompressBuf, //接收被压缩的数据
		dwPackSize,   //被压缩数据的大小
		pWorkmem,     //????????
		NULL, NULL);
	if (PackSize == APLIB_ERROR)
	{
		printf("压缩错误!\n");
		getchar();
		return;
	}
	//将压缩后的数据进行文件对齐
	DWORD dwAlignPackSize = Aligment(PackSize,
		GetOptHeader(pTarBuff)->FileAlignment);

	//把tls与rsrc节移动到头部下面
	if (pTls)
		pTls->PointerToRawData = dwHeaderSize;
	pRsrc->PointerToRawData = dwHeaderSize + dwTlsSize;
	
	//新文件的大小
	DWORD dwNewFileSize = dwHeaderSize + dwRsrcSize + 
		dwAlignPackSize + dwTlsSize;	
	   	 
	//为压缩的数据添加一个区段
	AddSection(pTarBuff, nTarSize, ".txdj", PackSize);
	GetSecHeader(pTarBuff, ".txdj")->PointerToRawData =
		dwHeaderSize + dwTlsSize + dwRsrcSize;
	
	//保存压缩段的Rva和大小
	si.sc->dwPackSecRva = GetSecHeader(pTarBuff,".txdj")->VirtualAddress;
	si.sc->dwPackSecSize = PackSize;	

	//申请一个最终版文件的缓冲区
	char* pNewFileBuff = new char[dwNewFileSize] {};
	//拷贝头文件
	memcpy(pNewFileBuff, pTarBuff, dwHeaderSize);
	//续接tls段
	if (pTls)
		memcpy(pNewFileBuff + dwHeaderSize, pTlsBuff, dwTlsSize);
	//续接rsrc段
	memcpy(pNewFileBuff + dwHeaderSize + dwTlsSize, pRsrcBuff, dwRsrcSize);

	//续接压缩段
	memcpy(pNewFileBuff + dwHeaderSize + dwTlsSize + dwRsrcSize,
		pCompressBuf, PackSize);

	delete[] pTarBuff;
	//指向新文件和改变大小
	pTarBuff = pNewFileBuff;
	nTarSize = dwNewFileSize;

	//释放
	delete[] pTlsBuff;
	delete[] pRsrcBuff;
	delete[] pPackBuff;
	free(pWorkmem);
	free(pCompressBuf);
}





//打开PE文件
HANDLE OpenPeFile(const char* pPath)
{
	return CreateFileA(pPath,
		GENERIC_READ,
		FILE_SHARE_READ,
		NULL, OPEN_ALWAYS,
		FILE_ATTRIBUTE_NORMAL,
		NULL);
}

//保存PE文件
void SavePeFile(const char* pFile, int nSize, const char* pPath)
{
	HANDLE hFile = CreateFileA(pPath, GENERIC_WRITE,
		FILE_SHARE_READ, NULL, CREATE_ALWAYS,
		FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE)
		return;
	DWORD dwWrite = 0;
	WriteFile(hFile, pFile, nSize, &dwWrite, NULL);
	CloseHandle(hFile);
}

char* GetFileData(const char* pPath, int* nSize)
{
	HANDLE hFile = OpenPeFile(pPath);
	if (hFile == INVALID_HANDLE_VALUE)
		return NULL;
	DWORD dwSize = GetFileSize(hFile, NULL);
	//改变大小
	*nSize = dwSize;

	char* pFileBuff = new char[dwSize];
	memset(pFileBuff, 0, dwSize);
	DWORD dwRead = 0;
	ReadFile(hFile, pFileBuff, dwSize, &dwRead, NULL);
	CloseHandle(hFile);
	//返回堆内存
	return pFileBuff;
}

//获取DOS头
PIMAGE_DOS_HEADER GetDosHeader(char* pBase)
{
	return (PIMAGE_DOS_HEADER)pBase;
}

//获取NT头
PIMAGE_NT_HEADERS GetNtHeader(char* pBase)
{
	return (PIMAGE_NT_HEADERS)
		(GetDosHeader(pBase)->e_lfanew + (DWORD)pBase);
}

//获取文件头
PIMAGE_FILE_HEADER GetFileHeader(char* pBase)
{
	return &GetNtHeader(pBase)->FileHeader;
}

//获取扩展头
PIMAGE_OPTIONAL_HEADER GetOptHeader(char* pBase)
{
	return &GetNtHeader(pBase)->OptionalHeader;
}

//获取区段头
PIMAGE_SECTION_HEADER GetSectionHeader(char* pBase)
{
	return IMAGE_FIRST_SECTION(GetNtHeader(pBase));
}

//获取指定的名字的区段头
PIMAGE_SECTION_HEADER GetSecHeader(char* pFile, const char* pName)
{
	//区段数目
	DWORD dwSecNum = GetFileHeader(pFile)->NumberOfSections;
	//区段头
	PIMAGE_SECTION_HEADER pSec = GetSectionHeader(pFile);
	char szBuf[10] = { 0 };
	for (size_t i = 0; i < dwSecNum; i++)
	{
		memcpy_s(szBuf, 8, (char*)pSec[i].Name, 8);
		if (!strcmp(pName, szBuf))
			return pSec + i;
	}
	return nullptr;
}

//获取最后一个区段头
PIMAGE_SECTION_HEADER GetLastSecHeader(char* pFile)
{
	//区段数目
	DWORD dwSecNum = GetFileHeader(pFile)->NumberOfSections;
	//区段头
	PIMAGE_SECTION_HEADER pSec = GetSectionHeader(pFile);

	return pSec + (dwSecNum - 1);
}

//对齐粒度
DWORD Aligment(DWORD dwSize, DWORD dwAlig)
{
	return (dwSize%dwAlig == 0) ? dwSize : (dwSize / dwAlig + 1)*dwAlig;
}

#endif