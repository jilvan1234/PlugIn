#include "ChangeReg.h"



//在ROOT_KEY下面初始化键值
NTSTATUS InitRootKey(PWSTR RootKeyName)
{
    /*
    
    
    1.在HKEY_CLASSES_ROOT 下建立一项键值
    HKEY_CLASSES_ROOT = HKEY_LOCAL_MACHINE\SOFTWARE\Classes
    HKEY_
        结构 :
    XbbrowserHTM.File
        Application
        DefaultIcon
        Shell
        Open
        command
        */

    
    HANDLE hKey;
    OBJECT_ATTRIBUTES obj;
    ULONG isRegStatus = 0; //注册表的状态.
    NTSTATUS ntStatus = 0;
    UNICODE_STRING uPathName;
    HANDLE hOutHandle ;
    UNICODE_STRING uSubKeyString;

    if (RootKeyName == NULL)
    {
        return STATUS_UNSUCCESSFUL;
    }

    //L"\\Registry\\Machine\\SoftWare\\Classes\\Zy360"
    RtlInitUnicodeString(&uPathName, RootKeyName);  //创建360的注册表项.
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
        //然后使用Cm函数

        if (CmCreateKey != NULL)
        {
            KdPrint(("开始使用CmCreateKey进行初始化\r\n"));
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
    //创建子key
   
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
  

    //下方使用上方创建的HANDLE.

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
///创建子Key
NTSTATUS CreateChildKey(HANDLE RoothKey,UNICODE_STRING uSubRegKey,OUT PHANDLE pHandle, BOOLEAN IsCloseHandle)
{

    //创建子key
    NTSTATUS ntStatus;
    HANDLE hSubKey;
    OBJECT_ATTRIBUTES obj;

    obj.Length = sizeof(OBJECT_ATTRIBUTES);
    obj.Attributes = OBJ_CASE_INSENSITIVE;
    obj.ObjectName = &uSubRegKey;
    obj.SecurityDescriptor = NULL;
    obj.SecurityQualityOfService = NULL;
    obj.RootDirectory = RoothKey;  //指向父进程.

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
                KdPrint(("开始使用CmCreateKey初始化子Key"));
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
    1.通过SSDT获取NtCreateKey
    2.通过NtCreateKey获取CmCreateKey
    3.通过CmCreateKey来进行创建Key.
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
            //内存可读.则进行操作.
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
                //找到CmCreateKey的偏移了.
                LONGLONG pCurrentAddress = 0;
                memcpy((PVOID)(&uCmCreateOffset), ((char *)pBeginAddress + 13), 4); //偏移.
                uCmCreateAddress.LowPart = uCmCreateOffset;
                pCurrentAddress = (LONGLONG)(char *)pBeginAddress + 13;
                uCmCreateAddress.QuadPart = uCmCreateAddress.QuadPart + pCurrentAddress + 5;
               // *pCmpCallCallBacksAddress = (LONGLONG*)uCmCreateAddress.QuadPart;       //找到地址进行返回.

                return STATUS_SUCCESS;

            }
            if (ExitMark1 == 0x249c8d4c && ExitMark2 == 0x8b49 && ExitMark3 == 0x5B)
            {
                break; //退出循环 找到退出标志了.
            }

            //寻找CmCreateKey
            continue;
        }

        break; //不可读直接退出

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
修改Key下面的 指定 value名字的 值
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
    //修改Value的值.
    ntStatus = SetKeyValueNameAnValue(hKey, uValueName, ValueData, DataSize);
    if (!NT_SUCCESS(ntStatus))
    {
        ZwClose(hKey);

        return STATUS_UNSUCCESSFUL;
    }
    ZwClose(hKey);

    return STATUS_SUCCESS;
}


//获取SSDT表首地址
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

//获取SSDT函数 根据索引
ULONGLONG GetSSDTFuncCurAddr(ULONG id)
{

    LONG dwtmp = 0;
    PULONG ServiceTableBase = NULL;
    KeServiceDescriptorTable = (PSYSTEM_SERVICE_TABLE)MyGetKeServiceDescriptorTable(); //获取SSDT表地址
    ServiceTableBase = (PULONG)KeServiceDescriptorTable->ServiceTableBase;             //获取SSDT表的基址
    dwtmp = ServiceTableBase[id];                                                      //基址里面存放的是索引号. 索引号 >>4 + 基址 = SSDT表函数地址.
    dwtmp = dwtmp >> 4;
    return (LONGLONG)dwtmp + (ULONGLONG)ServiceTableBase;
}

//获取CmReg 函数. WIN7下
NTSTATUS GetCmCreateKeyAddress(LONGLONG **pCmCreateKeyAddress,ULONG uId)
{

    KdBreakPoint();
    char *pNtAddress = NULL;

    char *pBeginAddress = pNtAddress;
    ULONG uCmCreateOffset = 0;
    LARGE_INTEGER uCmCreateAddress = { 0 };
    /*
    1.通过SSDT获取NtCreateKey
    2.通过NtCreateKey获取CmCreateKey
    3.通过CmCreateKey来进行创建Key.
    */
    pNtAddress = (char *)GetSSDTFuncCurAddr(uId);// win764是26 暂时写死.
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
            //内存可读.则进行操作.
            USHORT ExitMark1 = *(USHORT*)pBeginAddress;
            UCHAR  ExitMark2 = *((char *)pBeginAddress + 2);
            UCHAR  ExitMark3 = *((char *)pBeginAddress + 4);

            ULONG  uCmCreate1 = *(ULONG*)pBeginAddress;
            UCHAR  uCmCreate2 = *((char *)pBeginAddress + 5);
            if (uCmCreate1 == 0x24448948 && uCmCreate2 == 0xe8)
            {
                //找到CmCreateKey的偏移了.
                LONGLONG pCurrentAddress = 0;
                memcpy((PVOID)(&uCmCreateOffset), ((char *)pBeginAddress + 6), 4); //偏移.
                uCmCreateAddress.LowPart = uCmCreateOffset;
                uCmCreateAddress.HighPart = -1;

                pCurrentAddress = (LONGLONG)(char *)pBeginAddress + 5;
                uCmCreateAddress.QuadPart = uCmCreateAddress.QuadPart + pCurrentAddress + 5;
                *pCmCreateKeyAddress = (LONGLONG*)uCmCreateAddress.QuadPart;       //找到地址进行返回.

                return STATUS_SUCCESS;

            }
            if (ExitMark1 == 0x8348 && ExitMark2 == 0xC4 && ExitMark3 == 0xC3)
            {
                break; //退出循环
            }

            //寻找CmCreateKey
            continue;
        }

        break; //不可读直接退出

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
    CmCreateKey = (pFnCmCreateKey)pCmCreateKeyAddress;  //赋值全局函数地址
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
//    // 获取token信息 
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
