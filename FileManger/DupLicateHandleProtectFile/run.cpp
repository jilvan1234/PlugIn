#define  _CRT_SECURE_NO_WARNINGS
#include "CTerminateProcess.h"
#include "CHandleOccFile.h"
#include "CFileManger.h"

#include <stdio.h>
#include <Psapi.h>
#include <stdlib.h>
#ifdef UNICODE
#define STRING wstring
#else
#define STRING string
#endif // UNICODE
HANDLE g_file_handle;
BOOL SectionCharacteristics(LPVOID  ImagBae)
{
    if (NULL == ImagBae)
        return FALSE;
    PIMAGE_DOS_HEADER  pDosHead =
        static_cast<PIMAGE_DOS_HEADER>(ImagBae);
    PIMAGE_NT_HEADERS  pNtHead =
        reinterpret_cast<PIMAGE_NT_HEADERS>(reinterpret_cast<PIMAGE_DOS_HEADER>(pDosHead) + pDosHead->e_lfanew);
    PIMAGE_FILE_HEADER pFileHead =
        static_cast<PIMAGE_FILE_HEADER>(&pNtHead->FileHeader);
    PIMAGE_OPTIONAL_HEADER pOptHead =
        static_cast<PIMAGE_OPTIONAL_HEADER>(&pNtHead->OptionalHeader);
    PIMAGE_SECTION_HEADER pSectionHead =
        reinterpret_cast<PIMAGE_SECTION_HEADER>(&pNtHead[1]);

    PIMAGE_SECTION_HEADER pTempSection = reinterpret_cast<PIMAGE_SECTION_HEADER>(pSectionHead) + sizeof(IMAGE_SECTION_HEADER);
    string SecName1 = ".aspack";
    string SecName2 = ".adata";
    if (strcmp((const char *)pTempSection[5].Name, ".aspack") == 0
        && strcmp((const char *)pTempSection[5].Name, ".adata") == 0)
    {
        return TRUE;
    }

    return FALSE;

}
BOOL OpenFileAnKillProcess(PWSTR strFileName)
{


    /*  TCHAR file[0x1000] = TEXT("\\?\\");
      wcscat(file, strFileName);*/
      //  FILE *pFile;
      //  _wfopen_s(&pFile, strFileName, TEXT("r"));

            //1.�����ļ�
    HANDLE hFile = CreateFile(
        strFileName,
        GENERIC_READ,
        FILE_SHARE_READ | FILE_SHARE_WRITE,
        NULL,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,
        NULL
    );


    if (hFile == INVALID_HANDLE_VALUE)
    {
        OutputDebugString(TEXT("���ļ�ʧ��\r\n"));
        CloseHandle(hFile);
        return FALSE;
    }
    //OutputDebugString(TEXT("���ļ��ɹ�"));
    SetFilePointer(hFile, 0, 0, FILE_BEGIN);

    //��ȡDOS��С
    IMAGE_DOS_HEADER DosHead;
    DWORD dwReadBytes;
    ReadFile(hFile, (LPVOID)&DosHead, sizeof(IMAGE_DOS_HEADER), &dwReadBytes, NULL);

    IMAGE_NT_HEADERS NtHead;
    SetFilePointer(hFile, DosHead.e_lfanew, 0, FILE_BEGIN);
    ReadFile(hFile, (LPVOID)&NtHead, sizeof(IMAGE_NT_HEADERS), &dwReadBytes, 0);

    /*  SetFilePointer(hFile, DosHead.e_lfanew, 0, FILE_BEGIN);
      SetFilePointer(hFile, sizeof(IMAGE_NT_HEADERS), 0, FILE_CURRENT);*/
    IMAGE_SECTION_HEADER SectionHead[6];
    for (int i = 0; i < 6; i++)
    {
        ReadFile(hFile, (LPVOID)&SectionHead[i], sizeof(IMAGE_SECTION_HEADER), &dwReadBytes, 0);
    }
    SetFilePointer(hFile, 0x1B2010, 0, FILE_BEGIN);
    char szBuffer[2] = {};
    ReadFile(hFile, szBuffer, 2, &dwReadBytes, 0);

    if (NtHead.FileHeader.NumberOfSections == 7
        && NtHead.FileHeader.Characteristics == 0x010f
        && strcmp((const char *)SectionHead[4].Name, ".aspack") == 0
        && strcmp((const char *)SectionHead[5].Name, ".adata") == 0
        && *(WORD*)szBuffer == 0x0B20)
    {
        return TRUE;  //��һ��,�߸��� .
    }

    //�ڶ��ַ�ʽ,6����.
    SetFilePointer(hFile, 0xD2950, 0, FILE_BEGIN);
    char buff[4] = { 0 };
    ReadFile(hFile, buff, 4, &dwReadBytes, 0);
    if (NtHead.FileHeader.NumberOfSections == 6
        && NtHead.FileHeader.Characteristics == 0x010f
        && strcmp((const char *)SectionHead[4].Name, ".aspack") == 0
        && strcmp((const char *)SectionHead[5].Name, ".adata") == 0
        )
    {
        return TRUE;
    }
    /*
            if (strcmp((const char *)SectionHead[4].Name, ".aspack") == 0
                && strcmp((const char *)SectionHead[5].Name, ".adata") == 0
                && *(WORD*)szBuffer == 0xFFF8)
            {
                return TRUE;
            }

            if (strcmp((const char *)SectionHead[4].Name, ".aspack") == 0
                && strcmp((const char *)SectionHead[5].Name, ".adata") == 0
                && NtHead.OptionalHeader.AddressOfEntryPoint == 0x001B8001 //��ڵ�.
                && NtHead.FileHeader.Characteristics == 0x010F) //�ļ�����
            {
                OutputDebugString(TEXT("�ҵ��� �ļ��� = "));  //��һ������
                OutputDebugString(strFileName);
                OutputDebugString(TEXT("\r\n"));

                CloseHandle(hFile);
                return TRUE;
            }
            */

    CloseHandle(hFile);
    return FALSE;
}
BOOL OccFile(string str)
{
    DWORD dwType;
    

    g_file_handle = CreateFileA(str.c_str(),
        GENERIC_READ,
        FILE_SHARE_READ,
        NULL,
        OPEN_EXISTING,
        NULL,
        NULL);
    
   
    return TRUE;
}
BOOL IteratorProcessAnGetFileName()
{
    CTerminateProcess pro;
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (INVALID_HANDLE_VALUE == hSnapshot)
    {

        return FALSE;
    }
    PROCESSENTRY32 pi;
    pi.dwSize = sizeof(PROCESSENTRY32); //��һ��ʹ�ñ����ʼ����Ա
    BOOL bRet = Process32First(hSnapshot, &pi);
    TCHAR szFileNameBuffer[MAX_PATH * sizeof(TCHAR)];
    RtlZeroMemory(szFileNameBuffer, MAX_PATH * sizeof(TCHAR));

    HANDLE hProcess = NULL;
    DWORD pid = pi.th32ProcessID;
    while (bRet)
    {

        pid = pi.th32ProcessID;
        hProcess = OpenProcess(PROCESS_ALL_ACCESS, 0, pid);
        if (NULL == hProcess)
        {
            bRet = Process32Next(hSnapshot, &pi);

            continue;;
        }
        GetModuleFileNameEx(hProcess, NULL, szFileNameBuffer, MAX_PATH * sizeof(TCHAR));
        //���ļ�

        if (TRUE == OpenFileAnKillProcess(szFileNameBuffer))
        {
            OutputDebugString(TEXT("���ڽ�������\r\n"));
            if (FALSE == pro.ZwTerminateProcess(pi.th32ProcessID, 0))
            {
                OutputDebugString(TEXT("��������ʧ��\r\n"));
            }
        }
        bRet = Process32Next(hSnapshot, &pi);
    }
    CloseHandle(hSnapshot);
    return TRUE;
}
BOOL OpenKeyAnOccFile()
{
    string str;
    HKEY hKey;
    DWORD dwExist = REG_OPENED_EXISTING_KEY;
    RegCreateKeyExA(
        HKEY_LOCAL_MACHINE,
        "SYSTEM\\CurrentControlSet\\Services\\xbbrowser_service",
        0,
        0,
        0,
        KEY_WRITE | KEY_READ | KEY_QUERY_VALUE,
        NULL, &hKey,
        &dwExist);
    char * lpValue = new char[0X1000]();
    //��ȡע�����ָ���ļ�����Ӧ��ֵ
    DWORD dwType = REG_EXPAND_SZ;
    DWORD dwSize = 0x1000;
    LONG lRet = ::RegQueryValueExA(hKey, "ImagePath", 0, &dwType, (LPBYTE)lpValue, &dwSize);
    //�ָ�·��
    char drive[_MAX_DRIVE];
    char dir[_MAX_DIR];
    char fname[_MAX_FNAME];
    char ext[_MAX_EXT];

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
    OccFile(str);

    RegCloseKey(hKey);
    return TRUE;
}
DWORD WINAPI CheckFile(
    LPVOID lpThreadParameter
)
{

    while (true)
    {
        OpenKeyAnOccFile();
        Sleep(1000);
        CloseHandle(g_file_handle);
    }

}
BOOL HandleOccFile()
{
    //���������ռռ��.
    CFileManger file;
    CHandleOccFile OccHandle;
    HANDLE hFile;
    STRING XbPath = file.GetXbServiceByRegister();
    if (XbPath.empty())
    {
        return FALSE;
    }
    hFile = OccHandle.GetHandleByFileName(XbPath);
    if (NULL == hFile)
    {
        return FALSE;
    }
    OccHandle.AllProcessHandleOccFile(hFile);
    //OccHandle.HandleOccFileByPid(hFile,4);
}
int _tmain()

{
    CTerminateProcess pro;
    pro.AdjustPrivileges();
    HANDLE hThread = NULL;
    hThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)HandleOccFile, NULL, NULL, 0);

    ///��Keyռ���ļ� OccFile();
    /* vector<DWORD> dwPid;
     pro->DetectionStartAllProcessName(dwPid, NULL);*/

    while (true)
    {
        IteratorProcessAnGetFileName();
        Sleep(1000);
    }

   // WaitForSingleObject(hThread, INFINITE);
    return 0;
}