#pragma once
#include "CTerminateProcess.h"
#include "define.h"

class CHandleOccFile :
    public CTerminateProcess
{
public:
    CHandleOccFile();
    virtual ~CHandleOccFile();

    virtual BOOL AllProcessHandleOccFile(HANDLE handle);//遍历所有进程进行占用
    virtual BOOL HandleOccFileByPid(HANDLE handle,DWORD dwPid);
    virtual HANDLE GetHandleByFileName(STRING FileName);
};

