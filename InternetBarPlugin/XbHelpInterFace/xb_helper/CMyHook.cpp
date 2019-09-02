#include "CMyHook.h"

#include "CFilterByDll.h"



CMyHook::CMyHook()
{
   
}


CMyHook::~CMyHook()
{
}



BOOL CMyHook::SetMinHOOK()
{
    if (MH_Initialize() != MH_OK)
    {
        OutputDebugString(TEXT("HOOK 失败,初始化失败\r\n"));
        return FALSE;
    }


        //cout << it->first << it->second << endl;  first = Target函数。 Second = 目的函数
        if (MH_CreateHook(&LoadLibraryExW, &MyLoadLibraryExWCallBack,        //创建HOOK

            reinterpret_cast<void**>(&pfnLoadLibraryExWTarget)) != MH_OK)
        {

            OutputDebugString(TEXT("创建HOOK失败,初始化失败\r\n"));
            return FALSE;
        }



    // Create a hook for MessageBoxW, in disabled state.

  

    // Enable the hook for MessageBoxW.

    if (MH_EnableHook(&LoadLibraryExW) != MH_OK)            //启动HOOK
    {

        return FALSE;
    }


    return TRUE;
}

BOOL CMyHook::SetUnMinHook()
{
    // Disable the hook for MessageBoxW.
    if (MH_DisableHook(&LoadLibraryExW) != MH_OK)
    {
        return FALSE;
    }

    // Expected to tell "Not hooked...".


    // Uninitialize MinHook.
    if (MH_Uninitialize() != MH_OK)
    {
        return FALSE;
    }
}


#ifdef _WIN64

#else

BOOL CMyHook::SetDetoursHook86()
{

    DetourRestoreAfterWith();
    DetourTransactionBegin();
    DetourUpdateThread(GetCurrentThread());
    DetourAttach(&(LPVOID&)pfnLoadLibraryExWTarget, MyLoadLibraryExWCallBack);

    return DetourTransactionCommit();

}

BOOL CMyHook::SetUnDetoursHook86()
{
    DetourTransactionBegin();
    DetourUpdateThread(GetCurrentThread());
     //cout << it->first << it->second << endl;  first = Target函数。 Second = 目的函数


   
        //cout << it->first << it->second << endl;
   
     DetourDetach((void **)&pfnLoadLibraryExWTarget, MyLoadLibraryExWCallBack);//撤销拦截函数


   // DetourDetach((void **)&pfnLoadLibraryExWTarget, MyLoadLibraryExWCallBack);//撤销拦截函数
    return DetourTransactionCommit();//
}

#endif

