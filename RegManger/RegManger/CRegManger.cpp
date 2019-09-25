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


//��ȡָ��Key���������Key
vector<CBinString> CRegManger::RegGetSpecifiedKeyAllKey(IN HKEY RootKey, IN CBinString SubKey)
{
    /*
    1.��ָ�� RootKey ����� subKey. 
    2.ʹ��RegQueryInfor ��ȡ����Key������
    */
    HKEY hhKey;
    DWORD dwOpt = REG_OPENED_EXISTING_KEY;
    DWORD dwErrorCode = 0;
    CBinString RetString = TEXT("");
    vector<CBinString> cChKeyList;
    dwErrorCode = RegCreateKeyEx(
        RootKey, //ROOT KEY
        SubKey.c_str(),//sub key
        0,                                                      // Reserved
        NULL,                                                   // USER class type
        NULL,
        KEY_ENUMERATE_SUB_KEYS | KEY_QUERY_VALUE,
        NULL,
        &hhKey,
        &dwOpt
    );
    if (dwErrorCode != ERROR_SUCCESS || hhKey == 0)
        return vector<CBinString>();

    //����Key
    DWORD cSubKeys = 0;
    DWORD cValueS = 0;
   dwErrorCode =  RegQueryInfoKey(hhKey, NULL, NULL, 0, &cSubKeys, NULL, NULL, &cValueS, NULL, NULL, NULL,nullptr);
   if (dwErrorCode != ERROR_SUCCESS)
       return  vector<CBinString>();

   //���ݵó���Key������.����Keyֵ.
   for (int i = 0; i < cSubKeys; i++)
   {
       TCHAR *pBuffer = new TCHAR[0X100]();
       DWORD dwSize = sizeof(TCHAR) * 0X100;
       dwErrorCode =  RegEnumKeyEx(hhKey, i, pBuffer, &dwSize, 0, nullptr, nullptr, nullptr);
       if (dwErrorCode == ERROR_SUCCESS)
           vector<CBinString>();
       cChKeyList.push_back(pBuffer);

   }
  
    return cChKeyList;
}
