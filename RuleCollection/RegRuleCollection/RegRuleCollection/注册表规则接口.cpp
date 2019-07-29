#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <tchar.h>
#include <string>

using namespace std;

#ifdef UNICODE
#define  STRING wstring
#else
#define  STRING string
#endif



#include <Aclapi.h> 
#pragma comment (lib,"Advapi32.lib")
//获取注册表类型.
HKEY GetRootKeyType(STRING strRoot)
{

    STRING strType = TEXT("");
   // STRING strRoot;
    strRoot.erase(0, strRoot.find_first_not_of(TEXT(" ")));
    strType = TEXT("HKEY_CLASSES_ROOT");
    if (strRoot.compare(strType) == 0)
    {
        return HKEY_CLASSES_ROOT;
    }

    strType = TEXT("HKEY_CURRENT_USER");
    if (strRoot.compare(strType) == 0)
    {
        return HKEY_CURRENT_USER;
    }
    strType = TEXT("HKEY_LOCAL_MACHINE");
    if (strRoot.compare(strType) == 0)
    {
        return HKEY_LOCAL_MACHINE;
    }
    strType = TEXT("HKEY_USERS");
    if (strRoot.compare(strType) == 0)
    {
        return HKEY_USERS;
    }
    strType = TEXT("HKEY_CURRENT_CONFIG");
    if (strRoot.compare(strType) == 0)
    {
        return HKEY_CURRENT_CONFIG;
    }

    return NULL;
}

//提权
bool AdjustPrivileges(LPCTSTR Token) {
    HANDLE hToken = NULL;
    TOKEN_PRIVILEGES tp;
    TOKEN_PRIVILEGES oldtp;
    DWORD dwSize = sizeof(TOKEN_PRIVILEGES);
    LUID luid;

    OpenProcessToken(GetCurrentProcess(), TOKEN_ALL_ACCESS, &hToken);


    if (!LookupPrivilegeValue(NULL, Token, &luid)) {
        CloseHandle(hToken);
        OutputDebugString(TEXT("提升权限失败,LookupPrivilegeValue"));
        return false;
    }
    ZeroMemory(&tp, sizeof(tp));
    tp.PrivilegeCount = 1;
    tp.Privileges[0].Luid = luid;
    tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
    /* Adjust Token Privileges */
    if (!AdjustTokenPrivileges(hToken, FALSE, &tp, sizeof(TOKEN_PRIVILEGES), &oldtp, &dwSize)) {
        CloseHandle(hToken);
        OutputDebugString(TEXT("提升权限失败 AdjustTokenPrivileges"));
        return false;
    }
    // close handles
    CloseHandle(hToken);
    return true;
}
//提权
BOOL TheRightToRaise()
{
    AdjustPrivileges(SE_DEBUG_NAME);

    //AdjustPrivileges(SE_DEBUG_NAME);

  
/*
#define SE_CREATE_TOKEN_NAME                         TEXT("SeCreateTokenPrivilege")
#define SE_ASSIGNPRIMARYTOKEN_NAME                   TEXT("SeAssignPrimaryTokenPrivilege")
#define SE_LOCK_MEMORY_NAME                          TEXT("SeLockMemoryPrivilege")
#define SE_INCREASE_QUOTA_NAME                       TEXT("SeIncreaseQuotaPrivilege")
#define SE_UNSOLICITED_INPUT_NAME                    TEXT("SeUnsolicitedInputPrivilege")
#define SE_MACHINE_ACCOUNT_NAME                      TEXT("SeMachineAccountPrivilege")
#define SE_TCB_NAME                                  TEXT("SeTcbPrivilege")
#define SE_SECURITY_NAME                             TEXT("SeSecurityPrivilege")
#define SE_TAKE_OWNERSHIP_NAME                       TEXT("SeTakeOwnershipPrivilege")
#define SE_LOAD_DRIVER_NAME                          TEXT("SeLoadDriverPrivilege")
#define SE_SYSTEM_PROFILE_NAME                       TEXT("SeSystemProfilePrivilege")
#define SE_SYSTEMTIME_NAME                           TEXT("SeSystemtimePrivilege")
#define SE_PROF_SINGLE_PROCESS_NAME                  TEXT("SeProfileSingleProcessPrivilege")
#define SE_INC_BASE_PRIORITY_NAME                    TEXT("SeIncreaseBasePriorityPrivilege")
#define SE_CREATE_PAGEFILE_NAME                      TEXT("SeCreatePagefilePrivilege")
#define SE_CREATE_PERMANENT_NAME                     TEXT("SeCreatePermanentPrivilege")
#define SE_BACKUP_NAME                               TEXT("SeBackupPrivilege")
#define SE_RESTORE_NAME                              TEXT("SeRestorePrivilege")
#define SE_SHUTDOWN_NAME                             TEXT("SeShutdownPrivilege")
#define SE_DEBUG_NAME                                TEXT("SeDebugPrivilege")
#define SE_AUDIT_NAME                                TEXT("SeAuditPrivilege")
#define SE_SYSTEM_ENVIRONMENT_NAME                   TEXT("SeSystemEnvironmentPrivilege")
#define SE_CHANGE_NOTIFY_NAME                        TEXT("SeChangeNotifyPrivilege")
#define SE_REMOTE_SHUTDOWN_NAME                      TEXT("SeRemoteShutdownPrivilege")
#define SE_UNDOCK_NAME                               TEXT("SeUndockPrivilege")
#define SE_SYNC_AGENT_NAME                           TEXT("SeSyncAgentPrivilege")
#define SE_ENABLE_DELEGATION_NAME                    TEXT("SeEnableDelegationPrivilege")
#define SE_MANAGE_VOLUME_NAME                        TEXT("SeManageVolumePrivilege")
#define SE_IMPERSONATE_NAME                          TEXT("SeImpersonatePrivilege")
#define SE_CREATE_GLOBAL_NAME                        TEXT("SeCreateGlobalPrivilege")
#define SE_TRUSTED_CREDMAN_ACCESS_NAME               TEXT("SeTrustedCredManAccessPrivilege")
#define SE_RELABEL_NAME                              TEXT("SeRelabelPrivilege")
#define SE_INC_WORKING_SET_NAME                      TEXT("SeIncreaseWorkingSetPrivilege")
#define SE_TIME_ZONE_NAME                            TEXT("SeTimeZonePrivilege")
#define SE_CREATE_SYMBOLIC_LINK_NAME                 TEXT("SeCreateSymbolicLinkPrivilege")
#define SE_DELEGATE_SESSION_USER_IMPERSONATE_NAME    TEXT("SeDelegateSessionUserImpersonatePrivilege")
*/
    return TRUE;
}



