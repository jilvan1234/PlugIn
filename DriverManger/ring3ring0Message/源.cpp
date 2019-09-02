#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <tchar.h>
#include <string>
#include "../../publicstruct.h"
#include "../../DriverManger/CmRegCallBack/struct.h" //����������ͷ�ļ�
#include "../../RegManger/RegManger/CRegManger.h"
#include "../../RegManger/RegManger/CRegManger.cpp"
#include <sddl.h>

using namespace std;


#define ROOT_KEY_NAME L"Zy360"
void InitCmCreateKey(HANDLE hFile);
void InitRootKey(HANDLE hFile);
void SetRootKeyCommandValue(HANDLE hFile);
void SetCurretKeyUserChoiceValue(HANDLE hFile,CBinString strKeyPathName);
CBinString GetHkeyUserName();   //��ȡHkeyPath��·��


void InitCmCreateKey(HANDLE hFile)
{
    ULONG uSSDTId = 26;
    DWORD dwRetSize = 0;
    DeviceIoControl(hFile, CTL_INIT_CMCREATEKEY, &uSSDTId, sizeof(ULONG), NULL, NULL,&dwRetSize, NULL);
}
void SetCurretKeyUserChoiceValue(HANDLE hFile, CBinString strKeyPathName)
{
    /*
        ����RootKey�����Command��ֵ.
        */
    SET_KEYPATH_VALUENAME_VALUEDATA pSetKeyValueData = { 0 };
    CRegManger reg;
    DWORD dwOutSize = 0;

    //��ʼ���ṹ.
    pSetKeyValueData.KeyPathName = new wchar_t[0x200];
    pSetKeyValueData.ValueName = new wchar_t[0x200];
    pSetKeyValueData.ValueData = new wchar_t[0x200];
    pSetKeyValueData.ValueDataSize = 0;

    //���г�ʼ��
  
   
    wcscpy(pSetKeyValueData.KeyPathName, strKeyPathName.c_str());  //����Command

    


    //ƴ�Ӵ�������·��Ϊ "xxx·��" -- "%1"
    //����Ҫ����˫����

    wcscpy(pSetKeyValueData.ValueName, L"ProgId"); //Ϊ�ջ���NULL������Ĭ��ֵ.
    wcscpy(pSetKeyValueData.ValueData, ROOT_KEY_NAME);
    pSetKeyValueData.ValueDataSize = sizeof(ROOT_KEY_NAME) * sizeof(TCHAR);
    pSetKeyValueData.ControlCode = 0x2;                  //1 ������.
    DWORD dwSize = (sizeof(0x200) * 4) * sizeof(TCHAR) + 4;
    DeviceIoControl(hFile, CTL_SETKEYVALUE, &pSetKeyValueData, dwSize, NULL, NULL, &dwOutSize, NULL); //����ֵ.
}



void SetRootKeyCommandValue(HANDLE hFile)
{
    /*
    ����RootKey�����Command��ֵ.
    */
    SET_KEYPATH_VALUENAME_VALUEDATA pSetKeyValueData = { 0 };
    CRegManger reg;

    //��ʼ���ṹ.
    pSetKeyValueData.KeyPathName = new wchar_t[MAX_PATH];
    pSetKeyValueData.ValueName = new wchar_t[MAX_PATH];
    pSetKeyValueData.ValueData = new wchar_t[MAX_PATH];
    pSetKeyValueData.ValueDataSize = 0;

    //���г�ʼ��
    CBinString strValueNamePath = TEXT("\"");
    wstring  str = L"\\Registry\\Machine\\SoftWare\\Classes\\";
    str.append(ROOT_KEY_NAME);
    str.append(L"\\shell\\open\\command");

   
  
    wcscpy(pSetKeyValueData.KeyPathName,str.c_str());  //����Command
 
    DWORD dwOutSize = 0;
    strValueNamePath = strValueNamePath.append(reg.ReGetSpecifiedKeyValue(HKEY_LOCAL_MACHINE, TEXT("SYSTEM\\ControlSet001\\Services\\xbbrowser_service"), TEXT("ImagePath"), REG_EXPAND_SZ));
    strValueNamePath  = strValueNamePath.substr(0, strValueNamePath.find_last_of(TEXT("\\")));
    strValueNamePath = strValueNamePath.substr(0, strValueNamePath.find_last_of(TEXT("\\")));
    strValueNamePath = strValueNamePath.append(TEXT("\\xbstarter.exe"));
    strValueNamePath = strValueNamePath.append(TEXT("\""));
    //-- "%1"
    strValueNamePath = strValueNamePath.append(TEXT(" -- \"%1\""));

    //ƴ�Ӵ�������·��Ϊ "xxx·��" -- "%1"
    //����Ҫ����˫����

    wcscpy(pSetKeyValueData.ValueName, L""); //Ϊ�ջ���NULL������Ĭ��ֵ.
    wcscpy(pSetKeyValueData.ValueData, strValueNamePath.c_str());
    pSetKeyValueData.ValueDataSize = strValueNamePath.size() * sizeof(TCHAR) + 1;
    pSetKeyValueData.ControlCode = 0x1;                  //1 ������.
    DWORD dwSize = (sizeof(MAX_PATH) * 4) * sizeof(TCHAR) + 4;
    DeviceIoControl(hFile, CTL_SETKEYVALUE, &pSetKeyValueData, dwSize, NULL, NULL, &dwOutSize, NULL); //����ֵ.
}
void InitRootKey(HANDLE hFile)
{

    BOOL bRet = 0;
    DWORD dwRet;
    PINIT_ROOTKEY pRootKey = new INIT_ROOTKEY;
    pRootKey->KeyPathName = new wchar_t[MAX_PATH];
    //��ʼ����Ҫ���г�ʼ����ROOT KEY������
    wstring str = L"\\Registry\\Machine\\SoftWare\\Classes\\";
    str.append(ROOT_KEY_NAME);
    wcscpy(pRootKey->KeyPathName, str.c_str());
    
  
     DeviceIoControl(hFile, CTL_INITROOTKEY, pRootKey, sizeof(pRootKey), NULL,NULL , &dwRet, NULL);
}

