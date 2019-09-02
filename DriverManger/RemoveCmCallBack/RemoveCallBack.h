#pragma once
#include <ntddk.h>
#include <wdm.h>

#include <ntdef.h>


typedef struct _CM_NOTIFY_ENTRY
{
    LIST_ENTRY  ListEntryHead;
    ULONG   UnKnown1;
    ULONG   UnKnown2;
    LARGE_INTEGER Cookie;
    PVOID   Context;
    PVOID   Function;
}CM_NOTIFY_ENTRY, *PCM_NOTIFY_ENTRY;

//关闭写保护
KIRQL  WPOFFx64();

//打开写保护
void  WPONx64(KIRQL  irql);

//遍历内存
PVOID SearchMemory(PVOID pStartAddress, PVOID pEndAddress, PUCHAR pMemoryData, ULONG ulMemoryDataSize);

// 根据特征码获取 CallbackListHead 链表地址
PVOID SearchCallbackListHead(PUCHAR pSpecialData, ULONG ulSpecialDataSize, LONG lSpecialOffset);

//获取 CallbackListHead 链表地址
PVOID GetCallbackListHead();

BOOLEAN CheckCharat(PCM_NOTIFY_ENTRY pCmNotifyTable);
//遍历注册表回调.删除回调
BOOLEAN EnumCallbackAnRemoveCallBack();

//删除回调的方式.
NTSTATUS RemoveTheCmCallBackHook();

