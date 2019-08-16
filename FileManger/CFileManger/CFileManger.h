#pragma once
/*
获取文件句柄
读取文件
写入文件
创建/删除文件夹
锁定/解锁文件
设置文件/目录属性
*/
#include "../../publicstruct.h"

class CFileManger
{
public:
    CFileManger();
    ~CFileManger();

public:
    //传入文件名.获取文件句柄
    HANDLE FsCreateFile(
        CBinString fsFileName,
        DWORD dwDesiredAccess = GENERIC_READ |GENERIC_WRITE,
        DWORD dwShareMode = FILE_SHARE_WRITE | FILE_SHARE_READ, 
        LPSECURITY_ATTRIBUTES lpSecurityAttributes = NULL, 
        DWORD  dwCreationDisposition = OPEN_EXISTING,
        DWORD  dwFlagsAndAttributes = FILE_ATTRIBUTE_NORMAL,
        HANDLE hTemplateFile = NULL);
    CBinString FsGetCurFileName() const;
   

 

    //写文件
    BOOL FsWriteFile(
        HANDLE hFile,
        LPCVOID WriteBuffer,
        DWORD dwWriteBytes,
        LPDWORD OutNumberOfBytesWrite = NULL,
        LPOVERLAPPED lpOverlapped = NULL);

    //读文件
    BOOL FsReadFile(
        HANDLE hFile,
        LPVOID ReadBuffer,
        DWORD dwReadBufferSize,
        LPDWORD OutNumberOfBytesWrite = NULL,
        LPOVERLAPPED lpOverlapped = NULL);
   
 
    //移动文件指针.
    BOOL FsSetFilePoint(HANDLE hFile,LONG lDistanceToMove, PLONG lpDistanceToMoveHigh, DWORD dwMoveMethod);

public:
    //文件夹操作.
    BOOL FsCreateDirector(CBinString fsDirName,LPSECURITY_ATTRIBUTES lpSecurityAttributes = NULL);
    BOOL FsRemoveDirector(CBinString fsDirName, LPSECURITY_ATTRIBUTES lpSecurityAttributes = NULL);

private:
    CBinString m_CurFileName;
};

