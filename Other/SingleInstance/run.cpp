/*
�����崴����һʵ��

*/


#include <stdio.h>
#include <windows.h>
#include <tchar.h>


BOOL IsAlreadyRun()
{
    HANDLE hMutex = NULL;
    hMutex = ::CreateMutex(NULL, FALSE, TEXT("TEST")); //false�������κ��߳���ӵ�е�.�������ź�.
    if (hMutex)
    {
        if (ERROR_ALREADY_EXISTS == ::GetLastError())
        {
            return TRUE;
        }
    }
    
    return FALSE;
}


int _tmain(int argc, _TCHAR* argv[])
{
    // �ж��Ƿ��ظ�����
    if (IsAlreadyRun())
    {
        printf("Already Run!!!!\n");
    }
    else
    {
        printf("NOT Already Run!\n");
    }

    system("pause");
    return 0;
}