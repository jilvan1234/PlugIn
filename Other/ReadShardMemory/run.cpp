/*
1.遍历所有进程
2.遍历所有进程内存
3.找出MAP类型内存.
    4.输出映射地址, 属性. 以及地址里面的内容.

*/

#include <stdio.h>
#include <stdlib.h>
#include <tchar.h>
#include "CProcessOpt.h"
#include "CFileManger.h"
#include <math.h>
#include <publicstruct.h>
#include <Psapi.h>
CProcessOpt g_PsOpt;




//遍历进程下的模块信息需要用到.
PfnZwQueryInformationProcess ZwQueryInformationProcess = reinterpret_cast<PfnZwQueryInformationProcess>(g_PsOpt.MmGetAddress(TEXT("ntdll.dll"), "ZwQueryInformationProcess"));
//根据进程句柄获取名字信息.
PfnZwQueryObject g_NtQueryObject = reinterpret_cast<PfnZwQueryObject>(g_PsOpt.MmGetAddress(TEXT("ntdll.dll"), "NtQueryObject"));

PfnZwQuerySystemInformation ZwQuerySystemInformation = reinterpret_cast<PfnZwQuerySystemInformation>(g_PsOpt.MmGetAddress(TEXT("ntdll.dll"),"ZwQuerySystemInformation"));

//根据句柄获取文件名路径.
PfnNtQueryInformationFile ZwQueryInformationFile = reinterpret_cast<PfnNtQueryInformationFile>(g_PsOpt.MmGetAddress(TEXT("ntdll.dll"), "ZwQueryInformationFile"));
// 拼接路径,创建文件夹.
CBinString SplicingPathFileName(CBinString DirPath, CProcessOpt &PsOpt, DWORD dwPid, PMEMORY_BASIC_INFORMATION pMemInfo)
{
    CBinString PathName = TEXT("");
    CBinString PmProtect = TEXT("");
    PathName = DirPath;

    PathName = PathName + PsOpt.PsGetProcessFileNameByProcessId(dwPid);

    PathName = PathName.substr(0, PathName.find_last_of(TEXT(".")));
    PathName += TEXT("_";)
#ifdef UNICODE
    PathName += std::to_wstring(PsOpt.PsGetProcessIdByProcessFileName(PsOpt.PsGetProcessFileNameByProcessId(dwPid)));
#else
    PathName += std::to_string(PsOpt.PsGetProcessIdByProcessFileName(PsOpt.PsGetProcessFileNameByProcessId(dwPid)));
#endif
    PathName += TEXT("_");

    //地址转为16进制.
    TCHAR HexBuffer[0x100] = { 0 };
#ifdef UNICODE
    wsprintf(HexBuffer, TEXT("%P"), pMemInfo->BaseAddress);
#else
    sprintf_s(HexBuffer, 0x100, "%p", pMemInfo->BaseAddress);
#endif
   
    PathName += HexBuffer;

    PathName += TEXT("_");
    // 写成内存属性

    switch (pMemInfo->Protect) //根据类型比较
    {
    case PAGE_EXECUTE: PmProtect = TEXT("PAGE_EXECUTE"); break;
    case PAGE_EXECUTE_READ:PmProtect = TEXT("PAGE_EXECUTE_READ"); break;
    case PAGE_EXECUTE_READWRITE: PmProtect = TEXT("PAGE_EXECUTE_READWRITE"); break;
    case PAGE_EXECUTE_WRITECOPY: PmProtect = TEXT("PAGE_EXECUTE_WRITECOPY"); break;
    case PAGE_NOACCESS:PmProtect = TEXT("PAGE_NOACCESS"); break;
    case PAGE_READONLY: PmProtect = TEXT("PAGE_READONLY"); break;
    case PAGE_READWRITE:PmProtect = TEXT("PAGE_READWRITE"); break;
    case PAGE_WRITECOPY: PmProtect =TEXT ("PAGE_WRITECOPY"); break;
    case PAGE_TARGETS_INVALID: PmProtect =TEXT( "PAGE_TARGETS_INVALID"); break;
    default: PmProtect = TEXT("ERROR_PROTECT"); break;
    }

    PathName += PmProtect;
    PathName += TEXT("_");
    PathName += TEXT("Handle");
    PathName += TEXT(".bin");
    return CBinString(PathName);
}

