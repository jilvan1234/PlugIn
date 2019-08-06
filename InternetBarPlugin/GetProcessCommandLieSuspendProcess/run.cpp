#include "CTerminateProcess.h"
#include <tchar.h>


//HANDLE g_Event = CreateEvent(NULL, FALSE, TRUE,NULL);

 DWORD WINAPI MyThread(
    LPVOID lpThreadParameter
    )
{

     
     CTerminateProcess ter;
     vector<DWORD> dwpid;
     DWORD dwErrorCode = 0;
     DWORD dwCount = 0;
     while (true)
     {
         //WaitForSingleObject(g_Event, INFINITE);
         ter.Svchost(dwpid);

         vector<DWORD>dwChildPid;
         ter.iterSvchostChild(dwpid[0], dwChildPid); //第一次遍历.
        
         for (int i = 0; i < dwChildPid.size(); i++)
         {
             dwErrorCode = ter.iterModuleSuspend(dwChildPid[i]); //第一个不挂起
             if (dwErrorCode == 5)
             {
                 //挂起这个进程
                 ++dwCount; //第一次不修改
                 if (dwCount == 1)
                 {
                     continue;
                 }
                 ter.EnableSuspendAnResumeProcess(dwChildPid[i], TRUE, FALSE);
                 dwCount = 0;
             }

         }
         Sleep(2000);
         //SetEvent(g_Event); //设置为有信号.
     }
    
}
int _tmain(_In_ int argc, _In_reads_(argc) _Pre_z_ char** argv, _In_z_ char** envp)
{
   
    HANDLE hThread = NULL;
    hThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)MyThread, 0, 0, 0);
    WaitForSingleObject(hThread, INFINITE);
}