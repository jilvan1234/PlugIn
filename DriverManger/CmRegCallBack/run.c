#include "Driver.h"
#include "ChangeReg.h"
//NTSTATUS InitTestKey();

NTSTATUS DriverEntry(PDRIVER_OBJECT pDriverObj, PUNICODE_STRING pRegPath)
{
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

  
   
   // InitTestKey();
   
	return STATUS_SUCCESS;
}



//NTSTATUS InitTestKey()
//
//{
//
//
//    NTSTATUS ntStatus = 0;
//    UNICODE_STRING uPathKey = RTL_CONSTANT_STRING(L"\\Registry\\Machine\\SoftWare\\Classes\\Zy360"); //要修改Key的全路径.
//    UNICODE_STRING uSubValueName;
//    ULONG BufferSize = 0;
//    TCHAR *szBuffer = ExAllocatePoolWithTag(PagedPool, 100 * sizeof(TCHAR), 'niBi');
//
//    if (szBuffer == NULL)
//    {
//        ExFreePool(szBuffer);
//        return STATUS_UNSUCCESSFUL;
//    }
//    RtlZeroMemory(szBuffer, 100 * sizeof(TCHAR));
//
//    InitRootKey(); //初始化Key
//   
//    KdBreakPoint();
//    DbgBreakPoint();
//
//    RtlInitUnicodeString(&uSubValueName, L"AppUserModelId");  //要设置的键值.
//    memcpy(szBuffer, L"1111", 100);
//  
//    ntStatus = ChangeRootKeyValue(uPathKey, uSubValueName, szBuffer, 100); //修改Key的值.
//
//    if (!NT_SUCCESS(ntStatus))
//    {
//        ExFreePool(szBuffer);
//        return STATUS_UNSUCCESSFUL;
//    }
//
//
//    if (szBuffer != NULL)
//    {
//        ExFreePool(szBuffer);
//    }
//
//    return STATUS_SUCCESS;
//}

