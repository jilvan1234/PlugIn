#include "Driver.h"
#include "../../DriverManger/ObRegsisterCallBack/struct.h"



VOID Pass(PDRIVER_OBJECT pDriverObject)
{
    PLDR_DATA_TABLE_ENTRY ldr;
    ldr = (PLDR_DATA_TABLE_ENTRY)pDriverObject->DriverSection;
    ldr->Flags |= 0x20;//加载驱动的时候会判断此值。必须有特殊签名才行，增加0x20即可。否则将调用失败 
}
/*

注册表回调保护.
*/
LARGE_INTEGER Global_Address = { 0 };


NTSTATUS
RegistCallBack(
    _In_ PVOID CallbackContext,
    _In_opt_ PVOID Argument1,
    _In_opt_ PVOID Argument2
)
{
    //Arg1是类型
    //Arg2根据Arg1的类型产生不同的类型
    KdBreakPoint();
    REG_NOTIFY_CLASS RegType = (REG_NOTIFY_CLASS)Argument1;
    PREG_SET_VALUE_KEY_INFORMATION PsetReg = (PREG_SET_VALUE_KEY_INFORMATION)Argument2;
   
    int a = 10;
    return STATUS_SUCCESS;
}
NTSTATUS DriverEntry(PDRIVER_OBJECT pDriverObj, PUNICODE_STRING pRegPath)
{
	ULONG iCount = 0;
	NTSTATUS ntStatus;
    UNICODE_STRING strHigh;
	pDriverObj->DriverUnload = DriverUnLoad;
	//ntStatus = InitDeviceAnSybolicLinkName(pDriverObj);
	//if (!NT_SUCCESS(ntStatus))
	//{
	//	return ntStatus;
	//}

	//ntStatus = InitDisPatchFunction(pDriverObj);
	//if (!NT_SUCCESS(ntStatus))
	//{
	//	return ntStatus;
	//}
    Pass(pDriverObj);
    RtlInitUnicodeString(&strHigh, L"32010");
    CmRegisterCallbackEx(RegistCallBack, &strHigh, pDriverObj, NULL, &Global_Address, NULL);
    //CmUnRegisterCallback(Global_Address);
	return STATUS_SUCCESS;
}


