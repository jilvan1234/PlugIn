#pragma once
#include <list>
#include <vector>

using namespace std;
/*
PeManger主要作用

1.获取Dos头
2.获取Nt头
3.获取文件头
4.获取可选头
5.获取节信息
6.扩大节,合并节.删除节. 删除节名称.
7.获取导入表链表
8.获取导出表链表
//获取重定位表
//获取资源表.

*/
#include "../../publicstruct.h"

class CPeManger
{
public:
    CPeManger();
    ~CPeManger();

public:

    /*
    获取PE信息的各种头.可以通过文件名.也可以通过文件句柄.
    */


    // OPT_dwDesiredAccess


private:
#define PEOPT_READ  0
#define PEOPT_WRITE 1
#define PEOPT_READ_WRITE 2                      //权限相关
#define PEOPT_READ_WRITE_EXCUTE 3
public:
     DWORD PeDesireAccessGetDesiredAccessRead();
     DWORD PeDesireAccessGetDesiredAccessWrite();
     DWORD PeDesireAccessGetDesiredAccessReadWrite();
     DWORD PeDesireAccessGetDesiredAccessReadWriteExcute();
     BOOL  PeDesireInit(DWORD DesireAccess,DWORD&dwMapingDesireAccessType, DWORD&dwViewDesireAccessType, DWORD&dwFileDesireAccessType);

     //获取PE头信息相关
public:
    PIMAGE_DOS_HEADER PeHederGetDosHeader(CBinString FileName,HANDLE hFile = INVALID_HANDLE_VALUE,DWORD DesireAccess = PEOPT_READ);
    PIMAGE_NT_HEADERS PeHederGetNtHeader(CBinString FileName, HANDLE hFile = INVALID_HANDLE_VALUE, DWORD DesireAccess = PEOPT_READ);
    PIMAGE_FILE_HEADER PeHederGetFileHeader(CBinString FileName, HANDLE hFile = INVALID_HANDLE_VALUE, DWORD DesireAccess = PEOPT_READ);
    PIMAGE_OPTIONAL_HEADER PeHederGetOptHeader(CBinString FileName, HANDLE hFile = INVALID_HANDLE_VALUE, DWORD DesireAccess = PEOPT_READ);
    vector<PIMAGE_SECTION_HEADER> * PeHederGetSectionHeader(CBinString FileName,HANDLE hFile = INVALID_HANDLE_VALUE, DWORD DesireAccess = PEOPT_READ);

    //获取PE表相关
public:
    vector<PIMAGE_DATA_DIRECTORY> *PeDirGetDirectory(CBinString FileName, HANDLE hFile = INVALID_HANDLE_VALUE, DWORD DesireAccess = PEOPT_READ);//获取目录表表项.
    
public:
    //导入表相关
    /*
    1.获取所有导入表.加入到链表中
    2.获取文件导入表中的所有 模块名字.
    3.获取单个导入表项中的所有函数名字,以及对应的IAT地址.
    */
    vector<PIMAGE_IMPORT_DESCRIPTOR> *PeImportGetImport(CBinString FileName, HANDLE hFile = INVALID_HANDLE_VALUE, DWORD DesireAccess = PEOPT_READ);
};

