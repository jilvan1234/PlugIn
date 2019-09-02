#include "ChangeReg.h"



//��ROOT_KEY�����ʼ����ֵ
NTSTATUS InitRootKey(PWSTR RootKeyName)
{
    /*
    
    
    1.��HKEY_CLASSES_ROOT �½���һ���ֵ
    HKEY_CLASSES_ROOT = HKEY_LOCAL_MACHINE\SOFTWARE\Classes
    HKEY_
        �ṹ :
    XbbrowserHTM.File
        Application
        DefaultIcon
        Shell
        Open
        command
        */

    
    HANDLE hKey;
    OBJECT_ATTRIBUTES obj;
    ULONG isRegStatus = 0; //ע����״̬.
    NTSTATUS ntStatus = 0;
    UNICODE_STRING uPathName;
    HANDLE hOutHandle ;
    UNICODE_STRING uSubKeyString;

    if (RootKeyName == NULL)
    {
        return STATUS_UNSUCCESSFUL;
    }

    //L"\\Registry\\Machine\\SoftWare\\Classes\\Zy360"
    RtlInitUnicodeString(&uPathName, RootKeyName);  //����360��ע�����.
    InitializeObjectAttributes(&obj,&uPathName,OBJ_CASE_INSENSITIVE,NULL,NULL);
    ntStatus = ZwCreateKey(&hKey,
        KEY_ALL_ACCESS,
        &obj,
        0,
        NULL,
        REG_OPTION_BACKUP_RESTORE,
        &isRegStatus);
    if (!NT_SUCCESS(ntStatus))
    {
        ZwClose(hKey);
        //Ȼ��ʹ��Cm����

        if (CmCreateKey != NULL)
        {
            KdPrint(("��ʼʹ��CmCreateKey���г�ʼ��\r\n"));
            ntStatus = CmCreateKey(&hKey,
                KEY_ALL_ACCESS,
                &obj,
                0,
                NULL,
                REG_OPTION_BACKUP_RESTORE,
                &isRegStatus);
            if (!NT_SUCCESS(ntStatus))
            {
                ZwClose(hKey);
                return ntStatus;
            }
            return STATUS_UNSUCCESSFUL;
        }
        return ntStatus;
    }
    //������key
   
    RtlInitUnicodeString(&uSubKeyString, L"Application");
   ntStatus =  CreateChildKey(hKey,uSubKeyString,&hOutHandle,FALSE);
   if (!NT_SUCCESS(ntStatus))
   {
       ZwClose(hOutHandle);
       ZwClose(hKey);

       return STATUS_UNSUCCESSFUL;
   }
   ZwClose(hOutHandle);

    RtlInitUnicodeString(&uSubKeyString, L"DefaultIcon");
  
    ntStatus = CreateChildKey(hKey, uSubKeyString,&hOutHandle,FALSE);
    if (!NT_SUCCESS(ntStatus))
    {
        ZwClose(hOutHandle);
        ZwClose(hKey);

        return STATUS_UNSUCCESSFUL;
    }
    ZwClose(hOutHandle);
   
    RtlInitUnicodeString(&uSubKeyString, L"Shell");
   
    ntStatus = CreateChildKey(hKey, uSubKeyString,&hOutHandle,FALSE);
    if (!NT_SUCCESS(ntStatus))
    {
        ZwClose(hOutHandle);
        ZwClose(hKey);

        return STATUS_UNSUCCESSFUL;
    }
  

    //�·�ʹ���Ϸ�������HANDLE.

    RtlInitUnicodeString(&uSubKeyString, L"Open");
    ntStatus = CreateChildKey(hOutHandle, uSubKeyString,&hOutHandle,TRUE);
    if (!NT_SUCCESS(ntStatus))
    {
        ZwClose(hOutHandle);
        ZwClose(hKey);

        return STATUS_UNSUCCESSFUL;
    }

    RtlInitUnicodeString(&uSubKeyString, L"command");
   
    ntStatus = CreateChildKey(hOutHandle, uSubKeyString,&hOutHandle,TRUE);
    if (!NT_SUCCESS(ntStatus))
    {
        ZwClose(hOutHandle);
        ZwClose(hKey);

        return STATUS_UNSUCCESSFUL;
    }
  
    if (hOutHandle != 0)
    {
        ZwClose(hOutHandle);
    }
    if (hKey != 0)
    {
        ZwClose(hKey);
    }
    return ntStatus;
}
///������Key
NTSTATUS CreateChildKey(HANDLE RoothKey,UNICODE_STRING uSubRegKey,OUT PHANDLE pHandle, BOOLEAN IsCloseHandle)
{

    //������key
    NTSTATUS ntStatus;
    HANDLE hSubKey;
    OBJECT_ATTRIBUTES obj;

    obj.Length = sizeof(OBJECT_ATTRIBUTES);
    obj.Attributes = OBJ_CASE_INSENSITIVE;
    obj.ObjectName = &uSubRegKey;
    obj.SecurityDescriptor = NULL;
    obj.SecurityQualityOfService = NULL;
    obj.RootDirectory = RoothKey;  //ָ�򸸽���.

    ULONG uRegStatus;

    if (IsCloseHandle == TRUE)
    {

        ntStatus = ZwCreateKey(&hSubKey,
            KEY_ALL_ACCESS,
            &obj,
            0,
            NULL,
            REG_OPTION_NON_VOLATILE,
            &uRegStatus);

        if (!NT_SUCCESS(ntStatus))
        {
            ZwClose(hSubKey);

            if (CmCreateKey != NULL)
            {
                KdPrint(("��ʼʹ��CmCreateKey��ʼ����Key"));
                ntStatus = CmCreateKey(&hSubKey,
                    KEY_ALL_ACCESS,
                    &obj,
                    0,
                    NULL,
                    REG_OPTION_NON_VOLATILE,
                    &uRegStatus);
                if (!NT_SUCCESS(ntStatus))
                {
                    ZwClose(hSubKey);
                    return STATUS_UNSUCCESSFUL;
                }
                return STATUS_UNSUCCESSFUL;
            }
            return STATUS_UNSUCCESSFUL;
        }
        ZwClose(RoothKey);
        *pHandle = hSubKey;
        return STATUS_SUCCESS;
    }
    else
    {
        ntStatus = ZwCreateKey(&hSubKey,
            KEY_ALL_ACCESS,
            &obj,
            0,
            NULL,
            REG_OPTION_NON_VOLATILE,
            &uRegStatus);

        if (!NT_SUCCESS(ntStatus))
        {
            ZwClose(hSubKey);

            if (CmCreateKey != NULL)
            {
                ntStatus = CmCreateKey(&hSubKey,
                    KEY_ALL_ACCESS,
                    &obj,
                    0,
                    NULL,
                    REG_OPTION_NON_VOLATILE,
                    &uRegStatus);
                if (!NT_SUCCESS(ntStatus))
                {
                    ZwClose(hSubKey);
                    return STATUS_UNSUCCESSFUL;
                }
                return STATUS_UNSUCCESSFUL;
            }
            return STATUS_UNSUCCESSFUL;
        }
        *pHandle = hSubKey;
        return STATUS_SUCCESS;
    }

    //ZwClose(hSubKey);
   
    return STATUS_SUCCESS;
}


