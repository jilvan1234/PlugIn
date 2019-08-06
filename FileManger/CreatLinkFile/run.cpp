#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <tchar.h>
#include "atlconv.h"
#include <shlobj_core.h>

bool CreatLinkToStartMenu(char * pszPeFileName);

int _tmain(_In_ int argc, _In_reads_(argc) _Pre_z_ _TCHAR** argv, _In_z_ _TCHAR** envp)
{
    char szPath[] = TEXT("C:\\Users\Administrator\\AppData\\Local\\360Chrome\\Chrome\\Application\\360chrome.exe");
    CreatLinkToStartMenu(szPath);
}






bool CreatLinkToStartMenu(char * pszPeFileName)
{

    HRESULT hr = CoInitialize(NULL);
    if (SUCCEEDED(hr))
    {
        IShellLink *pisl;
        hr = CoCreateInstance(CLSID_ShellLink, NULL,
            CLSCTX_INPROC_SERVER, IID_IShellLink, (void**)&pisl);
        if (SUCCEEDED(hr))
        {
            IPersistFile* pIPF;

            /////////////////////////////////////////////////////////////////////////////////////////////////////////////

            //����������Ҫ������ݷ�ʽ��ԭʼ�ļ���ַ
            pisl->SetPath(pszPeFileName);
            hr = pisl->QueryInterface(IID_IPersistFile, (void**)&pIPF);
            if (SUCCEEDED(hr))
            {
                /////////////////////////////////////////////////////////////////////////////////////////////////////////////

                //����������Ҫ������ݷ�ʽ��Ŀ���ַ
                char szStartPath[MAX_PATH] = { 0 };
                SHGetSpecialFolderPath(NULL, szStartPath, CSIDL_STARTUP, 0);
                strcat(szStartPath, "\\Windows Message.lnk");

                USES_CONVERSION;
                LPCOLESTR lpOleStr = A2COLE(szStartPath);

                pIPF->Save(lpOleStr, FALSE);

                pIPF->Release();
            }
            pisl->Release();
        }
        CoUninitialize();
    }

    return true;
}

