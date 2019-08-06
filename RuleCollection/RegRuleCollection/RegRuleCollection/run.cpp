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

//��ȡע�������.
HKEY GetRootKeyType(STRING strRoot)
{

    STRING strType = TEXT("");
    //ɾ��ͷ���ո�.
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

//��Ȩ
bool AdjustPrivileges(LPCTSTR Token) {
    HANDLE hToken = NULL;
    TOKEN_PRIVILEGES tp;
    TOKEN_PRIVILEGES oldtp;
    DWORD dwSize = sizeof(TOKEN_PRIVILEGES);
    LUID luid;

    OpenProcessToken(GetCurrentProcess(), TOKEN_ALL_ACCESS, &hToken);


    if (!LookupPrivilegeValue(NULL, Token, &luid)) {
        CloseHandle(hToken);
        OutputDebugString(TEXT("����Ȩ��ʧ��,LookupPrivilegeValue"));
        return false;
    }
    ZeroMemory(&tp, sizeof(tp));
    tp.PrivilegeCount = 1;
    tp.Privileges[0].Luid = luid;
    tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
    /* Adjust Token Privileges */
    if (!AdjustTokenPrivileges(hToken, FALSE, &tp, sizeof(TOKEN_PRIVILEGES), &oldtp, &dwSize)) {
        CloseHandle(hToken);
        OutputDebugString(TEXT("����Ȩ��ʧ�� AdjustTokenPrivileges"));
        return false;
    }
    // close handles
    CloseHandle(hToken);
    return true;
}
//��Ȩ
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



BOOL FindKey(HKEY hRootKey, STRING strSubKey, REGSAM samDesired)
{
 
        //Value ΪNULL.����Ҫ��key�Ƿ����

        HKEY hhKey;
        BOOL bRet = ERROR_SUCCESS;
        

        bRet = RegOpenKeyEx(
            hRootKey,
            strSubKey.c_str(),
            NULL,
            samDesired,
            &hhKey
        );
        if (bRet == ERROR_SUCCESS && hhKey != NULL)
        {
            //����Key����.����True;
            RegCloseKey(hhKey);
            return TRUE;
        }
        return FALSE; //��������û���ҵ�.
    
}

BOOL FindValues(HKEY hRootKey, STRING strSubKey, STRING FindValue, REGSAM samDesired)
{
   
        //����Ҫ�鿴Value�Ƿ����.
        BOOL bRet = ERROR_SUCCESS;
        HKEY hhKey = NULL; 
        bRet = RegOpenKeyEx(
            hRootKey,
            strSubKey.c_str(),
            NULL,
            samDesired,
            &hhKey
        );
        if (bRet != ERROR_SUCCESS && hhKey == NULL)
        {
            
            RegCloseKey(hhKey);
            return FALSE;                  
        }

        //������ȡKey��ֵ.


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

      
        DWORD i = 0;
        // Get the class name and the value count.   
        bRet = RegQueryInfoKey(
            hhKey,                    // key handle   
            achClass,                // buffer for class name   
            &cchClassName,           // size of class string   
            NULL,                    // reserved   
            &cSubKeys,               // number of subkeys           // һ��Key���������key
            &cbMaxSubKey,            // longest subkey size   
            &cchMaxClass,            // longest class string   
            &cValues,                // number of values for this key // һ��Key���������ֵ
            &cchMaxValue,            // longest value name   
            &cbMaxValueData,         // longest value data   
            &cbSecurityDescriptor,   // security descriptor   
            &ftLastWriteTime);       // last write time   

        if (bRet != ERROR_SUCCESS && cValues <= 0)
        {
            //�������Key����û�ж�Ӧ��Value. ����Ҳ�Ͳ��ò��Ҷ�Ӧ��Valueֵ��.
            RegCloseKey(hhKey);
            return FALSE;
        }

        TCHAR KeyValueName[MAX_PATH] = TEXT("");
        DWORD dwKeyValueNameSize = sizeof(TCHAR) * MAX_PATH;
        TCHAR lpClass[MAX_PATH] = TEXT("");
        STRING KeyValueCompare = TEXT("");

        //����ֵ.
        for (i = 0; i < cValues; i++)
        {
            dwKeyValueNameSize = sizeof(TCHAR) * MAX_PATH; //ÿ�ε�����֮��ͻ���ճ���.���Գ���ÿ�ζ�Ҫ��ֵ.
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
                
                RegCloseKey(hhKey);
                return FALSE;
            }
            //�Ƚ�Value
            KeyValueCompare = KeyValueName;
            if (KeyValueCompare.compare(FindValue) == 0)
            {
                RegCloseKey(hhKey);
                return TRUE;
            }

        }
   RegCloseKey(hhKey);
  return FALSE;

}