CBinString CreateDirPathName(DWORD dwPid)
{
    CFileManger FsOpt;
    CProcessOpt PsOpt;
    CBinString DirPath = TEXT("D:\\SharMemory");
    FsOpt.FsRemoveDirector(DirPath);             //每次运行之前先删除.
    FsOpt.FsCreateDirector(DirPath);
    DirPath += TEXT("\\");

#ifdef UNICODE
    DirPath += std::to_wstring(dwPid);

#else
    DirPath += std::to_string(dwPid);
#endif // UNICODE

    FsOpt.FsCreateDirector(DirPath);
    DirPath += TEXT("\\");	
    return CBinString(DirPath);
}

//写入固定格式表头
BOOL WriteListHead(HANDLE hFile ,PMEMORY_BASIC_INFORMATION pMemInfo)
{
#define  BUFFER_SIZE 0X100
    CFileManger FsOpt;
    char szTempString[BUFFER_SIZE];
    memset(szTempString, 0, BUFFER_SIZE);
    DWORD dwWriteBytes = 0;
    string PmProtect = "";
    string PmState = "";
    string PmType = "";
    //内存的保护属性
    switch (pMemInfo->Protect) //根据类型比较
    {
    case PAGE_EXECUTE: PmProtect = "PAGE_EXECUTE"; break;
    case PAGE_EXECUTE_READ:PmProtect = "PAGE_EXECUTE_READ"; break;
    case PAGE_EXECUTE_READWRITE: PmProtect = "PAGE_EXECUTE_READWRITE";break;
    case PAGE_EXECUTE_WRITECOPY: PmProtect = "PAGE_EXECUTE_WRITECOPY"; break;
    case PAGE_NOACCESS:PmProtect = "PAGE_NOACCESS";break;
    case PAGE_READONLY: PmProtect = "PAGE_READONLY"; break;
    case PAGE_READWRITE:PmProtect = "PAGE_READWRITE";break;
    case PAGE_WRITECOPY: PmProtect = "PAGE_WRITECOPY"; break;
    case PAGE_TARGETS_INVALID: PmProtect = "PAGE_TARGETS_INVALID";break;
    default: PmProtect = "ERROR_PROTECT"; break;
    }
    //内存的状态
    switch (pMemInfo->State)
    {
    case MEM_COMMIT: PmState = "MEM_COMMIT"; break;
    case MEM_FREE: PmState = ""; break;
    case MEM_RELEASE:PmState = "";break;
    default: PmState = "ERROR_STATE";break;
    }

    //内存类型.
    switch (pMemInfo->Type)
    {
    case MEM_IMAGE: PmType = "MEM_IMAGE"; break;
    case MEM_MAPPED: PmType = "MEM_MAPPED"; break;
    case MEM_PRIVATE: PmType = "MEM_PRIVATE";break;
    default: PmType = "ERROR_MmType"; break;
    }
    sprintf_s(szTempString, BUFFER_SIZE, "type = %s staus = %s protect = %s \r\n", PmType.c_str(), PmState.c_str(), PmProtect.c_str());

    return FsOpt.FsWriteFile(hFile, szTempString, sizeof(szTempString) / sizeof(szTempString[0]), &dwWriteBytes, NULL);
    
}

