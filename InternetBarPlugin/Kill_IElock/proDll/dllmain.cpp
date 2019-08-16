// dllmain.cpp : 定义 DLL 应用程序的入口点。
#include "stdafx.h"


#include <TlHelp32.h>
#include <vector>
#include "../../../publicstruct.h"
#include "../../../ProcessManger/ProcessIterator/CProcessOpt.h"

using namespace std;
BOOL ItatorProcess();
BOOL IteratorModel(DWORD dwPid);

vector<DWORD> g_SaveIelockPid;
HANDLE g_Event = CreateEvent(NULL, FALSE, TRUE, NULL);
DWORD WINAPI StartAddress(
    LPVOID lpThreadParameter
)
{
    while (TRUE)
    {
        WaitForSingleObject(g_Event, INFINITE);
        if (ItatorProcess() == TRUE)
        {
            SetEvent(g_Event); //设置为有信号
            Sleep(2000);
        }
       
        SetEvent(g_Event); //设置为有信号

        Sleep(2000);       //降低CPU延时.

    }
}

BOOL IteratorModel(DWORD dwPid)
{
    DWORD dwErrorCode = 0;
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, dwPid);
    if (INVALID_HANDLE_VALUE == hSnapshot)
    {
        dwErrorCode = GetLastError();
        CloseHandle(hSnapshot);
        return dwErrorCode;
    }
    MODULEENTRY32 mi;
    mi.dwSize = sizeof(MODULEENTRY32); //第一次使用必须初始化成员
    BOOL  bRet = Module32First(hSnapshot, &mi);

    CBinString MyModelString = TEXT("");
    CBinString MyFindCompareString = TEXT("IELock.dll"); //转为小写.

    transform(MyFindCompareString.begin(), MyFindCompareString.end(), MyFindCompareString.begin(), ::tolower);
    while (bRet)
    {

        MyModelString = mi.szModule;
        transform(MyModelString.begin(), MyModelString.end(), MyModelString.begin(), ::tolower);
        if (MyFindCompareString.compare(MyModelString.substr(MyModelString.find_last_of(TEXT("_")) + 1)) == 0)
        {
            CloseHandle(hSnapshot);

            return TRUE;
        }


        bRet = Module32Next(hSnapshot, &mi);
    }

    CloseHandle(hSnapshot);

    return FALSE;
}
BOOL ItatorProcess()
{
    CProcessOpt psOpt;
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    DWORD dwErrCode = 0;
    BOOL bRet = FALSE;

    if (INVALID_HANDLE_VALUE == hSnapshot)
    {
        dwErrCode = GetLastError();
        CloseHandle(hSnapshot);
        return NULL;
    }

    PROCESSENTRY32 pi = { 0 };
    pi.dwSize = sizeof(PROCESSENTRY32); //第一次使用必须初始化成员
    bRet = Process32First(hSnapshot, &pi);
    while (bRet)
    {

        //传入进程PID 遍历所有模块.
        if (TRUE == IteratorModel(pi.th32ProcessID))
        {
            //挂起进程.
            psOpt.PsSusPendProcess(pi.th32ProcessID);
            CloseHandle(hSnapshot);
            return TRUE;
        }

        bRet = Process32Next(hSnapshot, &pi);
    }
    CloseHandle(hSnapshot);

    return FALSE;
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
       
        OutputDebugString(TEXT("111"));
        hThread =   CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)StartAddress, NULL, 0, 0);
        WaitForSingleObject(hThread, 3000);
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

