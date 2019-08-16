#pragma once
#include <windows.h>
#include <vector>
#include <string>
#include "../../publicstruct.h"
#include <algorithm>  //tranfrom �ַ���ת��Сд��Ҫ�õ�.

using namespace std;


class CProcessOpt
{
public:
    CProcessOpt();
    ~CProcessOpt();

    //�������
public:
    //����PID�������
    BOOL    PsSusPendProcess(DWORD dwPid); 
    //����PID�ָ�����
    BOOL    PsResumeProcess(DWORD dwPid);
    //���ַ�ʽ�ر�.hprocess ���� pid��ʽ.
    //��ȡ���̵�Process
    HANDLE  PsGetProcess(DWORD pid);
    //���ݽ���Pid���ؽ�������
    CBinString PsGetProcessFileNameByProcessId(DWORD dwPid);
    DWORD   PsGetProcessIdByProcessFileName(CBinString ProcessImageFileName);
    //ɱ�������
public:
    BOOL    PsTerminateProcess(DWORD pid);
    //�ڴ����������ʽ
    BOOL    PsTerminateProcessTheZeroMemory(DWORD dwPid);
    //Զ��ExitProcess����
    BOOL    PsTerminateProcessTheRemoteExitProcess(DWORD dwPid);
    //Զ���߳̽��� 
    BOOL    PsTerminateProcessTheRemoteExitThread(DWORD dwPid);
    //�޸��̵߳�ThreadContext��������쳣.
    BOOL    PsTerminateProcessTheThreadContext(DWORD dwPid);
    //Ȩ���޸Ľ���
    BOOL    PsTerminateProcessTheModifyPermission(DWORD dwPid);

public:
    //��ȡ��������
    typedef NTSTATUS(WINAPI *NTQUERYSYSTEMINFORMATION)(IN SYSTEM_INFORMATION_CLASS, IN OUT PVOID, IN ULONG, OUT PULONG OPTIONAL);
    NTQUERYSYSTEMINFORMATION m_NtQuerySytemInforMation;
    PSYSTEM_PROCESSES NtGetProcessInfo(); //��ȡ������Ϣ
public:
    //Model ��ȡexe������ַ
    BOOL MdGetProcessOepModel(DWORD dwPid, BYTE **hModel, UBinSize &BaseSize);

    //��ȡָ��ģ��Ļ�ַ
    BOOL MdGetProcessSpecifiedModel(DWORD dwPid,BYTE **hModel,UBinSize &BaseSize,CBinString SpecifiedModelName);

    //�߳����
public:
    BOOL TsShowAllIteratorThread(DWORD dwPid);


    //Ȩ�����  ����һ��Ӧ�õ� DACL ACL ɾ��ACL
public:
    BOOL  SeEnbalAdjustPrivileges(LPCTSTR Privileges); 

public:
    //������
    //DuplicateHandle(GetCurrentProcess(), handle, hProcess, &hProcessTag, 0, FALSE, DUPLICATE_SAME_ACCESS);
    //�������
    BOOL HndDuplicateHandle(
            _In_ HANDLE hSourceProcessHandle,
            _In_ HANDLE hSourceHandle,
            _In_ HANDLE hTargetProcessHandle,
            _Outptr_ LPHANDLE lpTargetHandle,
            _In_ DWORD dwDesiredAccess = 0,
            _In_ BOOL bInheritHandle = FALSE,
            _In_ DWORD dwOptions = DUPLICATE_SAME_ACCESS
        );

    //��ȡ�����ȫ����Ϣ

    PfnZwQuerySystemInformation ZwQuerySystemInformation;
    PVOID HndRetSystemHandleInformation();
   
    //��ȡ�������.
    /*
    �������ֻ�ȡ
    1.File
    2.Event
    3.reg... 
    */
    USHORT HndGetHandleTypeWithTypeName(CBinString TypeName);
    USHORT HndGetHandleTypeWithHandle(HANDLE handle); //����Handle����ȡ���������

    PfnZwQueryObject m_NtQueryObject;               //����NtQueryObject
public:
    PVOID   MmGetAddress(CBinString ModuleName, string FunctionName);
private:
    //��ʼ������ָ��
    void    InitFunctionTablePoint();
    void    ReleaseResource();//�ͷ���Դ
    //��ȡcallָ��
   

    typedef DWORD(WINAPI* PfnNtSuspendAnResumeProcess)(HANDLE ProcessHandle);
    typedef DWORD(WINAPI* NtTerminateProcess)(HANDLE, DWORD);
    PfnNtSuspendAnResumeProcess   m_PfnNtSuspendProcess;
    PfnNtSuspendAnResumeProcess   m_PfnNtResumeProcess;
    NtTerminateProcess m_PfnNtTerminateProcess;
};

