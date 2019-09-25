#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <tchar.h>
#include <dbghelp.h>
#include "../../publicstruct.h"
#include "../../ClassManger/CNativeApi/CNativeApiManger.h"
#include "../../ThreadManger/RemoteThread/CRemoteThread.h"

#pragma comment(lib,"../../PulicLib/Win32/Release/CNativeApi.lib")
#pragma comment(lib,"../../PulicLib/Win32/Release/RemoteThread.lib")



#include <dbghelp.h>
#pragma comment (lib,"Dbghelp.lib")

int _tmain(_In_ int argc, _In_reads_(argc) _Pre_z_ _TCHAR** argv, _In_z_ _TCHAR** envp)
{
   
 
  /* 
    system("pause");
    HMODULE hMod = LoadLibrary(TEXT("test.dll"));
    if (NULL == hMod)
    {
        printf("加载DLL失败\r\n");
        system("pause");
        return 0;
    }
    printf("DLL 加载成功\r\n");*/

   
  
    Sleep(2000);
    return 0;
}