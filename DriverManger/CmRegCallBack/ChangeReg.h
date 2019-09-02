#pragma once
#include <ntddk.h>
#include <ntstrsafe.h>
#include <ntstrsafe.h>
//#include <Ntifs.h>



NTSTATUS InitRootKey(PWSTR RootKeyName);  //����Ϊ���ַ�
NTSTATUS CreateChildKey(HANDLE RoothKey, UNICODE_STRING uSubRegKey, OUT PHANDLE pHandle,BOOLEAN IsCloseHandle);

/*
����ֵ �ֱ�����Root�´�����KeY�µĵ�ÿ�����µ�ÿ��ֵ.

1.��Ҫ���õ�Key
2.��Ҫ���õ�Value������
3.��Ҫ���õ�Value��ֵ.
*/


NTSTATUS SetKeyValueNameAnValue(HANDLE hKey, UNICODE_STRING uValueKeyName, PVOID Data, ULONG DataSize);



NTSTATUS ChangeRootKeyValue(UNICODE_STRING uKeyPath, UNICODE_STRING uValueName, PVOID ValueData, ULONG DataSize);




// ��ȡCmReg��δ��������

typedef struct _SYSTEM_SERVICE_TABLE {
    PVOID ServiceTableBase;
    PVOID ServiceCounterTableBase;
    ULONGLONG NumberOfServices;
    PVOID  ParamTableBase;
} SYSTEM_SERVICE_TABLE, *PSYSTEM_SERVICE_TABLE;
PSYSTEM_SERVICE_TABLE KeServiceDescriptorTable;  //SSDT ��


//����ID ��ȡ�ں�SSDT������ַ
ULONGLONG GetSSDTFuncCurAddr(ULONG id);

//��ȡSSDT��
ULONGLONG MyGetKeServiceDescriptorTable();

//��ȡCmCreateKey �ĵ�ַ Win7��
NTSTATUS GetCmCreateKeyAddress(LONGLONG **pCmCreateKeyAddress, ULONG uId);

//��ȡ���ûص��ĺ���.����Pass����ʧЧ
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

pFnCmCreateKey CmCreateKey;  //ȫ�ֺ���