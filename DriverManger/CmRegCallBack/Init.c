
#include "Driver.h"
#include "ChangeReg.h"
#include "struct.h"


NTSTATUS MyInitRootKeyName(PVOID pUserBuffer);              //��ʼ��Root Key��Ŀ¼�ṹ
NTSTATUS InitValueAnRootkeyShellOpenCommand(PSET_KEYPATH_VALUENAME_VALUEDATA pSetValueData); //��ʼ��Command�����ֵ.
NTSTATUS SetHkeyCurrentValue(PSET_KEYPATH_VALUENAME_VALUEDATA pSetValueData);               //����http�����ֵ.
NTSTATUS InitCmCreateKeyFunction(PVOID pUserBuffer);

NTSTATUS InitDisPatchFunction(PDRIVER_OBJECT pDriverObj)
{
	ULONG iCount = 0;
	for (iCount = 0; iCount < IRP_MJ_MAXIMUM_FUNCTION; iCount++)
	{
		pDriverObj->MajorFunction[iCount] = DisPatchCommand;
	}

	pDriverObj->MajorFunction[IRP_MJ_CREATE] = DisPatchCreate;
	pDriverObj->MajorFunction[IRP_MJ_READ] = DisPatchRead;
	pDriverObj->MajorFunction[IRP_MJ_WRITE] = DisPatchWrite;
	pDriverObj->MajorFunction[IRP_MJ_DEVICE_CONTROL] = DisPatchControl;
	pDriverObj->MajorFunction[IRP_MJ_CLOSE] = DisPatchClose;

	return STATUS_SUCCESS;

}
NTSTATUS InitDeviceAnSybolicLinkName(PDRIVER_OBJECT pDriverObj)
{
	UNICODE_STRING uDeviceName;
	UNICODE_STRING uSymbolicLinkName;
	PDEVICE_OBJECT pDeviceObj;
	NTSTATUS ntStatus;

	RtlUnicodeStringInit(&uDeviceName, DEVICE_NAME);
	RtlUnicodeStringInit(&uSymbolicLinkName, SYMBOLIC_LINK_NAME);
	//�����豸����
	
	ntStatus = IoCreateDevice(
		pDriverObj,
		0,
		&uDeviceName,
		FILE_DEVICE_UNKNOWN,
		0,
		FALSE,
		&pDeviceObj
	);
	if (!NT_SUCCESS(ntStatus))
	{
		KdPrint(("�����豸����ʧ��"));
		return ntStatus;
	}
	pDeviceObj->Flags = DO_BUFFERED_IO;

	ntStatus = IoCreateSymbolicLink(&uSymbolicLinkName, &uDeviceName);
	//������������
	if (!NT_SUCCESS(ntStatus))
	{
		IoDeleteDevice(pDeviceObj);
		KdPrint(("������������ʧ��"));
		return ntStatus;
	}
	return STATUS_SUCCESS;
}

NTSTATUS DisPatchControl(PDEVICE_OBJECT pDriverObj, PIRP pIrp)
{

	PVOID pUserBuffer = NULL;
	ULONG uControlCode = 0;
	ULONG pUserBufferLength = 0;
    NTSTATUS ntStatus = 0;
	PIO_STACK_LOCATION pIrpStack;
    PSET_KEYPATH_VALUENAME_VALUEDATA pSetKeyValueData = NULL;
	pIrpStack = IoGetCurrentIrpStackLocation(pIrp);

	pUserBuffer = pIrp->AssociatedIrp.SystemBuffer;
	uControlCode = pIrpStack->Parameters.DeviceIoControl.IoControlCode;
	pUserBufferLength = pIrpStack->Parameters.DeviceIoControl.OutputBufferLength; //Ĭ����д����.
	

	pIrp->IoStatus.Status = STATUS_SUCCESS; //IRP��¼��β�������.
	pIrp->IoStatus.Information = pUserBufferLength;         //Information������¼ʵ�ʴ�����ֽ�����.

    KdBreakPoint();
    //���ݲ�������в�ͬ�Ĳ���.
    switch (uControlCode)
    {
    case CTL_INITROOTKEY:
        KdBreakPoint();
        ntStatus = MyInitRootKeyName(pUserBuffer);
        break;
    case CTL_SETKEYVALUE:

    {
       

        pSetKeyValueData = (PSET_KEYPATH_VALUENAME_VALUEDATA)pUserBuffer;
        if (NULL == pSetKeyValueData)
        {
            ntStatus = STATUS_UNSUCCESSFUL;
            break;
        }
        switch (pSetKeyValueData->ControlCode)
        {
        case 1:   //����RootKey�����Commandֵ.
            ntStatus = InitValueAnRootkeyShellOpenCommand(pSetKeyValueData);
            break;
        case 2:
            ntStatus = SetHkeyCurrentValue(pSetKeyValueData);
            break;
        }
        break;
    }
    case CTL_INIT_CMCREATEKEY:
        ntStatus = InitCmCreateKeyFunction(pUserBuffer);
        break;
    default:
        //������������з���
        break;
    }


	//�ύ����.
    pIrp->IoStatus.Status = STATUS_SUCCESS; //IRP��¼��β�������.
    pIrp->IoStatus.Information = 0; //����Ϊ�ɹ���־
	IoCompleteRequest(pIrp, IO_NO_INCREMENT);
	return STATUS_SUCCESS;                  //����� 
}

