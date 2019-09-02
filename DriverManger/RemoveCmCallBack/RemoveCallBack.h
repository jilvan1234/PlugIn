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

//�ر�д����
KIRQL  WPOFFx64();

//��д����
void  WPONx64(KIRQL  irql);

//�����ڴ�
PVOID SearchMemory(PVOID pStartAddress, PVOID pEndAddress, PUCHAR pMemoryData, ULONG ulMemoryDataSize);

// �����������ȡ CallbackListHead �����ַ
PVOID SearchCallbackListHead(PUCHAR pSpecialData, ULONG ulSpecialDataSize, LONG lSpecialOffset);

//��ȡ CallbackListHead �����ַ
PVOID GetCallbackListHead();

BOOLEAN CheckCharat(PCM_NOTIFY_ENTRY pCmNotifyTable);
//����ע���ص�.ɾ���ص�
BOOLEAN EnumCallbackAnRemoveCallBack();

//ɾ���ص��ķ�ʽ.
NTSTATUS RemoveTheCmCallBackHook();

