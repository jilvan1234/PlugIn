// NoModelDll.cpp : 定义 DLL 应用程序的导出函数。
//

#include "stdafx.h"
#include <stdlib.h>
#include <stdio.h>
#include "../../../HOOK/MinHook/MinHookInclude/MinHook.h"  //挂钩CreateProcess拦截以 挂起进程注入的进程
#include "NoModelDll.h"


#ifdef _WIN64
#pragma comment(lib,"../../../HOOK/MinHook/MinHookLib/libMinHook.x64.lib")
#else
#pragma comment(lib,"../../../HOOK/MinHook/MinHookLib/libMinHook.x86.lib")
#endif


PFNCREATEPROCESS pfnTarCreateProcess;  //函数指针
PFNCREATEPROCESSA pfnTarCreateProcessA;

BOOL WINAPI MyCreateProcessA(_In_opt_ LPCSTR lpApplicationName, _Inout_opt_ LPSTR lpCommandLine, _In_opt_ LPSECURITY_ATTRIBUTES lpProcessAttributes, _In_opt_ LPSECURITY_ATTRIBUTES lpThreadAttributes, _In_ BOOL bInheritHandles, _In_ DWORD dwCreationFlags, _In_opt_ LPVOID lpEnvironment, _In_opt_ LPCSTR lpCurrentDirectory, _In_ LPSTARTUPINFOA lpStartupInfo, _Out_ LPPROCESS_INFORMATION lpProcessInformation)
{


    char OutPutBuf[0x200] = { 0 };
    RtlZeroMemory(OutPutBuf, sizeof(char) * 0x200);
    sprintf(OutPutBuf, "A进程名称 = %s", lpApplicationName);
    OutputDebugStringA(lpApplicationName);

    RtlZeroMemory(OutPutBuf, sizeof(TCHAR) * 0x200);
    sprintf(OutPutBuf, "A命令行 = %s", lpCommandLine);
    OutputDebugStringA(OutPutBuf);


    RtlZeroMemory(OutPutBuf, sizeof(TCHAR) * 0x200);
    sprintf(OutPutBuf, "A创建标志 = %d", dwCreationFlags);
    OutputDebugStringA(OutPutBuf);

    return pfnTarCreateProcessA(
        lpApplicationName,
        lpCommandLine,
        lpProcessAttributes,
        lpThreadAttributes,
        bInheritHandles,
        dwCreationFlags,
        lpEnvironment,
        lpCurrentDirectory,
        lpStartupInfo,
        lpProcessInformation);

}

BOOL WINAPI MyCreateProcessW(_In_opt_ LPCWSTR lpApplicationName, _Inout_opt_ LPWSTR lpCommandLine, _In_opt_ LPSECURITY_ATTRIBUTES lpProcessAttributes, _In_opt_ LPSECURITY_ATTRIBUTES lpThreadAttributes, _In_ BOOL bInheritHandles, _In_ DWORD dwCreationFlags, _In_opt_ LPVOID lpEnvironment, _In_opt_ LPCWSTR lpCurrentDirectory, _In_ LPSTARTUPINFOW lpStartupInfo, _Out_ LPPROCESS_INFORMATION lpProcessInformation)
{


    // 拦截 挂起创建进程的进程.
  /*  if (dwCreationFlags == CREATE_SUSPENDED)
    {
        SetLastError(5);
       
        return 0;

    }*/
    
    TCHAR OutPutBuf[0x200] = { 0 };
    RtlZeroMemory(OutPutBuf, sizeof(TCHAR) * 0x200);
    wsprintf(OutPutBuf, L"进程名称 = %ls", lpApplicationName);
    OutputDebugString(lpApplicationName);

    RtlZeroMemory(OutPutBuf, sizeof(TCHAR) * 0x200);
    wsprintf(OutPutBuf, L"命令行 = %ls", lpCommandLine);
    OutputDebugString(OutPutBuf);


    RtlZeroMemory(OutPutBuf, sizeof(TCHAR) * 0x200);
    wsprintf(OutPutBuf, L"创建标志 = %d", dwCreationFlags);
    OutputDebugString(OutPutBuf);

    return pfnTarCreateProcess(
        lpApplicationName,
        lpCommandLine,
        lpProcessAttributes,
        lpThreadAttributes,
        bInheritHandles,
        dwCreationFlags,
        lpEnvironment,
        lpCurrentDirectory,
        lpStartupInfo,
        lpProcessInformation);
}


