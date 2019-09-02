/*
1.�������н���
2.�������н����ڴ�
3.�ҳ�MAP�����ڴ�.
    4.���ӳ���ַ, ����. �Լ���ַ���������.

*/

#include <stdio.h>
#include <stdlib.h>
#include <tchar.h>
#include "../../ClassManger/CNativeApi/CNativeApiManger.h"
#include "../../ProcessManger/ProcessIterator/CProcessOpt.h"
#include "../../FileManger/CFileManger/CFileManger.h"
#include <math.h>
#include "../../publicstruct.h"
#include <Psapi.h>
#include "run.h"
#include <TlHelp32.h>


#ifdef _WIN64
#pragma comment(lib,"../../PulicLib/x64/Release/CNativeApi.lib")
#pragma comment(lib,"../../PulicLib/x64/Release/ProcessIterator.lib")
#pragma comment(lib,"../../PulicLib/x64/Release/RegManger.lib")
#pragma comment(lib,"../../PulicLib/x64/Release/CFileManger.lib")
#pragma comment(lib,"../../PulicLib/x64/Release/RemoteThread.lib")
#else
#pragma comment(lib,"../../PulicLib/Win32/Release/CNativeApi.lib")
#pragma comment(lib,"../../PulicLib/Win32/Release/ProcessIterator.lib")
#pragma comment(lib,"../../PulicLib/Win32/Release/RegManger.lib")
#pragma comment(lib,"../../PulicLib/Win32/Release/CFileManger.lib")
#pragma comment(lib,"../../PulicLib/Win32/Release/RemoteThread.lib")
#endif


CProcessOpt g_PsOpt;

USHORT g_dwType = 0;

typedef struct _PATH_HANDLE
{
    HANDLE hTarHandle;
    CBinString DirPath;

}PATH_HANDLE, *PPATH_HANDLE;


#define GLOBAL_DIR TEXT("D:\\SharMemory");

int MmGetSectionMemoryAnWriteFile(const HANDLE &hProcess, DWORD dwPid,CBinString Path);

//���������µ�ģ����Ϣ��Ҫ�õ�.
PfnZwQueryInformationProcess ZwQueryInformationProcess = reinterpret_cast<PfnZwQueryInformationProcess>(g_PsOpt.MmGetAddress(TEXT("ntdll.dll"), "ZwQueryInformationProcess"));
//���ݽ��̾����ȡ������Ϣ.
PfnZwQueryObject g_NtQueryObject = reinterpret_cast<PfnZwQueryObject>(g_PsOpt.MmGetAddress(TEXT("ntdll.dll"), "NtQueryObject"));

PfnZwQuerySystemInformation ZwQuerySystemInformation = reinterpret_cast<PfnZwQuerySystemInformation>(g_PsOpt.MmGetAddress(TEXT("ntdll.dll"),"ZwQuerySystemInformation"));

//���ݾ����ȡ�ļ���·��.

// ƴ��·��,�����ļ���.
CBinString SplicingPathFileName(CBinString DirPath, CProcessOpt &PsOpt, DWORD dwPid, PMEMORY_BASIC_INFORMATION pMemInfo)
{
    CBinString PathName = TEXT("");
    CBinString PmProtect = TEXT("");
    DirPath = DirPath + TEXT("\\");
    PathName = DirPath;

    PathName = PathName + PsOpt.PsGetProcessFileNameByProcessId(dwPid);

    PathName = PathName.substr(0, PathName.find_last_of(TEXT(".")));
    PathName += TEXT("_";)
#ifdef UNICODE
    PathName += std::to_wstring(PsOpt.PsGetProcessIdByProcessFileName(PsOpt.PsGetProcessFileNameByProcessId(dwPid)));
#else
    PathName += std::to_string(PsOpt.PsGetProcessIdByProcessFileName(PsOpt.PsGetProcessFileNameByProcessId(dwPid)));
#endif
    PathName += TEXT("_");

    //��ַתΪ16����.
    TCHAR HexBuffer[0x100] = { 0 };
#ifdef UNICODE
    wsprintf(HexBuffer, TEXT("%p"), pMemInfo->BaseAddress);
#else
    sprintf_s(HexBuffer, 0x100, "%p", pMemInfo->BaseAddress);
#endif
   
    PathName += HexBuffer;

    PathName += TEXT("_");
    // д���ڴ�����

    switch (pMemInfo->Protect) //�������ͱȽ�
    {
    case PAGE_EXECUTE: PmProtect = TEXT("PAGE_EXECUTE"); break;
    case PAGE_EXECUTE_READ:PmProtect = TEXT("PAGE_EXECUTE_READ"); break;
    case PAGE_EXECUTE_READWRITE: PmProtect = TEXT("PAGE_EXECUTE_READWRITE"); break;
    case PAGE_EXECUTE_WRITECOPY: PmProtect = TEXT("PAGE_EXECUTE_WRITECOPY"); break;
    case PAGE_NOACCESS:PmProtect = TEXT("PAGE_NOACCESS"); break;
    case PAGE_READONLY: PmProtect = TEXT("PAGE_READONLY"); break;
    case PAGE_READWRITE:PmProtect = TEXT("PAGE_READWRITE"); break;
    case PAGE_WRITECOPY: PmProtect =TEXT ("PAGE_WRITECOPY"); break;
    case PAGE_TARGETS_INVALID: PmProtect =TEXT( "PAGE_TARGETS_INVALID"); break;
    default: PmProtect = TEXT("ERROR_PROTECT"); break;
    }

    PathName += PmProtect;
    PathName += TEXT(".bin");
    return CBinString(PathName);
}

