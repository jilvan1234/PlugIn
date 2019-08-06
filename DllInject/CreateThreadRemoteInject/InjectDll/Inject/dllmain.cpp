// dllmain.cpp : 定义 DLL 应用程序的入口点。
#include "stdafx.h"


DWORD WINAPI MyThread(
    LPVOID lpThreadParameter
    )
{

    while (true)
    {
        OutputDebugString(TEXT("线程线程\r\n"));
        Sleep(1000);
    }
}

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    HANDLE hThread;
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:

        hThread = CreateThread(NULL, 0, MyThread, 0, 0, NULL);
        WaitForSingleObject(hThread, 2000);

     
        break;
    case DLL_THREAD_ATTACH:
        break;
    case DLL_THREAD_DETACH:
        break;
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

