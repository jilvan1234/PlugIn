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

    //突破SESSION隔离的 远程线程注入LoadLibrary
    virtual BOOL RtsRemoteThreadLoadLibrary(HANDLE hProcess,DWORD dwPid,LPCTSTR lpLoadLibraryName);
    //远程卸载模块名字.
    virtual BOOL RtsRemoteUnloadModule(DWORD dwPid,LPCTSTR lpRemoteModelName);
    //远程线程注入ShellCode
    virtual BOOL RtsRemoteThreadShellCode(HANDLE hProcess, DWORD dwPid, LPVOID InjectShellCode, UBinSize WriteSize);

    //EIP注入DLL
    BOOL RtsRemoteContextDll(HANDLE hProcess, DWORD dwPid, DWORD dwTid, LPCTSTR lpLoadLibraryName);

    //EIP 注入ShellCode
    /*
    1.hProcess 如果为NULL 则dwPid不能为NULL
    2.dwPid    如果为NULL 则hProcess不能为NULL
    3.给定的线程TID 不能为NULL
    4.注入的ShellCode 不能为NULL
    5.注入ShellCode的大小 不能为NULL
    */
    virtual BOOL RtsRemoteContextShellCode(HANDLE hProcess, DWORD dwPid, DWORD dwTid, LPVOID InjectShellCode, UBinSize WriteSize);


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

