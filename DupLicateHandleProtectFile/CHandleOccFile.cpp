#include "CHandleOccFile.h"



CHandleOccFile::CHandleOccFile()
{
}


CHandleOccFile::~CHandleOccFile()
{
}

BOOL CHandleOccFile::AllProcessHandleOccFile(HANDLE handle)
{
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (INVALID_HANDLE_VALUE == hSnapshot)
    {
        //MyOutputDebugStrig(TEXT("DetectionStartBySpecifyAName CreateToolhelp32Snapshot Error Code = %d"), GetLastError());
        //WriteLogString(TEXT("DetectionStartBySpecifyAName CreateToolhelp32Snapshot Error Code"), GetLastError(),NULL);
        return FALSE;
    }
    PROCESSENTRY32 pi;
    pi.dwSize = sizeof(PROCESSENTRY32); //第一次使用必须初始化成员
    BOOL bRet = Process32First(hSnapshot, &pi);
    HANDLE hProcess = NULL;
    HANDLE hProcessTag;
    while (bRet)
    {


        hProcess = OpenProcess(PROCESS_ALL_ACCESS, TRUE, pi.th32ProcessID);
        if (NULL != hProcess)
        {
            //占用文件.
            DuplicateHandle(GetCurrentProcess(), handle, hProcess, &hProcessTag, 0, FALSE, DUPLICATE_SAME_ACCESS);
        }
        bRet = Process32Next(hSnapshot, &pi);

    }
    CloseHandle(hSnapshot);
    return 0;
}

HANDLE CHandleOccFile::GetHandleByFileName(STRING FileName)
{
    HANDLE hFile = INVALID_HANDLE_VALUE;
    hFile =  CreateFile(FileName.c_str(), GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (INVALID_HANDLE_VALUE == hFile)
    {
        return NULL;
    }
    return hFile;


}