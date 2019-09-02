#include "Driver.h"


typedef struct _SYSTEM_SERVICE_TABLE {
    PVOID ServiceTableBase;
    PVOID ServiceCounterTableBase;
    ULONGLONG NumberOfServices;
    PVOID  ParamTableBase;
} SYSTEM_SERVICE_TABLE, *PSYSTEM_SERVICE_TABLE;
PSYSTEM_SERVICE_TABLE KeServiceDescriptorTable;  //SSDT 表

//获取CmCreateKey 的地址 Win7下
NTSTATUS GetCmCreateKeyAddress(LONGLONG **pCmCreateKeyAddress);

//传入ID 获取内核SSDT表函数地址
ULONGLONG GetSSDTFuncCurAddr(ULONG id);     

//获取SSDT表
ULONGLONG MyGetKeServiceDescriptorTable();

//获取WIN7的 MASK值
NTSTATUS  GetWin7Sp1MaskValue(LONGLONG **pMaskAddress)
{
    char  *PlaAddress  = NULL;
    char  *PspNotifyEnableMaskAddress = NULL;
    LARGE_INTEGER Offset = {-1};
    ULONG LowOffset = 0;
    char  *PSerchBeginlaAddress = NULL;

    PlaAddress = (char *)PsSetLoadImageNotifyRoutine; 
    PSerchBeginlaAddress = PlaAddress;
    for (PSerchBeginlaAddress; PSerchBeginlaAddress < (PSerchBeginlaAddress + 0x1000); PSerchBeginlaAddress ++)
    {
        USHORT Feature1 = *(USHORT*)((char *)PSerchBeginlaAddress); 
        if (Feature1 == 0x058B)
        {
            int a = 10;
        }
        if (Feature1 == 0x058B && *(USHORT *)((char *)PSerchBeginlaAddress + 6) == 0x01A8)  
        {
            LowOffset = *(ULONG *)((char *)PSerchBeginlaAddress + 2);
            Offset.LowPart = LowOffset; //高位扩展.
            Offset.HighPart = -1;       //需要高位扩展为-1

            PspNotifyEnableMaskAddress = PSerchBeginlaAddress + Offset.QuadPart + 6;
            *pMaskAddress = (LONGLONG*)PspNotifyEnableMaskAddress;
            return STATUS_SUCCESS;
        }
        //函数结尾寻找正确.
        USHORT uRetExit = *(USHORT*)((char *)PSerchBeginlaAddress);
        ULONG RetExit = *(ULONG*)((char *)PSerchBeginlaAddress + 2);
        if (uRetExit == 0xc033 && RetExit == 0x245c8b48)
        {
            return STATUS_UNSUCCESSFUL;
        }
    }
    
    return STATUS_SUCCESS;
}
//修改MASK值过保护
NTSTATUS  PassCallBack(LONGLONG *pMaskAddress,LONGLONG MaskValue)
{
    if (pMaskAddress == NULL)
    {
        return STATUS_UNSUCCESSFUL;
    }

   
    //保存至.清零回调.让其无法使用

    *pMaskAddress = *pMaskAddress | MaskValue;  //让进程回调无效

    return STATUS_SUCCESS;
}

NTSTATUS GetCmpCallCallBacksAddress(LONGLONG **pCmpCallCallBacksAddress, ULONG uId);



KIRQL  WPOFFx64()
{
    KIRQL  irql = KeRaiseIrqlToDpcLevel();
    UINT64  cr0 = __readcr0();
    cr0 &= 0xfffffffffffeffff;
    _disable();
    __writecr0(cr0);
    return  irql;
}

void  WPONx64(KIRQL  irql)
{
    UINT64  cr0 = __readcr0();
    cr0 |= 0x10000;
    _enable();
    __writecr0(cr0);
    KeLowerIrql(irql);
}


typedef struct _CM_NOTIFY_ENTRY
{
    LIST_ENTRY  ListEntryHead;
    ULONG   UnKnown1;
    ULONG   UnKnown2;
    LARGE_INTEGER Cookie;
    PVOID   Context;
    PVOID   Function;
}CM_NOTIFY_ENTRY, *PCM_NOTIFY_ENTRY;

//遍历内存
PVOID SearchMemory(PVOID pStartAddress, PVOID pEndAddress, PUCHAR pMemoryData, ULONG ulMemoryDataSize)
{
    PVOID pAddress = NULL;
    PUCHAR i = NULL;
    ULONG m = 0;
    // 扫描内存
    for (i = (PUCHAR)pStartAddress; i < (PUCHAR)pEndAddress; i++)
    {
        // 判断特征码
        for (m = 0; m < ulMemoryDataSize; m++)
        {
            if (*(PUCHAR)(i + m) != pMemoryData[m])
            {
                break;
            }
        }
        // 判断是否找到符合特征码的地址
        if (m >= ulMemoryDataSize)
        {
            // 找到特征码位置, 获取紧接着特征码的下一地址
            pAddress = (PVOID)(i + ulMemoryDataSize);
            break;
        }
    }
    return pAddress;
}

