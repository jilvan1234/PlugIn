#pragma once


#include "../../../publicstruct.h"
#include "../../../HOOK/MinHook/MinHookInclude/MinHook.h"
#include "../../../HOOK/DetoursInclude/detours.h"
#include "../../../HOOK/DetoursInclude/detver.h"
#include "HookFunctionDefine.h"


#ifdef _WIN64
//../../../../HOOK/X64Detours.lib\detours.lib;
#pragma comment(lib,"../../../HOOK/MinHook/MinHookLib/libMinHook.x64.lib")
#pragma comment(lib,"../../../HOOK/X64Detours.lib/detours.lib")
#else
#pragma comment(lib,"../../../HOOK/MinHook/MinHookLib/libMinHook.x86.lib")
#pragma comment(lib,"../../../HOOK/X86Detours.lib/detours.lib")
#endif // _WIN64

class CMyHook
{
public:
    CMyHook();
    ~CMyHook();

    BOOL SetMinHOOK();
    BOOL SetUnMinHook();

#ifdef _WIN64

#else
    BOOL SetDetoursHook86();
    BOOL SetUnDetoursHook86();
#endif

};

