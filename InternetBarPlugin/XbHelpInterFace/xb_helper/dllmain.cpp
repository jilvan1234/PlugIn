// dllmain.cpp : 定义 DLL 应用程序的入口点。
#include "stdafx.h"
#include "CFilterByDll.h"

#include "CMyHook.h"

CFilterByDll PassDll;
CMyHook g_Hook;

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
   


    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
#ifdef _WIN64
        
#else
        g_Hook.SetDetoursHook86();
#endif // _WIN64

      
        break;
    case DLL_THREAD_ATTACH:
        break;
    case DLL_THREAD_DETACH:
        break;
    case DLL_PROCESS_DETACH:
#ifdef _WIN64

#else
        g_Hook.SetUnDetoursHook86();
#endif
        break;
    }
    return TRUE;
}

