#include "CShellCode.h"
#include <windows.h>
#include <winternl.h>

CShellCode::CShellCode()
{
}


CShellCode::~CShellCode()
{
}

#ifdef _WIN64
unsigned __int64  CShellCode::GetKernelBase()
{
    _TEB *pTeb64 = NULL;
    PPEB pPeb64 = NULL;
    PLIST_ENTRY pInMemoryOrderLinks;
    __int64 DllBase = NULL;
    PLDR_DATA_TABLE_ENTRY pldrTableEntry;
    pTeb64 = NtCurrentTeb();
    pPeb64 = pTeb64->ProcessEnvironmentBlock;
    pldrTableEntry = (PLDR_DATA_TABLE_ENTRY)pPeb64->Ldr;
    pInMemoryOrderLinks = (PLIST_ENTRY)&pldrTableEntry->InMemoryOrderLinks;
    pInMemoryOrderLinks = pInMemoryOrderLinks->Flink;
    pldrTableEntry = (PLDR_DATA_TABLE_ENTRY)pInMemoryOrderLinks;
    pInMemoryOrderLinks = pInMemoryOrderLinks->Flink;
    pldrTableEntry = (PLDR_DATA_TABLE_ENTRY)pInMemoryOrderLinks;    //走三次. 自己 ntdll kernel32
    pInMemoryOrderLinks = pInMemoryOrderLinks->Flink;
    pldrTableEntry = (PLDR_DATA_TABLE_ENTRY)pInMemoryOrderLinks;
   
    DllBase = (__int64)pldrTableEntry->DllBase;
    return DllBase;
}
#else
//32位下获取Kernel32地址.
unsigned long CShellCode::GetKernelBase()
{
    __asm
    {
        xor edx,edx
        mov ecx, fs:[0x30];             //获取PEB
        mov ecx, [ecx + 0xC];           //获取PEB中的LDR
        mov ecx, [ecx + 0x1C];          //获得 _LDR_DATA_TABLE_ENTRY 指向的内存结构

        MYLOOP:
            mov eax, [ecx + 0x8];    //获得模块基址 ,第一次获取
            mov ebx, [ecx + 0x20];  //获得成员SizeofImage
            mov ebp, [ebx + 0x18];
            mov ecx, [ecx];
            cmp[ebx + 0x18], dx;    //比较[SizeofImage + 0x18] 低位 跟 dx相比是否为0
            
        JNZ MYLOOP;
        ret 

    }
    return 0;
}
#endif

