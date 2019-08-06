#include "CProcessOpt.h"
#include <TlHelp32.h>


CProcessOpt::CProcessOpt()
{

    m_PfnNtSuspendProcess = NULL;
    m_PfnNtResumeProcess = NULL;
    m_PfnNtTerminateProcess = NULL;

    InitFunctionTablePoint(); //��ʼ��������

}


CProcessOpt::~CProcessOpt()
{
    ReleaseResource();
}

//�������
BOOL CProcessOpt::PsSusPendProcess(DWORD dwPid)
{
    HANDLE hProcess = NULL;
    if (dwPid == 0 || dwPid == 4)
        return FALSE;

    hProcess = OpenProcess(PROCESS_SUSPEND_RESUME, FALSE, dwPid);
    if (NULL != hProcess)
    {
        m_PfnNtSuspendProcess(hProcess);
        CloseHandle(hProcess);
        return TRUE;
    }
    CloseHandle(hProcess);
    return FALSE;
}
//�ָ��������
BOOL CProcessOpt::PsResumeProcess(DWORD dwPid)
{

    HANDLE hProcess = NULL;
    if (dwPid == 0 || dwPid == 4)
        return FALSE;

    hProcess = OpenProcess(PROCESS_SUSPEND_RESUME, FALSE, dwPid);
    if (NULL != hProcess)
    {
        m_PfnNtResumeProcess(hProcess);
        CloseHandle(hProcess);
        return TRUE;
    }
    CloseHandle(hProcess);
    return FALSE;

}

//TerminateProcess��������
BOOL CProcessOpt::PsTerminateProcess(DWORD pid)
{
    DWORD dwErrordCode = 0;
    HANDLE hTermpProcess = NULL;

         hTermpProcess = OpenProcess(PROCESS_TERMINATE, FALSE, pid);
         if (NULL == hTermpProcess)
         {
             printf("��������ʧ��\r\n");
             //CloseHandle(hTermpProcess);
             return FALSE;
         }
         {
            
            // CloseHandle(hTermpProcess);
             printf("��������\r\n");
             TerminateProcess(hTermpProcess, 0);
             dwErrordCode = GetLastError();
             if (5 == dwErrordCode)
             {
                 printf("��������Ȩ�޲���%d\r\n", GetLastError());
             }
             printf("�������� %d\r\n", GetLastError());
             CloseHandle(hTermpProcess);

         }

    return  FALSE;
}
//�ڴ����㷽ʽ��������
BOOL CProcessOpt::PsTerminateProcessTheZeroMemory(DWORD dwPid)
{
    

    BYTE *Model = NULL;
    HANDLE hProcess = NULL;
    UBinSize dwBaseSize = 0;
    DWORD dwOldPro;
    UBinSize bRet = 0;

    
    MdGetProcessOepModel(dwPid, &Model, dwBaseSize);
    if (NULL == Model)
        return FALSE;  //����ģ��ʧ��
    hProcess = PsGetProcess(dwPid);
    if (NULL == hProcess)
        return FALSE;
    //�ڴ�����.
    
    for (UBinSize i =0 ; i < dwBaseSize; i += 0x1000)
    {
        char szBuffer[0x1000] = { 0 };
        bRet = VirtualProtectEx(hProcess,(LPVOID )(Model + i), 0x1000, PAGE_READWRITE, &dwOldPro);
        if (bRet != 0)
        {
            //�޸ĳɹ�
            WriteProcessMemory(hProcess, (LPVOID)(Model + i), szBuffer, 0x1000, NULL);
        }
    }
    CloseHandle(hProcess);
    return FALSE;
}
//ExitProcess��������
BOOL CProcessOpt::PsTerminateProcessTheRemoteExitProcess(DWORD dwPid)
{
    HANDLE hProcess = NULL;
    HANDLE RetHandle = NULL;
    hProcess = PsGetProcess(dwPid);
    if (NULL != hProcess)
    {
        RetHandle = CreateRemoteThread(hProcess, NULL, 0, (LPTHREAD_START_ROUTINE)ExitProcess, 0, 0, 0);
        if (RetHandle == NULL)
        {
            //ʧ��
            
            return FALSE;
        }
        CloseHandle(hProcess);
        return TRUE;
    }
    CloseHandle(hProcess);
    return TRUE;
}
//Զ��ExitThread�˳��߳�
BOOL CProcessOpt::PsTerminateProcessTheRemoteExitThread(DWORD dwPid)
{
    HANDLE hProcess = NULL;
    HANDLE RetHandle = NULL;
    hProcess = PsGetProcess(dwPid);
    if (NULL != hProcess)
    {
        for (int i = 0; i < 50; i++)
        {
            RetHandle = CreateRemoteThread(hProcess, NULL, 0, (LPTHREAD_START_ROUTINE)ExitThread, 0, 0, 0);
            if (RetHandle == NULL)
            {
                //ʧ��
                CloseHandle(hProcess);
                return FALSE;
            }
        }
       
        CloseHandle(hProcess);
        return TRUE;
    }
    CloseHandle(hProcess);
    return TRUE;
}

