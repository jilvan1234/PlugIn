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
//    UNICODE_STRING uPathKey = RTL_CONSTANT_STRING(L"\\Registry\\Machine\\SoftWare\\Classes\\Zy360"); //Ҫ�޸�Key��ȫ·��.
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
//    InitRootKey(); //��ʼ��Key
//   
//    KdBreakPoint();
//    DbgBreakPoint();
//
//    RtlInitUnicodeString(&uSubValueName, L"AppUserModelId");  //Ҫ���õļ�ֵ.
//    memcpy(szBuffer, L"1111", 100);
//  
//    ntStatus = ChangeRootKeyValue(uPathKey, uSubValueName, szBuffer, 100); //�޸�Key��ֵ.
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

