#pragma once

#include <ntddk.h>
#include <ntstrsafe.h>


#define PROCESS_TERMINATE         (0x0001)  // winnt
#define PROCESS_CREATE_THREAD     (0x0002)  // winnt
#define PROCESS_SET_SESSIONID     (0x0004)  // winnt
#define PROCESS_VM_OPERATION      (0x0008)  // winnt
#define PROCESS_VM_READ           (0x0010)  // winnt
#define PROCESS_VM_WRITE          (0x0020)  // winnt
// begin_ntddk begin_wdm begin_ntifs
#define PROCESS_DUP_HANDLE        (0x0040)  // winnt
// end_ntddk end_wdm end_ntifs
#define PROCESS_CREATE_PROCESS    (0x0080)  // winnt
#define PROCESS_SET_QUOTA         (0x0100)  // winnt
#define PROCESS_SET_INFORMATION   (0x0200)  // winnt
#define PROCESS_QUERY_INFORMATION (0x0400)  // winnt
#define PROCESS_SET_PORT          (0x0800)
#define PROCESS_SUSPEND_RESUME    (0x0800)  // winnt


//安装进程钩子


typedef struct _OPT_PRO
{
    BOOLEAN Pro_Terminate;
    BOOLEAN Pro_CreateThread;
    BOOLEAN Pro_SetSessionId;
    BOOLEAN Pro_VmOperation;
    BOOLEAN Pro_VmRead;
    BOOLEAN Pro_VmWrite;
    BOOLEAN Pro_DupHandle;
    BOOLEAN Pro_CreateProcess;
    BOOLEAN Pro_SetQuota;
    BOOLEAN Pro_SetInformation;
    BOOLEAN Pro_QueryInformation;
    BOOLEAN Pro_SetPort;
    BOOLEAN Pro_Suspend_Resume;
    const char  * pszName;
}OPT_PRO,*POPT_PRO;



NTSTATUS InstallObjectProtecteHOOk();
//卸载进程钩子
NTSTATUS UnstallObjectProtecteHook();

//初始化要进行保护的选项.一定要设置ImageName
VOID SetProTerminate(BOOLEAN IsSet);
VOID SetProCreateThread(BOOLEAN IsSet);
VOID SetProVmOperation(BOOLEAN IsSet);
VOID SetProSessionId(BOOLEAN IsSet);
VOID SetProVmRead(BOOLEAN IsSet);
VOID SetProVmWrite(BOOLEAN IsSet);
VOID SetProDupHandle(BOOLEAN IsSet);
VOID SetProCreateProcess(BOOLEAN IsSet);
VOID SetProSetQuota(BOOLEAN IsSet);
VOID SetProSetInformation(BOOLEAN IsSet);
VOID SetProQueryinformation(BOOLEAN IsSet);
VOID SetProSetPort(BOOLEAN IsSet);
VOID SetProSuspend_Resume(BOOLEAN IsSet);
VOID SetProImageName(const char *pszName);


//
NTKERNELAPI UCHAR * PsGetProcessImageFileName(__in PEPROCESS Process);

NTSTATUS AllocateOptMem();
NTSTATUS UnAllocateOptMem();