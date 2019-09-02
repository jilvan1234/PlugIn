
#include "CProcessOpt.h"
#include <TlHelp32.h>


CProcessOpt::CProcessOpt()
{

    m_PfnNtSuspendProcess = NULL;
    m_PfnNtResumeProcess = NULL;
    m_PfnNtTerminateProcess = NULL;
   
    InitFunctionTablePoint(); //��ʼ��������

    SeEnbalAdjustPrivileges(SE_DEBUG_NAME);

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
        return 0;
    }

    PROCESSENTRY32 pi = { 0 };
    pi.dwSize = sizeof(PROCESSENTRY32); //��һ��ʹ�ñ����ʼ����Ա
    bRet = Process32First(hSnapshot, &pi);
    CBinString TempString = TEXT("");
    //


    transform(TempString.begin(), TempString.end(), TempString.begin(), ::toupper);
    transform(ProcessImageFileName.begin(), ProcessImageFileName.end(), ProcessImageFileName.begin(), ::toupper);
    while (bRet)
    {
        TempString = pi.szExeFile;
        transform(TempString.begin(), TempString.end(), TempString.begin(), ::toupper);

        if (ProcessImageFileName.compare(TempString) == 0)
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

//��ȡ������Ϣ.
PSYSTEM_PROCESS_INFORMATION CProcessOpt::NtGetProcessInfo()
{

    PfnZwQuerySystemInformation ZwQuerySystemInformation = NULL;
    ZwQuerySystemInformation =
        reinterpret_cast<PfnZwQuerySystemInformation>(CNativeApiManger::NatGetZwQuerySystemInformation());

    SYSTEM_BASIC_INFORMATION sbi = { 0 };
    DWORD dwSize = 0x1000;
    DWORD uRetSize = 10;
    ULONG * pBuffer = new ULONG[dwSize]();
    NTSTATUS status = 0;

    if (ZwQuerySystemInformation != NULL)
    {

        do
        {

            status = ZwQuerySystemInformation(SystemProcessesAndThreadsInformation, pBuffer, dwSize, &uRetSize);
            if (status == STATUS_INFO_LENGTH_MISMATCH)
            {
                delete[] pBuffer;
                pBuffer = new ULONG[dwSize *= 2]();
            }
            else
            {
                if (!NT_SUCCESS(status))
                {
                    delete[] pBuffer;
                    return NULL;
                }

              
            }

        } while (status == STATUS_INFO_LENGTH_MISMATCH);



        return (PSYSTEM_PROCESS_INFORMATION)pBuffer;
    }
    return NULL;
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
    m_NtQueryObject = static_cast<PfnZwQueryObject>(MmGetAddress(TEXT("ntdll.dll"),"NtQueryObject"));
    
   
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
//�������

BOOL CProcessOpt::HndDuplicateHandle(
    HANDLE hSourceProcessHandle,
    HANDLE hSourceHandle,
    HANDLE hTargetProcessHandle, 
    LPHANDLE lpTargetHandle,
    DWORD dwDesiredAccess, 
    BOOL bInheritHandle, 
    DWORD dwOptions)
{
    return DuplicateHandle(
        hSourceProcessHandle,
        hSourceHandle,
        hTargetProcessHandle,
        lpTargetHandle,
        dwDesiredAccess,
        bInheritHandle,
        dwOptions);
}
//��ȡ�������ϸ��Ϣ.
PVOID CProcessOpt::HndRetSystemHandleInformation()
{
    //����ϵͳ�����Ϣ.
    PULONG szBuffer = NULL;
    DWORD dwSize = 0x1000;
    DWORD dwRetSize = 0;
    NTSTATUS ntStatus;
    szBuffer = new ULONG[dwSize];
    PfnZwQuerySystemInformation ZwQuerySystemInformation = NULL;
    ZwQuerySystemInformation = 
        reinterpret_cast<PfnZwQuerySystemInformation>(CNativeApiManger::NatGetZwQuerySystemInformation());
    if (NULL == szBuffer)
        return NULL;
    if (NULL == ZwQuerySystemInformation)
        return NULL;
    //��һ����ÿ��ܲ��ɹ�.���Ի�ȡ����ֵ. ���Ҹ���DwRetSize��ֵȥ��������ռ�
    do
    {
        ntStatus = ZwQuerySystemInformation(SystemHandleInformation, szBuffer, dwSize, &dwRetSize);
        if (ntStatus == STATUS_INFO_LENGTH_MISMATCH) //����ռ䲻��.��������ռ�
        {

            delete[] szBuffer;
            szBuffer = new ULONG[dwRetSize *= 2];

        }
        else
        {
            if (!NT_SUCCESS(ntStatus))
            {
                delete[] szBuffer;
                return NULL;
            }
        }
    } while (ntStatus == STATUS_INFO_LENGTH_MISMATCH);

    //�ɹ�������Ϣ
    return szBuffer;
}

USHORT CProcessOpt::HndGetHandleTypeWithTypeName(CBinString TypeName)
{
    /*

    1.���ݲ����������ֶ���
    2.�����Լ����̾��
    3.�ó����̵�Typeid
    */
  
    CBinString CompareTypeName = TEXT("");
    transform(TypeName.begin(), TypeName.end(), TypeName.begin(), ::toupper); //ת��Ϊ��д���л�ȡ.
    CompareTypeName = TEXT("FILE");
    if (TypeName == CompareTypeName)
    {
        //�����ļ�����.����ȫ�־����.�ó���Ϣ.
        TCHAR szPath[MAX_PATH] = { 0 };
        GetModuleFileName(NULL, szPath, MAX_PATH);
        HANDLE hFile = CreateFile(szPath, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, NULL, NULL);
        if (hFile == INVALID_HANDLE_VALUE)
            return 0;
        //����ȫ�־����.�ó���Ϣ.
        PVOID pBuffer = NULL;
        DWORD dwHandCount = 0;
        PSYSTEM_HANDLE_INFORMATION pGlobalHandleInfo = NULL;
        SYSTEM_HANDLE_TABLE_ENTRY_INFO HandleInformation = { 0 };
        pBuffer = HndRetSystemHandleInformation();
        if (pBuffer == NULL)
        {
            CloseHandle(hFile);
            return 0;
        }
        pGlobalHandleInfo = reinterpret_cast<PSYSTEM_HANDLE_INFORMATION>(pBuffer);
        if (NULL == pGlobalHandleInfo)
        {
            CloseHandle(hFile);
            return NULL;
        }
        dwHandCount = pGlobalHandleInfo->NumberOfHandles;
        for (int i = 0; i < dwHandCount; i++)
        {
            HandleInformation = pGlobalHandleInfo->Handles[i];
            if (HandleInformation.HandleValue == (USHORT) hFile && GetCurrentProcessId() == HandleInformation.UniqueProcessId)
            {
                CloseHandle(hFile);
                return HandleInformation.ObjectTypeIndex; //����������.
            }
        }
        CloseHandle(hFile);
    }
    return 0;
}

USHORT CProcessOpt::HndGetHandleTypeWithHandle(HANDLE handle)
{
    if (INVALID_HANDLE_VALUE == handle || 0 == handle)
        return 0;
    if (NULL == m_NtQueryObject)
        return 0;


    char * pBuffer = new char[0x300];
    ULONG uRetSize = 0;
    OBJECT_INFORMATION_CLASS ob = ObjectTypeInformation;
    m_NtQueryObject(handle, ob, pBuffer, 0x300, &uRetSize);
    USHORT Type = 0;
    PPUBLIC_OBJECT_TYPE_INFORMATION PtypeInfo = reinterpret_cast<PPUBLIC_OBJECT_TYPE_INFORMATION>(pBuffer);  //��ѯ������Ϣ
    Type = PtypeInfo->MaintainTypeList;
    delete[] pBuffer;
    return Type;
}

// 



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
