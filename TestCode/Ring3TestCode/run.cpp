#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <tchar.h>
#include "../../publicstruct.h"


#pragma comment(lib, "urlmon.lib")  

PfnZwQuerySystemInformation ZwQuerySystemInformation;
void InitFunction()
{
    HMODULE hDll = LoadLibrary(TEXT("ntdll.dll"));
    if (NULL == hDll)
    {
        return;
    }
    ZwQuerySystemInformation = reinterpret_cast<PfnZwQuerySystemInformation>(GetProcAddress(hDll, "ZwQuerySystemInformation"));
    if (NULL == ZwQuerySystemInformation)
        return;
}

PVOID RetSystemHandleInformation()
{
    //����ϵͳ�����Ϣ.
    PULONG szBuffer = NULL;
    DWORD dwSize = 0x1000;
    DWORD dwRetSize = 0;
    NTSTATUS ntStatus;
    szBuffer = new ULONG[dwSize];
    if (NULL == szBuffer)
    {
        return NULL;
    }
    //��һ����ÿ��ܲ��ɹ�.���Ի�ȡ����ֵ. ���Ҹ���DwRetSize��ֵȥ��������ռ�
    do 
    {
        ntStatus = ZwQuerySystemInformation(SystemHandleInformation, szBuffer, dwSize, &dwRetSize);
        if (ntStatus == STATUS_INFO_LENGTH_MISMATCH) //����ռ䲻��.��������ռ�
        {
          
            delete[] szBuffer;
            szBuffer = new ULONG[dwSize *= 2];

        }
        else
        {
            if (!NT_SUCCESS(ntStatus))
            {
                delete [] szBuffer;
                return NULL;
            }
        }
    } while (ntStatus == STATUS_INFO_LENGTH_MISMATCH);
    
    //�ɹ�������Ϣ
    return szBuffer;
}


HANDLE GetFileHandle()
{
    //������Լ�
    TCHAR tszPath[MAX_PATH] = { 0 };
    GetModuleFileName(NULL,tszPath,MAX_PATH );
    return CreateFile(
        tszPath,
        GENERIC_READ ,
        FILE_SHARE_READ,
        NULL,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,
        NULL);
}
int _tmain(_In_ int argc, _In_reads_(argc) _Pre_z_ _TCHAR** argv, _In_z_ _TCHAR** envp)
{
   
    //getchar();
    //system("pause");

    ////A���� ��������ҳ
    //HANDLE hMap = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, 0x1000, TEXT("�����ڴ�"));

    //if (NULL == hMap)
    //    return FALSE;

    ////2.ӳ�������ڴ�(���Ե�ַ��)
    //LPVOID lpBuf = MapViewOfFile(hMap, FILE_MAP_ALL_ACCESS, 0, 0, 0x1000);

    //char szBuff[0x100] = "xbbrowser.exe";
    //memcpy(lpBuf, szBuff, 0x100);
    //// A����д�ڴ�

    //printf("ӳ���ڴ�ɹ�%p \r\n",lpBuf);
    //getchar();
    //system("pause");

    //printf("�Ƿ�ʼж���ڴ�\r\n");
    ////UnmapViewOfFile(lpBuf);
    //system("pause");
    //CloseHandle(hMap);

    //system("pause");

   /*
   1.��ʼ������ָ��
   2.����ȫ�־�����ȡ��Ϣ
   3.���ļ�
   4.����ȫ�־����ȡ��Ϣ.
   
   */
    PSYSTEM_HANDLE_INFORMATION pGlobaleHandleInfo = NULL;  //ȫ�־����Ϣ
    SYSTEM_HANDLE_TABLE_ENTRY_INFO HandleInfor = { 0 }; //�������ϸ��Ϣ.
    PVOID pBuffer = NULL;
    HANDLE hFile;
    DWORD dwNumberOfHandles = 0;  //����ĸ���
    InitFunction();
    hFile = GetFileHandle();
    pBuffer =  RetSystemHandleInformation();
    pGlobaleHandleInfo = reinterpret_cast<PSYSTEM_HANDLE_INFORMATION>(pBuffer);
  
    //����˾����������Ϣ.���򿪸��ļ����.�ж������Ƿ����.

    dwNumberOfHandles = pGlobaleHandleInfo->NumberOfHandles;
    for (int i = 0; i < dwNumberOfHandles; i++)
    {
        HandleInfor = pGlobaleHandleInfo->Handles[i];//��ȡ�����������Ϣ
        if (GetCurrentProcessId() == HandleInfor.UniqueProcessId)
        {
            if (HandleInfor.HandleValue == (USHORT)hFile)
            {

                printf("hFile��Ӧ���ļ�����Ϊ %d \r\n", HandleInfor.ObjectTypeIndex);
            }
        }
    }

    system("pause");
    return 0;

}