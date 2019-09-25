#include "CRemoteThread.h"
#include <minwindef.h>


CRemoteThread::CRemoteThread()
{
    m_ZwCreateThreadEx = NULL;
    InitTableFunction();
}


CRemoteThread::~CRemoteThread()
{

}

BOOL CRemoteThread::RtsRemoteThreadLoadLibrary(HANDLE hProcess,DWORD dwPid,LPCTSTR lpLoadLibraryName)
{
    HANDLE hTempProcess = NULL;
    LPVOID lpBuffer = NULL;
    SIZE_T dwWriteBytes = 0;
    DWORD dwRetErrorCode = 0;
    HANDLE hThreadHandle;
    PVOID pFuncProcAddr = NULL;
#ifdef UNICODE
    HMODULE hNtdllDll = ::LoadLibrary(TEXT("ntdll.dll"));
    if (NULL == hNtdllDll)
    {
        OutputDebugString(TEXT("inject model load error \r\n"));
        return FALSE;
}
    // ��ȡLoadLibraryA������ַ
    pFuncProcAddr = ::GetProcAddress(::GetModuleHandle(TEXT("Kernel32.dll")), "LoadLibraryW");
    if (NULL == pFuncProcAddr)
    {
        OutputDebugString(TEXT("inject loadLibraryW Error \r\n"));
        return FALSE;
    }
#else
    HMODULE hNtdllDll = ::LoadLibrary(TEXT("ntdll.dll"));
    if (NULL == hNtdllDll)
    {
        OutputDebugString(TEXT("inject model load error \r\n"));
        return FALSE;
    }
    // ��ȡLoadLibraryA������ַ
    pFuncProcAddr = ::GetProcAddress(::GetModuleHandle(TEXT("Kernel32.dll")), "LoadLibraryA");
    if (NULL == pFuncProcAddr)
    {
        OutputDebugString(TEXT("inject loadLibraryW Error \r\n"));
        return FALSE;
    }
#endif // UNICODE

    if (hProcess != NULL)
    {
       ;
        hTempProcess = hProcess;

        if (NULL == hTempProcess)
        {
            
            return FALSE;
        }
        lpBuffer = VirtualAllocEx(hTempProcess, 0, 0x1000, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
        if (NULL == lpBuffer)
        {
            
            return FALSE;
        }

        dwRetErrorCode = WriteProcessMemory(hTempProcess, lpBuffer, lpLoadLibraryName, 1024 * sizeof(TCHAR), &dwWriteBytes);
        if (0 == dwRetErrorCode)
        {
            if (NULL != lpBuffer)
                VirtualFreeEx(hTempProcess, lpBuffer, 0, MEM_RELEASE);
        
            return FALSE;
        }

        if (NULL == m_ZwCreateThreadEx)
        {
            if (NULL != lpBuffer)
                VirtualFreeEx(hTempProcess, lpBuffer, 0, MEM_RELEASE);
            return FALSE;
        }

        m_ZwCreateThreadEx(&hThreadHandle, PROCESS_ALL_ACCESS, NULL, hTempProcess, (LPTHREAD_START_ROUTINE)pFuncProcAddr, lpBuffer, 0, 0, 0, 0, NULL);
        //hThreadHandle = CreateRemoteThreadEx(hProcess, NULL, 0, (LPTHREAD_START_ROUTINE)LoadLibrary, lpBuffer, 0, 0, 0);
        WaitForSingleObject(hTempProcess, 2000);

        if (NULL == hThreadHandle)

        {
            if (NULL != lpBuffer)
                VirtualFreeEx(hTempProcess, lpBuffer, 0, MEM_RELEASE);
            return FALSE;
        }
        if (NULL != lpBuffer)
            VirtualFreeEx(hTempProcess, lpBuffer, 0, MEM_RELEASE);
        return TRUE;

    }
    else
    {
        if (dwPid == 0 || dwPid == 4)
            return FALSE;
        hTempProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwPid);

        if (NULL == hTempProcess)
        {
            OutputDebugString(TEXT("�򿪽���ʧ��\r\n"));
            return FALSE;
        }
        lpBuffer = VirtualAllocEx(hTempProcess, 0, 0x1000, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
        if (NULL == lpBuffer)
        {
            OutputDebugString(TEXT("�����ڴ�ʧ��\r\n"));
            return FALSE;
        }

        dwRetErrorCode = WriteProcessMemory(hTempProcess, lpBuffer, lpLoadLibraryName, 1024 * sizeof(TCHAR), &dwWriteBytes);
        if (0 == dwRetErrorCode)
        {
            if (NULL != lpBuffer)
                VirtualFreeEx(hTempProcess, lpBuffer, 0, MEM_RELEASE);
            if (NULL != hTempProcess)
                CloseHandle(hTempProcess);
            return FALSE;
        }

         if (NULL == m_ZwCreateThreadEx)
         {
             if (NULL != lpBuffer)
                 VirtualFreeEx(hTempProcess, lpBuffer, 0, MEM_RELEASE);
             if (NULL != hTempProcess)
                 CloseHandle(hTempProcess);
             return FALSE;
         }

        m_ZwCreateThreadEx(&hThreadHandle, PROCESS_ALL_ACCESS, NULL, hTempProcess, (LPTHREAD_START_ROUTINE)pFuncProcAddr, lpBuffer, 0, 0, 0, 0, NULL);
        //hThreadHandle = CreateRemoteThreadEx(hProcess, NULL, 0, (LPTHREAD_START_ROUTINE)LoadLibrary, lpBuffer, 0, 0, 0);
        WaitForSingleObject(hTempProcess, 2000);

        if (NULL == hThreadHandle)

        {
            if (NULL != lpBuffer)
                VirtualFreeEx(hTempProcess, lpBuffer, 0, MEM_RELEASE);
            if (NULL != hTempProcess)
                CloseHandle(hTempProcess);
            return FALSE;
        }
        if (NULL != lpBuffer)
            VirtualFreeEx(hTempProcess, lpBuffer, 0, MEM_RELEASE);
        if (NULL != hTempProcess)
            CloseHandle(hTempProcess);
        return TRUE;


    }
    if (NULL != lpBuffer)
        VirtualFreeEx(hProcess, lpBuffer, 0x1000, MEM_RELEASE);
    if (NULL != hProcess)
        CloseHandle(hProcess);
    return TRUE;
}

BOOL CRemoteThread::RtsRemoteUnloadModule(DWORD dwPid, LPCTSTR lpRemoteModelName)
{
    /*
    1.����ж�ص�ģ������,Ϊ����Զ���߳�������ռ�.
    2.Զ���̵߳���GetModelHandle
    3.����GetExitCodeThread ��ȡԶ���̵߳���GetModuleHandle���ص�ģ����
    4.�ͷ������Զ���߳�ģ��ռ�
    5.Զ���̵߳��� FreeLibraryAndExitThread �����ͷ�ģ����
    */


 
    HANDLE hProcess = 0;
    SIZE_T  sizWriteBytes = 0;
    LPVOID lpRemoteDllNameBuffer = NULL;
    DWORD dwRetErrorCode = FALSE;
    HANDLE hThread;
    HMODULE hDll = NULL;
    PfnFreeLibraryAndExitThread pFreeLibraryAndExitThread = NULL;
    DWORD dwRetDllModel;
    CBinString UnLoadRemoteModelName = TEXT("");
    CProcessOpt PsOpt;
    BYTE *RemoteModelAddr = NULL;
    UBinSize dwBaseSize = 0;

   
    PfnGetModuleHandle PGetModuleHandle = NULL;

    hDll = LoadLibrary(TEXT("KERNEL32.DLL"));
    if (NULL == hDll)
        return FALSE;

#ifdef UNICODE
    PGetModuleHandle = reinterpret_cast<PfnGetModuleHandle>(GetProcAddress(hDll, "GetModuleHandleW"));
#else
    PGetModuleHandle = reinterpret_cast<PfnGetModuleHandle>(GetProcAddress(hDll, "GetModuleHandleA"));
#endif
    pFreeLibraryAndExitThread = reinterpret_cast<PfnFreeLibraryAndExitThread>(GetProcAddress(hDll,"FreeLibraryAndExitThread"));
    if (NULL == PGetModuleHandle)
        return FALSE;
   
   
    if (NULL == lpRemoteModelName)
        return FALSE;

    if (NULL == pFreeLibraryAndExitThread)
        return FALSE;

    UnLoadRemoteModelName = lpRemoteModelName;
    if (UnLoadRemoteModelName.empty())
        return FALSE;

   
   

    if (dwPid != 0 && dwPid != 4)
    {

        hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwPid);
        if (NULL == hProcess)
            return FALSE;
        lpRemoteDllNameBuffer = VirtualAllocEx(hProcess, 0, 0x1000, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
        if (NULL == lpRemoteDllNameBuffer)
            return FALSE;

        if (NULL != hProcess)
        {
            dwRetErrorCode = WriteProcessMemory(hProcess, lpRemoteDllNameBuffer, lpRemoteModelName, MAX_PATH * sizeof(TCHAR), &sizWriteBytes);
            if (0 == dwRetErrorCode)
            {
                if (NULL != lpRemoteDllNameBuffer)
                    VirtualFreeEx(hProcess, lpRemoteDllNameBuffer, 0, MEM_RELEASE);
                if (NULL != hProcess)
                    CloseHandle(hProcess);
                return FALSE;
            }



            //// m_ZwCreateThreadEx(&hThread, PROCESS_ALL_ACCESS, NULL, hProcess, (LPTHREAD_START_ROUTINE)PGetModuleHandle, lpRemoteDllNameBuffer, 0, 0, 0, 0, NULL);
            // //DWORD dwThreadId = 0;
            // //hThread = CreateRemoteThreadEx(hProcess, NULL, 0, (LPTHREAD_START_ROUTINE)PGetModuleHandle, lpRemoteDllNameBuffer, 0, 0, &dwThreadId);

#ifdef _WIN64

            //PsOpt.MdGetProcessSpecifiedModel(dwPid, &RemoteModelAddr, dwBaseSize, UnLoadRemoteModelName);
            m_ZwCreateThreadEx(&hThread, PROCESS_ALL_ACCESS, NULL, hProcess, (LPTHREAD_START_ROUTINE)PGetModuleHandle, lpRemoteDllNameBuffer, 0, 0, 0, 0, NULL);
            WaitForSingleObject(hThread, INFINITE);

            GetExitCodeThread(hThread, &dwRetDllModel);

            if (NULL == RemoteModelAddr)
            {
                if (NULL != lpRemoteDllNameBuffer)
                    VirtualFreeEx(hProcess, lpRemoteDllNameBuffer, 0, MEM_RELEASE);
                if (NULL != hProcess)
                    CloseHandle(hProcess);
                return FALSE;
            }

            m_ZwCreateThreadEx(&hThread, PROCESS_ALL_ACCESS, NULL, hProcess, (LPTHREAD_START_ROUTINE)pFreeLibraryAndExitThread, (LPVOID)RemoteModelAddr, 0, 0, 0, 0, 0);
            WaitForSingleObject(hThread, INFINITE);

            if (NULL != lpRemoteDllNameBuffer)
                VirtualFreeEx(hProcess, lpRemoteDllNameBuffer, 0, MEM_RELEASE);
            if (NULL != hProcess)
                CloseHandle(hProcess);

            return TRUE;

            
           

#else

            m_ZwCreateThreadEx(&hThread, PROCESS_ALL_ACCESS, NULL, hProcess, (LPTHREAD_START_ROUTINE)PGetModuleHandle, lpRemoteDllNameBuffer, 0, 0, 0, 0, NULL);
            WaitForSingleObject(hThread, INFINITE);
            GetExitCodeThread(hThread, &dwRetDllModel);

            if (NULL == dwRetDllModel)
            {
                if (NULL != lpRemoteDllNameBuffer)
                    VirtualFreeEx(hProcess, lpRemoteDllNameBuffer, 0, MEM_RELEASE);
                if (NULL != hProcess)
                    CloseHandle(hProcess);
                return FALSE;
            }

            m_ZwCreateThreadEx(&hThread, PROCESS_ALL_ACCESS, NULL, hProcess, (LPTHREAD_START_ROUTINE)pFreeLibraryAndExitThread, (LPVOID)dwRetDllModel, 0, 0, 0, 0, 0);
            WaitForSingleObject(hThread, INFINITE);

            if (NULL != lpRemoteDllNameBuffer)
                VirtualFreeEx(hProcess, lpRemoteDllNameBuffer, 0, MEM_RELEASE);
            if (NULL != hProcess)
                CloseHandle(hProcess);
            return TRUE;
           

#endif // _WIN32


            //���ģ���ַ,����Զ���߳�.


            //hThread = CreateRemoteThreadEx(hProcess, NULL, 0, (LPTHREAD_START_ROUTINE)pFreeLibraryAndExitThread, (LPVOID)dwRetDllModel, 0, 0, &dwThreadId);



            return TRUE;
        }
    }
    /*if (dwPid != 0 && dwPid != 4)
    {

        hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwPid);
        if (NULL == hProcess)
            return FALSE;
        
        lpRemoteDllNameBuffer = VirtualAllocEx(hProcess, 0, 0x1000, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
        if (NULL == lpRemoteDllNameBuffer)
            return FALSE;

        dwRetErrorCode = WriteProcessMemory(hProcess, lpRemoteDllNameBuffer, lpRemoteModelName, MAX_PATH * sizeof(TCHAR), &sizWriteBytes);
        if (0 == dwRetErrorCode)
        {
            if (NULL != lpRemoteDllNameBuffer)
                VirtualFreeEx(hProcess, lpRemoteDllNameBuffer, 0, MEM_RELEASE);
            if (NULL != hProcess)
                CloseHandle(hProcess);
            return FALSE;
        }

#ifdef __WIN32
        PsOpt.PsSusPendProcess(dwPid);
        m_ZwCreateThreadEx(&hThread, PROCESS_ALL_ACCESS, NULL, hProcess, (LPTHREAD_START_ROUTINE)PGetModuleHandle, lpRemoteDllNameBuffer, 0, 0, 0, 0, NULL);
        WaitForSingleObject(hThread, INFINITE);
        GetExitCodeThread(hThread, &dwRetDllModel);

        if (NULL == dwRetDllModel)
        {
            if (NULL != lpRemoteDllNameBuffer)
                VirtualFreeEx(hProcess, lpRemoteDllNameBuffer, 0, MEM_RELEASE);
            if (NULL != hProcess)
                CloseHandle(hProcess);
            return FALSE;
        }

        m_ZwCreateThreadEx(&hThread, PROCESS_ALL_ACCESS, NULL, hProcess, (LPTHREAD_START_ROUTINE)pFreeLibraryAndExitThread, (LPVOID)dwRetDllModel, 0, 0, 0, 0, 0);
        WaitForSingleObject(hThread, INFINITE);

        if (NULL != lpRemoteDllNameBuffer)
            VirtualFreeEx(hProcess, lpRemoteDllNameBuffer, 0, MEM_RELEASE);
        if (NULL != hProcess)
            CloseHandle(hProcess);
        PsOpt.PsResumeProcess(dwPid);
        return TRUE;
#else
        
        PsOpt.MdGetProcessSpecifiedModel(dwPid, &RemoteModelAddr, dwBaseSize, UnLoadRemoteModelName);
        if (NULL == RemoteModelAddr)
        {
            if (NULL != lpRemoteDllNameBuffer)
                VirtualFreeEx(hProcess, lpRemoteDllNameBuffer, 0, MEM_RELEASE);
            if (NULL != hProcess)
                CloseHandle(hProcess);
            return FALSE;
        }

        m_ZwCreateThreadEx(&hThread, PROCESS_ALL_ACCESS, NULL, hProcess, (LPTHREAD_START_ROUTINE)pFreeLibraryAndExitThread, (LPVOID)RemoteModelAddr, 0, 0, 0, 0, 0);
        WaitForSingleObject(hThread, INFINITE);

        if (NULL != lpRemoteDllNameBuffer)
            VirtualFreeEx(hProcess, lpRemoteDllNameBuffer, 0, MEM_RELEASE);
        if (NULL != hProcess)
            CloseHandle(hProcess);
       
        return TRUE;

#endif

    }
    return FALSE;*/
    return FALSE;
}

//Զ���߳�ע��ShellCode
BOOL CRemoteThread::RtsRemoteThreadShellCode(HANDLE hProcess, DWORD dwPid, LPVOID InjectShellCode, UBinSize WriteSize)
{
    LPVOID lpRemoteDllNameBuffer = NULL;
    DWORD dwRetErrorCode = NULL;
    SIZE_T sizeWriteBytes = 0;
    HANDLE hThread = INVALID_HANDLE_VALUE;
    if (NULL == m_ZwCreateThreadEx)
        return FALSE;
    if (hProcess != NULL)
    {
        lpRemoteDllNameBuffer = VirtualAllocEx(hProcess, 0, 0x1000, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
        if (NULL == lpRemoteDllNameBuffer)
            return FALSE;

        dwRetErrorCode = WriteProcessMemory(hProcess, lpRemoteDllNameBuffer, InjectShellCode, WriteSize, &sizeWriteBytes);
        if (0 == dwRetErrorCode)
        {
            if (NULL != lpRemoteDllNameBuffer)
                VirtualFreeEx(hProcess, lpRemoteDllNameBuffer, 0, MEM_RELEASE);
            if (NULL != hProcess)
                CloseHandle(hProcess);
            return FALSE;
        }
        m_ZwCreateThreadEx(&hThread, PROCESS_ALL_ACCESS, NULL, hProcess, (LPTHREAD_START_ROUTINE)lpRemoteDllNameBuffer, 0, 0, 0, NULL, NULL, NULL);
        WaitForSingleObject(hThread, INFINITE);
        CloseHandle(hThread);
        if (NULL != lpRemoteDllNameBuffer)
            VirtualFreeEx(hProcess, lpRemoteDllNameBuffer, 0, MEM_RELEASE);
        if (NULL != hProcess)
            CloseHandle(hProcess);
        return TRUE;
    }
    else if (dwPid != 0 && dwPid != 4)
    {
        hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwPid);
        if (NULL == hProcess)
            return FALSE;
        lpRemoteDllNameBuffer = VirtualAllocEx(hProcess, 0, 0x1000, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
        if (NULL == lpRemoteDllNameBuffer)
            return FALSE;

        if (NULL != hProcess)
        {
            dwRetErrorCode = WriteProcessMemory(hProcess, lpRemoteDllNameBuffer, InjectShellCode, WriteSize, &sizeWriteBytes);
            if (0 == dwRetErrorCode)
            {
                if (NULL != lpRemoteDllNameBuffer)
                    VirtualFreeEx(hProcess, lpRemoteDllNameBuffer, 0, MEM_RELEASE);
                if (NULL != hProcess)
                    CloseHandle(hProcess);
                return FALSE;
            }
            m_ZwCreateThreadEx(&hThread, PROCESS_ALL_ACCESS, NULL, hProcess, (LPTHREAD_START_ROUTINE)lpRemoteDllNameBuffer, 0, 0, 0, NULL, NULL, NULL);
            WaitForSingleObject(hThread, INFINITE);
            CloseHandle(hThread);
            if (NULL != lpRemoteDllNameBuffer)
                VirtualFreeEx(hProcess, lpRemoteDllNameBuffer, 0, MEM_RELEASE);
            if (NULL != hProcess)
                CloseHandle(hProcess);
            return TRUE;
        }
        return FALSE;
    }

    return FALSE;
}

//Զ��EIPע��DLL
BOOL CRemoteThread::RtsRemoteContextDll(HANDLE hProcess, DWORD dwPid, DWORD dwTid,LPCTSTR lpLoadLibraryName)
{

    CShellCode Shell;
    
    /*
    ����ShellCode����ע��.
    
    
    */
    DWORD64 KernelBase = Shell.GetKernelBase();

    CBinString RemotePathString = lpLoadLibraryName;
    
    if (hProcess != 0 && lpLoadLibraryName != NULL && !(RemotePathString.empty()))
    {

    }

    return FALSE;
}

//Զ���߳�EIPע��ShellCode
BOOL CRemoteThread::RtsRemoteContextShellCode(HANDLE hProcess, DWORD dwPid, DWORD dwTid, LPVOID InjectShellCode, UBinSize WriteSize)
{

    /*
ԭ��:
    1.�����߳�
    2.���߳�.��ȡ�߳̾��
    3.�����߳̾��.����GetThreadContext ��ȡ EIP �ȼĴ�����Ϣ.
    4.����Զ���ڴ�. д��ShellCode
    5.�޸�Զ���ڴ��׵�ַΪContext.Eip/rip ��ִ�е�ַ.
    6.ʹ��SetThreadContext ����EIP
    7.ע�����.
    */
    HANDLE hTemProcess = 0;
    LPVOID lpRemoteBuffer = NULL;
    HANDLE hRemoteThread = NULL;


    CONTEXT CRemoteContext = { NULL };

   
    DWORD dwErrorCode = 0;
    SIZE_T sizeWriteBytes;
    if (hProcess != 0 && dwTid != 0)
    {
        hTemProcess = hProcess;
        if (0 == hTemProcess)
            return FALSE;

        hRemoteThread = OpenThread(THREAD_ALL_ACCESS, FALSE, dwTid);
        if (NULL == hRemoteThread)
        {
            //����Ȩ�޷���.
            hRemoteThread = OpenThread(THREAD_SUSPEND_RESUME | THREAD_SET_CONTEXT | THREAD_GET_CONTEXT, FALSE, dwTid);
            if (NULL == hRemoteThread)
            {
              
                return FALSE;
            }
        }
#ifdef _WIN64
        dwErrorCode = Wow64SuspendThread(hRemoteThread);
#else
        dwErrorCode = SuspendThread(hRemoteThread);
#endif
        
        if (-1 == dwErrorCode)
        {

            if (0 != hRemoteThread)
                CloseHandle(hRemoteThread);
            return FALSE; //����ʧ��,������ǰ����ļ���.
        }

        CRemoteContext.ContextFlags = CONTEXT_FULL;
#ifdef _WIN64

        //dwErrorCode = Wow64GetThreadContext(hRemoteThread, &CRemoteContext);
        dwErrorCode = GetThreadContext(hRemoteThread, &CRemoteContext);
#else
        dwErrorCode = GetThreadContext(hRemoteThread, &CRemoteContext);
#endif
        if (dwErrorCode == 0)
        {
            if (0 != hRemoteThread)
                CloseHandle(hRemoteThread);
            return FALSE;
        }

        //����Զ�̺�����ַ
        lpRemoteBuffer = VirtualAllocEx(hTemProcess, 0, 0x1000, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
        if (NULL == lpRemoteBuffer)
        {
            if (0 != hRemoteThread)
                CloseHandle(hRemoteThread);
            return FALSE;
        }

        //д��ShellCode

        if (WriteSize <= 0 || WriteSize > 0x1000)
        {
            if (NULL != lpRemoteBuffer)
                VirtualFreeEx(hTemProcess, lpRemoteBuffer, 0, MEM_RELEASE);
            if (0 != hRemoteThread)
                CloseHandle(hRemoteThread);
            return FALSE;
        }
        dwErrorCode = WriteProcessMemory(hTemProcess, lpRemoteBuffer, InjectShellCode, WriteSize, &sizeWriteBytes);


        if (0 == dwErrorCode)
        {
            printf("w error = %d \r\n", GetLastError());
            if (NULL != lpRemoteBuffer)
                VirtualFreeEx(hTemProcess, lpRemoteBuffer, 0, MEM_RELEASE);
            if (NULL != hRemoteThread)
                CloseHandle(hRemoteThread);
            return FALSE;
        }
        
        //����Context EIP��ֵΪԶ��ShellCode��ʼ��ַ
#ifdef _WIN64

        CRemoteContext.Rip = (DWORD64)lpRemoteBuffer;
       // dwErrorCode = Wow64SetThreadContext(hRemoteThread, &CRemoteContext);
        SetThreadContext(hRemoteThread, &CRemoteContext);
        ResumeThread(hRemoteThread);
        if (0 != hRemoteThread)
            CloseHandle(hRemoteThread);
        return TRUE;
#else
        //32λ��

        CRemoteContext.Eip = (DWORD)lpRemoteBuffer;
        dwErrorCode = SetThreadContext(hRemoteThread, &CRemoteContext);
        dwErrorCode = ResumeThread(hRemoteThread);
        if (dwErrorCode == -1)
        {
            if (NULL != lpRemoteBuffer)
                VirtualFreeEx(hTemProcess, lpRemoteBuffer, 0, MEM_RELEASE);
            if (0 != hRemoteThread)
                CloseHandle(hRemoteThread);
            return FALSE;
        }
        while (dwErrorCode != 0)
        {

            dwErrorCode = ResumeThread(hRemoteThread);
            if (dwErrorCode <= 0)
            {
                break;
            }
        }
        if (0 != hRemoteThread)
            CloseHandle(hRemoteThread);
        /*  if (isReleaseShellCodeMemory == TRUE)
              VirtualFreeEx(hTemProcess, lpRemoteBuffer, 0, MEM_RELEASE);*/
        return TRUE;
#endif // _WIN64


    }
    else if (dwPid != 0 && dwPid != 4 && dwTid != 0)
    {
        hTemProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwPid);
        if (0 == hTemProcess)
            return FALSE;

        hRemoteThread = OpenThread(THREAD_ALL_ACCESS,FALSE,dwTid);
        if (NULL == hRemoteThread)
        {
            //����Ȩ�޷���.
            hRemoteThread = OpenThread(THREAD_SUSPEND_RESUME | THREAD_SET_CONTEXT | THREAD_GET_CONTEXT, FALSE, dwTid);
            if (NULL == hRemoteThread)
            {
                if (0 != hTemProcess)
                    CloseHandle(hTemProcess);
                return FALSE;
            }
        }
         
#ifdef _WIN64
        dwErrorCode = Wow64SuspendThread(hRemoteThread);
#else
        dwErrorCode = SuspendThread(hRemoteThread);
#endif
        if (-1 == dwErrorCode)
        {
            
            if (0 != hRemoteThread)
                CloseHandle(hRemoteThread);
            if (0 != hTemProcess)
                CloseHandle(hTemProcess);
            return FALSE; //����ʧ��,������ǰ����ļ���.
        }

        CRemoteContext.ContextFlags = CONTEXT_FULL;
#ifdef _WIN64
        
        //dwErrorCode = Wow64GetThreadContext(hRemoteThread, &CRemoteContext);
        dwErrorCode = GetThreadContext(hRemoteThread, &CRemoteContext);
#else
        dwErrorCode = GetThreadContext(hRemoteThread, &CRemoteContext);
#endif
        
        if (dwErrorCode == 0)
        {
            if (0 != hRemoteThread)
                CloseHandle(hRemoteThread);
            if (0 != hTemProcess)
                CloseHandle(hTemProcess);
            return FALSE;
        }

        //����Զ�̺�����ַ
        lpRemoteBuffer = VirtualAllocEx(hTemProcess, 0, 0x1000, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
        if (NULL == lpRemoteBuffer)
        {
            if (0 != hRemoteThread)
                CloseHandle(hRemoteThread);
            if (0 != hTemProcess)
                CloseHandle(hTemProcess);
            return FALSE;
        }

        //д��ShellCode
      
        if (WriteSize <= 0 || WriteSize > 0x1000)
        {
            if (NULL != lpRemoteBuffer)
                VirtualFreeEx(hTemProcess, lpRemoteBuffer, 0, MEM_RELEASE);
            if (0 != hRemoteThread)
                CloseHandle(hRemoteThread);
            if (0 != hTemProcess)
                CloseHandle(hTemProcess);
            return FALSE;
        }
        dwErrorCode = WriteProcessMemory(hTemProcess, lpRemoteBuffer, InjectShellCode, WriteSize, &sizeWriteBytes);
      

        if (0 == dwErrorCode)
        {
            printf("w error = %d \r\n", GetLastError());
            if (NULL != lpRemoteBuffer)
                VirtualFreeEx(hTemProcess, lpRemoteBuffer, 0, MEM_RELEASE);
            if (NULL != hRemoteThread)
                CloseHandle(hRemoteThread);
            if (NULL != hTemProcess)
                CloseHandle(hTemProcess);
            return FALSE;
        }
        printf("addr = %p  \r\n", lpRemoteBuffer);
        //����Context EIP��ֵΪԶ��ShellCode��ʼ��ַ
#ifdef _WIN64

        CRemoteContext.Rip = (DWORD64)lpRemoteBuffer;
        //dwErrorCode = Wow64SetThreadContext(hRemoteThread, &CRemoteContext);
        dwErrorCode = SetThreadContext(hRemoteThread, &CRemoteContext);
        ResumeThread(hRemoteThread);
        if (0 != hRemoteThread)
            CloseHandle(hRemoteThread);
        if (0 != hTemProcess)
            CloseHandle(hTemProcess);
       
        return TRUE;
#else
        //32λ��

        CRemoteContext.Eip = (DWORD)lpRemoteBuffer;
        dwErrorCode =  SetThreadContext(hRemoteThread, &CRemoteContext);
        dwErrorCode = ResumeThread(hRemoteThread);
        if (dwErrorCode == -1)
        {
            if (NULL != lpRemoteBuffer)
                VirtualFreeEx(hTemProcess, lpRemoteBuffer, 0, MEM_RELEASE);
            if (0 != hRemoteThread)
                CloseHandle(hRemoteThread);
            if (0 != hTemProcess)
                CloseHandle(hTemProcess);
            return FALSE;
        }
      
        if (0 != hRemoteThread)
            CloseHandle(hRemoteThread);
        if (0 != hTemProcess)
            CloseHandle(hTemProcess);
      /*  if (isReleaseShellCodeMemory == TRUE)
            VirtualFreeEx(hTemProcess, lpRemoteBuffer, 0, MEM_RELEASE);*/
        return TRUE;
#endif // _WIN64

        
    }
    return FALSE;
}


/*
�������ע��:
1.���𴴽�����.
2.���� �ɶ�дִ�пռ�
3.д��ShellCode
4.��ȡ��ǰ��������������.
5.�޸�������.
6.�ָ�����ִ��.
*/

BOOL CRemoteThread::RtsCreateSusPendProcessInject(
    CBinString InjectName, 
    TCHAR szCmd[], 
    char * ShellCode,
    DWORD dwShellCodeSize)
{
    DWORD dwOldPro = 0;
    LPVOID lpBuffer = nullptr;
    SIZE_T size = 0;

    STARTUPINFO si = { 0 };
    PROCESS_INFORMATION pi = { 0 };

    si.cb = sizeof(STARTUPINFO);

    HANDLE hProcess = 0;
    HANDLE hThread = 0;
    BOOL bRet = FALSE;

    if (!CreateProcess(
        InjectName.c_str(),
        szCmd,
        nullptr,
        nullptr,
        false,
        CREATE_SUSPENDED,    //���𴴽�����.
        nullptr,
        nullptr,
        &si,
        &pi))
    {
        return FALSE;
    }

    hProcess = pi.hProcess;

    hThread = pi.hThread;

    if (hProcess == NULL)
        return FALSE;
#ifdef _WIN64
    ULONGLONG OldEIP = 0;
#else
    ULONG     OldEIP = 0;
#endif // _WIN64


    CONTEXT context = { 0 };
    context.ContextFlags = CONTEXT_FULL;
    if (!GetThreadContext(hThread, &context))
    {
        //����ԭʼ��RIP.����HHO.
        return FALSE;
    }
#ifdef _WIN64
    OldEIP = context.Rip;
#else
    OldEIP = context.Eip;
#endif
    lpBuffer = VirtualAllocEx(hProcess, 0, 0x1000, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
    if (NULL == lpBuffer)
        return FALSE;
  


    if (!WriteProcessMemory(hProcess, lpBuffer, ShellCode, dwShellCodeSize, &size))
    {
        return FALSE;
    }

    //��ȡ�̵߳�������.
   

    //�޸�������Ϊ�µ�OEPλ��.
#ifdef _WIN64
    context.Rip = (DWORD64)lpBuffer;
#else
    context.Eip = (DWORD)lpBuffer;
#endif
    SetThreadContext(hThread, &context);
    //�ָ���������
    ResumeThread(hThread);
    return 0;
}

VOID CRemoteThread::InitTableFunction()
{

    HMODULE hModule = NULL;
    hModule = LoadLibrary(TEXT("ntdll.dll"));
    if (NULL == hModule)
    {
        m_ZwCreateThreadEx = NULL;
        return;
    }

    m_ZwCreateThreadEx = reinterpret_cast<PfnZwCreateThreadEx>(GetProcAddress(hModule, "ZwCreateThreadEx"));
    if (NULL == m_ZwCreateThreadEx)
        return;

}
