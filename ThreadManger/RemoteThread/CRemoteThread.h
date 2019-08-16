#pragma once
#include <windows.h>
#include "../../publicstruct.h"
#include "../../ProcessManger/ProcessIterator/CProcessOpt.h"
#include "../../ThreadManger/ShellCode/ShellCode/CShellCode.h"



using namespace std;
class CRemoteThread
{
public:
    CRemoteThread();
    ~CRemoteThread();

    //ͻ��SESSION����� Զ���߳�ע��LoadLibrary
    virtual BOOL RtsRemoteThreadLoadLibrary(HANDLE hProcess,DWORD dwPid,LPCTSTR lpLoadLibraryName);
    //Զ��ж��ģ������.
    virtual BOOL RtsRemoteUnloadModule(DWORD dwPid,LPCTSTR lpRemoteModelName);
    //Զ���߳�ע��ShellCode
    virtual BOOL RtsRemoteThreadShellCode(HANDLE hProcess, DWORD dwPid, LPVOID InjectShellCode, UBinSize WriteSize);

    //EIPע��DLL
    BOOL RtsRemoteContextDll(HANDLE hProcess, DWORD dwPid, DWORD dwTid, LPCTSTR lpLoadLibraryName);

    //EIP ע��ShellCode
    /*
    1.hProcess ���ΪNULL ��dwPid����ΪNULL
    2.dwPid    ���ΪNULL ��hProcess����ΪNULL
    3.�������߳�TID ����ΪNULL
    4.ע���ShellCode ����ΪNULL
    5.ע��ShellCode�Ĵ�С ����ΪNULL
    */
    virtual BOOL RtsRemoteContextShellCode(HANDLE hProcess, DWORD dwPid, DWORD dwTid, LPVOID InjectShellCode, UBinSize WriteSize);


private:
    //��ʼ�� Զ���߳�
    VOID InitTableFunction();
#ifdef _WIN64

    typedef DWORD(WINAPI *PfnZwCreateThreadEx)(
        PHANDLE ThreadHandle,
        ACCESS_MASK DesiredAccess,
        LPVOID ObjectAttributes,
        HANDLE ProcessHandle,
        LPTHREAD_START_ROUTINE lpStartAddress,
        LPVOID lpParameter,
        ULONG CreateThreadFlags,
        SIZE_T ZeroBits,
        SIZE_T StackSize,
        SIZE_T MaximumStackSize,
        LPVOID pUnkown);
   
#else
    typedef DWORD(WINAPI *PfnZwCreateThreadEx)(
        PHANDLE ThreadHandle,
        ACCESS_MASK DesiredAccess,
        LPVOID ObjectAttributes,
        HANDLE ProcessHandle,
        LPTHREAD_START_ROUTINE lpStartAddress,
        LPVOID lpParameter,
        BOOL CreateSuspended,
        DWORD dwStackSize,
        DWORD dw1,
        DWORD dw2,
        LPVOID pUnkown);
#endif // DEBUG
    PfnZwCreateThreadEx m_ZwCreateThreadEx;

    typedef  HMODULE (WINAPI * PfnGetModuleHandle)( _In_opt_ LPCTSTR lpModuleName);
    typedef VOID (WINAPI * PfnFreeLibraryAndExitThread)(
            _In_ HMODULE hLibModule,
            _In_ DWORD dwExitCode
        );

    
};

