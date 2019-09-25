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

//通用类型结构.保存 导出表函数地址表的 RVA 函数名字表的RVA 函数序号表的RVA
typedef struct _EXPORT_TYPE_ADDRESSRVA
{
    DWORD ExpTypeRva;
}EXPORT_TYPE_ADDRESSRVA,*PEXPORT_TYPE_ADDRESSRVA;

typedef struct _EXPORT_FULLNAME
{
    TCHAR *FunctionName = NULL;
    TCHAR *UndName = NULL;      //名称粉碎后的数值.
}EXPORT_FULLNAME, *PEXPORT_FULLNAME;


typedef struct _IMPORT_IMPORTPE_FULLNAME
{
    TCHAR *ImportDllName;
}IMPORT_IMPORTPE_FULLNAME, *PIMPORT_IMPORTPE_FULLNAME;


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
    作用: 返回文件中所有导入表.
    1.参数1: 文件名字
    2.参数2: 文件句柄
    3.参数3: 权限  文件的权限.内部操作文件会用到
    返回值: 返回文件的所有导入表.存放在List表格中.
    */
    vector<PIMAGE_IMPORT_DESCRIPTOR> *PeImportGetFileImportList(CBinString FileName, HANDLE hFile = INVALID_HANDLE_VALUE, DWORD DesireAccess = PEOPT_READ);
    
    //获取导入表中所有依赖的DLL
    vector<PIMPORT_IMPORTPE_FULLNAME> *PeImportGetFullBaseName(CBinString FileName, HANDLE hFile = INVALID_HANDLE_VALUE, DWORD DesireAccess = PEOPT_READ);
    /**
    参数1: 由peImportGetImport返回的导入表List结构
    返回值: 释放内存,成功返回TRUE,失败返回False
    */
    BOOL  PeFreeImportFileList(vector<PIMAGE_IMPORT_DESCRIPTOR> * pList); //释放ImportList的内存.


    vector<PIMAGE_EXPORT_DIRECTORY> *PeExPortGetFileExPort(CBinString FileName, HANDLE hFile = INVALID_HANDLE_VALUE, DWORD DesireAccess = PEOPT_READ);

    //获取导出表中的所有名字的RVA
    vector< PEXPORT_TYPE_ADDRESSRVA> *PeExPortGetFullFunctionNameRva(CBinString FileName, HANDLE hFile = INVALID_HANDLE_VALUE, DWORD DesireAccess = PEOPT_READ);

    //获取导出表中所有导出函数的地址表的RVA
    vector< PEXPORT_TYPE_ADDRESSRVA> *PeExPortGetFullFunctionAddressRva(CBinString FileName, HANDLE hFile = INVALID_HANDLE_VALUE, DWORD DesireAccess = PEOPT_READ);


    //获取导出表所有函数名称.
    vector<PEXPORT_FULLNAME> *PeExportGetFullFunctionName(CBinString FileName, HANDLE hFile = INVALID_HANDLE_VALUE, DWORD DesireAccess = PEOPT_READ);

public:
    DWORD RvAToFoA(DWORD RvA,CBinString FileName, HANDLE hFile = INVALID_HANDLE_VALUE, DWORD DesireAccess = PEOPT_READ);
    DWORD FoAToRva(DWORD FoA,CBinString FileName, HANDLE hFile = INVALID_HANDLE_VALUE, DWORD DesireAccess = PEOPT_READ);
public:
    //节相关.


    //扩大一个节.
    //BOOL ExpansionSection(); 
    ////增加一个节
    //BOOL AddSection();

    ////删除一个节
    //BOOL DeleteSection();
    BOOL AddSection(CBinString FileName, HANDLE hFile = INVALID_HANDLE_VALUE);

};

