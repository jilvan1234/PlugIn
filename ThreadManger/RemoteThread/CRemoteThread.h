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

    //突破SESSION隔离的 远程线程注入LoadLibrary
    virtual BOOL RtsRemoteThreadLoadLibrary(HANDLE hProcess,DWORD dwPid,LPCTSTR lpLoadLibraryName);
    //远程卸载模块名字.
    virtual BOOL RtsRemoteUnloadModule(DWORD dwPid,LPCTSTR lpRemoteModelName);
    //EIP注入DLL
    virtual BOOL RtsRemoteContext(HANDLE hProcess, DWORD dwPid, LPCTSTR lpLoadLibraryName);

    //EIP 注入ShellCode
    virtual BOOL RtsRemoteContextShellCode(HANDLE hProcess, DWORD dwPid, LPVOID InjectShellCode, UBinSize WriteSize);


private:
    //初始化 远程线程
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

