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
    pldrTableEntry = (PLDR_DATA_TABLE_ENTRY)pInMemoryOrderLinks;    //������. �Լ� ntdll kernel32
    pInMemoryOrderLinks = pInMemoryOrderLinks->Flink;
    pldrTableEntry = (PLDR_DATA_TABLE_ENTRY)pInMemoryOrderLinks;
   
    DllBase = (__int64)pldrTableEntry->DllBase;
    return DllBase;
}
#else
//32λ�»�ȡKernel32��ַ.
unsigned long CShellCode::GetKernelBase()
{
    __asm
    {
        xor edx,edx
        mov ecx, fs:[0x30];             //��ȡPEB
        mov ecx, [ecx + 0xC];           //��ȡPEB�е�LDR
        mov ecx, [ecx + 0x1C];          //��� _LDR_DATA_TABLE_ENTRY ָ����ڴ�ṹ

        MYLOOP:
            mov eax, [ecx + 0x8];    //���ģ���ַ ,��һ�λ�ȡ
            mov ebx, [ecx + 0x20];  //��ó�ԱSizeofImage
            mov ebp, [ebx + 0x18];
            mov ecx, [ecx];
            cmp[ebx + 0x18], dx;    //�Ƚ�[SizeofImage + 0x18] ��λ �� dx����Ƿ�Ϊ0
            
        JNZ MYLOOP;
        ret 

    }
    return 0;
}
#endif

