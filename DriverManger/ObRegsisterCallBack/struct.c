#include "struct.h"


VOID Pass(PDRIVER_OBJECT pDriverObject)
{
    PLDR_DATA_TABLE_ENTRY ldr;
    ldr = (PLDR_DATA_TABLE_ENTRY)pDriverObject->DriverSection;
    ldr->Flags |= 0x20;//加载驱动的时候会判断此值。必须有特殊签名才行，增加0x20即可。否则将调用失败 
}