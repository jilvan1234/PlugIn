#include <Windows.h>
#include <stdlib.h>
#include <stdlib.h>
#include "CProcessOpt.h"


int main()
{
    HANDLE hMutex1 = CreateMutex(NULL, TRUE, "{B8592103-AE8C-4D37-807F-F1CB76E62B7C}");
    if (GetLastError() == ERROR_ALREADY_EXISTS)
    {
        OutputDebugString("1111111111111111111111111111\r\n");
        OutputDebugString("1111111111111111111111111111\r\n");
        return 0;
    }
    HANDLE hProcess = 0;
    HANDLE hTarProcess = 0;
    CProcessOpt PsOpt;
    PsOpt.SeEnbalAdjustPrivileges(SE_DEBUG_NAME);
    DWORD dwErrorCode = 0;
    for (int i = 0; i < 10000; i += 4)
    {
        hProcess = PsOpt.PsGetProcess(i);
        if (hProcess != 0)
        {
            dwErrorCode =  DuplicateHandle(GetCurrentProcess(),hMutex1 , hProcess, &hTarProcess, 0, 0, DUPLICATE_SAME_ACCESS);
            if (dwErrorCode != 0)
            {
                CloseHandle(hProcess);
                CloseHandle(hMutex1);
                return 0;
            }
            continue;
        }
    }
    
    
    //while (true)
    //{
    //    Sleep(180000); // 延迟三分钟.
    //    return 0;
    //}
    return 0;

}
