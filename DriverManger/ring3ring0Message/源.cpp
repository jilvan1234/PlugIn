#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <tchar.h>
#include <string>

using namespace std;

typedef struct _KILLPROCESS
{
    unsigned int pid;

}KILLPROCESS, *PKILLPROCESS;


#define IOCTRL_BASE 0x800
#define MYIOCTRL_CODE(i)\
    CTL_CODE(FILE_DEVICE_UNKNOWN,IOCTRL_BASE+i,METHOD_BUFFERED,FILE_ANY_ACCESS)
//�����豸���� �豸��������ֵ  ����R3��R0��ͨѶ��ʽ.��ָ��Device.  ���ǵ�Ȩ��.

#define CTL_PAKILLPROCESS  MYIOCTRL_CODE(1) //ǿɱ
#define CTL_ZWKILLPROCESS  MYIOCTRL_CODE(2) //��������

int _tmain(_In_ int argc, _In_reads_(argc) _Pre_z_ _TCHAR** argv, _In_z_ _TCHAR** envp)
{

    PKILLPROCESS pUserBuffer = new KILLPROCESS;
    string str = argv[1];
    pUserBuffer->pid = atoi(str.c_str());
    HANDLE hFile = CreateFile("\\\\?\\InterceptLink",
        GENERIC_WRITE | GENERIC_READ,
        0,
        NULL,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,
        NULL);
    if (hFile != INVALID_HANDLE_VALUE)
    {
        DeviceIoControl(hFile, CTL_ZWKILLPROCESS, pUserBuffer, sizeof(KILLPROCESS), pUserBuffer, sizeof(KILLPROCESS), NULL, NULL);

    }
    CloseHandle(hFile);
}
    

