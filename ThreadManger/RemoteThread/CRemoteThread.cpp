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
    // 获取LoadLibraryA函数地址
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
    // 获取LoadLibraryA函数地址
    pFuncProcAddr = ::GetProcAddress(::GetModuleHandle(TEXT("Kernel32.dll")), "LoadLibraryA");
    if (NULL == pFuncProcAddr)
    {
        OutputDebugString(TEXT("inject loadLibraryW Error \r\n"));
        return FALSE;
    }
#endif // UNICODE

    if (hProcess != NULL)
    {
        lpBuffer = VirtualAllocEx(hProcess, 0, 0x1000, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
        if (NULL == lpBuffer)
            return FALSE;

        dwRetErrorCode = WriteProcessMemory(hProcess, lpBuffer, lpLoadLibraryName, 1024 * sizeof(TCHAR), &dwWriteBytes);
        if (0 == dwRetErrorCode)
        {
            if (NULL != lpBuffer)
                VirtualFreeEx(hProcess, lpBuffer, 0, MEM_RELEASE);
            if (NULL != hProcess)
                CloseHandle(hProcess);
            return FALSE;
        }

        if (NULL == m_ZwCreateThreadEx)
        {
            if (NULL != lpBuffer)
                VirtualFreeEx(hProcess, lpBuffer, 0, MEM_RELEASE);
            if (NULL != hProcess)
                CloseHandle(hProcess);
            return FALSE;
        }
        m_ZwCreateThreadEx(&hThreadHandle, PROCESS_ALL_ACCESS, NULL, hProcess, (LPTHREAD_START_ROUTINE)pFuncProcAddr, lpBuffer, 0, 0, 0, 0, NULL);

        if (NULL == hThreadHandle)

        {
            if (NULL != lpBuffer)
                VirtualFreeEx(hProcess, lpBuffer, 0, MEM_RELEASE);
            if (NULL != hProcess)
                CloseHandle(hProcess);
            return FALSE;
        }
        if (NULL != lpBuffer)
            VirtualFreeEx(hProcess, lpBuffer, 0, MEM_RELEASE);
        if (NULL != hProcess)
            CloseHandle(hProcess);
        return TRUE;

    }
    else
    {
        if (dwPid == 0 || dwPid == 4)
            return FALSE;
        hTempProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwPid);


        if (NULL == hTempProcess)
        {
            OutputDebugString(TEXT("打开进程失败\r\n"));
            return FALSE;
        }
        lpBuffer = VirtualAllocEx(hTempProcess, 0, 0x1000, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
        if (NULL == lpBuffer)
        {
            OutputDebugString(TEXT("申请内存失败\r\n"));
            return FALSE;
        }

        dwRetErrorCode = WriteProcessMemory(hTempProcess, lpBuffer, lpLoadLibraryName, 1024 * sizeof(TCHAR), &dwWriteBytes);
        if (0 == dwRetErrorCode)
        {
            if (NULL != lpBuffer)
                VirtualFreeEx(hTempProcess, lpBuffer, 0, MEM_RELEASE);
            if (NULL != hTempProcess)
                CloseHandle(hTempProcess);
            OutputDebugString(TEXT("写入内存失败\r\n"));
            return FALSE;
        }

         if (NULL == m_ZwCreateThreadEx)
         {
             if (NULL != lpBuffer)
                 VirtualFreeEx(hProcess, lpBuffer, 0, MEM_RELEASE);
             if (NULL != hProcess)
                 CloseHandle(hProcess);
             OutputDebugString(TEXT("0000000000\r\n"));
             return FALSE;
         }

        m_ZwCreateThreadEx(&hThreadHandle, PROCESS_ALL_ACCESS, NULL, hTempProcess, (LPTHREAD_START_ROUTINE)pFuncProcAddr, lpBuffer, 0, 0, 0, 0, NULL);
        //hThreadHandle = CreateRemoteThreadEx(hProcess, NULL, 0, (LPTHREAD_START_ROUTINE)LoadLibrary, lpBuffer, 0, 0, 0);

        if (NULL == hThreadHandle)

        {
            if (NULL != lpBuffer)
                VirtualFreeEx(hTempProcess, lpBuffer, 0, MEM_RELEASE);
            if (NULL != hTempProcess)
                CloseHandle(hTempProcess);
            OutputDebugString(TEXT("ininin\r\n"));
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
    1.传入卸载的模块名字,为其在远程线程中申请空间.
    2.远程线程调用GetModelHandle
    3.调用GetExitCodeThread 获取远程线程调用GetModuleHandle返回的模块句柄
    4.释放申请的远程线程模块空间
    5.远程线程调用 FreeLibraryAndExitThread 进行释放模块句柄
    */


 
    HANDLE hProcess = 0;
    HMODULE mModel;
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


            //获得模块基址,继续远程线程.


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
}

BOOL CRemoteThread::RtsRemoteContext(HANDLE hProcess, DWORD dwPid, LPCTSTR lpLoadLibraryName)
{
    return 0;
}

BOOL CRemoteThread::RtsRemoteContextShellCode(HANDLE hProcess, DWORD dwPid, LPVOID InjectShellCode, UBinSize WriteSize)
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
