#pragma once
#include "CTerminateProcess.h"
#include "define.h"

class CHandleOccFile :
    public CTerminateProcess
{
public:
    CHandleOccFile();
    virtual ~CHandleOccFile();

    virtual BOOL AllProcessHandleOccFile(HANDLE handle);//�������н��̽���ռ��
    virtual BOOL HandleOccFileByPid(HANDLE handle,DWORD dwPid);
    virtual HANDLE GetHandleByFileName(STRING FileName);
};

