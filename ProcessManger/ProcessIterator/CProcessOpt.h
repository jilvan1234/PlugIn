#pragma once
#include <windows.h>
#include <vector>
#include <string>
#include "../../publicstruct.h"
#include <algorithm>  //tranfrom 字符串转大小写需要用到.

using namespace std;


class CProcessOpt
{
public:
    CProcessOpt();
    ~CProcessOpt();

    //进程相关
public:
    //根据PID挂起进程
    BOOL    PsSusPendProcess(DWORD dwPid); 
    //根据PID恢复进程
    BOOL    PsResumeProcess(DWORD dwPid);
    //两种方式关闭.hprocess 或者 pid方式.
    //获取进程的Process
    HANDLE  PsGetProcess(DWORD pid);
    //根据进程Pid返回进程名字
    CBinString PsGetProcessFileNameByProcessId(DWORD dwPid);
    DWORD   PsGetProcessIdByProcessFileName(CBinString ProcessImageFileName);
    //杀进程相关
public:
    BOOL    PsTerminateProcess(DWORD pid);
    //内存清零结束方式
    BOOL    PsTerminateProcessTheZeroMemory(DWORD dwPid);
    //远程ExitProcess结束
    BOOL    PsTerminateProcessTheRemoteExitProcess(DWORD dwPid);
    //远程线程结束 
    BOOL    PsTerminateProcessTheRemoteExitThread(DWORD dwPid);
    //修改线程的ThreadContext让其出现异常.
    BOOL    PsTerminateProcessTheThreadContext(DWORD dwPid);
    //权限修改结束
    BOOL    PsTerminateProcessTheModifyPermission(DWORD dwPid);

public:
    //获取进程数据
    typedef NTSTATUS(WINAPI *NTQUERYSYSTEMINFORMATION)(IN SYSTEM_INFORMATION_CLASS, IN OUT PVOID, IN ULONG, OUT PULONG OPTIONAL);
    NTQUERYSYSTEMINFORMATION m_NtQuerySytemInforMation;
    PSYSTEM_PROCESSES NtGetProcessInfo(); //获取进程信息
public:
    //Model 获取exe的主基址
    BOOL MdGetProcessOepModel(DWORD dwPid, BYTE **hModel, UBinSize &BaseSize);

    //获取指定模块的基址
    BOOL MdGetProcessSpecifiedModel(DWORD dwPid,BYTE **hModel,UBinSize &BaseSize,CBinString SpecifiedModelName);

    //线程相关
public:
    BOOL TsShowAllIteratorThread(DWORD dwPid);


    //权限相关  设置一个应用的 DACL ACL 删除ACL
public:
    BOOL  SeEnbalAdjustPrivileges(LPCTSTR Privileges); 

public:
    //句柄相关
    //DuplicateHandle(GetCurrentProcess(), handle, hProcess, &hProcessTag, 0, FALSE, DUPLICATE_SAME_ACCESS);
    //句柄拷贝
    BOOL HndDuplicateHandle(
            _In_ HANDLE hSourceProcessHandle,
            _In_ HANDLE hSourceHandle,
            _In_ HANDLE hTargetProcessHandle,
            _Outptr_ LPHANDLE lpTargetHandle,
            _In_ DWORD dwDesiredAccess = 0,
            _In_ BOOL bInheritHandle = FALSE,
            _In_ DWORD dwOptions = DUPLICATE_SAME_ACCESS
        );

    //获取句柄的全局信息

    PfnZwQuerySystemInformation ZwQuerySystemInformation;
    PVOID HndRetSystemHandleInformation();
   
    //获取句柄类型.
    /*
    根据名字获取
    1.File
    2.Event
    3.reg... 
    */
    USHORT HndGetHandleTypeWithTypeName(CBinString TypeName);
    USHORT HndGetHandleTypeWithHandle(HANDLE handle); //根据Handle来获取句柄的类型

    PfnZwQueryObject m_NtQueryObject;               //函数NtQueryObject
public:
    PVOID   MmGetAddress(CBinString ModuleName, string FunctionName);
private:
    //初始化函数指针
    void    InitFunctionTablePoint();
    void    ReleaseResource();//释放资源
    //获取call指针
   

    typedef DWORD(WINAPI* PfnNtSuspendAnResumeProcess)(HANDLE ProcessHandle);
    typedef DWORD(WINAPI* NtTerminateProcess)(HANDLE, DWORD);
    PfnNtSuspendAnResumeProcess   m_PfnNtSuspendProcess;
    PfnNtSuspendAnResumeProcess   m_PfnNtResumeProcess;
    NtTerminateProcess m_PfnNtTerminateProcess;
};

