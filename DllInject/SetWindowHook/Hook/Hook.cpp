// Hook.cpp : 定义 DLL 应用程序的导出函数。
//

#include "stdafx.h"
#include "Hook.h"







//开始进行HOOK
HHOOK g_HHOOK = NULL;

extern HMODULE g_hDllModule;

LRESULT CALLBACK MyHookProc(
    _In_ int    nCode,
    _In_ WPARAM wParam,
    _In_ LPARAM lParam
)
{

    return CallNextHookEx(g_HHOOK, nCode, wParam, lParam);
}
BOOL Hook()
{

    g_HHOOK = SetWindowsHookEx(WH_CBT, (HOOKPROC)MyHookProc, g_hDllModule,0);
    if (NULL != g_HHOOK)
    {
        return TRUE;
    }
    return FALSE;
}


BOOL UnHook()
{
    if (NULL != g_HHOOK)
    {
        ::UnhookWindowsHookEx(g_HHOOK);
        return TRUE;
    }
    return FALSE;
}