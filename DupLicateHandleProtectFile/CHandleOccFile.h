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
    virtual HANDLE GetHandleByFileName(STRING FileName);
};