CBinString CreateDirPathName(DWORD dwPid)
{
    CFileManger FsOpt;
    CProcessOpt PsOpt;
    CBinString DirPath = GLOBAL_DIR;
    FsOpt.FsRemoveDirector(DirPath.substr(0,DirPath.find_last_of(TEXT("\\"))));             //ÿ������֮ǰ��ɾ��.
    FsOpt.FsCreateDirector(DirPath);
    DirPath += TEXT("\\");

#ifdef UNICODE
    DirPath += std::to_wstring(dwPid);
    DirPath += TEXT("_");
    DirPath += PsOpt.PsGetProcessFileNameByProcessId(dwPid);

#else
    DirPath += std::to_string(dwPid);
    DirPath += TEXT("_");
    DirPath += PsOpt.PsGetProcessFileNameByProcessId(dwPid);
#endif // UNICODE
    DirPath = DirPath.substr(0, DirPath.find_last_of(TEXT(".")));
    FsOpt.FsCreateDirector(DirPath);
    DirPath += TEXT("\\");	
    return CBinString(DirPath);
}

//д��̶���ʽ��ͷ
BOOL WriteListHead(HANDLE hFile ,PMEMORY_BASIC_INFORMATION pMemInfo)
{
#define  BUFFER_SIZE 0X100
    CFileManger FsOpt;
    char szTempString[BUFFER_SIZE];
    memset(szTempString, 0, BUFFER_SIZE);
    DWORD dwWriteBytes = 0;
    string PmProtect = "";
    string PmState = "";
    string PmType = "";
    //�ڴ�ı�������
    switch (pMemInfo->Protect) //�������ͱȽ�
    {
    case PAGE_EXECUTE: PmProtect = "PAGE_EXECUTE"; break;
    case PAGE_EXECUTE_READ:PmProtect = "PAGE_EXECUTE_READ"; break;
    case PAGE_EXECUTE_READWRITE: PmProtect = "PAGE_EXECUTE_READWRITE";break;
    case PAGE_EXECUTE_WRITECOPY: PmProtect = "PAGE_EXECUTE_WRITECOPY"; break;
    case PAGE_NOACCESS:PmProtect = "PAGE_NOACCESS";break;
    case PAGE_READONLY: PmProtect = "PAGE_READONLY"; break;
    case PAGE_READWRITE:PmProtect = "PAGE_READWRITE";break;
    case PAGE_WRITECOPY: PmProtect = "PAGE_WRITECOPY"; break;
    case PAGE_TARGETS_INVALID: PmProtect = "PAGE_TARGETS_INVALID";break;
    default: PmProtect = "ERROR_PROTECT"; break;
    }
    //�ڴ��״̬
    switch (pMemInfo->State)
    {
    case MEM_COMMIT: PmState = "MEM_COMMIT"; break;
    case MEM_FREE: PmState = ""; break;
    case MEM_RELEASE:PmState = "";break;
    default: PmState = "ERROR_STATE";break;
    }

    //�ڴ�����.
    switch (pMemInfo->Type)
    {
    case MEM_IMAGE: PmType = "MEM_IMAGE"; break;
    case MEM_MAPPED: PmType = "MEM_MAPPED"; break;
    case MEM_PRIVATE: PmType = "MEM_PRIVATE";break;
    default: PmType = "ERROR_MmType"; break;
    }
    sprintf_s(szTempString, BUFFER_SIZE, "type = %s staus = %s protect = %s \r\n", PmType.c_str(), PmState.c_str(), PmProtect.c_str());

    return FsOpt.FsWriteFile(hFile, szTempString, sizeof(szTempString) / sizeof(szTempString[0]), &dwWriteBytes, NULL);
    
}