NTSTATUS GetCmpCallCallBacksAddress(LONGLONG **pCmpCallCallBacksAddress, ULONG uId)
{
    KdBreakPoint();
    char *pNtAddress = NULL;

    char *pBeginAddress = pNtAddress;
    ULONG uCmCreateOffset = 0;
    LARGE_INTEGER uCmCreateAddress = { 0 };
    /*
    1.ͨ��SSDT��ȡNtCreateKey
    2.ͨ��NtCreateKey��ȡCmCreateKey
    3.ͨ��CmCreateKey�����д���Key.
    */
    pNtAddress = (char *)GetSSDTFuncCurAddr(19);// 26 = NtCreateKey 19 = NtQueryKey
    if (NULL == pNtAddress)
    {
        return STATUS_UNSUCCESSFUL;
    }
    pBeginAddress = pNtAddress;
    for (pBeginAddress;; pBeginAddress++)
    {
        if (MmIsAddressValid(pBeginAddress)
            && MmIsAddressValid((char *)pBeginAddress + 4)
            && MmIsAddressValid((char *)pBeginAddress + 8))
        {
            //�ڴ�ɶ�.����в���.
            ULONG ExitMark1 = *(USHORT*)pBeginAddress;
            USHORT  ExitMark2 = *(USHORT*)((char *)pBeginAddress + 8);
            UCHAR  ExitMark3 = *((char *)pBeginAddress + 10);

            ULONG  uCmCreate1 = *(ULONG*)pBeginAddress;
            USHORT  uCmCreate2 = *(USHORT*)((char *)pBeginAddress + 8);
            UCHAR   uCmCreate3 = *((char *)pBeginAddress + 10);
            UCHAR   uCmCreate4 = *((char *)pBeginAddress + 12);
            if (uCmCreate1 == 0x24948D48
                && uCmCreate2 == 0x8d41
                && uCmCreate3 == 0x49
                && uCmCreate4 == 0xE8)
            {
                //�ҵ�CmCreateKey��ƫ����.
                LONGLONG pCurrentAddress = 0;
                memcpy((PVOID)(&uCmCreateOffset), ((char *)pBeginAddress + 13), 4); //ƫ��.
                uCmCreateAddress.LowPart = uCmCreateOffset;
                pCurrentAddress = (LONGLONG)(char *)pBeginAddress + 13;
                uCmCreateAddress.QuadPart = uCmCreateAddress.QuadPart + pCurrentAddress + 5;
               // *pCmpCallCallBacksAddress = (LONGLONG*)uCmCreateAddress.QuadPart;       //�ҵ���ַ���з���.

                return STATUS_SUCCESS;

            }
            if (ExitMark1 == 0x249c8d4c && ExitMark2 == 0x8b49 && ExitMark3 == 0x5B)
            {
                break; //�˳�ѭ�� �ҵ��˳���־��.
            }

            //Ѱ��CmCreateKey
            continue;
        }

        break; //���ɶ�ֱ���˳�

    }


    return STATUS_UNSUCCESSFUL;
}