//�޸�EIP Context
BOOL CProcessOpt::PsTerminateProcessTheThreadContext(DWORD dwPid)
{
    HANDLE hProcess = NULL;
    HANDLE hThread = NULL;
    LPVOID lpAddr = NULL;
    hProcess = PsGetProcess(dwPid);

    if (NULL == hProcess)
        return FALSE;

    HANDLE hThreadSnap = INVALID_HANDLE_VALUE;
    BOOL bRet = FALSE;
    hThreadSnap = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, dwPid);
    if (hThreadSnap == INVALID_HANDLE_VALUE)
        return(FALSE);


    // ��ʹ�� Thread32First ǰ��ʼ�� THREADENTRY32 �Ľṹ��С.
    THREADENTRY32 te32 = { 0 };
    te32.dwSize = sizeof(THREADENTRY32);

    lpAddr = VirtualAllocEx(hProcess, NULL, NULL, MEM_COMMIT,PAGE_EXECUTE_READWRITE);
    if (NULL == lpAddr)
        return FALSE;

    // ��ȡ��һ���߳���Ϣ, ���ʧ�����˳�.
    bRet = Thread32First(hThreadSnap, &te32);
    if (!bRet)
    {
       
       

        //CloseHandle(hThreadSnap);     // ������ʹ�ú�������ն���!
        return(FALSE);
    }

    // ���ڻ�ȡϵͳ�߳��б�, ����ʾ��ָ��������ص�ÿ���̵߳���Ϣ
    while (bRet)
    {
        //printf("Tid = %d, TAddr = %p  TTAddr = %p\r\n", te32.th32ThreadID, te32.tpBasePri,te32.tpDeltaPri);
        hThread = OpenThread(THREAD_ALL_ACCESS, FALSE, te32.th32ThreadID);
        if (NULL == hThread)
        {
            bRet = Thread32Next(hThreadSnap, &te32);
            continue;
        }
        else
        {
            CONTEXT context = { 0 };
            context.ContextFlags = CONTEXT_FULL;  //�����ʼ����־
            SuspendThread(hThread);
            BOOL bRet = GetThreadContext(hThread, &context);
            if (bRet != 0)
            {
#ifdef _WIN64

                context.Rip = (__int64)lpAddr;
                SetThreadContext(hThread, &context);
                ResumeThread(hThread);
#else
                context.Eip = (int)lpAddr;
                SetThreadContext(hThread, &context);
                ResumeThread(hThread);
#endif // _WIN64


            }
        }
      
        CloseHandle(hThread);
        bRet = Thread32Next(hThreadSnap, &te32);
    }




    //  ǧ��Ҫ����������ն���!
    CloseHandle(hThreadSnap);
    return(TRUE);

 
    return 0;
}

//��ȡ����hProcess,�ⲿ��Ҫ�ر�
HANDLE CProcessOpt::PsGetProcess(DWORD pid)
{
    if (pid == 0 || pid == 4)
        return NULL;
    return OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);
}

//ͨ������PID ��ȡ��������
CBinString CProcessOpt::PsGetProcessFileNameByProcessId(DWORD dwPid)
{

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

        if (dwPid == pi.th32ProcessID)
        {
            CBinString ImageFileName = TEXT("");
            ImageFileName = pi.szExeFile;
            CloseHandle(hSnapshot);
            return CBinString(ImageFileName);
        }
     
        bRet = Process32Next(hSnapshot, &pi);
    }
    CloseHandle(hSnapshot);
    
    return CBinString(TEXT(""));
}

