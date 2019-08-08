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



int _tmain(_In_ int argc, _In_reads_(argc) _Pre_z_ _TCHAR** argv, _In_z_ _TCHAR** envp)
{
    CProcessOpt PsOpt;
    CFileManger FsOpt;
    PsOpt.SeEnbalAdjustPrivileges(SE_DEBUG_NAME);
    HANDLE hProcess = NULL;
    PMEMORY_BASIC_INFORMATION pMemInfo = new MEMORY_BASIC_INFORMATION();
    DWORD dwPid = 27088;
   

    hProcess = PsOpt.PsGetProcess(dwPid); // 根据PROCESS 读取.
    CreateDirectory("D:\\SharMemory", NULL);
    VirtualQueryEx(hProcess, 0, pMemInfo, sizeof(MEMORY_BASIC_INFORMATION));
    for (__int64 i = pMemInfo->RegionSize; i < (i + pMemInfo->RegionSize); i += pMemInfo->RegionSize)
    {

        VirtualQueryEx(hProcess, (LPVOID)i, pMemInfo, sizeof(MEMORY_BASIC_INFORMATION));
        if (pMemInfo->State != MEM_COMMIT)
            continue;
        
        if (pMemInfo->Protect == PAGE_READWRITE || pMemInfo->Protect == PAGE_EXECUTE_READWRITE || pMemInfo->Protect == PAGE_READONLY)
        {
          
            if (pMemInfo->Type == MEM_MAPPED)
            {
                SIZE_T dwReadBytes = 0;
                printf("ImageBase = %p Type = 0x%0x staus = 0x%x Protect = 0x%0x \r\n", pMemInfo->BaseAddress, pMemInfo->Type, pMemInfo->State, pMemInfo->Protect);
                char *szBuffer = new char[pMemInfo->State]();
                ReadProcessMemory(hProcess, pMemInfo->BaseAddress, szBuffer,pMemInfo->State,&dwReadBytes);
                CBinString PathName = "";
                PathName = TEXT("D:\\SharMemory\\");
                PathName = PathName+ PsOpt.PsGetProcessFileNameByProcessId(dwPid);
                PathName = PathName.substr(0, PathName.find_last_of("."));
                PathName += "_";
#ifdef UNICODE
                PathName += std::to_wstring(PsOpt.PsGetProcessIdByProcessFileName(PsOpt.PsGetProcessFileNameByProcessId(dwPid)));
#else
                PathName += std::to_string(PsOpt.PsGetProcessIdByProcessFileName(PsOpt.PsGetProcessFileNameByProcessId(dwPid)));
#endif
                PathName += TEXT("_");
                PathName += TEXT("xxxx");
                PathName += TEXT(".bin");
                
               HANDLE hFile = FsOpt.FsGetFileHandle(PathName.c_str());
               if (INVALID_HANDLE_VALUE == hFile)
               {
                   delete[] szBuffer;
                   continue;
               }
               FsOpt.FsSetFilePoint(hFile, 0, 0, FILE_END);
            
               char szTempString[] = TEXT("====================================================");
               DWORD dwWriteBytes = 0;
               WriteFile(hFile, szTempString, sizeof(szTempString) / sizeof(szTempString[0]), &dwWriteBytes, NULL);
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
}