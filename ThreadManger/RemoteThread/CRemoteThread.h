#pragma once
#include <windows.h>
#include "publicstruct.h"
#include "CProcessOpt.h"

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
    //EIPע��DLL
    virtual BOOL RtsRemoteContext(HANDLE hProcess, DWORD dwPid, LPCTSTR lpLoadLibraryName);

    //EIP ע��ShellCode
    virtual BOOL RtsRemoteContextShellCode(HANDLE hProcess, DWORD dwPid, LPVOID InjectShellCode, UBinSize WriteSize);


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

