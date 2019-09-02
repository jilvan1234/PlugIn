// ProcessTestCode.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include "pch.h"
#include <iostream>
#include "../../ProcessManger/ProcessIterator/CProcessOpt.h"
#include <tchar.h>

#include <Aclapi.h>

#pragma comment(lib,"Advapi32.lib")


#ifdef _WIN64
#pragma comment(lib,"../../../PulicLib/x64/Release/CNativeApi.lib")
#pragma comment(lib,"../../../PulicLib/x64/Release/ProcessIterator.lib")
#pragma comment(lib,"../../../PulicLib/x64/Release/RegManger.lib")
#pragma comment(lib,"../../../PulicLib/x64/Release/CFileManger.lib")
#pragma comment(lib,"../../../PulicLib/x64/Release/RemoteThread.lib")
#else
#ifdef _DEBUG
#pragma comment(lib,"../../PulicLib/Win32/Debug/ProcessIterator.lib")
#else

#pragma comment(lib,"../../PulicLib/Win32/Release/ProcessIterator.lib")
#endif

#endif


typedef void(__stdcall*NTPROC)(DWORD*, DWORD*, DWORD*);
void GetWinVer()
{
    HINSTANCE hinst = LoadLibrary(TEXT("ntdll.dll"));
    DWORD dwMajor, dwMinor, dwBuildNumber;
    NTPROC proc = (NTPROC)GetProcAddress(hinst, "RtlGetNtVersionNumbers"); proc(&dwMajor, &dwMinor, &dwBuildNumber);
    proc(&dwMajor, &dwMinor, &dwBuildNumber);  //windows 10 10.0代表是10.

    if (dwMajor == 6 && dwMinor == 3 )
    {
        //win8 . 1
    }
    if (dwMajor == 10 && dwMinor ==0)
    {
        //win10
    }

    //win8 一下.

}




int _tmain(int argc, _TCHAR* argv[])
{
 
    //获得系统版本

    //GetWinVer();

    printf("value = %d \r\n", getSystemName());
    return 0;
}

