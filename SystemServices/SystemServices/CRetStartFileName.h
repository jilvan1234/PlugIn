#pragma once
#include "IGetFileName.h"
class CRetStartFileName :
    public IGetFileName
{
public:
    CRetStartFileName();
    virtual ~CRetStartFileName();
    virtual STRING GetStartFileName();
};