// 根据特征码获取 CallbackListHead 链表地址
PVOID SearchCallbackListHead(PUCHAR pSpecialData, ULONG ulSpecialDataSize, LONG lSpecialOffset)
{
    UNICODE_STRING ustrFuncName;
    PVOID pAddress = NULL;
    LONG lOffset = 0;
    PVOID pCmUnRegisterCallback = NULL;
    PVOID pCallbackListHead = NULL;
    // 先获取 CmUnRegisterCallback 函数地址
    RtlInitUnicodeString(&ustrFuncName, L"CmUnRegisterCallback");
    pCmUnRegisterCallback = MmGetSystemRoutineAddress(&ustrFuncName);
    if (NULL == pCmUnRegisterCallback)
    {
       
        return pCallbackListHead;
    }
    // 然后, 查找 PspSetCreateProcessNotifyRoutine 函数地址
    pAddress = SearchMemory(pCmUnRegisterCallback,
        (PVOID)((PUCHAR)pCmUnRegisterCallback + 0xFF),
        pSpecialData, ulSpecialDataSize);
    if (NULL == pAddress)
    {
        
        return pCallbackListHead;
    }
    // 获取地址
#ifdef _WIN64
    // 64 位先获取偏移, 再计算地址
    lOffset = *(PLONG)((PUCHAR)pAddress + lSpecialOffset);
    pCallbackListHead = (PVOID)((PUCHAR)pAddress + lSpecialOffset + sizeof(LONG) + lOffset);
#else
    // 32 位直接获取地址
    pCallbackListHead = *(PVOID *)((PUCHAR)pAddress + lSpecialOffset);
#endif
    return pCallbackListHead;
}

//摘掉钩子.

//获取 CallbackListHead 链表地址
PVOID GetCallbackListHead()
{
    PVOID pCallbackListHeadAddress = NULL;
    RTL_OSVERSIONINFOW osInfo = { 0 };
    UCHAR pSpecialData[50] = { 0 };
    ULONG ulSpecialDataSize = 0;
    LONG lSpecialOffset = 0;
    // 获取系统版本信息, 判断系统版本
    RtlGetVersion(&osInfo);
    if (6 == osInfo.dwMajorVersion)
    {
        if (1 == osInfo.dwMinorVersion)
        {
            // Win7
#ifdef _WIN64
            // 64 位
            // 488D54
            pSpecialData[0] = 0x48;
            pSpecialData[1] = 0x8D;
            pSpecialData[2] = 0x54;
            ulSpecialDataSize = 3;
            lSpecialOffset = 5;
#else
            // 32 位
            // BF
            pSpecialData[0] = 0xBF;
            ulSpecialDataSize = 1;
#endif    
        }
        else if (2 == osInfo.dwMinorVersion)
        {
            // Win8
#ifdef _WIN64
            // 64 位
#else
            // 32 位
#endif
        }
        else if (3 == osInfo.dwMinorVersion)
        {
            // Win8.1
#ifdef _WIN64
            // 64 位
            // 488D0D
            pSpecialData[0] = 0x48;
            pSpecialData[1] = 0x8D;
            pSpecialData[2] = 0x0D;
            ulSpecialDataSize = 3;
#else
            // 32 位
            // BE
            pSpecialData[0] = 0xBE;
            ulSpecialDataSize = 1;
#endif            
        }
    }
    else if (10 == osInfo.dwMajorVersion)
    {
        // Win10
#ifdef _WIN64
        // 64 位
        // 488D0D
        pSpecialData[0] = 0x48;
        pSpecialData[1] = 0x8D;
        pSpecialData[2] = 0x0D;
        ulSpecialDataSize = 3;
#else
        // 32 位
        // B9
        pSpecialData[0] = 0xB9;
        ulSpecialDataSize = 1;
#endif
    }
    // 根据特征码获取地址
    pCallbackListHeadAddress = SearchCallbackListHead(pSpecialData, ulSpecialDataSize, lSpecialOffset);
    return pCallbackListHeadAddress;
}

