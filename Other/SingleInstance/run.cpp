/*
互斥体创建单一实例

*/


#include <stdio.h>
#include <windows.h>
#include <tchar.h>


BOOL IsAlreadyRun()
{
    HANDLE hMutex = NULL;
    hMutex = ::CreateMutex(NULL, FALSE, TEXT("TEST")); //false代表不是任何线程所拥有的.代表有信号.
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
    // 判断是否重复运行
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