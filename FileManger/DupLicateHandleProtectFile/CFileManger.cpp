#include "CFileManger.h"
#include <windows.h>



CFileManger::CFileManger()
{
}


CFileManger::~CFileManger()
{
}

MYISTRING CFileManger::GetXbServiceByRegister()
{
    MYISTRING str;
    HKEY hKey;
    DWORD dwExist = REG_OPENED_EXISTING_KEY;
    RegCreateKeyEx(
        HKEY_LOCAL_MACHINE,
        TEXT("SYSTEM\\CurrentControlSet\\Services\\xbbrowser_service"),
        0,
        0,
        0,
        KEY_WRITE | KEY_READ | KEY_QUERY_VALUE,
        NULL, &hKey,
        &dwExist);
    TCHAR * lpValue = new TCHAR[0X1000]();
    //获取注册表中指定的键所对应的值
    DWORD dwType = REG_EXPAND_SZ;
    DWORD dwSize = 0x1000;
    LONG lRet = ::RegQueryValueEx(hKey, TEXT("ImagePath"), 0, &dwType, (LPBYTE)lpValue, &dwSize);
    //分割路径
    TCHAR drive[_MAX_DRIVE];
    TCHAR dir[_MAX_DIR];
    TCHAR fname[_MAX_FNAME];
    TCHAR ext[_MAX_EXT];
#ifdef UNICODE
    _wsplitpath(lpValue,
        drive,
        dir,
        fname,
        ext);
    str = TEXT("");
    str.append(drive);
    str.append(dir);
    str.append(TEXT("xbbrowser.exe"));
#else

    _splitpath(
        lpValue,
        drive,
        dir,
        fname,
        ext
    );
    str = "";
    str.append(drive);
    str.append(dir);
    str.append("xbbrowser.exe");
#endif

    
    //OccFile(str);

    RegCloseKey(hKey);
    
    return str;
}
