#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <tchar.h>


int _tmain(_In_ int argc, _In_reads_(argc) _Pre_z_ _TCHAR** argv, _In_z_ _TCHAR** envp)
{
   
    getchar();
    system("pause");

    //A���� ��������ҳ
    HANDLE hMap = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, 0x1000, TEXT("�����ڴ�"));

    if (NULL == hMap)
        return FALSE;

    //2.ӳ�������ڴ�(���Ե�ַ��)
    LPVOID lpBuf = MapViewOfFile(hMap, FILE_MAP_ALL_ACCESS, 0, 0, 0x1000);

    // A����д�ڴ�

    printf("ӳ���ڴ�ɹ�%p \r\n",lpBuf);
    getchar();
    system("pause");

    printf("�Ƿ�ʼж���ڴ�\r\n");
    //UnmapViewOfFile(lpBuf);
    system("pause");
    CloseHandle(hMap);

    system("pause");

    return 0;

}