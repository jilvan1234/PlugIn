// PeManger.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include "pch.h"
#include <iostream>
#include "CPeManger.h"
#include <stdlib.h>
/*


*/
#define FILE_NAME TEXT("F:\\公司文档\\公司代码\\PlugIn\\TestCode\\Ring3TestCode\\Release\\Ring3TestCode.exe")
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
    HANDLE hFile = CreateFile(FILE_NAME, GENERIC_READ , FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, NULL, NULL);
    //pFileHeader = PeOpt.PeHederGetFileHeader(TEXT(""),hFile);
    pDirectoryList = PeOpt.PeDirGetDirectory(TEXT(""), hFile,PeOpt.PeDesireAccessGetDesiredAccessRead());

    PeOpt.PeImportGetImport(TEXT(""), hFile, PeOpt.PeDesireAccessGetDesiredAccessRead());
    system("pause");
}

