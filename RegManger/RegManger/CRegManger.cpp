#include "CRegManger.h"



CRegManger::CRegManger()
{
}


CRegManger::~CRegManger()
{
}

CBinString CRegManger::ReGetSpecifiedKeyValue(IN HKEY RootKey, IN CBinString SubKey, IN CBinString FindValueName, IN DWORD FindValueNameType)
{
    HKEY hhKey;
    DWORD dwOpt = REG_OPENED_EXISTING_KEY;
    DWORD dwErrorCode = 0;
    CBinString RetString = TEXT("");
   dwErrorCode =  RegCreateKeyEx(
        RootKey, //ROOT KEY
        SubKey.c_str(),//sub key
        0,                                                      // Reserved
        NULL,                                                   // USER class type
        NULL,
        KEY_ALL_ACCESS,
        NULL,
        &hhKey,
        &dwOpt
    );
   if (dwErrorCode != ERROR_SUCCESS || hhKey == 0)
       return TEXT("");

   LPBYTE  lpValueBuffer = new BYTE[MAX_PATH]();
   DWORD dwSize = MAX_PATH * sizeof(TCHAR);
   RegQueryValueEx(hhKey, FindValueName.c_str(), NULL, &FindValueNameType, lpValueBuffer, &dwSize);

   TCHAR szBuffer[MAX_PATH] = { 0 };
   memcpy(szBuffer, lpValueBuffer, MAX_PATH * sizeof(TCHAR));
   RetString = szBuffer;
   if (NULL != lpValueBuffer)
       delete lpValueBuffer;
   RegCloseKey(hhKey);

    return RetString;
}