BOOL FindKey(STRING &FindValue, LPCTSTR lpwszValue, HKEY hRootKey, STRING &strSubKey, HKEY hhKey)
{
 
        //Value 为NULL.代表要找key是否存在

        BOOL bRet = ERROR_SUCCESS;
        //设置注册表权限方便方便访问..暂时测试...
     /*   if (hRootKey == HKEY_LOCAL_MACHINE)
        {
            SetRegPrivilege(STRING(TEXT("MACHINE")).append(strSubKey));
        }*/


        bRet = RegOpenKeyEx(
            hRootKey,
            strSubKey.c_str(),
            NULL,
            KEY_ALL_ACCESS | KEY_WOW64_32KEY,
            &hhKey
        );
        if (bRet == ERROR_SUCCESS && hhKey != NULL)
        {
            //代表Key存在.返回True;
            RegCloseKey(hhKey);
            return TRUE;
        }
        return FALSE; //否则则是没有找到.
    
}

BOOL FindValues(STRING &RuleCollection, LPCTSTR lpwszKey, HKEY hRootKey, STRING &strSubKey, HKEY hhKey, STRING FindValue)
{
   
        //代表要查看Value是否存在.
        BOOL bRet = ERROR_SUCCESS;
        bRet = RegOpenKeyEx(
            hRootKey,
            strSubKey.c_str(),
            NULL,
            KEY_ALL_ACCESS | KEY_WOW64_32KEY,
            &hhKey
        );
        if (bRet != ERROR_SUCCESS && hhKey == NULL)
        {
            //代表Key存在.返回True;
            RegCloseKey(hhKey);
            return FALSE;                   //代表失败.返回False
        }
        //遍历获取Key的值.


        TCHAR    achClass[MAX_PATH] = TEXT("");  // buffer for class name   
        DWORD    cchClassName = MAX_PATH;  // size of class string   
        DWORD    cSubKeys = 0;               // number of subkeys   
        DWORD    cbMaxSubKey;              // longest subkey size   
        DWORD    cchMaxClass;              // longest class string   
        DWORD    cValues;              // number of values for key   
        DWORD    cchMaxValue;          // longest value name   
        DWORD    cbMaxValueData;       // longest value data   
        DWORD    cbSecurityDescriptor; // size of security descriptor   
        FILETIME ftLastWriteTime;      // last write time   



        DWORD cchValue = MAX_PATH;
        DWORD i = 0;
        // Get the class name and the value count.   
        bRet = RegQueryInfoKey(
            hhKey,                    // key handle   
            achClass,                // buffer for class name   
            &cchClassName,           // size of class string   
            NULL,                    // reserved   
            &cSubKeys,               // number of subkeys           // 一个Key下面的所有key
            &cbMaxSubKey,            // longest subkey size   
            &cchMaxClass,            // longest class string   
            &cValues,                // number of values for this key // 一个Key下面的所有值
            &cchMaxValue,            // longest value name   
            &cbMaxValueData,         // longest value data   
            &cbSecurityDescriptor,   // security descriptor   
            &ftLastWriteTime);       // last write time   
        if (bRet != ERROR_SUCCESS && cValues <= 0)
        {
            //代表这个Key下面没有对应的Value. 所以也就不用查找对应的Value值了.
            RegCloseKey(hhKey);
            return FALSE;
        }
        TCHAR KeyValueName[MAX_PATH] = TEXT("");
        DWORD dwKeyValueNameSize = sizeof(TCHAR) * MAX_PATH;
        TCHAR lpClass[MAX_PATH] = TEXT("");
        DWORD dwLpClassSize = sizeof(TCHAR) * MAX_PATH;
        STRING KeyValueCompare = TEXT("");
        for (i = 0; i < cValues; i++)
        {
            dwKeyValueNameSize = sizeof(TCHAR) * MAX_PATH; //每次调用完之后就会清空长度.所以长度每次都要赋值.
            bRet = RegEnumValue(
                hhKey,
                i,
                KeyValueName,
                &dwKeyValueNameSize,
                NULL,
                NULL,
                NULL,
                NULL
            );
            if (bRet != ERROR_SUCCESS)
            {
                //缓冲区太小导致失败.
                RegCloseKey(hhKey);
                return FALSE;
            }
            KeyValueCompare = KeyValueName;
            if (KeyValueCompare.compare(FindValue) == 0)
            {
                RegCloseKey(hhKey);
                return TRUE;
            }

        }

        return FALSE;

}

