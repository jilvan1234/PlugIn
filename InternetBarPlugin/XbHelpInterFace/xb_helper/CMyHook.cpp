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
        OutputDebugString(TEXT("HOOK ʧ��,��ʼ��ʧ��\r\n"));
        return FALSE;
    }


        //cout << it->first << it->second << endl;  first = Target������ Second = Ŀ�ĺ���
        if (MH_CreateHook(&LoadLibraryExW, &MyLoadLibraryExWCallBack,        //����HOOK

            reinterpret_cast<void**>(&pfnLoadLibraryExWTarget)) != MH_OK)
        {

            OutputDebugString(TEXT("����HOOKʧ��,��ʼ��ʧ��\r\n"));
            return FALSE;
        }



    // Create a hook for MessageBoxW, in disabled state.

  

    // Enable the hook for MessageBoxW.

    if (MH_EnableHook(&LoadLibraryExW) != MH_OK)            //����HOOK
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
     //cout << it->first << it->second << endl;  first = Target������ Second = Ŀ�ĺ���


   
        //cout << it->first << it->second << endl;
   
     DetourDetach((void **)&pfnLoadLibraryExWTarget, MyLoadLibraryExWCallBack);//�������غ���


   // DetourDetach((void **)&pfnLoadLibraryExWTarget, MyLoadLibraryExWCallBack);//�������غ���
    return DetourTransactionCommit();//
}

#endif

