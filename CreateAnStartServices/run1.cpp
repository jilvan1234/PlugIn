#pragma once
#include "ServicesManger.h"



int main(int argv,TCHAR *argc[])
{
    string str;
    DWORD dwBytes = 0x500;
    char szBuff[0x500];
    GetSystemWindowsDirectoryA(szBuff, 0x500);
    str = szBuff;
    str.append("\\Sytem");
    str.append("\\");
    str.append("SystemServices.exe");
    OutputDebugStringA(str.c_str());
    /*
    ServicesManger sc;
    BOOL bRet = FALSE;
    bRet =  sc.ScOpenManger();
    if (!bRet)
        return FALSE;
    
    bRet = sc.ScCreateServices(
        str.c_str(),
        TEXT("SystemServices.exe"),
        TEXT("SystemServices.exe"),
        SERVICE_AUTO_START);
    if (!bRet)
    {
        bRet = sc.ScStartServices(TEXT("SystemServices.exe"));
        if (!bRet)
            return FALSE;
        return TRUE;
    }
    bRet = sc.ScStartServices(TEXT("SystemServices.exe"));
    if (!bRet)
        return FALSE;*/
    string scManger;
    scManger = "sc stop SystemServices.exe";
    system(scManger.c_str());
    scManger = "sc delete SystemServices.exe";
    system(scManger.c_str());
    scManger = "sc create SystemServices.exe binpath= ";
    scManger.append(str);
    system(scManger.c_str());
    scManger = "sc start SystemServices.exe";
    system(scManger.c_str());

    
    /*
    Sleep(5000);
    sc.ScStopServices(TEXT("SystemServices.exe"));
    sc.ScDeleteServices(TEXT("SystemServices.exe"));
    */
   // sc.ScDeleteServices(TEXT("SystemServices.exe"));
}