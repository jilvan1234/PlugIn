// dllmain.cpp : 定义 DLL 应用程序的入口点。
#include "stdafx.h"
#include <windows.h>
#include <shlwapi.h>
#include <winternl.h>
#include <stdlib.h>
#include <stdio.h>
#pragma comment(lib,"shlwapi.lib")

#define NtCurrentPeb()(((_TEB *)NtCurrentTeb())->ProcessEnvironmentBlock)

/*
DLL 劫持的实现

1.首先我们加载我们想要劫持的DLL. 获取其DLLModule
2.遍历PEB中的模块表.找到->DllBae,修改为我们劫持DLL的hModule即可.

*/

BOOL ChangeDllBaseToHackModel(HMODULE hMod)
{
    PPEB pPeb = nullptr;
    pPeb = NtCurrentPeb();
    if (pPeb == nullptr)
    {
        return 0;
    }

    //遍历模块表.
    PLDR_DATA_TABLE_ENTRY pDataModuleList = nullptr;
    PLIST_ENTRY pModuleListEntry = nullptr;
    PLIST_ENTRY pModuleCurrentListEntry = nullptr;

    pDataModuleList = reinterpret_cast<PLDR_DATA_TABLE_ENTRY>(&pPeb->Ldr->InMemoryOrderModuleList);
    pModuleListEntry = reinterpret_cast<PLIST_ENTRY>(pDataModuleList); // +0偏移就是一个模块表
    pModuleCurrentListEntry = pModuleListEntry; //进行遍历

    //从后往前遍历使用 Blink 从前往后遍历是Flink
    while (pModuleCurrentListEntry->Flink != pModuleListEntry)
    {

        //解析为pDataModuleList
        pDataModuleList = reinterpret_cast<PLDR_DATA_TABLE_ENTRY>(pModuleCurrentListEntry);

        TCHAR szCmpareBuffer[MAX_PATH] = { 0 };
        if (pDataModuleList->FullDllName.Buffer != NULL)
        {
            memcpy(szCmpareBuffer, pDataModuleList->FullDllName.Buffer, sizeof(TCHAR) * MAX_PATH);
        }
        
        //如果遍历的符号为我们的模块名字.则把DllBase进行替换.
        if (wcscmp(TEXT("MyDllHack.dll"), szCmpareBuffer) == 0)
        {
            //相等.替换DataBase
            pDataModuleList->DllBase = hMod;
            break;
        }
        //遍历链表.
        pModuleCurrentListEntry = pModuleCurrentListEntry->Flink;
    }


}
void DllHack()
{
    /*
    1.通过hModule获取当前模块路径
    2.去掉后缀xxx.dll
    3.添加后缀为你要劫持的DLL名. 得出 劫持DLL的hModule
    4.加载此DLL
    5.遍历PEB->LDR_DATA_TABLE_ENTRY表. 得出当前的ImageBase.进行修改.
    */
    TCHAR szHackDllName[MAX_PATH] = { 0 };
    BOOL bRet = FALSE;
    HMODULE h_HackModel = nullptr;
  


    if (!GetCurrentDirectory(sizeof(TCHAR) * MAX_PATH, szHackDllName))
    {
        return ;
    }

    bRet = PathAppend(szHackDllName, TEXT("MyDllHack1.dll"));
    if (bRet == FALSE)
    {
        return;
    }

    //加载此DLL
    h_HackModel = LoadLibrary(szHackDllName);
    if (nullptr == h_HackModel)
    {
        return;
    }

    //遍历PEB的模块表.进行修改DLL Base.
    
    bRet = ChangeDllBaseToHackModel(h_HackModel);
    if (bRet == FALSE)
    {
        return;
    }
}

//重定位表的子成员
typedef struct _RELOCAT_ITEM
{
    WORD offset : 12;
    WORD Mark : 4;

}RELOCAT_ITEM, *PRELOCAT_ITEM;