//遍历注册表回调.删除回调
BOOLEAN EnumCallback()
{
    ULONG i = 0;
    PVOID pCallbackListHeadAddress = NULL;
    PCM_NOTIFY_ENTRY pNotifyEntry = NULL;
    // 获取 CallbackListHead 链表地址
    pCallbackListHeadAddress = GetCallbackListHead();
    if (NULL == pCallbackListHeadAddress)
    {
        DbgPrint("GetCallbackListHead Error!\n");
        return FALSE;
    }
    DbgPrint("pCallbackListHeadAddress=0x%p\n", pCallbackListHeadAddress);
    // 开始遍历双向链表
    pNotifyEntry = (PCM_NOTIFY_ENTRY)pCallbackListHeadAddress;
    do
    {
        // 判断 pNotifyEntry 地址是否有效
        if (FALSE == MmIsAddressValid(pNotifyEntry))
        {
            break;
        }
        // 判断 回调函数 地址是否有效
        if (MmIsAddressValid(pNotifyEntry->Function))
        {
            // 显示
            
            ULONG uCharat1 = *(ULONG*)((char *)pNotifyEntry->Function);
            USHORT  uCharat2 = *(USHORT*)((char *)pNotifyEntry->Function + 4);
          /*  if (uCharat1 == 0x894c && uCharat2 == 0x44)
            {
                调用回调进行删除.
                CmUnRegisterCallback(pNotifyEntry->Cookie);
            }*/
            //DbgPrint("CallbackFunction=0x%p, Cookie=0x%I64X\n", pNotifyEntry->Function, pNotifyEntry->Cookie.QuadPart);

            //删除钩子
            if (uCharat1 == 0X48018B48 && uCharat2 == 0X60FF)
            {
                CmUnRegisterCallback(pNotifyEntry->Cookie);
            }
            //或者钩子头部写ret

        }
        // 获取下一链表
        pNotifyEntry = (PCM_NOTIFY_ENTRY)pNotifyEntry->ListEntryHead.Flink;
    } while (pCallbackListHeadAddress != (PVOID)pNotifyEntry);
    return TRUE;
}

//删除回调的方式.
BOOLEAN RemoveTheCmCallBackHook()
{

}
//此方法20分钟之内会蓝屏.等待继续研究.
NTSTATUS PassCallBackInit()
{
    LONGLONG *pCmCreateKey = NULL;
    LONGLONG *pCmCreateKey1 = NULL;
    KIRQL kIrql;
    GetCmpCallCallBacksAddress(&pCmCreateKey, 19);
    pCmCreateKey1 = pCmCreateKey;
    //关闭写保护.修改
    kIrql = WPOFFx64();
    char szBuffer[15] = { 0 };
    memcpy(szBuffer, pCmCreateKey, 15);
    UCHAR ShellCode[] = "\x48\x33\xc0\xc3";
    memcpy((char *)pCmCreateKey, ShellCode, 4);
    // *(char *)pCmCreateKey = 0xc3;
    WPONx64(kIrql);
    return STATUS_SUCCESS;
}
NTSTATUS GetCmpCallCallBacksAddress(LONGLONG **pCmpCallCallBacksAddress, ULONG uId)
{
    //KdBreakPoint();
    char *pNtAddress = NULL;

    char *pBeginAddress = pNtAddress;
    ULONG uCmCreateOffset = 0;

    LONGLONG uCmCreateAddress = 0;
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
                //uCmCreateAddress.LowPart = uCmCreateOffset;
                pCurrentAddress = (LONGLONG)(char *)pBeginAddress + 12;
                uCmCreateAddress = (LONGLONG)uCmCreateOffset + pCurrentAddress + 5;
                *pCmpCallCallBacksAddress = (LONGLONG*)uCmCreateAddress;       //找到地址进行返回.

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


NTSTATUS DriverEntry(PDRIVER_OBJECT pDriverObj, PUNICODE_STRING pRegPath)
{
	ULONG iCount = 0;
	NTSTATUS ntStatus;
	pDriverObj->DriverUnload = DriverUnLoad;
	
   
    KIRQL kIrql;
    kIrql = WPOFFx64();
    EnumCallback();
    WPONx64(kIrql);
    //PassCallBackInit();
    
	return STATUS_SUCCESS;
}


//获取SSDT表
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
//获取SSDT中指向的函数
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

//获取CM导出函数
NTSTATUS GetCmCreateKeyAddress(LONGLONG **pCmCreateKeyAddress)
{

    KdBreakPoint();
    char *pNtAddress = NULL;
   
    char *pBeginAddress = pNtAddress;
    ULONG uCmCreateOffset = 0;
    LARGE_INTEGER uCmCreateAddress = {0};
    /*
    1.通过SSDT获取NtCreateKey
    2.通过NtCreateKey获取CmCreateKey
    3.通过CmCreateKey来进行创建Key.
    */
    pNtAddress = (char *)GetSSDTFuncCurAddr(26);// win764是26 暂时写死.
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
                memcpy((PVOID)(&uCmCreateOffset),((char *)pBeginAddress+6),4); //偏移.
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
