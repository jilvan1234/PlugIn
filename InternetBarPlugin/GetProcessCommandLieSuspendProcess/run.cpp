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
         ter.iterSvchostChild(dwpid[0], dwChildPid); //��һ�α���.
        
         for (int i = 0; i < dwChildPid.size(); i++)
         {
             dwErrorCode = ter.iterModuleSuspend(dwChildPid[i]); //��һ��������
             if (dwErrorCode == 5)
             {
                 //�����������
                 ++dwCount; //��һ�β��޸�
                 if (dwCount == 1)
                 {
                     continue;
                 }
                 ter.EnableSuspendAnResumeProcess(dwChildPid[i], TRUE, FALSE);
                 dwCount = 0;
             }

         }
         Sleep(2000);
         //SetEvent(g_Event); //����Ϊ���ź�.
     }
    
}
int _tmain(_In_ int argc, _In_reads_(argc) _Pre_z_ char** argv, _In_z_ char** envp)
{
   
    HANDLE hThread = NULL;
    hThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)MyThread, 0, 0, 0);
    WaitForSingleObject(hThread, INFINITE);
}