NTSTATUS DisPatchRead(PDEVICE_OBJECT pDriverObj, PIRP pIrp)
{
	//��ȡRing3������.��Ϊ�ǻ�������ȡ.ֱ�ӻ�ȡ������.
	PVOID pUserReadBuffer = NULL;
	ULONG uPuserReadBufferLeng = 0;
	ULONG uHelloWorldLength = 0;
	ULONG uMinLength = 0;

	PIO_STACK_LOCATION pIrpStack;
	

	pUserReadBuffer = pIrp->AssociatedIrp.SystemBuffer;

	pIrpStack = IoGetCurrentIrpStackLocation(pIrp); //�õĵĲ����ڶ�ջ����
	uPuserReadBufferLeng = pIrpStack->Parameters.Read.Length;
	uHelloWorldLength =(ULONG)wcslen(L"HelloWorld");

	uMinLength = uHelloWorldLength > uPuserReadBufferLeng
		? uHelloWorldLength : uPuserReadBufferLeng;

	//�������ݸ�Ring3
	__try
	{
		ProbeForRead(pUserReadBuffer, uPuserReadBufferLeng, 0);
		RtlCopyMemory(pUserReadBuffer, L"HelloWorld", uMinLength);
	}
	__except (STATUS_ACCESS_VIOLATION)
	{
		KdPrint(("�����쳣��"));
	}
	
	
	

	pIrp->IoStatus.Status = STATUS_SUCCESS; //IRP��¼��β�������.
	pIrp->IoStatus.Information = uMinLength;         //Information������¼ʵ�ʴ�����ֽ�����.

	//�ύ����.
	IoCompleteRequest(pIrp, IO_NO_INCREMENT);
	return STATUS_SUCCESS;                  //����� 
}
NTSTATUS DisPatchWrite(PDEVICE_OBJECT pDriverObj, PIRP pIrp)
{
	PVOID pUserWriteBuffer = NULL;
	ULONG uPuserWriteBufferLeng = 0;
	PVOID pKernelBuffer = NULL;

	PIO_STACK_LOCATION pIrpStack;

	pUserWriteBuffer = pIrp->AssociatedIrp.SystemBuffer;

	pIrpStack = IoGetCurrentIrpStackLocation(pIrp); //�õĵĲ����ڶ�ջ����
	uPuserWriteBufferLeng = pIrpStack->Parameters.Write.Length;
	
	
	pKernelBuffer = ExAllocatePoolWithTag(PagedPool, uPuserWriteBufferLeng, 0);
	if (NULL == pKernelBuffer)
	{
		KdPrint(("�Բ���.д�ڴ����"));
		pIrp->IoStatus.Status = STATUS_SUCCESS; //IRP��¼��β�������.
		pIrp->IoStatus.Information = 0;         //Information������¼ʵ�ʴ�����ֽ�����.

		//�ύ����.
		IoCompleteRequest(pIrp, IO_NO_INCREMENT);
		return STATUS_SUCCESS;
	}
	RtlCopyMemory(pKernelBuffer, pUserWriteBuffer, uPuserWriteBufferLeng);
	
	pIrp->IoStatus.Status = STATUS_SUCCESS; //IRP��¼��β�������.
	pIrp->IoStatus.Information = uPuserWriteBufferLeng;         //Information������¼ʵ�ʴ�����ֽ�����.

	//�ύ����.
	IoCompleteRequest(pIrp, IO_NO_INCREMENT);
	return STATUS_SUCCESS;                  //����� 
}