/*
无模块DLL注入
原理:

1.在DLLmain中 返回false则我们的DLL不会加载到可执行进程
2.申请内存 可读可写可执行. 拷贝自身DLL到内存中.
3.修复重定位表. 不需要修复IAT
*/

BOOL Hook()
{

    if (MH_Initialize() != MH_OK)
    {
        OutputDebugString(TEXT("HOOK 失败,初始化失败\r\n"));
        return FALSE;
    }


    //cout << it->first << it->second << endl;  first = Target函数。 Second = 目的函数
    if (MH_CreateHook(&CreateProcessW, &MyCreateProcessW,        //创建HOOK

        reinterpret_cast<void**>(&pfnTarCreateProcess)) != MH_OK)
    {

        OutputDebugString(TEXT("创建HOOK失败,初始化失败\r\n"));
        return FALSE;
    }

    if (MH_CreateHook(&CreateProcessA, &MyCreateProcessA,        //创建HOOK

        reinterpret_cast<void**>(&pfnTarCreateProcessA)) != MH_OK)
    {

        OutputDebugString(TEXT("创建HOOK失败,初始化失败\r\n"));
        return FALSE;
    }

    // Create a hook for MessageBoxW, in disabled state.



    // Enable the hook for MessageBoxW.

    if (MH_EnableHook(&CreateProcessA) != MH_OK)            //启动HOOK
    {

        return FALSE;
    }
    if (MH_EnableHook(&CreateProcessW) != MH_OK)            //启动HOOK
    {

        return FALSE;
    }

    return TRUE;

}

BOOL UnHook()
{
    if (MH_DisableHook(&CreateProcessW) != MH_OK)
    {
        return FALSE;
    }

    // Expected to tell "Not hooked...".


    // Uninitialize MinHook.
    if (MH_Uninitialize() != MH_OK)
    {
        return FALSE;
    }
}
typedef struct _RELOCATE_ARRAY
{
    short offset : 12;
    short Mark : 4;

}RELOCATE_ARRAY,*PRELOCATE_ARRAY;