//模块隐藏测试. 
PVOID HideMode(HMODULE hMod)
{
   

    PIMAGE_DOS_HEADER pDosHead = reinterpret_cast<PIMAGE_DOS_HEADER>(hMod);
    PIMAGE_NT_HEADERS pNtHead = reinterpret_cast<PIMAGE_NT_HEADERS> ((char *)hMod + pDosHead->e_lfanew);
    PIMAGE_FILE_HEADER pFileHead = reinterpret_cast<PIMAGE_FILE_HEADER>(&pNtHead->FileHeader);
    PIMAGE_OPTIONAL_HEADER pOptHead = reinterpret_cast<PIMAGE_OPTIONAL_HEADER>(&pNtHead->OptionalHeader);


    //申请空间.拷贝自己到新的空间.
    PVOID pNewImageBase = VirtualAlloc(NULL, pOptHead->SizeOfImage, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
    if (pNewImageBase == nullptr)
        return nullptr;

    char szBuffer[0x100];
    sprintf(szBuffer, "申请的地址位: %p", pNewImageBase);
    OutputDebugStringA(szBuffer);

    memcpy(pNewImageBase, hMod, pOptHead->SizeOfImage);
    // 获取重定位表

    PIMAGE_BASE_RELOCATION pRelocateTableList = reinterpret_cast<PIMAGE_BASE_RELOCATION>((ULONGLONG)pNewImageBase + pOptHead->DataDirectory[IMAGE_DIRECTORY_ENTRY_BASERELOC].VirtualAddress);

    if (pRelocateTableList == nullptr)
        return nullptr;

    // 定义重定位表
    while (true)
    {
        if (pRelocateTableList->SizeOfBlock == 0)
            break;
        PRELOCAT_ITEM pRelocateTable =
            reinterpret_cast<PRELOCAT_ITEM>((char *)pRelocateTableList + 8);//定位到后面偏移
        if (pRelocateTable == nullptr)
            break;

        // 判断高位是否是要进行修复的.
        int nCount = (pRelocateTableList->SizeOfBlock - 8) / 2;
        for (int i = 0; i < nCount; i++)
        {
            if (pRelocateTable[i].Mark == 3)   //为3代表是要修正4个偏移.
            {
                //定位偏移.
                ULONGLONG * pChangeAddress = (ULONGLONG *)((ULONGLONG)pNewImageBase + pRelocateTableList->VirtualAddress + pRelocateTable[i].offset); //pRelocateTableList也可以不加[i].直接 ->用.

               // 修改偏移.
                *pChangeAddress = *pChangeAddress - (ULONGLONG)hMod + (ULONGLONG)pNewImageBase;

            }
        }


        //指向下一页
        pRelocateTableList = reinterpret_cast<PIMAGE_BASE_RELOCATION>((PBYTE)pRelocateTableList + pRelocateTableList->SizeOfBlock);
    }

    //抹掉PE头
    memset(pNewImageBase, 0, 0x1000);
    return pNewImageBase;

}

DWORD WINAPI MyTestFunction(LPVOID lpParam)
{
   /* while (true)
    {
        OutputDebugStringA("1111");
        Sleep(1000);
    }*/
    ///DllHack();
    MessageBox(nullptr, L"HelloWorld", NULL, NULL);
    return 1;
}

using PfnDllEntry = DWORD(*_stdcall)(LPVOID LParam);

BOOL ModuleHide()
{
    /*
    1.获取PEB
    2.获取LDR_DATA_TABLE_ENTRY
    3.遍历
    4.判断名字是否是我们.是的话进行脱链表

    */

    PPEB pPeb = nullptr;
    pPeb = NtCurrentPeb();
    if (pPeb == nullptr)
    {
        return 0;
    }

    //遍历模块表.
    PLDR_DATA_TABLE_ENTRY pDataModuleList = nullptr;
    PLIST_ENTRY pModuleListEntry = nullptr;
    PLIST_ENTRY pModuleCurrentListEntry = nullptr;

    PLIST_ENTRY pInMemoryOrDerLinks = nullptr;
    PLIST_ENTRY pIninitializeationOrderlinks = nullptr;

    pDataModuleList = reinterpret_cast<PLDR_DATA_TABLE_ENTRY>(&pPeb->Ldr->InMemoryOrderModuleList);
    pModuleListEntry = reinterpret_cast<PLIST_ENTRY>(pDataModuleList); // +0偏移就是一个模块表
    pModuleCurrentListEntry = pModuleListEntry; //进行遍历

    //从后往前遍历使用 Blink 从前往后遍历是Flink
    while (pModuleCurrentListEntry->Flink != pModuleListEntry)
    {

        //解析为pDataModuleList
        pDataModuleList = reinterpret_cast<PLDR_DATA_TABLE_ENTRY>(pModuleCurrentListEntry);

        TCHAR szCmpareBuffer[MAX_PATH] = { 0 };
        if (pDataModuleList->FullDllName.Buffer != NULL)
        {
            memcpy(szCmpareBuffer, pDataModuleList->FullDllName.Buffer, sizeof(TCHAR) * MAX_PATH);
        }

        //如果遍历的符号为我们的模块名字.则把DllBase进行替换.
        if (wcscmp(TEXT("MyDllHack.dll"), szCmpareBuffer) == 0)
        {
            //相等.替换DataBase
           //脱链表.
            pModuleCurrentListEntry->Blink->Flink = pModuleCurrentListEntry->Flink;
            pModuleCurrentListEntry->Flink->Blink = pModuleCurrentListEntry->Blink;
           
            pInMemoryOrDerLinks = reinterpret_cast<PLIST_ENTRY>(&pDataModuleList->InMemoryOrderLinks);
            

            pInMemoryOrDerLinks->Blink->Flink = pInMemoryOrDerLinks->Flink;
            pInMemoryOrDerLinks->Flink->Blink = pInMemoryOrDerLinks->Blink;

            //剩下两个继续拖链.

          

            break;
        }
        //遍历链表.
        pModuleCurrentListEntry = pModuleCurrentListEntry->Flink;
    }

}

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{



    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
     
        
        DllHack();
        return TRUE;

    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}




 /*
    1.获取各种PE头.
    2.定位重定位表.
    3.遍历重定位表高位.修正.

    重定位表位置获取:  新的ImageBase + 重定位.页VA + 重定位.偏移.
    修复重定位表:      重定位.内容 - 旧的ImageBase + 新的ImageBase
    */

//PIMAGE_DOS_HEADER pDosHead = reinterpret_cast<PIMAGE_DOS_HEADER>(hMod);
//PIMAGE_NT_HEADERS pNtHead = reinterpret_cast<PIMAGE_NT_HEADERS> ((char *)hMod + pDosHead->e_lfanew);
//PIMAGE_FILE_HEADER pFileHead = reinterpret_cast<PIMAGE_FILE_HEADER>(&pNtHead->FileHeader);
//PIMAGE_OPTIONAL_HEADER pOptHead = reinterpret_cast<PIMAGE_OPTIONAL_HEADER>(&pNtHead->OptionalHeader);
//
//
////申请空间.拷贝自己到新的空间.
//PVOID pNewImageBase = VirtualAlloc(NULL, pOptHead->SizeOfImage, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
//if (pNewImageBase == nullptr)
//return nullptr;
//
//char szBuffer[0x100];
//sprintf(szBuffer, "申请的地址位: %p", pNewImageBase);
//OutputDebugStringA(szBuffer);
//
//memcpy(pNewImageBase, hMod, pOptHead->SizeOfImage);
////获取重定位表
//
//PIMAGE_BASE_RELOCATION pRelocateTableList = reinterpret_cast<PIMAGE_BASE_RELOCATION>((ULONGLONG)pNewImageBase + pOptHead->DataDirectory[IMAGE_DIRECTORY_ENTRY_BASERELOC].VirtualAddress);
//
//if (pRelocateTableList == nullptr)
//return nullptr;
//
////定义重定位表
//while (true)
//{
//    if (pRelocateTableList->SizeOfBlock == 0)
//        break;
//    PRELOCAT_ITEM pRelocateTable =
//        reinterpret_cast<PRELOCAT_ITEM>((char *)pRelocateTableList + 8);//定位到后面偏移
//    if (pRelocateTable == nullptr)
//        break;
//
//    //判断高位是否是要进行修复的.
//    int nCount = (pRelocateTableList->SizeOfBlock - 8) / 2;
//    for (int i = 0; i < nCount; i++)
//    {
//        if (pRelocateTable[i].Mark == 3)   //为3代表是要修正4个偏移.
//        {
//            //定位偏移.
//            ULONGLONG * pChangeAddress = (ULONGLONG *)((ULONGLONG)pNewImageBase + pRelocateTableList->VirtualAddress + pRelocateTable[i].offset); //pRelocateTableList也可以不加[i].直接 ->用.
//
//            //修改偏移.
//            *pChangeAddress = *pChangeAddress - (ULONGLONG)hMod + (ULONGLONG)pNewImageBase;
//
//        }
//    }
//
//
//    //指向下一页
//    pRelocateTableList = reinterpret_cast<PIMAGE_BASE_RELOCATION>((PBYTE)pRelocateTableList + pRelocateTableList->SizeOfBlock);
//}
//
////抹掉PE头
//memset(pNewImageBase, 0, 0x1000);
//return pNewImageBase;