NTSTATUS SetKeyValueNameAnValue(HANDLE hKey, UNICODE_STRING uValueKeyName, PVOID Data,ULONG DataSize)
{

    NTSTATUS ntStatus;

    ntStatus = ZwSetValueKey(hKey, &uValueKeyName, 0, REG_SZ, Data, DataSize);
    if (!NT_SUCCESS(ntStatus))
    {
        return ntStatus;
    }

    return STATUS_SUCCESS;
}

/*
�޸�Key����� ָ�� value���ֵ� ֵ
*/
NTSTATUS ChangeRootKeyValue(UNICODE_STRING uKeyPath,UNICODE_STRING uValueName,PVOID ValueData, ULONG DataSize)
{
    OBJECT_ATTRIBUTES obj;
    NTSTATUS ntStatus;
    HANDLE hKey;
    InitializeObjectAttributes(&obj, &uKeyPath, OBJ_CASE_INSENSITIVE | OBJ_KERNEL_HANDLE, NULL, NULL);

    ntStatus = ZwOpenKey(&hKey, KEY_ALL_ACCESS, &obj);
    if (!NT_SUCCESS(ntStatus))
    {
        ZwClose(hKey);

        return STATUS_UNSUCCESSFUL;
    }
    //�޸�Value��ֵ.
    ntStatus = SetKeyValueNameAnValue(hKey, uValueName, ValueData, DataSize);
    if (!NT_SUCCESS(ntStatus))
    {
        ZwClose(hKey);

        return STATUS_UNSUCCESSFUL;
    }
    ZwClose(hKey);

    return STATUS_SUCCESS;
}