PVOID MyHideModule(HMODULE hModule)
{
    /*
  
    1.申请可读写内存空间
    2.拷贝自身到内存中.
    3.在新的ImageBase中获取 重定位表
    4.遍历重定位表的每一页的重定位表. 进行修复.
    */

    //1.获取PE各种头

    LARGE_INTEGER laOldImageBase = { 0 };
#ifdef _WIN64
    laOldImageBase.QuadPart = (ULONGLONG)hModule;
#else
    laOldImageBase.LowPart = (DWORD)hModule;
#endif

    PIMAGE_DOS_HEADER pDosHead = reinterpret_cast<PIMAGE_DOS_HEADER>(hModule);
    PIMAGE_NT_HEADERS pNtHead = reinterpret_cast<PIMAGE_NT_HEADERS>(pDosHead->e_lfanew + laOldImageBase.QuadPart);
    PIMAGE_FILE_HEADER pFileHead = reinterpret_cast<PIMAGE_FILE_HEADER>(&pNtHead->FileHeader);
    PIMAGE_OPTIONAL_HEADER pOptHead = reinterpret_cast<PIMAGE_OPTIONAL_HEADER>(&pNtHead->OptionalHeader);
    



    //2.申请内存空间.
    LPVOID lpNewImageBase = nullptr;
    lpNewImageBase = VirtualAlloc(nullptr, pOptHead->SizeOfImage, MEM_COMMIT, PAGE_EXECUTE_READWRITE);

    if (nullptr == lpNewImageBase)
    {
        return lpNewImageBase;
    }

    char szBuffer[0x100];
    sprintf(szBuffer, "申请的地址位: %p", lpNewImageBase);
    OutputDebugStringA(szBuffer);
    memcpy(lpNewImageBase, (void *)hModule, pOptHead->SizeOfImage);

    //获取重定位表.修复重定位表
    /*
    重定位表修复公式:
    1.定位修复位置:
       修复位置 =  新的ImageBase + 重定位.RVA + 重定位.偏移
    2.修复公式:
        修复: 原偏移值 - 原ImageBase + 新的ImageBase == 新的ImageBase - 原ImageBase + 原偏移值
    
    */
    
#ifdef _WIN64
   ULONGLONG laOffset = (ULONGLONG)lpNewImageBase - (ULONGLONG)hModule;
#else
    DWORD laOffset = (DWORD)lpNewImageBase - (DWORD)hModule;
#endif
   
    //获取到重定位表.修复重定位.
#ifdef _WIN64

    PIMAGE_BASE_RELOCATION pPageRelocatBase = reinterpret_cast<PIMAGE_BASE_RELOCATION>(((ULONGLONG)lpNewImageBase + pOptHead->DataDirectory[IMAGE_DIRECTORY_ENTRY_BASERELOC].VirtualAddress));
#else
    PIMAGE_BASE_RELOCATION  pPageRelocatBase = reinterpret_cast<PIMAGE_BASE_RELOCATION>((PIMAGE_BASE_RELOCATION)((DWORD)lpNewImageBase + pOptHead->DataDirectory[IMAGE_DIRECTORY_ENTRY_BASERELOC].VirtualAddress));
#endif
   
    //遍历重定位表格.

    while (true)
    {   
        if (pPageRelocatBase->SizeOfBlock == 0)
        {
            break;          //判断当前页的重定位表格是否为NULL
        }

        //定位修复重定位表格.
        PRELOCATE_ARRAY pRelocatArray = reinterpret_cast<PRELOCATE_ARRAY>(((PBYTE)pPageRelocatBase + 8)); //得到数组首地址
        //得到循环次数 ,一个数组大小是2个字节. 

        int nCount = (pPageRelocatBase->SizeOfBlock - 8) / 2;
        for (int i = 0; i < nCount; i++)
        {
            //定位修改的偏移地址. 高位为3,修改4个字节. 低位偏移 + 旧的模块基址 + 当前页的VA = 修改位置
            if (pRelocatArray[i].Mark == 3 )
            {
#ifdef _WIN64
                ULONGLONG * ChangeAddressOffset = (ULONGLONG*)(pRelocatArray[i].offset + pPageRelocatBase->VirtualAddress + (ULONGLONG)lpNewImageBase);
#else
                DWORD *ChangeAddressOffset = (DWORD*)(pRelocatArray[i].offset + pPageRelocatBase->VirtualAddress + (DWORD)lpNewImageBase);
#endif // _WIN64

                //开始修改.  新的ImageBase - 原ImageBase + 原偏移值
#ifdef _WIN64
                *ChangeAddressOffset = (*ChangeAddressOffset + laOffset);
#else
                *ChangeAddressOffset = (*ChangeAddressOffset + laOffset);
#endif
                
            }
           
        }

        pPageRelocatBase = reinterpret_cast<PIMAGE_BASE_RELOCATION>((PBYTE)pPageRelocatBase + pPageRelocatBase->SizeOfBlock);
    }
   

    //遍历下一页的重定位表格进行修复

    //抹掉PE头
    memset(lpNewImageBase, 0, 0x1000);

    return lpNewImageBase;
}



DWORD WINAPI MyTestFunction(LPVOID lpParam)
{
   
    //Hook();
    while (true)
    {
        OutputDebugStringA("AA");
        Sleep(1000);
    }
    return 1;
}

