#include "Driver.h"
#include <ntimage.h>


/*
��������������������

����DriverObject�е� DriverSection. �����Ա�Ƕ�Ӧһ�� LDR_DATA_TABLE_ENTRY�ṹ��.

*/
typedef struct _LDR_DATA_TABLE_ENTRY {
    LIST_ENTRY InLoadOrderLinks;
    LIST_ENTRY InMemoryOrderLinks;
    LIST_ENTRY InInitializationOrderLinks;
    PVOID DllBase;
    PVOID EntryPoint;
    UNICODE_STRING FullDllName;
} LDR_DATA_TABLE_ENTRY, *PLDR_DATA_TABLE_ENTRY;


NTSTATUS EnumDevice(PDRIVER_OBJECT pDriver)
{

}
NTSTATUS DriverEntry(PDRIVER_OBJECT pDriverObj, PUNICODE_STRING pRegPath)
{

    PLDR_DATA_TABLE_ENTRY pDataTableEntry = NULL;

	ULONG iCount = 0;
	NTSTATUS ntStatus;
	pDriverObj->DriverUnload = DriverUnLoad;
	ntStatus = InitDeviceAnSybolicLinkName(pDriverObj);
	if (!NT_SUCCESS(ntStatus))
	{
		return ntStatus;
	}

	ntStatus = InitDisPatchFunction(pDriverObj);
	if (!NT_SUCCESS(ntStatus))
	{
		return ntStatus;
	}

    WideCharToMultiByte()
        MultiByteToWideChar()
	return STATUS_SUCCESS;
}