//����Handle��ȡ�ļ�����.
HANDLE g_GetFileNmaeEvent = CreateEvent(NULL, FALSE, TRUE, NULL);
HANDLE g_WriteMemoryHandleInforEvent = CreateEvent(NULL, FALSE, TRUE, NULL);

DWORD  WINAPI GetFileName(LPVOID lParam )
{

    //�¼�ͬ��.
    WaitForSingleObject(g_GetFileNmaeEvent, INFINITE);




 
    CFileManger fsOpt;
    PPATH_HANDLE PtahHandleInfor = (PPATH_HANDLE)lParam;
    HANDLE hFile = INVALID_HANDLE_VALUE;
    ULONG uRetLength = 0;
    OBJECT_INFORMATION_CLASS ob = ObjectFileInformation;
    char * Buffer = new char[0x100];
    memset(Buffer, 0, 0x100);
    g_NtQueryObject(PtahHandleInfor->hTarHandle, ob, Buffer, 0x100, &uRetLength);
    POBJECT_NAME_INFORMATION pFileInfo = reinterpret_cast<POBJECT_NAME_INFORMATION>(Buffer);
    /* TCHAR szBuffer[0x256] = { 0 };
     memcpy(szBuffer, pFileInfo->Name.Buffer, pFileInfo->Name.MaximumLength);*/
  

 
  
    CBinString TempDirPath = PtahHandleInfor->DirPath;
    TempDirPath  += TEXT("GlobalHandleName.txt");
  
    DWORD dwWriteBytes = 0;
   // DWORD dwSize = ::lstrlen(pFileInfo);

    if (pFileInfo->Name.Length == 0 || pFileInfo->Name.MaximumLength == 0)
    {
        delete[] Buffer;
        SetEvent(g_GetFileNmaeEvent);
        return NULL;
    }
    hFile = fsOpt.FsCreateFile(TempDirPath.c_str(), GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_ALWAYS, NULL, NULL);
    SetFilePointer(hFile, 0, 0, FILE_END);
    WriteFile(hFile, pFileInfo->Name.Buffer, pFileInfo->Name.Length, &dwWriteBytes, NULL);
    char szLine[] = "\n";
    WriteFile(hFile, szLine, sizeof(szLine), &dwWriteBytes, nullptr);
    ////str.substr(2,str.find_first_of(TEXT("\\")));
   
    //CloseHandle(hFile);
    delete[] Buffer;
    SetEvent(g_GetFileNmaeEvent);
    return NULL;
}



 BOOL  WriteMemoryHandleInfor(HANDLE  hProcess,CBinString DirPath)
{

     CProcessOpt PsOpt;
    DWORD handlecount;
    ZwQueryInformationProcess(hProcess, ProcessHandleCount, &handlecount, sizeof(handlecount), NULL); //hc == �������
    HANDLE hTarHandle;
  

    PATH_HANDLE pathHandleInfo = { 0 };
  

    for (DWORD64 i = 0; i < handlecount * 8; i += 4)
    {

        if (1 == DuplicateHandle(hProcess, (HANDLE)i, GetCurrentProcess(), &hTarHandle, 0, 0, DUPLICATE_SAME_ACCESS))
        {
            //ʹ��NtQueryObject������������Ϣ

            //���������͵�ֵ.��ȡ��Ӧ������

                //���߳�,��ȡ.������.

            ULONG uRetLength = 0;
            char * Buffer = new char[0X400]();
            OBJECT_INFORMATION_CLASS ob = ObjectTypeInformation;
            g_NtQueryObject(hTarHandle, ob, Buffer, 0x100, &uRetLength);
            PPUBLIC_OBJECT_TYPE_INFORMATION pBuffer = reinterpret_cast<PPUBLIC_OBJECT_TYPE_INFORMATION>(Buffer);  //��ѯ������Ϣ
            //printf("�������Ϊ: %ws       �������Ϊ: %d  ", pBuffer->TypeName.Buffer, pBuffer->MaintainTypeList);

            if (pBuffer->MaintainTypeList == g_dwType)
            {
                
                pathHandleInfo.hTarHandle = hTarHandle;
                pathHandleInfo.DirPath = DirPath;
                HANDLE hThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)GetFileName, &pathHandleInfo, 0, 0);
                //GetFileName(pBuffer->MaintainTypeList,(HANDLE)hTarHandle);

                WaitForSingleObject(hThread, 100);
            }

        }

    }
  
    return 0;
}

