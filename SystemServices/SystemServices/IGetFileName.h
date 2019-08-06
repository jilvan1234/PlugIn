#pragma once
#include <string>
using namespace std
;
#ifdef UNICODE
#define MYISTRING wstring
#else
#define STRING string
#endif // UNICODE

class IGetFileName
{
public:
    virtual MYISTRING GetStartFileName() = 0; //»ñÈ¡Ãû×Ö.
};

