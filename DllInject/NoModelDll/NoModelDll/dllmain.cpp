// dllmain.cpp : 定义 DLL 应用程序的入口点。
#include "stdafx.h"
#include "NoModelDll.h"


BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
   
    HANDLE hThread = 0;
#ifdef _WIN64
    ULONGLONG newModule = 0;
#else
    DWORD   newModule = 0;
#endif
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:

        
#ifdef _WIN64
      
        newModule = (ULONGLONG)MyHideModule(hModule);
#else
        
        newModule = (DWORD)MyHideModule(hModule);
#endif // _WIN64

       
        if (newModule)
        {

            ////重定位修复调用.新模块基址加上 - 老模块基址 + MessageBox
            //LPTHREAD_START_ROUTINE  hook = (LPTHREAD_START_ROUTINE)(newModule + ((DWORD)HookMessageBox - (DWORD)hModule));
            //hook(NULL);
            //CreateThread(0, 0, hook, 0, 0, 0);          //进行HOOK
            //Sleep(300);

            //调用自定义的DLL main新函数.
            DWORD(WINAPI * pfn)(LPVOID); //定义函数指针
#ifdef _WIN64
            pfn = (DWORD(WINAPI *)(LPVOID))((ULONGLONG)MyTestFunction - (ULONGLONG)hModule + newModule);
#else
            pfn = (DWORD(WINAPI *)(LPVOID))((DWORD)MyTestFunction - (DWORD)hModule + newModule);
#endif
            
            //pfn(NULL);

            hThread = CreateThread(0, 0, pfn, 0, 0, 0);
            Sleep(300);
        }
       
      
        return false;

    case DLL_THREAD_ATTACH:

    case DLL_THREAD_DETACH:

    case DLL_PROCESS_DETACH:
        break;
    }
    return false;
}

