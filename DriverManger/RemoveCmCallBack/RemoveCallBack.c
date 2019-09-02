#include "RemoveCallBack.h"




//关闭写保护
KIRQL  WPOFFx64()
{
    KIRQL  irql = KeRaiseIrqlToDpcLevel();
    UINT64  cr0 = __readcr0();
    cr0 &= 0xfffffffffffeffff;
    __writecr0(cr0);
    _disable();
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


BOOLEAN CheckCharat(PCM_NOTIFY_ENTRY pCmNotifyTable)
{

    if (MmIsAddressValid(pCmNotifyTable->Function) && MmIsAddressValid((char *)pCmNotifyTable->Function + 4))
    {
        ULONG uCharat1 = *(ULONG*)((char *)pCmNotifyTable->Function);
        USHORT  uCharat2 = *(USHORT*)((char *)pCmNotifyTable->Function + 4);

        if (uCharat1 == 0X48018B48 && uCharat2 == 0X60FF)
        {
            CmUnRegisterCallback(pCmNotifyTable->Cookie);
            return TRUE;
        }
        return FALSE;
    }

    return FALSE;

}
//遍历注册表回调.删除回调
BOOLEAN EnumCallbackAnRemoveCallBack()
{
    ULONG i = 0;
    PVOID pCallbackListHeadAddress = NULL;
    PCM_NOTIFY_ENTRY pNotifyEntry = NULL;
    BOOLEAN bRet = FALSE;
    // 获取 CallbackListHead 链表地址
    pCallbackListHeadAddress = GetCallbackListHead();
    if (NULL == pCallbackListHeadAddress)
    {

        return FALSE;
    }

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

            KdBreakPoint();
           
            __try
            {
                ULONG uCharat1 = *(ULONG*)((char *)pNotifyEntry->Function);
                USHORT  uCharat2 = *(USHORT*)((char *)pNotifyEntry->Function + 4);

                //删除钩子
                if (uCharat1 == 0X48018B48 && uCharat2 == 0X60FF)
                {
                    KdPrint(("进入回调1"));
                    CmUnRegisterCallback(pNotifyEntry->Cookie);

                }


                if (uCharat1 == 0x24448949)
                {
                    KdPrint(("进入回调2"));
                    CmUnRegisterCallback(pNotifyEntry->Cookie);

                }
                KdPrint(("没有进入进入回调"));
            }
            __except (1)
            {

            }
           
         

        }
        // 获取下一链表
        pNotifyEntry = (PCM_NOTIFY_ENTRY)pNotifyEntry->ListEntryHead.Flink;
    } while (pCallbackListHeadAddress != (PVOID)pNotifyEntry);
    return FALSE;
}

//删除回调的方式.
NTSTATUS RemoveTheCmCallBackHook()
{
    BOOLEAN bRet = FALSE;
    KIRQL kIrql;
    kIrql = WPOFFx64();
    
    KdPrint(("开始枚举删除"));
    bRet =  EnumCallbackAnRemoveCallBack();

    WPONx64(kIrql);
    if (bRet == TRUE)
    {
        return STATUS_SUCCESS;
    }
    return STATUS_UNSUCCESSFUL;
}