void RemoveCmCallBack(HANDLE hFile)
{
    ULONG dwRet;
    DeviceIoControl(hFile, CM_REMOVE_CALLBACK, NULL, NULL, NULL, NULL, &dwRet, NULL);
}
int _tmain(_In_ int argc, _In_reads_(argc) _Pre_z_ _TCHAR** argv, _In_z_ _TCHAR** envp)
{



    HANDLE hFile = CreateFile(TEXT("\\\\?\\InterceptLink"),
        GENERIC_WRITE | GENERIC_READ,
        FILE_SHARE_READ | FILE_SHARE_WRITE,
        NULL,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,
        NULL);
    
    if (hFile != INVALID_HANDLE_VALUE)
    {
        
        ////��ʼ��Cm���� CTL_INIT_CMCREATEKEY
        //InitCmCreateKey(hFile);
        //InitRootKey(hFile);//��ʼ��RootKeyĿ¼. 
       

        //SetRootKeyCommandValue(hFile);//��ʼCommand�����ֵ.
        ////����HKEY_CURRENT_USER\Software\Microsoft\Windows\Shell\Associations\UrlAssociations\http\UserChoice] �� ProgidΪ �ո����õ��ֶμ���.
        //wstring  str = GetHkeyUserName();
        //str.append(TEXT("Software\\Microsoft\\Windows\\Shell\\Associations\\UrlAssociations\\http\\UserChoice"));
        //SetCurretKeyUserChoiceValue(hFile,str);         //����HTTP

        //str = GetHkeyUserName() + TEXT("Software\\Microsoft\\Windows\\Shell\\Associations\\UrlAssociations\\https\\UserChoice");
        //SetCurretKeyUserChoiceValue(hFile, str);        //����Https

        RemoveCmCallBack(hFile);
    }
   

    CloseHandle(hFile);
    //SetRootKeyCommandValue(NULL);

    
   
   
    return 0;
}
    



CBinString  GetHkeyUserName()
{
    CBinString str = TEXT("\\Registry\\User\\");
    HANDLE hProcess = GetCurrentProcess();
    if (!hProcess) {
        return 0;
    }

    HANDLE hToken;
    if (!OpenProcessToken(hProcess, TOKEN_QUERY, &hToken) || !hToken) {
        CloseHandle(hProcess);
        return 0;
    }

    DWORD dwTemp = 0;
    char tagTokenInfoBuf[256] = { 0 };
    PTOKEN_USER tagTokenInfo = (PTOKEN_USER)tagTokenInfoBuf;
    if (!GetTokenInformation(hToken, TokenUser, tagTokenInfoBuf, sizeof(tagTokenInfoBuf), \
        &dwTemp)) {
        CloseHandle(hToken);
        CloseHandle(hProcess);
        return 0;
    }

    /* typedef BOOL(WINAPI* PtrConvertSidToStringSid)(
         PSID Sid,
         LPTSTR* StringSid
         );


     PtrConvertSidToStringSid dwPtr = (PtrConvertSidToStringSid)GetProcAddress(
         LoadLibrary(TEXT("Advapi32.dll")), "ConvertSidToStringSidA");

     ConvertSidToStringSidA()*/

    LPTSTR MySid = NULL;
    ConvertSidToStringSid(tagTokenInfo->User.Sid, (LPTSTR*)&MySid);

  
    str.append(MySid);
    LocalFree((HLOCAL)MySid);
    str.append(TEXT("\\"));
    CloseHandle(hToken);
    CloseHandle(hProcess);

    return str;
}