//ͨ���������ֻ�ȡ���̵�PID
DWORD CProcessOpt::PsGetProcessIdByProcessFileName(CBinString ProcessImageFileName)
{

    DWORD dwPid = 0;
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
        if (ProcessImageFileName.compare(pi.szExeFile) == 0)
        {
            dwPid = pi.th32ProcessID;
            CloseHandle(hSnapshot);
            return dwPid;
        }

        bRet = Process32Next(hSnapshot, &pi);
    }
    CloseHandle(hSnapshot);

  
    return 0;
}

//ͨ���޸�Ȩ�޽�������
BOOL CProcessOpt::PsTerminateProcessTheModifyPermission(DWORD dwPid)
{

    BYTE *Model = NULL;
    HANDLE hProcess = NULL;
    UBinSize dwBaseSize = 0;
    DWORD dwOldPro;
    UBinSize bRet = 0;


    MdGetProcessOepModel(dwPid, &Model, dwBaseSize);
    if (NULL == Model)
        return FALSE;  //����ģ��ʧ��
    hProcess = PsGetProcess(dwPid);
    if (NULL == hProcess)
        return FALSE;
    //�ڴ�����.

    for (UBinSize i = 0; i < dwBaseSize; i += 0x1000)
    {
     
        bRet = VirtualProtectEx(hProcess, (LPVOID)(Model + i), 0x1000, PAGE_READWRITE, &dwOldPro);
 
    }
    CloseHandle(hProcess);
    return FALSE;
}

//��ȡEXE��ģ���ַ.
BOOL CProcessOpt::MdGetProcessOepModel(DWORD dwPid,BYTE **hModel, UBinSize &BaseSize)
{
    int dwErrorCode = -3;
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, dwPid);
    if (INVALID_HANDLE_VALUE == hSnapshot)
    {
        dwErrorCode = GetLastError();
        CloseHandle(hSnapshot);
        return dwErrorCode;
    }
    MODULEENTRY32 mi;
    mi.dwSize = sizeof(MODULEENTRY32); //��һ��ʹ�ñ����ʼ����Ա
    BOOL  bRet = Module32First(hSnapshot, &mi);
  
    CBinString strModule = TEXT(".exe");
    while (bRet)
    {
      
        CBinString TempString = TEXT("");
        TempString = mi.szModule;
        TempString = TempString.substr(TempString.find_last_of(TEXT(".")));
        if (strModule == TempString)
        {
            *hModel = mi.modBaseAddr;
            BaseSize = mi.modBaseSize;
            return TRUE;
        }
        bRet = Module32Next(hSnapshot, &mi);
    }

    CloseHandle(hSnapshot);

    return TRUE;
}

//��ȡָ��ģ��Ļ�ַ
BOOL CProcessOpt::MdGetProcessSpecifiedModel(DWORD dwPid, BYTE ** hModel, UBinSize & BaseSize, CBinString SpecifiedModelName)
{
    int dwErrorCode = -3;
    if (SpecifiedModelName.empty())
        return FALSE;

    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, dwPid);
    if (INVALID_HANDLE_VALUE == hSnapshot)
    {
        dwErrorCode = GetLastError();
        CloseHandle(hSnapshot);
        return dwErrorCode;
    }
    MODULEENTRY32 mi;
    mi.dwSize = sizeof(MODULEENTRY32); //��һ��ʹ�ñ����ʼ����Ա
    BOOL  bRet = Module32First(hSnapshot, &mi);

    
    while (bRet)
    {

        CBinString strModule = mi.szModule;
        transform(strModule.begin(), strModule.end(), strModule.begin(), ::toupper);  //��תΪ��д���бȽ�
        transform(SpecifiedModelName.begin(), SpecifiedModelName.end(), SpecifiedModelName.begin(), ::toupper);

        if (SpecifiedModelName.compare(strModule) == 0)
        {
            *hModel = mi.modBaseAddr;
            BaseSize = mi.modBaseSize;
            CloseHandle(hSnapshot);
            return TRUE;
        }
       
        bRet = Module32Next(hSnapshot, &mi);
    }

    CloseHandle(hSnapshot);

    return FALSE;
}

