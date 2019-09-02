#include "RemoveCallBack.h"




//�ر�д����
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




//�����ڴ�
PVOID SearchMemory(PVOID pStartAddress, PVOID pEndAddress, PUCHAR pMemoryData, ULONG ulMemoryDataSize)
{
    PVOID pAddress = NULL;
    PUCHAR i = NULL;
    ULONG m = 0;
    // ɨ���ڴ�
    for (i = (PUCHAR)pStartAddress; i < (PUCHAR)pEndAddress; i++)
    {
        // �ж�������
        for (m = 0; m < ulMemoryDataSize; m++)
        {
            if (*(PUCHAR)(i + m) != pMemoryData[m])
            {
                break;
            }
        }
        // �ж��Ƿ��ҵ�����������ĵ�ַ
        if (m >= ulMemoryDataSize)
        {
            // �ҵ�������λ��, ��ȡ���������������һ��ַ
            pAddress = (PVOID)(i + ulMemoryDataSize);
            break;
        }
    }
    return pAddress;
}


// �����������ȡ CallbackListHead �����ַ
PVOID SearchCallbackListHead(PUCHAR pSpecialData, ULONG ulSpecialDataSize, LONG lSpecialOffset)
{
    UNICODE_STRING ustrFuncName;
    PVOID pAddress = NULL;
    LONG lOffset = 0;
    PVOID pCmUnRegisterCallback = NULL;
    PVOID pCallbackListHead = NULL;
    // �Ȼ�ȡ CmUnRegisterCallback ������ַ
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
    // ��ȡ��ַ
#ifdef _WIN64
    // 64 λ�Ȼ�ȡƫ��, �ټ����ַ
    lOffset = *(PLONG)((PUCHAR)pAddress + lSpecialOffset);
    pCallbackListHead = (PVOID)((PUCHAR)pAddress + lSpecialOffset + sizeof(LONG) + lOffset);
#else
    // 32 λֱ�ӻ�ȡ��ַ
    pCallbackListHead = *(PVOID *)((PUCHAR)pAddress + lSpecialOffset);
#endif
    return pCallbackListHead;
}

//ժ������.

//��ȡ CallbackListHead �����ַ
PVOID GetCallbackListHead()
{
    PVOID pCallbackListHeadAddress = NULL;
    RTL_OSVERSIONINFOW osInfo = { 0 };
    UCHAR pSpecialData[50] = { 0 };
    ULONG ulSpecialDataSize = 0;
    LONG lSpecialOffset = 0;
    // ��ȡϵͳ�汾��Ϣ, �ж�ϵͳ�汾
    RtlGetVersion(&osInfo);
    if (6 == osInfo.dwMajorVersion)
    {
        if (1 == osInfo.dwMinorVersion)
        {
            // Win7
#ifdef _WIN64
            // 64 λ
            // 488D54
            pSpecialData[0] = 0x48;
            pSpecialData[1] = 0x8D;
            pSpecialData[2] = 0x54;
            ulSpecialDataSize = 3;
            lSpecialOffset = 5;
#else
            // 32 λ
            // BF
            pSpecialData[0] = 0xBF;
            ulSpecialDataSize = 1;
#endif    
        }
        else if (2 == osInfo.dwMinorVersion)
        {
            // Win8
#ifdef _WIN64
            // 64 λ
#else
            // 32 λ
#endif
        }
        else if (3 == osInfo.dwMinorVersion)
        {
            // Win8.1
#ifdef _WIN64
            // 64 λ
            // 488D0D
            pSpecialData[0] = 0x48;
            pSpecialData[1] = 0x8D;
            pSpecialData[2] = 0x0D;
            ulSpecialDataSize = 3;
#else
            // 32 λ
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
        // 64 λ
        // 488D0D
        pSpecialData[0] = 0x48;
        pSpecialData[1] = 0x8D;
        pSpecialData[2] = 0x0D;
        ulSpecialDataSize = 3;
#else
        // 32 λ
        // B9
        pSpecialData[0] = 0xB9;
        ulSpecialDataSize = 1;
#endif
    }
    // �����������ȡ��ַ
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
//����ע���ص�.ɾ���ص�
BOOLEAN EnumCallbackAnRemoveCallBack()
{
    ULONG i = 0;
    PVOID pCallbackListHeadAddress = NULL;
    PCM_NOTIFY_ENTRY pNotifyEntry = NULL;
    BOOLEAN bRet = FALSE;
    // ��ȡ CallbackListHead �����ַ
    pCallbackListHeadAddress = GetCallbackListHead();
    if (NULL == pCallbackListHeadAddress)
    {

        return FALSE;
    }

    // ��ʼ����˫������
    pNotifyEntry = (PCM_NOTIFY_ENTRY)pCallbackListHeadAddress;
    do
    {
        // �ж� pNotifyEntry ��ַ�Ƿ���Ч
        if (FALSE == MmIsAddressValid(pNotifyEntry))
        {
            break;
        }
        // �ж� �ص����� ��ַ�Ƿ���Ч
        if (MmIsAddressValid(pNotifyEntry->Function))
        {
            // ��ʾ

            KdBreakPoint();
           
            __try
            {
                ULONG uCharat1 = *(ULONG*)((char *)pNotifyEntry->Function);
                USHORT  uCharat2 = *(USHORT*)((char *)pNotifyEntry->Function + 4);

                //ɾ������
                if (uCharat1 == 0X48018B48 && uCharat2 == 0X60FF)
                {
                    KdPrint(("����ص�1"));
                    CmUnRegisterCallback(pNotifyEntry->Cookie);

                }


                if (uCharat1 == 0x24448949)
                {
                    KdPrint(("����ص�2"));
                    CmUnRegisterCallback(pNotifyEntry->Cookie);

                }
                KdPrint(("û�н������ص�"));
            }
            __except (1)
            {

            }
           
         

        }
        // ��ȡ��һ����
        pNotifyEntry = (PCM_NOTIFY_ENTRY)pNotifyEntry->ListEntryHead.Flink;
    } while (pCallbackListHeadAddress != (PVOID)pNotifyEntry);
    return FALSE;
}

//ɾ���ص��ķ�ʽ.
NTSTATUS RemoveTheCmCallBackHook()
{
    BOOLEAN bRet = FALSE;
    KIRQL kIrql;
    kIrql = WPOFFx64();
    
    KdPrint(("��ʼö��ɾ��"));
    bRet =  EnumCallbackAnRemoveCallBack();

    WPONx64(kIrql);
    if (bRet == TRUE)
    {
        return STATUS_SUCCESS;
    }
    return STATUS_UNSUCCESSFUL;
}

