// PeManger.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include "pch.h"
#include <iostream>
#include "CPeManger.h"
#include <stdlib.h>

#define FILE_NAME TEXT("F:\\公司文档\\公司代码\\PlugIn\\TestCode\\Ring3TestCode\\Release\\kernel32.dll")

int main()
{
    CPeManger PeOpt;
    LARGE_INTEGER la = { 0 };
   

    printf("low %0x high %0x u.low %0x u.hight %0x \r\n",la.LowPart,la.HighPart,la.u.LowPart,la.u.HighPart);
    PIMAGE_DOS_HEADER pDos = nullptr;
    PIMAGE_NT_HEADERS pNtHeader = nullptr;
    PIMAGE_FILE_HEADER pFileHeader = nullptr;
    PIMAGE_OPTIONAL_HEADER pOptHeader = nullptr;
    vector <PIMAGE_SECTION_HEADER> *pSectionList = nullptr;
    vector <PIMAGE_DATA_DIRECTORY> *pDirectoryList = nullptr;
    vector<PIMAGE_IMPORT_DESCRIPTOR> *pList = NULL;
    vector<PIMAGE_EXPORT_DIRECTORY> *pExportList = nullptr;
    vector<PEXPORT_TYPE_ADDRESSRVA> *pExPortFunctionNameRva = nullptr;
    vector<PEXPORT_FULLNAME> *pExportFullNameList = nullptr;
    vector<PIMPORT_IMPORTPE_FULLNAME> *pImportFullBaseName = nullptr;

    HANDLE hFile = CreateFile(FILE_NAME, GENERIC_READ , FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, NULL, NULL);
    //pFileHeader = PeOpt.PeHederGetFileHeader(TEXT(""),hFile);
    /*pDirectoryList = PeOpt.PeDirGetDirectory(TEXT(""), hFile,PeOpt.PeDesireAccessGetDesiredAccessRead());

    pList = PeOpt.PeImportGetFileImportList(TEXT(""), hFile, PeOpt.PeDesireAccessGetDesiredAccessRead());
    PeOpt.PeFreeImportFileList(pList);*/

    pExportList = PeOpt.PeExPortGetFileExPort(TEXT(""),hFile,PeOpt.PeDesireAccessGetDesiredAccessRead());
    pExPortFunctionNameRva = PeOpt.PeExPortGetFullFunctionNameRva(TEXT(""), hFile, PeOpt.PeDesireAccessGetDesiredAccessRead());

    pExPortFunctionNameRva = PeOpt.PeExPortGetFullFunctionAddressRva(TEXT(""), hFile, PeOpt.PeDesireAccessGetDesiredAccessRead());

    pExportFullNameList = PeOpt.PeExportGetFullFunctionName(TEXT(""), hFile, PeOpt.PeDesireAccessGetDesiredAccessRead());
    pImportFullBaseName = PeOpt.PeImportGetFullBaseName(TEXT(""), hFile, PeOpt.PeDesireAccessGetDesiredAccessRead());

    
    system("pause");
}

