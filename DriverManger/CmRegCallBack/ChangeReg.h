#pragma once
#include <ntddk.h>
#include <ntstrsafe.h>
#include <ntstrsafe.h>
//#include <Ntifs.h>



NTSTATUS InitRootKey(PWSTR RootKeyName);  //必须为宽字符
NTSTATUS CreateChildKey(HANDLE RoothKey, UNICODE_STRING uSubRegKey, OUT PHANDLE pHandle,BOOLEAN IsCloseHandle);

/*
设置值 分别设置Root下创建的KeY下的的每个键下的每个值.

1.你要设置的Key
2.你要设置的Value的名字
3.你要设置的Value的值.
*/


NTSTATUS SetKeyValueNameAnValue(HANDLE hKey, UNICODE_STRING uValueKeyName, PVOID Data, ULONG DataSize);



NTSTATUS ChangeRootKeyValue(UNICODE_STRING uKeyPath, UNICODE_STRING uValueName, PVOID ValueData, ULONG DataSize);




// 获取CmReg等未导出函数

typedef struct _SYSTEM_SERVICE_TABLE {
    PVOID ServiceTableBase;
    PVOID ServiceCounterTableBase;
    ULONGLONG NumberOfServices;
    PVOID  ParamTableBase;
} SYSTEM_SERVICE_TABLE, *PSYSTEM_SERVICE_TABLE;
PSYSTEM_SERVICE_TABLE KeServiceDescriptorTable;  //SSDT 表


//传入ID 获取内核SSDT表函数地址
ULONGLONG GetSSDTFuncCurAddr(ULONG id);

//获取SSDT表
ULONGLONG MyGetKeServiceDescriptorTable();

//获取CmCreateKey 的地址 Win7下
NTSTATUS GetCmCreateKeyAddress(LONGLONG **pCmCreateKeyAddress, ULONG uId);

//获取调用回调的函数.进行Pass让其失效
NTSTATUS GetCmpCallCallBacksAddress(LONGLONG **pCmpCallCallBacksAddress, ULONG uId);

NTSTATUS InitCmCreateKey(ULONG uId);

typedef NTSTATUS(NTAPI *pFnCmCreateKey)(
    OUT PHANDLE             pKeyHandle,
    IN ACCESS_MASK          DesiredAccess,
    IN POBJECT_ATTRIBUTES   ObjectAttributes,
    IN ULONG                TitleIndex,
    IN PUNICODE_STRING      Class OPTIONAL,
    IN ULONG                CreateOptions,
    OUT PULONG              Disposition OPTIONAL);

pFnCmCreateKey CmCreateKey;  //全局函数