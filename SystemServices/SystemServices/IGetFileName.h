#pragma once
#include <string>
using namespace std
;
#ifdef UNICODE
#define STRING wstring
#else
#define STRING string
#endif // UNICODE

class IGetFileName
{
public:
    virtual STRING GetStartFileName() = 0; //»ñÈ¡Ãû×Ö.
};

