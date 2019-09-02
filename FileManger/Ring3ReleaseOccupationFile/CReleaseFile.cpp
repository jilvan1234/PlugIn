#include "CReleaseFile.h"
#include "../../publicstruct.h"
#include "../../FileManger/CFileManger/CFileManger.h"



CReleaseFile::CReleaseFile()
{
    GetNtQuerySystemInfomation();
}


CReleaseFile::~CReleaseFile()
{
}

PfnNtQuerySystemInformation CReleaseFile::GetNtQuerySystemInfomation()
{
    HINSTANCE ntdll_dll = GetModuleHandle(TEXT("ntdll.dll"));

    if (ntdll_dll == NULL)
        return NULL;
  
    NtQuerySystemInformation = (PfnNtQuerySystemInformation)GetProcAddress(ntdll_dll, "NtQuerySystemInformation");
    if (NULL == NtQuerySystemInformation)
        return NULL;
     
    return NtQuerySystemInformation;
}

PSYSTEM_PROCESSES CReleaseFile::GetProcessInfo()
{
    PSYSTEM_PROCESSES pspInfo = NULL ;
    PVOID pspBuffer = NULL;
    NTSTATUS ntStatus;
    DWORD dwSize = 0;
    ntStatus = NtQuerySystemInformation(SystemProcessesAndThreadsInformation, NULL, 0, &dwSize);
    if (STATUS_INFO_LENGTH_MISMATCH == ntStatus)
    {
        pspBuffer = new char[dwSize];
        if (NULL == pspBuffer)
            return NULL;
        ntStatus = NtQuerySystemInformation(SystemProcessesAndThreadsInformation, pspBuffer, dwSize, NULL);
        if (!NT_SUCCESS(ntStatus))
            return NULL;
        pspInfo = static_cast<PSYSTEM_PROCESSES>(pspBuffer);
    }

  /* if (NULL != pspInfo)
    {
        do 
        {
            pspInfo = (PSYSTEM_PROCESSES)((ULONG64)(pspInfo) + pspInfo->NextEntryDelta);
        } while (pspInfo->NextEntryDelta != 0);
    }*/
    return pspInfo;
}

//获取句柄表的基础信息
PSYSTEM_HANDLE_INFORMATION CReleaseFile::GetHandleBasicInfo()
{
    NTSTATUS ntStatus;
    DWORD dwSize = 0x1000;
    PVOID HandleBuffer = NULL;
    PSYSTEM_HANDLE_INFORMATION pHndleInfo;
    HandleBuffer = new char[dwSize];
    ntStatus = NtQuerySystemInformation(SystemHandleInformation, HandleBuffer, dwSize, &dwSize);
    if (STATUS_INFO_LENGTH_MISMATCH == ntStatus)
    {
        delete[] HandleBuffer;
        HandleBuffer = new char[dwSize];
        if (NULL == HandleBuffer)
            return NULL;
        ntStatus = NtQuerySystemInformation(SystemHandleInformation, HandleBuffer, dwSize, NULL);
        if (!NT_SUCCESS(ntStatus))
        {
            delete[] HandleBuffer;
            return NULL;
        }
        pHndleInfo = static_cast<PSYSTEM_HANDLE_INFORMATION>(HandleBuffer);
    }
  

    return pHndleInfo;
}

//强制删除文件句柄.
BOOL CReleaseFile::RemoteFileByHandle(MYISTRING fileName, HANDLE hProcess, HANDLE Handle)
{

    HANDLE hTarHandle;
    if (DuplicateHandle(          //拷贝文件句柄
        hProcess,
        Handle, 
        GetCurrentProcess(),
        &hTarHandle, 
        0, 
        FALSE, DUPLICATE_SAME_ACCESS));
    {

    }
    return 0;
}

BOOL CReleaseFile::CloseHandleForFile(MYISTRING fileName)
{
    PSYSTEM_HANDLE_INFORMATION pHndleBasicInfo = NULL;
    SYSTEM_HANDLE_TABLE_ENTRY_INFO HandleTableInfo = { NULL };//句柄值的详细信息.
    HANDLE hFile = INVALID_HANDLE_VALUE;
    ULONG uHandleCount = 0;
    DWORD ModuleFileNameSize = 0x200;
    TCHAR ModuleFileName[0x200];
    DWORD dwHandleType = 0;
    HANDLE hProcess = NULL;
    RtlZeroMemory(ModuleFileName, sizeof(0x200 * sizeof(TCHAR)));

    GetModuleFileName(NULL, ModuleFileName, ModuleFileNameSize);
    hFile = CreateFile(
        ModuleFileName,
        GENERIC_READ,
        FILE_SHARE_READ,
        0,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,
        0);
   


    //遍历进程.找到所有跟这个句柄类型相似的类型.
    for (int i = 0; i < uHandleCount; i++)
    {
        HandleTableInfo = pHndleBasicInfo->Handles[i];
        if (HandleTableInfo.ObjectTypeIndex != dwHandleType)
            continue;
        //找到了对应的文件类型
        hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, HandleTableInfo.UniqueProcessId);
        if (hProcess)
        {
            //如果成功.关闭文件句柄.
            RemoteFileByHandle(fileName, hProcess, (HANDLE)HandleTableInfo.HandleValue);
        }
    }
    return 0;
}