//��ȡSSDT���׵�ַ
ULONGLONG MyGetKeServiceDescriptorTable()
{
    PUCHAR StartSearchAddress = (PUCHAR)__readmsr(0xC0000082);
    PUCHAR EndSearchAddress = StartSearchAddress + 0x500;
    PUCHAR i = NULL;
    UCHAR b1 = 0, b2 = 0, b3 = 0;
    ULONG templong = 0;
    ULONGLONG addr = 0;

    for (i = StartSearchAddress; i < EndSearchAddress; i++)
    {
        if (MmIsAddressValid(i) && MmIsAddressValid(i + 1) && MmIsAddressValid(i + 2))
        {
            b1 = *i;
            b2 = *(i + 1);
            b3 = *(i + 2);
            if (b1 == 0x4c && b2 == 0x8d && b3 == 0x15) //4c8d15
            {
                memcpy(&templong, i + 3, 4);
                addr = (ULONGLONG)templong + (ULONGLONG)i + 7;
                return addr;
            }
        }
    }
    return 0;
}

//��ȡSSDT���� ��������
ULONGLONG GetSSDTFuncCurAddr(ULONG id)
{

    LONG dwtmp = 0;
    PULONG ServiceTableBase = NULL;
    KeServiceDescriptorTable = (PSYSTEM_SERVICE_TABLE)MyGetKeServiceDescriptorTable(); //��ȡSSDT���ַ
    ServiceTableBase = (PULONG)KeServiceDescriptorTable->ServiceTableBase;             //��ȡSSDT��Ļ�ַ
    dwtmp = ServiceTableBase[id];                                                      //��ַ�����ŵ���������. ������ >>4 + ��ַ = SSDT������ַ.
    dwtmp = dwtmp >> 4;
    return (LONGLONG)dwtmp + (ULONGLONG)ServiceTableBase;
}

//��ȡCmReg ����. WIN7��
NTSTATUS GetCmCreateKeyAddress(LONGLONG **pCmCreateKeyAddress,ULONG uId)
{

    KdBreakPoint();
    char *pNtAddress = NULL;

    char *pBeginAddress = pNtAddress;
    ULONG uCmCreateOffset = 0;
    LARGE_INTEGER uCmCreateAddress = { 0 };
    /*
    1.ͨ��SSDT��ȡNtCreateKey
    2.ͨ��NtCreateKey��ȡCmCreateKey
    3.ͨ��CmCreateKey�����д���Key.
    */
    pNtAddress = (char *)GetSSDTFuncCurAddr(uId);// win764��26 ��ʱд��.
    if (NULL == pNtAddress)
    {
        return STATUS_UNSUCCESSFUL;
    }
    pBeginAddress = pNtAddress;
    for (pBeginAddress;; pBeginAddress++)
    {
        if (MmIsAddressValid(pBeginAddress)
            && MmIsAddressValid((char *)pBeginAddress + 2)
            && MmIsAddressValid((char *)pBeginAddress + 4))
        {
            //�ڴ�ɶ�.����в���.
            USHORT ExitMark1 = *(USHORT*)pBeginAddress;
            UCHAR  ExitMark2 = *((char *)pBeginAddress + 2);
            UCHAR  ExitMark3 = *((char *)pBeginAddress + 4);

            ULONG  uCmCreate1 = *(ULONG*)pBeginAddress;
            UCHAR  uCmCreate2 = *((char *)pBeginAddress + 5);
            if (uCmCreate1 == 0x24448948 && uCmCreate2 == 0xe8)
            {
                //�ҵ�CmCreateKey��ƫ����.
                LONGLONG pCurrentAddress = 0;
                memcpy((PVOID)(&uCmCreateOffset), ((char *)pBeginAddress + 6), 4); //ƫ��.
                uCmCreateAddress.LowPart = uCmCreateOffset;
                uCmCreateAddress.HighPart = -1;

                pCurrentAddress = (LONGLONG)(char *)pBeginAddress + 5;
                uCmCreateAddress.QuadPart = uCmCreateAddress.QuadPart + pCurrentAddress + 5;
                *pCmCreateKeyAddress = (LONGLONG*)uCmCreateAddress.QuadPart;       //�ҵ���ַ���з���.

                return STATUS_SUCCESS;

            }
            if (ExitMark1 == 0x8348 && ExitMark2 == 0xC4 && ExitMark3 == 0xC3)
            {
                break; //�˳�ѭ��
            }

            //Ѱ��CmCreateKey
            continue;
        }

        break; //���ɶ�ֱ���˳�

    }


    return STATUS_UNSUCCESSFUL;
}



