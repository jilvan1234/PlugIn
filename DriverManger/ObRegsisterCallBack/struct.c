#include "struct.h"


VOID Pass(PDRIVER_OBJECT pDriverObject)
{
    PLDR_DATA_TABLE_ENTRY ldr;
    ldr = (PLDR_DATA_TABLE_ENTRY)pDriverObject->DriverSection;
    ldr->Flags |= 0x20;//����������ʱ����жϴ�ֵ������������ǩ�����У�����0x20���ɡ����򽫵���ʧ�� 
}