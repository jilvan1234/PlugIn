#pragma once
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <tchar.h>
#include "Hook.h"

typedef BOOL (*_cdecl PfnHook)();


/*
�����߳�.�ȴ�������. �յ�֪ͨ.���н���.
*/
HANDLE g_hEvent = CreateEvent(NULL, TRUE, FALSE, TEXT("WhCnm"));
DWORD WINAPI MyGetStatus()
{
  
    WaitForSingleObject(g_hEvent, INFINITE); 

    Sleep(3000);
    ExitProcess(0);

    return 0;
}
int _tmain(int argc, _TCHAR *argv)
{
    TCHAR szPath[1024] = { NULL };
    GetCurrentDirectory(1024 * sizeof(TCHAR), szPath);
    ::lstrcat(szPath, TEXT("\\Tv_x86.dll"));
    HMODULE hModule = ::LoadLibrary(szPath);
    if (NULL == hModule)
        return FALSE;
    PfnHook SetHook = reinterpret_cast<PfnHook>(GetProcAddress(hModule, TEXT("Hook")));
    if (NULL == SetHook)
        return FALSE;

    SetHook();
    
    HANDLE hThread =  CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)MyGetStatus, NULL, 0, nullptr);
    WaitForSingleObject(hThread, INFINITE); //�ȴ��߳��˳�. �̵߳ȴ��¼��˳�.
   
    
    //�����̶߳�ȡ.
    system("pause");
    return 0;
}