#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <tchar.h>


int _tmain(_In_ int argc, _In_reads_(argc) _Pre_z_ _TCHAR** argv, _In_z_ _TCHAR** envp)
{
   
    getchar();
    system("pause");

    //A进程 申请物理页
    HANDLE hMap = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, 0x1000, TEXT("共享内存"));

    if (NULL == hMap)
        return FALSE;

    //2.映射虚拟内存(线性地址的)
    LPVOID lpBuf = MapViewOfFile(hMap, FILE_MAP_ALL_ACCESS, 0, 0, 0x1000);

    // A进程写内存

    printf("映射内存成功%p \r\n",lpBuf);
    getchar();
    system("pause");

    printf("是否开始卸载内存\r\n");
    //UnmapViewOfFile(lpBuf);
    system("pause");
    CloseHandle(hMap);

    system("pause");

    return 0;

}