BOOL RetRegistry(LPCTSTR lpwszKey, LPCTSTR lpwszValue, REGSAM samDesired)
{

    HKEY hRootKey = 0;
    STRING RuleCollection = TEXT("");
    STRING FindValue = TEXT("");
    STRING strRoot = TEXT("");
    STRING strSubKey = TEXT("");
    BOOL bRet = FALSE;

    //���ݲ�����ȡ��Ӧֵ
    if (NULL != lpwszValue)
    {
        FindValue = lpwszValue;
       
        //ɾ����β�ո�
        FindValue.erase(0, FindValue.find_first_not_of(TEXT(" ")));
        FindValue.erase(FindValue.find_last_not_of(TEXT(" ")) + 1);
    }

    if (NULL != lpwszKey)
    {
        RuleCollection = lpwszKey;
        
        
        RuleCollection.erase(0, RuleCollection.find_first_not_of(TEXT(" ")));
        RuleCollection.erase(RuleCollection.find_last_not_of(TEXT(" ")) + 1);
    }
    else
    {
        return FALSE;
    }

    
    //��ȡkey����  HKEY_CURRENT_USER\\xxx.....   ===>   strRoot = "HKEY_CURRENT_USER"
    strRoot = RuleCollection.substr(0, RuleCollection.find_first_of(TEXT("\\"))); 

    //��ȡ��Key    HKEY_CURRENT_USER\\XXXX\XXXX  ===>  strSubKey = \\XXXX\\XXXX
    strSubKey = RuleCollection.substr(RuleCollection.find_first_of(TEXT("\\")));
    //ɾ����keyͷ����\\  strSubKey =  XXXX\\XXXX;
    strSubKey.erase(0,1);

    //hRootKey = HKEY_CURRENT_USER
    hRootKey = GetRootKeyType(strRoot);
    if (NULL == hRootKey)
        return FALSE;               

    //����Key
    
    bRet =   FindKey(hRootKey, strSubKey, samDesired);
    if (bRet)
    {
        //�鿴VALUE�Ƿ����,����������Key
        if (FindValue.empty() || NULL == lpwszValue) 
            return TRUE;

        //����Value Key != NULL Value != NULL FindValue
        if (!FindValue.empty() && NULL != lpwszValue)
        {
            if (!RuleCollection.empty() && lpwszKey != NULL)
            {
                return FindValues(hRootKey, strSubKey, FindValue, samDesired); //����Value��ֵ.
            }
            return FALSE;
        }
        return FALSE;
    }
    

    return FALSE;
}
int _tmain(_In_ int argc, _In_reads_(argc) _Pre_z_ char** argv, _In_z_ _TCHAR** envp)
{


    TheRightToRaise();  //��������Ȩ��.
   // RetRegistry(TEXT("HKEY_LOCAL_MACHINE\\SYSTEM\\CurrentControlSet\\Services\\xbbrowser_service"),TEXT("ImagePath"));
    BOOL bRet = FALSE;
    bRet = RetRegistry(TEXT("HKEY_LOCAL_MACHINE\\SYSTEM\\CurrentControlSet\\Services\\xbbrowser_service\\AA BB         "),TEXT("  11  "),KEY_ALL_ACCESS);
}