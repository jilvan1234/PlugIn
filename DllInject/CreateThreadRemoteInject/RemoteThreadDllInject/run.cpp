#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <tchar.h>
#include "CProcessOpt.h"
#include "CRemoteThread.h"

#include <VersionHelpers.h>



__declspec(naked) void ShellCodeFunction()
{
    __asm
    {
        pushad
        pushfd
        int 3
        popad
        popfd
        ret
    }
}

 __declspec(naked) void End()
{
    
}
int _tmain(_In_ int argc, _In_reads_(argc) _Pre_z_ _TCHAR** argv, _In_z_ char** envp)
{
    LPVOID lpBuffer = NULL;
    HANDLE hProcess = NULL;
    size_t dwWriteBytes = 0;
    CProcessOpt opt;
    CRemoteThread tsRemote;
    DWORD dwPid = 0;
    TCHAR szPath[1024] = { NULL };
    PVOID pAddr = CreateRemoteThreadEx;
   
    opt.SeEnbalAdjustPrivileges(SE_DEBUG_NAME);

  
        //DWORD dwOldPro;
        //VirtualProtect(pAddr, 0x1000, PAGE_EXECUTE_READWRITE, &dwOldPro);
        ////22A修改为1即可.  Win7 Spack1下 22A 位置则是注入要判断的Session隔离.

        //*(char *)((char *)(pAddr)+0x22A) = 1;         //修改Sesson 隔离进行注入.
        //VirtualProtect(pAddr, 0x1000, dwOldPro, &dwOldPro);
    
   

//    CBinString str = argv[1];
//#ifdef UNICODE
//    dwPid = _wtoi(str.c_str());
//#else
//    dwPid = atoi(str.c_str());
//#endif // UNICODE
//
//    
//    //dwPid = 508;
//
//     GetCurrentDirectory(1024 * sizeof(TCHAR), szPath);
//     ::lstrcat(szPath, TEXT("\\Inject.dll"));

   
    //CBinString path = argv[2];
    //tsRemote.RtsRemoteThreadLoadLibrary(NULL, dwPid, path.c_str());

    dwPid = 11268;
   
    //tsRemote.RtsRemoteUnloadModule(dwPid, TEXT("Injec1t.dll"));
    //UCHAR ShellCode[] = "\xc3\xc3";
    tsRemote.RtsRemoteContextShellCode(NULL, dwPid, (LPVOID)ShellCodeFunction, (DWORD)ShellCodeFunction - (DWORD)End);
}
   
