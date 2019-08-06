#include "CProcessOpt.h"
#include<stdio.h>
#include <tchar.h>



bool AdjustPrivileges() {
    HANDLE hToken = NULL;
    TOKEN_PRIVILEGES tp;
    TOKEN_PRIVILEGES oldtp;
    DWORD dwSize = sizeof(TOKEN_PRIVILEGES);
    LUID luid;

    OpenProcessToken(GetCurrentProcess(), TOKEN_ALL_ACCESS, &hToken);


    if (!LookupPrivilegeValue(NULL, SE_DEBUG_NAME, &luid)) {
        CloseHandle(hToken);
        OutputDebugString(TEXT("提升权限失败,LookupPrivilegeValue"));
        return false;
    }
    ZeroMemory(&tp, sizeof(tp));
    tp.PrivilegeCount = 1;
    tp.Privileges[0].Luid = luid;
    tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
    /* Adjust Token Privileges */
    if (!AdjustTokenPrivileges(hToken, FALSE, &tp, sizeof(TOKEN_PRIVILEGES), &oldtp, &dwSize)) {
        CloseHandle(hToken);
        OutputDebugString(TEXT("提升权限失败 AdjustTokenPrivileges"));
        return false;
    }
    // close handles
    CloseHandle(hToken);
    return true;
}

int _tmain(_In_ int argc, _In_reads_(argc) _Pre_z_ _TCHAR** argv, _In_z_ _TCHAR** envp)
{
    CProcessOpt opt;
    HANDLE hProcess = NULL;
   // hProcess =  opt.PsGetProcess(9404);
    BYTE *ModelBase = NULL;
    BOOL bRet = FALSE;
   // bRet =  opt.MdGetProcessModel(9404,&ModelBase);//获取模块基址
    //opt.TsShowAllIteratorThread(12780);
    AdjustPrivileges();
   // system("pause");
    //opt.PsTerminateProcess(hProcess, 7456);
    //printf("Errord Code %d \r\n " , GetLastError());
    char szBuffer[0x1000] = { 0 };
    size_t dwWriteBytes = 0;
    HMODULE hMod = GetModuleHandleA("ntdll.dll");
    DWORD dwOldPro;
   // system("pause");
    //opt.PsTerminateProcessTheRemoteExitThread(10892);
    //opt.PsTerminateProcessTheZeroMemory(10892);
    //opt.PsTerminateProcessTheThreadContext(10508);
   // opt.PsTerminateProcessTheRemoteExitProcess(6236);

    
    system("pause");
    //opt.PsTerminateProcessTheRemoteExitThread(6236);
    opt.PsTerminateProcessTheRemoteExitProcess(6236);
    system("pause");

    //OpenThread(THREAD_ALL_ACCESS, FALSE, );
    for (int i = 1;  i < 0x10000000; i += 1)
    {
        bRet = VirtualProtectEx(hProcess, (PVOID)((DWORD64)ModelBase + 0x1000 * i), 0x1000, PAGE_EXECUTE_READWRITE, &dwOldPro);
        if (bRet != 0)
        {
            printf("Addr = %p Change Address = %p \r\n", ModelBase, ModelBase + 0x1000 * i);

            //WriteProcessMemory(hProcess, (PVOID)(ModelBase + 0x1000 * i), szBuffer, 0x1000,&dwWriteBytes);
            //printf("Error = %d \r\n", GetLastError());
        }
        printf("Error = %d \r\n", GetLastError());
       /* if (GetLastError() == 998)
            continue;*/
       
        //BOOL bRet = WriteProcessMemory(hProcess, (PVOID)(0x1000 * i), szBuffer, 0x1000, &dwWriteBytes);
        //printf("Errord Code = %d \r\n", GetLastError());
        //system("pause");
    }
   

    system("pause");
}