int _tmain(_In_ int argc, _In_reads_(argc) _Pre_z_ _TCHAR** argv, _In_z_ _TCHAR** envp)
{
    CProcessOpt PsOpt;
    CFileManger FsOpt;
    PsOpt.SeEnbalAdjustPrivileges(SE_DEBUG_NAME);
    HANDLE hProcess = NULL;
    SIZE_T dwErrorCode = 0;
    PSYSTEM_PROCESSES psp = NULL;
    CBinString DirPath = TEXT("");

    HANDLE hFileMapping = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, 0x1000, TEXT("MyType"));
    g_dwType = PsOpt.HndGetHandleTypeWithHandle(hFileMapping);
    CloseHandle(hFileMapping);

    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    DWORD dwErrCode = 0;
    BOOL bRet = FALSE;

    if (INVALID_HANDLE_VALUE == hSnapshot)
    {
        dwErrCode = GetLastError();
        CloseHandle(hSnapshot);
        return NULL;
    }

    PROCESSENTRY32 pi = { 0 };
    pi.dwSize = sizeof(PROCESSENTRY32); //��һ��ʹ�ñ����ʼ����Ա
    bRet = Process32First(hSnapshot, &pi);
    while (bRet)
    {
       
        hProcess = PsOpt.PsGetProcess(pi.th32ProcessID); // ����PROCESS ��ȡ.
        if (0 == hProcess)
        {
            bRet = Process32Next(hSnapshot, &pi);
            continue;
        }
      /*  if (pi.th32ProcessID != 2708)
        {
            bRet = Process32Next(hSnapshot, &pi);
            continue;   //���Ե�PID
        }*/
        DirPath = CreateDirPathName(pi.th32ProcessID);
       // HANDLE hThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)WriteMemoryHandleInfor, hProcess, 0, 0);
        WriteMemoryHandleInfor(hProcess,DirPath);
       // WaitForSingleObject(hThread, 300);
        //WriteMemoryHandleInfor(hProcess);
        //�����˽����µ������ڴ�.д�뵽�ļ�
       MmGetSectionMemoryAnWriteFile(hProcess, pi.th32ProcessID, DirPath);
        bRet = Process32Next(hSnapshot, &pi);
    }
    CloseHandle(hSnapshot);
  
    system("pause");
    return 0;
}

int MmGetSectionMemoryAnWriteFile( const HANDLE &hProcess,DWORD dwPid,CBinString DirPath)
{
    
    DWORD dwErrorCode;
    CProcessOpt PsOpt;
    CFileManger FsOpt;
    CBinString PathName = TEXT("");
    bool retflag = true;
   
    FsOpt.FsCreateDirector(DirPath);
    PMEMORY_BASIC_INFORMATION pMemInfo = new MEMORY_BASIC_INFORMATION();
    dwErrorCode = VirtualQueryEx(hProcess, 0, pMemInfo, sizeof(MEMORY_BASIC_INFORMATION));
    if (0 == dwErrorCode)
        return 0;


    for (__int64 i = pMemInfo->RegionSize; i < (i + pMemInfo->RegionSize); i += pMemInfo->RegionSize)
    {

        dwErrorCode = VirtualQueryEx(hProcess, (LPVOID)i, pMemInfo, sizeof(MEMORY_BASIC_INFORMATION));
        if (0 == dwErrorCode)
            break;

        if (pMemInfo->State != MEM_COMMIT)
            continue;

        if (pMemInfo->Protect == PAGE_READWRITE || pMemInfo->Protect == PAGE_EXECUTE_READWRITE || pMemInfo->Protect == PAGE_READONLY)
        {

            if (pMemInfo->Type == MEM_MAPPED)
            {
                SIZE_T dwReadBytes = 0;
                char *szBuffer = new char[pMemInfo->State]();
                ReadProcessMemory(hProcess, pMemInfo->BaseAddress, szBuffer, pMemInfo->State, &dwReadBytes);

                PathName = SplicingPathFileName(DirPath, PsOpt, dwPid, pMemInfo);


                HANDLE hFile = FsOpt.FsCreateFile(PathName.c_str(), GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, 0, CREATE_ALWAYS, NULL, 0);
                if (INVALID_HANDLE_VALUE == hFile)
                {
                    delete[] szBuffer;
                    continue;
                }
                FsOpt.FsSetFilePoint(hFile, 0, 0, FILE_END);

                /*

                д���ڴ�type
                д���ڴ�state.
                д���ڴ�protect
                д���ڴ�OldProtect
                д���ͷ
                */
                WriteListHead(hFile, pMemInfo);

                DWORD dwWriteBytes;
                SetFilePointer(hFile, 0, 0, FILE_END);
                WriteFile(hFile, szBuffer, pMemInfo->State, &dwWriteBytes, NULL);
                CloseHandle(hFile);
                delete[] szBuffer;

            }
        }
        continue;

    }
    retflag = false;
    return {};
}

