#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <tchar.h>
#include "CTerminateProcess.h"




int _tmain(_In_ int argc, _In_reads_(argc) _Pre_z_ _TCHAR** argv, _In_z_ char** envp)
{
    LPVOID lpBuffer = NULL;
    HANDLE hProcess = NULL;
    DWORD dwWriteBytes = 0;
    CTerminateProcess CterPro;
    DWORD dwPid = 0;
    TCHAR szPath[1024] = { NULL };



    GetCurrentDirectory(1024 * sizeof(TCHAR), szPath);
    ::lstrcat(szPath, TEXT("\\Inject.dll"));
    CterPro.GetProcessPidByName(TEXT("calc.exe"), dwPid, NULL);

    hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwPid);

  
    if (NULL == hProcess)
        return FALSE;
    lpBuffer = VirtualAllocEx(hProcess, 0, 0x1000, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
    if (NULL == lpBuffer)
        return FALSE;

    WriteProcessMemory(hProcess, lpBuffer, szPath, 1024 * sizeof(TCHAR), &dwWriteBytes);

    HANDLE hThreadHandle = CreateRemoteThread(hProcess,
        NULL,
        0,
        (LPTHREAD_START_ROUTINE)LoadLibrary,
        lpBuffer,
        0,
        NULL);

}
