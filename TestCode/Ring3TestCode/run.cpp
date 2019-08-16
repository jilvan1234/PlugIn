#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <tchar.h>
#include "../../publicstruct.h"


#pragma comment(lib, "urlmon.lib")  

PfnZwQuerySystemInformation ZwQuerySystemInformation;
void InitFunction()
{
    HMODULE hDll = LoadLibrary(TEXT("ntdll.dll"));
    if (NULL == hDll)
    {
        return;
    }
    ZwQuerySystemInformation = reinterpret_cast<PfnZwQuerySystemInformation>(GetProcAddress(hDll, "ZwQuerySystemInformation"));
    if (NULL == ZwQuerySystemInformation)
        return;
}

PVOID RetSystemHandleInformation()
{
    //返回系统句柄信息.
    PULONG szBuffer = NULL;
    DWORD dwSize = 0x1000;
    DWORD dwRetSize = 0;
    NTSTATUS ntStatus;
    szBuffer = new ULONG[dwSize];
    if (NULL == szBuffer)
    {
        return NULL;
    }
    //第一遍调用可能不成功.所以获取返回值. 并且根据DwRetSize的值去重新申请空间
    do 
    {
        ntStatus = ZwQuerySystemInformation(SystemHandleInformation, szBuffer, dwSize, &dwRetSize);
        if (ntStatus == STATUS_INFO_LENGTH_MISMATCH) //代表空间不足.重新申请空间
        {
          
            delete[] szBuffer;
            szBuffer = new ULONG[dwSize *= 2];

        }
        else
        {
            if (!NT_SUCCESS(ntStatus))
            {
                delete [] szBuffer;
                return NULL;
            }
        }
    } while (ntStatus == STATUS_INFO_LENGTH_MISMATCH);
    
    //成功返回信息
    return szBuffer;
}


HANDLE GetFileHandle()
{
    //这里打开自己
    TCHAR tszPath[MAX_PATH] = { 0 };
    GetModuleFileName(NULL,tszPath,MAX_PATH );
    return CreateFile(
        tszPath,
        GENERIC_READ ,
        FILE_SHARE_READ,
        NULL,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,
        NULL);
}
int _tmain(_In_ int argc, _In_reads_(argc) _Pre_z_ _TCHAR** argv, _In_z_ _TCHAR** envp)
{
   
    //getchar();
    //system("pause");

    ////A进程 申请物理页
    //HANDLE hMap = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, 0x1000, TEXT("共享内存"));

    //if (NULL == hMap)
    //    return FALSE;

    ////2.映射虚拟内存(线性地址的)
    //LPVOID lpBuf = MapViewOfFile(hMap, FILE_MAP_ALL_ACCESS, 0, 0, 0x1000);

    //char szBuff[0x100] = "xbbrowser.exe";
    //memcpy(lpBuf, szBuff, 0x100);
    //// A进程写内存

    //printf("映射内存成功%p \r\n",lpBuf);
    //getchar();
    //system("pause");

    //printf("是否开始卸载内存\r\n");
    ////UnmapViewOfFile(lpBuf);
    //system("pause");
    //CloseHandle(hMap);

    //system("pause");

   /*
   1.初始化函数指针
   2.遍历全局句柄表获取信息
   3.打开文件
   4.遍历全局句柄获取信息.
   
   */
    PSYSTEM_HANDLE_INFORMATION pGlobaleHandleInfo = NULL;  //全局句柄信息
    SYSTEM_HANDLE_TABLE_ENTRY_INFO HandleInfor = { 0 }; //句柄的详细信息.
    PVOID pBuffer = NULL;
    HANDLE hFile;
    DWORD dwNumberOfHandles = 0;  //句柄的个数
    InitFunction();
    hFile = GetFileHandle();
    pBuffer =  RetSystemHandleInformation();
    pGlobaleHandleInfo = reinterpret_cast<PSYSTEM_HANDLE_INFORMATION>(pBuffer);
  
    //获得了句柄的相信信息.随便打开个文件句柄.判断类型是否相等.

    dwNumberOfHandles = pGlobaleHandleInfo->NumberOfHandles;
    for (int i = 0; i < dwNumberOfHandles; i++)
    {
        HandleInfor = pGlobaleHandleInfo->Handles[i];//获取句柄的相信信息
        if (GetCurrentProcessId() == HandleInfor.UniqueProcessId)
        {
            if (HandleInfor.HandleValue == (USHORT)hFile)
            {

                printf("hFile对应的文件类型为 %d \r\n", HandleInfor.ObjectTypeIndex);
            }
        }
    }

    system("pause");
    return 0;

}