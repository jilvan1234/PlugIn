// Hook.cpp : 定义 DLL 应用程序的导出函数。
//

#include "stdafx.h"
#include "Hook.h"
#include "../../../publicstruct.h"
#include "../../../HOOK/MinHook/MinHookInclude/MinHook.h"

#ifdef _WIN64                   //64位版本下
#ifdef _DEBUG
#else
#pragma comment (lib,"../../../HOOK/MinHook/MinHookLib/libMinHook.x64.lib")
#endif
#else                           //32位版本下
#ifdef _DEBUG                   //Debug版本下   

#else
#pragma comment (lib,"../../../HOOK/MinHook/MinHookLib/libMinHook.x86.lib")
#endif // _DEBUG                   //Debug版本下   

#endif // _WIN64            





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


//创建函数指针
using PFNCRATEFILEA = HANDLE(WINAPI *)(_In_ LPCSTR lpFileName, _In_ DWORD dwDesiredAccess, _In_ DWORD dwShareMode, _In_opt_ LPSECURITY_ATTRIBUTES lpSecurityAttributes, _In_ DWORD dwCreationDisposition, _In_ DWORD dwFlagsAndAttributes, _In_opt_ HANDLE hTemplateFile);

PFNCRATEFILEA pFnCreateFileTarget = NULL;  //创建函数指针

//自己的过程处理函数

HANDLE
WINAPI
MyCreateFileA(
    _In_ LPCSTR lpFileName,
    _In_ DWORD dwDesiredAccess,
    _In_ DWORD dwShareMode,
    _In_opt_ LPSECURITY_ATTRIBUTES lpSecurityAttributes,
    _In_ DWORD dwCreationDisposition,
    _In_ DWORD dwFlagsAndAttributes,
    _In_opt_ HANDLE hTemplateFile)
{

   
    ////添加处理函数
    ////OutputDebugStringA(lpFileName);
    string strFileName = lpFileName;
  /*  if (strstr(lpFileName, "WTPcap") != NULL)
    {


    }*/
    if (strFileName.find(TEXT("WTPcap")) != strFileName.npos)
    {
        //CBinString strOutPut = TEXT("找到了WTPCAP");
        //OutputDebugString(strOutPut.c_str());   //找到文化的东西.可以通知主进程退出.
        //互斥体

        //OutputDebugString(lpFileName);
        
        HANDLE hModel = CreateEvent(NULL, TRUE, FALSE, TEXT("WhCnm"));
        //
        if (NULL != ERROR_ALREADY_EXISTS)   //GetLastError会出问题.
        {
            SetEvent(hModel); //通知主进程结束地方有问题.
        }

        ExitProcess(0);
       
    }
    if (NULL == pFnCreateFileTarget)
        return 0;
    return pFnCreateFileTarget(lpFileName, dwDesiredAccess, dwShareMode, lpSecurityAttributes, dwCreationDisposition, dwFlagsAndAttributes, hTemplateFile);
}
BOOL CreateFileStartHook()
{
    if (MH_Initialize() != MH_OK)
    {
        //OutputDebugString(TEXT("HOOK 失败,初始化失败\r\n"));
        return FALSE;
    }

    // Create a hook for MessageBoxW, in disabled state.

    if (MH_CreateHook(&CreateFileA, &MyCreateFileA,        //创建HOOK

        reinterpret_cast<void**>(&pFnCreateFileTarget)) != MH_OK)
    {

        //OutputDebugString(TEXT("创建HOOK失败,初始化失败\r\n"));
        return FALSE;
    }

    // Enable the hook for MessageBoxW.

    if (MH_EnableHook(&CreateFileA) != MH_OK)            //启动HOOK
    {


        return FALSE;
    }


    return TRUE;
}

