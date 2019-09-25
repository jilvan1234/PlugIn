
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <tchar.h>
#include "../../publicstruct.h"
#include <TlHelp32.h>

#include "ClassManger/CNativeApi/CNativeApiManger.h"
#include "ProcessManger/ProcessIterator/CProcessOpt.h"
#include "ThreadManger/RemoteThread/CRemoteThread.h"
#include "ThreadManger/ShellCode/ShellCode/CShellCode.h"
#include "FileManger/CFileManger/CFileManger.h"
#include "RegManger/RegManger/CRegManger.h"



#ifdef _WIN64
#ifdef _DEBUG
#pragma comment(lib,"../../../PulicLib/x64/Debug/CNativeApi.lib")
#pragma comment(lib,"../../../PulicLib/x64/Debug/ProcessIterator.lib")
#pragma comment(lib,"../../../PulicLib/x64/Debug/RegManger.lib")
#pragma comment(lib,"../../../PulicLib/x64/Debug/CFileManger.lib")
#pragma comment(lib,"../../../PulicLib/x64/Debug/RemoteThread.lib")
#else
#pragma comment(lib,"../../../PulicLib/x64/Release/CNativeApi.lib")
#pragma comment(lib,"../../../PulicLib/x64/Release/ProcessIterator.lib")
#pragma comment(lib,"../../../PulicLib/x64/Release/RegManger.lib")
#pragma comment(lib,"../../../PulicLib/x64/Release/CFileManger.lib")
#pragma comment(lib,"../../../PulicLib/x64/Release/RemoteThread.lib")
#endif

#else
#ifdef _DEBUG
#pragma comment(lib,"../../../PulicLib/Win32/Debug/CNativeApi.lib")
#pragma comment(lib,"../../../PulicLib/Win32/Debug/ProcessIterator.lib")
#pragma comment(lib,"../../../PulicLib/Win32/Debug/RegManger.lib")
#pragma comment(lib,"../../../PulicLib/Win32/Debug/CFileManger.lib")
#pragma comment(lib,"../../../PulicLib/Win32/Debug/RemoteThread.lib")
#else
#pragma comment(lib,"../../../PulicLib/Win32/Release/CNativeApi.lib")
#pragma comment(lib,"../../../PulicLib/Win32/Release/ProcessIterator.lib")
#pragma comment(lib,"../../../PulicLib/Win32/Release/RegManger.lib")
#pragma comment(lib,"../../../PulicLib/Win32/Release/CFileManger.lib")
#pragma comment(lib,"../../../PulicLib/Win32/Release/RemoteThread.lib")
#endif


//#pragma comment(lib,"../../../HOOK/MinHook/MinHookLib/libMinHook.x86.lib")

#endif



#include <VersionHelpers.h>


//__declspec(naked) void ShellCodeFunction()
//{
//    __asm
//    {
//        pushad
//        pushfd
//        popad
//        popfd
//        ret
//    }
//}
//
// __declspec(naked) void End()
//{
//    
//}
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
        ////22A�޸�Ϊ1����.  Win7 Spack1�� 22A λ������ע��Ҫ�жϵ�Session����.

        //*(char *)((char *)(pAddr)+0x22A) = 1;         //�޸�Sesson �������ע��.
        //VirtualProtect(pAddr, 0x1000, dwOldPro, &dwOldPro);
    
   

    CBinString str = argv[1];
#ifdef UNICODE
    dwPid = _wtoi(str.c_str());
#else
    dwPid = atoi(str.c_str());
#endif // UNICODE
//
//    
//    //dwPid = 508;
//
     GetCurrentDirectory(1024 * sizeof(TCHAR), szPath);
     ::lstrcat(szPath, TEXT("\\"));

     CBinString strPatn = szPath;
     //strPatn += argv[2];
 
    //tsRemote.RtsRemoteThreadLoadLibrary(NULL, dwPid, path.c_str());


   
    //tsRemote.RtsRemoteUnloadModule(dwPid, TEXT("Injec1t.dll"));
    //UCHAR ShellCode[] = "\xc3\xc3";
    //tsRemote.RtsRemoteThreadShellCode(NULL, dwPid, (LPVOID)ShellCodeFunction, (DWORD)ShellCodeFunction - (DWORD)End);
 
     //�������н���ע��
     strPatn += argv[1];


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
     pi.dwSize = sizeof(PROCESSENTRY32); //��һ��ʹ�ñ����ʼ����Ա
     bRet = Process32First(hSnapshot, &pi);
     while (bRet)
     {

         tsRemote.RtsRemoteThreadLoadLibrary(NULL, pi.th32ProcessID, strPatn.c_str()); //ȫϵͳע��

         bRet = Process32Next(hSnapshot, &pi);
     }
     CloseHandle(hSnapshot);


    
   
   // tsRemote.RtsRemoteContextShellCode(hProcess,NULL ,dwTid , (LPVOID)ShellCode, 100);
   //dwPid =  NtUserQueryWindow(hwnd,0);
   //dwTid =  NtUserQueryWindow(hwnd,2);
   // tsRemote.RtsRemoteContextDll(hProcess, NULL, dwTid, TEXT("D:\\123.dll")); //δ���
    CloseHandle(hProcess);
    
}
   
