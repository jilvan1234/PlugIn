#pragma once
#include "publicstruct.h"


//ShllCode管理类.编写ShellCode会用到.


class CShellCode
{
public:
    CShellCode();
    ~CShellCode();
public:

#ifdef _WIN64
    

    unsigned __int64 GetKernelBase();
#else
    unsigned long  GetKernelBase(); //获取Kernel32基地址
#endif // _WIN64


#ifdef _WIN64

#else
   
#endif
};

