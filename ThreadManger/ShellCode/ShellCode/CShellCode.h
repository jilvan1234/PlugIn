#pragma once
#include "publicstruct.h"


//ShllCode������.��дShellCode���õ�.


class CShellCode
{
public:
    CShellCode();
    ~CShellCode();
public:

#ifdef _WIN64
    

    unsigned __int64 GetKernelBase();
#else
    unsigned long  GetKernelBase(); //��ȡKernel32����ַ
#endif // _WIN64


#ifdef _WIN64

#else
   
#endif
};

