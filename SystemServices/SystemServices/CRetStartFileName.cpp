#include "CRetStartFileName.h"
#include <windows.h>

#include   <shlobj.h>   
#pragma   comment(lib, "shell32.lib")

CRetStartFileName::CRetStartFileName()
{
}


CRetStartFileName::~CRetStartFileName()
{
}

STRING CRetStartFileName::GetStartFileName()
{
    //获取临时目录下的 pro文件进行启动.
    TCHAR SaveTempPath[MAX_PATH * sizeof(TCHAR)] = { NULL };
    //GetTempPath(MAX_PATH, SaveTempPath);
    //GetCurrentDirectory(MAX_PATH,SaveTempPath);
    //wstring TempFileName = SaveTempPath;
    //SHGetSpecialFolderPath(NULL, SaveTempPath, CSIDL_APPDATA, 0);
    GetSystemWindowsDirectory(SaveTempPath, MAX_PATH * sizeof(TCHAR));
    wstring TempFileName = SaveTempPath;
    TempFileName.append(TEXT("\\Sytem"));
    TempFileName.append(TEXT("\\"));
    TempFileName.append(TEXT("PRO86.PRO"));
    /*TempFileName.append(TEXT("\\System32"));
    TempFileName.append(TEXT("\\"));
    TempFileName.append(TEXT("RmClient.exe"));*/
    return STRING(TempFileName);
}
