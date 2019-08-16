#pragma once
/*
��ȡ�ļ����
��ȡ�ļ�
д���ļ�
����/ɾ���ļ���
����/�����ļ�
�����ļ�/Ŀ¼����
*/
#include "../../publicstruct.h"

class CFileManger
{
public:
    CFileManger();
    ~CFileManger();

public:
    //�����ļ���.��ȡ�ļ����
    HANDLE FsCreateFile(
        CBinString fsFileName,
        DWORD dwDesiredAccess = GENERIC_READ |GENERIC_WRITE,
        DWORD dwShareMode = FILE_SHARE_WRITE | FILE_SHARE_READ, 
        LPSECURITY_ATTRIBUTES lpSecurityAttributes = NULL, 
        DWORD  dwCreationDisposition = OPEN_EXISTING,
        DWORD  dwFlagsAndAttributes = FILE_ATTRIBUTE_NORMAL,
        HANDLE hTemplateFile = NULL);
    CBinString FsGetCurFileName() const;
   

 

    //д�ļ�
    BOOL FsWriteFile(
        HANDLE hFile,
        LPCVOID WriteBuffer,
        DWORD dwWriteBytes,
        LPDWORD OutNumberOfBytesWrite = NULL,
        LPOVERLAPPED lpOverlapped = NULL);

    //���ļ�
    BOOL FsReadFile(
        HANDLE hFile,
        LPVOID ReadBuffer,
        DWORD dwReadBufferSize,
        LPDWORD OutNumberOfBytesWrite = NULL,
        LPOVERLAPPED lpOverlapped = NULL);
   
 
    //�ƶ��ļ�ָ��.
    BOOL FsSetFilePoint(HANDLE hFile,LONG lDistanceToMove, PLONG lpDistanceToMoveHigh, DWORD dwMoveMethod);

public:
    //�ļ��в���.
    BOOL FsCreateDirector(CBinString fsDirName,LPSECURITY_ATTRIBUTES lpSecurityAttributes = NULL);
    BOOL FsRemoveDirector(CBinString fsDirName, LPSECURITY_ATTRIBUTES lpSecurityAttributes = NULL);

private:
    CBinString m_CurFileName;
};

