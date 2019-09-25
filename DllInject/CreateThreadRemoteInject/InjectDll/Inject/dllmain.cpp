// dllmain.cpp : 定义 DLL 应用程序的入口点。
#include "stdafx.h"
#include "../../../../publicstruct.h"
#include "../../../../HOOK/MinHook/MinHookInclude/MinHook.h"


#ifdef _WIN64
//../../../../HOOK/X64Detours.lib\detours.lib;
#pragma comment(lib,"../../../../HOOK/MinHook/MinHookLib/libMinHook.x64.lib")

#else
#pragma comment(lib,"../../../../HOOK/MinHook/MinHookLib/libMinHook.x86.lib")
#endif // _WIN64

typedef BOOL(WINAPI * PfnCreateProcessW)(_In_opt_ LPCWSTR lpApplicationName,
    _Inout_opt_ LPWSTR lpCommandLine,
    _In_opt_ LPSECURITY_ATTRIBUTES lpProcessAttributes,
    _In_opt_ LPSECURITY_ATTRIBUTES lpThreadAttributes,
    _In_ BOOL bInheritHandles,
    _In_ DWORD dwCreationFlags,
    _In_opt_ LPVOID lpEnvironment,
    _In_opt_ LPCWSTR lpCurrentDirectory,
    _In_ LPSTARTUPINFOW lpStartupInfo,
    _Out_ LPPROCESS_INFORMATION lpProcessInformation);
PfnCreateProcessW OldPfnCreateProcessW = NULL;
   

BOOL WINAPI MyCreateProcessW(
    _In_opt_ LPCWSTR lpApplicationName,
    _Inout_opt_ LPWSTR lpCommandLine,
    _In_opt_ LPSECURITY_ATTRIBUTES lpProcessAttributes,
    _In_opt_ LPSECURITY_ATTRIBUTES lpThreadAttributes,
    _In_ BOOL bInheritHandles,
    _In_ DWORD dwCreationFlags,
    _In_opt_ LPVOID lpEnvironment,
    _In_opt_ LPCWSTR lpCurrentDirectory,
    _In_ LPSTARTUPINFOW lpStartupInfo,
    _Out_ LPPROCESS_INFORMATION lpProcessInformation
)
{
    CBinString strCmd = lpCommandLine;
    CBinString strApplicateName = lpApplicationName;
    OutputDebugString(lpApplicationName);


    //if (strApplicateName.find(TEXT("calc")) == 0)
    //{
    //    TEXT("strApplicateName 找到Calc 进行阻止 \r\n");
    //    return 0;
    //}
    //if (strCmd.find(TEXT("calc")) == 0)
    //{
    //    OutputDebugString(TEXT("strCmd 找到Calc 进行阻止 \r\n"));
    //    return 0;
    //}

    return OldPfnCreateProcessW(
        lpApplicationName,
        lpCommandLine,
        lpProcessAttributes,
        lpThreadAttributes,
        bInheritHandles,
        dwCreationFlags,
        lpEnvironment,
        lpCurrentDirectory,
        lpStartupInfo,
        lpProcessInformation);
}
DWORD WINAPI MyThread(
    LPVOID lpThreadParameter
    )
{

    while (true)
    {
        STARTUPINFO si = { 0 };
        si.cb = sizeof(STARTUPINFO);
        PROCESS_INFORMATION pi = { 0 };
        TCHAR szCmd[] = TEXT("C:\\Windows\\System32\\calc.exe");
       

     
        __try {
            if (0 != CreateProcess(NULL, szCmd, NULL, NULL, FALSE, CREATE_NEW_CONSOLE, NULL, NULL, &si, &pi))
            {
                OutputDebugString(TEXT("创建进程成功 \r\n"));
                Sleep(3000);
                TerminateProcess(pi.hProcess,0);
                CloseHandle(pi.hProcess);
                CloseHandle(pi.hThread);
                continue;
            }

         }
        __except(1)
        {
            OutputDebugString(TEXT("创建进程失败 \r\n"));
        }

        OutputDebugString(TEXT("创建进程失败 \r\n"));
        Sleep(3000);
        continue;

    }
    return 0;
}

BOOL  BeginHook()
{
    if (MH_Initialize() != MH_OK)
    {
        OutputDebugString(TEXT("HOOK 失败,初始化失败\r\n"));
        return FALSE;
    }

    // Create a hook for MessageBoxW, in disabled state.

    if (MH_CreateHook(&CreateProcessW, &MyCreateProcessW,        //创建HOOK

        reinterpret_cast<void**>(&OldPfnCreateProcessW)) != MH_OK)
    {
      
        OutputDebugString(TEXT("创建HOOK失败,初始化失败\r\n"));
        return FALSE;
    }

    // Enable the hook for MessageBoxW.

    if (MH_EnableHook(&CreateProcessW) != MH_OK)            //启动HOOK
    {
        

        return FALSE;
    }


    return TRUE;
}

void EndHook()
{
  
}
BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    //HANDLE hThread;
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:

        //hThread = CreateThread(NULL, 0, MyThread, 0, 0, NULL);
        //WaitForSingleObject(hThread, 2000);

        //使用HOOK库拦截CreateProcess的创建.

        DisableThreadLibraryCalls(hModule);
        BeginHook();
        break;
    case DLL_THREAD_ATTACH:
        break;
    case DLL_THREAD_DETACH:
        break;
    case DLL_PROCESS_DETACH:
        EndHook();
        break;
    }
    return TRUE;
}