NTSTATUS InitCmCreateKey(ULONG uId)
{
    NTSTATUS ntStatus = STATUS_UNSUCCESSFUL;
    LONGLONG * pCmCreateKeyAddress = NULL;
    ntStatus = GetCmCreateKeyAddress(&pCmCreateKeyAddress, uId);
    if (!NT_SUCCESS(ntStatus) || pCmCreateKeyAddress == NULL)
    {
        return STATUS_UNSUCCESSFUL;
    }
    CmCreateKey = (pFnCmCreateKey)pCmCreateKeyAddress;  //��ֵȫ�ֺ�����ַ
    return STATUS_SUCCESS;
}

//NTSTATUS GetHkeyCurrentPath(char * a)
//{
//    NTSTATUS status = STATUS_SUCCESS;
//    HANDLE         hProcess;
//    HANDLE         TokenHandle;
//    ULONG         ReturnLength;
//    ULONG       size;
//    UNICODE_STRING SidString;
//    PTOKEN_USER TokenInformation;
//    char SidStringBuffer[512];
//
//    status = ZwOpenThreadTokenEx(NtCurrentThread(),
//        TOKEN_READ,
//        TRUE,
//        OBJ_KERNEL_HANDLE,
//        &TokenHandle);
//
//    if (!NT_SUCCESS(status)) {
//        status = ZwOpenProcessTokenEx(NtCurrentProcess(),
//            TOKEN_READ,
//            OBJ_KERNEL_HANDLE,
//            &TokenHandle);
//
//        if (!NT_SUCCESS(status)) {
//            return status;
//        }
//    }
//
//    // ��ȡtoken��Ϣ 
//    size = 0x1000;
//    TokenInformation = ExAllocatePool(NonPagedPool, size);
//
//    do {
//        status = NtQueryInformationToken(TokenHandle,
//            TokenUser,
//            TokenInformation,
//            size,
//            &ReturnLength);
//
//        if (status == STATUS_BUFFER_TOO_SMALL) {
//            ExFreePool(TokenInformation);
//            size *= 2;
//            TokenInformation = ExAllocatePool(NonPagedPool, size);
//
//        }
//        else if (!NT_SUCCESS(status)) {
//            DbgPrint(" ZwQueryInformationToken error\n");
//            ExFreePool(TokenInformation);
//            ZwClose(TokenHandle);
//
//            return STATUS_UNSUCCESSFUL;
//        }
//
//    } while (status == STATUS_BUFFER_TOO_SMALL);
//
//    ZwClose(TokenHandle);
//
//    RtlZeroMemory(SidStringBuffer, sizeof(SidStringBuffer));
//    SidString.Buffer = (PWCHAR)SidStringBuffer;
//    SidString.MaximumLength = sizeof(SidStringBuffer);
//
//    status = RtlConvertSidToUnicodeString(&SidString,
//        ((PTOKEN_USER)TokenInformation)->User.Sid,
//        FALSE);
//
//    ExFreePool(TokenInformation);
//    a = SidStringBuffer;
//    return STATUS_SUCCESS;
//}