//根据Handle获取文件类型.
DWORD  WINAPI GetFileName(LPVOID lParam )
{



    HANDLE handle = (HANDLE)lParam;
    CFileManger fsOpt;
    IO_STATUS_BLOCK iostu;
    wstring StrName;
    TCHAR PathBuffer[0x200];
    memset(PathBuffer, 0, 0x200);
    char *pBuffer = new char[0x256]();
 
    ZwQueryInformationFile((HANDLE)handle, &iostu, pBuffer, 0x256, RFILE_INFORMATION_CLASS::FileNameInformation);
    PFILE_NAME_INFORMATION pFileInfo = reinterpret_cast<PFILE_NAME_INFORMATION>(pBuffer);
    
    
    memcpy(PathBuffer, pFileInfo->FileName, pFileInfo->FileNameLength);
    StrName = PathBuffer;
    printf("句柄对应路径 = %ws \r\n", pFileInfo->FileName);
    
    //delete[] pBuffer;

    return NULL;
}
int _tmain(_In_ int argc, _In_reads_(argc) _Pre_z_ _TCHAR** argv, _In_z_ _TCHAR** envp)
{
    CProcessOpt PsOpt;
    CFileManger FsOpt;
    PsOpt.SeEnbalAdjustPrivileges(SE_DEBUG_NAME);
    HANDLE hProcess = NULL;
    PMEMORY_BASIC_INFORMATION pMemInfo = new MEMORY_BASIC_INFORMATION();
    SIZE_T dwErrorCode = 0;
    SIZE_T AddressHex = 0;
   
    DWORD dwPid = 1288;
    PSYSTEM_PROCESSES psp = NULL;
    CBinString PathName = TEXT("");
    CBinString DirPath = TEXT("");

    PVOID handlecount;
    DWORD64 handle = 0;
  
    DirPath = CreateDirPathName(dwPid);

    hProcess = PsOpt.PsGetProcess(dwPid); // 根据PROCESS 读取.

    PsOpt.PsSusPendProcess(dwPid);//挂起
    
    ZwQueryInformationProcess(hProcess, ProcessHandleCount, &handlecount, sizeof(handlecount), NULL); //hc == 句柄个数
    HANDLE hTarHandle;
    for (DWORD64 i = 0; i < 400000; i+=4)
    {

        if (1 == DuplicateHandle(hProcess, (HANDLE)i, GetCurrentProcess(), &hTarHandle, 0, 0, DUPLICATE_SAME_ACCESS))
        {
            //使用NtQueryObject遍历这个句柄信息
            ULONG uRetLength = 0;
            char * Buffer = new char[0x100]();
            OBJECT_INFORMATION_CLASS ob = ObjectTypeInformation;
            g_NtQueryObject(hTarHandle, ob, Buffer, 0x100, &uRetLength);
            PPUBLIC_OBJECT_TYPE_INFORMATION pBuffer = reinterpret_cast<PPUBLIC_OBJECT_TYPE_INFORMATION>(Buffer);  //查询类型信息
            printf("句柄名字为: %ws \t      句柄类型为: %d \t \r\n", pBuffer->TypeName.Buffer, pBuffer->MaintainTypeList);
            ////传入句柄类型的值.获取对应的名字
            //if (pBuffer->MaintainTypeList == 36)
            //{

            //    //开线程,读取.会阻塞.
            //
            //    HANDLE hThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)GetFileName,hTarHandle, 0, 0);
            //    //GetFileName(pBuffer->MaintainTypeList,(HANDLE)hTarHandle);
            //    WaitForSingleObject(hThread, 1000);
            // 
            //    continue;
            //}
          

       }
       
          
            
            

            int a = 10;
         
    }

    system("pause");
    return 0;
    dwErrorCode  = VirtualQueryEx(hProcess, 0, pMemInfo, sizeof(MEMORY_BASIC_INFORMATION));
    if (0 == dwErrorCode)
        return 0;


    for (__int64 i = pMemInfo->RegionSize; i < (i + pMemInfo->RegionSize); i += pMemInfo->RegionSize)
    {

        dwErrorCode = VirtualQueryEx(hProcess, (LPVOID)i, pMemInfo, sizeof(MEMORY_BASIC_INFORMATION));
        if (0 == dwErrorCode)
            break;

        if (pMemInfo->State != MEM_COMMIT)
            continue;

        if (pMemInfo->Protect == PAGE_READWRITE || pMemInfo->Protect == PAGE_EXECUTE_READWRITE || pMemInfo->Protect == PAGE_READONLY)
        {

            if (pMemInfo->Type == MEM_MAPPED)
            {
                SIZE_T dwReadBytes = 0;
                printf("ImageBase = %p Type = 0x%0x staus = 0x%x Protect = 0x%0x \r\n", pMemInfo->BaseAddress, pMemInfo->Type, pMemInfo->State, pMemInfo->Protect);
                char *szBuffer = new char[pMemInfo->State]();
                ReadProcessMemory(hProcess, pMemInfo->BaseAddress, szBuffer, pMemInfo->State, &dwReadBytes);

                PathName = SplicingPathFileName(DirPath, PsOpt, dwPid, pMemInfo);


                HANDLE hFile = FsOpt.FsGetFileHandle(PathName.c_str(), GENERIC_READ | GENERIC_WRITE,FILE_SHARE_READ | FILE_SHARE_WRITE,0, CREATE_ALWAYS,NULL,0);
                if (INVALID_HANDLE_VALUE == hFile)
                {
                    delete[] szBuffer;
                    continue;
                }
                FsOpt.FsSetFilePoint(hFile, 0, 0, FILE_END);

                /*
                
                写入内存type
                写入内存state.
                写入内存protect
                写入内存OldProtect
                写入表头
                */
                WriteListHead(hFile,pMemInfo);

                DWORD dwWriteBytes;
                SetFilePointer(hFile, 0, 0, FILE_END);
                WriteFile(hFile, szBuffer, pMemInfo->State, &dwWriteBytes, NULL);
                CloseHandle(hFile);
                delete[] szBuffer;
                
            }
        }
        continue;

    }


   

    CloseHandle(hProcess);
        
    system("pause");
    return 0;
}