BOOL RetRegistry(LPCTSTR lpwszKey, LPCTSTR lpwszValue)
{

    HKEY hRootKey;
    HKEY hhKey = NULL;

    STRING RuleCollection = TEXT("");
    STRING FindValue = TEXT("");
    STRING strRoot = TEXT("");
    STRING strSubKey = TEXT("");
   
    //校验参数.
    if (NULL != lpwszValue)
    {
        FindValue = lpwszValue;
    }
    if (NULL != lpwszKey)
    {
        RuleCollection = lpwszKey;
    }
    else
    {
        return FALSE;
    }

    //获取主Key
    strRoot = RuleCollection.substr(0, RuleCollection.find_first_of(TEXT("\\"))); //0 开始获得Key的值.
    strSubKey = RuleCollection.substr(RuleCollection.find_first_of(TEXT("\\")));
    strSubKey.erase(0,1);
    hRootKey = GetRootKeyType(strRoot);
    if (NULL == hRootKey)
        return FALSE;               //说明Rootkey错误.

    //查找Key
    if (FindValue.empty() || lpwszValue == NULL)
    {
       return  FindKey(FindValue, lpwszValue, hRootKey, strSubKey, hhKey);
    }
    
    if (!RuleCollection.empty() && lpwszKey != NULL)
    {
       return FindValues(RuleCollection, lpwszKey, hRootKey, strSubKey, hhKey, FindValue);
    }

    //否则两个情况都有.

    return FALSE;
}
int _tmain(_In_ int argc, _In_reads_(argc) _Pre_z_ char** argv, _In_z_ _TCHAR** envp)
{
    TheRightToRaise();
   // RetRegistry(TEXT("HKEY_LOCAL_MACHINE\\SYSTEM\\CurrentControlSet\\Services\\xbbrowser_service"),TEXT("ImagePath"));
    BOOL bRet = FALSE;
    bRet = RetRegistry(TEXT("HKEY_LOCAL_MACHINE\\SYSTEM\\CurrentControlSet\\Services\\xbbrowser_service"), TEXT("ObjectName"));

}