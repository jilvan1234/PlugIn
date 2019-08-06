#include <ntifs.h>
#include "Driver.h"

#include "struct.h"

//��������
void ZwKillProcess(PVOID pUserBuffer)
{
    HANDLE hProcess = NULL;
    CLIENT_ID ClientId;
    OBJECT_ATTRIBUTES oa;
    //��� CID
    PKILLPROCESS pKillProcessInfo;
    pKillProcessInfo = (PKILLPROCESS)pUserBuffer;
    ClientId.UniqueProcess = (HANDLE)pKillProcessInfo->pid; //�����޸�Ϊ��Ҫ�� PID
    ClientId.UniqueThread = 0;
    //��� OA
    oa.Length = sizeof(oa);
    oa.RootDirectory = 0;
    oa.ObjectName = 0;
    oa.Attributes = 0;
    oa.SecurityDescriptor = 0;
    oa.SecurityQualityOfService = 0;
    //�򿪽��̣� ��������Ч�����������
    ZwOpenProcess(&hProcess, 1, &oa, &ClientId);
    if (hProcess)
    {
        ZwTerminateProcess(hProcess, 0);
        ZwClose(hProcess);
    };
}
//ǿɱ����
void PassKill(PVOID pUserBuffer)
{
    NTSTATUS ntStatus = 0;
    PEPROCESS pProcess = NULL;
    HANDLE hPid;
    PKILLPROCESS pKillProcessInfo;
    pKillProcessInfo = (PKILLPROCESS)pUserBuffer;
    
    ntStatus = PsLookupProcessByProcessId((HANDLE)pKillProcessInfo->pid, &pProcess);
    if (NT_SUCCESS(ntStatus))
    {
        SIZE_T i = 0;
        //��������
        KeAttachProcess((PEPROCESS)pProcess); //�����Ϊָ�����̵� EPROCESS
        for (i = 0x10000; i < 0x20000000; i += PAGE_SIZE)
        {
            __try
            {
                memset((PVOID)i, 0, PAGE_SIZE); //�ѽ����ڴ�ȫ������
            }
            _except(1)
            {
                ;
            }
        }
        //�˳���������
        KeDetachProcess();
    }
}



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
    
	PIO_STACK_LOCATION pIrpStack;

	pIrpStack = IoGetCurrentIrpStackLocation(pIrp);

	pUserBuffer = pIrp->AssociatedIrp.SystemBuffer;
	uControlCode = pIrpStack->Parameters.DeviceIoControl.IoControlCode;
	pUserBufferLength = pIrpStack->Parameters.DeviceIoControl.OutputBufferLength; //Ĭ����д����.
	

	pIrp->IoStatus.Status = STATUS_SUCCESS; //IRP��¼��β�������.
	pIrp->IoStatus.Information = pUserBufferLength;         //Information������¼ʵ�ʴ�����ֽ�����.
    switch (uControlCode)
    {
    case CTL_PAKILLPROCESS:
    {
        PassKill(pUserBuffer);
    }
    case CTL_ZWKILLPROCESS:
    {
        ZwKillProcess(pUserBuffer);
    }
    default:
        break;
    }
	//�ύ����.
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

	/*UNICODE_STRING uSymbolicLinkName;

	RtlUnicodeStringInit(&uSymbolicLinkName, SYMBOLIC_LINK_NAME);

	IoDeleteDevice(pDriverObj->DeviceObject);
	IoDeleteSymbolicLink(&uSymbolicLinkName);*/
	KdPrint(("����ж�سɹ�"));
}