NTSTATUS DisPatchCommand(PDEVICE_OBJECT pDriverObj, PIRP pIrp)
{
	pIrp->IoStatus.Status = STATUS_SUCCESS; //IRP��¼��β�������.
	pIrp->IoStatus.Information = 0;         //Information������¼ʵ�ʴ�����ֽ�����.

	//�ύ����.
	IoCompleteRequest(pIrp, IO_NO_INCREMENT);
	return STATUS_SUCCESS;                  //����� 
}

NTSTATUS DisPatchClose(PDEVICE_OBJECT pDriverObj, PIRP pIrp)
{
	pIrp->IoStatus.Status = STATUS_SUCCESS; //IRP��¼��β�������.
	pIrp->IoStatus.Information = 0;         //Information������¼ʵ�ʴ�����ֽ�����.

	//�ύ����.
	IoCompleteRequest(pIrp, IO_NO_INCREMENT);
	return STATUS_SUCCESS;                  //����� 
}
NTSTATUS DisPatchCreate(PDEVICE_OBJECT pDriverObj, PIRP pIrp)
{
	pIrp->IoStatus.Status = STATUS_SUCCESS; //IRP��¼��β�������.
	pIrp->IoStatus.Information = 0;         //Information������¼ʵ�ʴ�����ֽ�����.

	//�ύ����.
	IoCompleteRequest(pIrp, IO_NO_INCREMENT);
	return STATUS_SUCCESS;                  //����� 
}
void DriverUnLoad(PDRIVER_OBJECT pDriverObj)
{
	//�ͷ���Դ.ɾ���豸����.

    UNICODE_STRING uSymbolicLinkName;

    RtlUnicodeStringInit(&uSymbolicLinkName, SYMBOLIC_LINK_NAME);

    IoDeleteDevice(pDriverObj->DeviceObject);
    IoDeleteSymbolicLink(&uSymbolicLinkName);
	KdPrint(("����ж�سɹ�"));
}


//��ʼ��ROOTkey�ṹ
NTSTATUS MyInitRootKeyName(PVOID pUserBuffer)
{
    PINIT_ROOTKEY pRootKey = (PINIT_ROOTKEY)pUserBuffer;

    if (pRootKey == NULL)
    {
        return STATUS_UNSUCCESSFUL;
    }
    //��ȡRootKey����Ϣ.

    return InitRootKey(pRootKey->KeyPathName);
}

//��ʼ�� Root Key shell open command��ֵ

NTSTATUS InitValueAnRootkeyShellOpenCommand(PSET_KEYPATH_VALUENAME_VALUEDATA pSetValueData)
{

    UNICODE_STRING uKeyPath = { 0 };
    UNICODE_STRING uValueName = { 0 };

    RtlInitUnicodeString(&uKeyPath, pSetValueData->KeyPathName);
    RtlInitUnicodeString(&uValueName, pSetValueData->ValueName);

    return  ChangeRootKeyValue(uKeyPath,uValueName,pSetValueData->ValueData,pSetValueData->ValueDataSize);
}

NTSTATUS SetHkeyCurrentValue(PSET_KEYPATH_VALUENAME_VALUEDATA pSetValueData)
{
    UNICODE_STRING uKeyPath = { 0 };
    UNICODE_STRING uValueName = { 0 };

    RtlInitUnicodeString(&uKeyPath, pSetValueData->KeyPathName);
    RtlInitUnicodeString(&uValueName, pSetValueData->ValueName);

    return  ChangeRootKeyValue(uKeyPath, uValueName, pSetValueData->ValueData, pSetValueData->ValueDataSize);
}

//��ʼ��fun��ֵ
NTSTATUS InitCmCreateKeyFunction(PVOID pUserBuffer)
{
    PLONGLONG pMarsk = NULL;
    KdBreakPoint();
    if (pUserBuffer == NULL)
    {
        return STATUS_UNSUCCESSFUL;
    }

    PULONG pSSDTId = (PULONG)pUserBuffer;

    return InitCmCreateKey(*pSSDTId);

    
    //return GetCmpCallCallBacksAddress(&pMarsk,*pSSDTId);
}