//��ʾ�����߳�
BOOL CProcessOpt::TsShowAllIteratorThread(DWORD dwPid)
{
    HANDLE hThreadSnap = INVALID_HANDLE_VALUE;
    BOOL bRet = FALSE;
    hThreadSnap = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, dwPid);
    if (hThreadSnap == INVALID_HANDLE_VALUE)
        return(FALSE);


    // ��ʹ�� Thread32First ǰ��ʼ�� THREADENTRY32 �Ľṹ��С.
    THREADENTRY32 te32 = { 0 };
    te32.dwSize = sizeof(THREADENTRY32);

    // ��ȡ��һ���߳���Ϣ, ���ʧ�����˳�.
    bRet = Thread32First(hThreadSnap, &te32);
    if (!bRet)
    {
 
        CloseHandle(hThreadSnap);     // ������ʹ�ú�������ն���!
        return(FALSE);
    }

    // ���ڻ�ȡϵͳ�߳��б�, ����ʾ��ָ��������ص�ÿ���̵߳���Ϣ
    while (bRet)
    {
        //printf("Tid = %d, TAddr = %p  TTAddr = %p\r\n", te32.th32ThreadID, te32.tpBasePri,te32.tpDeltaPri);

        bRet = Thread32Next(hThreadSnap, &te32);
    }

       
  

    //  ǧ��Ҫ����������ն���!
    CloseHandle(hThreadSnap);
    return(TRUE);

}

//����Ȩ��.
BOOL CProcessOpt::SeEnbalAdjustPrivileges(LPCTSTR Privileges)
{

    HANDLE hToken = NULL;
    TOKEN_PRIVILEGES tp;
    TOKEN_PRIVILEGES oldtp;
    DWORD dwSize = sizeof(TOKEN_PRIVILEGES);
    LUID luid;

    OpenProcessToken(GetCurrentProcess(), TOKEN_ALL_ACCESS, &hToken);


    if (!LookupPrivilegeValue(NULL, Privileges, &luid)) {
        CloseHandle(hToken);
        OutputDebugString(TEXT("����Ȩ��ʧ��,LookupPrivilegeValue"));
        return FALSE;
    }
    ZeroMemory(&tp, sizeof(tp));
    tp.PrivilegeCount = 1;
    tp.Privileges[0].Luid = luid;
    tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
    /* Adjust Token Privileges */
    if (!AdjustTokenPrivileges(hToken, FALSE, &tp, sizeof(TOKEN_PRIVILEGES), &oldtp, &dwSize)) {
        CloseHandle(hToken);
        OutputDebugString(TEXT("����Ȩ��ʧ�� AdjustTokenPrivileges"));
        return FALSE;
    }
    // close handles
    CloseHandle(hToken);

    return TRUE;
}

//��ʼ��NT����
void CProcessOpt::InitFunctionTablePoint()
{


    m_PfnNtTerminateProcess = 
        static_cast<NtTerminateProcess>( MmGetAddress(TEXT("ntdll.dll"), "NtTerminateProcess"));

    m_PfnNtSuspendProcess = 
        static_cast<PfnNtSuspendAnResumeProcess>(MmGetAddress(TEXT("ntdll.dll"), "NtSuspendProcess"));
    m_PfnNtResumeProcess =
        static_cast<PfnNtSuspendAnResumeProcess>(MmGetAddress(TEXT("ntdll.dll"), "NtResumeProcess"));

    return;
}
//�ͷ���Դ
void CProcessOpt::ReleaseResource()
{
    if (NULL != m_PfnNtTerminateProcess)
        m_PfnNtTerminateProcess = NULL;

    if (NULL != m_PfnNtSuspendProcess)
        m_PfnNtSuspendProcess = NULL;

    if (NULL != m_PfnNtResumeProcess)
        m_PfnNtResumeProcess = NULL;
}
//��GetProcess�Լ�Loadlibrary�ķ�װ.
PVOID CProcessOpt::MmGetAddress(CBinString ModuleName, string FunctionName)
{
    PVOID pAddress = NULL;
    HMODULE hMod = NULL;
    hMod = LoadLibrary(ModuleName.c_str());
    if (NULL == hMod)
        return NULL;
    if (FunctionName.empty())
        return NULL;
    pAddress = GetProcAddress(hMod, FunctionName.c_str());
 
    return pAddress;
}
