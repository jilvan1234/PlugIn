#include "CFileManger.h"



CFileManger::CFileManger()
{
    m_CurFileName = TEXT("");
}


CFileManger::~CFileManger()
{
}

//传入文件名获取文件句柄
HANDLE CFileManger::FsGetFileHandle(CBinString fsFileName, DWORD dwDesiredAccess, DWORD dwShareMode, LPSECURITY_ATTRIBUTES lpSecurityAttributes, DWORD dwCreationDisposition, DWORD dwFlagsAndAttributes, HANDLE hTemplateFile)
{
  
    m_CurFileName = fsFileName;
    return CreateFile(
        fsFileName.c_str(),
        dwDesiredAccess,
        dwShareMode,
        lpSecurityAttributes,
        dwCreationDisposition,
        dwFlagsAndAttributes,
        hTemplateFile);
}

CBinString CFileManger::FsGetCurFileName() const
{
    return CBinString(m_CurFileName);
}

BOOL CFileManger::FsWriteFile(HANDLE hFile, LPCVOID WriteBuffer, DWORD dwWriteBytes, LPDWORD OutNumberOfBytesWrite, LPOVERLAPPED lpOverlapped)
{
    if (OutNumberOfBytesWrite == NULL)
    {
        DWORD dwOutNumberOfBytesWrite;
        return WriteFile(hFile, WriteBuffer, dwWriteBytes, &dwOutNumberOfBytesWrite, lpOverlapped);
    }
    DWORD dwOutNumberOfBytesWrite;
    if (0 != WriteFile(hFile, WriteBuffer, dwWriteBytes, &dwOutNumberOfBytesWrite, lpOverlapped))
    {
        *(DWORD*)OutNumberOfBytesWrite = dwOutNumberOfBytesWrite;
        return TRUE;
    }
    OutNumberOfBytesWrite = NULL;
    return FALSE;
}

BOOL CFileManger::FsReadFile(HANDLE hFile, LPVOID ReadBuffer, DWORD dwReadBufferSize, LPDWORD OutNumberOfBytesWrite, LPOVERLAPPED lpOverlapped)
{
    if (OutNumberOfBytesWrite == NULL)
    {
        DWORD dwOutNumberOfBytesWrite;
        return ReadFile(hFile, ReadBuffer, dwReadBufferSize, &dwOutNumberOfBytesWrite, lpOverlapped);
    }
    DWORD dwOutNumberOfBytesWrite;
    if (0 != ReadFile(hFile, ReadBuffer, dwReadBufferSize, &dwOutNumberOfBytesWrite, lpOverlapped))
    {
        *(DWORD*)OutNumberOfBytesWrite = dwOutNumberOfBytesWrite;
        return TRUE;
    }
    OutNumberOfBytesWrite = NULL;
    return FALSE;
}

BOOL CFileManger::FsSetFilePoint(HANDLE hFile, LONG lDistanceToMove, PLONG lpDistanceToMoveHigh, DWORD dwMoveMethod)
{
    return SetFilePointer(hFile, lDistanceToMove, lpDistanceToMoveHigh, dwMoveMethod);
}

BOOL CFileManger::FsCreateDirector(CBinString fsDirName, LPSECURITY_ATTRIBUTES lpSecurityAttributes)
{
 
    return CreateDirectory(fsDirName.c_str(), lpSecurityAttributes);
    
}
